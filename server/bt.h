#ifndef BC_BT_H
#define BC_BT_H
#ifdef __cplusplus
extern "C" {
#endif

#define RET_ADDR (__builtin_extract_return_addr(__builtin_return_address(0)))

void bt(const char *err, const void *p);

#ifdef __cplusplus
}
#endif
#endif
