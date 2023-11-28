#ifndef COSMOPOLITAN_LIBC_NT_ENUM_THREADACCESS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_THREADACCESS_H_
#include "libc/nt/enum/accessmask.h"

#define kNtThreadTerminate 0x0001
#define kNtThreadSuspendResume 0x0002
#define kNtThreadGetContext 0x0008
#define kNtThreadSetContext 0x0010
#define kNtThreadQueryInformation 0x0040
#define kNtThreadSetInformation 0x0020
#define kNtThreadSetThreadToken 0x0080
#define kNtThreadImpersonate 0x0100
#define kNtThreadDirectImpersonation 0x0200
#define kNtThreadSetLimitedInformation 0x0400
#define kNtThreadQueryLimitedInformation 0x0800
#define kNtThreadResume 0x1000
#define kNtThreadAllAccess (kNtStandardRightsRequired | kNtSynchronize | 0xFFFF)

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_THREADACCESS_H_ */
