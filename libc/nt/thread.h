#ifndef COSMOPOLITAN_LIBC_NT_THREADS_H_
#define COSMOPOLITAN_LIBC_NT_THREADS_H_
#include "libc/nt/struct/context.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/struct/securityattributes.h"
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
│ cosmopolitan § new technology » threads                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int64_t CreateThread(const struct NtSecurityAttributes *lpThreadAttributes,
                     size_t dwStackSize, void *lpStartAddress,
                     void *lpParameter, uint32_t dwCreationFlags,
                     uint32_t *opt_lpThreadId) paramsnonnull((3));

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
bool32 CancelIoEx(int64_t hFile, struct NtOverlapped *opt_lpOverlapped);

uint32_t TlsAlloc(void);
bool32 TlsFree(uint32_t);
bool32 TlsSetValue(uint32_t, void *);
void *TlsGetValue(uint32_t);

uint32_t SuspendThread(int64_t hThread);
uint32_t ResumeThread(int64_t hThread);
bool32 GetThreadContext(int64_t hThread, struct NtContext *in_out_lpContext);
bool32 SetThreadContext(int64_t hThread, const struct NtContext *lpContext);

void *SetThreadDescription(int64_t hThread,
                           const char16_t *lpThreadDescription);
void *GetThreadDescription(int64_t hThread,
                           char16_t *out_ppszThreadDescription);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/thread.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_THREADS_H_ */
