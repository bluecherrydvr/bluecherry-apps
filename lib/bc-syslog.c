#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>

#include "bc-syslog.h"
#include "iov-macros.h"

static char hostname[16];

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

	if (connect(sock, (struct sockaddr *)&usock, sizeof(usock)))
		_exit(1);

	syslog_fd = sock;

	int ret = gethostname(hostname, sizeof(hostname));
	if (ret == -1) {
		if (errno == ENAMETOOLONG)
			hostname[sizeof(hostname) - 1] = 0;
		else
			strcpy(hostname, "localhost");
	}
}

static
size_t mkmsghdr(char *dst, size_t max, unsigned prio, const char *host)
{
	struct tm tm;
	time_t t = time(NULL);
	localtime_r(&t, &tm);

	const char month[12][4] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};

	/* XXX: getpid on glibc caches the result, using another libc might
	 * require catching it ourselves... */
	return snprintf(dst, max, "<%u>%s %2d %02d:%02d:%02d %s bc-server[%d]: ",
			prio, month[tm.tm_mon],
			tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
			host, getpid());
}

void bc_syslogv(bc_logv *iov, int iovcnt)
{
	char hdr[72];
	size_t hdr_size = mkmsghdr(hdr, sizeof(hdr), 30, hostname);

	iov--;
	iovcnt++;

	/* LOG_INFO | LOG_DAEMON */
	VSET(iov[0], hdr, hdr_size);
	writev(syslog_fd, iov, iovcnt);
}
