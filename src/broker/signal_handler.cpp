#include "signal_handler.h"

void send_connack(int fd, int id) {
    char code = signal_code_to_char(CONNACK);
    write(fd, &code, 1);
	write(fd, &id, sizeof(id));
}

int read_conn(int fd) {
    const int BUFF_SIZE = 100;
    char buf[BUFF_SIZE]{};
	int len = read(fd, buf, 1);
	if (len < 1 || char_to_signal_code(buf[0]) != CONN) {
		return -1;
	}
    return 0;
}

int read_sub(int fd, std::string &name) {
    size_t name_len;
    if (read_type(fd, &name_len, sizeof(size_t)) < 0) {
        return -1;
    }
    if (read_string(fd, name, name_len) < 0) {
        return -1;
    }
    return 0;
}

void send_suback(int fd, suback_success_code success_code, Topic* topic) {
    char code = signal_code_to_char(SUBACK);
    write(fd, &code, 1);

    write(fd, &success_code, sizeof(success_code));

    if (success_code == SUBACK_FAILURE) {
        return;
    }

    size_t len = topic->get_messages().size();
    write(fd, &len, sizeof(len));
    for (auto m : topic->get_messages()) {
        write(fd, &m.id, sizeof(m.id));

        len = m.content.size();
        write(fd, &len, sizeof(len));
        write(fd, m.content.c_str(), len);
    }
}

int read_pub(int fd, std::string &topic_name, std::string &message_content) {
    size_t name_len;
    if (read_type(fd, &name_len, sizeof(size_t)) < 0) {
        return -1;
    }
    if (read_string(fd, topic_name, name_len) < 0) {
        return -1;
    }
    if (read_type(fd, &name_len, sizeof(size_t)) < 0) {
        return -1;
    }
    if (read_string(fd, message_content, name_len) < 0) {
        return -1;
    }
    return 0;
}

void send_puback(int fd, int id) {
    char code = signal_code_to_char(PUBACK);
    write(fd, &code, 1);

    write(fd, &id, sizeof(id));
}

void send_newmes(int fd, std::string topic_name, int id, std::string message_content) {
    char code = signal_code_to_char(NEWMES);
    write(fd, &code, 1);

    size_t len = topic_name.size();
    write(fd, &len, sizeof(len));
    write(fd, topic_name.c_str(), len);

    write(fd, &id, sizeof(id));

    len = message_content.size();
    write(fd, &len, sizeof(len));
    write(fd, message_content.c_str(), len);
}

void send_disconn(int fd) {
    char code = signal_code_to_char(DISCONN);
    write(fd, &code, 1);
}

int read_unsub(int fd, std::string &topic_name) {
    size_t name_len;
    if (read_type(fd, &name_len, sizeof(size_t)) < 0) {
        return -1;
    }
    if (read_string(fd, topic_name, name_len) < 0) {
        return -1;
    }
    return 0;
}

void send_unsuback(int fd, unsuback_success_code success_code) {
    char code = signal_code_to_char(UNSUBACK);
    write(fd, &code, 1);

    write(fd, &success_code, sizeof(success_code));
}