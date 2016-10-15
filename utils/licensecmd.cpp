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

#include <stdio.h>

void usage(const char *argv0)
{
	printf("%s function_name [ARG1 [ARG2]]\n", argv0);
}

int main(int argc, char **argv)
{
	const char *name, *arg1, *arg2;

	if (argc < 2)
	{
		usage(argv[0]);
		return -1;
	}

	

	return 0;
}

