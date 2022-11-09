#ifndef COSMOPOLITAN_LIBC_THREAD_TLS_H_
#define COSMOPOLITAN_LIBC_THREAD_TLS_H_

#define TLS_ALIGNMENT 64

#define TIB_FLAG_TIME_CRITICAL 1
#define TIB_FLAG_VFORKED       2
#define TIB_FLAG_WINCRASHING   4

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct CosmoFtrace {   /* 16 */
  bool ft_once;        /*  0 */
  bool ft_noreentry;   /*  1 */
  int ft_skew;         /*  4 */
  int64_t ft_lastaddr; /*  8 */
};

struct CosmoTib {
  struct CosmoTib *tib_self;      /* 0x00 */
  struct CosmoFtrace tib_ftracer; /* 0x08 */
  void *tib_garbages;             /* 0x18 */
  intptr_t tib_locale;            /* 0x20 */
  intptr_t tib_pthread;           /* 0x28 */
  struct CosmoTib *tib_self2;     /* 0x30 */
  _Atomic(int32_t) tib_tid;       /* 0x38 transitions -1 → tid → 0 */
  int32_t tib_errno;              /* 0x3c */
  uint64_t tib_flags;             /* 0x40 */
  void *tib_nsync;
  int tib_ftrace;       /* inherited */
  int tib_strace;       /* inherited */
  uint64_t tib_sigmask; /* inherited */
  void *tib_reserved4;
  void *tib_reserved5;
  void *tib_reserved6;
  void *tib_reserved7;
  void *tib_keys[128];
};

extern int __threaded;
extern bool __tls_enabled;
extern unsigned __tls_index;

void __require_tls(void);
void __set_tls(struct CosmoTib *);

/**
 * Returns location of thread information block.
 *
 * This can't be used in privileged functions.
 */
#define __get_tls()                                                \
  ({                                                               \
    struct CosmoTib *_t;                                           \
    asm("mov\t%%fs:0,%0" : "=r"(_t) : /* no inputs */ : "memory"); \
    _t;                                                            \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_TLS_H_ */
