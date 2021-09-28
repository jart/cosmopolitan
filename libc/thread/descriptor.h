#ifndef COSMOPOLITAN_LIBC_THREAD_DESCRIPTOR_H_
#define COSMOPOLITAN_LIBC_THREAD_DESCRIPTOR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview thread types
 */

struct cthread_descriptor_t {
    struct cthread_descriptor_t* self; // mandatory for TLS
    struct {
      void *top, *bottom;
    } stack, tls, alloc;
    int tid;
};

typedef struct cthread_descriptor_t* cthread_t;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_DESCRIPTOR_H_ */
