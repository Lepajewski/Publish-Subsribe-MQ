#ifndef TOPIC_H
#define TOPIC_H

#include "message.h"
#include <vector>
#include <string>
#include <queue>
#include <stdio.h>

class Topic {
private:
    std::string name;
    std::vector<Message> messages;
    std::queue<std::string> awaiting_messages;
public:
    Topic(std::string name);
    void add_message(int id, std::string content);
    std::string get_name();
    void print_messages();
    int get_messages_lenght();
    void queue_message(std::string content);
    void put_message(int id);
};

#endif