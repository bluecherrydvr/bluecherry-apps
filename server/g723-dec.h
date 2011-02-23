/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __G723_DEC_H
#define	__G723_DEC_H

struct g723_state {
	int yl;

	short yu;
	short dms;
	short dml;
	short ap;

	short a[2];
	short b[6];
	short pk[2];
	short dq[6];
	short sr[2];

	char td;
};

void g723_init(struct g723_state *);

/* Decode a block of codes. Out buf must be at least
 * ((in_size * 8) / 3) * 2) bytes in length (or half that in shorts). */
int g723_decode(struct g723_state *state, const unsigned char *in_buf,
		int in_bytes, short *out_buf);

#endif /* __G723_DEC_H */
