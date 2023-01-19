#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <time.h>

class Message {
    time_t expire_timestamp;
public:
    Message(int id, std::string content, time_t duration);
    int id;
    std::string content;

    bool time_update();
};

#endif