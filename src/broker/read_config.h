#ifndef READ_CONFIG_H
#define READ_CONFIG_H

#include <fstream>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

enum config_status {
    NOTHING_GIVEN = 0,
    ADDRESS_GIVEN = 1,
    PORT_GIVEN = 2,
    VERBOSITY_GIVEN = 4,
    MAX_CLIENTS_GIVEN = 8,
    FILE_NOT_OPENABLE = 16
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
