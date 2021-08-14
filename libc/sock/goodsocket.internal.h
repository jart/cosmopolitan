#ifndef COSMOPOLITAN_LIBC_SOCK_GOODSOCKET_H_
#define COSMOPOLITAN_LIBC_SOCK_GOODSOCKET_H_
#include "libc/calls/struct/timeval.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int GoodSocket(int, int, int, bool, const struct timeval *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_GOODSOCKET_H_ */
