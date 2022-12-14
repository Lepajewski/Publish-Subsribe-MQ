build-all: build-broker build-client

build-broker:
	mkdir -p broker/bin
	cd broker/src && g++ read_config.cpp setup_socket.cpp broker_utils.cpp client.cpp main.cpp -pthread -o ../bin/broker -Wall -Wextra

build-client:
	mkdir -p client/bin
	cd client/src && g++ main.cpp -o ../bin/client -Wall -Wextra

run-broker:
	./broker/bin/broker broker.conf

run-client:
	./client/bin/client