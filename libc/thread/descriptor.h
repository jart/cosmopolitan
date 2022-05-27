#ifndef COSMOPOLITAN_LIBC_THREAD_DESCRIPTOR_H_
#define COSMOPOLITAN_LIBC_THREAD_DESCRIPTOR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
#include "libc/runtime/runtime.h"

/**
 * @fileoverview thread types
 */

enum cthread_state {
  cthread_started = 0,
  cthread_joining = 1,
  cthread_finished = 2,
  cthread_detached = 4,
  cthread_main = 127,
};

struct cthread_descriptor_t {
  struct cthread_descriptor_t *self;  /* 0x00 */
  void *(*func)(void *);              /* 0x08 */
  int32_t __pad0;                     /* 0x10 */
  int32_t state;                      /* 0x14 */
  void *arg;                          /* 0x18 */
  void *pthread_ret_ptr;              /* 0x20 */
  int64_t __pad1;                     /* 0x28 */
  struct cthread_descriptor_t *self2; /* 0x30 */
  int32_t tid;                        /* 0x38 */
  int32_t err;                        /* 0x3c */
  void *exitcode;
  struct {
    char *top, *bottom;
  } stack, alloc;
  jmp_buf exiter;
};

typedef struct cthread_descriptor_t *cthread_t;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_DESCRIPTOR_H_ */
