/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <string.h>

#include "bc-db.h"

#include <mysql/mysql.h>
#include <mysql/errmsg.h>

struct bc_db_mysql_res {
	MYSQL_RES *res;
	MYSQL_ROW row;
	int ncols;
};


static char *dbname, *dbuser, *dbpass, *dbhost, *dbsock;
static int dbport = 0;
static MYSQL *my_con_global;

static inline void free_null(char **p)
{
	free(*p);
	*p = NULL;
}


#define GET_VAL(__res, __var, __ret) do {				\
	const char *val;						\
	free_null(&__res);						\
	if (!config_lookup_string(cfg, BC_CONFIG_DB "." __var, &val)) {	\
		__res = NULL;						\
		if (__ret) {						\
			bc_db_mysql_close();				\
                	return -1;					\
		}							\
	} else								\
		__res = strdup(val);					\
} while(0)

static MYSQL *get_handle(void)
{
	if (my_con_global != NULL) {
		/* On successful ping, just re-use connection */
		if (!mysql_ping(my_con_global))
			return my_con_global;

		/* Failure, so setup to retry */
		mysql_close(my_con_global);
		my_con_global = NULL;
	}

	if (!dbname || !dbuser || !dbpass)
		return NULL;

	if ((my_con_global = mysql_init(NULL)) == NULL) {
		bc_log(Fatal, "MySQL initialization failed");
		return NULL;
	}

	if (mysql_real_connect(my_con_global, dbhost, dbuser, dbpass, dbname,
			       dbport, dbsock, 0) == NULL) {
		mysql_close(my_con_global);
		my_con_global = NULL;
        	return NULL;
    	}

	return my_con_global;
}

static void bc_db_mysql_close(void)
{
	free_null(&dbname);
	free_null(&dbuser);
	free_null(&dbpass);
	free_null(&dbhost);
	free_null(&dbsock);
	dbport = 0;

	if (my_con_global) {
		mysql_close(my_con_global);
		my_con_global = NULL;
	}
}

static int bc_db_mysql_open(struct config_t *cfg)
{
	if (get_handle() != NULL)
		return 0;

	GET_VAL(dbname, "dbname", 1);
	GET_VAL(dbuser, "user", 1);
	GET_VAL(dbpass, "password", 1);

	GET_VAL(dbhost, "host", 0);
	if (dbhost)
		config_lookup_int(cfg, BC_CONFIG_DB ".port", &dbport);
	else
		GET_VAL(dbsock, "socket", 0);

	if (get_handle() == NULL) {
		bc_db_mysql_close();
		return -1;
	}

	return 0;
}

static MYSQL *reset_con(void)
{
	// close the con by force and re-connect to DBMS  
	if (my_con_global != NULL) {
		mysql_close(my_con_global);
	}

	if (!dbname || !dbuser || !dbpass)
		return NULL;

	if ((my_con_global = mysql_init(NULL)) == NULL) {
		bc_log(Fatal, "MySQL initialization failed");
		return NULL;
	}

	if (mysql_real_connect(my_con_global, dbhost, dbuser, dbpass, dbname,
			       dbport, dbsock, 0) == NULL) {
		mysql_close(my_con_global);
		my_con_global = NULL;
        	return NULL;
    	}

	return my_con_global;
}

static int bc_db_mysql_query(const char *query)
{
	MYSQL *my_con = get_handle();
	int ret;

	if (my_con == NULL)
		return -1;

	ret = mysql_query(my_con, query);

	if (ret != 0)
	{
		/* if error_no is in CR_CONN_HOST_ERROR, CR_SERVER_GONE_ERROR and CR_SERVER_LOST, 
		   then reconnect to DBMS */ 
		int errNo = mysql_errno(my_con);
		if (errNo == CR_CONN_HOST_ERROR || errNo == CR_SERVER_GONE_ERROR || errNo == CR_SERVER_LOST)
		{
			/* reconnect to DBMS */
			my_con = reset_con();
			if (my_con == NULL)
			{
				bc_log(Error, "Query error: [%s] => %s", query, mysql_error(my_con));
				return -1;
			}

			/* excute query again */
			ret = mysql_query(my_con, query);
			if (ret != 0)
			{
				bc_log(Error, "Query error: [%s] => %s", query, mysql_error(my_con));
			}
		}
		else
		{
			bc_log(Error, "Query error: [%s] => %s", query, mysql_error(my_con));
		}		
	}

	return ret;
}

static BC_DB_RES bc_db_mysql_get_table(char *query)
{
	MYSQL *my_con = get_handle();
	struct bc_db_mysql_res *dbres;
	int ret;

	if (my_con == NULL)
		return NULL;

	dbres = (struct bc_db_mysql_res*) malloc(sizeof(*dbres));

	if (dbres == NULL)
		return NULL;

	ret = mysql_query(my_con, query);

	if (ret != 0) {

		/* if error_no is in the CR_CONN_HOST_ERROR, CR_SERVER_GONE_ERROR and CR_SERVER_LOST, 
		   then reconnect to DBMS */ 
		int errNo = mysql_errno(my_con);
		if (errNo == CR_CONN_HOST_ERROR || errNo == CR_SERVER_GONE_ERROR || errNo == CR_SERVER_LOST)
		{
			/* reconnect to DBMS */
			my_con = reset_con();
			if (my_con == NULL)
			{
				free(dbres);
				bc_log(Error, "Query error: [%s] => %s", query, mysql_error(my_con));
				return NULL;
			}
			
			/* excute query again */
			ret = mysql_query(my_con, query);
			if (ret != 0)
			{
				free(dbres);
				bc_log(Error, "Query error: [%s] => %s", query, mysql_error(my_con));
				return NULL;
			}
		}
		else
		{
			free(dbres);
			bc_log(Error, "Query error: [%s] => %s", query, mysql_error(my_con));
			return NULL;
		}	
	}

	dbres->res = mysql_store_result(my_con);
	if (dbres->res == NULL) {
		free(dbres);
		bc_log(Error, "Query has no result: [%s]", query);
		return NULL;
	}

	dbres->ncols = mysql_num_fields(dbres->res);

	return dbres;
}

static void bc_db_mysql_free_table(BC_DB_RES __dbres)
{
	struct bc_db_mysql_res *dbres = (struct bc_db_mysql_res*) __dbres;

	mysql_free_result(dbres->res);
	free(dbres);
}

static int bc_db_mysql_fetch_row(BC_DB_RES __dbres)
{
	struct bc_db_mysql_res *dbres = (struct bc_db_mysql_res*) __dbres;

	dbres->row = mysql_fetch_row(dbres->res);
	if (dbres->row == NULL) {
		mysql_data_seek(dbres->res, 0);
		return -1;
	}

	return 0;
}

static const char *bc_db_mysql_get_val(BC_DB_RES __dbres,
				       const char *colname,
				       size_t *length)
{
	struct bc_db_mysql_res *dbres = (struct bc_db_mysql_res*) __dbres;
	if (dbres->row == NULL)
		return NULL;

	unsigned long *lengths;
	if (length)
		lengths = mysql_fetch_lengths(dbres->res);

	for (int i = 0; i < dbres->ncols; i++) {
		MYSQL_FIELD *mf = mysql_fetch_field_direct(dbres->res, i);
		if (mf != NULL && strcmp(colname, mf->name) == 0) {
			if (length)
				*length = lengths[i];
			return dbres->row[i];
		}
	}

	return NULL;
}

static int bc_db_mysql_num_fields(BC_DB_RES __dbres)
{
	struct bc_db_mysql_res *dbres = (struct bc_db_mysql_res*) __dbres;

	return dbres->ncols;
}

static const char *bc_db_mysql_get_field(BC_DB_RES __dbres,
					 int nfield)
{
	struct bc_db_mysql_res *dbres = (struct bc_db_mysql_res*) __dbres;
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

static void bc_db_mysql_escape_string(char *to, const char *from, size_t len)
{
	MYSQL *my_con = get_handle();

	if (my_con == NULL)
		mysql_escape_string(to, from, len);
	else
		mysql_real_escape_string(my_con, to, from, len);
}

static int bc_db_mysql_start_trans(void)
{
	return bc_db_mysql_query("START TRANSACTION");
}

static int bc_db_mysql_commit_trans(void)
{
	return bc_db_mysql_query("COMMIT");
}

static int bc_db_mysql_rollback_trans(void)
{
	return bc_db_mysql_query("ROLLBACK");
}

struct bc_db_ops bc_db_mysql = {
	bc_db_mysql_open,
	bc_db_mysql_close,
	bc_db_mysql_get_table,
	bc_db_mysql_free_table,
	bc_db_mysql_fetch_row,
	bc_db_mysql_get_val,
	bc_db_mysql_get_field,
	bc_db_mysql_num_fields,
	bc_db_mysql_query,
	bc_db_mysql_last_insert_rowid,
	bc_db_mysql_escape_string,
	bc_db_mysql_start_trans,
	bc_db_mysql_commit_trans,
	bc_db_mysql_rollback_trans,
};
