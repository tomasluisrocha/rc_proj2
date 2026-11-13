#pragma once

#include "parser.h"

// Executes the transfer (connect, login, passive, retrieve) and saves file to current directory
// Returns 0 on success, or -1 on failure
int ftp_download(const ftp_url_t *url);