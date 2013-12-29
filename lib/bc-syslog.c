#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <unistd.h>
#include "bc-syslog.h"
#include "iov-macros.h"

static int syslog_fd;

void bc_syslog_init()
{
	int sock = socket(AF_UNIX, SOCK_DGRAM, 0);

	if (sock == -1)
		_exit(1);

	struct sockaddr_un usock = {
		.sun_family = AF_UNIX,
		.sun_path = "/dev/log",
	};

	if (connect(sock, &usock, sizeof(usock)))
		_exit(1);

	syslog_fd = sock;
}


void bc_syslogv(struct iovec *iov, int iovcnt)
{
	if (!syslog_fd)
		return;

	/* LOG_INFO | LOG_DAEMON */
	VSTR(iov[0], "<30>");
	writev(syslog_fd, iov, iovcnt);
}
