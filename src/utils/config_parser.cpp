#include "config_parser.h"

inline config_status operator|(config_status a, config_status b) {
    return static_cast<config_status>(static_cast<int>(a) | static_cast<int>(b));
}

inline config_status operator&(config_status a, config_status b) {
    return static_cast<config_status>(static_cast<int>(a) & static_cast<int>(b));
}

Config parse_config(const char *path) {
    Config cfg;
    config_status status = read_config(path, &cfg);

    if (status & FILE_NOT_OPENABLE) {
        fprintf(stderr, "Config: couldn't open file. Using default values.\n");
        printf("Address set to 127.0.0.1.\n");
        inet_aton("127.0.0.1", &cfg.address);
        printf("Port set to 1313.\n");
        cfg.port = htons(1313);
        printf("Verbosity set to false.\n");
        cfg.verbose = false;
        printf("Max clients set to 1.\n");
        cfg.max_clients = 1;
        printf("Ping after set to 300.\n");
        cfg.ping_after = 300;
        return cfg;
    }

    if (!(status & ADDRESS_GIVEN)) {
        fprintf(stderr, "Config: address invalid or not given. Address set to 127.0.0.1.\n");
        inet_aton("127.0.0.1", &cfg.address);
    }
    if (!(status & PORT_GIVEN)) {
        fprintf(stderr, "Config: port invalid or not given. Port set to 1313.\n");
        cfg.port = htons(1313);
    }
    if (!(status & VERBOSITY_GIVEN)) {
        fprintf(stderr, "Config: verbosity invalid or not given. Verbosity set to false.\n");
        cfg.verbose = false;
    }
    if (!(status & MAX_CLIENTS_GIVEN)) {
        fprintf(stderr, "Config: max clients invalid or not given. Max clients set to 1.\n");
        cfg.max_clients = 1;
    }
    if (!(status & PING_AFTER_GIVEN)) {
        fprintf(stderr, "Config: ping after invalid or not given. Ping after set to 300.\n");
        cfg.ping_after = 300;
    }
    return cfg;
}

config_status read_config(const char *path, Config *conf) {
    std::ifstream f(path);
    config_status status = NOTHING_GIVEN;

    if (f.is_open()) {
        std::string line;
        while (std::getline(f, line)) {
            if (line[0] != '#') {
                std::string key = line.substr(0, line.find(" "));
                std::string value = line.substr(line.find(" ") + 1, line.length());
                if (key.back() == '\r') {
                    key.pop_back();
                }
                if (value.back() == '\r') {
                    value.pop_back();
                }

                if (key.compare("") != 0) {
                    if (key.compare("ADDRESS") == 0) {
                        if (inet_aton(value.c_str(), &conf->address) != 0) {
                            status = status | ADDRESS_GIVEN;
                        }
                    }
                    else if (key.compare("PORT") == 0) {
                        if ((conf->port = htons((uint16_t) atoi(value.c_str()))) != 0) {
                            status = status | PORT_GIVEN;
                        }
                    }
                    else if (key.compare("VERBOSE") == 0) {
                        if (value == "true" || value == "false") {
                            conf->verbose = value == "true" ? true : false;
                            status = status | VERBOSITY_GIVEN;
                        }
                    }
                    else if (key.compare("MAX_CLIENTS") == 0) {
                        int _mc = 0;
                        _mc = atoi(value.c_str());
                        if (_mc >= 1) {                        
                            conf->max_clients = _mc;
                            status = status | MAX_CLIENTS_GIVEN;
                        }
                    }
                    else if (key.compare("PING_AFTER") == 0) {
                        time_t ping_after = 0;
                        ping_after = atoi(value.c_str());
                        if (ping_after > 0) {
                            conf->ping_after = ping_after;
                            status = status | PING_AFTER_GIVEN;
                        }
                    }
                    else {
                        printf("Unknown option: {%s}\n", key.c_str());
                    }
                }
            }
        }
        f.close();
    }
    else {
        status = status | FILE_NOT_OPENABLE;
    }
    return status;
};