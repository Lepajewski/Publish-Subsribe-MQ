#include "client.h"

void Client::client_thread_body() {	
	if (this->read_conn() != 0) {
		fprintf(stderr, "No connection request signal sent from client\n");
		*this->should_close = true;
	}
	else {
		this->send_connack(this->id);
	}

	char action_code;
	while (!*this->should_close) {
		int len = read(this->sock_fd, &action_code, 1);
		if (len < 1) {
			*this->should_close = true;
			break;
		}

		bool valid_action = true;
		switch (char_to_signal_code(action_code)) {
			case SUB: {
				if (this->handle_sub() < 0) {
					fprintf(stderr, "Error with haldling suback from id: %d. Closing connection...\n", this->id);
					*this->should_close = true;
				}
				break;
			}
			
			case PUB: {
				if (this->handle_pub() < 0) {
					fprintf(stderr, "Error with haldling puback from id: %d. Closing connection...\n", this->id);
					*this->should_close = true;
				}
				break;
			}

			case UNSUB: {
				if (this->handle_unsub() < 0) {
					fprintf(stderr, "Error with haldling unsuback from id: %d. Closing connection...\n", this->id);
					*this->should_close = true;
				}
				break;
			}

			case DISCONN: {
				this->broker->printf_verbose("----------------\n");
				this->broker->printf_verbose("Received DISCONN from %d\n", this->id);
				this->broker->printf_verbose("----------------\n");
				*this->should_close = true;
				break;
			}

			case PING: {
				this->broker->printf_verbose("----------------\n");
				this->broker->printf_verbose("Received PING from %d\n", this->id);
				this->broker->printf_verbose("----------------\n");
				*this->sent_ping = false;
				break;
			}

			default: {
				this->broker->printf_verbose("----------------\n");
				this->broker->printf_verbose("Received not supported action from %d\n", this->id);
				this->broker->printf_verbose("----------------\n");
				valid_action = false;
				break;
			}
		}

		if (valid_action) {
			this->ping_timestamp = time(nullptr) + this->ping_every_seconds;
		}

		if (*this->sent_ping && time(nullptr) >= this->ping_wait_timestamp) {
			*this->should_close = true;
		}
	}


	this->disconnect();

	printf("Client disconnected: %s:%hu, id: %d\n", inet_ntoa(this->client_addr.sin_addr), ntohs(this->client_addr.sin_port), this->id);
}

int Client::handle_sub() {
	printf("----------------\n");

	this->broker->printf_verbose("Received SUB from id: %d\n", this->id);
	std::string topic_name;
	if (this->read_sub(topic_name) < 0) {
		this->send_suback(SUBACK_FAILURE, NULL);
		this->broker->printf_verbose("SUB failed reading topic name\n");
		printf("----------------\n");
		return -1;
	}
	this->broker->printf_verbose("Sub (id: %d) topic name: %s\n", this->id, topic_name.c_str());

	auto [topic, created] = this->broker->subscribe_client_to_topic(this, topic_name);

	printf("Id: %d subscribed to '%s'\n", this->id, topic_name.c_str());

	suback_success_code code = created ? SUBACK_CREATE : SUBACK_SUBSCRIBE;
	this->send_suback(code, topic);

	printf("----------------\n");
	return 0;
}

int Client::handle_pub() {
	printf("----------------\n");

	this->broker->printf_verbose("Received PUB from %d\n", this->id);
	std::string topic_name, message_content;
	if (this->read_pub(topic_name, message_content) < 0) {
		this->send_puback(-1);
		this->broker->printf_verbose("PUB failed reading topic name and message content\n");
		printf("----------------\n");
		return -1;
	}

	printf("Id: %d send a message: '%s' to the topic: '%s'\n", this->id, message_content.c_str(), topic_name.c_str());

	int id = this->broker->send_message(this, topic_name, message_content);
	this->send_puback(id);
	this->broker->printf_verbose("Sent puback to id: %d\n", this->id);

	printf("----------------\n");
	return 0;
}

int Client::handle_unsub() {
	printf("----------------\n");

	this->broker->printf_verbose("Received UNSUBACK from %d\n", this->id);
	
	std::string topic_name;
	if (this->read_unsub(topic_name) < 0) {
		this->send_unsuback(UNSUBACK_FAILURE);
		this->broker->printf_verbose("UNSUB failed reading topic name\n");
		printf("----------------\n");
		return -1;
	}

	printf("Id: %d unsubscribed from topic: '%s'\n", this->id, topic_name.c_str());
    this->broker->unsubscribe(this, topic_name);
	this->send_unsuback(UNSUBACK_SUCCESS);

	printf("----------------\n");
	return 0;
}