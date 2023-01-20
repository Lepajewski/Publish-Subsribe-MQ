#include "client_lib.h"

Client::Client() {
    this->should_close = new bool;
    this->new_data = new bool;

    *this->should_close = false;
    *this->new_data = false;
    this->sock_fd = -1;
}

Client::~Client() {
    this->disconnect();

    delete this->should_close;
    delete this->new_data;
}

int Client::init(const char * config_path) {
    this->disconnect();

    Config cfg = parse_config(config_path);

    this->sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->sock_fd == -1) {
        this->last_error = "Socket init failed";
        return -1;
    }

    sockaddr_in sock_addr = {};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = cfg.port;
    sock_addr.sin_addr.s_addr = cfg.address.s_addr;

    if (connect(this->sock_fd, (sockaddr*) &sock_addr, sizeof(sock_addr))) {
        this->last_error = "Connect failed";
        return -1;
    }

    this->send_conn();
    if (this->read_connack(id) != 0) {
        this->last_error = "No connect acknowlegment from broker";
        shutdown(this->sock_fd, SHUT_RDWR);
        close(this->sock_fd);
        return -1;
    }

    *this->should_close = false;
    *this->new_data = false;
    this->receiver_thread = std::thread(&Client::receiver_thread_body, this);

    return 0;
}

int Client::get_topic_index(std::string name) {
    int topic_index = -1;
    int i = 0;
    for (auto it : this->subscribed_topics) {
        std::string topic_name_substr = it.second->get_name().substr(0, name.size());
        if (topic_name_substr == name) {
            if (topic_index != -1) {
                return -2;
            }
            else {
                topic_index = i;
            }
        }
        i++;
    }
    return topic_index;
}

void Client::sub_to_topic(std::string name) {
    if (name == "") {
        this->last_error = "No topic name given";
        return;
    }
    if (this->subscribed_topics.count(name) > 0) {
        this->last_error = "Topic already subscribed";
        return;
    }
    this->awaiting_topics.push(name);
    this->send_sub(name);
}

void Client::send_message(Topic* topic, std::string message) {
    if (message == "") {
        this->last_error = "No message given";
        return;
    }
    this->awaiting_topic_messages.push(topic->get_name());
    topic->queue_message(message);
    this->send_pub(topic->get_name(), message);
}

void Client::unsub_from_topic(std::string name) {
    if (name == "") {
        this->last_error = "No topic name given";
        return;
    }
    int index = this->get_topic_index(name);
    if (index == -1) {
        this->last_error = "Topic is not in your subscribtion list";
        return;
    }
    if (index == -2) {
        this->last_error = "More than one topic contain given name as prefix";
        return;
    }
    std::map<std::string, Topic*>::iterator it = std::next(this->subscribed_topics.begin(), index);
    Topic* topic = it->second;
    this->unsub_awaiting_topics.push(topic->get_name());
    this->send_unsub(topic->get_name());
}

void Client::disconnect() {
    if (!*this->should_close)
    {
        this->send_disconn();
        *this->should_close = true;
    }
    if (this->receiver_thread.joinable()) {
        this->receiver_thread.join();
    }

    for (auto [n, t] : this->subscribed_topics)
    {
        delete t;
    }
    this->subscribed_topics.clear();

    if (this->sock_fd > 0) {
        shutdown(this->sock_fd, SHUT_RDWR);
        close(this->sock_fd);
        this->sock_fd = -1;
    }
}

std::string Client::get_last_error() {
    return this->last_error;
}

bool Client::get_should_close() {
    return *this->should_close;
}

bool Client::get_new_data() {
    bool r = *this->new_data;
    *this->new_data = false;
    return r;
}

int Client::get_id() {
    return this->id;
}

Topic* Client::get_topic(int index) {
    std::map<std::string, Topic*>::iterator it = std::next(this->subscribed_topics.begin(), index);
    return it->second;
}

std::map<std::string, Topic*> Client::get_subscribed_topics() {
    return this->subscribed_topics;
}