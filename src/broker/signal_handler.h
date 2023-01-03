#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <stdio.h>
#include <unistd.h>
#include <string>

#include "../utils/signal_code.h"
#include "../utils/socket_io.h"

void send_connack(int fd, int id);

int read_connack(int fd);

int read_suback(int fd, std::string &name);

#endif