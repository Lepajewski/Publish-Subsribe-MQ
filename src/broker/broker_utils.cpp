#include "broker_utils.h"

int get_new_id(std::vector<Client*> &clients) {
    int id = 1;

    for (auto &it : clients) {
        printf("%d\n", it->getId());
        if (id == it->getId()) {
            id++;
        } else {
            id = it->getId();
            break;
        }
    }

    return id;
}

void print_clients(std::vector<Client*> clients) {
    printf("--- CLIENTS ---\n");
    for (auto &it : clients) {
        printf("Client: %d, %s:%hu\n", 
            it->getId(),
            inet_ntoa(it->getAddrInfo().sin_addr),
            ntohs(it->getAddrInfo().sin_port));
    }
    printf("---------------\n");
}