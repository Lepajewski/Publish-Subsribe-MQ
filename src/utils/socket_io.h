#ifndef SOCKET_IO_H
#define SOCKET_IO_H

#include <stdio.h>
#include <unistd.h>
#include <string>

int read_type(int fd, void* buf, size_t size);
int read_string(int fd, std::string &str, size_t size);
int flush_sock(int fd);

#endif