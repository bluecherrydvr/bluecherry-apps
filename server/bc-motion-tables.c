/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "bc-server.h"

/* Convert between known character values and real ranges for the hardware */
struct motion_table {
	unsigned int    levels[6];
};

/* Order needs to match the bc_motion_type_t vals */
struct motion_table mts[] = {
	/* Solo6x10 - Range 0 highest, 0xffff lowest */
	{ { 0xffff, 1152, 1024, 768, 512, 384 } },
};

/* Takes a character from '0' (off) to '5' (high sen) */
int bc_motion_val(bc_motion_type_t type, const char v)
{
	struct motion_table *mt = &mts[type];

	if (v < '0' || v > '5')
		return -1;

	return mt->levels[v - '0'];
}
