#ifndef COSMOPOLITAN_LIBC_NT_EVENTS_H_
#define COSMOPOLITAN_LIBC_NT_EVENTS_H_
#include "libc/nt/struct/msg.h"
#include "libc/nt/struct/point.h"
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
│ cosmopolitan § new technology » events                                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

COSMOPOLITAN_C_START_

int32_t GetMessage(struct NtMsg *lpMsg, int64_t hWnd, uint32_t wMsgFilterMin,
                   uint32_t wMsgFilterMax);
int32_t TranslateMessage(const struct NtMsg *lpMsg);
intptr_t DispatchMessage(const struct NtMsg *lpMsg);
void PostQuitMessage(int nExitCode);
bool32 GetCursorPos(struct NtPoint *lpPoint);
int64_t SendMessage(int64_t hWnd, uint32_t Msg, uint64_t wParam,
                    int64_t lParam);

#define EVENTLOG_SUCCESS          0x00000000
#define EVENTLOG_ERROR_TYPE       0x00000001
#define EVENTLOG_WARNING_TYPE     0x00000002
#define EVENTLOG_INFORMATION_TYPE 0x00000004
#define EVENTLOG_AUDIT_SUCCESS    0x00000008
#define EVENTLOG_AUDIT_FAILURE    0x00000010

int32_t ReportEventA(int64_t handle, uint16_t wType, uint16_t wCategory,
                     uint32_t dwEventID, const char *lpUserId,
                     uint16_t wNumStrings, uint32_t dwDataSize,
                     const char **lpStrings, void **lpRawData);
int64_t RegisterEventSource(const char16_t *lpUNCServerName,
                            const char16_t *lpSourceName);
int32_t DeregisterEventSource(uint64_t handle);

int64_t CreateEvent(const struct NtSecurityAttributes *opt_lpEventAttributes,
                    bool32 bManualReset, bool32 bInitialState,
                    const char16_t *opt_lpName);
int64_t CreateEventEx(const struct NtSecurityAttributes *lpEventAttributes,
                      const char16_t *lpName, uint32_t dwFlags,
                      uint32_t dwDesiredAccess);

int32_t SetEvent(int64_t hEvent);
int32_t ResetEvent(int64_t hEvent);
int32_t PulseEvent(int64_t hEvent);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_EVENTS_H_ */
