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
public:
    Topic(std::string name);
    void add_message(Message message);
    void subscribe_client(Client* client);
    std::string get_name();
    std::vector<Message> get_messages();
};

#endif