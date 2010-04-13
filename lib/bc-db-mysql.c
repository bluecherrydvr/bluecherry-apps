/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <libbluecherry.h>

#include <mysql/mysql.h>

static void *bc_db_mysql_open(struct config_t *cfg)
{
	const char *dbname;
	MYSQL *mysql;

        if (!config_lookup_string(cfg, BC_CONFIG_DB ".dbname", &dbname))
                return NULL;

	if ((mysql = mysql_init(NULL)) == NULL)
		return NULL;

	if (mysql_real_connect(mysql, NULL, NULL, NULL, dbname,
			       0, NULL, 0) == NULL) {
		mysql_close(mysql);
		return NULL;
	}

	return mysql;
}

static void bc_db_mysql_close(void *handle)
{
	mysql_close(handle);
}

static int bc_db_mysql_get_table(void *handle, int *nrows, int *ncols,
				 char ***res, const char *fmt, va_list ap)
{
	return -1;
}

static void bc_db_mysql_free_table(void *handle, char **res)
{
	return;
}

struct bc_db_ops bc_db_mysql = {
	.type		= BC_DB_SQLITE,
	.open		= bc_db_mysql_open,
	.close		= bc_db_mysql_close,
	.get_table	= bc_db_mysql_get_table,
	.free_table	= bc_db_mysql_free_table,
};
