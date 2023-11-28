#ifndef COSMOPOLITAN_LIBC_NT_PROCESS_H_
#define COSMOPOLITAN_LIBC_NT_PROCESS_H_
#include "libc/nt/startupinfo.h"
#include "libc/nt/struct/processentry32.h"
#include "libc/nt/struct/processinformation.h"
#include "libc/nt/struct/processmemorycounters.h"
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

COSMOPOLITAN_C_START_

bool32 CreateProcess(const char16_t *opt_lpApplicationName,
                     char16_t *lpCommandLine,
                     const struct NtSecurityAttributes *opt_lpProcessAttributes,
                     const struct NtSecurityAttributes *opt_lpThreadAttributes,
                     bool32 bInheritHandles, uint32_t dwCreationFlags,
                     void *opt_lpEnvironment,
                     const char16_t *opt_lpCurrentDirectory,
                     const struct NtStartupInfo *lpStartupInfo,
                     struct NtProcessInformation *opt_out_lpProcessInformation)
    paramsnonnull((9));

uint32_t GetThreadId(int64_t hThread);   /* cf. NT_TID */
uint32_t GetProcessId(int64_t hProcess); /* cf. NT_PID */
void SetLastError(uint32_t dwErrCode);
uint32_t FormatMessage(uint32_t dwFlags, const void *lpSource,
                       uint32_t dwMessageId, uint32_t dwLanguageId,
                       char16_t *lpBuffer, uint32_t nSize, va_list *Arguments);
uint32_t FormatMessageA(uint32_t dwFlags, const void *lpSource,
                        uint32_t dwMessageId, uint32_t dwLanguageId,
                        char *lpBuffer, uint32_t nSize, va_list *Arguments);
int64_t OpenProcess(uint32_t dwDesiredAccess, bool32 bInheritHandle,
                    uint32_t dwProcessId);
uint32_t GetCurrentProcessId(void); /* %gs:0x40 */
uint32_t GetEnvironmentVariable(const char16_t *lpName, char16_t *lpBuffer,
                                uint32_t nSize);
uint32_t SetEnvironmentVariable(const char16_t *lpName,
                                const char16_t *lpValue);
int32_t SetEnvironmentStrings(char16_t *NewEnvironment);
bool32 GetProcessAffinityMask(int64_t hProcess, uint64_t *lpProcessAffinityMask,
                              uint64_t *lpSystemAffinityMask);
uint64_t /*bool32*/ SetProcessAffinityMask(int64_t hProcess,
                                           uint64_t dwProcessAffinityMask);

/* e.g. kNtAboveNormalPriorityClass, kNtHighPriorityClass */
uint32_t GetPriorityClass(int64_t hProcess);
bool32 SetPriorityClass(int64_t hProcess, uint32_t dwPriorityClass);
bool32 SetProcessPriorityBoost(int64_t hProcess, bool32 bDisablePriorityBoost);
bool32 GetProcessPriorityBoost(int64_t hProcess, bool32 *pDisablePriorityBoost);

bool32 GetProcessMemoryInfo(
    int64_t hProcess, struct NtProcessMemoryCountersEx *out_ppsmemCounters,
    uint32_t cb);

int64_t CreateToolhelp32Snapshot(uint32_t dwFlags, uint32_t th32ProcessID);
bool32 Process32First(int64_t hSnapshot, struct NtProcessEntry32 *in_out_lppe);
bool32 Process32Next(int64_t hSnapshot, struct NtProcessEntry32 *out_lppe);

bool32 EnumProcesses(uint32_t *out_lpidProcess, uint32_t cb,
                     uint32_t *out_lpcbNeeded) paramsnonnull();
bool32 EnumProcessModules(int64_t hProcess, int64_t *out_lphModule, uint32_t cb,
                          uint32_t *out_lpcbNeeded) paramsnonnull();
bool32 EnumProcessModulesEx(int64_t hProcess, int64_t *out_lphModule,
                            uint32_t cb, uint32_t *out_lpcbNeeded,
                            uint32_t dwFilterFlag) paramsnonnull();
uint32_t GetModuleBaseName(int64_t hProcess, int64_t opt_hModule,
                           char16_t *out_lpBaseName, uint32_t nSize)
    paramsnonnull();

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/process.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_PROCESS_H_ */
