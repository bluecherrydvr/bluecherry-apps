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

#include "g723-dec.h"

static const short _dqlntab[8] = {
	-2048, 135, 273, 373, 373, 273, 135, -2048
};

static const short _witab[8] = {
	-128, 960, 4384, 18624, 18624, 4384, 960, -128
};

static const short _fitab[8] = {
	0, 0x200, 0x400, 0xE00, 0xE00, 0x400, 0x200, 0
};

static inline int ilog2(int val) __attribute__((const));

#if defined(__i386__) || defined(i386)
static inline int ilog2(int val)
{
	int a;
	__asm__
	("\
		xorl	%0, %0	;\
		decl	%0	;\
		bsrl	%1, %0	;\
		"
		: "=&r" (a)
		: "mr" (val)
		: "cc"
	);

	return a;
}
#else
static inline int ilog2(int val)
{
	int i;
	for (i = -1; val; ++i, val >>= 1)
		/* Do nothing */;
	return i;
}
#endif

static int fmult(int an, int srn) __attribute__((const));

static int fmult(int an, int srn)
{
	short anmag, anexp, anmant;
	short wanexp, wanmant;
	short ret;

	anmag = (an > 0) ? an : ((-an) & 0x1FFF);
	anexp = ilog2(anmag) - 6;

	if (anmag == 0)
		anmant = 32;
	else if (anexp >= 0)
		anmant = anmag >> anexp;
	else
		anmant = anmag << -anexp;

	wanexp = anexp + ((srn >> 6) & 0xF) - 13;

	wanmant = (anmant * (srn & 077) + 0x30) >> 4;
	if (wanexp >= 0)
		ret = (wanmant << wanexp) & 0x7FFF;
	else
		ret = wanmant >> -wanexp;

	return ((an ^ srn) < 0) ? -ret : ret;
}

static int predictor_zero(struct g723_state *st)
{
	int i;
	int sezi;

	sezi = fmult(st->b[0] >> 2, st->dq[0]);
	for (i = 1; i < 6; i++)
		sezi += fmult(st->b[i] >> 2, st->dq[i]);

	return sezi;
}

static int predictor_pole(struct g723_state *st)
{
	return fmult(st->a[1] >> 2, st->sr[1]) +
	       fmult(st->a[0] >> 2, st->sr[0]);
}

static int step_size(struct g723_state *st)
{
	int y;
	int dif;
	int al;

	if (st->ap >= 256)
		return st->yu;

	y = st->yl >> 6;
	dif = st->yu - y;
	al = st->ap >> 2;

	if (dif > 0)
		y += (dif * al) >> 6;
	else if (dif < 0)
		y += (dif * al + 0x3F) >> 6;

	return y;
}

static int reconstruct(int sign, int dqln, int y)
{
	short dql;
	short dex;
	short dqt;
	short dq;

	dql = dqln + (y >> 2);

	if (dql < 0)
		return sign ? -0x8000 : 0;

	dex = (dql >> 7) & 15;
	dqt = 128 + (dql & 127);
	dq = (dqt << 7) >> (14 - dex);

	return sign ? (dq - 0x8000) : dq;
}

static short range(short val, short min, short max)
{
	if (val < min)
		return min;
	else if (val > max)
		return max;

	return val;
}

static void update(int y, int wi, int fi, int dq, int sr, int dqsez,
		   struct g723_state *st)
{
	int cnt;
	short mag, exp;
	short a2p = 0; /* FIXME: initialized to avoid warning */
	short a1ul;
	short pks1;
	short fa1;
	char tr;
	short ylint, thr2, dqthr;
	short ylfrac, thr1;
	short pk0;

	pk0 = (dqsez < 0) ? 1 : 0;

	mag = dq & 0x7FFF;

	ylint = st->yl >> 15;
	ylfrac = (st->yl >> 10) & 0x1F;
	thr1 = (32 + ylfrac) << ylint;
	thr2 = (ylint > 9) ? 31 << 10 : thr1;
	dqthr = (thr2 + (thr2 >> 1)) >> 1;

	if (st->td == 0)
		tr = 0;
	else if (mag <= dqthr)
		tr = 0;
	else
		tr = 1;

	st->yu = y + ((wi - y) >> 5);

	st->yu = range(st->yu, 544, 5120);

	st->yl += st->yu + ((-st->yl) >> 6);

	if (tr == 1) {
		memset(st->a, 0, sizeof(st->a));
		memset(st->b, 0, sizeof(st->b));
	} else {
		pks1 = pk0 ^ st->pk[0];

		a2p = st->a[1] - (st->a[1] >> 7);
		if (dqsez != 0) {
			fa1 = (pks1) ? st->a[0] : -st->a[0];
			if (fa1 < -8191)
				a2p -= 0x100;
			else if (fa1 > 8191)
				a2p += 0xFF;
			else
				a2p += fa1 >> 5;

			if (pk0 ^ st->pk[1])
				if (a2p <= -12160)
					a2p = -12288;
				else if (a2p >= 12416)
					a2p = 12288;
				else
					a2p -= 0x80;
			else if (a2p <= -12416)
				a2p = -12288;
			else if (a2p >= 12160)
				a2p = 12288;
			else
				a2p += 0x80;
		}

		st->a[1] = a2p;

		st->a[0] -= st->a[0] >> 8;
		if (dqsez != 0) {
			if (pks1 == 0)
				st->a[0] += 192;
			else
				st->a[0] -= 192;
		}

		a1ul = 15360 - a2p;
		if (st->a[0] < -a1ul)
			st->a[0] = -a1ul;
		else if (st->a[0] > a1ul)
			st->a[0] = a1ul;

		for (cnt = 0; cnt < 6; cnt++) {
			st->b[cnt] -= st->b[cnt] >> 8;
			if (dq & 0x7FFF) {	/* XOR */
				if ((dq ^ st->dq[cnt]) >= 0)
					st->b[cnt] += 128;
				else
					st->b[cnt] -= 128;
			}
		}
	}

	for (cnt = 5; cnt > 0; cnt--)
		st->dq[cnt] = st->dq[cnt - 1];

	if (mag == 0) {
		st->dq[0] = (dq >= 0) ? 0x20 : 0xFC20;
	} else {
		exp = ilog2(mag);
		st->dq[0] = (dq >= 0) ?
		    (exp << 6) + ((mag << 6) >> exp) :
		    (exp << 6) + ((mag << 6) >> exp) - 0x400;
	}

	st->sr[1] = st->sr[0];

	if (sr == 0) {
		st->sr[0] = 0x20;
	} else if (sr > 0) {
		exp = ilog2(sr);
		st->sr[0] = (exp << 6) + ((sr << 6) >> exp);
	} else if (sr > -32768) {
		mag = -sr;
		exp = ilog2(mag);
		st->sr[0] = (exp << 6) + ((mag << 6) >> exp) - 0x400;
	} else
		st->sr[0] = 0xFC20;

	st->pk[1] = st->pk[0];
	st->pk[0] = pk0;

	if (tr == 1)
		st->td = 0;
	else if (a2p < -11776)
		st->td = 1;
	else
		st->td = 0;

	st->dms += (fi - st->dms) >> 5;
	st->dml += ((fi << 2) - st->dml) >> 7;

	if (tr == 1)
		st->ap = 256;
	else if (y < 1536)
		st->ap += (0x200 - st->ap) >> 4;
	else if (st->td == 1)
		st->ap += (0x200 - st->ap) >> 4;
	else if (abs((st->dms << 2) - st->dml) >= (st->dml >> 3))
		st->ap += (0x200 - st->ap) >> 4;
	else
		st->ap += (-st->ap) >> 4;
}

static short decode_one(struct g723_state *st, unsigned char in)
{
	short sezi, se;
	short y;
	short sr;
	short dq;
	short dqsez;

	in &= 0x07;

	sezi = predictor_zero(st);
	se = (sezi + predictor_pole(st)) >> 1;

	y = step_size(st);
	dq = reconstruct(in & 0x04, _dqlntab[in], y);

	sr = (dq < 0) ? (se - (dq & 0x3FFF)) : (se + dq);

	dqsez = sr - se + (sezi >> 1);

	update(y, _witab[in], _fitab[in], dq, sr, dqsez, st);

	return (sr << 2);
}

void g723_init(struct g723_state *st)
{
	int i;

	memset(st, 0, sizeof(*st));

	st->yl = 34816;
	st->yu = 544;

	for (i = 0; i < 6; i++) {
		st->dq[i] = 32;
		if (i < 2)
			st->sr[i] = 32;
	}
}

int g723_decode(struct g723_state *st, const unsigned char *in_buf,
		int in_bytes, short *out_buf)
{
	unsigned char bits = 0;
	unsigned short buf = 0;
	unsigned char code;
	int idx = 0;
	int out_idx = 0;

	while (idx < in_bytes || bits >= 3) {
		if (bits < 3) {
			buf |= in_buf[idx++] << bits;
			bits += 8;
		}

		code = buf & 0x07;
		buf >>= 3;
		bits -= 3;

		out_buf[out_idx++] = decode_one(st, code);
	}

	return out_idx;
}
