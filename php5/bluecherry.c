/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/ioctl.h>

#include "libbluecherry.h"
#include "php.h"

#define BCH_NAME	"BC Handle"
#define BCDB_NAME	"BC-DB Handle"

static int bch_id;
static int bcdb_id;

static void bch_destructor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	struct bc_handle *bch = (struct bc_handle *)rsrc->ptr;

	if (bch)
		bc_handle_free(bch);

	return;
}

static void bcdb_destructor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	struct bc_db_handle *bcdb;

	if (bcdb)
		bc_db_close(bcdb);

	return;
}

PHP_MINIT_FUNCTION(bluecherry)
{
	bch_id = zend_register_list_destructors_ex(bch_destructor, NULL,
						   BCH_NAME, module_number);
	bcdb_id = zend_register_list_destructors_ex(bcdb_destructor, NULL,
						    BCDB_NAME, module_number);

	REGISTER_LONG_CONSTANT("BC_CID_HUE", V4L2_CID_HUE, 0);
	REGISTER_LONG_CONSTANT("BC_CID_CONTRAST", V4L2_CID_CONTRAST, 0);
	REGISTER_LONG_CONSTANT("BC_CID_BRIGHTNESS", V4L2_CID_BRIGHTNESS, 0);
	REGISTER_LONG_CONSTANT("BC_CID_SATURATION", V4L2_CID_SATURATION, 0);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(bluecherry)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(bluecherry)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Bluecherry library support", "enabled");
	php_info_print_table_end();

	return;
}

#define BCH_GET_RES(__func) do {					\
	zval *z_ctx;							\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",	\
				  &z_ctx) == FAILURE)			\
		RETURN_FALSE;						\
	ZEND_FETCH_RESOURCE(bch, struct bc_handle *, &z_ctx, -1,	\
			    BCH_NAME, bch_id);				\
	if (bch == NULL) {						\
		php_error_docref(NULL TSRMLS_CC, E_WARNING,		\
			__func ": invalid context");			\
		RETURN_FALSE;						\
	}								\
} while(0)

#define BCDB_GET_RES(__func) do {					\
	zval *z_ctx;							\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",	\
				  &z_ctx) == FAILURE)			\
		RETURN_FALSE;						\
	ZEND_FETCH_RESOURCE(bcdb, struct bc_db_handle *, &z_ctx, -1,	\
			    BCDB_NAME, bcdb_id);			\
	if (bcdb == NULL) {						\
		php_error_docref(NULL TSRMLS_CC, E_WARNING,		\
			__func ": invalid context");			\
		RETURN_FALSE;						\
	}								\
} while(0)

PHP_FUNCTION(bc_db_open)
{
	struct bc_db_handle *bcdb;

	if ((bcdb = bc_db_open()) == NULL)
		RETURN_FALSE;

	ZEND_REGISTER_RESOURCE(return_value, bcdb, bcdb_id);
}

PHP_FUNCTION(bc_db_close)
{
	struct bc_db_handle *bcdb;

	BCDB_GET_RES("bc_db_close");

	zend_list_delete((long)bcdb);

	RETURN_TRUE;
}

PHP_FUNCTION(bc_db_get_table)
{
	struct bc_db_handle *bcdb;
	zval *z_ctx;
	int nrows, ncols;
	char *sql;
	int sql_len;
	char **rows;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ctx,
				  &sql, &sql_len) == FAILURE)
		RETURN_FALSE;
	ZEND_FETCH_RESOURCE(bcdb, struct bc_db_handle *, &z_ctx, -1,
			    BCDB_NAME, bcdb_id);
	if (bcdb == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
				 "bc_db_get_table: invalid context");
		RETURN_FALSE;
	}

	i = bc_db_get_table(bcdb, &nrows, &ncols, &rows, sql);
	if (i || !nrows)
		RETURN_FALSE;

	array_init(return_value);

	for (i = 0; i < nrows; i++) {
		zval *row_arr;
		int t;

		ALLOC_INIT_ZVAL(row_arr);
		array_init(row_arr);

		for (t = 0; t < ncols; t++) {
			char *str = rows[((i + 1) * ncols) + t];
			if (str == NULL)
				continue;
			add_assoc_string(row_arr, rows[t], str, 1);
		}

		add_next_index_zval(return_value, row_arr);
	}

	bc_db_free_table(bcdb, rows);
}

PHP_FUNCTION(bc_handle_get)
{
	struct bc_handle *bch;
	char *devname;
	int devname_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &devname,
				  &devname_len) == FAILURE)
		RETURN_FALSE;

	if ((bch = bc_handle_get(devname)) == NULL)
		RETURN_FALSE;

	ZEND_REGISTER_RESOURCE(return_value, bch, bch_id);
}

PHP_FUNCTION(bc_set_control)
{
	zval *z_ctx;
	struct bc_handle *bch;
	long ctrl, val;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &z_ctx,
				  &ctrl, &val) == FAILURE)
		RETURN_FALSE;
	ZEND_FETCH_RESOURCE(bch, struct bc_handle *, &z_ctx, -1,
			    BCH_NAME, bch_id);
	if (bch == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
				 "bc_set_control: invalid context");
		RETURN_FALSE;
	}

	if (bc_set_control(bch, ctrl, val))
		RETURN_FALSE;

	RETURN_TRUE;
}

PHP_FUNCTION(bc_handle_free)
{
	struct bc_handle *bch;

	BCH_GET_RES("bc_handle_free");

	zend_list_delete((long)bch);

	RETURN_TRUE;
}

PHP_FUNCTION(bc_handle_start)
{
	struct bc_handle *bch;

	BCH_GET_RES("bc_handle_start");

	if (bc_handle_start(bch) < 0)
		RETURN_FALSE;

	RETURN_TRUE;
}

PHP_FUNCTION(bc_buf_get)
{
	struct bc_handle *bch;

	BCH_GET_RES("bc_buf_get");

	bc_buf_get(bch);

	RETURN_TRUE;
}

PHP_FUNCTION(bc_buf_size)
{
	struct bc_handle *bch;

	BCH_GET_RES("bc_buf_size");

	RETURN_LONG(bc_buf_size(bch));
}

PHP_FUNCTION(bc_buf_data)
{
	struct bc_handle *bch;
        
	BCH_GET_RES("bc_buf_data");

	RETURN_STRINGL(bc_buf_data(bch), bc_buf_size(bch), 1);
}

PHP_FUNCTION(bc_set_mjpeg)
{
	struct bc_handle *bch;

	BCH_GET_RES("bc_set_mjpeg");

	bch->vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	if (ioctl(bch->dev_fd, VIDIOC_S_FMT, &bch->vfmt) < 0)
		RETURN_FALSE;

	RETURN_TRUE;
}

static function_entry bluecherry_functions[] = {
	/* Bluecherry DB Handlers */
	PHP_FE(bc_db_open, NULL)
	PHP_FE(bc_db_close, NULL)
	PHP_FE(bc_db_get_table, NULL)
	/* Bluecherry Video Handlers */
	PHP_FE(bc_handle_get, NULL)
	PHP_FE(bc_handle_free, NULL)
	PHP_FE(bc_handle_start, NULL)
	PHP_FE(bc_buf_get, NULL)
	PHP_FE(bc_buf_size, NULL)
	PHP_FE(bc_buf_data, NULL)
	PHP_FE(bc_set_mjpeg, NULL)
	PHP_FE(bc_set_control, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry bluecherry_module_entry = {
	STANDARD_MODULE_HEADER,
	"bluecherry",
	bluecherry_functions,
	PHP_MINIT(bluecherry),
	PHP_MSHUTDOWN(bluecherry),
	NULL,
	NULL,
	PHP_MINFO(bluecherry),
	"1.0",
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_BLUECHERRY
ZEND_GET_MODULE(bluecherry)
#endif
