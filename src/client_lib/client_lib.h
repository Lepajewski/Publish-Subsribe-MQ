#ifndef CLIENT_LIB_H
#define CLIENT_LIB_H

#include <string>
#include <map>
#include <queue>
#include <thread>

#include "topic.h"
#include "../utils/config_parser.h"
#include "../utils/signal_code.h"
#include "../utils/socket_io.h"

class Client {
private:
    int id;
    int sock_fd;
    std::map<std::string, Topic*> subscribed_topics;
    std::queue<std::string> awaiting_topic_messages;
    std::queue<std::string> awaiting_topics;
    std::queue<std::string> unsub_awaiting_topics;
    std::string last_error;

    bool* should_close;
    bool* new_data;

    std::thread receiver_thread;

    //receiver thread
    void receiver_thread_body();
    int handle_suback();
    int handle_puback();
    int handle_newmes();
    int handle_unsuback();

    // signal handler
    void send_conn();
    int read_connack(int &id);

    void send_ping();

    void send_sub(std::string name);
    int read_suback_success(suback_success_code &success);
    int read_suback_messages(Topic* topic);

    void send_pub(std::string topic_name, std::string message);
    int read_puback(int &id);

    void send_unsub(std::string topic_name);
    int read_unsuback(unsuback_success_code &success);

    void send_disconn();

    int read_newmes(std::string &topic_name, int &id, std::string &content);

public:
    // main
    Client();
    ~Client();

    int init(const char * config_path);

    void sub_to_topic(std::string name);
    void send_message(Topic* topic, std::string message);
    void unsub_from_topic(std::string name);
    void disconnect();

    std::string get_last_error();
    bool get_should_close();
    bool get_new_data();
    int get_id();
    int get_topic_index(std::string name);
    Topic* get_topic(int index);
    std::map<std::string, Topic*> get_subscribed_topics();
};

#endif