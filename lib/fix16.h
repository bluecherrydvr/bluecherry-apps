/* fix16 -- 16.16-bit fixed-point math
 *
 * Copyright (C) 2013 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __FIX16_H__
#define __FIX16_H__

typedef int32_t fix16_t;

static inline
fix16_t fix16_from_float(float a)
{
	return (fix16_t)(a * 0x10000);

}

static inline
fix16_t fix16_from_frac(uint32_t n, uint32_t d)
{
	uint32_t u = (n & ~0xffff) / d;
	return (u << 16) + (n << 16) / d;
}

static inline
int fix16_to_int(fix16_t a)
{
	return (a >> 16) + ((a & 0x8000) >> 15);
}

#endif /* !defined(__FIX16_H__) */
