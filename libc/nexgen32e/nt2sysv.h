#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_NT2SYSV_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_NT2SYSV_H_
#include "libc/nexgen32e/trampoline.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Creates function to thunk FUNCTION from MSX64 to System V ABI.
 *
 * This macro should be used when specifying callbacks in the WIN32 API.
 */
#define NT2SYSV(FUNCTION) TRAMPOLINE(FUNCTION, __nt2sysv)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_NT2SYSV_H_ */
