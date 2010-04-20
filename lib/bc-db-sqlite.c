/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <libbluecherry.h>

#include <sqlite3.h>

static void *bc_db_sqlite_open(struct config_t *cfg)
{
	sqlite3 *sqlite;
	const char *file;

	if (!config_lookup_string(cfg, BC_CONFIG_DB ".file", &file))
		return NULL;

	if (sqlite3_open(file, &sqlite)) {
		sqlite3_close(sqlite);
		return NULL;
	}

	return sqlite;
}

static void bc_db_sqlite_close(void *handle)
{
	sqlite3_close(handle);
}

static int bc_db_sqlite_get_table(void *handle, int *nrows, int *ncols,
				  char ***res, const char *fmt, va_list ap)
{
	int ret;
	char *query = sqlite3_vmprintf(fmt, ap);

	if (query == NULL)
		return -1;

	ret = sqlite3_get_table(handle, query, res, nrows, ncols, NULL);
	sqlite3_free(query);

	return ret;
}

static void bc_db_sqlite_free_table(void *handle, char **res)
{
	sqlite3_free_table(res);
}

struct bc_db_ops bc_db_sqlite = {
	.type		= BC_DB_SQLITE,
	.open		= bc_db_sqlite_open,
	.close		= bc_db_sqlite_close,
	.get_table	= bc_db_sqlite_get_table,
	.free_table	= bc_db_sqlite_free_table,
};
