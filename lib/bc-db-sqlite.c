/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <libbluecherry.h>

#include <sqlite3.h>

struct bc_db_sqlite_res {
	int	nrows;
	int	ncols;
	char	**rows;
	int	row_idx;
};

static pthread_mutex_t db_lock =
	PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;

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

static int bc_db_sqlite_query(void *handle, char *query)
{
	int ret;
	char *err;

	ret = sqlite3_exec(handle, query, NULL, NULL, &err);

	if (ret)
		bc_log("(SQL ERROR): '%s' => '%s'", query, err);

	sqlite3_free(err);

	return ret;
}

static BC_DB_RES bc_db_sqlite_get_table(void *handle, char *query)
{
	struct bc_db_sqlite_res *dbres = malloc(sizeof(*dbres));
	char *err = NULL;
	int ret;

	if (dbres == NULL)
		return NULL;

	dbres->row_idx = -1;
	ret = sqlite3_get_table(handle, query, &dbres->rows,
				&dbres->nrows, &dbres->ncols, &err);

	if (ret) {
		free(dbres);
		dbres = NULL;
		bc_log("(SQL ERROR): '%s' => '%s'", query, err);
	}

	sqlite3_free(err);

	return dbres;
}

static const char *bc_db_sqlite_get_field(void *handle,
					  BC_DB_RES __dbres,
					  int nfield)
{
	struct bc_db_sqlite_res *dbres = __dbres;

	if (nfield < 0 || nfield >= dbres->ncols)
		return NULL;

	return (const char *)dbres->rows[nfield];
}

static int bc_db_sqlite_num_fields(void *handle, BC_DB_RES __dbres)
{
	struct bc_db_sqlite_res *dbres = __dbres;
	return dbres->ncols;
}

static int bc_db_sqlite_fetch_row(void *handle, BC_DB_RES __dbres)
{
	struct bc_db_sqlite_res *dbres = __dbres;

	dbres->row_idx++;
	if (dbres->row_idx >= dbres->nrows) {
		dbres->row_idx = -1;
		return -1;
	}

	return 0;
}

static const char *bc_db_sqlite_get_val(void *handle, BC_DB_RES __dbres,
					const char *colname)
{
	struct bc_db_sqlite_res *dbres = __dbres;
	int i;

	if (dbres->row_idx < 0)
		return NULL;

	for (i = 0; i < dbres->ncols; i++) {
		if (strcmp(colname, dbres->rows[i]) == 0)
			break;
	}

	return (i == dbres->ncols) ? NULL :
		dbres->rows[((dbres->row_idx + 1) * dbres->ncols) + i];
}

static void bc_db_sqlite_free_table(void *handle, BC_DB_RES __dbres)
{
	struct bc_db_sqlite_res *dbres = __dbres;

	sqlite3_free_table(dbres->rows);
	free(dbres);
}

static unsigned long bc_db_sqlite_last_insert_rowid(void *handle)
{
	return (unsigned long)sqlite3_last_insert_rowid(handle);
}

static void bc_db_sqlite_lock(void *handle)
{
	if (pthread_mutex_lock(&db_lock) == EDEADLK)
		bc_log("E: Deadlock detected in db_lock!");
}

static void bc_db_sqlite_unlock(void *handle)
{
	if (pthread_mutex_unlock(&db_lock) == EPERM)
		bc_log("E: Unlocking db_lock owned by another thread!");
}

struct bc_db_ops bc_db_sqlite = {
	.type		= BC_DB_SQLITE,
	.open		= bc_db_sqlite_open,
	.close		= bc_db_sqlite_close,
	.get_table	= bc_db_sqlite_get_table,
	.free_table	= bc_db_sqlite_free_table,
	.fetch_row	= bc_db_sqlite_fetch_row,
	.get_val	= bc_db_sqlite_get_val,
	.query		= bc_db_sqlite_query,
	.num_fields	= bc_db_sqlite_num_fields,
	.get_field	= bc_db_sqlite_get_field,
	.last_insert_rowid = bc_db_sqlite_last_insert_rowid,
	.lock		= bc_db_sqlite_lock,
	.unlock		= bc_db_sqlite_unlock,
};
