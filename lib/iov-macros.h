#ifndef BC_IOV_MACROS_H
#define BC_IOV_MACROS_H

#define VSET(v, base, len) {		\
	struct iovec *__v = &(v);	\
	__v->iov_base = (void *)(base); \
	__v->iov_len = (len);		\
}
#define VSTR(v, str) VSET((v), (str), strlen(str))
#define VBUF(v, buf) VSET((v), (buf), sizeof(buf))

#endif
