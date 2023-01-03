#include "broker_utils.h"

int get_new_id(std::set<Client*> clients) {
    int id = 1;

    for (auto &it : clients) {
        if (id == it->getId()) {
            id++;
        } else {
            id = it->getId();
            break;
        }
    }

    return id;
}

void print_clients(std::set<Client*> clients) {
    printf("--- CLIENTS ---\n");
    for (auto it : clients) {
        printf("Client: %d, %s:%hu\n", 
            it->getId(),
            inet_ntoa(it->getAddrInfo().sin_addr),
            ntohs(it->getAddrInfo().sin_port));
    }
    printf("---------------\n");
}