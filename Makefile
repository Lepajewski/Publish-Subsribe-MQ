build-all: build-broker build-client

build-broker:
	mkdir -p bin
	cd src/broker && g++ ../utils/config_parser.cpp ../utils/signal_code.cpp ../utils/socket_io.cpp setup_socket.cpp broker_utils.cpp topic.cpp client.cpp signal_handler.cpp message.cpp main.cpp -pthread -o ../../bin/broker -Wall -Wextra

build-client:
	mkdir -p bin
	cd src && g++ utils/config_parser.cpp utils/signal_code.cpp utils/socket_io.cpp client_lib/message.cpp client_lib/topic.cpp client_lib/client_signal_handler.cpp client_lib/client_receiver_thread.cpp client_lib/client_main.cpp sample_client/user_input.cpp sample_client/main.cpp -pthread -o ../bin/client -Wall -Wextra

run-broker:
	./bin/broker config.conf

run-client:
	./bin/client config.conf