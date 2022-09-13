#ifndef COSMOPOLITAN_LIBC_THREAD_TLS_H_
#define COSMOPOLITAN_LIBC_THREAD_TLS_H_
#include "libc/thread/thread.h"

#define TLS_ALIGNMENT 64

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct CosmoFtrace {   /* 16 */
  bool ft_once;        /*  0 */
  bool ft_noreentry;   /*  1 */
  int ft_skew;         /*  4 */
  int64_t ft_lastaddr; /*  8 */
};

struct CosmoTib {
  struct CosmoTib *tib_self;     /* 0x00 */
  struct CosmoFtrace tib_ftrace; /* 0x08 */
  void *tib_garbages;            /* 0x18 */
  intptr_t tib_locale;           /* 0x20 */
  intptr_t tib_pthread;          /* 0x28 */
  struct CosmoTib *tib_self2;    /* 0x30 */
  _Atomic(int32_t) tib_tid;      /* 0x38 */
  int32_t tib_errno;             /* 0x3c */
  void *tib_nsync;
  void *tib_reserved1;
  void *tib_reserved2;
  void *tib_reserved3;
  void *tib_reserved4;
  void *tib_reserved5;
  void *tib_reserved6;
  void *tib_reserved7;
  void *tib_keys[PTHREAD_KEYS_MAX];
};

extern int __threaded;
extern bool __tls_enabled;
extern unsigned __tls_index;

void __require_tls(void);

#if defined(__GNUC__) && defined(__x86_64__) && !defined(__STRICT_ANSI__)
/**
 * Returns location of thread information block.
 *
 * This can't be used in privileged functions.
 */
static inline struct CosmoTib *__get_tls(void) {
  struct CosmoTib *_tib;
  asm("mov\t%%fs:0,%0" : "=r"(_tib) : /* no inputs */ : "memory");
  return _tib;
}
#endif /* GNU x86-64 */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_TLS_H_ */
