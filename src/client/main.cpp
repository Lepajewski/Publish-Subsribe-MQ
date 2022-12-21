#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <sys/ioctl.h>

#include "../utils/config_parser.h"
#include "../utils/signal_code.h"
#include "signal_handler.h"
#include "topic.h"
#include "user_input.h"

int id;
int sock_fd;
std::vector<Topic> subscribed_topics;
std::string last_error;
std::string clear_line;

void print_menu();
void print_and_get_input(int& action, std::string& argument);

void sub_to_topic(std::string name);

int main(int argc, char** argv) {
    if (argc < 2) {
		fprintf(stderr, "No config file passed");
		return -1;
	}

    winsize window_size;
    ioctl(0, TIOCGWINSZ, &window_size);
    clear_line = std::string(window_size.ws_col - 1, ' ');

    Config cfg = parse_config(argv[1]);
    printf("Config loaded\n");

    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd == -1) {
        perror("Socket init failed");
        return -1;
    }

    sockaddr_in sock_addr = {};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = cfg.port;
    sock_addr.sin_addr.s_addr = cfg.address.s_addr;

    if (connect(sock_fd, (sockaddr*) &sock_addr, sizeof(sock_addr))) {
        perror("Connect failed");
        return -1;
    }

    send_connack(sock_fd);
    if (read_connack(sock_fd, id) != 0) {
        fprintf(stderr, "No connect acknowlegment from broker.\n");
        shutdown(sock_fd, SHUT_RDWR);
        close(sock_fd);
        return -1;
    }

    bool should_close = false;
    bool first_frame = true;
    system("clear");
    while (!should_close) {

        if (first_frame) {
            print_menu();
            first_frame = false;
        }

        std::string argument;
        int action;
        print_and_get_input(action, argument);

        switch (action) {
            case 1:
                sub_to_topic(argument);
                break;

            case 2:
                last_error = "Not implemented";
                break;

            case 3:
                last_error = "Not implemented";
                break;

            case 4:
                last_error = "Not implemented";
                break;

            case 2137:
                last_error = "Najwiekszy Polak";
                break;

            default:
                last_error = "Not a valid command";
                break;
        }
    }

    shutdown(sock_fd, SHUT_RDWR);
    close(sock_fd);

    return 0;
}

void print_menu() {
    printf("Hello user: %d!\n", id);
    printf("Your topics: \n");
    for (Topic topic : subscribed_topics) {
        printf("%s\n", topic.get_name().c_str());
    }
    printf("\n");
    printf("1. Subscribe to topic\n");
    printf("2. Open topic\n");
    printf("3. Unsubscribe from topic\n");
    printf("4. Exit\n");
}

void print_and_get_input(int& action, std::string& argument) {
    printf("\n%s\r", clear_line.c_str());
    printf("\x1b[31m%s\x1b[0m\n", last_error.c_str());
    //set font color to red; print error; unset font style; newline
    last_error.clear();

    std::string command;
    std::getline(std::cin, command);
    printf("%s\r", clear_line.c_str());
    printf("%s", command.c_str());
    printf("\x1b[1A\r%s\x1b[2A\r", clear_line.c_str());
    //go up one line; go to the begining of line; print clear; go up 2 lines; go to the begining of line

    parse_input(command, action, argument);
}

void sub_to_topic(std::string name) {
    if (name == "") {
        last_error = "No topic name given in agrument";
        return;
    }
    if (check_if_topic_is_in_list(subscribed_topics, name)) {
        last_error = "Topic already subscribed";
        return;
    }
    send_suback(sock_fd, name);
}