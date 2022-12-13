#include "setup_socket.h"

int setup_socket(Config *cfg) {
    sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = cfg->address.s_addr;
	addr.sin_port = cfg->port;

	int fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == -1) {
		perror("Socket init failed");
		return -1;
	}
	
	if (bind(fd, (sockaddr*) &addr, sizeof(addr))) {
		perror("Socket bind failed");
		return -1;
	}
    return fd;
}
