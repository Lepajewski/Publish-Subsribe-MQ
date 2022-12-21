#include "signal_handler.h"

void send_connack(int fd) {
    char code = signal_code_to_char(CONNACK);
    write(fd, &code, 1);
}

int read_connack(int fd, int& id) {
    char buff;
    id = 0;
    int n = read(fd, &buff, 1);
    if (n < 1 || char_to_signal_code(buff) != CONNACK ) {
        return -1;
    }
    read(fd, &id, sizeof(int));
    return 0;
}

void send_suback(int fd, std::string name) {
    char code = signal_code_to_char(SUBACK);
    write(fd, &code, 1);
    size_t len = name.size();
    write(fd, &len, sizeof(len));
    write(fd, name.c_str(), len);
}