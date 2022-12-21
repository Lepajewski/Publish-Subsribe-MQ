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
    read(fd, &buff, 1);
    while (buff != (char)3) {
        id *= 10;
        id += buff - '0';
        read(fd, &buff, 1);
    }
    return 0;
}