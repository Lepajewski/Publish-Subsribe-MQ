#include "client.h"

void Client::send_connack(int id) {
    char code = signal_code_to_char(CONNACK);
    write(this->sock_fd, &code, 1);
	write(this->sock_fd, &id, sizeof(id));
}

int Client::read_conn() {
    char buf;
	int len = read(this->sock_fd, &buf, 1);
	if (len < 1 || char_to_signal_code(buf) != CONN) {
		return -1;
	}
    return 0;
}

void Client::send_ping() {
    char code = signal_code_to_char(PING);
    write(this->sock_fd, &code, 1);
}

int Client::read_sub(std::string &name) {
    size_t name_len;
    if (read_type(this->sock_fd, &name_len, sizeof(size_t)) < 0) {
        return -1;
    }
    if (read_string(this->sock_fd, name, name_len) < 0) {
        return -1;
    }
    return 0;
}

void Client::send_suback(suback_success_code success_code, Topic* topic) {
    char code = signal_code_to_char(SUBACK);
    write(this->sock_fd, &code, 1);

    write(this->sock_fd, &success_code, sizeof(success_code));

    if (success_code == SUBACK_FAILURE) {
        return;
    }

    size_t len = topic->get_messages().size();
    write(this->sock_fd, &len, sizeof(len));
    for (Message* m : topic->get_messages()) {
        write(this->sock_fd, &m->id, sizeof(m->id));

        len = m->content.size();
        write(this->sock_fd, &len, sizeof(len));
        write(this->sock_fd, m->content.c_str(), len);
    }
}

int Client::read_pub(std::string &topic_name, std::string &message_content) {
    size_t name_len;
    if (read_type(this->sock_fd, &name_len, sizeof(size_t)) < 0) {
        return -1;
    }
    if (read_string(this->sock_fd, topic_name, name_len) < 0) {
        return -1;
    }
    if (read_type(this->sock_fd, &name_len, sizeof(size_t)) < 0) {
        return -1;
    }
    if (read_string(this->sock_fd, message_content, name_len) < 0) {
        return -1;
    }
    return 0;
}

void Client::send_puback(int id) {
    char code = signal_code_to_char(PUBACK);
    write(this->sock_fd, &code, 1);

    write(this->sock_fd, &id, sizeof(id));
}

void Client::send_newmes(std::string topic_name, int id, std::string message_content) {
    char code = signal_code_to_char(NEWMES);
    write(this->sock_fd, &code, 1);

    size_t len = topic_name.size();
    write(this->sock_fd, &len, sizeof(len));
    write(this->sock_fd, topic_name.c_str(), len);

    write(this->sock_fd, &id, sizeof(id));

    len = message_content.size();
    write(this->sock_fd, &len, sizeof(len));
    write(this->sock_fd, message_content.c_str(), len);

    this->broker->printf_verbose("Sent newmes to id: %d\n", this->id);
}

void Client::send_disconn() {
    char code = signal_code_to_char(DISCONN);
    write(this->sock_fd, &code, 1);
}

int Client::read_unsub(std::string &topic_name) {
    size_t name_len;
    if (read_type(this->sock_fd, &name_len, sizeof(size_t)) < 0) {
        return -1;
    }
    if (read_string(this->sock_fd, topic_name, name_len) < 0) {
        return -1;
    }
    return 0;
}

void Client::send_unsuback(unsuback_success_code success_code) {
    char code = signal_code_to_char(UNSUBACK);
    write(this->sock_fd, &code, 1);

    write(this->sock_fd, &success_code, sizeof(success_code));
}