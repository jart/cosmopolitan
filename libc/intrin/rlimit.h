#ifndef COSMOPOLITAN_LIBC_INTRIN_RLIMIT_H_
#define COSMOPOLITAN_LIBC_INTRIN_RLIMIT_H_
#include "libc/calls/struct/rlimit.h"
COSMOPOLITAN_C_START_

#define RLIMIT_VAR_BUF_SIZE (14 + (1 + 20) * RLIM_NLIMITS * 2 + 1)

void __rlimit_launch(void);
void __rlimit_serialize(char[RLIMIT_VAR_BUF_SIZE]);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_RLIMIT_H_ */
