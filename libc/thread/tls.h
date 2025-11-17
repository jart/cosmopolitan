#ifndef COSMOPOLITAN_LIBC_THREAD_TLS_H_
#define COSMOPOLITAN_LIBC_THREAD_TLS_H_
#include "libc/dce.h"

#define TLS_ALIGNMENT 64

#define TIB_FLAG_VFORKED 1
#define TIB_FLAG_SPAWNED 2

#ifdef MODE_DBG
#define PTHREAD_KEYS_FAST 0
#else
#define PTHREAD_KEYS_FAST 37
#endif

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct CosmoFtrace {   /* 16 */
  char ft_once;        /*  0 */
  int ft_skew;         /*  4 */
  int64_t ft_lastaddr; /*  8 */
};

/* cosmopolitan thread information block (1024 bytes) */
/* NOTE: update aarch64 libc/errno.h if sizeof changes */
/* NOTE: update aarch64 libc/proc/vfork.S if sizeof changes */
/* NOTE: update aarch64 libc/nexgen32e/gc.S if sizeof changes */
/* NOTE: update aarch64 libc/thread/mktls.c if sizeof changes */
struct CosmoTib {
  struct CosmoTib *tib_self;      /* 0x00 */
  struct CosmoFtrace tib_ftracer; /* 0x08 */
  void *(*tib_malloc)(void);      /* 0x18 */
  _Atomic(int32_t) tib_ptid;      /* 0x20 transitions 0 → tid */
  intptr_t tib_pthread;           /* 0x28 */
  struct CosmoTib *tib_self2;     /* 0x30 */
  _Atomic(int32_t) tib_ctid;      /* 0x38 transitions -1 → tid → 0 */
  int32_t tib_errno;              /* 0x3c */
  uint64_t tib_flags;             /* 0x40 */
  void *tib_garbages;             /* 0x48 */
  int tib_ftrace;                 /* inherited */
  int tib_strace;                 /* inherited */
  _Atomic(uint64_t) tib_sigmask;  /* inherited */
  _Atomic(uint64_t) tib_sigpending;
  _Atomic(uint64_t) tib_syshand; /* win32=kThread, xnusilicon=pthread_t */
  char *tib_sigstack_addr;
  uint32_t tib_sigstack_size;
  uint32_t tib_sigstack_flags;
  void *tib_nsync;
  void *tib_atexit;
  intptr_t tib_events[2];
  void *tib_sigjmpbuf;
  void *tib_vfork;
  void *tib_keys_static[37];
  void **tib_keys_dynamic;
  void *tib_locks[64];
  char tib_rseq[32];
} __attribute__((__aligned__(64)));

extern unsigned __tls_index;

char *_mktls(struct CosmoTib **) libcesque;
void __bootstrap_tls(struct CosmoTib *, char *) libcesque;

#ifdef __x86_64__
extern char __tls_enabled;
#define __tls_enabled_set(x) __tls_enabled = x
#elif defined(__aarch64__)
#define __tls_enabled        true
#define __tls_enabled_set(x) (void)0
#else
#error "unsupported architecture"
#endif

void __set_tls(struct CosmoTib *) libcesque;
struct CosmoTib *__get_tls_rax(void) dontthrow pureconst;

#if defined(__x86_64__) && (SupportsWindows() || SupportsXnu())
#define __get_tls_privileged() __get_tls_rax()
#else
#define __get_tls_privileged() __get_tls()
#endif

/**
 * Returns location of thread information block.
 *
 * This can't be used in privileged functions.
 */
forceinline pureconst struct CosmoTib *__get_tls(void) {
#ifdef __chibicc__
  return __get_tls_rax();
#elif __x86_64__
  struct CosmoTib *__tib;
  __asm__("movq\t%%fs:0, %0" : "=r"(__tib));
  return __tib;
#elif defined(__aarch64__)
  register struct CosmoTib *__tls __asm__("x28");
  return __tls - 1;
#endif
}

forceinline pureconst struct CosmoTib *__get_tls_win32(void) {
#ifdef __x86_64__
  struct CosmoTib *tib;
  __asm__("mov\t%%gs:0x1480(,%1,8),%0"
          : "=r"(tib)
          : "r"((long)__tls_index)
          : "memory");
  return tib;
#else
  return 0;
#endif
}

forceinline void __set_tls_win32(void *tls) {
#ifdef __x86_64__
  __asm__ volatile("mov\t%1,%%gs:0x1480(,%0,8)"
                   : /* no outputs */
                   : "r"((long)__tls_index), "r"(tls)
                   : "memory");
#endif
}

#ifdef __x86_64__
#define __adj_tls(tib) (tib)
#elif defined(__aarch64__)
#define __adj_tls(tib) ((struct CosmoTib *)(tib) + 1)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_TLS_H_ */
