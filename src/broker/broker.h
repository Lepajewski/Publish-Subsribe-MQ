#ifndef BROKER_H
#define BROKER_H

#include <string>
#include <set>
#include <map>
#include <utility>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mutex>
#include <algorithm>
#include <thread>

#include "../utils/config_parser.h"
#include "topic.h"
#include "client.h"

class Topic;
class Client;

class Broker {
private:
    std::set<Client*> clients;
    std::map<std::string, Topic*> topics;
    Config cfg;

    std::mutex connect_mutex;
    std::mutex subscribe_mutex;

    std::thread accept_thread;
    std::thread main_thread;

    int sock_fd;
    bool* should_close;

    void accept_client();
    void remove_client();
    void time_update();

    void accept_thread_body();
    void main_thread_body();
public:
    Broker(const char * config_path);
    ~Broker();
    
    int init();

    int get_new_id();
    std::pair<Topic*, bool> subscribe_client_to_topic(Client* client, std::string name);
    int send_message(Client* client, std::string topic_name, std::string content);
    void unsubscribe(Client* client, std::string name);

    bool get_should_close();
    void printf_verbose(const char* format, ...);
    void print_clients();
};

#endif