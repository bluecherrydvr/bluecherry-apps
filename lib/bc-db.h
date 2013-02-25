/*
 * Copyright (C) 2011 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __BC_DB_H
#define __BC_DB_H

#include "libbluecherry.h"

struct bc_db_ops {
	int (*open)(struct config_t *cfg);
	void (*close)(void);
	BC_DB_RES (*get_table)(char *query);
	void (*free_table)(BC_DB_RES dbres);
	int (*fetch_row)(BC_DB_RES dbres);
	const char *(*get_val)(BC_DB_RES dbres, const char *field,
			       size_t *length);
	const char *(*get_field)(BC_DB_RES dbres, int nfield);
	int (*num_fields)(BC_DB_RES dbres);
	int (*query)(const char *query);
	unsigned long (*last_insert_rowid)(void);
	void (*escape_string)(char *to, const char *from, size_t len);
	int (*start_trans)(void);
	int (*commit_trans)(void);
	int (*rollback_trans)(void);
};

extern struct bc_db_ops bc_db_mysql;

#endif /* __BC_DB_H */
