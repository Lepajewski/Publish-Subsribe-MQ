#include "random_helper.h"

Topic get_random_topic(int messages_count) {
    Topic topic(get_random_string(5));
    for (int i = 0; i < messages_count; i++) {
        Message message(i, get_random_string(10));
        topic.add_message(message);
    }
    return topic;
}

std::string get_random_string(int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}