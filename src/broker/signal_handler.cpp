#include "signal_handler.h"

void send_connack(int fd, int id) {
    const int BUFF_SIZE = 100;
    char buf[BUFF_SIZE]{};
    int len = 0;
    buf[len++] = signal_code_to_char(CONNACK);
	len += sprintf(&buf[1], "%d", id);
	buf[len++] = (char)3;
	write(fd, buf, len);
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