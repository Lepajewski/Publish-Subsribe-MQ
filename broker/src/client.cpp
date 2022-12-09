#include "client.h"
#include <stdio.h>

Client::Client(int id, int fd, float keepalive, sockaddr_in addr) {
    this->id = id;
    this->sock_fd = fd;
    this->keepalive = keepalive;
    this->client_addr = addr;
}

Client::~Client() {
    printf("POLAKI CEBULAKI %d\n", this->id);
    shutdown(this->sock_fd, SHUT_RDWR);
	close(this->sock_fd);
}

int Client::getId() {
    return this->id;
}

int Client::getSockFd() {
    return this->sock_fd;
}

float Client::getKeepalive() {
    return this->keepalive;
}

sockaddr_in Client::getAddrInfo() {
    return this->client_addr;
}
