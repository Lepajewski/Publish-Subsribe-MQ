#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <stdio.h>
#include <unistd.h>
#include <string>

#include "topic.h"
#include "../utils/signal_code.h"
#include "../utils/socket_io.h"

void send_connack(int fd, int id);

int read_conn(int fd);

int read_sub(int fd, std::string &name);

void send_suback(int fd, suback_success_code success_code, Topic* topic);

int read_pub(int fd, std::string &topic_name, std::string &message_content);

void send_puback(int fd, int id);

void send_newmes(int fd, std::string topic_name, int id, std::string message_content);

void send_disconn(int fd);

int read_unsub(int fd, std::string &topic_name);

void send_unsuback(int fd, unsuback_success_code success_code, std::string topic_name);

#endif