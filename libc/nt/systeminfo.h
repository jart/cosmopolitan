#ifndef COSMOPOLITAN_LIBC_NT_INFO_H_
#define COSMOPOLITAN_LIBC_NT_INFO_H_
#include "libc/nt/struct/systeminfo.h"
#include "libc/nt/thunk/msabi.h"
COSMOPOLITAN_C_START_

void GetSystemInfo(struct NtSystemInfo *lpSystemInfo);
uint32_t GetSystemDirectory(char16_t *lpBuffer, uint32_t uSize);
uint32_t GetSystemDirectoryA(char *lpBuffer, uint32_t uSize);
uint32_t GetWindowsDirectory(char16_t *lpBuffer, uint32_t uSize);
uint32_t GetTempPath(uint32_t uSize, char16_t *lpBuffer);

bool32 GetComputerNameEx(/* enum/computernameformat.h */ int NameType,
                         char16_t *opt_lpBuffer, uint32_t *nSize);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/systeminfo.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_INFO_H_ */
