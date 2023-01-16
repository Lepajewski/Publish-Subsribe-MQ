#include <stdio.h>
#include <sys/ioctl.h>
#include <iostream>

#include "../client_lib/client_lib.h"
#include "user_input.h"

Client client;

std::string last_error;
std::string clear_line;

void print_menu();
void print_and_get_input(int& action, std::string& argument);
void print_chat(Topic* topic);

void open_topic(std::string name);

int main(int argc, char** argv) {
    if (argc < 2) {
		fprintf(stderr, "No config file passed\n");
		return -1;
	}

    if (client.init(argv[1]) < 0) {
        fprintf(stderr, "%s\n", client.get_last_error().c_str());
        return -1;
    }

    winsize window_size;
    ioctl(0, TIOCGWINSZ, &window_size);
    clear_line = std::string(window_size.ws_col - 1, ' ');

    bool should_close = false;
    bool first_frame = true;
    while (!should_close && !client.get_should_close()) {

        if (first_frame || client.get_new_data()) {
            system("clear");
            print_menu();
            first_frame = false;
        }

        std::string argument;
        int action;
        print_and_get_input(action, argument);

        switch (action) {
            case 1: {
                client.sub_to_topic(argument);
                break;
            }

            case 2: {
                open_topic(argument);
                first_frame = true;
                break;
            }

            case 3: {
                client.unsub_from_topic(argument);
                break;
            }

            case 4: {
                client.disconnect();
                should_close = true;
                break;
            }

            case 2137: {
                last_error = "Najwiekszy Polak";
                break;
            }

            default: {
                last_error = "Not a valid command";
                break;
            }
        }
    }

    system("clear");

    return 0;
}

void print_menu() {
    printf("Hello user: %d!\n", client.get_id());
    printf("Your topics: \n");
    for (auto topic : client.get_subscribed_topics()) {
        printf("%s\n", topic.first.c_str());
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
    if (last_error.empty()) {
        last_error = client.get_last_error();
    }
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

void print_chat(Topic* topic) {
    printf("Opened topic: %s\n", topic->get_name().c_str());
    topic->print_messages();
    printf("\n1. Write message\n");
    printf("2. Back to menu\n");
}

void open_topic(std::string name) {
    if (name == "") {
        last_error = "No topic name given";
        return;
    }
    int index = client.get_topic_index(name);
    if (index == -1) {
        last_error = "Topic is not in your subscribtion list";
        return;
    }
    if (index == -2) {
        last_error = "More than one topic contain given name as prefix";
        return;
    }
    Topic* topic = client.get_topic(index);
    bool topic_menu = true;
    int action;
    std::string argument;
    bool first_frame = true;
    while (topic_menu && !client.get_should_close()) {

        if (first_frame || client.get_new_data()) {
            system("clear");
            print_chat(topic);
            first_frame = false;
        }

        print_and_get_input(action, argument);
        switch (action) {
            case 1: {
                client.send_message(topic, argument);
                break;
            }

            case 2: {
                topic_menu = false;
                break;
            }

            default: {
                last_error = "Not a valid command";
                break;
            }
        }
    }
}