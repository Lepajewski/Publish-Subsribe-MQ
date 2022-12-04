#include "read_config.h"

Config parse_config(const char *path) {
    Config cfg;

	switch (read_config(path, &cfg)) {
		case SUCCESS:
			return cfg;
			break;
		case FILE_NOT_FOUND:
			perror("No config file found!\n");
			break;
        case ADDRESS_INVALID:
			perror("Config: address invalid. Using 127.0.0.1\n");
			break;
        case PORT_INVALID:
			perror("Config: port invalid. Using 1313\n");
			break;
		case UNKNOWN_PARAM:
			perror("Config: unknown parameter\n");
			break;
        case VERBOSITY_INVALID:
            perror("Config: wrong verbosity. Set verbosity to false\n");
			break;
        case MAX_CLIENTS_INVALID:
            perror("Config: max clients # invalid. Max clients will be set to 1\n");
            break;
	}
    printf("Using default config values...\n");
    inet_aton("127.0.0.1", &cfg.address);
    cfg.port = htons((uint16_t) atoi("1313"));
    cfg.verbose = false;
    cfg.max_clients = 1;

    return cfg;
}

config_status read_config(const char *path, Config *conf) {
    std::ifstream f(path);

    if (f.is_open()) {
        std::string line;
        while (std::getline(f, line)) {
            if (line[0] != '#') {
                std::string key = line.substr(0, line.find(" "));
                std::string value = line.substr(line.find(" ") + 1, line.length());

                if (key.compare("") != 0) {
                    if (key.compare("ADDRESS") == 0) {
                        if (inet_aton(value.c_str(), &conf->address) == 0) {
                            return ADDRESS_INVALID;
                        }
                    } else if (key.compare("PORT") == 0) {
                        if ((conf->port = htons((uint16_t) atoi(value.c_str()))) == 0) {
                            return PORT_INVALID;
                        }
                    } else if (key.compare("VERBOSE") == 0) {
                        if (value[0] == '1' || value[0] == '0') {
                            conf->verbose = true ? value[0] == '1' : false;
                        } else {
                            return VERBOSITY_INVALID;
                        }
                    } else if (key.compare("MAX_CLIENTS") == 0) {
                        int _mc = 0;
                        _mc = atoi(value.c_str());
                        if (_mc < 1) {
                            return MAX_CLIENTS_INVALID;
                        }
                        conf->max_clients = _mc;
                    } else {
                        printf("Unknown option: %s\n", key.c_str());
                        return UNKNOWN_PARAM;
                    }
                }
            }
        }
        f.close();
        return SUCCESS;
    }
    return FILE_NOT_FOUND;
};
