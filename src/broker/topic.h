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
    int add_message(std::string message);
    void notify_subscribers(Client* client, int id, std::string content);
    std::mutex message_mutex;
    std::mutex subscribe_mutex;
public:

    Topic(std::string name);
    ~Topic();
    void subscribe_client(Client* client);
    int send_message(Client* client, std::string content);
    std::string get_name();
    std::vector<Message*> get_messages();
    std::set<Client*> get_subscribers();
    int unsubscribe_client(Client* client);
    void time_update();
};

#endif