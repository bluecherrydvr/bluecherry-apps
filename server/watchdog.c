#include "watchdog.h"

static struct watchdog *watchdogs_list = 0;

void bc_watchdog_check_all()
{
	struct watchdog *p = watchdogs_list;
	for (; p; p = p->next) {
		if (p->cnt)
			p->cnt = 0;
		else
			p->cb(p);
	}
}


void bc_watchdog_add(struct watchdog *p, watchdog_cb_t cb)
{
	p->cnt = 0;
	p->cb = cb;

	struct watchdog *list;
	do {
		/* Avoid stressing the memory bus */
#if defined(__x86_64) || defined(__i386)
		__asm__("pause":::"memory");
#endif
		list = watchdogs_list;
		p->next = list;
	} while (!__sync_bool_compare_and_swap(&watchdogs_list, list, p));
}


void bc_watchdog_rm(struct watchdog *p)
{
	if (__sync_bool_compare_and_swap(&watchdogs_list, p, p->next))
		return;

	struct watchdog *cur, *prev = watchdogs_list;
	for (; (cur = prev->next); prev = cur) {
		if (cur != p)
			continue;
		prev->next = cur->next;
	}
}
