#include "signal_handler.h"

void send_connack(int fd, int id) {
    char code = signal_code_to_char(CONNACK);
    write(fd, &code, 1);
	write(fd, &id, sizeof(id));
}

int read_connack(int fd) {
    const int BUFF_SIZE = 100;
    char buf[BUFF_SIZE]{};
	int len = read(fd, buf, 1);
	if (len < 1 || char_to_signal_code(buf[0]) != CONNACK) {
		return -1;
	}
    return 0;
}

int read_suback(int fd, std::string &name) {
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

    size_t len = topic->get_name().size();
    write(fd, &len, sizeof(len));
    write(fd, topic->get_name().c_str(), len);

    len = topic->get_messages().size();
    write(fd, &len, sizeof(len));
    for (auto m : topic->get_messages()) {
        write(fd, &m.id, sizeof(m.id));

        len = m.content.size();
        write(fd, &len, sizeof(len));
        write(fd, m.content.c_str(), len);
    }
}