/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include <bc-db.h>

enum bc_db_type {
	BC_DB_SQLITE = 0,
	BC_DB_PSQL = 1,
	BC_DB_MYSQL = 2,
};

static pthread_mutex_t db_lock = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;

static struct bc_db_ops *db_ops = NULL;

void bc_db_lock(void)
{
	if (pthread_mutex_lock(&db_lock) == EDEADLK)
		bc_log("E: Deadlock detected obtaining db_lock");
}

void bc_db_unlock(void)
{
	if (pthread_mutex_unlock(&db_lock) == EPERM)
		bc_log("E: Unlocking db_lock when not held by this thread");
}

int bc_db_start_trans(void)
{
	int ret = 0;

	bc_db_lock();

	if (db_ops->start_trans)
		ret = db_ops->start_trans();

	if (ret)
		bc_db_unlock();

	return ret;
}

void bc_db_commit_trans(void)
{
	if (db_ops->commit_trans)
		db_ops->commit_trans();
	bc_db_unlock();
}

void bc_db_rollback_trans(void)
{
	if (db_ops->rollback_trans)
		db_ops->rollback_trans();
	bc_db_unlock();
}

void bc_db_close(void)
{
	if (db_ops == NULL)
		return;

	db_ops->close();
	db_ops = NULL;
}

int bc_db_open(void)
{
	struct config_t cfg;
	long type;
	int ret = 0;

	if (db_ops != NULL)
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
	case BC_DB_MYSQL:
		db_ops = &bc_db_mysql;
		break;
	default:
		bc_log("E(%s): DB type %ld is not supported", BC_CONFIG, type);
	}

	if (db_ops)
		ret = db_ops->open(&cfg);

db_error:
	if (ret)
		db_ops = NULL;

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

	ret = db_ops->query(query);
	free(query);

	return ret;
}

const char *bc_db_get_field(BC_DB_RES dbres, int nfield)
{
	return db_ops->get_field(dbres, nfield);
}

int bc_db_num_fields(BC_DB_RES dbres)
{
	return db_ops->num_fields(dbres);
}

int bc_db_fetch_row(BC_DB_RES dbres)
{
	return db_ops->fetch_row(dbres);
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

	dbres = db_ops->get_table(query);
	free(query);

	return dbres;
}

void bc_db_free_table(BC_DB_RES dbres)
{
	if (dbres)
		db_ops->free_table(dbres);
}

const char *bc_db_get_val(BC_DB_RES dbres, const char *colname)
{
	return db_ops->get_val(dbres, colname);
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
	return db_ops->last_insert_rowid();
}

char *bc_db_escape_string(const char *from)
{
	char *to = malloc((strlen(from) * 2) + 1);

	if (to == NULL)
		return NULL;

	db_ops->escape_string(to, from);

	return to;
}
