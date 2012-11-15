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
	static void open();
	static void write(log_level l, const char *context, const char *msg);
};

class log_context
{
public:
	static const log_context &default_context();

	log_context();
	log_context(const char *name, ...) __attribute__ ((format (printf, 2, 3)));

	log_level level() const;
	void set_level(log_level level);
	bool test_level(log_level level) const;

	std::string name() const;

	void log(log_level level, const char *msg, ...) const __attribute__ ((format (printf, 3, 4)));
	void vlog(log_level level, const char *msg, va_list args) const;

private:
	struct data;
	std::shared_ptr<data> d;
};

void bc_log(log_level l, const char *msg, ...) __attribute__ ((format (printf, 2, 3)));
const log_context &bc_log_context();
log_context &bc_log_context_default();
void bc_log_context_push(const log_context &context);
void bc_log_context_pop();

#endif

