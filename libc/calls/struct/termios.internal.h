#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TERMIOS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TERMIOS_INTERNAL_H_
#include "libc/calls/struct/termios.h"
#include "libc/mem/alloca.h"
COSMOPOLITAN_C_START_

const char *DescribeTermios(char[1024], ssize_t, const struct termios *);

#define DescribeTermios(rc, tio) DescribeTermios(alloca(1024), rc, tio)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TERMIOS_INTERNAL_H_ */
