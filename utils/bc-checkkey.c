/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <libbluecherry.h>

extern char *__progname;

static void usage(void)
{
        fprintf(stderr, "Usage: %s <key1> ... <keyN>\n", __progname);
	exit(1);
}

int main(int argc, char **argv)
{
	int opt;

	while ((opt = getopt(argc, argv, "h")) != -1) {
		switch (opt) {
		case 'h': default: usage();
		}
	}

	if (optind >= argc)
		usage();

	while (optind < argc) {
		struct bc_key_data res;

		printf ("Checking key: %s\n", argv[optind]);

		if (bc_key_process(&res, argv[optind]))
			printf("  E: Invalid key\n");
		else {
			printf("  I: Valid key\n");
			printf("  I: type: %d\n", res.type);
			printf("  I: minor: %d\n", res.minor);
			printf("  I: major: %d\n", res.major);
			printf("  I: count: %d\n", res.count);
			printf("  I: id: %d\n", res.id);
		}

		optind++;
	}

	exit(0);
}
