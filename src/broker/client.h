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
#include <mutex>

#include "broker.h"
#include "topic.h"
#include "../utils/signal_code.h"
#include "../utils/socket_io.h"
#include "../utils/config_parser.h"

#define PING_WAIT_TIME 5 //seconds

class Broker;
class Topic;

class Client {
    Broker* broker;

    int id;
    int sock_fd;
    time_t ping_timestamp;
    time_t ping_every_seconds;
    sockaddr_in client_addr;
    std::thread client_thread;
    std::mutex disconnect_mutex;

    bool* should_close;

    time_t ping_wait_timestamp;
    bool* sent_ping;

    void client_thread_body();
    int handle_sub();
    int handle_pub();
    int handle_unsub();

    void send_connack(int id);
    int read_conn();

    void send_ping();

    int read_sub(std::string &name);
    void send_suback(suback_success_code success_code, Topic* topic);

    int read_pub(std::string &topic_name, std::string &message_content);
    void send_puback(int id);

    void send_disconn();

    int read_unsub(std::string &topic_name);
    void send_unsuback(unsuback_success_code success_code);

    void disconnect();
public:
    Client(Broker* broker, int fd, sockaddr_in addr, time_t ping_every_seconds);
    ~Client();
    void send_newmes(std::string topic_name, int id, std::string message_content);
    int get_id();
    int get_sock_fd();
    sockaddr_in get_addr_info();
    bool get_should_close();
    void time_update();
};

#endif
