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

#include "../utils/config_parser.h"
#include "../utils/signal_code.h"
#include "setup_socket.h"
#include "broker_utils.h"
#include "client.h"
#include "signal_handler.h"


// global socket descriptors (for signal handling)
int broker_sock = -1;
std::vector<int> client_sockets;


static void signal_handler(int signum);
static void client_thread(std::vector<Client*> *clients, Client *c);


int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "No config file passed");
		return -1;
	}

	// setup signal handler
	signal(SIGINT, signal_handler);

	// load config
	Config cfg = parse_config(argv[1]);
	printf("Config loaded\n");

	char s_addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &cfg.address, s_addr, INET_ADDRSTRLEN);
	printf("Opening ipv4 listen socket on: %s:%u\n", s_addr, ntohs(cfg.port));
	printf("Max clients: %u\n", cfg.max_clients);

	// init clients
	std::vector<Client*> clients;

	// open broker socket
	broker_sock = setup_socket(&cfg);
	if (broker_sock == -1) {
		fprintf(stderr, "Socket setup failed");
		return -1;
	}

	// start listening on the socket
	if (listen(broker_sock, cfg.max_clients)) {
		perror("Broker socket listen failed");
		return -1;
	}

	// main broker loop
	bool should_close = false;
	while (!should_close) {
		// registering new client should be mutually exclusive
		sockaddr_in client_addr{};
		socklen_t client_addr_size = sizeof(client_addr);

		int client_sock = accept(broker_sock, (sockaddr*) &client_addr, &client_addr_size);
		if (client_sock == -1) {
			perror("Client accept failed");
			return -1;
		}

		client_sockets.push_back(client_sock);

		// register new client
		// to fix: two clients must not have same id
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
		std::thread new_client_thread(client_thread, &clients, c);

		new_client_thread.detach();
	}
	
	// close socket descriptors
	close(broker_sock);

	return 0;
}

static void signal_handler(int signum) {
	printf("\nKeyboard Interrupt ...closing socket\n");

	close(broker_sock);
	for (auto it : client_sockets) {
		shutdown(it, SHUT_RDWR);
		close(it);
	}
	exit(signum);
}

static void client_thread(std::vector<Client*> *clients, Client *c) {
	bool should_close = false;
	
	if (read_connack(c->getSockFd()) != 0) {
		fprintf(stderr, "No connection acknowlegment from client\n");
		should_close = true;
	}
	else {
		send_connack(c->getSockFd(), c->getId());
	}

	// main client loop
	char data_in[100]{};
	while (!should_close) {
		int len = read(c->getSockFd(), data_in, sizeof(data_in)-1);

		if (len < 1) break;

		printf("Received %2d bytes: %s\n", len, data_in);
	}

	close(c->getSockFd());

	printf("Client disconnected: %s:%hu, id: %d\n", 
			inet_ntoa(c->getAddrInfo().sin_addr),
			ntohs(c->getAddrInfo().sin_port), 
			c->getId());


	// here will be semaphore take
	clients->erase(std::remove(clients->begin(), clients->end(), c), clients->end());
	// here will be semaphore release
}