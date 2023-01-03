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