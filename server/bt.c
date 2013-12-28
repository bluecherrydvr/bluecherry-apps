#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/uio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <execinfo.h>
#include <dlfcn.h>
#include "bt.h"
#include "bc-syslog.h"
#include "iov-macros.h"

static
void ptrtohex(char *dst, unsigned size, size_t ptr)
{
	for (; size--; ptr >>= 4) {
		char v = ptr & 15;
		dst[size] = v + ((v >= 10) ? 'a' - 10 : '0');
	}
}

#define PTRS_SZ (sizeof(size_t) * 2)

static
unsigned bt_fill_entry(struct iovec *out, char *faddr, char *saddr, size_t addr)
{
	Dl_info d;

	unsigned last = 4;

	int ret = dladdr((void *)addr, &d);
	if (!ret) {
		ptrtohex(faddr, PTRS_SZ, addr);
		return last;
	}

	ptrtohex(faddr, PTRS_SZ, addr - (size_t)d.dli_fbase);

	if (d.dli_sname) {
		VSTR(out[last++], d.dli_sname);
		VSTR(out[last++], "+0x");
		VBUF(out[last++], saddr);
		ptrtohex(saddr, PTRS_SZ, addr - (size_t)d.dli_saddr);
	}

	VSTR(out[last++], " @ ");
	VSTR(out[last++], d.dli_fname);
	return last;
}

static
size_t rebase(size_t p)
{
	Dl_info d;

	int ret = dladdr((void *)p, &d);
	return p - (ret ? (size_t)d.dli_fbase : 0x400000);
}

static
void bt_print_entries(size_t *btrace, unsigned len, size_t p, const char *err)
{
	char addr[2][PTRS_SZ];
	struct iovec out[9];


	VSTR(out[1], "BUG: ");
	VSTR(out[2], err);
	VSTR(out[3], " at 0x");
	VBUF(out[4], addr[0]);
	ptrtohex(addr[0], PTRS_SZ, rebase(p));
	bc_syslogv(out, 5);

	VSTR(out[1], "Call trace:");
	bc_syslogv(out, 2);

	if (!len)
		return;

	VSTR(out[1], "[0x");
	VBUF(out[2], addr[0]);
	VSTR(out[3], "] ");

	while (len--) {
		unsigned last = bt_fill_entry(out, addr[0], addr[1], *btrace++);
		bc_syslogv(out, last);
	}
}

void bt(const char *err, const void *ptr)
{
	size_t p = (size_t)ptr;

	size_t btrace[64];

	size_t size = backtrace((void **)btrace,
				sizeof(btrace) / sizeof(btrace[0]));

	/* Start from ptr if possible */
	size_t first;
	for (first = 1; first < size && btrace[first] != p ; first++);
	if (first >= size)
		first = 1;

	bt_print_entries(btrace + first, size - first, p, err);
}
