#include "topic.h"

Topic::Topic(std::string name) {
    this->name = name;
}

void Topic::add_message(Message message) {
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