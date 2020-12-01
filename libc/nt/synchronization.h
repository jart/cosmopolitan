#ifndef COSMOPOLITAN_LIBC_NT_SYNCHRONIZATION_H_
#define COSMOPOLITAN_LIBC_NT_SYNCHRONIZATION_H_
#include "libc/nt/struct/criticalsection.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/struct/linkedlist.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/nt/struct/systemtime.h"
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
│ cosmopolitan § new technology » synchronization                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define InterlockedAdd(PTR, VAL)                                           \
  ({                                                                       \
    typeof(*(PTR)) Res;                                                    \
    typeof(Res) Val = (VAL);                                               \
    asm volatile("lock xadd\t%0,%1" : "=r"(Res), "+m"(*(PTR)) : "0"(Val)); \
    Res + Val;                                                             \
  })

#define InterlockedExchange(PTR, VAL)                      \
  ({                                                       \
    typeof(*(PTR)) Res = (VAL);                            \
    asm volatile("xchg\t%0,%1" : "+r"(Res), "+m"(*(PTR))); \
    Res;                                                   \
  })

typedef void (*NtTimerapcroutine)(void *lpArgToCompletionRoutine,
                                  uint32_t dwTimerLowValue,
                                  uint32_t dwTimerHighValue);

void Sleep(uint32_t dwMilliseconds);
uint32_t SleepEx(uint32_t dwMilliseconds, bool32 bAlertable);

void GetSystemTime(struct NtSystemTime *lpSystemTime);
bool32 SystemTimeToFileTime(const struct NtSystemTime *lpSystemTime,
                            struct NtFileTime *lpFileTime);
void GetSystemTimeAsFileTime(struct NtFileTime *);         // win8+
void GetSystemTimePreciseAsFileTime(struct NtFileTime *);  // win8+

uint32_t WaitForSingleObject(int64_t hHandle, uint32_t dwMilliseconds);
uint32_t WaitForMultipleObjects(uint32_t nCount, const int64_t *lpHandles,
                                bool32 bWaitAll, uint32_t dwMilliseconds);
uint32_t WaitForSingleObjectEx(int64_t hHandle, uint32_t dwMilliseconds,
                               bool32 bAlertable);
uint32_t WaitForMultipleObjectsEx(unsigned int nCount, const int64_t *lpHandles,
                                  bool32 bWaitAll, uint32_t dwMilliseconds,
                                  bool32 bAlertable);

int64_t CreateWaitableTimer(struct NtSecurityAttributes *lpTimerAttributes,
                            bool32 bManualReset, const char16_t *lpTimerName);
bool32 SetWaitableTimer(int64_t hTimer, const int64_t *lpDueTimeAsFtOrNegRela,
                        int32_t opt_lPeriodMs, NtTimerapcroutine opt_callback,
                        void *lpArgToCallback, bool32 fUnsleepSystem);

int32_t SetEvent(int64_t hEvent);
int32_t ResetEvent(int64_t hEvent);
int32_t PulseEvent(int64_t hEvent);

int32_t ReleaseMutex(int64_t hMutex);
int32_t ReleaseSemaphore(int64_t hSemaphore, int32_t lReleaseCount,
                         int *lpPreviousCount);

void InitializeCriticalSection(struct NtCriticalSection *lpCriticalSection);
void EnterCriticalSection(struct NtCriticalSection *lpCriticalSection);
void LeaveCriticalSection(struct NtCriticalSection *lpCriticalSection);
int32_t TryEnterCriticalSection(struct NtCriticalSection *lpCriticalSection);
void DeleteCriticalSection(struct NtCriticalSection *lpCriticalSection);
int32_t InitializeCriticalSectionAndSpinCount(
    struct NtCriticalSection *lpCriticalSection, uint32_t dwSpinCount);
uint32_t SetCriticalSectionSpinCount(
    struct NtCriticalSection *lpCriticalSection, uint32_t dwSpinCount);

void InitializeSRWLock(intptr_t *);
void AcquireSRWLockExclusive(intptr_t *);
void AcquireSRWLockShared(intptr_t *);
void ReleaseSRWLockExclusive(intptr_t *);
void ReleaseSRWLockShared(intptr_t *);
void TryAcquireSRWLockExclusive(intptr_t *);
void TryAcquireSRWLockShared(intptr_t *);

uint64_t GetTickCount64(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_SYNCHRONIZATION_H_ */
