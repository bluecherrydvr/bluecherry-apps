#ifndef WATCHDOG_H
#define WATCHDOG_H
#ifdef __cplusplus
extern "C" {
#endif

struct watchdog;

typedef void (*watchdog_cb_t)(struct watchdog *);

struct watchdog {
	int cnt;
	watchdog_cb_t cb;
	struct watchdog *next;
};

static inline
void bc_watchdog_update(struct watchdog *p)
{
	p->cnt++;
}

void bc_watchdog_add(struct watchdog *p, watchdog_cb_t cb);
void bc_watchdog_rm(struct watchdog *p);
void bc_watchdog_check_all();

#ifdef __cplusplus
}
#endif
#endif /* !defined(WATCHDOG_H) */
