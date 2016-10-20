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

#include <vector>

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
		fputs("Failed to open bluecherry database\n", stderr);
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

		if (strcmp(name, "bc_license_check") == 0)
		{
			if (!arg1)
			{
				fputs("missing \"key\" argument for function bc_license_check()\n", stderr);
				return -1;
			}

			int re = bc_license_check(arg1);
			
			printf("%i\n", re); //0 casts to boolean FALSE in php
			break;
		}

		if (strcmp(name, "bc_license_check_auth") == 0)
		{
			if (!arg1 || !arg2)
			{
				fputs("missing \"key\" and \"auth\" arguments for function bc_license_check_auth()\n", stderr);
				return -1;
			}

			if (bc_license_check_auth(arg1, arg2) == 1)
				puts("true");
			else
				puts("0");
			break;
		}

		if (strcmp(name, "bc_license_devices_allowed") == 0)
		{
			std::vector<bc_license> licenses;
			if (bc_read_licenses(licenses) < 0)
			{
				puts("0");
				break;
			}

			int num = 0;

			for (std::vector<bc_license>::iterator it = licenses.begin(); it != licenses.end(); ++it)
				num += it->n_devices;

			printf("%i\n", num);
		}

		fprintf(stderr, "unknown function name \"%s\"\n", name);
		return -1;

	}
	while(0);

	return 0;
}

