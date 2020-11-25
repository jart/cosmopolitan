#ifndef COSMOPOLITAN_LIBC_NT_IOCP_H_
#define COSMOPOLITAN_LIBC_NT_IOCP_H_
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/struct/overlappedentry.h"
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
│ cosmopolitan § new technology » i/o completion ports                     ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define kNtFileSkipCompletionPortOnSuccess 1
#define kNtFileSkipSetEventOnHandle        2

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

typedef void (*NtOverlappedCompletionRoutine)(
    uint32_t dwErrorCode, uint32_t dwNumberOfBytesTransfered,
    struct NtOverlapped *inout_lpOverlapped);

int64_t CreateIoCompletionPort(int64_t FileHandleOrNeg1,
                               int64_t opt_ExistingCompletionPortOrZero,
                               void *StatePointer,
                               uint32_t NumberOfConcurrentThreads);

bool32 GetQueuedCompletionStatus(int64_t CompletionPort,
                                 uint32_t *lpNumberOfBytesTransferred,
                                 void *StatePointerPointer,
                                 struct NtOverlapped **lpOverlapped,
                                 uint32_t dwMilliseconds);

bool32 GetQueuedCompletionStatusEx(
    int64_t CompletionPort,
    struct NtOverlappedEntry *out_lpCompletionPortEntries, uint32_t ulCount,
    uint32_t *out_ulNumEntriesRemoved, uint32_t dwMilliseconds,
    bool32 fAlertable);

bool32 PostQueuedCompletionStatus(int64_t CompletionPort,
                                  uint32_t dwNumberOfBytesTransferred,
                                  uint32_t *dwCompletionKey,
                                  struct NtOverlapped *opt_lpOverlapped);

bool32 SetFileCompletionNotificationModes(int64_t FileHandle,
                                          unsigned char Flags);

bool32 ReadFileEx(int64_t hFile, void *lpBuffer, uint32_t nNumberOfBytesToRead,
                  uint32_t *lpNumberOfBytesRead,
                  struct NtOverlapped *opt_lpOverlapped,
                  NtOverlappedCompletionRoutine lpCompletionRoutine);

bool32 WriteFileEx(int64_t hFile, const void *lpBuffer,
                   uint32_t nNumberOfBytesToWrite,
                   struct NtOverlapped *lpOverlapped,
                   NtOverlappedCompletionRoutine lpCompletionRoutine);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_IOCP_H_ */
