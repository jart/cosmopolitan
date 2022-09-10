#ifndef COSMOPOLITAN_LIBC_STDIO_LOCK_H_
#define COSMOPOLITAN_LIBC_STDIO_LOCK_H_
#include "libc/intrin/nopl.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/thread/tls.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#ifdef _NOPL1
#define flockfile(f)    _NOPL1("__threadcalls", flockfile, f)
#define funlockfile(f)  _NOPL1("__threadcalls", funlockfile, f)
#define ftrylockfile(f) _NOPL1("__threadcalls", ftrylockfile, f)
#else
#define flockfile(f)    (__threaded ? flockfile(f) : 0)
#define funlockfile(f)  (__threaded ? funlockfile(f) : 0)
#define ftrylockfile(f) (__threaded ? ftrylockfile(f) : 0)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_LOCK_H_ */
