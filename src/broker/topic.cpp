#include "topic.h"

Topic::Topic(std::string name) {
    this->name = name;
}

Topic::~Topic() {
    for (Message* m : this->messages) {
        delete m;
    }
}

int Topic::add_message(std::string message) {
    std::lock_guard<std::mutex> lock_mes(this->message_mutex);
    int id = this->get_new_id();
    Message* m = new Message(id, message, FIVE_MINUTES);
    this->messages.push_back(m);
    return id;
}

void Topic::notify_subscribers(Client* client, int id, std::string content) {
    std::lock_guard<std::mutex> lock_sub(this->subscribe_mutex);
	for (auto c : this->subscribers) {
		if (c == client || c->get_should_close()) {
			continue;
		}
		c->send_newmes(this->name, id, content);
	}
}

int Topic::send_message(Client* client, std::string content) {
    int id = this->add_message(content);
    this->notify_subscribers(client, id, content);
    return id;
}

void Topic::subscribe_client(Client* client) {
    std::lock_guard<std::mutex> lock_sub(this->subscribe_mutex);
    this->subscribers.insert(client);
}

std::string Topic::get_name() {
    return this->name;
}

std::vector<Message*> Topic::get_messages() {
    return this->messages;
}

int Topic::get_new_id() {
    int i = 0;
    for (Message* m : this->messages) {
        if (m->id > i) {
            i = m->id;
        }
    }
    return i+1;
}

std::set<Client*> Topic::get_subscribers() {
    return this->subscribers;
}

int Topic::unsubscribe_client(Client* client) {
    std::lock_guard<std::mutex> lock_sub(this->subscribe_mutex);
    this->subscribers.erase(client);
    return this->subscribers.size();
}

void Topic::time_update() {
    std::lock_guard<std::mutex> lock_mes(this->message_mutex);
    this->messages.erase(std::remove_if(this->messages.begin(), this->messages.end(), [](Message* m) {
        bool rem = m->time_update();
        if (rem) {
            delete m;
        }
        return rem;
    }), this->messages.end());
}
