#include "socket_io.h"

int read_type(int fd, void* buf, size_t size) {
    size_t n = 0;
    do {
        int ret = read(fd, (void*)((char*)buf + n), size - n);
        if (ret < 0) {
            return -1;
        }
        n += ret;
    } while (n < size);
    return 0;
}

int read_string(int fd, std::string &str, size_t size) {
    char* name_c = new char[size];
    if (read_type(fd, name_c, size) < 0) {
        delete name_c;
        return -1;
    }
    str = name_c;
    delete name_c;
    return 0;
}