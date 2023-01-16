#include "user_input.h"

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