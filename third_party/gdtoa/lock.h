#ifndef COSMOPOLITAN_THIRD_PARTY_GDTOA_LOCK_H_
#define COSMOPOLITAN_THIRD_PARTY_GDTOA_LOCK_H_
#include "libc/intrin/nopl.internal.h"
#include "libc/thread/tls.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int __gdtoa_lock(void);
int __gdtoa_unlock(void);
int __gdtoa_lock1(void);
int __gdtoa_unlock1(void);

#ifdef _NOPL0
#define __gdtoa_lock()    _NOPL0("__threadcalls", __gdtoa_lock)
#define __gdtoa_unlock()  _NOPL0("__threadcalls", __gdtoa_unlock)
#define __gdtoa_lock1()   _NOPL0("__threadcalls", __gdtoa_lock1)
#define __gdtoa_unlock1() _NOPL0("__threadcalls", __gdtoa_unlock1)
#else
#define __gdtoa_lock()    (__threaded ? __gdtoa_lock() : 0)
#define __gdtoa_unlock()  (__threaded ? __gdtoa_unlock() : 0)
#define __gdtoa_lock1()   (__threaded ? __gdtoa_lock1() : 0)
#define __gdtoa_unlock1() (__threaded ? __gdtoa_unlock1() : 0)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_GDTOA_LOCK_H_ */
