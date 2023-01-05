#include "topic.h"

Topic::Topic(std::string name) {
    this->name = name;
}

void Topic::add_message(Message message) {
    this->messages.push_back(message);
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