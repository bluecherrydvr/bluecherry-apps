#ifndef LOGGING_H
#define LOGGING_H

#include <memory>
#include <string>
#include <cstdarg>

enum log_level {
	Debug,
	Info,
	Warning,
	Error,
	Bug,
	Fatal
};

class server_log
{
public:
	/* Open the log for this process; to be called once on startup */
	static void open();
	/* Write the given message to the log, prefixed with level and context.
	 * No other formatting is applied, and no minimum level is enforced.
	 * Threadsafe. */
	static void write(log_level l, const char *context, const char *msg);
};

class log_context
{
public:
	static log_context &default_context();

	/* Create an instance of the default context, which shares its data and
	 * level. */
	log_context();
	/* Create a context with the given string as name. Printf-style formatting
	 * is applied for convenience. */
	log_context(const char *name, ...) __attribute__ ((format (printf, 2, 3)));

	/* Minimum log level to be emitted for this context; may be changed and 
	 * tested thread-safely. Overrides the default log level. */
	log_level level() const __attribute__((pure));
	void set_level(log_level level);
	bool test_level(log_level level) const;

	std::string name() const;

	/* Log a message to this context with the given level and printf-style formatting */
	void log(log_level level, const char *msg, ...) const __attribute__ ((format (printf, 3, 4)));
	/* Variable-argument overload of log() */
	void vlog(log_level level, const char *msg, va_list args) const;

private:
	struct data;
	std::shared_ptr<data> d;
};

/* Log a message to the context on the top of the stack for the current thread.
 * See bc_log_context_push for details.
 *
 * The minimum level of the context (or, if unset, the process default) will be applied. 
 * Printf-style formatting is used. */
void bc_log(log_level l, const char *msg, ...) __attribute__ ((format (printf, 2, 3)));
/* Returns the log_context at the top of the stack for the current thread,
 * as would be used by bc_log.
 *
 * If no contexts are on the stack for this thread, returns the default context. */
const log_context &bc_log_context();
/* Push the context to the stack for the current thread, to send subsequent bc_log calls
 * to that context. Must be matched by a call to bc_log_context_pop, unless the context
 * will remain applicable for the lifetime of the thread.
 *
 * Threadsafe. */
void bc_log_context_push(const log_context &context);
void bc_log_context_pop();

#endif

