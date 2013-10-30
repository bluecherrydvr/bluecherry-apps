#include "watchdog.h"

static struct watchdog *watchdogs_list = 0;

static
void __bc_watchdog_rm(struct watchdog *p, struct watchdog *base)
{
	struct watchdog *cur, *prev = base ? base : watchdogs_list;

	if (!base && __sync_bool_compare_and_swap(&watchdogs_list, p, p->next))
		return;

	for (; (cur = prev->next); prev = cur) {
		if (cur != p)
			continue;
		prev->next = cur->next;
	}
}

void bc_watchdog_check_all()
{
	struct watchdog *p = watchdogs_list, *prev = 0;
	for (; p; prev = p, p = p->next) {
		if (p->cnt)
			p->cnt = 0;
		else {
			__bc_watchdog_rm(p, prev);
			p->cb(p);
		}
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
	__bc_watchdog_rm(p, 0);
}
