build-broker:
	cd broker/src && g++ main.cpp -o ../bin/broker -Wall -Wextra

make run-broker:
	./broker/bin/broker