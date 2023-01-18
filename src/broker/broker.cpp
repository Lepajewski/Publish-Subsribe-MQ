#include "broker.h"

Broker::Broker(const char * config_path) {
    this->cfg = parse_config(config_path);
}

Broker::~Broker() {
    for (Client* c : this->clients) {
		delete c;
	}

    for (auto t : this->topics) {
		delete t.second;
	}

    shutdown(this->sock_fd, SHUT_RDWR);
    close(this->sock_fd);
}

int Broker::init() {
    sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = this->cfg.address.s_addr;
	addr.sin_port = this->cfg.port;

	this->sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->sock_fd == -1) {
		perror("Socket init failed");
		return -1;
	}
    this->printf_verbose("Broker socket created\n");
	
    const int enable = 1;
    if (setsockopt(this->sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
        perror("Set socket options failed");
        return -1;
    }
    this->printf_verbose("Set SO_REUSEADDR\n");

	if (bind(this->sock_fd, (sockaddr*) &addr, sizeof(addr))) {
		perror("Socket bind failed");
		return -1;
	}
    this->printf_verbose("Socket bound\n");

    if (this->sock_fd == -1) {
		fprintf(stderr, "Socket setup failed");
		return -1;
	}
    this->printf_verbose("Broker socket setup\n");

	if (listen(this->sock_fd, cfg.max_clients)) {
		perror("Broker socket listen failed");
		return -1;
	}
    this->printf_verbose("Started listening\n");

    return 0;
}

void Broker::accept_client() {
    sockaddr_in client_addr{};
	socklen_t client_addr_size = sizeof(client_addr);

	int client_sock = accept(this->sock_fd, (sockaddr*) &client_addr, &client_addr_size);
	if (client_sock == -1) {
		perror("Client accept failed");
	}

	Client* c = new Client(this, client_sock, client_addr, 60.0);
	this->clients.insert(c);

    if (this->cfg.verbose) {
		this->print_clients();
		printf("New connection from %s:%hu as %d\n", 
				inet_ntoa(client_addr.sin_addr),
				ntohs(client_addr.sin_port), 
				c->get_id());
	}
}

void Broker::remove_client() {
    std::set<Client*> to_remove;

    for (Client* c : this->clients) {
        if (c->get_should_close()) {
            to_remove.insert(c);
        }
    }

    for (Client* c : to_remove) {
        this->clients.erase(c);
        delete c;
    }
}

void Broker::loop() {
    this->remove_client();
    this->accept_client();
}

int Broker::get_new_id() {
	std::lock_guard<std::mutex> lock_sub(this->subscribe_mutex);

	std::vector<int> taken_ids;
    for (auto &it : this->clients) {
        if (!it->get_should_close()) {
            taken_ids.push_back(it->get_id());
        }
    }

	std::sort(taken_ids.begin(), taken_ids.end());

    int id = 1;
	for (int i : taken_ids) {
		if (id == i) {
			id++;
		}
	}

    return id;
}

std::pair<Topic*, bool> Broker::subscribe_client_to_topic(Client* client, std::string name) {
	std::lock_guard<std::mutex> lock_sub(this->subscribe_mutex);
    bool created = false;
    if ( (created = (this->topics.count(name) < 1)) ) {
		Topic* t = new Topic(name);
		topics.insert({name, t});
		this->printf_verbose("Topic '%s' created\n", name.c_str());
	}

    topics.at(name)->subscribe_client(client);
    return {topics.at(name), created};
}

int Broker::send_message(Client* client, std::string topic_name, std::string content) {
	std::lock_guard<std::mutex> lock_sub(this->message_mutex);
    int id = this->topics.at(topic_name)->add_message(content);

	std::lock_guard<std::mutex> lock_mes(this->subscribe_mutex);
    this->printf_verbose("Sending newmes to subscribers:\n");
	for (auto c : topics.at(topic_name)->get_subscribers()) {
		if (c == client) {
			continue;
		}
		send_newmes(c->get_sock_fd(), topic_name, id, content);
		this->printf_verbose("Sent newmes to id: %d\n", c->get_id());
	}

    return id;
}

void Broker::unsubscribe(Client* client, std::string name) {
	std::lock_guard<std::mutex> lock_sub(this->subscribe_mutex);
    if (topics.at(name)->unsubscribe_client(client) == 0) {
		this->printf_verbose("No subscribers in topic: '%s'\n", name.c_str());
	}
}

void Broker::printf_verbose(const char* format, ...) {
	if (!this->cfg.verbose) {
		return;
	}
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

void Broker::print_clients() {
    printf("--- CLIENTS ---\n");
    for (auto it : this->clients) {
        printf("Client: %d, %s:%hu\n", it->get_id(), inet_ntoa(it->get_addr_info().sin_addr), ntohs(it->get_addr_info().sin_port));
    }
    printf("---------------\n");
}