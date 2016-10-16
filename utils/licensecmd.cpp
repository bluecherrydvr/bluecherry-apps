/*
 *  * Copyright (C) 2016 Bluecherry, LLC
 *  *
 *  * Confidential, all rights reserved. No distribution is permitted.
 *  */

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>


#include "libbluecherry.h"
#include "utils.h"

#include <stdio.h>



void usage(const char *argv0)
{
	printf("%s function_name [ARG1 [ARG2]]\n", argv0);
}

int main(int argc, char **argv)
{
	const char *name, *arg1 = NULL, *arg2 = NULL;

	if (argc < 2)
	{
		usage(argv[0]);
		return -1;
	}

	name = argv[1];

	if (argc >= 3)
		arg1 = argv[2];
	if (argc >= 4)
		arg2 = argv[3];

	if (bc_db_open(BC_CONFIG_DEFAULT))
	{
		fputs("Failed to open bluecherry database", stderr);
		return -1;
	}

	do
	{
		if (strcmp(name, "bc_license_machine_id") == 0)
		{
			char buf[64];
			int re = bc_license_machine_id(buf, sizeof(buf));

			if (re < 1)
				return -1;

			puts(buf);
			break;
		}
	}
	while(0);

	return 0;
}

