#include "read_config.h"


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
                        conf->address = value;
                    } else if (key.compare("PORT") == 0) {
                        conf->port = value;
                    } else {
                        printf("Unknown option: %s\n", key.c_str());
                        return WRONG_KEY;
                    }
                }
            }
        }
        f.close();
        return SUCCESS;
    }
    return FILE_NOT_FOUND;
};