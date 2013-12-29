#ifndef BC_SYSLOG_H
#define BC_SYSLOG_H

#include <sys/uio.h>
#include <alloca.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iovec bc_logv;

void bc_syslog_init();
void bc_syslogv(bc_logv *iov, int iovcnt);

#define bc_logv_alloc(len) (1 + \
	(bc_logv *)alloca((1 + len) * sizeof(bc_logv)))

#ifdef __cplusplus
}
#endif
#endif
