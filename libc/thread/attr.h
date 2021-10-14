#ifndef COSMOPOLITAN_LIBC_THREAD_ATTR_H_
#define COSMOPOLITAN_LIBC_THREAD_ATTR_H_

#define CTHREAD_CREATE_DETACHED 1
#define CTHREAD_CREATE_JOINABLE 0

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview cosmopolitan thread attributes
 */

typedef struct cthread_attr_t {
  size_t stacksize, guardsize;
  int mode;
} cthread_attr_t;

int cthread_attr_init(cthread_attr_t*);
int cthread_attr_destroy(cthread_attr_t*);
int cthread_attr_setstacksize(cthread_attr_t*, size_t);
size_t thread_attr_getstacksize(const cthread_attr_t*);
int cthread_attr_setguardsize(cthread_attr_t*, size_t);
size_t cthread_attr_getguardsize(const cthread_attr_t*);
int cthread_attr_setdetachstate(cthread_attr_t*, int);
int cthread_attr_getdetachstate(const cthread_attr_t*);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_ATTR_H_ */
