#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_LDRDLLNOTIFICATIONFUNCTION_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_LDRDLLNOTIFICATIONFUNCTION_H_
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/struct/ldrdllnotificationdata.h"
COSMOPOLITAN_C_START_

typedef void (*__msabi NtLdrDllNotificationFunction)(uint32_t Reason,
						     const union NtLdrDllNotificationData *Data,
						     void *opt_Context);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_LDRDLLNOTIFICATIONFUNCTION_H_ */
