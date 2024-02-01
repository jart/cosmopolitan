#ifndef COSMOPOLITAN_LIBC_NT_TEB_H_
#define COSMOPOLITAN_LIBC_NT_TEB_H_
#include "libc/nt/struct/peb.h"
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

/*
 * These macros address directly into NT's TEB a.k.a. TIB
 * Any function that does this needs the `dontasan` keyword
 */
#define NtGetPeb()           ((__seg_gs struct NtPeb *)0x60)
#define NtGetTeb()           ((void *)*(__seg_gs uintptr_t *)0x30)
#define NtGetPid()           (*(__seg_gs uint32_t *)0x40)
#define NtGetTid()           (*(__seg_gs uint32_t *)0x48)
#define NtGetErr()           (*(__seg_gs int *)0x68)
#define _NtGetSeh()          ((void *)*(__seg_gs uintptr_t *)0x00)
#define _NtGetStackHigh()    ((void *)*(__seg_gs uintptr_t *)0x08)
#define _NtGetStackLow()     ((void *)*(__seg_gs uintptr_t *)0x10)
#define _NtGetSubsystemTib() ((void *)*(__seg_gs uintptr_t *)0x18)
#define _NtGetFib()          ((void *)*(__seg_gs uintptr_t *)0x20)
#define _NtGetEnv()          ((char16_t *)*(__seg_gs intptr_t *)0x38)
#define _NtGetRpc()          ((void *)*(__seg_gs uintptr_t *)0x50)
#define _NtGetTls()          ((void *)*(__seg_gs uintptr_t *)0x58)

#endif /* __GNUC__ && !__STRICT_ANSI__ */
#endif /* COSMOPOLITAN_LIBC_NT_TEB_H_ */
