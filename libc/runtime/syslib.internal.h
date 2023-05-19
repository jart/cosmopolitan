#ifndef COSMOPOLITAN_LIBC_RUNTIME_SYSLIB_H_
#define COSMOPOLITAN_LIBC_RUNTIME_SYSLIB_H_
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/thread/thread.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define SYSLIB_MAGIC   ('s' | 'l' << 8 | 'i' << 16 | 'b' << 24)
#define SYSLIB_VERSION 0

struct Syslib {
  int magic;
  int version;
  void (*exit)(int) wontreturn;
  long (*fork)(void);
  long (*read)(int, void *, size_t);
  long (*pread)(int, void *, size_t, int64_t);
  long (*readv)(int, const struct iovec *, int);
  long (*write)(int, const void *, size_t);
  long (*pwrite)(int, const void *, size_t, int64_t);
  long (*writev)(int, const struct iovec *, int);
  long (*openat)(int, const char *, int, ...);
  long (*pipe)(int[2]);
  long (*close)(int);
  long (*clock_gettime)(int, struct timespec *);
  long (*nanosleep)(const struct timespec *, struct timespec *);
  long (*mmap)(void *, size_t, int, int, int, int64_t);
  long (*sigaction)(int, const struct sigaction *restrict,
                    struct sigaction *restrict);
  int (*pthread_jit_write_protect_supported_np)(void);
  void (*pthread_jit_write_protect_np)(int);
  void (*sys_icache_invalidate)(void *, size_t);
  pthread_t (*pthread_self)(void);
  int (*pthread_create)(pthread_t *, const pthread_attr_t *, void *(*)(void *),
                        void *);
  int (*pthread_detach)(pthread_t);
  int (*pthread_join)(pthread_t, void **);
  void (*pthread_exit)(void *);
  int (*pthread_kill)(pthread_t, int);
  int (*pthread_sigmask)(int, const sigset_t *restrict, sigset_t *restrict);
  int (*pthread_setname_np)(const char *);
  int (*pthread_key_create)(pthread_key_t *, void (*)(void *));
  int (*pthread_setspecific)(pthread_key_t, const void *);
  void *(*pthread_getspecific)(pthread_key_t);
};

extern struct Syslib *__syslib;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_SYSLIB_H_ */
