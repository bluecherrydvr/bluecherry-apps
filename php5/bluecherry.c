/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "libbluecherry.h"
#include "php.h"

typedef struct {
	struct bc_handle *bch;
} bch_ctx_t;

typedef struct {
	struct bc_db_handle *bcdb;
} bcdb_ctx_t;

static void bch_ctx_destructor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	bch_ctx_t *ctx = (bch_ctx_t *)rsrc->ptr;

	if (ctx) {
		if (ctx->bch) {
			bc_handle_free(ctx->bch);
			ctx->bch = NULL;
		}
		efree(ctx);
	}

	return;
}

static int bch_ctx_id;
#define bch_ctx_name "BC Handle context"

PHP_MINIT_FUNCTION(bluecherry)
{
	bch_ctx_id = zend_register_list_destructors_ex(bch_ctx_destructor,
						       NULL, bch_ctx_name,
						       module_number);

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
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",	\
				  &z_ctx) == FAILURE)			\
		RETURN_FALSE;						\
	ZEND_FETCH_RESOURCE(ctx, bch_ctx_t *, &z_ctx, -1, bch_ctx_name,	\
			    bch_ctx_id);				\
	if (ctx == NULL || ctx->bch == NULL) {				\
		php_error_docref(NULL TSRMLS_CC, E_WARNING,		\
			__func ": invalid context");			\
		RETURN_FALSE;						\
	}								\
}while(0)

PHP_FUNCTION(bc_handle_get)
{
	bch_ctx_t *ctx;
	char *devname;
	int devname_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &devname,
				  &devname_len) == FAILURE)
		RETURN_FALSE;

	if ((ctx = emalloc(sizeof(*ctx))) == NULL)
		RETURN_FALSE;

	if ((ctx->bch = bc_handle_get(devname)) == NULL) {
		efree(ctx);
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, ctx, bch_ctx_id);
}

PHP_FUNCTION(bc_handle_free)
{
	zval *z_ctx;
	bch_ctx_t *ctx;

	BCH_GET_RES("bc_handle_free");

	bc_handle_free(ctx->bch);
	ctx->bch = NULL;

	zend_list_delete(Z_LVAL_P(z_ctx));

	RETURN_TRUE;
}

PHP_FUNCTION(bc_handle_start)
{
	zval *z_ctx;
	bch_ctx_t *ctx;

	BCH_GET_RES("bc_handle_start");

	if (bc_handle_start(ctx->bch) < 0)
		RETURN_FALSE;

	RETURN_TRUE;
}

PHP_FUNCTION(bc_buf_get)
{
	zval *z_ctx;
	bch_ctx_t *ctx;

	BCH_GET_RES("bc_buf_get");

	bc_buf_get(ctx->bch);

	RETURN_TRUE;
}

PHP_FUNCTION(bc_buf_size)
{
	zval *z_ctx;
	bch_ctx_t *ctx;

	BCH_GET_RES("bc_buf_size");

	RETURN_LONG(bc_buf_size(ctx->bch));
}

PHP_FUNCTION(bc_buf_data)
{
	zval *z_ctx;
	bch_ctx_t *ctx;
        
	BCH_GET_RES("bc_buf_data");

	RETURN_STRINGL(bc_buf_data(ctx->bch), bc_buf_size(ctx->bch), 1);
}

PHP_FUNCTION(bc_set_mjpeg)
{
	zval *z_ctx;
	bch_ctx_t *ctx;

	BCH_GET_RES("bc_set_mjpeg");

	ctx->bch->vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	if (ioctl(ctx->bch->dev_fd, VIDIOC_S_FMT, &ctx->bch->vfmt) < 0)
		RETURN_FALSE;

	RETURN_TRUE;
}

static function_entry bluecherry_functions[] = {
	PHP_FE(bc_handle_get, NULL)
	PHP_FE(bc_handle_free, NULL)
	PHP_FE(bc_handle_start, NULL)
	PHP_FE(bc_buf_get, NULL)
	PHP_FE(bc_buf_size, NULL)
	PHP_FE(bc_buf_data, NULL)
	PHP_FE(bc_set_mjpeg, NULL)
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
