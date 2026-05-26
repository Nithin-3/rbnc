#include "args.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

const char *url;
const char *port;
const char *name;

static void usage(char *argv[]) {
	fprintf(stderr, "usage: %s -u <url> -p <port> -n <name>\n", argv[0]);
}

void parseArgs(int argc, char *argv[]) {
	int opt;

	while ((opt = getopt(argc, argv, "u:p:n:")) != -1) {
		switch (opt) {
			case 'u':
				url = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case 'n':
				name = optarg;
				break;
			default:
				usage(argv);
				exit(1);
		}
	}

	if (!url || !port || !name) {
		usage(argv);
		exit(1);
	}
}
