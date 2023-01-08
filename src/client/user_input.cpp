#include "user_input.h"

int get_topic_index(std::map<std::string, Topic*> topics, std::string name) {
    int topic_index = -1;
    int i = 0;
    for (auto it : topics) {
        std::string topic_name_substr = it.second->get_name().substr(0, name.size());
        if (topic_name_substr == name) {
            if (topic_index != -1) {
                return -2;
            }
            else {
                topic_index = i;
            }
        }
        i++;
    }
    return topic_index;
}

void parse_input(std::string input, int& action, std::string& argument) {
    size_t idx;
    try {
        action = std::stoi(input, &idx);
    }
    catch (...) {
        action = -1;
    }
    try {
        argument = input.substr(idx + 1);
    }
    catch (...) {
        argument = "";
    }
}