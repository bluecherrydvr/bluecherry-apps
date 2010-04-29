/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <string.h>

#include "g723-dec.h"

static const short power2_data[15] = {
	1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
	0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000
};

static const short _dqlntab[8] = {
	-2048, 135, 273, 373, 373, 273, 135, -2048
};

static const short _witab[8] = {
	-128, 960, 4384, 18624, 18624, 4384, 960, -128
};

static const short _fitab[8] = {
	0, 0x200, 0x400, 0xE00, 0xE00, 0x400, 0x200, 0
};

static int power2(int val)
{
	int i;

	for (i = 0; i < 15; i++)
		if (val < power2_data[i])
			break;

	return i;
}

static int fmult(int an, int srn)
{
	short anmag, anexp, anmant;
	short wanexp, wanmant;
	short ret;

	anmag = (an > 0) ? an : ((-an) & 0x1FFF);
	anexp = power2(anmag) - 6;

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

static int predictor_zero(struct g723_state *state)
{
	int i;
	int sezi;

	sezi = fmult(state->b[0] >> 2, state->dq[0]);
	for (i = 1; i < 6; i++)
		sezi += fmult(state->b[i] >> 2, state->dq[i]);

	return sezi;
}

static int predictor_pole(struct g723_state *state)
{
	return fmult(state->a[1] >> 2, state->sr[1]) +
	       fmult(state->a[0] >> 2, state->sr[0]);
}

static int step_size(struct g723_state *state)
{
	int y;
	int dif;
	int al;

	if (state->ap >= 256)
		return state->yu;

	y = state->yl >> 6;
	dif = state->yu - y;
	al = state->ap >> 2;

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
		   struct g723_state *state)
{
	int cnt;
	short mag, exp;
	short a2p;
	short a1ul;
	short pks1;
	short fa1;
	char tr;
	short ylint, thr2, dqthr;
	short ylfrac, thr1;
	short pk0;

	pk0 = (dqsez < 0) ? 1 : 0;

	mag = dq & 0x7FFF;

	ylint = state->yl >> 15;
	ylfrac = (state->yl >> 10) & 0x1F;
	thr1 = (32 + ylfrac) << ylint;
	thr2 = (ylint > 9) ? 31 << 10 : thr1;
	dqthr = (thr2 + (thr2 >> 1)) >> 1;

	if (state->td == 0)
		tr = 0;
	else if (mag <= dqthr)
		tr = 0;
	else
		tr = 1;

	state->yu = y + ((wi - y) >> 5);

	state->yu = range(state->yu, 544, 5120);

	state->yl += state->yu + ((-state->yl) >> 6);

	if (tr == 1) {
		memset(state->a, 0, sizeof(state->a));
		memset(state->b, 0, sizeof(state->b));
	} else {
		pks1 = pk0 ^ state->pk[0];

		a2p = state->a[1] - (state->a[1] >> 7);
		if (dqsez != 0) {
			fa1 = (pks1) ? state->a[0] : -state->a[0];
			if (fa1 < -8191)
				a2p -= 0x100;
			else if (fa1 > 8191)
				a2p += 0xFF;
			else
				a2p += fa1 >> 5;

			if (pk0 ^ state->pk[1])
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

		state->a[1] = a2p;

		state->a[0] -= state->a[0] >> 8;
		if (dqsez != 0) {
			if (pks1 == 0)
				state->a[0] += 192;
			else
				state->a[0] -= 192;
		}

		a1ul = 15360 - a2p;
		if (state->a[0] < -a1ul)
			state->a[0] = -a1ul;
		else if (state->a[0] > a1ul)
			state->a[0] = a1ul;

		for (cnt = 0; cnt < 6; cnt++) {
			state->b[cnt] -= state->b[cnt] >> 8;
			if (dq & 0x7FFF) {	/* XOR */
				if ((dq ^ state->dq[cnt]) >= 0)
					state->b[cnt] += 128;
				else
					state->b[cnt] -= 128;
			}
		}
	}

	for (cnt = 5; cnt > 0; cnt--)
		state->dq[cnt] = state->dq[cnt - 1];

	if (mag == 0) {
		state->dq[0] = (dq >= 0) ? 0x20 : 0xFC20;
	} else {
		exp = power2(mag);
		state->dq[0] = (dq >= 0) ?
		    (exp << 6) + ((mag << 6) >> exp) :
		    (exp << 6) + ((mag << 6) >> exp) - 0x400;
	}

	state->sr[1] = state->sr[0];

	if (sr == 0) {
		state->sr[0] = 0x20;
	} else if (sr > 0) {
		exp = power2(sr);
		state->sr[0] = (exp << 6) + ((sr << 6) >> exp);
	} else if (sr > -32768) {
		mag = -sr;
		exp = power2(mag);
		state->sr[0] = (exp << 6) + ((mag << 6) >> exp) - 0x400;
	} else
		state->sr[0] = 0xFC20;

	state->pk[1] = state->pk[0];
	state->pk[0] = pk0;

	if (tr == 1)
		state->td = 0;
	else if (a2p < -11776)
		state->td = 1;
	else
		state->td = 0;

	state->dms += (fi - state->dms) >> 5;
	state->dml += ((fi << 2) - state->dml) >> 7;

	if (tr == 1)
		state->ap = 256;
	else if (y < 1536)
		state->ap += (0x200 - state->ap) >> 4;
	else if (state->td == 1)
		state->ap += (0x200 - state->ap) >> 4;
	else if (abs((state->dms << 2) - state->dml) >= (state->dml >> 3))
		state->ap += (0x200 - state->ap) >> 4;
	else
		state->ap += (-state->ap) >> 4;
}

static short decode_one(struct g723_state *state, unsigned char in)
{
	short sezi, se;
	short y;
	short sr;
	short dq;
	short dqsez;

	in &= 0x07;

	sezi = predictor_zero(state);
	se = (sezi + predictor_pole(state)) >> 1;

	y = step_size(state);
	dq = reconstruct(in & 0x04, _dqlntab[in], y);

	sr = (dq < 0) ? (se - (dq & 0x3FFF)) : (se + dq);

	dqsez = sr - se + (sezi >> 1);

	update(y, _witab[in], _fitab[in], dq, sr, dqsez, state);

	return (sr << 2);
}

void g723_init(struct g723_state *state)
{
	int i;

	memset(state, 0, sizeof(*state));

	state->yl = 34816;
	state->yu = 544;

	for (i = 0; i < 6; i++) {
		state->dq[i] = 32;
		if (i < 2)
			state->sr[i] = 32;
	}
}

int g723_decode(struct g723_state *state, const unsigned char *in_buf,
		int in_bytes, short *out_buf)
{
	unsigned char code;
	int idx = 0;
	int out_idx = 0;

	while (idx < in_bytes || state->bits >= 3) {
		if (state->bits < 3) {
			state->buf |= in_buf[idx++] << state->bits;
			state->bits += 8;
		}

		code = state->buf & 0x07;
		state->buf >>= 3;
		state->bits -= 3;

		out_buf[out_idx++] = decode_one(state, code);
	}

	return out_idx;
}
