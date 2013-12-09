/*
 * Fowler/Noll/Vo hash implementation
 *
 * FNV hashes are designed to be fast while maintaining a low
 * collision rate. The FNV speed allows one to quickly hash lots
 * of data while maintaining a reasonable collision rate.  See:
 *
 *   http://www.isthe.com/chongo/tech/comp/fnv/index.html
 *
 * for more details as well as other forms of the FNV hash.
 *
 *
 * Copyright 2011 Ismael Luceno <ismael.luceno@gmail.com>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__i386__) || defined (__x86_64__)
#define FNV_AVOID_MUL 1
#else
#define FNV_AVOID_MUL 0
#endif

#include <sys/types.h>

#define FNV_INIT  ((uint64_t)0xcbf29ce484222325ULL)
#define FNV_PRIME ((uint64_t)0x100000001b3ULL)


inline static
uint64_t fnv_pass(uint64_t hval, char octet)
{
	hval ^= (uint64_t) octet;

	if (FNV_AVOID_MUL)
		hval += (hval << 1) + (hval << 4) + (hval << 5) +
			(hval << 7) + (hval << 8) + (hval << 40);
	else
		hval *= FNV_PRIME;

	return hval;
}

/**
 * Perform a 64 bit FNV-1a hash on a buffer.
 *
 * @param buf	Buffer address.
 * @param len	Length of the buffer, in bytes.
 * @return	Hash of the buffer contents.
 */
inline static
uint64_t fnv_buf(void *buf, size_t len)
{
	const char *bp = (const char *)buf;
	const char *be = bp + len;

	uint64_t hval = FNV_INIT;

	while (bp < be)
		hval = fnv_pass(hval, *bp++);

	return hval;
}


/**
 * Perform a 64 bit FNV-1a hash on a buffer.
 *
 * @param buf	String address.
 * @return	Hash of the string contents.
 */
inline static
uint64_t fnv_str(const char *str)
{
	uint64_t hval = FNV_INIT;

	while (*str)
		hval = fnv_pass(hval, *str++);

	return hval;
}

#ifdef __cplusplus
}
#endif
