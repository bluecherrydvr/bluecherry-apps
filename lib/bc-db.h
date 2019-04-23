/*
 * Copyright 2010-2019 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
