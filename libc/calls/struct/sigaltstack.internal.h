#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_INTERNAL_H_
#include "libc/calls/struct/sigaltstack.h"
#include "libc/mem/alloca.h"
COSMOPOLITAN_C_START_

const char *_DescribeSigaltstack(char[128], int, const struct sigaltstack *);
#define DescribeSigaltstack(rc, ss) _DescribeSigaltstack(alloca(128), rc, ss)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_INTERNAL_H_ */
