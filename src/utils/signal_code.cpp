#include "signal_code.h"

char signal_code_to_char(signal_code code) {
    return (char)((int)code + 'a');
}

signal_code char_to_signal_code(char symbol) {
    int n = symbol - 'a';
    if (0 < n && n < 12) {
        return (signal_code)n;
    }
    return NAS;
}