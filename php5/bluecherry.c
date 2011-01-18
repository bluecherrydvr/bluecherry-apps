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

static void bch_destructor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	struct bc_handle *bch = (struct bc_handle *)rsrc->ptr;

	if (bch)
		bc_handle_free(bch);

	return;
}

PHP_MINIT_FUNCTION(bluecherry)
{
	bch_id = zend_register_list_destructors_ex(bch_destructor, NULL,
						   BCH_NAME, module_number);

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

PHP_FUNCTION(bc_db_open)
{
	if (bc_db_open())
		RETURN_FALSE;

	RETURN_TRUE;
}

PHP_FUNCTION(bc_db_close)
{
	bc_db_close();
	RETURN_TRUE;
}

PHP_FUNCTION(bc_db_escape_string)
{
	zval *z_ctx;
	char *str;
	int str_len;
	char *tmp_str;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
				  &str, &str_len) == FAILURE)
		RETURN_FALSE;

	str[str_len] = '\0';
	tmp_str = bc_db_escape_string(str);
	if (tmp_str == NULL)
		RETURN_FALSE;

	/* We're supposed to be able to pass this to PHP and let it
	 * free() it on it's own, but instead we have to let it copy
	 * and free() ourselves, else it crashes in some weird way. */
	RETURN_STRING(tmp_str, 1);
	free(tmp_str);
}

PHP_FUNCTION(bc_db_query)
{
	zval *z_ctx;
	char *sql;
	int sql_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
				  &sql, &sql_len) == FAILURE)
		RETURN_FALSE;

	if (bc_db_query("%s", sql))
		RETURN_FALSE;

	RETURN_TRUE;
}

PHP_FUNCTION(bc_db_get_table)
{
	zval *z_ctx;
	BC_DB_RES dbres;
	char *sql;
	int sql_len;
	int ncols;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
				  &sql, &sql_len) == FAILURE)
		RETURN_FALSE;

	dbres = bc_db_get_table("%s", sql);
	if (dbres == NULL)
		RETURN_FALSE;

	ncols = bc_db_num_fields(dbres);
	if (ncols < 0) {
		bc_db_free_table(dbres);
		RETURN_FALSE;
	}

	array_init(return_value);

	while (!bc_db_fetch_row(dbres)) {
		zval *row_arr;
		int i;

		ALLOC_INIT_ZVAL(row_arr);
		array_init(row_arr);

		for (i = 0; i < ncols; i++) {
			const char *field = bc_db_get_field(dbres, i);
			const char *str;

			if (field == NULL)
				continue;

			str = bc_db_get_val(dbres, field);
			if (str == NULL)
				continue;
			add_assoc_string(row_arr, field,
					 (char *)str, 1);
		}

		add_next_index_zval(return_value, row_arr);
	}

	bc_db_free_table(dbres);
}

PHP_FUNCTION(bc_handle_get)
{
	struct bc_handle *bch;
	char *devname;
	char *driver;
	int devname_len, driver_len;
	long card_id;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl", &devname,
				  &devname_len, &driver, &driver_len,
				  &card_id) == FAILURE)
		RETURN_FALSE;

	if ((bch = bc_handle_get(devname, driver, card_id)) == NULL)
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

PHP_FUNCTION(bc_log)
{
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
				  &str, &str_len) == FAILURE)
		RETURN_FALSE;

	bc_log("%s", str);

	RETURN_TRUE;
}

static function_entry bluecherry_functions[] = {
	/* Bluecherry DB Handlers */
	PHP_FE(bc_db_open, NULL)
	PHP_FE(bc_db_close, NULL)
	PHP_FE(bc_db_get_table, NULL)
	PHP_FE(bc_db_query, NULL)
	PHP_FE(bc_db_escape_string, NULL)
	/* Bluecherry Video Handlers */
	PHP_FE(bc_handle_get, NULL)
	PHP_FE(bc_handle_free, NULL)
	PHP_FE(bc_handle_start, NULL)
	PHP_FE(bc_buf_get, NULL)
	PHP_FE(bc_buf_size, NULL)
	PHP_FE(bc_buf_data, NULL)
	PHP_FE(bc_set_mjpeg, NULL)
	PHP_FE(bc_set_control, NULL)
	/* Miscellaneous */
	PHP_FE(bc_log, NULL)
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
