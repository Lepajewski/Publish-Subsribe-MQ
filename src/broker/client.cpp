#include "client.h"

Client::Client(Broker* broker, int fd, sockaddr_in addr, time_t ping_every_seconds) {
    this->broker = broker;
    this->id = broker->get_new_id();
    this->sock_fd = fd;
    this->ping_every_seconds = ping_every_seconds;
    this->ping_timestamp = time(nullptr) + ping_every_seconds;
    this->client_addr = addr;

    this->should_close = new bool;
    *this->should_close = false;

    this->sent_ping = new bool;
    *this->sent_ping = false;

    this->client_thread = std::thread(&Client::client_thread_body, this);
}

Client::~Client() {
    printf("POLAKI CEBULAKI %d\n", this->id);

    *this->should_close = true;
    this->disconnect();
    if (this->client_thread.joinable()) {
        this->client_thread.join();
    }

    delete this->should_close;
    delete this->sent_ping;
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

sockaddr_in Client::get_addr_info() {
    return this->client_addr;
}

bool Client::get_should_close() {
    return *this->should_close;
}

void Client::time_update() {
    if (time(nullptr) >= this->ping_timestamp && !*this->sent_ping) {
        this->broker->printf_verbose("Sending PING to id: %d\n", this->id);
        send_ping(this->sock_fd);
        *this->sent_ping = true;
        this->ping_wait_timestamp = time(nullptr) + PING_WAIT_TIME;
    }
}