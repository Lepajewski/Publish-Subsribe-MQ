#ifndef READ_CONFIG_H
#define READ_CONFIG_H

#include <fstream>
#include <string.h>

enum config_status {
    SUCCESS,
    FILE_NOT_FOUND,
    WRONG_KEY,
    WRONG_VALUE
};


typedef struct Config {
    std::string address;
    std::string port;
} Config;


config_status read_config(const char *path, Config *conf);

#endif