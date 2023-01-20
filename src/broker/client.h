#ifndef CLIENT_H
#define CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <thread>
#include <stdarg.h>
#include <time.h>

#include "broker.h"
#include "signal_handler.h"
#include "../utils/config_parser.h"

#define PING_WAIT_TIME 5 //seconds

class Broker;

class Client {
    Broker* broker;

    int id;
    int sock_fd;
    time_t ping_timestamp;
    time_t ping_every_seconds;
    sockaddr_in client_addr;
    std::thread client_thread;

    bool* should_close;

    time_t ping_wait_timestamp;
    bool* sent_ping;

    void client_thread_body();
    int handle_sub();
    int handle_pub();
    int handle_unsub();

    void disconnect();
public:
    Client(Broker* broker, int fd, sockaddr_in addr, time_t ping_every_seconds);
    ~Client();
    int get_id();
    int get_sock_fd();
    sockaddr_in get_addr_info();
    bool get_should_close();
    void time_update();
};

#endif
