#ifndef COSMOPOLITAN_LIBC_THREAD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_THREAD_INTERNAL_H_
#include "libc/thread/thread.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern uint64_t _pthread_key_usage[(PTHREAD_KEYS_MAX + 63) / 64];
hidden extern pthread_key_dtor _pthread_key_dtor[PTHREAD_KEYS_MAX];
hidden extern _Thread_local void *_pthread_keys[PTHREAD_KEYS_MAX];

void _pthread_key_destruct(void *[PTHREAD_KEYS_MAX]);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_INTERNAL_H_ */
