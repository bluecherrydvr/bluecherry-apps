/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

/*
 * This is a simple obfuscation and passcode transform key checker. The basic
 * principal is that the bits we use are not stored in the 64-bit key in the
 * order we need them. Instead each byte in the key is treated as a FILO, and
 * we round-robin these buckets leaving a bit in each one, and moving to the
 * next. We reverse this procedure to extract the bits.
 *
 * In addition, we munge the 8 bytes using an addition transform with a known
 * and shared 8-byte value. The values in each position are added to the same
 * position in the license key buckets, and we carry overflow to the next
 * bucket. We likewise reverse this procedure to decode the license key before
 * extracting bits from the buckets. On subtraction, we assume that if the
 * passcode value is larger than the key value we are subtracting from, then
 * we need to carry 1 from the higher bucket (subtract 1 from it, and add it
 * to the current bucket).
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
	{ 0x32, 0x14, 0xfe, 0xed, 0xf0, 0x0c, 0x43, 0x25 };

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
				ctx->bytes[i + 1]--;
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

	res->major = bc_key_pullbits(&c, 4);
	res->minor = bc_key_pullbits(&c, 4);

	/* We don't know this license format yet */
	if (res->major != 2 || res->minor != 1)
		return EINVAL;

	/* Padding for now, reserved for later */
	bc_key_pullbits(&c, 7);

	res->type = bc_key_pullbits(&c, 4);
	res->count = bc_key_pullbits(&c, 5);
	res->id = bc_key_pullbits(&c, 32);

	return 0;
}
