build-all: build-broker build-client

build-broker:
	mkdir -p bin
	cd src/broker && g++ ../utils/config_parser.cpp ../utils/signal_code.cpp setup_socket.cpp broker_utils.cpp client.cpp signal_handler.cpp main.cpp -pthread -o ../../bin/broker -Wall -Wextra

build-client:
	mkdir -p bin
	cd src/client && g++ ../utils/config_parser.cpp ../utils/signal_code.cpp message.cpp topic.cpp signal_handler.cpp user_input.cpp main.cpp -o ../../bin/client -Wall -Wextra

run-broker:
	./bin/broker broker.conf

run-client:
	./bin/client broker.conf