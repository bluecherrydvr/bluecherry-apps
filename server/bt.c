#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <execinfo.h>
#include "bt.h"


void bt(const char *err, const void *p)
{
	fprintf(stderr, "F: %s at %p\n\nBacktrace:\n", err, p);
	fflush(stderr);

	void *btrace[64];

	size_t size = backtrace(btrace, sizeof(btrace) / sizeof(btrace[0]));
	size_t first;

	for (first = 1; first < size && btrace[first] != p ; first++);
	if (first >= size)
		first = 1;

	backtrace_symbols_fd(btrace + first, size - first, STDERR_FILENO);
}
