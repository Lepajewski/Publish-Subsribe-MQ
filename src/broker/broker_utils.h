#ifndef BROKER_SIGNAL_H
#define BROKER_SIGNAL_H

#include <stdio.h>
#include <vector>
#include <arpa/inet.h>
#include "client.h"

int get_new_id(std::vector<Client*> &clients);
void print_clients(std::vector<Client*> clients);

#endif
