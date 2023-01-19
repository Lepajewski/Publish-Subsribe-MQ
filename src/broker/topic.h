#ifndef TOPIC_H
#define TOPIC_H

#include "message.h"
#include "client.h"
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <mutex>

#define FIVE_MINUTES 5*60

class Client;

class Topic {
private:
    std::string name;
    std::vector<Message*> messages;
    std::set<Client*> subscribers;
    int get_new_id();
public:
    std::mutex message_mutex;
    std::mutex subscribe_mutex;

    Topic(std::string name);
    ~Topic();
    int add_message(std::string message);
    void subscribe_client(Client* client);
    std::string get_name();
    std::vector<Message*> get_messages();
    std::set<Client*> get_subscribers();
    int unsubscribe_client(Client* client);
    void time_update();
};

#endif