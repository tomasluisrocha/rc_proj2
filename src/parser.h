#pragma once

#define MAX_CRED_LEN 256
#define MAX_HOST_LEN 256  // RFC 1034 DNS max domain name is 255 chars + '\0'
#define MAX_PATH_LEN 4096 // Standard Linux/POSIX max path length + '\0'

typedef struct ftp_url
{
    char user[MAX_CRED_LEN];
    char password[MAX_CRED_LEN];
    char host[MAX_HOST_LEN];
    char path[MAX_PATH_LEN];
    const char *filename;
} ftp_url_t;

// Parses ftp://[<user>:<password>@]<host>/<url-path> into a ftp_url struct
int parse_ftp_url(const char *raw_url, ftp_url_t *parsed);