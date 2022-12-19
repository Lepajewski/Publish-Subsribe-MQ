#ifndef SETUP_SOCKET_H
#define SETUP_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../utils/config_parser.h"


int setup_socket(Config *cfg);

#endif
