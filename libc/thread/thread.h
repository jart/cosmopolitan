#ifndef COSMOPOLITAN_LIBC_THREAD_THREAD_H_
#define COSMOPOLITAN_LIBC_THREAD_THREAD_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview thread
 */

typedef struct thread_descriptor_t {
    struct thread_descriptor_t* self; // mandatory for TLS
    void* stack;
} thread_descriptor_t;

thread_descriptor_t* allocate_thread();
long start_thread(thread_descriptor_t*, int (*)(void*), void*);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_THREAD_H_ */
