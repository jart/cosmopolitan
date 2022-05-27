#ifndef COSMOPOLITAN_LIBC_THREAD_SELF_H_
#define COSMOPOLITAN_LIBC_THREAD_SELF_H_
#include "libc/nexgen32e/threaded.h"
#include "libc/thread/descriptor.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const void *const _main_thread_ctor[];

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define cthread_self()        \
  ({                          \
    YOINK(_main_thread_ctor); \
    ((cthread_t)__get_tls()); \
  })
#else
cthread_t cthread_self(void);
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_SELF_H_ */
