#include "signal_handler.h"

void send_connack(int fd) {
    char code = signal_code_to_char(CONNACK);
    write(fd, &code, 1);
}

int read_connack(int fd, int& id) {
    const int BUFF_SIZE = 100;
    char buf[BUFF_SIZE];
    id = 0;
    int n = read(fd, buf, BUFF_SIZE);
    if (n < 1 || char_to_signal_code(buf[0]) != CONNACK ) {
        return -1;
    }
    int index = 1;
    if (index >= n) {
        n = read(fd, buf, BUFF_SIZE);
        index = 0;
    }
    while (buf[index] != (char)3) {
        id *= 10;
        id += buf[index] - '0';
        index ++;
        if (index >= n) {
            n = read(fd, buf, BUFF_SIZE);
            index = 0;
        }
    }
    //We should remember unused buffer bytes, there could be new data already
    //Maybe take buffer as arg and return index of the first unread char in the buff
    return 0;
}