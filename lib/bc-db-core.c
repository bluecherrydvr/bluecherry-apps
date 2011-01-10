/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include <libbluecherry.h>

extern struct bc_db_ops bc_db_sqlite;
extern struct bc_db_ops bc_db_mysql;

struct bc_db_handle bcdb = {
	.db_type	= BC_DB_SQLITE,
	.dbh		= NULL,
	.db_ops		= NULL,
};

void bc_db_lock(void)
{
	bcdb.db_ops->lock(bcdb.dbh);
}

void bc_db_unlock(void)
{
	bcdb.db_ops->unlock(bcdb.dbh);
}

void bc_db_close(void)
{
	if (bcdb.db_ops)
		bcdb.db_ops->close(bcdb.dbh);
	bcdb.dbh = NULL;
	bcdb.db_ops = NULL;
}

int bc_db_open(void)
{
	struct config_t cfg;
	long type;
	int ret = 0;

	if (bcdb.dbh != NULL)
		return 0;

	config_init(&cfg);
	if (!config_read_file(&cfg, BC_CONFIG)) {
		bc_log("E(%s): Error parsing config at line %d", BC_CONFIG,
		       config_error_line(&cfg));
		goto db_error;
	}

	if (!config_lookup_int(&cfg, BC_CONFIG_DB ".type", &type)) {
		bc_log("E(%s): Could not get db type", BC_CONFIG);
		goto db_error;
	}

	switch (type) {
	case BC_DB_SQLITE:
		bcdb.db_ops = &bc_db_sqlite;
		break;
	case BC_DB_MYSQL:
		bcdb.db_ops = &bc_db_mysql;
		break;
	default:
		bc_log("E(%s): DB type %ld is not supported", BC_CONFIG, type);
	}

	bcdb.db_type = type;
	if (bcdb.db_ops)
		bcdb.dbh = bcdb.db_ops->open(&cfg);

db_error:
	if (!bcdb.dbh) {
		bcdb.db_ops = NULL;
		ret = -1;
	}

	config_destroy(&cfg);

	return ret;
}

int bc_db_query(const char *sql, ...)
{
	va_list ap;
	char *query;
	int ret;

	va_start(ap, sql);
	if (vasprintf(&query, sql, ap) < 0)
		return -1;
	va_end(ap);

	ret = bcdb.db_ops->query(bcdb.dbh, query);
	free(query);

	return ret;
}

const char *bc_db_get_field(BC_DB_RES dbres, int nfield)
{
	return bcdb.db_ops->get_field(bcdb.dbh, dbres, nfield);
}

int bc_db_num_fields(BC_DB_RES dbres)
{
	return bcdb.db_ops->num_fields(bcdb.dbh, dbres);
}

int bc_db_fetch_row(BC_DB_RES dbres)
{
	return bcdb.db_ops->fetch_row(bcdb.dbh, dbres);
}

BC_DB_RES bc_db_get_table(const char *sql, ...)
{
	va_list ap;
	char *query;
	BC_DB_RES dbres;

	va_start(ap, sql);
	if (vasprintf(&query, sql, ap) < 0)
		return NULL;
	va_end(ap);

	dbres = bcdb.db_ops->get_table(bcdb.dbh, query);
	free(query);

	return dbres;
}

void bc_db_free_table(BC_DB_RES dbres)
{
	if (dbres)
		bcdb.db_ops->free_table(bcdb.dbh, dbres);
}

const char *bc_db_get_val(BC_DB_RES dbres, const char *colname)
{
	return bcdb.db_ops->get_val(bcdb.dbh, dbres, colname);
}

int bc_db_get_val_int(BC_DB_RES dbres, const char *colname)
{
	const char *val = bc_db_get_val(dbres, colname);

	return val ? atoi(val) : -1;
}

int bc_db_get_val_bool(BC_DB_RES dbres, const char *colname)
{
	const char *val = bc_db_get_val(dbres, colname);

	return val ? (atoi(val) ? 1 : 0) : 0;
}

long unsigned bc_db_last_insert_rowid(void)
{
	return bcdb.db_ops->last_insert_rowid(bcdb.dbh);
}

char *bc_db_escape_string(const char *str)
{
	int len = strlen(str);
	char *p = malloc((len * 2) + 1);
	int i, t;

	if (p == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	/* <= so we include the terminating null */
	for (i = t = 0; i <= len; i++, t++) {
		if (str[i] == '\'' || str[i] == '\\')
			p[t++] = '\\';
		p[t] = str[i];
	}

	return p;
}
