#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

#include "../utils/config_parser.h"
#include "../utils/signal_code.h"
#include "signal_handler.h"

int main(int argc, char** argv) {
    if (argc < 2) {
		fprintf(stderr, "No config file passed");
		return -1;
	}

    Config cfg = parse_config(argv[1]);
    printf("Config loaded\n");

    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd == -1) {
        perror("Socket init failed");
        return -1;
    }

    sockaddr_in sock_addr = {};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = cfg.port;
    sock_addr.sin_addr.s_addr = cfg.address.s_addr;

    if (connect(sock_fd, (sockaddr*) &sock_addr, sizeof(sock_addr))) {
        perror("Connect failed");
        return -1;
    }

    send_connack(sock_fd);
    int id;
    if (read_connack(sock_fd, id) != 0) {
        fprintf(stderr, "No connect acknowlegment from broker.\n");
        shutdown(sock_fd, SHUT_RDWR);
        close(sock_fd);
        return -1;
    }

    printf("Connected with id: %d\n", id);

    shutdown(sock_fd, SHUT_RDWR);
    close(sock_fd);

    return 0;
}