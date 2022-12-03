#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "read_config.h"

int main() {
	printf("Hello, world!\n");
	Config cfg;

	switch (read_config("broker.conf", &cfg)) {
		case SUCCESS:
			printf("Config: %s, %s", cfg.address.c_str(), cfg.port.c_str());
			break;
		case FILE_NOT_FOUND:
			printf("No config file found");
			break;
		case WRONG_KEY:
			printf("Config: wrong key");
			break;
		case WRONG_VALUE:
			printf("Config: wrong value");
			break;
	}

	return 0;
}
