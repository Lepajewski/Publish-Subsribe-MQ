#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <unistd.h>
#include <string>

#include "../utils/signal_code.h"

void send_connack(int fd);

int read_connack(int fd, int& id);

void send_suback(int fd, std::string name);

void send_puback(int fd, std::string topic_name, std::string message);

void send_unsuback(int fd, std::string topic_name);

void send_disconnack(int fd);

#endif