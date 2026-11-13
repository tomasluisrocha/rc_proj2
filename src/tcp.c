#include "tcp.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

int tcp_connect(const char *host, const char *port)
{
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo; // will point to the results

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_INET;       // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    if ((status = getaddrinfo(host, port, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        return -1;
    }

    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (sockfd < 0)
    {
        perror("socket");
        freeaddrinfo(servinfo);
        return -1;
    }

    if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
    {
        perror("connect");
        close(sockfd);
        freeaddrinfo(servinfo);
        return -1;
    }

    freeaddrinfo(servinfo);
    return sockfd;
}

int tcp_send_all(int sockfd, const void *buf, size_t len)
{
    const char *ptr = (const char *)buf;

    while (len > 0)
    {
        ssize_t bytes = write(sockfd, ptr, len);
        if (bytes <= 0)
        {
            perror("write");
            return -1;
        }
        ptr += bytes;
        len -= (size_t)bytes;
    }

    return 0;
}

ssize_t tcp_read_line(int sockfd, char *buf, size_t buf_size)
{
    if (buf == NULL || buf_size < 2)
    {
        return -1;
    }

    size_t i = 0;
    while (i < buf_size - 1)
    {
        char ch;
        ssize_t n = read(sockfd, &ch, 1);
        if (n < 0)
        {
            perror("read");
            return -1;
        }
        if (n == 0)
        {
            break; // EOF (connection closed)
        }

        buf[i++] = ch;
        if (ch == '\n')
        {
            break; // End of line reached
        }
    }

    if (i == buf_size - 1 && buf[i - 1] != '\n')
    {
        fprintf(stderr, "tcp_read_line: line exceeds buffer size\n");
        return -1;
    }

    buf[i] = '\0';
    return (ssize_t)i;
}
