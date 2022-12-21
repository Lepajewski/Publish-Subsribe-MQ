#include "user_input.h"

int get_topic_index(std::vector<Topic> topics, std::string name) {
    int topic_index = -1;
    for (size_t i = 0; i < topics.size(); i++) {
        std::string topic_name_substr = topics[i].get_name().substr(0, name.size());
        if (topic_name_substr == name) {
            if (topic_index != -1) {
                return -2;
            }
            else {
                topic_index = i;
            }
        }
    }
    return topic_index;
}

bool check_if_topic_is_in_list(std::vector<Topic> topics, std::string name) {
    for (Topic topic : topics) {
        if (topic.get_name() == name) {
            return true;
        }
    }
    return false;
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