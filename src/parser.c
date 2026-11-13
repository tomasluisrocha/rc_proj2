#include "parser.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int parse_ftp_url(const char *raw_url, ftp_url_t *parsed)
{
    // Check arguments for null
    if (raw_url == NULL || parsed == NULL)
    {
        fprintf(stderr, "parse_ftp_url: invalid NULL argument\n");
        return -1;
    }

    memset(parsed,0, sizeof(*parsed));

    // Check if it starts with ftp://
    if (strncmp(raw_url, "ftp://", 6) != 0)
    {
        fprintf(stderr, "parse_ftp_url: URL must start with ftp://\n");
        return -1;
    }

    // Strip ftp:// and set cursor at -> [<user>:<password>@]<host>/<url-path>
    const char *cursor = raw_url + 6;

    const char *first_slash = strchr(cursor, '/');
    if (first_slash == NULL || *(first_slash + 1) == '\0')
    {
        fprintf(stderr, "parse_ftp_url: missing file path in URL\n");
        return -1;
    }

    // Look for @ before the path
    const char *at_sign = NULL;
    for (const char *p = cursor; p < first_slash; p++)
    {
        if (*p == '@')
        {
            at_sign = p;
        }
    }

    // Parse user and password
    if (at_sign != NULL)
    {
        const char *colon = strchr(cursor, ':');
        if (colon == NULL || colon >= at_sign)
        {
            fprintf(stderr, "parse_ftp_url: expected '<user>:<password>@' format\n");
            return -1;
        }

        size_t user_len = (size_t)(colon - cursor); // Equivalent to strlen, does NOT include '\0' in it's length
        size_t pass_len = (size_t)(at_sign - (colon + 1));

        if (user_len == 0 || user_len >= sizeof(parsed->user) || pass_len >= sizeof(parsed->password))
        {
            fprintf(stderr, "parse_ftp_url: invalid username or password length\n");
            return -1;
        }

        memcpy(parsed->user, cursor, user_len);
        parsed->user[user_len] = '\0';
        memcpy(parsed->password, colon + 1, pass_len);
        parsed->password[pass_len] = '\0';

        cursor = at_sign + 1; // cursor at -> <host>/<url-path>
    }

    // Parse host
    size_t host_len = (size_t)(first_slash - cursor);
    if (host_len == 0 || host_len >= sizeof(parsed->host))
    {
        fprintf(stderr, "parse_ftp_url: invalid or missing host\n");
        return -1;
    }
    memcpy(parsed->host, cursor, host_len);
    parsed->host[host_len] = '\0';

    // Copy path
    cursor = first_slash + 1;
    size_t path_size = strlen(cursor) + 1;
    if (path_size > sizeof(parsed->path))
    {
        fprintf(stderr, "parse_ftp_url: file path exceeds maximum length\n");
        return -1;
    }
    memcpy(parsed->path, cursor, path_size);

    // Extract filename inside parsed->path (handles both "dir/file.txt" and "file.txt")
    const char *last_slash = strrchr(parsed->path, '/');
    parsed->filename = (last_slash != NULL) ? (last_slash + 1) : parsed->path;

    if (parsed->filename[0] == '\0')
    {
        fprintf(stderr, "parse_ftp_url: URL path must end with a file, not '/'\n");
        return -1;
    }

    return 0;
}