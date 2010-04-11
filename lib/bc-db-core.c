/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <errno.h>

#include <libbluecherry.h>

extern struct bc_db_ops bc_db_sqlite;

void bc_db_close(struct bc_handle *bc)
{
	if (bc->dbh == NULL)
		return;

	if (bc->db_ops)
		bc->db_ops->close(bc->dbh);

	bc->dbh = NULL;
}

int bc_db_open(struct bc_handle *bc, enum bc_db_type type)
{
	/* Maybe check this first? */
	bc_db_close(bc);

	switch (type) {
	case BC_DB_SQLITE:
		bc->db_type = BC_DB_SQLITE;
		bc->db_ops = &bc_db_sqlite;
		bc->dbh = bc->db_ops->open();
		break;
	default:
		return EINVAL;
	}

	return bc->dbh == NULL ? -1 : 0;
}
