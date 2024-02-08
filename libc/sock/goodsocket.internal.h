#ifndef COSMOPOLITAN_LIBC_SOCK_GOODSOCKET_H_
#define COSMOPOLITAN_LIBC_SOCK_GOODSOCKET_H_
#include "libc/calls/struct/timeval.h"
COSMOPOLITAN_C_START_

int GoodSocket(int, int, int, bool, const struct timeval *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_GOODSOCKET_H_ */
