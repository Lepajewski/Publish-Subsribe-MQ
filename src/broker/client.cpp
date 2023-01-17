#include "client.h"

Client::Client(Broker* broker, int fd, sockaddr_in addr, float keepalive) {
    this->broker = broker;
    this->id = broker->get_new_id();
    this->sock_fd = fd;
    this->keepalive = keepalive;
    this->client_addr = addr;

    this->should_close = new bool;
    *this->should_close = false;

    this->client_thread = std::thread(&Client::client_thread_body, this);
}

Client::~Client() {
    printf("POLAKI CEBULAKI %d\n", this->id);

    *this->should_close = true;
    this->disconnect();
    if (this->client_thread.joinable()) {
        this->client_thread.join();
    }
}

void Client::disconnect() {
    send_disconn(this->sock_fd);
    shutdown(this->sock_fd, SHUT_RDWR);
	close(this->sock_fd);
}

int Client::get_id() {
    return this->id;
}

int Client::get_sock_fd() {
    return this->sock_fd;
}

float Client::get_keepalive() {
    return this->keepalive;
}

sockaddr_in Client::get_addr_info() {
    return this->client_addr;
}

bool Client::get_should_close() {
    return *this->should_close;
}