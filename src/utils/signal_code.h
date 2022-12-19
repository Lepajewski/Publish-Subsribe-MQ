#ifndef SIGNAL_CODE_H
#define SIGNAL_CODE_H

enum signal_code {
    CONNACK = 1,            // connect acknowlegdement
    PING = 2,               // ping signal
    SUBACK = 3,             // topic subscribed acknowlegdement
    PUBACK = 4,             // message publish acknowlegdement
    NEWMES = 5,             // new message
    UNSUBACK = 6,           // unsubscribe topic acknowlegdement
    DISCONNACK = 7,         // disconnect acknowlegdement
    NAS = 0                 // not a signal
};

char signal_code_to_char(signal_code code);
signal_code char_to_signal_code(char symbol);

#endif