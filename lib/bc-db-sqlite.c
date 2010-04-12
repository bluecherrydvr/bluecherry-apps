/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <libbluecherry.h>

#include <sqlite.h>

static const char db_name[] = "/var/lib/bluecherry/" BC_DB_NAME;

static void *bc_db_sqlite_open(void)
{
	return sqlite_open(db_name, 0, NULL);
}

static void bc_db_sqlite_close(void *handle)
{
	sqlite_close(handle);
}

static int bc_db_sqlite_get_table(void *handle, int *nrows, int *ncols,
				  char ***res, const char *fmt, va_list ap)
{
	return sqlite_get_table_vprintf(handle, fmt, res, nrows, ncols,
					NULL, ap);
}

static void bc_db_sqlite_free_table(void *handle, char **res)
{
	sqlite_free_table(res);
}

struct bc_db_ops bc_db_sqlite = {
	.type		= BC_DB_SQLITE,
	.open		= bc_db_sqlite_open,
	.close		= bc_db_sqlite_close,
	.get_table	= bc_db_sqlite_get_table,
	.free_table	= bc_db_sqlite_free_table,
};
