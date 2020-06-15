#ifndef COSMOPOLITAN_LIBC_NT_ENUM_PROCESSACCESS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_PROCESSACCESS_H_
#include "libc/nt/enum/accessmask.h"

#define kNtProcessCreateProcess 0x0080u
#define kNtProcessCreateThread 0x0002u
#define kNtProcessDupHandle 0x0040u
#define kNtProcessQueryInformation 0x0400u
#define kNtProcessQueryLimitedInformation 0x1000u
#define kNtProcessSetInformation 0x0200u
#define kNtProcessSetQuota 0x0100u
#define kNtProcessSuspendResume 0x0800u
#define kNtProcessTerminate 0x0001u
#define kNtProcessVmOperation 0x0008u
#define kNtProcessVmRead 0x0010u
#define kNtProcessVmWrite 0x0020u
#define kNtProcessSynchronize kNtSynchronize
#define kNtProcessAllAccess \
  (kNtStandardRightsRequired | kNtSynchronize | 0xffffu)

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_PROCESSACCESS_H_ */
