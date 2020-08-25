#ifndef COSMOPOLITAN_LIBC_RUNTIME_SYSCONF_H_
#define COSMOPOLITAN_LIBC_RUNTIME_SYSCONF_H_
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"

#define _SC_ARG_MAX   0
#define _SC_CLK_TCK   2
#define _SC_PAGESIZE  30
#define _SC_PAGE_SIZE 30

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

long sysconf(int);

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define sysconf(X) __sysconf(X)
forceinline long __sysconf(int thing) {
  switch (thing) {
    case _SC_ARG_MAX:
      return ARG_MAX;
    case _SC_CLK_TCK: {
      extern const long __AT_CLKTCK asm("AT_CLKTCK");
      long res = getauxval(__AT_CLKTCK);
      if (!res) res = 100;
      return res;
    }
    case _SC_PAGESIZE:
      return FRAMESIZE;
    default:
      return -1;
  }
}
#endif /* GNU && !ANSI */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_SYSCONF_H_ */
