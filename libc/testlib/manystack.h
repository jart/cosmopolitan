#ifndef COSMOPOLITAN_LIBC_TESTLIB_MANYSTACK_H_
#define COSMOPOLITAN_LIBC_TESTLIB_MANYSTACK_H_
#include "libc/thread/thread.h"
COSMOPOLITAN_C_START_

pthread_t manystack_start(void);
void manystack_stop(pthread_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TESTLIB_MANYSTACK_H_ */
