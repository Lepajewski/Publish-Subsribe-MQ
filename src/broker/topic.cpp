#include "topic.h"

Topic::Topic(std::string name) {
    this->name = name;
}

int Topic::add_message(std::string message) {
    int id = this->get_new_id();
    Message m(id, message);
    this->messages.push_back(m);
}

void Topic::subscribe_client(Client* client) {
    this->subscribers.insert(client);
}

std::string Topic::get_name() {
    return this->name;
}

std::vector<Message> Topic::get_messages() {
    return this->messages;
}

int Topic::get_new_id() {
    return this->messages.size()+1;
}

std::set<Client*> Topic::get_subscribers() {
    return this->subscribers;
}