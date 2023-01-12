#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <unistd.h>
#include <string>

#include "topic.h"
#include "../utils/signal_code.h"
#include "../utils/socket_io.h"

void send_conn(int fd);

int read_connack(int fd, int &id);

void send_sub(int fd, std::string name);

void send_pub(int fd, std::string topic_name, std::string message);

void send_unsub(int fd, std::string topic_name);

void send_disconn(int fd);

int read_suback_success(int fd, suback_success_code &success);

int read_suback_messages(int fd, Topic* topic);

int read_puback(int fd, int &id);

int read_newmes(int fd, std::string &topic_name, int &id, std::string &content);

#endif