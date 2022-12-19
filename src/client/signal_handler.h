#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <unistd.h>

#include "../utils/signal_code.h"

void send_connack(int fd);

int read_connack(int fd, int& id);

#endif