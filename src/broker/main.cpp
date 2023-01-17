#include <signal.h>
#include <stdio.h>

#include "broker.h"

Broker* broker;

static void signal_handler(int signum);

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "No config file passed");
		return -1;
	}

	broker = new Broker(argv[1]);

	if (broker->init() < 0) {
		delete broker;
		return -1;
	}

	printf("Broker running...\n");

	// setup signal handler
	signal(SIGINT, signal_handler);


	// main broker loop
	bool should_close = false;
	while (!should_close) {
		broker->loop();
	}

	delete broker;

	return 0;
}

static void signal_handler(int signum) {
	printf("\nKeyboard Interrupt ...closing broker\n");

	delete broker;

	exit(signum);
}