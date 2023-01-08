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
#include <unordered_set>
#include <thread>

#include "../utils/config_parser.h"
#include "../utils/signal_code.h"
#include "signal_handler.h"
#include "topic.h"
#include "user_input.h"
#include "random_helper.h"

int id;
int sock_fd;
std::vector<Topic> subscribed_topics;
std::string last_error;
std::string clear_line;
std::unordered_set<std::string> sub_awaiting_topics;
std::unordered_set<std::string> unsub_awaiting_topics;

void print_menu();
void print_and_get_input(int& action, std::string& argument);
void print_chat(Topic topic);

void sub_to_topic(std::string name);
void open_topic(std::string name);
void send_message(Topic topic, std::string message);
void unsub_from_topic(std::string name);

static void receiver_thread_body();

void load_sample_data();

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

    std::thread receiver_thread(receiver_thread_body);

    bool should_close = false;
    bool first_frame = true;
    while (!should_close) {

        if (first_frame) {
            system("clear");
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
                open_topic(argument);
                first_frame = true;
                break;

            case 3:
                unsub_from_topic(argument);
                break;

            case 4:
                send_disconnack(sock_fd);
                should_close = true;
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

    system("clear");

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
    //set font color to red; print error; unset font style; newline
    printf("\x1b[31m%s\x1b[0m\n", last_error.c_str());
    last_error.clear();

    std::string command;
    std::getline(std::cin, command);
    printf("%s\r", clear_line.c_str());
    printf("%s", command.c_str());
    //go up one line; go to the begining of line; print clear; go up 2 lines; go to the begining of line
    printf("\x1b[1A\r%s\x1b[2A\r", clear_line.c_str());

    parse_input(command, action, argument);
}

void print_chat(Topic topic) {
    printf("Opened topic: %s\n", topic.get_name().c_str());
    topic.print_messages();
    printf("\n1. Write message\n");
    printf("2. Back to menu\n");
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
    sub_awaiting_topics.insert(name);
}

void open_topic(std::string name) {
    int index = get_topic_index(subscribed_topics, name);
    if (index == -1) {
        last_error = "Topic is not in your subscribtion list";
        return;
    }
    if (index == -2) {
        last_error = "More than one topic contain given name as prefix";
        return;
    }
    Topic topic = subscribed_topics[index];
    bool should_close = false;
    system("clear");
    print_chat(topic);
    int action;
    std::string argument;
    while (!should_close) {
        print_and_get_input(action, argument);
        switch (action) {
            case 1:
                send_message(topic, argument);
                break;

            case 2:
                should_close = true;
                break;

            default:
                last_error = "Not a valid command";
                break;
        }
    }
}

void send_message(Topic topic, std::string message) {
    if (message == "") {
        last_error = "No message given";
        return;
    }
    send_puback(sock_fd, topic.get_name(), message);
}

void unsub_from_topic(std::string name) {
    if (name == "") {
        last_error = "No topic name given";
        return;
    }
    int index = get_topic_index(subscribed_topics, name);
    if (index == -1) {
        last_error = "Topic is not in your subscribtion list";
        return;
    }
    if (index == -2) {
        last_error = "More than one topic contain given name as prefix";
        return;
    }
    Topic topic = subscribed_topics[index];
    send_unsuback(sock_fd, topic.get_name());
    unsub_awaiting_topics.insert(topic.get_name());
}

static void receiver_thread_body() {
    bool should_close = false;
    char action_code;
	while (!should_close) {
		int len = read(sock_fd, &action_code, 1);
		if (len < 1) {
			should_close = true;
			break;
		}

        switch (char_to_signal_code(action_code)) {
            case SUBACK:
                printf("received suback from broker\n");
                break;
            
            case PUBACK:
                printf("received puback from broker\n");
                break;

            case NEWMES:
                printf("received newmes from broker \n");
                break;

            case UNSUBACK:
                printf("received unsuback from broker\n");
                break;

            case DISCONNACK:
                printf("received disconnack from broker\n");
                break;

            default:
                printf("received not supported signal from broker\n");
                break;
        }
    }
}

void load_sample_data() {
    for (int i = 0; i < 3; i++) {
        subscribed_topics.push_back(get_random_topic(20));
    }
}