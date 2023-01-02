#ifndef RANDOM_HELPER_H
#define RANDOM_HELPER_H

#include "topic.h"
#include "message.h"
#include <string>
#include <random>

Topic get_random_topic(int messages_count);

std::string get_random_string(int len);

#endif