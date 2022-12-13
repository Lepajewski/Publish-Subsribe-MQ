#ifndef CLIENT_H
#define CLIENT_H

#include <unistd.h>
#include <netinet/in.h>


class Client {
    int id;
    int sock_fd;
    float keepalive;
    sockaddr_in client_addr;
public:
    Client(int id, int fd, float keepalive, sockaddr_in addr);
    ~Client();
    int getId();
    int getSockFd();
    float getKeepalive();
    sockaddr_in getAddrInfo();
};

#endif
