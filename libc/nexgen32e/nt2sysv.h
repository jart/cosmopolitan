#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_NT2SYSV_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_NT2SYSV_H_
#include "libc/nexgen32e/trampoline.h"

/**
 * Creates function to thunk FUNCTION from MSX64 to System V ABI.
 *
 * This macro should be used when specifying callbacks in the WIN32 API.
 */
#define NT2SYSV(FUNCTION) TRAMPOLINE(FUNCTION, __nt2sysv)

#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_NT2SYSV_H_ */
