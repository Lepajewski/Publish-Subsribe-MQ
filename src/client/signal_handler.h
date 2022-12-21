#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <unistd.h>
#include <string>

#include "../utils/signal_code.h"

void send_connack(int fd);

int read_connack(int fd, int& id);

void send_suback(int fd, std::string name);

#endif