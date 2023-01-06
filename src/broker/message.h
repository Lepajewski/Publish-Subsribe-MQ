#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

class Message {
public:
    Message(int id, std::string content);
    int id;
    std::string content;
};

#endif