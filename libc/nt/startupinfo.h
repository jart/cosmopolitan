#ifndef COSMOPOLITAN_LIBC_NT_NTSTARTUPINFO_H_
#define COSMOPOLITAN_LIBC_NT_NTSTARTUPINFO_H_
#include "libc/nt/struct/procthreadattributelist.h"
#include "libc/nt/struct/startupinfo.h"
#include "libc/nt/thunk/msabi.h"
COSMOPOLITAN_C_START_

#define kNtProcThreadAttributeParentProcess 0x00020000
#define kNtProcThreadAttributeHandleList    0x00020002

void GetStartupInfo(struct NtStartupInfo *lpStartupInfo);

bool32 InitializeProcThreadAttributeList(
    struct NtProcThreadAttributeList *opt_inout_lpAttributeList,
    uint32_t dwAttributeCount, uint32_t reserved_dwFlags, size_t *inout_lpSize);
bool32 UpdateProcThreadAttribute(
    struct NtProcThreadAttributeList *inout_lpAttributeList, uint32_t dwFlags,
    uint64_t Attribute, const void *lpValue, size_t cbSize,
    void *reserved_lpPreviousValue, size_t *reserved_lpReturnSize);
void DeleteProcThreadAttributeList(
    struct NtProcThreadAttributeList *inout_lpAttributeList);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/startupinfo.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_NTSTARTUPINFO_H_ */
