#include "client_lib.h"

void Client::receiver_thread_body() {
    char action_code;
	while (!*this->should_close) {
		int len = read(this->sock_fd, &action_code, 1);
		if (len < 1) {
			*this->should_close = true;
			break;
		}

        switch (char_to_signal_code(action_code)) {
            case SUBACK: {
                int ret = this->handle_suback();
                if (ret < 0) {
                    *this->should_close = true;
                    this->last_error = "Error handling suback";
                }
                else if (ret > 0) {
                    // signalize that main menu should be redrawn (show just subscribed topic) to change
                    *this->new_data = true;
                }
                break;
            }
            
            case PUBACK: {
                if (this->handle_puback() < 0) {
                    this->last_error = "Error handling puback";
                    *this->should_close = true;
                }
                else {
                    // signalize that main menu should be redrawn (show just published message) to change
                    *this->new_data = true;
                }
                break;
            }

            case NEWMES: {
                if (this->handle_newmes() < 0) {
                    this->last_error = "Error handling newmes";
                    *this->should_close = true;
                }
                else {
                    // idk what to do here now
                    *this->new_data = true;
                }
                break;
            }

            case UNSUBACK: {
                if (this->handle_unsuback() < 0) {
                    this->last_error = "Error handling unsuback";
                    *this->should_close = true;
                }
                else {
                    // signalize that main menu should be redrawn (show just unsubscribed topic) to change
                    *this->new_data = true;
                }
                break;
            }

            case DISCONN: {
                *this->should_close = true;
                break;
            }

            default: {
                break;
            }
        }
    }
}

int Client::handle_suback() {
    suback_success_code success;

    std::string topic_name = this->awaiting_topics.front();
    this->awaiting_topics.pop();

    if (this->read_suback_success(success) < 0) {
        return -1;
    }

    if (success == SUBACK_FAILURE) {
        return 0;
    }

    Topic* t = new Topic(topic_name);
    this->subscribed_topics.insert({topic_name, t});

    if (this->read_suback_messages(this->subscribed_topics.at(topic_name)) < 0) {
        return -1;
    }

    return 1;
}

int Client::handle_puback() {
    int id;

    std::string topic_name = this->awaiting_topic_messages.front();
    this->awaiting_topic_messages.pop();

    if (this->read_puback(id) < 0) {
        return -1;
    }

    this->subscribed_topics.at(topic_name)->put_message(id);

    return 0;
}

int Client::handle_newmes() {
    int id;
    std::string topic_name;
    std::string content;

    if (this->read_newmes(topic_name, id, content) < 0) {
        return -1;
    }

    this->subscribed_topics.at(topic_name)->add_message(id, content);

    return 0;
}

int Client::handle_unsuback() {
    unsuback_success_code success;
    std::string topic_name = this->unsub_awaiting_topics.front();
    this->unsub_awaiting_topics.pop();

    if (this->read_unsuback(success) < 0) {
        return -1;
    }

    if (success == UNSUBACK_FAILURE) {
        return 0;
    }

    delete this->subscribed_topics.at(topic_name);
    this->subscribed_topics.erase(topic_name);

    return 0;
}