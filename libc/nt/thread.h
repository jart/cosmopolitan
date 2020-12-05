#ifndef COSMOPOLITAN_LIBC_NT_THREADS_H_
#define COSMOPOLITAN_LIBC_NT_THREADS_H_
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/nt/thunk/msabi.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
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
│ cosmopolitan § new technology » threads                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

typedef uint32_t (*NtThreadStartRoutine)(void *lpParameter);

int64_t CreateThread(struct NtSecurityAttributes *lpThreadAttributes,
                     size_t dwStackSize, NtThreadStartRoutine lpStartAddress,
                     void *lpParameter, uint32_t dwCreationFlags,
                     uint32_t *opt_lpThreadId);

void ExitThread(uint32_t dwExitCode) wontreturn;
int64_t GetCurrentThread(void);
uint32_t GetCurrentThreadId(void);
uint64_t SetThreadAffinityMask(int64_t hThread, uintptr_t dwThreadAffinityMask);
int64_t OpenThread(uint32_t dwDesiredAccess, bool32 bInheritHandle,
                   uint32_t dwThreadId);
bool32 TerminateThread(int64_t hThread, uint32_t dwExitCode);
bool32 GetExitCodeThread(int64_t hThread, uint32_t *lpExitCode);

/* e.g. kNtThreadPriorityAboveNormal, -1u on error */
uint32_t GetThreadPriority(int64_t hThread);
bool32 SetThreadPriority(int64_t hThread, int32_t nPriority);
bool32 SetThreadPriorityBoost(int64_t hThread, bool32 bDisablePriorityBoost);
bool32 GetThreadPriorityBoost(int64_t hThread, bool32 *pDisablePriorityBoost);
bool32 GetThreadIOPendingFlag(int64_t hThread, bool32 *lpIOIsPending);

bool32 CancelSynchronousIo(int64_t hThread);
bool32 CancelIo(int64_t hFile);
bool32 CancelIoEx(int64_t hFile, struct NtOverlapped opt_lpOverlapped);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/thread.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_THREADS_H_ */
