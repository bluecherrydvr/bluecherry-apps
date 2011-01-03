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
		bc_log("(SQL ERROR): Opening db %s: %s", file,
		       sqlite3_errmsg(sqlite));
		sqlite3_close(sqlite);
		return NULL;
	}

	/* Enable foreign keys */
	sqlite3_exec(sqlite, "PRAGMA foreign_keys=ON;", NULL, NULL, NULL);

	return sqlite;
}

static void bc_db_sqlite_close(void *handle)
{
	sqlite3_close(handle);
}

static int bc_db_sqlite_query(void *handle, const char *sql, va_list ap)
{
	int ret;
	char *query = sqlite3_vmprintf(sql, ap);
	char *err;

	if (query == NULL)
		return -1;

	ret = sqlite3_exec(handle, query, NULL, NULL, &err);

	if (ret)
		bc_log("(SQL ERROR): '%s' => '%s'", query, err);

	sqlite3_free(query);
	sqlite3_free(err);

	return ret;
}

static int bc_db_sqlite_get_table(void *handle, int *nrows, int *ncols,
				  char ***res, const char *fmt, va_list ap)
{
	int ret;
	char *err = NULL;
	char *query = sqlite3_vmprintf(fmt, ap);

	if (query == NULL)
		return -1;

	ret = sqlite3_get_table(handle, query, res, nrows, ncols, &err);

	if (ret)
		bc_log("(SQL ERROR): '%s' => '%s'", query, err);

	sqlite3_free(query);
	sqlite3_free(err);

	return ret;
}

static void bc_db_sqlite_free_table(void *handle, char **res)
{
	sqlite3_free_table(res);
}

static unsigned long bc_db_sqlite_last_insert_rowid(void *handle)
{
	return (unsigned long)sqlite3_last_insert_rowid(handle);
}

struct bc_db_ops bc_db_sqlite = {
	.type		= BC_DB_SQLITE,
	.open		= bc_db_sqlite_open,
	.close		= bc_db_sqlite_close,
	.get_table	= bc_db_sqlite_get_table,
	.free_table	= bc_db_sqlite_free_table,
	.query		= bc_db_sqlite_query,
	.last_insert_rowid = bc_db_sqlite_last_insert_rowid,
};
