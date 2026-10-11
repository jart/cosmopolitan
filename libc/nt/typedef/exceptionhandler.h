#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_EXCEPTIONHANDLER_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_EXCEPTIONHANDLER_H_
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/struct/context.h"
#include "libc/nt/struct/exceptionframe.h"
#include "libc/nt/struct/ntexceptionrecord.h"
COSMOPOLITAN_C_START_

typedef unsigned (*__msabi NtExceptionHandler)(struct NtExceptionRecord *,
					       struct NtExceptionFrame *,
					       struct NtContext *, void *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_EXCEPTIONHANDLER_H_ */
