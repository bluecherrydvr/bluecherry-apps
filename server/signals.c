#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "bt.h"

static const char * const sig_name[] = {
	[SIGABRT] = "Aborted",
	[SIGBUS]  = "Bad memory access",
	[SIGSEGV] = "Segment violation",
	[SIGILL]  = "Illegal instruction",
	[SIGFPE]  = "Floating point exception",
};

static void sighandler(int signum, siginfo_t *info, void *ctx)
{
	(void)ctx;

	switch (signum) {
	case SIGABRT:
		info->si_addr = RET_ADDR;
	case SIGBUS:
	case SIGSEGV:
	case SIGILL:
	case SIGFPE:
		bt(sig_name[signum], info->si_addr);
		_exit(1);
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
