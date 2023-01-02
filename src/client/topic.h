#ifndef TOPIC_H
#define TOPIC_H

#include "message.h"
#include <vector>
#include <string>
#include <stdio.h>

class Topic {
private:
    std::string name;
    std::vector<Message> messages;
public:
    Topic(std::string name);
    void add_message(Message message);
    std::string get_name();
    void print_messages();
};

#endif