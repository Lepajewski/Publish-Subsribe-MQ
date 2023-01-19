#include "message.h"

Message::Message(int id, std::string content, time_t duration) {
    this->id = id;
    this->content = content;
    this->expire_timestamp = time(nullptr) + duration;
}

bool Message::time_update() {
    return time(nullptr) >= this->expire_timestamp;
}