#ifndef BROKER_SIGNAL_H
#define BROKER_SIGNAL_H

#include <stdio.h>
#include <set>
#include <arpa/inet.h>
#include "client.h"

int get_new_id(std::set<Client*> clients);
void print_clients(std::set<Client*> clients);

#endif
