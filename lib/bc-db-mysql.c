/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <string.h>

#include <bc-db.h>

#include <mysql/mysql.h>

struct bc_db_mysql_res {
	MYSQL_RES *res;
	MYSQL_ROW row;
	int ncols;
};

static const char *dbname, *dbuser, *dbpass;
static const char *dbhost = NULL;
static const char *dbsocket = NULL;
static long dbport = 0;

static MYSQL *get_handle(void)
{
	static MYSQL *my_con = NULL;

	if (my_con != NULL) {
		/* On successful ping, just re-use connection */
		if (!mysql_ping(my_con))
			return my_con;

		/* Failure, so setup to retry */
		mysql_close(my_con);
		my_con = NULL;
	}

	if ((my_con = mysql_init(NULL)) == NULL) {
		bc_log("(SQL ERROR): Initializing MySQL");
		return NULL;
	}

	if (mysql_real_connect(my_con, dbhost, dbuser, dbpass, dbname,
			       (int)dbport, dbsocket, 0) == NULL) {
		bc_log("(SQL ERROR): Connecting to MySQL database: %s",
		       mysql_error(my_con));
		mysql_close(my_con);
		my_con = NULL;
                return NULL;
        }

	return my_con;
}

static int bc_db_mysql_open(struct config_t *cfg)
{
        if (!config_lookup_string(cfg, BC_CONFIG_DB ".dbname", &dbname))
		return -1;
	if (!config_lookup_string(cfg, BC_CONFIG_DB ".user", &dbuser))
		return -1;
	if (!config_lookup_string(cfg, BC_CONFIG_DB ".password", &dbpass))
		return -1;

	if (config_lookup_string(cfg, BC_CONFIG_DB ".host", &dbhost))
		config_lookup_int(cfg, BC_CONFIG_DB ".port", &dbport);
	else
		config_lookup_string(cfg, BC_CONFIG_DB ".socket", &dbsocket);

	if (get_handle() == NULL)
		return -1;

	return 0;
}

static void bc_db_mysql_close(void)
{
	MYSQL *my_con = get_handle();

	if (my_con != NULL)
		mysql_close(my_con);
}

static int bc_db_mysql_query(char *query)
{
	MYSQL *my_con = get_handle();
	int ret;

	if (my_con == NULL)
		return -1;

	ret = mysql_query(my_con, query);

	if (ret != 0)
		bc_log("(SQL ERROR): [%s] => %s", query, mysql_error(my_con));
		
	return ret;
}

static BC_DB_RES bc_db_mysql_get_table(char *query)
{
	MYSQL *my_con = get_handle();
	struct bc_db_mysql_res *dbres;
	int ret;

	if (my_con == NULL)
		return NULL;

	dbres = malloc(sizeof(*dbres));

	if (dbres == NULL)
		return NULL;

	ret = mysql_query(my_con, query);

	if (ret != 0) {
		free(dbres);
		bc_log("(SQL ERROR): [%s] => %s", query, mysql_error(my_con));
		return NULL;
	}

	dbres->res = mysql_store_result(my_con);
	if (dbres->res == NULL) {
		free(dbres);
		bc_log("(SQL ERROR): No result: [%s]", query);
		return NULL;
	}

	dbres->ncols = mysql_num_fields(dbres->res);

	return dbres;
}

static void bc_db_mysql_free_table(BC_DB_RES __dbres)
{
	struct bc_db_mysql_res *dbres = __dbres;

	mysql_free_result(dbres->res);
}

static int bc_db_mysql_fetch_row(BC_DB_RES __dbres)
{
	struct bc_db_mysql_res *dbres = __dbres;

	dbres->row = mysql_fetch_row(dbres->res);
	if (dbres->row == NULL) {
		mysql_data_seek(dbres->res, 0);
		return -1;
	}

	return 0;
}

static const char *bc_db_mysql_get_val(BC_DB_RES __dbres,
				       const char *colname)
{
	struct bc_db_mysql_res *dbres = __dbres;
	int i;

	if (dbres->row == NULL)
		return NULL;

	for (i = 0; i < dbres->ncols; i++) {
		MYSQL_FIELD *mf = mysql_fetch_field_direct(dbres->res, i);
		if (mf == NULL)
			continue;
		if (strcmp(colname, mf->name) == 0)
			break;
	}

	return (i == dbres->ncols) ? NULL : dbres->row[i];
}

static int bc_db_mysql_num_fields(BC_DB_RES __dbres)
{
	struct bc_db_mysql_res *dbres = __dbres;

	return dbres->ncols;
}

static const char *bc_db_mysql_get_field(BC_DB_RES __dbres,
					 int nfield)
{
	struct bc_db_mysql_res *dbres = __dbres;
	MYSQL_FIELD *fields = mysql_fetch_fields(dbres->res);

	if (!fields || nfield < 0 || nfield >= dbres->ncols)
		return NULL;

	return fields[nfield].name;
}

static unsigned long bc_db_mysql_last_insert_rowid(void)
{
	MYSQL *my_con = get_handle();

	if (my_con == NULL)
		return 0;

	return mysql_insert_id(my_con);
}

static void bc_db_mysql_escape_string(char *to, const char *from)
{
	MYSQL *my_con = get_handle();

	if (my_con == NULL)
		mysql_escape_string(to, from, strlen(from));
	else
		mysql_real_escape_string(my_con, to, from, strlen(from));
}

static void bc_db_mysql_lock(const char *table)
{
	bc_db_query("LOCK TABLES %s WRITE", table);
}

static void bc_db_mysql_unlock(const char *table)
{
	bc_db_query("UNLOCK TABLES");
}

struct bc_db_ops bc_db_mysql = {
	.open		= bc_db_mysql_open,
	.close		= bc_db_mysql_close,
	.get_table	= bc_db_mysql_get_table,
	.free_table	= bc_db_mysql_free_table,
	.fetch_row	= bc_db_mysql_fetch_row,
	.get_val	= bc_db_mysql_get_val,
	.query		= bc_db_mysql_query,
	.num_fields	= bc_db_mysql_num_fields,
	.get_field	= bc_db_mysql_get_field,
	.last_insert_rowid = bc_db_mysql_last_insert_rowid,
	.escape_string	= bc_db_mysql_escape_string,
	.lock		= bc_db_mysql_lock,
	.unlock		= bc_db_mysql_unlock,
};
