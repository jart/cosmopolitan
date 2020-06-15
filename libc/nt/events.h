#ifndef COSMOPOLITAN_LIBC_NT_EVENTS_H_
#define COSMOPOLITAN_LIBC_NT_EVENTS_H_
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
│ cosmopolitan § new technology » events                                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#endif

#define NT_EVENT_SYSTEM_SOUND 0x0001
#define NT_EVENT_SYSTEM_ALERT 0x0002
#define NT_EVENT_SYSTEM_FOREGROUND 0x0003
#define NT_EVENT_SYSTEM_MENUSTART 0x0004
#define NT_EVENT_SYSTEM_MENUEND 0x0005
#define NT_EVENT_SYSTEM_MENUPOPUPSTART 0x0006
#define NT_EVENT_SYSTEM_MENUPOPUPEND 0x0007
#define NT_EVENT_SYSTEM_CAPTURESTART 0x0008
#define NT_EVENT_SYSTEM_CAPTUREEND 0x0009
#define NT_EVENT_SYSTEM_MOVESIZESTART 0x000A
#define NT_EVENT_SYSTEM_MOVESIZEEND 0x000B
#define NT_EVENT_SYSTEM_CONTEXTHELPSTART 0x000C
#define NT_EVENT_SYSTEM_CONTEXTHELPEND 0x000D
#define NT_EVENT_SYSTEM_DRAGDROPSTART 0x000E
#define NT_EVENT_SYSTEM_DRAGDROPEND 0x000F
#define NT_EVENT_SYSTEM_DIALOGSTART 0x0010
#define NT_EVENT_SYSTEM_DIALOGEND 0x0011
#define NT_EVENT_SYSTEM_SCROLLINGSTART 0x0012
#define NT_EVENT_SYSTEM_SCROLLINGEND 0x0013
#define NT_EVENT_SYSTEM_SWITCHSTART 0x0014
#define NT_EVENT_SYSTEM_SWITCHEND 0x0015
#define NT_EVENT_SYSTEM_MINIMIZESTART 0x0016
#define NT_EVENT_SYSTEM_MINIMIZEEND 0x0017

#define NT_EVENT_CONSOLE_CARET 0x4001
#define NT_EVENT_CONSOLE_UPDATE_REGION 0x4002
#define NT_EVENT_CONSOLE_UPDATE_SIMPLE 0x4003
#define NT_EVENT_CONSOLE_UPDATE_SCROLL 0x4004
#define NT_EVENT_CONSOLE_LAYOUT 0x4005
#define NT_EVENT_CONSOLE_START_APPLICATION 0x4006
#define NT_EVENT_CONSOLE_END_APPLICATION 0x4007

#define NT_EVENT_OBJECT_CREATE 0x8000
#define NT_EVENT_OBJECT_DESTROY 0x8001
#define NT_EVENT_OBJECT_SHOW 0x8002
#define NT_EVENT_OBJECT_HIDE 0x8003
#define NT_EVENT_OBJECT_REORDER 0x8004
#define NT_EVENT_OBJECT_FOCUS 0x8005
#define NT_EVENT_OBJECT_SELECTION 0x8006
#define NT_EVENT_OBJECT_SELECTIONADD 0x8007
#define NT_EVENT_OBJECT_SELECTIONREMOVE 0x8008
#define NT_EVENT_OBJECT_SELECTIONWITHIN 0x8009
#define NT_EVENT_OBJECT_STATECHANGE 0x800A
#define NT_EVENT_OBJECT_LOCATIONCHANGE 0x800B
#define NT_EVENT_OBJECT_NAMECHANGE 0x800C
#define NT_EVENT_OBJECT_DESCRIPTIONCHANGE 0x800D
#define NT_EVENT_OBJECT_VALUECHANGE 0x800E
#define NT_EVENT_OBJECT_PARENTCHANGE 0x800F
#define NT_EVENT_OBJECT_HELPCHANGE 0x8010
#define NT_EVENT_OBJECT_DEFACTIONCHANGE 0x8011
#define NT_EVENT_OBJECT_ACCELERATORCHANGE 0x8012

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct NtMsg;
struct NtPoint;

int32_t GetMessage(struct NtMsg *lpMsg, int64_t hWnd, uint32_t wMsgFilterMin,
                   uint32_t wMsgFilterMax);
int32_t TranslateMessage(const struct NtMsg *lpMsg);
intptr_t DispatchMessage(const struct NtMsg *lpMsg);
void PostQuitMessage(int nExitCode);
bool32 GetCursorPos(struct NtPoint *lpPoint);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_EVENTS_H_ */
