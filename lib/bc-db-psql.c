/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <libbluecherry.h>

#include <postgresql/libpq-fe.h>

static void *bc_db_psql_open(struct config_t *cfg)
{
	const char *dbname;
	char buf[256];

	if (!config_lookup_string(cfg, BC_CONFIG_DB ".dbname", &dbname))
		return NULL;

	snprintf(buf, sizeof(buf), "dbname='%s'", dbname);

	return PQconnectdb(buf);
}

static void bc_db_psql_close(void *handle)
{
	PQfinish(handle);
}

static int bc_db_psql_query(void *handle, const char *sql, va_list ap)
{
	return -1;
}

static int bc_db_psql_get_table(void *handle, int *nrows, int *ncols,
				  char ***res, const char *fmt, va_list ap)
{
	return -1;
}

static void bc_db_psql_free_table(void *handle, char **res)
{
	return;
}

struct bc_db_ops bc_db_psql = {
	.type		= BC_DB_SQLITE,
	.open		= bc_db_psql_open,
	.close		= bc_db_psql_close,
	.get_table	= bc_db_psql_get_table,
	.free_table	= bc_db_psql_free_table,
	.query		= bc_db_psql_query,
};
