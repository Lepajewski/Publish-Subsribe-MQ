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

    std::mutex subscribe_mutex;
    std::mutex message_mutex;

    int sock_fd;

    void accept_client();
    void remove_client();
public:
    Broker(const char * config_path);
    ~Broker();

    void loop();
    int init();

    int get_new_id();
    std::pair<Topic*, bool> subscribe_client_to_topic(Client* client, std::string name);
    int send_message(Client* client, std::string topic_name, std::string content);
    void unsubscribe(Client* client, std::string name);

    void printf_verbose(const char* format, ...);
    void print_clients();
};

#endif