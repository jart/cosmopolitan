#ifndef COSMOPOLITAN_LIBC_THREAD_TLS2_H_
#define COSMOPOLITAN_LIBC_THREAD_TLS2_H_
#include "libc/dce.h"
#include "libc/thread/tls.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
#if defined(__GNUC__) && defined(__x86_64__) && !defined(__STRICT_ANSI__)

/**
 * Returns location of thread information block.
 *
 * This should be favored over __get_tls() for .privileged code that
 * can't be self-modified by __enable_tls().
 */
static noasan inline struct CosmoTib *__get_tls_privileged(void) {
  char *tib, *lin = (char *)0x30;
  if (IsLinux() || IsFreebsd() || IsNetbsd() || IsOpenbsd() || IsMetal()) {
    asm("mov\t%%fs:(%1),%0" : "=a"(tib) : "r"(lin) : "memory");
  } else {
    asm("mov\t%%gs:(%1),%0" : "=a"(tib) : "r"(lin) : "memory");
    if (IsWindows()) {
      tib = *(char **)(tib + 0x1480 + __tls_index * 8);
    }
  }
  return (struct CosmoTib *)tib;
}

static noasan inline struct CosmoTib *__get_tls_win32(void) {
  char *tib, *lin = (char *)0x30;
  asm("mov\t%%gs:(%1),%0" : "=a"(tib) : "r"(lin) : "memory");
  tib = *(char **)(tib + 0x1480 + __tls_index * 8);
  return (struct CosmoTib *)tib;
}

static noasan inline void __set_tls_win32(void *tls) {
  asm("mov\t%1,%%gs:%0" : "=m"(*((long *)0x1480 + __tls_index)) : "r"(tls));
}

#endif /* GNU x86-64 */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_TLS2_H_ */
