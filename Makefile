build-broker:
	cd broker/src && g++ read_config.cpp setup_socket.cpp main.cpp  -o ../bin/broker -Wall -Wextra

make run-broker:
	./broker/bin/broker broker.conf
