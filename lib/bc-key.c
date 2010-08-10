/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <string.h>
#include <errno.h>

#include <libbluecherry.h>

#define BC_KEY_LEN		8
#define BC_KEY_MAGIC		0xBC

struct bc_key_ctx {
	int idx;
	unsigned char bytes[BC_KEY_LEN];
};

/* Our super secret passcode, used for license key codec */
static unsigned char segment_end[BC_KEY_LEN] =
	{ 0x32, 0x14, 0xfe, 0xed, 0xfu, 0xcc, 0x43, 0x25 };

static void bc_key_start(struct bc_key_ctx *ctx, unsigned char *key)
{
	int i;

	ctx->idx = 0;

	/* Copy the current key, and transform it with passcode */
	for (i = BC_KEY_LEN - 1; i >= 0; i--) {
		unsigned short j = key[i];

		/* If this byte is too small, carry one from next byte */
		if (j < segment_end[i]) {
			j += 0x100;
			if (i < BC_KEY_LEN - 1)
				key[i + 1]--;
		}

		ctx->bytes[i] = j - segment_end[i];
	}
}

static unsigned int bc_key_pullbits(struct bc_key_ctx *ctx, int len)
{
	unsigned int bits = 0;
	int i;

	for (i = 0; i < len; i++) {
		bits <<= 1;
		bits |= ctx->bytes[ctx->idx] & 0x1;
		ctx->bytes[ctx->idx] >>= 1;
		ctx->idx = (ctx->idx + 1) % BC_KEY_LEN;
	}

	return bits;
}

/* Returns errno or 0 for success */
int bc_key_process(struct bc_key_data *res, char *str)
{
	unsigned char key[BC_KEY_LEN];
	struct bc_key_ctx c;
	int i;
	int len;

	memset(key, 0, BC_KEY_LEN);

	for (i = len = 0; str[i] != '\0'; i++) {
		unsigned char t;

		/* Means incoming string is too long */
		if (len >= BC_KEY_LEN * 2)
			return 0;

		/* If it's valid hex, process it */
		if (str[i] >= 'a' && str[i] <= 'f')
			t = (str[i] - 'a') + 0x0a;
		else if (str[i] >= 'A' && str[i] <= 'F')
			t = (str[i] - 'A') + 0x0a;
		else if (str[i] >= '0' && str[i] <= '9')
			t = str[i] - '0';
		else if (str[i] == '-')
			continue;
		else
			return EINVAL;

		/* Valid character and value now */
		if (!(len & 0x1))
			t <<= 4;

		key[len / 2] |= t;
		len++;
	}

	/* Too few characters */
	if (len != BC_KEY_LEN * 2)
		return EINVAL;

	bc_key_start(&c, key);

	/* ORDER MATTERS HERE! */
	if (bc_key_pullbits(&c, 8) != BC_KEY_MAGIC)
		return EINVAL;

	/* Padding for now, reserved for later */
	bc_key_pullbits(&c, 12);

	res->type = bc_key_pullbits(&c, 4);
	res->major = bc_key_pullbits(&c, 4);
	res->count = bc_key_pullbits(&c, 4);
	res->id = bc_key_pullbits(&c, 32);

	return 0;
}
