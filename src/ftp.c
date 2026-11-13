#include "ftp.h"
#include "tcp.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define REPLY_BUF_SIZE 1024
#define CMD_BUF_SIZE 4200
#define DATA_CHUNK_SIZE 4096

// Formats an FTP command and sends all bytes over the control socket
static int ftp_send_cmd(int ctrl_fd, const char *fmt, ...)
{
    char cmd[CMD_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(cmd, sizeof(cmd), fmt, args);
    va_end(args);

    if (len < 0 || (size_t)len >= sizeof(cmd))
    {
        fprintf(stderr, "ftp_send_cmd: command too long\n");
        return -1;
    }

    if (strncmp(cmd, "PASS ", 5) == 0)
    {
        printf("> PASS ********\n");
    }
    else
    {
        printf("> %s", cmd);
    }

    return tcp_send_all(ctrl_fd, cmd, (size_t)len);
}

// Reads a single-line or multi-line FTP reply and returns the 3-digit status code
static int ftp_read_reply(int ctrl_fd, char *last_line, size_t buf_size)
{
    char line[REPLY_BUF_SIZE];
    int expected_code = -1;

    while (1)
    {
        ssize_t n = tcp_read_line(ctrl_fd, line, sizeof(line));
        if (n <= 0)
        {
            return -1;
        }

        printf("< %s", line);

        if (last_line != NULL && buf_size > 0)
        {
            snprintf(last_line, buf_size, "%s", line);
        }

        // Check if line starts with 3 digits
        if (isdigit((unsigned char)line[0]) &&
            isdigit((unsigned char)line[1]) &&
            isdigit((unsigned char)line[2]))
        {
            int code = (line[0] - '0') * 100 + (line[1] - '0') * 10 + (line[2] - '0');

            if (expected_code == -1)
            {
                expected_code = code;
            }

            // A space after the 3-digit code marks the final line of the reply
            if (code == expected_code && line[3] == ' ')
            {
                return code;
            }
        }
    }
}

// Parses "(h1,h2,h3,h4,p1,p2)" from a 227 PASV response
static int parse_pasv_reply(const char *reply, char *ip_out, size_t ip_size, char *port_out, size_t port_size)
{
    const char *open_paren = strchr(reply, '(');
    if (open_paren == NULL)
    {
        return -1;
    }

    unsigned int h1, h2, h3, h4, p1, p2;
    if (sscanf(open_paren + 1, "%u,%u,%u,%u,%u,%u", &h1, &h2, &h3, &h4, &p1, &p2) != 6)
    {
        return -1;
    }

    unsigned int port = p1 * 256 + p2;
    snprintf(ip_out, ip_size, "%u.%u.%u.%u", h1, h2, h3, h4);
    snprintf(port_out, port_size, "%u", port);
    return 0;
}

// Reads raw bytes from data_fd until EOF and writes them to filename in CWD
static int download_data_stream(int data_fd, const char *filename)
{
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        perror("fopen");
        return -1;
    }

    char buf[DATA_CHUNK_SIZE];
    while (1)
    {
        ssize_t n = read(data_fd, buf, sizeof(buf));
        if (n < 0)
        {
            perror("read");
            fclose(fp);
            return -1;
        }
        if (n == 0)
        {
            break; // Transfer complete (EOF)
        }

        if (fwrite(buf, 1, (size_t)n, fp) != (size_t)n)
        {
            perror("fwrite");
            fclose(fp);
            return -1;
        }
    }

    if (fclose(fp) != 0)
    {
        perror("fclose");
        return -1;
    }

    return 0;
}

int ftp_download(const ftp_url_t *url)
{
    if (url == NULL)
    {
        return -1;
    }

    int ctrl_fd = -1;
    int data_fd = -1;
    char reply[REPLY_BUF_SIZE];
    int status = -1;

    // Connect to control port 21
    ctrl_fd = tcp_connect(url->host, "21");
    if (ctrl_fd < 0)
    {
        fprintf(stderr, "FTP error (connect): could not connect to %s:21\n", url->host);
        goto cleanup;
    }

    if (ftp_read_reply(ctrl_fd, reply, sizeof(reply)) != 220)
    {
        fprintf(stderr, "FTP error (connect): unexpected greeting: %s", reply);
        goto cleanup;
    }

    // Authenticate (default to anonymous if no credentials provided)
    const char *user = (url->user[0] != '\0') ? url->user : "anonymous";
    const char *pass = (url->user[0] != '\0') ? url->password : "anonymous";

    if (ftp_send_cmd(ctrl_fd, "USER %s\r\n", user) != 0)
    {
        fprintf(stderr, "FTP error (login): failed to send USER\n");
        goto cleanup;
    }

    int code = ftp_read_reply(ctrl_fd, reply, sizeof(reply));
    if (code == 331)
    {
        if (ftp_send_cmd(ctrl_fd, "PASS %s\r\n", pass) != 0)
        {
            fprintf(stderr, "FTP error (login): failed to send PASS\n");
            goto cleanup;
        }
        code = ftp_read_reply(ctrl_fd, reply, sizeof(reply));
    }

    if (code != 230)
    {
        fprintf(stderr, "FTP error (login): authentication failed: %s", reply);
        goto cleanup;
    }

    // Set binary transfer mode (TYPE I) so binary files aren't corrupted
    if (ftp_send_cmd(ctrl_fd, "TYPE I\r\n") != 0 ||
        ftp_read_reply(ctrl_fd, reply, sizeof(reply)) != 200)
    {
        fprintf(stderr, "FTP error (type): failed to set binary mode: %s", reply);
        goto cleanup;
    }

    // Enter Passive Mode (PASV -> 227)
    char data_ip[32];
    char data_port[8];
    if (ftp_send_cmd(ctrl_fd, "PASV\r\n") != 0 ||
        ftp_read_reply(ctrl_fd, reply, sizeof(reply)) != 227 ||
        parse_pasv_reply(reply, data_ip, sizeof(data_ip), data_port, sizeof(data_port)) != 0)
    {
        fprintf(stderr, "FTP error (passive): failed to enter passive mode: %s", reply);
        goto cleanup;
    }

    // Open data connection and request file (RETR -> 150/125)
    data_fd = tcp_connect(data_ip, data_port);
    if (data_fd < 0)
    {
        fprintf(stderr, "FTP error (data connect): could not connect to %s:%s\n", data_ip, data_port);
        goto cleanup;
    }

    if (ftp_send_cmd(ctrl_fd, "RETR %s\r\n", url->path) != 0)
    {
        fprintf(stderr, "FTP error (retrieve): failed to send RETR\n");
        goto cleanup;
    }

    code = ftp_read_reply(ctrl_fd, reply, sizeof(reply));
    if (code != 150 && code != 125)
    {
        fprintf(stderr, "FTP error (retrieve): server rejected RETR: %s", reply);
        goto cleanup;
    }

    // Download file stream and verify completion (226)
    if (download_data_stream(data_fd, url->filename) != 0)
    {
        fprintf(stderr, "FTP error (transfer): failed saving %s\n", url->filename);
        goto cleanup;
    }

    close(data_fd);
    data_fd = -1;

    if (ftp_read_reply(ctrl_fd, reply, sizeof(reply)) != 226)
    {
        fprintf(stderr, "FTP error (transfer): incomplete transfer: %s", reply);
        goto cleanup;
    }

    ftp_send_cmd(ctrl_fd, "QUIT\r\n");
    ftp_read_reply(ctrl_fd, reply, sizeof(reply));
    status = 0;

cleanup:
    if (data_fd >= 0)
    {
        close(data_fd);
    }
    if (ctrl_fd >= 0)
    {
        close(ctrl_fd);
    }
    return status;
}