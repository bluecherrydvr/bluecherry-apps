/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "bc-server.h"

struct motion_table {
	unsigned int    levels[5];
};

/* Order needs to match the bc_motion_type_t vals */
struct motion_table mts[] = {
	/* Solo6x10 - Range 0 lowest, 0xffff highest */
	{ { 64, 384, 768, 2048, 4096 } },
};

int bc_motion_val(bc_motion_type_t type, const char *str)
{
	struct motion_table *mt = &mts[type];

	if (!strcasecmp(str, "very-low"))
		return mt->levels[0];
	else if (!strcasecmp(str, "low"))
		return mt->levels[1];
	else if (!strcasecmp(str, "medium"))
		return mt->levels[2];
	else if (!strcasecmp(str, "high"))
		return mt->levels[3];
	else if (!strcasecmp(str, "very-high"))
		return mt->levels[4];

	return -1;
}
