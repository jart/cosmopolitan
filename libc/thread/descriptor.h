#ifndef COSMOPOLITAN_LIBC_THREAD_DESCRIPTOR_H_
#define COSMOPOLITAN_LIBC_THREAD_DESCRIPTOR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

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
  struct cthread_descriptor_t* self; /* mandatory for TLS */
  struct {
    void *top, *bottom;
  } stack, tls, alloc;
  int state;
  int tid;
  int rc;
  void* pthread_ret_ptr;
};

typedef struct cthread_descriptor_t* cthread_t;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_DESCRIPTOR_H_ */
