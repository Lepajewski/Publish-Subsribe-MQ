#ifndef USER_INPUT_H
#define USER_INPUT_H

#include <string>
#include <vector>

#include "topic.h"

int get_topic_index(std::vector<Topic> topics, std::string name);

void parse_input(std::string input, int& action, std::string& argument);

#endif