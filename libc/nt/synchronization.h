#ifndef COSMOPOLITAN_LIBC_NT_SYNCHRONIZATION_H_
#define COSMOPOLITAN_LIBC_NT_SYNCHRONIZATION_H_
#include "libc/nt/struct/linkedlist.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
#if 0
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
#endif

struct NtCriticalSection;
struct NtFileTime;
struct NtSystemTime;

void Sleep(uint32_t dwMilliseconds);
uint32_t SleepEx(uint32_t dwMilliseconds, bool32 bAlertable);
void GetSystemTime(struct NtSystemTime *lpSystemTime);
bool32 SystemTimeToFileTime(const struct NtSystemTime *lpSystemTime,
                            struct NtFileTime *lpFileTime);
void GetSystemTimeAsFileTime(
    struct NtFileTime *out_lpSystemTimeAsFileTime); /* win8+ */
void GetSystemTimePreciseAsFileTime(
    struct NtFileTime *out_lpSystemTimeAsFileTime); /* win8+ */
void InitializeCriticalSection(struct NtCriticalSection *lpCriticalSection);
void EnterCriticalSection(struct NtCriticalSection *lpCriticalSection);
void LeaveCriticalSection(struct NtCriticalSection *lpCriticalSection);
int32_t InitializeCriticalSectionAndSpinCount(
    struct NtCriticalSection *lpCriticalSection, uint32_t dwSpinCount);
uint32_t SetCriticalSectionSpinCount(
    struct NtCriticalSection *lpCriticalSection, uint32_t dwSpinCount);
int32_t TryEnterCriticalSection(struct NtCriticalSection *lpCriticalSection);
void DeleteCriticalSection(struct NtCriticalSection *lpCriticalSection);
int32_t SetEvent(int64_t hEvent);
int32_t ResetEvent(int64_t hEvent);
int32_t PulseEvent(int64_t hEvent);
int32_t ReleaseSemaphore(int64_t hSemaphore, int32_t lReleaseCount,
                         int *lpPreviousCount);
int32_t ReleaseMutex(int64_t hMutex);
uint32_t WaitForSingleObject(int64_t hHandle, uint32_t dwMilliseconds);
uint32_t WaitForMultipleObjects(uint32_t nCount, const void **lpHandles,
                                bool32 bWaitAll, uint32_t dwMilliseconds);
uint32_t WaitForSingleObjectEx(int64_t hHandle, uint32_t dwMilliseconds,
                               bool32 bAlertable);
uint32_t WaitForMultipleObjectsEx(unsigned int nCount, const void **lpHandles,
                                  bool32 bWaitAll, uint32_t dwMilliseconds,
                                  bool32 bAlertable);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_SYNCHRONIZATION_H_ */
