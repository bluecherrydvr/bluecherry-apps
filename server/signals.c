#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <execinfo.h>

#include "bt.h"

static const char * const sig_name[] = {
	[SIGABRT] = "Aborted",
	[SIGBUS]  = "Bad memory access",
	[SIGSEGV] = "Segment violation",
	[SIGILL]  = "Illegal instruction",
	[SIGFPE]  = "Floating point exception",
};

void bt(const char *err, const void *p)
{
	fprintf(stderr, "F: %s at %p\n\nBacktrace:\n", err, p);
	fflush(stderr);

	void *btrace[32];

	size_t first, size = backtrace(btrace, sizeof(btrace) / sizeof(btrace[0]));

	for (first = 0; first < size && btrace[first] != p ; first++);
	if (first >= size) {
		fprintf(stderr,
			"E: Can't find starting point in the backtrace\n");
		return;
	}

	backtrace_symbols_fd(btrace + first, size - first, STDERR_FILENO);
}

static void sighandler(int signum, siginfo_t *info, void *ctx)
{
	(void)ctx;

	switch (signum) {
	case SIGABRT:
		bt(sig_name[signum], RET_ADDR);
	case SIGBUS:
	case SIGSEGV:
	case SIGILL:
	case SIGFPE:
		bt(sig_name[signum], info->si_addr);
	default:
		/* SIGCHLD */
		return;
	}

	_exit(signum);
}

void signals_setup()
{
	const int sig[] = { SIGCHLD, SIGFPE, SIGILL, SIGSEGV, SIGBUS, SIGABRT };
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = sighandler;
	sa.sa_flags = SA_SIGINFO | SA_NOCLDWAIT;

	for (unsigned int i = 0; i < sizeof(sig) / sizeof(sig[0]); i++)
		sigaction(sig[i], &sa, NULL);

	signal(SIGPIPE, SIG_IGN);
}
