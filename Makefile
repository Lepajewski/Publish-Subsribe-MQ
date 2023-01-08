build-all: build-broker build-client

build-broker:
	mkdir -p bin
	cd src/broker && g++ ../utils/config_parser.cpp ../utils/signal_code.cpp ../utils/socket_io.cpp setup_socket.cpp broker_utils.cpp client.cpp signal_handler.cpp message.cpp topic.cpp main.cpp -pthread -o ../../bin/broker -Wall -Wextra

build-client:
	mkdir -p bin
	cd src/client && g++ ../utils/config_parser.cpp ../utils/signal_code.cpp ../utils/socket_io.cpp random_helper.cpp message.cpp topic.cpp signal_handler.cpp user_input.cpp main.cpp -pthread -o ../../bin/client -Wall -Wextra

run-broker:
	./bin/broker broker.conf

run-client:
	./bin/client broker.conf