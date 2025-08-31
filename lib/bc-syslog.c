#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>

#include "bc-syslog.h"
#include "iov-macros.h"

static char prgid[48];

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

	char *p = prgid;
	int p_len = sizeof(prgid);

#ifdef BC_SYSLOG_USE_HOSTNAME
	const int hn_max_len = 16;
	int ret = gethostname(p, hn_max_len);
	if (ret == -1) {
		if (errno == ENAMETOOLONG)
			p[hn_max_len] = 0;
		else
			strcpy(p, "localhost");
	}
	{
		size_t hn_len = strlen(p);
		p[hn_len++] = ' ';
		p += hn_len;
		p_len -= hn_len;
	}
#endif

	snprintf(p, p_len, "bc-server[%d]", getpid());
}

static
size_t mkmsghdr(char *dst, size_t max, unsigned prio)
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
	return snprintf(dst, max, "<%u>%s %2d %02d:%02d:%02d %s: ",
			prio, month[tm.tm_mon],
			tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
			prgid);
}

void bc_syslogv(bc_logv *iov, int iovcnt)
{
	char hdr[72];
	size_t hdr_size = mkmsghdr(hdr, sizeof(hdr), 30);

	iov--;
	iovcnt++;

	/* LOG_INFO | LOG_DAEMON */
	VSET(iov[0], hdr, hdr_size);

	ssize_t ret = writev(syslog_fd, iov, iovcnt);
	if (ret == -1 && (errno == ECONNREFUSED || errno == ENOTCONN || errno == EBADF)) {
		// Attempt to reconnect to /dev/log and retry once
		int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
		if (sock != -1) {
			struct sockaddr_un usock = {
				.sun_family = AF_UNIX,
				.sun_path = "/dev/log",
			};
			if (connect(sock, (struct sockaddr *)&usock, sizeof(usock)) == 0) {
				close(syslog_fd);
				syslog_fd = sock;
				// Retry the log write once
				writev(syslog_fd, iov, iovcnt);
			} else {
				close(sock);
			}
		}
	}
}
