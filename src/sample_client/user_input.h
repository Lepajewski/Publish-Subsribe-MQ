#ifndef USER_INPUT_H
#define USER_INPUT_H

#include <string>
#include <map>

#include "../client_lib/topic.h"

void parse_input(std::string input, int& action, std::string& argument);

#endif