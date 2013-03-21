#include "logging.h"
#include <syslog.h>
#include <pthread.h>
#include <vector>

extern char *__progname;
static log_context *bc_default_context = 0;
static pthread_key_t bc_log_thread_context;

#define DEFAULT_LOG_LEVEL Info

static void destroy_thread_context(void *vp)
{
	std::vector<log_context> *p = reinterpret_cast<std::vector<log_context>*>(vp);
	delete p;
}

struct log_context::data {
	// threadsafe only when read-only.
	const std::string name;
	log_level level;

	data(const std::string &n)
		: name(n), level((log_level)-1)
	{
	}
};

log_context &log_context::default_context()
{
	/* Not thread safe, but assumed to be safe at startup */
	if (!bc_default_context) {
		bc_default_context = new log_context("");
		bc_default_context->set_level(DEFAULT_LOG_LEVEL);
	}
	return *bc_default_context;
}

log_context::log_context()
	: d(default_context().d)
{
	d = std::shared_ptr<data>(new data(""));
	d->level = DEFAULT_LOG_LEVEL;
}

log_context::log_context(const char *name, ...)
{
	va_list args;
	va_start(args, name);
	char buf[128];
	vsnprintf(buf, sizeof(buf), name, args);
	va_end(args);

	d = std::shared_ptr<data>(new data(buf));
}

log_level log_context::level() const
{
	return d->level;
}

void log_context::set_level(log_level l)
{
	d->level = l;
}

bool log_context::level_check(log_level l) const
{
	log_level clvl = d->level < 0 ? default_context().level() : d->level;
	return (l >= clvl);
}

std::string log_context::name() const
{
	return d->name;
}

void log_context::log(log_level l, const char *msg, ...) const
{
	va_list args;
	va_start(args, msg);
	vlog(l, msg, args);
	va_end(args);
}

void log_context::vlog(log_level l, const char *msg, va_list args) const
{
	if (!level_check(l))
		return;

	char buf[1024];
	vsnprintf(buf, sizeof(buf), msg, args);
	server_log::write(l, d->name.c_str(), buf);
}

void server_log::open()
{
	openlog(__progname, LOG_PID | LOG_PERROR, LOG_DAEMON);
	pthread_key_create(&bc_log_thread_context, destroy_thread_context);
}

void server_log::write(log_level l, const char *context, const char *msg)
{
	char level = '?';
	switch (l) {
		case Debug: level = 'D'; break;
		case Info: level = 'I'; break;
		case Warning: level = 'W'; break;
		case Error: level = 'E'; break;
		case Bug: level = 'B'; break;
		case Fatal: level = 'F'; break;
	}

	syslog(LOG_INFO | LOG_DAEMON, "%c(%s): %s", level, context, msg);
}

const log_context &bc_log_context()
{
	std::vector<log_context> *p = reinterpret_cast<std::vector<log_context>*>(
			pthread_getspecific(bc_log_thread_context));
	if (p && !p->empty())
		return p->back();
	return log_context::default_context();
}

void bc_log(log_level l, const char *msg, ...)
{
	const log_context &context = bc_log_context();
	va_list args;
	va_start(args, msg);
	context.vlog(l, msg, args);
	va_end(args);
}

void bc_log_context_push(const log_context &context)
{
	std::vector<log_context> *p = reinterpret_cast<std::vector<log_context>*>(
			pthread_getspecific(bc_log_thread_context));

	if (!p) {
		p = new std::vector<log_context>;
		pthread_setspecific(bc_log_thread_context, reinterpret_cast<void*>(p));
	}

	p->push_back(context);
}

void bc_log_context_pop()
{
	std::vector<log_context> *p = reinterpret_cast<std::vector<log_context>*>(
			pthread_getspecific(bc_log_thread_context));
	if (p && !p->empty())
		p->pop_back();
}

