#include "message.h"

Message::Message(int id, std::string content) {
    this->id = id;
    this->content = content;
}

void Message::print_self() {
    printf("%d : %s", this->id, this->content.c_str());
}