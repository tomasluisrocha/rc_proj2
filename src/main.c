#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "parser.h"
#include "ftp.h"

int main(int argc, char **argv)
{
    // Require ftp://[<user>:<password>@]<host>/<url-path> as a single argument
    if (argc != 2)
    {
        fprintf(stderr, "%s: invalid arguments\n", argv[0]);
        fprintf(stderr, "Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        exit(-1);
    }

    ftp_url_t parsed;
    if (parse_ftp_url(argv[1], &parsed) != 0)
    {
        exit(-1);
    }

    if (ftp_download(&parsed) != 0)
    {
        exit(-1);
    }

    printf("File %s transfer complete\n", parsed.filename);

    return 0;
}