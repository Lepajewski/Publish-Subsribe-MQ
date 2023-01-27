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
    char* name_c = new char[size + 1] {0};
    if (read_type(fd, name_c, size) < 0) {
        delete name_c;
        return -1;
    }
    str = name_c;
    delete name_c;
    return 0;
}

int flush_sock(int fd) {
    char buf;
    int len = pread(fd, &buf, 1, 0);
    if (len < 1) {
        return -1;
    }
    while(buf != (char)2) {
        len = read(fd, &buf, 1);
        if (len < 1) {
            return -1;
        }
        len = pread(fd, &buf, 1, 0);
        if (len < 1) {
            return -1;
        }
    }
    return 0;
}