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
    *this->should_close = true;
    this->disconnect();
    if (this->client_thread.joinable()) {
        this->client_thread.join();
    }

    delete this->should_close;
    delete this->sent_ping;
}

void Client::disconnect() {
    std::lock_guard<std::mutex> lock_discon(this->disconnect_mutex);
    if (this->sock_fd < 0) {
        return;
    }

    this->send_disconn();
    shutdown(this->sock_fd, SHUT_RDWR);
	close(this->sock_fd);
    this->sock_fd = -1;
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
        this->send_ping();
        *this->sent_ping = true;
        this->ping_wait_timestamp = time(nullptr) + PING_WAIT_TIME;
    }
}