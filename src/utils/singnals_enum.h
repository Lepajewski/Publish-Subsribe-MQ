#ifndef SIGNALS_ENUM_H
#define SIGNALS_ENUM_H

enum broker_signals {
    CONNACK,                // connect acknowlegdement
    PING,                   // ping signal
    SUBACK,                 // topic subscribed acknowlegdement
    PUBACK,                 // message publish acknowlegdement
    NEWMES,                 // new message
    UNSUBACK,               // unsubscribe topic acknowlegdement
    DISCONNACK,             // disconnect acknowlegdement
};

#endif