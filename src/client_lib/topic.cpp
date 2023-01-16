#include "topic.h"

Topic::Topic(std::string name) {
    this->name = name;
}

void Topic::add_message(int id, std::string content) {
    Message message(id, content);
    messages.push_back(message);
}

std::string Topic::get_name() {
    return this->name;
}

void Topic::print_messages() {
    for (Message msg : this->messages) {
        msg.print_self();
        printf("\n");
    }
}

int Topic::get_messages_lenght() {
    return this->messages.size();
}

void Topic::queue_message(std::string content) {
    this->awaiting_messages.push(content);
}

void Topic::put_message(int id) {
    std::string content = this->awaiting_messages.front();
    this->awaiting_messages.pop();
    this->add_message(id, content);
}