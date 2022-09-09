#ifndef COSMOPOLITAN_LIBC_THREAD_THREAD_H_
#define COSMOPOLITAN_LIBC_THREAD_THREAD_H_
#include "libc/intrin/pthread.h"
#include "libc/runtime/runtime.h"
#include "libc/str/locale.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Ftrace {     /* 16 */
  bool once;        /*  0 */
  bool noreentry;   /*  1 */
  int skew;         /*  4 */
  int64_t lastaddr; /*  8 */
};

struct cthread_descriptor_t {
  struct cthread_descriptor_t *self;  /* 0x00 */
  struct Ftrace ftrace;               /* 0x08 */
  void *garbages;                     /* 0x18 */
  locale_t locale;                    /* 0x20 */
  pthread_t pthread;                  /* 0x28 */
  struct cthread_descriptor_t *self2; /* 0x30 */
  int32_t tid;                        /* 0x38 */
  int32_t err;                        /* 0x3c */
};

typedef struct cthread_descriptor_t *cthread_t;

void cthread_ungarbage(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_THREAD_H_ */
