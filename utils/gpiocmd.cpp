/*
 *  * Copyright (C) 2017 Bluecherry, LLC
 *  *
 *  * Confidential, all rights reserved. No distribution is permitted.
 *  */

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>


//#include "libbluecherry.h"
//#include "utils.h"

#include <stdio.h>

#include <vector>

void usage(const char *argv0)
{       
        printf("%s function_name [ARG1 [ARG2]]\n", argv0);
}

int main(int argc, char **argv)
{
        const char *name, *arg1 = NULL, *arg2 = NULL;
	FILE *f;

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

	do
	{
		if (strcmp(name, "export_pin") == 0)
		{
			if (!arg1)
			{
				fputs("pin number is required\n", stderr);
				return -1;
			}

			f = fopen("/sys/class/gpio/export", "w");

			if (!f)
			{
				perror("export_pin");
				return -1;
			}

			if (!fwrite(arg1, strlen(arg1), 1, f))
			{
				perror("export_pin");
				return -1;
			}

			fclose(f);

			break;
		}

		fprintf(stderr, "unknown function name \"%s\"\n", name);
                return -1;
	}
	while(0);


	return 0;
}

