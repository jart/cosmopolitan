#ifndef COSMOPOLITAN_LIBC_THREAD_THREAD_H_
#define COSMOPOLITAN_LIBC_THREAD_THREAD_H_
#include "libc/calls/struct/timespec.h"
#include "libc/intrin/pthread.h"
#include "libc/runtime/runtime.h"

#define CTHREAD_CREATE_DETACHED 1
#define CTHREAD_CREATE_JOINABLE 0

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

enum cthread_state {
  cthread_started = 0,
  cthread_joining = 1,
  cthread_finished = 2,
  cthread_detached = 4,
};

struct FtraceTls {  /* 16 */
  bool once;        /*  0 */
  bool noreentry;   /*  1 */
  int skew;         /*  4 */
  int64_t lastaddr; /*  8 */
};

struct cthread_descriptor_t {
  struct cthread_descriptor_t *self;  /* 0x00 */
  struct FtraceTls ftrace;            /* 0x08 */
  void *garbages;                     /* 0x10 */
  int64_t __pad1;                     /* 0x20 */
  int64_t __pad2;                     /* 0x28 */
  struct cthread_descriptor_t *self2; /* 0x30 */
  int32_t tid;                        /* 0x38 */
  int32_t err;                        /* 0x3c */
};

typedef struct cthread_descriptor_t *cthread_t;

typedef union cthread_sem_t {
  struct {
    uint64_t count;
  } linux;
} cthread_sem_t;

typedef struct cthread_attr_t {
  size_t stacksize, guardsize;
  int mode;
} cthread_attr_t;

cthread_t cthread_self(void);
int cthread_attr_init(cthread_attr_t *);
int cthread_attr_destroy(cthread_attr_t *);
int cthread_attr_setstacksize(cthread_attr_t *, size_t);
size_t thread_attr_getstacksize(const cthread_attr_t *);
int cthread_attr_setguardsize(cthread_attr_t *, size_t);
size_t cthread_attr_getguardsize(const cthread_attr_t *);
int cthread_attr_setdetachstate(cthread_attr_t *, int);
int cthread_attr_getdetachstate(const cthread_attr_t *);
int cthread_sem_init(cthread_sem_t *, int);
int cthread_sem_destroy(cthread_sem_t *);
int cthread_sem_wait(cthread_sem_t *, int, const struct timespec *);
int cthread_sem_signal(cthread_sem_t *);
int cthread_memory_wait32(int *, int, const struct timespec *);
int cthread_memory_wake32(int *, int);
void cthread_zombies_add(cthread_t);
void cthread_zombies_reap(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_THREAD_H_ */
