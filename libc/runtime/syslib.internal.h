#ifndef COSMOPOLITAN_LIBC_RUNTIME_SYSLIB_H_
#define COSMOPOLITAN_LIBC_RUNTIME_SYSLIB_H_
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/thread/thread.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview System DSO interfaces provided by APE loader.
 *
 * These functions are owned by the platform C library. Regardless of
 * platform, POSIX APIs returning `long` will follow the Linux Kernel
 * `-errno` convention, and hence should be wrapped with `_sysret()`.
 */

#define SYSLIB_MAGIC   ('s' | 'l' << 8 | 'i' << 16 | 'b' << 24)
#define SYSLIB_VERSION 1

typedef uint64_t dispatch_time_t;
typedef uint64_t dispatch_semaphore_t;

struct Syslib {
  int magic;
  int version;
  long (*fork)(void);
  long (*pipe)(int[2]);
  long (*clock_gettime)(int, struct timespec *);
  long (*nanosleep)(const struct timespec *, struct timespec *);
  long (*mmap)(void *, size_t, int, int, int, int64_t);
  int (*pthread_jit_write_protect_supported_np)(void);
  void (*pthread_jit_write_protect_np)(int);
  void (*sys_icache_invalidate)(void *, size_t);
  int (*pthread_create)(pthread_t *, const pthread_attr_t *, void *(*)(void *),
                        void *);
  void (*pthread_exit)(void *);
  int (*pthread_kill)(pthread_t, int);
  int (*pthread_sigmask)(int, const sigset_t *restrict, sigset_t *restrict);
  int (*pthread_setname_np)(const char *);
  dispatch_semaphore_t (*dispatch_semaphore_create)(long);
  long (*dispatch_semaphore_signal)(dispatch_semaphore_t);
  long (*dispatch_semaphore_wait)(dispatch_semaphore_t, dispatch_time_t);
  dispatch_time_t (*dispatch_walltime)(const struct timespec *, int64_t);
};

extern struct Syslib *__syslib;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_SYSLIB_H_ */
