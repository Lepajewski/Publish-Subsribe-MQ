#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <csignal>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <algorithm>
#include <thread>

#include "read_config.h"
#include "setup_socket.h"
#include "broker_utils.h"
#include "client.h"


int broker_sock = -1;
std::vector<int> client_sockets;


static void signal_handler(int signum);
static void client_thread(std::vector<Client*> clients, Client *c);


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

	// init clients
	std::vector<Client*> clients;

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
		int client_sock = accept(broker_sock, (sockaddr*) &client_addr, &client_addr_size);
		client_sockets.push_back(client_sock);

		if (client_sock == -1) {
			perror("Client accept failed");
			return -1;
		}

		// register new client
		int id = get_new_id(clients);
		Client *c = new Client(id, client_sock, 60.0, client_addr);
		clients.push_back(c);

		if (cfg.verbose) {
			print_clients(clients);
			printf("New connection from %s:%hu as %d\n", 
					inet_ntoa(client_addr.sin_addr),
					ntohs(client_addr.sin_port), 
					c->getId());
		}

		// start thread on new client
		std::thread new_client_thread(client_thread, clients, c);

		new_client_thread.detach();
	}
	
	// close socket descriptors
	close(broker_sock);
	// shutdown(client_sock, SHUT_RDWR);
	// close(client_sock);

	return 0;
}

static void signal_handler(int signum) {
	printf("Keyboard Interrupt ...closing socket\n");

	close(broker_sock);
	for (auto it : client_sockets) {
		shutdown(it, SHUT_RDWR);
		close(it);
	}
	exit(signum);
}

static void client_thread(std::vector<Client*> clients, Client *c) {
	while (true) {
		char data[100]{};
		int len = read(c->getSockFd(), data, sizeof(data)-1);

		if (len < 1) break;

		printf("Received %2d bytes: %s\n", len, data);
	}

	close(c->getSockFd());

	printf("Client disconnected: %s:%hu, id: %d\n", 
			inet_ntoa(c->getAddrInfo().sin_addr),
			ntohs(c->getAddrInfo().sin_port), 
			c->getId());


	// here will be semaphore take
	clients.erase(std::remove(clients.begin(), clients.end(), c), clients.end());
	// here will be semaphore release
}