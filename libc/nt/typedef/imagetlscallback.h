#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_IMAGETLSCALLBACK_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_IMAGETLSCALLBACK_H_
#include "libc/nt/thunk/msabi.h"
COSMOPOLITAN_C_START_

typedef void (*__msabi NtImageTlsCallback)(void *DllHandle, uint32_t Reason,
					   void *Reserved);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_IMAGETLSCALLBACK_H_ */
