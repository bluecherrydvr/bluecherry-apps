#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <assert.h>

#include "bt.h"
#include "bc-server-sockets.h"

static const char * const sig_name[] = {
	[SIGABRT] = "Aborted",
	[SIGBUS]  = "Bad memory access",
	[SIGSEGV] = "Segment violation",
	[SIGILL]  = "Illegal instruction",
	[SIGFPE]  = "Floating point exception",
};

volatile sig_atomic_t shutdown_flag;
static volatile sig_atomic_t fatal_signal_in_progress;

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
		if (fatal_signal_in_progress)
			_exit(128 + signum);
		fatal_signal_in_progress = 1;
		bt(sig_name[signum], info->si_addr);
		bc_server_unlink_socket_paths();
		_exit(1);

	case SIGINT:
	case SIGTERM:
	case SIGQUIT:
	case SIGHUP:
		shutdown_flag = 1;

	default:
		return;
	}

	_exit(signum);
}

void signals_setup()
{
	int ret;

	shutdown_flag = 0;
	fatal_signal_in_progress = 0;

	const int sig[] = { SIGFPE, SIGILL, SIGSEGV, SIGBUS, SIGABRT, SIGINT, SIGTERM, SIGQUIT, SIGHUP };
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = sighandler;
	sa.sa_flags = SA_SIGINFO | SA_NOCLDWAIT;

	for (unsigned int i = 0; i < sizeof(sig) / sizeof(sig[0]); i++) {
		ret = sigaction(sig[i], &sa, NULL);
		assert(!ret);
	}

	// these signals are ignored:
	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_NOCLDWAIT;
	sa.sa_handler = SIG_IGN;
	ret = sigaction(SIGCHLD, &sa, NULL);
	assert(!ret);
	ret = sigaction(SIGPIPE, &sa, NULL);
	assert(!ret);
}
