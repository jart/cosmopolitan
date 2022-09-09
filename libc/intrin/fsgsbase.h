#ifndef COSMOPOLITAN_LIBC_RUNTIME_FSGSBASE_H_
#define COSMOPOLITAN_LIBC_RUNTIME_FSGSBASE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void *_rdfsbase(void);
void *_rdgsbase(void);
void *_wrfsbase(void *);
void *_wrgsbase(void *);
int _have_fsgsbase(void);

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define _rdfsbase()                 \
  ({                                \
    void *_p;                       \
    asm("rdfsbase\t%0" : "=r"(_p)); \
    _p;                             \
  })
#define _rdgsbase()                 \
  ({                                \
    void *_p;                       \
    asm("rdgsbase\t%0" : "=r"(_p)); \
    _p;                             \
  })
#define _wrfsbase(p)                \
  ({                                \
    void *_p = p;                   \
    asm volatile("wrfsbase\t%0"     \
                 : /* no outputs */ \
                 : "r"(_p)          \
                 : "memory");       \
    _p;                             \
  })
#define _wrgsbase(p)                \
  ({                                \
    void *_p = p;                   \
    asm volatile("wrgsbase\t%0"     \
                 : /* no outputs */ \
                 : "r"(_p)          \
                 : "memory");       \
    _p;                             \
  })
#endif /* GNUC && !ANSI */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_FSGSBASE_H_ */
