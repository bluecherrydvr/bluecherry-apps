#ifndef BC_SYSLOG_H
#define BC_SYSLOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/uio.h>

void bc_syslog_init();
void bc_syslogv(struct iovec *iov, int iovcnt);

#ifdef __cplusplus
}
#endif
#endif
