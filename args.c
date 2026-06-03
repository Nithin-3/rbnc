#include "args.h"
#include <stdio.h>
#include <stdlib.h>

const char *url;
const char *port;
const char *name;
const char *ip;

static void usage(char *argv[]) {
	fprintf(stderr, "usage: %s -u <url> [-i <ip>] [-p <port>] -n <name>\n", argv[0]);
}

void parseArgs(int argc, char *argv[]) {
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-' && argv[i][1] != '\0' && argv[i][2] == '\0') {
			if (i + 1 >= argc) {
				usage(argv);
				exit(1);
			}
			switch (argv[i][1]) {
				case 'u':
					url = argv[++i];
					break;
				case 'i':
					ip = argv[++i];
					break;
				case 'p':
					port = argv[++i];
					break;
				case 'n':
					name = argv[++i];
					break;
				default:
					usage(argv);
					exit(1);
			}
		} else {
			usage(argv);
			exit(1);
		}
	}

	if (!name || (!url && !(ip && port))) {
		usage(argv);
		exit(1);
	}
}
