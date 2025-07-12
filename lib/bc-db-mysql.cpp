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

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "bc-db.h"

#ifdef BC_USE_MARIADB
#include <mariadb/mysql.h>
#include <mariadb/errmsg.h>
#include <mariadb/mysqld_error.h>
#else
#include <mysql/mysql.h>
#include <mysql/errmsg.h>
#include <mysql/mysqld_error.h>
#endif

struct bc_db_mysql_res {
	MYSQL_RES *res;
	MYSQL_ROW row;
	int ncols;
};

// CRITICAL FIX: Connection pool structure
struct mysql_connection {
	MYSQL *con;
	time_t last_used;
	bool in_use;
	pthread_mutex_t lock;
};

#define MAX_CONNECTIONS 10
#define CONNECTION_TIMEOUT 300  // 5 minutes
#define CONNECTION_CHECK_INTERVAL 60  // Check every minute

static char *dbname, *dbuser, *dbpass, *dbhost, *dbsock;
static int dbport = 0;
static MYSQL *my_con_global;  // Keep for backward compatibility
static struct mysql_connection connection_pool[MAX_CONNECTIONS];
static pthread_mutex_t pool_lock = PTHREAD_MUTEX_INITIALIZER;
static time_t last_connection_check = 0;

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

// CRITICAL FIX: Initialize connection pool
static void init_connection_pool(void)
{
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		connection_pool[i].con = NULL;
		connection_pool[i].last_used = 0;
		connection_pool[i].in_use = false;
		pthread_mutex_init(&connection_pool[i].lock, NULL);
	}
}

// CRITICAL FIX: Clean up connection pool
static void cleanup_connection_pool(void)
{
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		if (connection_pool[i].con) {
			mysql_close(connection_pool[i].con);
			connection_pool[i].con = NULL;
		}
		pthread_mutex_destroy(&connection_pool[i].lock);
	}
}

// CRITICAL FIX: Get available connection from pool
static MYSQL *get_pooled_connection(void)
{
	time_t now = time(NULL);
	
	// Check for stale connections periodically
	if (now - last_connection_check > CONNECTION_CHECK_INTERVAL) {
		last_connection_check = now;
		for (int i = 0; i < MAX_CONNECTIONS; i++) {
			if (connection_pool[i].con && !connection_pool[i].in_use) {
				if (now - connection_pool[i].last_used > CONNECTION_TIMEOUT) {
					// Close stale connection
					mysql_close(connection_pool[i].con);
					connection_pool[i].con = NULL;
				}
			}
		}
	}
	
	// Find available connection
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		if (pthread_mutex_trylock(&connection_pool[i].lock) == 0) {
			if (!connection_pool[i].in_use) {
				if (!connection_pool[i].con) {
					// Create new connection
					connection_pool[i].con = mysql_init(NULL);
					if (!connection_pool[i].con) {
						pthread_mutex_unlock(&connection_pool[i].lock);
						continue;
					}
					
					// CRITICAL FIX: Improved timeout settings
					mysql_options(connection_pool[i].con, MYSQL_OPT_READ_TIMEOUT, (const int[]){30});
					mysql_options(connection_pool[i].con, MYSQL_OPT_WRITE_TIMEOUT, (const int[]){30});
					mysql_options(connection_pool[i].con, MYSQL_OPT_CONNECT_TIMEOUT, (const int[]){10});
					mysql_options(connection_pool[i].con, MYSQL_OPT_RETRY_COUNT, (const int[]){3});
					
					MYSQL *ret = mysql_real_connect(connection_pool[i].con, dbhost, dbuser, dbpass,
									dbname, dbport, dbsock, 0);
					if (!ret) {
						mysql_close(connection_pool[i].con);
						connection_pool[i].con = NULL;
						pthread_mutex_unlock(&connection_pool[i].lock);
						continue;
					}
				}
				
				connection_pool[i].in_use = true;
				connection_pool[i].last_used = now;
				return connection_pool[i].con;
			}
			pthread_mutex_unlock(&connection_pool[i].lock);
		}
	}
	
	// No available connection, fall back to global connection
	return my_con_global;
}

// CRITICAL FIX: Release connection back to pool
static void release_pooled_connection(MYSQL *con)
{
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		if (connection_pool[i].con == con) {
			connection_pool[i].in_use = false;
			pthread_mutex_unlock(&connection_pool[i].lock);
			return;
		}
	}
	// If not found in pool, it's the global connection - no need to release
}

static MYSQL *reset_con(void)
{
	if (my_con_global)
		mysql_close(my_con_global);

	if (!dbname || !dbuser || !dbpass)
		return NULL;

	my_con_global = mysql_init(NULL);
	if (!my_con_global) {
		bc_log(Fatal, "MySQL initialization failed");
		return NULL;
	}

	/* NOTE: Retries multiply the specified timeouts */
	// CRITICAL FIX: Improved timeout settings for global connection
	mysql_options(my_con_global, MYSQL_OPT_READ_TIMEOUT, (const int[]){30});
	mysql_options(my_con_global, MYSQL_OPT_WRITE_TIMEOUT, (const int[]){30});
	mysql_options(my_con_global, MYSQL_OPT_CONNECT_TIMEOUT, (const int[]){10});
	mysql_options(my_con_global, MYSQL_OPT_RETRY_COUNT, (const int[]){3});

	MYSQL *ret = mysql_real_connect(my_con_global, dbhost, dbuser, dbpass,
					dbname, dbport, dbsock, 0);
	if (!ret) {
		mysql_close(my_con_global);
		my_con_global = NULL;
		return NULL;
	}

	// CRITICAL FIX: Reset database lock availability when connection is restored
	extern bool db_lock_available;
	db_lock_available = true;

	return my_con_global;
}

static MYSQL *get_handle(void)
{
	// CRITICAL FIX: Try pooled connection first
	MYSQL *pooled_con = get_pooled_connection();
	if (pooled_con) {
		return pooled_con;
	}
	
	// Fall back to global connection
	return my_con_global ? my_con_global : reset_con();
}

static void bc_db_mysql_close(void)
{
	free_null(&dbname);
	free_null(&dbuser);
	free_null(&dbpass);
	free_null(&dbhost);
	free_null(&dbsock);
	dbport = 0;

	// CRITICAL FIX: Clean up connection pool
	cleanup_connection_pool();

	if (my_con_global) {
		mysql_close(my_con_global);
		my_con_global = NULL;
	}
}

static int bc_db_mysql_open(struct config_t *cfg)
{
	if (get_handle() != NULL) {
		// CRITICAL FIX: Initialize connection pool on first open
		init_connection_pool();
		return 0;
	}

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

	// CRITICAL FIX: Initialize connection pool
	init_connection_pool();

	return 0;
}

static bool is_con_lost(MYSQL *con)
{
	int err = mysql_errno(con);
	switch (err) {
	case CR_CONN_HOST_ERROR:
	case CR_SERVER_GONE_ERROR:
	case CR_SERVER_LOST:
	case ER_SERVER_SHUTDOWN:
		return 1;
	default:
		return 0;
	}
}

// CRITICAL FIX: Check if connection is still valid
static bool is_connection_valid(MYSQL *con)
{
	if (!con) return false;
	
	// Try a simple ping to check connection health
	return mysql_ping(con) == 0;
}

static int bc_db_mysql_query(const char *query)
{
	MYSQL *my_con = get_handle();
	int ret;

	if (my_con == NULL)
		return -1;

	// CRITICAL FIX: Check connection health before query
	if (!is_connection_valid(my_con)) {
		bc_log(Warning, "Database connection lost, attempting to reconnect");
		my_con = reset_con();
		if (!my_con) {
			return -1;
		}
	}

	unsigned int retries = 3;
	for (; (ret = mysql_query(my_con, query)) && retries; retries--) {
		if (!is_con_lost(my_con))
			break;

		/* reconnect to DBMS */
		my_con = reset_con();
		if (!my_con)
			break;
	}

	if (ret)
		bc_log(Error, "Query error: [%s] => %s", query,
		       mysql_error(my_con));

	// CRITICAL FIX: Release pooled connection if used
	release_pooled_connection(my_con);

	return ret;
}

static BC_DB_RES bc_db_mysql_get_table(char *query)
{
	struct bc_db_mysql_res *dbres;

	dbres = (struct bc_db_mysql_res *)malloc(sizeof(*dbres));
	if (!dbres)
		goto error;

	if (bc_db_mysql_query(query))
		goto error;

	dbres->res = mysql_store_result(get_handle());
	if (!dbres->res) {
		bc_log(Error, "Query has no result: [%s]", query);
		goto error;
	}

	dbres->ncols = mysql_num_fields(dbres->res);
	return dbres;

error:
	free(dbres);
	return NULL;
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
	MYSQL *my_con = get_handle();
	int ret;

	if (my_con == NULL)
		return -1;

	unsigned int retries = 3;
	for (; retries; retries--) {
		ret = mysql_query(my_con, "START TRANSACTION");
		if (!ret)
			break;

		int err = mysql_errno(my_con);
		if (err == CR_COMMANDS_OUT_OF_SYNC) {
			// CRITICAL FIX: Handle "Commands out of sync" error
			bc_log(Warning, "MySQL commands out of sync, attempting to reset connection");
			
			// Try to reset the connection
			my_con = reset_con();
			if (!my_con) {
				bc_log(Error, "Failed to reset MySQL connection");
				break;
			}
			
			// Small delay before retry
			usleep(100000); // 100ms
			continue;
		} else if (is_con_lost(my_con)) {
			// Handle connection loss
			my_con = reset_con();
			if (!my_con)
				break;
			continue;
		} else {
			// Other error, don't retry
			break;
		}
	}

	if (ret)
		bc_log(Error, "Failed to start transaction after %d retries: %s", 3 - retries + 1, mysql_error(my_con));

	return ret;
}

static int bc_db_mysql_commit_trans(void)
{
	MYSQL *my_con = get_handle();
	int ret;

	if (my_con == NULL)
		return -1;

	unsigned int retries = 3;
	for (; retries; retries--) {
		ret = mysql_query(my_con, "COMMIT");
		if (!ret)
			break;

		int err = mysql_errno(my_con);
		if (err == CR_COMMANDS_OUT_OF_SYNC) {
			// CRITICAL FIX: Handle "Commands out of sync" error
			bc_log(Warning, "MySQL commands out of sync during commit, attempting to reset connection");
			
			// Try to reset the connection
			my_con = reset_con();
			if (!my_con) {
				bc_log(Error, "Failed to reset MySQL connection during commit");
				break;
			}
			
			// Small delay before retry
			usleep(100000); // 100ms
			continue;
		} else if (is_con_lost(my_con)) {
			// Handle connection loss
			my_con = reset_con();
			if (!my_con)
				break;
			continue;
		} else {
			// Other error, don't retry
			break;
		}
	}

	if (ret)
		bc_log(Error, "Failed to commit transaction after %d retries: %s", 3 - retries + 1, mysql_error(my_con));

	return ret;
}

static int bc_db_mysql_rollback_trans(void)
{
	MYSQL *my_con = get_handle();
	int ret;

	if (my_con == NULL)
		return -1;

	unsigned int retries = 3;
	for (; retries; retries--) {
		ret = mysql_query(my_con, "ROLLBACK");
		if (!ret)
			break;

		int err = mysql_errno(my_con);
		if (err == CR_COMMANDS_OUT_OF_SYNC) {
			// CRITICAL FIX: Handle "Commands out of sync" error
			bc_log(Warning, "MySQL commands out of sync during rollback, attempting to reset connection");
			
			// Try to reset the connection
			my_con = reset_con();
			if (!my_con) {
				bc_log(Error, "Failed to reset MySQL connection during rollback");
				break;
			}
			
			// Small delay before retry
			usleep(100000); // 100ms
			continue;
		} else if (is_con_lost(my_con)) {
			// Handle connection loss
			my_con = reset_con();
			if (!my_con)
				break;
			continue;
		} else {
			// Other error, don't retry
			break;
		}
	}

	if (ret)
		bc_log(Error, "Failed to rollback transaction after %d retries: %s", 3 - retries + 1, mysql_error(my_con));

	return ret;
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
