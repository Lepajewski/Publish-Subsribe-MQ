#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <stdio.h>
#include <unistd.h>
#include <string>

#include "topic.h"
#include "../utils/signal_code.h"
#include "../utils/socket_io.h"

void send_connack(int fd, int id);

int read_connack(int fd);

int read_suback(int fd, std::string &name);

void send_suback(int fd, suback_success_code success_code, Topic* topic);

#endif