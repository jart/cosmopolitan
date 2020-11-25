#ifndef COSMOPOLITAN_LIBC_NT_PROCESS_H_
#define COSMOPOLITAN_LIBC_NT_PROCESS_H_
#include "libc/nt/startupinfo.h"
#include "libc/nt/struct/processinformation.h"
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
│ cosmopolitan § new technology » processes                                ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

bool32 CreateProcess(const char16_t *opt_lpApplicationName,
                     char16_t *lpCommandLine,
                     struct NtSecurityAttributes *opt_lpProcessAttributes,
                     struct NtSecurityAttributes *opt_lpThreadAttributes,
                     bool32 bInheritHandles, uint32_t dwCreationFlags,
                     void *opt_lpEnvironment,
                     const char16_t *opt_lpCurrentDirectory,
                     const struct NtStartupInfo *lpStartupInfo,
                     struct NtProcessInformation *opt_out_lpProcessInformation)
    paramsnonnull((2, 9));

uint32_t GetThreadId(int64_t Thread);   /* cf. NT_TID */
uint32_t GetProcessId(int64_t Process); /* cf. NT_PID */
void SetLastError(uint32_t dwErrCode);
uint32_t FormatMessage(uint32_t dwFlags, const void *lpSource,
                       uint32_t dwMessageId, uint32_t dwLanguageId,
                       char16_t *lpBuffer, uint32_t nSize, va_list *Arguments);
int64_t OpenProcess(uint32_t dwDesiredAccess, bool32 bInheritHandle,
                    uint32_t dwProcessId);
uint32_t GetCurrentProcessId(void); /* %gs:0x40 */
uint32_t GetEnvironmentVariable(const char16_t *lpName, char16_t *lpBuffer,
                                uint32_t nSize);
uint32_t SetEnvironmentVariable(const char16_t *lpName, char16_t *lpValue);
int32_t SetEnvironmentStrings(char16_t *NewEnvironment);
bool32 GetProcessAffinityMask(int64_t hProcess,
                              uintptr_t *lpProcessAffinityMask,
                              uintptr_t *lpSystemAffinityMask);
uint64_t /*bool32*/ SetProcessAffinityMask(int64_t hProcess,
                                           uintptr_t dwProcessAffinityMask);

/* e.g. kNtAboveNormalPriorityClass, kNtHighPriorityClass */
uint32_t GetPriorityClass(int64_t hProcess);
bool32 SetPriorityClass(int64_t hProcess, uint32_t dwPriorityClass);
bool32 SetProcessPriorityBoost(int64_t hProcess, bool32 bDisablePriorityBoost);
bool32 GetProcessPriorityBoost(int64_t hProcess, bool32 *pDisablePriorityBoost);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/process.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_PROCESS_H_ */
