#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <stdio.h>

class Message {
public:
    int id;
    std::string content;
    Message(int id, std::string content);
    void print_self();
};

#endif