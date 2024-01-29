#ifndef COSMOPOLITAN_LIBC_NT_SYNCHRONIZATION_H_
#define COSMOPOLITAN_LIBC_NT_SYNCHRONIZATION_H_
#include "libc/intrin/atomic.h"
#include "libc/nt/struct/criticalsection.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/struct/linkedlist.h"
#include "libc/nt/struct/processornumber.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/nt/struct/systemtime.h"
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
│ cosmopolitan § new technology » synchronization                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

static inline int32_t InterlockedAdd(int32_t volatile *p, int32_t x) {
  return atomic_fetch_add((_Atomic(int32_t) *)p, x) + x;
}

static inline int32_t InterlockedExchange(int32_t volatile *p, int32_t x) {
  return atomic_exchange((_Atomic(int32_t) *)p, x);
}

typedef void (*NtTimerapcroutine)(void *lpArgToCompletionRoutine,
                                  uint32_t dwTimerLowValue,
                                  uint32_t dwTimerHighValue);
typedef void (*NtWaitOrTimerCallback)(void *lpParameter,
                                      bool32 TimerOrWaitFired);

void WakeByAddressAll(void *Address);
void WakeByAddressSingle(void *Address);
bool32 WaitOnAddress(const volatile void *Address, void *CompareAddress,
                     size_t AddressSize, uint32_t opt_dwMilliseconds);

void Sleep(uint32_t dwMilliseconds);
uint32_t SleepEx(uint32_t dwMilliseconds, bool32 bAlertable);

void GetSystemTime(struct NtSystemTime *lpSystemTime);
bool32 SystemTimeToFileTime(const struct NtSystemTime *lpSystemTime,
                            struct NtFileTime *lpFileTime);
void GetSystemTimeAsFileTime(struct NtFileTime *);
void GetSystemTimePreciseAsFileTime(struct NtFileTime *); /* win8+ */

uint32_t WaitForSingleObject(int64_t hHandle, uint32_t dwMilliseconds);
uint32_t WaitForMultipleObjects(uint32_t nCount, const int64_t *lpHandles,
                                bool32 bWaitAll, uint32_t dwMilliseconds);
uint32_t WaitForSingleObjectEx(int64_t hHandle, uint32_t dwMilliseconds,
                               bool32 bAlertable);
uint32_t WaitForMultipleObjectsEx(unsigned int nCount, const int64_t *lpHandles,
                                  bool32 bWaitAll, uint32_t dwMilliseconds,
                                  bool32 bAlertable);
bool32 RegisterWaitForSingleObject(int64_t *phNewWaitObject, int64_t hObject,
                                   NtWaitOrTimerCallback Callback,
                                   void *Context, uint32_t dwMilliseconds,
                                   uint32_t dwFlags);

int64_t CreateWaitableTimer(
    const struct NtSecurityAttributes *lpTimerAttributes, bool32 bManualReset,
    const char16_t *lpTimerName);
bool32 SetWaitableTimer(int64_t hTimer, const int64_t *lpDueTimeAsFtOrNegRela,
                        int32_t opt_lPeriodMs, NtTimerapcroutine opt_callback,
                        void *lpArgToCallback, bool32 fUnsleepSystem);

int64_t CreateSemaphore(
    const struct NtSecurityAttributes *opt_lpSemaphoreAttributes,
    uint32_t lInitialCount, uint32_t lMaximumCount, const char16_t *opt_lpName);

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

bool32 QueryPerformanceFrequency(uint64_t *lpFrequency);
bool32 QueryPerformanceCounter(uint64_t *lpPerformanceCount);
bool32 GetSystemTimeAdjustment(uint32_t *lpTimeAdjustment,
                               uint32_t *lpTimeIncrement,
                               bool32 *lpTimeAdjustmentDisabled);

void GetCurrentProcessorNumberEx(struct NtProcessorNumber *out_ProcNumber);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/synchronization.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_SYNCHRONIZATION_H_ */
