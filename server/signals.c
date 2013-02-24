#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <execinfo.h>

#define RET_ADDR (__builtin_extract_return_addr(__builtin_return_address(0)))

static void bt(int signum, void *p) __attribute__((noreturn));

static const char * const sig_name[] = {
	[SIGABRT] = "Aborted",
	[SIGBUS]  = "Bad memory access",
	[SIGSEGV] = "Segment violation",
	[SIGILL]  = "Illegal instruction",
	[SIGFPE]  = "Floating point exception",
};

static void bt(int signum, void *p)
{
	fprintf(stderr, "F: %s at %p\n\nBacktrace:\n", sig_name[signum], p);
	fflush(stderr);

	void *btrace[18];

	size_t size = backtrace(btrace, sizeof(btrace) / sizeof(btrace[0]));
	backtrace_symbols_fd(btrace + 2, size - 2, STDERR_FILENO);

	_exit(signum);
}

static void sighandler(int signum, siginfo_t *info, void *ctx)
{
	switch (signum) {
	case SIGABRT:
		bt(signum, RET_ADDR);
	case SIGBUS:
	case SIGSEGV:
	case SIGILL:
	case SIGFPE:
		bt(signum, info->si_addr);
	}

	/* SIGCHLD */
}

void signals_setup()
{
	int sig[] = { SIGCHLD, SIGFPE, SIGILL, SIGSEGV, SIGBUS, SIGABRT };
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = sighandler;
	sa.sa_flags = SA_SIGINFO | SA_NOCLDWAIT;

	for (unsigned int i = 0; i < sizeof(sig) / sizeof(sig[0]); i++)
		sigaction(sig[i], &sa, NULL);

	signal(SIGPIPE, SIG_IGN);
}
