#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd == -1) {
        perror("Socket init failed");
        return -1;
    }

    in_addr addr;
    inet_aton("127.0.0.1", &addr);

    sockaddr_in sock_addr = {};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(2137);
    sock_addr.sin_addr.s_addr = addr.s_addr;

    if (connect(sock_fd, (sockaddr*) &sock_addr, sizeof(sock_addr))) {
        perror("Connect failed");
        return -1;
    }

    write(sock_fd, "Hello world!", 12);

    shutdown(sock_fd, SHUT_RDWR);
    close(sock_fd);

    return 0;
}