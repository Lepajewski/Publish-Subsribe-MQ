build-broker:
	mkdir -p broker/bin
	cd broker/src && g++ read_config.cpp setup_socket.cpp broker_utils.cpp client.cpp main.cpp -pthread -o ../bin/broker -Wall -Wextra

make run-broker:
	./broker/bin/broker broker.conf
