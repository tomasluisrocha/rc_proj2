#pragma once

#include <stddef.h>
#include <sys/types.h>

// Resolves host:port, opens a socket and 
// Returns the socket file descriptor on sucess, or -1 on error
int tcp_connect(const char *host, const char *port);

// Writes len bytes from buffer to socketfd
// Returns 0 on sucess, or -1 on error
int tcp_send_all(int sockfd, const void *buf, size_t len);

// Reads a '\n' terminated line from sockfd into buffer, also null terminated
// Returns number of bytes read (> 0), 0 on EOF, -1 on error
ssize_t tcp_read_line(int sockfd, char *buf, size_t buf_size);