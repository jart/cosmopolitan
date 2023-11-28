#ifndef COSMOPOLITAN_LIBC_NT_IPC_H_
#define COSMOPOLITAN_LIBC_NT_IPC_H_
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/struct/securityattributes.h"
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
│ cosmopolitan § new technology » ipc                                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

/* CreateNamedPipe:dwOpenMode */
#define kNtPipeAccessInbound  0x00000001
#define kNtPipeAccessOutbound 0x00000002
#define kNtPipeAccessDuplex   0x00000003

/* CreateNamedPipe::dwPipeMode */
#define kNtPipeWait                0x00000000
#define kNtPipeNowait              0x00000001
#define kNtPipeReadmodeByte        0x00000000
#define kNtPipeReadmodeMessage     0x00000002
#define kNtPipeTypeByte            0x00000000
#define kNtPipeTypeMessage         0x00000004
#define kNtPipeAcceptRemoteClients 0x00000000
#define kNtPipeRejectRemoteClients 0x00000008

/* CreateNamedPipe::nMaxInstances */
#define kNtPipeUnlimitedInstances 255

/* GetNamedPipeInfo */
#define kNtPipeClientEnd 0x00000000
#define kNtPipeServerEnd 0x00000001

COSMOPOLITAN_C_START_

bool32 CreatePipe(int64_t *out_hReadPipe, int64_t *out_hWritePipe,
                  const struct NtSecurityAttributes *opt_lpPipeAttributes,
                  uint32_t nSize) paramsnonnull((1, 2));

int64_t CreateNamedPipe(
    const char16_t *lpName, uint32_t dwOpenMode, uint32_t dwPipeMode,
    uint32_t nMaxInstances, uint32_t nOutBufferSize, uint32_t nInBufferSize,
    uint32_t nDefaultTimeOut,
    const struct NtSecurityAttributes *opt_lpSecurityAttributes)
    paramsnonnull((1));

bool32 CallNamedPipe(const char16_t *lpNamedPipeName, void *lpInBuffer,
                     uint32_t nInBufferSize, void *lpOutBuffer,
                     uint32_t nOutBufferSize, uint32_t *lpBytesRead,
                     uint32_t nTimeOut);

bool32 ConnectNamedPipe(int64_t hNamedPipe, struct NtOverlapped *lpOverlapped);
bool32 WaitNamedPipe(const char16_t *lpNamedPipeName, uint32_t nTimeOut);
bool32 DisconnectNamedPipe(int64_t hNamedPipe);

bool32 SetNamedPipeHandleState(int64_t hNamedPipe, uint32_t *lpMode,
                               uint32_t *lpMaxCollectionCount,
                               uint32_t *lpCollectDataTimeout);

bool32 PeekNamedPipe(int64_t hNamedPipe, void *lpBuffer, uint32_t nBufferSize,
                     uint32_t *opt_lpBytesRead, uint32_t *opt_lpTotalBytesAvail,
                     uint32_t *opt_lpBytesLeftThisMessage);

bool32 TransactNamedPipe(int64_t hNamedPipe, void *lpInBuffer,
                         uint32_t nInBufferSize, void *lpOutBuffer,
                         uint32_t nOutBufferSize, uint32_t *lpBytesRead,
                         struct NtOverlapped *lpOverlapped);

bool32 GetNamedPipeInfo(int64_t hNamedPipe, uint32_t *opt_out_lpFlags,
                        uint32_t *opt_out_lpOutBufferSize,
                        uint32_t *opt_out_lpInBufferSize,
                        uint32_t *opt_out_lpMaxInstances);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_IPC_H_ */
