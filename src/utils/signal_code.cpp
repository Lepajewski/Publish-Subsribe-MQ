#include "signal_code.h"

char signal_code_to_char(signal_code code) {
    return (char)((int)code + '0');
}

signal_code char_to_signal_code(char symbol) {
    int n = symbol - '0';
    if (0 < n && n < 8) {
        return (signal_code)n;
    }
    return NAS;
}