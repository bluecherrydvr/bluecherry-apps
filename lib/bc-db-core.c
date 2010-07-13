/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <libbluecherry.h>

extern struct bc_db_ops bc_db_sqlite;
extern struct bc_db_ops bc_db_psql;
extern struct bc_db_ops bc_db_mysql;

void bc_db_close(struct bc_db_handle *bc_db)
{
	if (bc_db->db_ops)
		bc_db->db_ops->close(bc_db->dbh);

	bc_db->dbh = NULL;
}

struct bc_db_handle *bc_db_open(void)
{
	struct bc_db_handle *bc_db = malloc(sizeof(*bc_db));
	struct config_t cfg;
	long type;

	if (bc_db == NULL)
		return NULL;
	memset(bc_db, 0, sizeof(*bc_db));

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
		bc_db->db_type = BC_DB_SQLITE;
		bc_db->db_ops = &bc_db_sqlite;
		bc_db->dbh = bc_db->db_ops->open(&cfg);
		break;
	case BC_DB_PSQL:
	case BC_DB_MYSQL:
	default:
		bc_log("E(%s): Invalid db type %ld", BC_CONFIG, type);
	}

db_error:
	if (bc_db && !bc_db->dbh) {
		free(bc_db);
		bc_db = NULL;
	}

	config_destroy(&cfg);

	return bc_db;
}

int bc_db_query(struct bc_db_handle *bc_db, const char *sql, ...)
{
	va_list ap;
	int ret;

	va_start(ap, sql);
	ret = bc_db->db_ops->query(bc_db->dbh, sql, ap);
	va_end(ap);

	return ret;
}

int bc_db_get_table(struct bc_db_handle *bc_db, int *nrows, int *ncols,
		    char ***res, const char *fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = bc_db->db_ops->get_table(bc_db->dbh, nrows, ncols, res, fmt, ap);
	va_end(ap);

	return ret;
}

void bc_db_free_table(struct bc_db_handle *bc_db, char **res)
{
	bc_db->db_ops->free_table(bc_db->dbh, res);
}

char *bc_db_get_val(char **rows, int ncols, int row, const char *colname)
{
	int i;

	for (i = 0; i < ncols; i++) {
		if (strcmp(colname, rows[i]) == 0)
			break;
	}

	return (i == ncols) ? NULL : rows[((row + 1) * ncols) + i];
}

int bc_db_get_val_int(char **rows, int ncols, int row, const char *colname)
{
	char *val = bc_db_get_val(rows, ncols, row, colname);

	return val ? atoi(val) : -1;
}

int bc_db_get_val_bool(char **rows, int ncols, int row, const char *colname)
{
	char *val = bc_db_get_val(rows, ncols, row, colname);

	return val ? (atoi(val) ? 1 : 0) : 0;
}
