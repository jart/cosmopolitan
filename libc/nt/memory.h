#ifndef COSMOPOLITAN_LIBC_NT_MEMORY_H_
#define COSMOPOLITAN_LIBC_NT_MEMORY_H_
#include "libc/nt/struct/memorybasicinformation.h"
#include "libc/nt/struct/memoryrangeentry.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/nt/thunk/msabi.h"
/*                            ░░░░
                       ▒▒▒░░░▒▒▒▒▒▒▒▓▓▓░
                      ▒▒▒▒░░░▒▒▒▒▒▒▓▓▓▓▓▓░
                     ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▒▓░
                     ▒▒▒░░░░▒▒▒▒▒▒▓▓▓▓▓▓   ▓▓▓▓▓▓▒        ▒▒▒▓▓█
                    ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
                   ░▒▒▒░░░░▒▒▒▒▒▒▓▓▓▓▓▓   █▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                   ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓░  ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
                  ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▒▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▒
                  ▒▒▒▒▓▓      ▓▒▒▓▓▓▓   ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                                   ▒▓  ▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓
                  ░░░░░░░░░░░▒▒▒▒      ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                ▒▒░░░░░░░░░░▒▒▒▒▒▓▓▓     ▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
               ░▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓░      ░▓███▓
               ▒▒░░░░░░░░░░▒▒▒▒▒▓▓░  ▒▓▓▓▒▒▒         ░▒▒▒▓   ████████████
              ▒▒░░░░░░░░░░░▒▒▒▒▒▓▓  ▒▓▓▓▓▒▒▒▒▒▒▒▒░░░▒▒▒▒▒░           ░███
              ▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▓            ███
             ▒▒░░░░░░░░░░▒▒▒▒▒▒▓▓  ▒▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▒            ▓██
             ▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓▓▓▓▒▒▒▒▒▒▒▒░░░▒▒▒▒▒▓           ▓██
            ▒▒░░░▒▒▒░░░▒▒░▒▒▒▓▓▒  ▒▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▒           ███
                            ░▒▓  ░▓▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▓          ▓██
╔────────────────────────────────────────────────────────────────▀▀▀─────────│─╗
│ cosmopolitan § new technology » memory                                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define kNtNumaNoPreferredNode 0xffffffffu

COSMOPOLITAN_C_START_

void *LocalFree(void *hMem);

int64_t CreateFileMapping(
    int64_t opt_hFile,
    const struct NtSecurityAttributes *opt_lpFileMappingAttributes,
    uint32_t flProtect, uint32_t dwMaximumSizeHigh, uint32_t dwMaximumSizeLow,
    const char16_t *opt_lpName);
int64_t CreateFileMappingNuma(
    int64_t opt_hFile,
    const struct NtSecurityAttributes *opt_lpFileMappingAttributes,
    uint32_t flProtect, uint32_t dwMaximumSizeHigh, uint32_t dwMaximumSizeLow,
    const char16_t *opt_lpName, uint32_t nndDesiredNumaNode);

void *MapViewOfFileEx(int64_t hFileMappingObject, uint32_t dwDesiredAccess,
                      uint32_t dwFileOffsetHigh, uint32_t dwFileOffsetLow,
                      size_t dwNumberOfBytesToMap,
                      void *opt_lpDesiredBaseAddress);
void *MapViewOfFileExNuma(int64_t hFileMappingObject, uint32_t dwDesiredAccess,
                          uint32_t dwFileOffsetHigh, uint32_t dwFileOffsetLow,
                          size_t dwNumberOfBytesToMap,
                          void *opt_lpDesiredBaseAddress,
                          uint32_t nndDesiredNumaNode);

bool32 UnmapViewOfFile(const void *lpBaseAddress);
bool32 FlushViewOfFile(const void *lpBaseAddress,
                       size_t dwNumberOfBytesToFlush);

void *VirtualAlloc(void *opt_lpAddress, uint64_t dwSize,
                   uint32_t flAllocationType, uint32_t flProtect);
bool32 VirtualFree(void *lpAddress, uint64_t dwSize, uint32_t dwFreeType);
bool32 VirtualProtect(void *lpAddress, uint64_t dwSize, uint32_t flNewProtect,
                      uint32_t *lpflOldProtect) paramsnonnull();
bool32 VirtualLock(const void *lpAddress, size_t dwSize);
bool32 VirtualUnlock(const void *lpAddress, size_t dwSize);
uint64_t VirtualQuery(const void *lpAddress,
                      struct NtMemoryBasicInformation *lpBuffer,
                      uint64_t dwLength);
void *VirtualAllocEx(int64_t hProcess, void *lpAddress, uint64_t dwSize,
                     uint32_t flAllocationType, uint32_t flProtect);

int64_t GetProcessHeap(void);
void *HeapAlloc(int64_t hHeap, uint32_t dwFlags, size_t dwBytes) __wur;
bool32 HeapFree(int64_t hHeap, uint32_t dwFlags, void *opt_lpMem);
void *HeapReAlloc(int64_t hHeap, uint32_t dwFlags, void *lpMem,
                  size_t dwBytes) __wur;

void *GlobalAlloc(uint32_t uFlags, uint64_t dwBytes) __wur;
void *GlobalFree(void *hMem);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/memory.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_MEMORY_H_ */
