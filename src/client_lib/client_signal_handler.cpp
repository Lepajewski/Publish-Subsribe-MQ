#include "client_lib.h"

void Client::send_conn() {
    char stx = (char)2;
    write(this->sock_fd, &stx, 1);

    char code = signal_code_to_char(CONN);
    write(this->sock_fd, &code, 1);
}

int Client::read_connack(int &id) {
    char buff;
    id = 0;
    int len = read(this->sock_fd, &buff, 1);
	if (len < 1 || buff != (char)2) {
		return -1;
	}
    int n = read(this->sock_fd, &buff, 1);
    if (n < 1 || char_to_signal_code(buff) != CONNACK ) {
        return -1;
    }
    if (read_type(this->sock_fd, &id, sizeof(id)) < 0) {
        return -1;
    }
    return 0;
}

void Client::send_ping() {
    char stx = (char)2;
    write(this->sock_fd, &stx, 1);

    char code = signal_code_to_char(PING);
    write(this->sock_fd, &code, 1);
}

void Client::send_sub(std::string name) {
    char stx = (char)2;
    write(this->sock_fd, &stx, 1);

    char code = signal_code_to_char(SUB);
    write(this->sock_fd, &code, 1);
    size_t len = name.size();
    write(this->sock_fd, &len, sizeof(len));
    write(this->sock_fd, name.c_str(), len);
}

void Client::send_pub(std::string topic_name, std::string message) {
    char stx = (char)2;
    write(this->sock_fd, &stx, 1);

    char code = signal_code_to_char(PUB);
    write(this->sock_fd, &code, 1);
    size_t len = topic_name.size();
    write(this->sock_fd, &len, sizeof(len));
    write(this->sock_fd, topic_name.c_str(), len);
    len = message.size();
    write(this->sock_fd, &len, sizeof(len));
    write(this->sock_fd, message.c_str(), len);
}

void Client::send_unsub(std::string topic_name) {
    char stx = (char)2;
    write(this->sock_fd, &stx, 1);

    char code = signal_code_to_char(UNSUB);
    write(this->sock_fd, &code, 1);
    size_t len = topic_name.size();
    write(this->sock_fd, &len, sizeof(len));
    write(this->sock_fd, topic_name.c_str(), len);
}

void Client::send_disconn() {
    char stx = (char)2;
    write(this->sock_fd, &stx, 1);

    char code = signal_code_to_char(DISCONN);
    write(this->sock_fd, &code, 1);
}

int Client::read_suback_success(suback_success_code &success) {
    if (read_type(this->sock_fd, &success, sizeof(success)) < 0) {
        return -1;
    }
    return 0;
}

int Client::read_suback_messages(Topic* topic) {
    size_t n;
    size_t len;
    std::string content;
    int id;

    if (read_type(this->sock_fd, &n, sizeof(n)) < 0) {
        return -1;
    }
    
    for (size_t i = 0; i < n; i++) {
        if (read_type(this->sock_fd, &id, sizeof(id)) < 0) {
            return -1;
        }
        if (read_type(this->sock_fd, &len, sizeof(len)) < 0) {
            return -1;
        }
        if (read_string(this->sock_fd, content, len) < 0) {
            return -1;
        }

        topic->add_message(id, content);
    }
    return 0;
}

int Client::read_puback(int &id) {
    if (read_type(this->sock_fd, &id, sizeof(id)) < 0) {
        return -1;
    }
    return 0;
}

int Client::read_newmes(std::string &topic_name, int &id, std::string &content) {
    size_t len;
    if (read_type(this->sock_fd, &len, sizeof(len)) < 0) {
        return -1;
    }
    if (read_string(this->sock_fd, topic_name, len) < 0) {
        return -1;
    }
    if (read_type(this->sock_fd, &id, sizeof(id)) < 0) {
        return -1;
    }
    if (read_type(this->sock_fd, &len, sizeof(len)) < 0) {
        return -1;
    }
    if (read_string(this->sock_fd, content, len) < 0) {
        return -1;
    }
    return 0;
}

int Client::read_unsuback(unsuback_success_code &success) {
    if (read_type(this->sock_fd, &success, sizeof(success)) < 0) {
        return -1;
    }
    return 0;
}