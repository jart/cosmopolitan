#ifndef COSMOPOLITAN_LIBC_INTRIN_RLIMIT_H_
#define COSMOPOLITAN_LIBC_INTRIN_RLIMIT_H_
#include "libc/calls/struct/rlimit.h"
COSMOPOLITAN_C_START_

void __rlimit_stack_set(struct rlimit);
struct rlimit __rlimit_stack_get(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_RLIMIT_H_ */
