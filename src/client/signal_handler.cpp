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

void send_unsuback(int fd, std::string topic_name) {
    char code = signal_code_to_char(UNSUBACK);
    write(fd, &code, 1);
    size_t len = topic_name.size();
    write(fd, &len, sizeof(len));
    write(fd, topic_name.c_str(), len);
}

void send_disconnack(int fd) {
    char code = signal_code_to_char(DISCONNACK);
    write(fd, &code, 1);
}

int read_suback_success(int fd, suback_success_code &success) {
    if (read_type(fd, &success, sizeof(success)) < 0) {
        return -1;
    }
    return 0;
}

int read_suback_name(int fd, std::string &topic_name) {
    size_t len;
    if (read_type(fd, &len, sizeof(len)) < 0) {
        return -1;
    }
    if (read_string(fd, topic_name, len) < 0) {
        return -1;
    }
    return 0;
}

int read_suback_messages(int fd, Topic* topic) {
    size_t n;
    size_t len;
    std::string content;
    int id;

    if (read_type(fd, &n, sizeof(n)) < 0) {
        return -1;
    }

    for (size_t i = 0; i < n; i++) {
        if (read_type(fd, &id, sizeof(id)) < 0) {
            return -1;
        }
        if (read_type(fd, &len, sizeof(len)) < 0) {
            return -1;
        }
        if (read_string(fd, content, len) < 0) {
            return -1;
        }

        topic->add_message(id, content);
    }
    return 0;
}