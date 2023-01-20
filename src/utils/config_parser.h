#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <fstream>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

enum config_status {
    NOTHING_GIVEN = 0,
    ADDRESS_GIVEN = 1,
    PORT_GIVEN = 2,
    VERBOSITY_GIVEN = 4,
    MAX_CLIENTS_GIVEN = 8,
    PING_AFTER_GIVEN = 16,
    FILE_NOT_OPENABLE = 32
};

typedef struct Config {
    in_addr address;
    in_port_t port;
    bool verbose;
    uint32_t max_clients;
    time_t ping_after;
} Config;

Config parse_config(const char *path);
config_status read_config(const char *path, Config *conf);

#endif