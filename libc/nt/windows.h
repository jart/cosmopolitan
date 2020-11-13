#ifndef COSMOPOLITAN_LIBC_NT_WINDOWS_H_
#define COSMOPOLITAN_LIBC_NT_WINDOWS_H_
#include "libc/nt/struct/rect.h"
#include "libc/nt/struct/wndclass.h"
#include "libc/nt/typedef/timerproc.h"
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
│ cosmopolitan § new technology » windows                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#endif
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int64_t CreateWindowEx(uint32_t dwExStyle, const char16_t *lpClassName,
                       const char16_t *lpWindowName, uint32_t dwStyle, int X,
                       int Y, int nWidth, int nHeight, int64_t hWndParent,
                       int64_t hMenu, int64_t hInstance, int64_t lpParam);

uint16_t RegisterClass(const struct NtWndClass *lpWndClass);

int64_t DefWindowProc(int64_t hWnd, uint32_t Msg, uint64_t wParam,
                      int64_t lParam);

int32_t CloseWindow(int64_t hWnd);
int32_t DestroyWindow(int64_t hWnd);
int32_t ShowWindow(int64_t hWnd, int sw);
int32_t ShowCursor(bool32 bShow);
int64_t LoadCursor(int64_t opt_hInstance, const char16_t *lpCursorNameOrIdc);
int32_t ShowCaret(bool32 bShow);
int32_t AnimateWindow(int64_t hWnd, uint32_t dwTime, uint32_t dwFlags);
int64_t LoadIcon(int64_t hInstance, const char16_t *lpIconName);
int32_t IsWindow(int64_t hWnd);
int32_t IsMenu(int64_t hMenu);
int32_t IsChild(int64_t hWndParent, int64_t hWnd);
int32_t MoveWindow(int64_t hWnd, int X, int Y, int nWidth, int nHeight,
                   bool32 bRepaint);
int32_t BringWindowToTop(int64_t hWnd);
int32_t IsWindowVisible(int64_t hWnd);
int32_t IsZoomed(int64_t hWnd);
int32_t SetWindowText(int64_t hWnd, const char16_t *lpString);
int32_t GetWindowText(int64_t hWnd, char16_t *lpString, int nMaxCount);
int32_t SetWindowPos(int64_t hWnd, int64_t hWndInsertAfter, int X, int Y,
                     int cx, int cy, uint32_t uFlags);

uintptr_t SetTimer(int64_t opt_hWnd, uintptr_t nIDEvent, uint32_t uElapseMs,
                   NtTimerProc lpTimerFunc);
int32_t KillTimer(int64_t hWnd, uintptr_t uIDEvent);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_WINDOWS_H_ */
