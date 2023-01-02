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

void send_puback(int fd, std::string topic_name, std::string message) {
    char code = signal_code_to_char(PUBACK);
    write(fd, &code, 1);
    size_t len = topic_name.size();
    write(fd, &len, sizeof(len));
    write(fd, topic_name.c_str(), len);
    len = message.size();
    write(fd, &len, sizeof(len));
    write(fd, message.c_str(), len);
}