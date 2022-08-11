#ifndef COSMOPOLITAN_LIBC_NT_TEB_H_
#define COSMOPOLITAN_LIBC_NT_TEB_H_
#include "libc/intrin/segmentation.h"
#include "libc/nt/struct/peb.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

/*
 * These macros address directly into NT's TEB a.k.a. TIB
 * Any function that does this needs the `noasan` keyword
 */
#define NtGetPeb()           gs((struct NtPeb **)(0x60ULL))
#define NtGetTeb()           gs((void **)(0x30))    /* %gs:0 linear address */
#define NtGetPid()           gs((uint32_t *)(0x40)) /* GetCurrentProcessId() */
#define NtGetTid()           gs((uint32_t *)(0x48)) /* GetCurrentThreadId() */
#define NtGetErr()           gs((int *)(0x68))
#define _NtGetSeh()          gs((void **)(0x00))
#define _NtGetStackHigh()    gs((void **)(0x08))
#define _NtGetStackLow()     gs((void **)(0x10))
#define _NtGetSubsystemTib() gs((void **)(0x18))
#define _NtGetFib()          gs((void **)(0x20))
#define _NtGetEnv()          gs((char16_t **)(0x38))
#define _NtGetRpc()          gs((void **)(0x50))
#define _NtGetTls()          gs((void **)(0x58)) /* cf. gs((long *)0x1480 + i0..64) */

#endif /* __GNUC__ && !__STRICT_ANSI__ */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_TEB_H_ */
