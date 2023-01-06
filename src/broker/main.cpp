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
#include <stdarg.h>
#include <map>
#include <set>

#include "../utils/config_parser.h"
#include "../utils/signal_code.h"
#include "setup_socket.h"
#include "broker_utils.h"
#include "client.h"
#include "signal_handler.h"
#include "topic.h"

Config cfg;

// global socket descriptors (for signal handling)
int broker_sock = -1;
std::set<Client*> clients;

std::map<std::string, Topic*> topics;

static void signal_handler(int signum);
static void client_thread(Client *c);

int handle_suback(Client *c);
int handle_puback(Client *c);

void printf_verbose(const char* format, ...);

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "No config file passed");
		return -1;
	}

	// setup signal handler
	signal(SIGINT, signal_handler);

	// load config
	cfg = parse_config(argv[1]);
	printf("Config loaded\n");

	char s_addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &cfg.address, s_addr, INET_ADDRSTRLEN);
	printf("Opening ipv4 listen socket on: %s:%u\n", s_addr, ntohs(cfg.port));
	printf("Max clients: %u\n", cfg.max_clients);

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

		// register new client
		// to fix: two clients must not have same id
		int id = get_new_id(clients);
		Client* c = new Client(id, client_sock, 60.0, client_addr);
		clients.insert(c);

		if (cfg.verbose) {
			print_clients(clients);
			printf("New connection from %s:%hu as %d\n", 
					inet_ntoa(client_addr.sin_addr),
					ntohs(client_addr.sin_port), 
					c->getId());
		}

		// start thread on new client
		std::thread new_client_thread(client_thread, c);

		new_client_thread.detach();
	}
	
	// close socket descriptors
	close(broker_sock);

	return 0;
}

static void signal_handler(int signum) {
	printf("\nKeyboard Interrupt ...closing socket\n");

	close(broker_sock);
	for (Client* c : clients) {
		send_disconnack(c->getSockFd());
		shutdown(c->getSockFd(), SHUT_RDWR);
		close(c->getSockFd());
		delete c;
	}

	for (auto t : topics) {
		delete t.second;
	}
	exit(signum);
}

static void client_thread(Client *c) {
	bool should_close = false;
	
	if (read_connack(c->getSockFd()) != 0) {
		fprintf(stderr, "No connection acknowlegment from client\n");
		should_close = true;
	}
	else {
		send_connack(c->getSockFd(), c->getId());
	}

	// main client loop
	char action_code;
	while (!should_close) {
		int len = read(c->getSockFd(), &action_code, 1);
		if (len < 1) {
			should_close = true;
			break;
		}

		switch (char_to_signal_code(action_code)) {
			case SUBACK:
				if (handle_suback(c) < 0) {
					fprintf(stderr, "Error with haldling suback from id: %d. Closing connection...\n", c->getId());
					should_close = true;
				}
				break;
			
			case PUBACK:
				handle_puback(c);
				break;

			case UNSUBACK:
				printf_verbose("Received UNSUBACK from %d\n", c->getId());
				break;

			case DISCONNACK:
				printf_verbose("----------------\n");
				printf_verbose("Received DISCONNACK from %d\n", c->getId());
				printf_verbose("----------------\n");
				should_close = true;
				break;

			default:
				printf_verbose("----------------\n");
				printf_verbose("Received not supported action from %d\n", c->getId());
				printf_verbose("----------------\n");
				break;
		}
	}

	close(c->getSockFd());

	printf("Client disconnected: %s:%hu, id: %d\n", 
			inet_ntoa(c->getAddrInfo().sin_addr),
			ntohs(c->getAddrInfo().sin_port), 
			c->getId());


	// here will be semaphore take
	clients.erase(c);
	// here will be semaphore release

	delete c;
}

int handle_suback(Client* c) {
	printf("----------------\n");

	printf_verbose("Received SUBACK from id: %d\n", c->getId());
	std::string topic_name;
	if (read_suback(c->getSockFd(), topic_name) < 0) {
		send_suback(c->getSockFd(), SUBACK_FAILURE, NULL);
		return -1;
	}
	printf_verbose("Suback (id: %d) topic name: %s\n", c->getId(), topic_name.c_str());

	bool created = false;
	if ( (created = (topics.count(topic_name) < 1)) ) {
		Topic* t = new Topic(topic_name);
		topics.insert({topic_name, t});
		printf_verbose("Topic '%s' created\n", topic_name.c_str());
	}

	topics.at(topic_name)->subscribe_client(c);
	printf("Id: %d subscribed to '%s'\n", c->getId(), topic_name.c_str());

	send_suback(c->getSockFd(), (created ? SUBACK_CREATE : SUBACK_SUBSCRIBE), topics.at(topic_name));

	printf("----------------\n");
	return 0;
}

int handle_puback(Client *c) {
	printf("----------------\n");

	printf_verbose("Received PUBACK from %d\n", c->getId());
	std::string topic_name, message_content;
	if (read_puback(c->getSockFd(), topic_name, message_content) < 0) {
		send_puback(c->getSockFd(), -1);
		return -1;
	}

	printf("Id: %d send a message: '%s' to the topic: '%s'\n", c->getId(), message_content.c_str(), topic_name.c_str());

	int id = topics.at(topic_name)->add_message(message_content);
	send_puback(c->getSockFd(), id);
	printf_verbose("Sent puback to id: %d\n", c->getId());

	printf_verbose("Sending newmes to subscribers:\n");
	for (auto cl : topics.at(topic_name)->get_subscribers()) {
		if (cl == c) {
			continue;
		}
		send_newmes(cl->getSockFd(), topic_name, id, message_content);
		printf_verbose("Sent newmes to id: %d\n", cl->getId());
	}

	printf("----------------\n");
	return 0;
}

void printf_verbose(const char* format, ...) {
	if (!cfg.verbose) {
		return;
	}
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}