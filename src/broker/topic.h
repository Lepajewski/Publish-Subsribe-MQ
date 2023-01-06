#ifndef TOPIC_H
#define TOPIC_H

#include "message.h"
#include "client.h"
#include <string>
#include <vector>
#include <set>

class Topic {
private:
    std::string name;
    std::vector<Message> messages;
    std::set<Client*> subscribers;
    int get_new_id();
public:
    Topic(std::string name);
    int add_message(std::string message);
    void subscribe_client(Client* client);
    std::string get_name();
    std::vector<Message> get_messages();
    std::set<Client*> get_subscribers();
    int unsubscribe_client(Client* client);
};

#endif