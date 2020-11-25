#ifndef COSMOPOLITAN_LIBC_NT_NTSTARTUPINFO_H_
#define COSMOPOLITAN_LIBC_NT_NTSTARTUPINFO_H_
#include "libc/nt/struct/procthreadattributelist.h"
#include "libc/nt/struct/startupinfo.h"
#include "libc/nt/thunk/msabi.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void GetStartupInfo(struct NtStartupInfo *lpStartupInfo);

bool32 InitializeProcThreadAttributeList(
    struct NtProcThreadAttributeList *opt_inout_lpAttributeList,
    uint32_t dwAttributeCount, uint32_t reserved_dwFlags, size_t *inout_lpSize)
    paramsnonnull((4));
bool32 UpdateProcThreadAttribute(
    struct NtProcThreadAttributeList *inout_lpAttributeList, uint32_t dwFlags,
    const uint32_t *Attribute, const void *lpValue, size_t cbSize,
    void *reserved_lpPreviousValue, size_t *reserved_lpReturnSize)
    paramsnonnull((1, 3, 4));
void DeleteProcThreadAttributeList(
    struct NtProcThreadAttributeList *inout_lpAttributeList) paramsnonnull();

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/startupinfo.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_NTSTARTUPINFO_H_ */
