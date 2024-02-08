#ifndef COSMOPOLITAN_LIBC_SOCK_SELECT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_SELECT_INTERNAL_H_
#include "libc/mem/alloca.h"
#include "libc/sock/select.h"
COSMOPOLITAN_C_START_

const char *DescribeFdSet(char[100], ssize_t, int, fd_set *) libcesque;
#define DescribeFdSet(x, y, z) DescribeFdSet(alloca(100), x, y, z)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_SELECT_INTERNAL_H_ */
