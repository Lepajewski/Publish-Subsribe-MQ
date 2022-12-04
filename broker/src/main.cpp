#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <csignal>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "read_config.h"
#include "setup_socket.h"
#include "broker_signal.h"


int broker_sock = -1;
int client_sock = -1;

void signal_handler(int signum);


int main(int argc, char **argv) {
	if (argc < 2) {
		perror("No config file passed");
		return -1;
	}

	// setup signal handler
	signal(SIGINT, signal_handler);

	// load config
	Config cfg = parse_config(argv[1]);
	printf("Config loaded from: %s\n", argv[1]);

	char saddr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &cfg.address, saddr, INET_ADDRSTRLEN);
	printf("Opening ipv4 listen socket on: %s:%u\n", saddr, (uint16_t) ((cfg.port>>8) | (cfg.port<<8)));
	printf("max clients: %u\n", cfg.max_clients);

	// open broker socket
	broker_sock = setup_socket(&cfg);
	if (broker_sock == -1) {
		perror("Socket setup failed");
		return -1;
	}

	// start listening on the socket
	if (listen(broker_sock, cfg.max_clients)) {
		perror("Broker socket listen failed");
		return -1;
	}

	// main broker loop
	while (true) {
		sockaddr_in client_addr{};
		socklen_t client_addr_size = sizeof(client_addr);
		client_sock = accept(broker_sock, (sockaddr*) &client_addr, &client_addr_size);

		if (client_sock == -1) {
			perror("Client accept failed");
			return -1;
		}

		if (cfg.verbose) {
			printf("New connection from %s:%hu\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		}

		// start thread on new client

		while (true) {
			char data[11]{};
			int len = read(client_sock, data, sizeof(data)-1);

			if (len < 1) break;

			printf("Received %2d bytes: |%s|\n", len, data);
		}
		close(client_sock);
		printf("Connection closed\n");
	}
	
	// close socket descriptors 
	close(broker_sock);
	shutdown(client_sock, SHUT_RDWR);
	close(client_sock);

	return 0;
}

void signal_handler(int signum) {
	printf("Keyboard Interrupt ...closing socket\n");

	close(broker_sock);
	shutdown(client_sock, SHUT_RDWR);
	close(client_sock);

	exit(signum);
}