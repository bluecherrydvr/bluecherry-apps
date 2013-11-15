#ifndef BC_LOGC_H
#define BC_LOGC_H
#ifdef __cplusplus
extern "C" {
#endif

enum log_level {
	Debug,
	Info,
	Warning,
	Error,
	Bug,
	Fatal
};

/* Log a message to the context on the top of the stack for the current thread.
 * See bc_log_context_push for details.
 *
 * The minimum level of the context (or, if unset, the process default) will
 * be applied.
 *
 * Printf-style formatting is used.
 */
void bc_log(enum log_level l, const char *msg, ...)
	__attribute__ ((format (printf, 2, 3)));

#ifdef __cplusplus
}
#endif
#endif
