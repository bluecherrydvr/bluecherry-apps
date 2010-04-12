/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <errno.h>

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

struct bc_db_handle *bc_db_open(enum bc_db_type type)
{
	struct bc_db_handle *bc_db = malloc(sizeof(*bc_db));

	if (bc_db == NULL)
		return NULL;

	switch (type) {
	case BC_DB_SQLITE:
		bc_db->db_type = BC_DB_SQLITE;
		bc_db->db_ops = &bc_db_sqlite;
		bc_db->dbh = bc_db->db_ops->open();
		break;
	case BC_DB_PSQL:
	case BC_DB_MYSQL:
	default:
		free(bc_db);
		return NULL;
	}

	return bc_db;
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
