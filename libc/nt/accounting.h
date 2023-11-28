#ifndef COSMOPOLITAN_LIBC_NT_ACCOUNTING_H_
#define COSMOPOLITAN_LIBC_NT_ACCOUNTING_H_
#include "libc/nt/struct/filetime.h"
#include "libc/nt/struct/iocounters.h"
#include "libc/nt/struct/memorystatusex.h"
#include "libc/nt/thunk/msabi.h"
COSMOPOLITAN_C_START_
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
│ cosmopolitan § new technology » accounting                               ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

uint32_t GetMaximumProcessorCount(uint16_t GroupNumber);
int GetUserName(char16_t (*buf)[257], uint32_t *in_out_size);
bool32 GlobalMemoryStatusEx(struct NtMemoryStatusEx *lpBuffer);
int32_t GetExitCodeProcess(int64_t hProcess, uint32_t *lpExitCode);
int32_t GetProcessHandleCount(int64_t hProcess, uint32_t *pdwHandleCount);
bool32 GetSystemTimes(struct NtFileTime *opt_out_lpIdleTime,
                      struct NtFileTime *opt_out_lpKernelTime,
                      struct NtFileTime *opt_out_lpUserTime);
bool32 GetProcessTimes(int64_t hProcess,
                       struct NtFileTime *out_lpCreationFileTime,
                       struct NtFileTime *out_lpExitFileTime,
                       struct NtFileTime *out_lpKernelFileTime,
                       struct NtFileTime *out_lpUserFileTime);
bool32 GetThreadTimes(int64_t hThread,
                      struct NtFileTime *out_lpCreationFileTime,
                      struct NtFileTime *out_lpExitFileTime,
                      struct NtFileTime *out_lpKernelFileTime,
                      struct NtFileTime *out_lpUserFileTime);
int32_t GetProcessIoCounters(int64_t hProcess,
                             struct NtIoCounters *lpIoCounters);
int32_t GetProcessWorkingSetSize(int64_t hProcess,
                                 uint64_t *lpMinimumWorkingSetSize,
                                 uint64_t *lpMaximumWorkingSetSize);
int32_t GetProcessWorkingSetSizeEx(int64_t hProcess,
                                   uint64_t *lpMinimumWorkingSetSize,
                                   uint64_t *lpMaximumWorkingSetSize,
                                   uint32_t *Flags);
int32_t SetProcessWorkingSetSize(int64_t hProcess,
                                 uint64_t dwMinimumWorkingSetSize,
                                 uint64_t dwMaximumWorkingSetSize);
int32_t SetProcessWorkingSetSizeEx(int64_t hProcess,
                                   uint64_t dwMinimumWorkingSetSize,
                                   uint64_t dwMaximumWorkingSetSize,
                                   uint32_t Flags);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/accounting.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_ACCOUNTING_H_ */
