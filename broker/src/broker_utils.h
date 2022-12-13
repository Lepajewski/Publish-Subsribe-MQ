#ifndef BROKER_SIGNAL_H
#define BROKER_SIGNAL_H

#include <stdio.h>
#include <vector>
#include <arpa/inet.h>
#include "client.h"


enum broker_signals {
    CONNACK,                // client connected acknowlegdement
    PING,                   // ping signal
    SUBACK,                 // topic subscribed acknowlegdement
    PUBACK,                 // message published acknowlegdement
    MESRCVSACK,             // message received acknowlegdement
    UNSUBACK,               // client unsubscribed topic acknowlegdement
    DISCONNACK,             // client disconnected acknowlegdement
};

int get_new_id(std::vector<Client*> &clients);
void print_clients(std::vector<Client*> clients);

#endif
