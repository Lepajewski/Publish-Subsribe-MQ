build-all: build-broker build-client

build-broker:
	mkdir -p bin
	cd src/broker && g++ read_config.cpp setup_socket.cpp broker_utils.cpp client.cpp main.cpp -pthread -o ../../bin/broker -Wall -Wextra

build-client:
	mkdir -p bin
	cd src/client && g++ main.cpp -o ../../bin/client -Wall -Wextra

run-broker:
	./bin/broker broker.conf

run-client:
	./bin/client