#ifndef READ_CONFIG_H
#define READ_CONFIG_H

#include <fstream>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>


enum config_status {
    SUCCESS,
    FILE_NOT_FOUND,
    UNKNOWN_PARAM,
    ADDRESS_INVALID,
    PORT_INVALID,
    VERBOSITY_INVALID,
    MAX_CLIENTS_INVALID,
};

typedef struct Config {
    in_addr address;
    in_port_t port;
    bool verbose;
    uint32_t max_clients;
} Config;

Config parse_config(const char *path);
config_status read_config(const char *path, Config *conf);

#endif
