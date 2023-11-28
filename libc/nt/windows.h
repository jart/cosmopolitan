#ifndef COSMOPOLITAN_LIBC_NT_WINDOWS_H_
#define COSMOPOLITAN_LIBC_NT_WINDOWS_H_
#include "libc/nt/struct/rect.h"
#include "libc/nt/struct/windowplacement.h"
#include "libc/nt/struct/wndclass.h"
#include "libc/nt/typedef/timerproc.h"
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
int32_t ShowCaret(bool32 bShow);
int32_t AnimateWindow(int64_t hWnd, uint32_t dwTime, uint32_t dwFlags);
int64_t LoadIcon(int64_t hInstance, const char16_t *lpIconName);
int32_t MoveWindow(int64_t hWnd, int X, int Y, int nWidth, int nHeight,
                   bool32 bRepaint);
int32_t BringWindowToTop(int64_t hWnd);
int32_t IsWindowVisible(int64_t hWnd);
int32_t SetWindowText(int64_t hWnd, const char16_t *lpString);
int32_t GetWindowText(int64_t hWnd, char16_t *lpString, int nMaxCount);
int32_t SetWindowPos(int64_t hWnd, int64_t hWndInsertAfter, int X, int Y,
                     int cx, int cy, uint32_t uFlags);
bool32 GetWindowPlacement(int64_t hWnd, struct NtWindowPlacement *lpwndpl);
bool32 SetWindowPlacement(int64_t hWnd,
                          const struct NtWindowPlacement *lpwndpl);

int64_t GetCursor(void);
int64_t SetCursor(int64_t hCursor);
int32_t ShowCursor(bool32 bShow);
int64_t LoadCursor(int64_t opt_hInstance, const char16_t *lpCursorNameOrIdc);

bool32 IsWindow(int64_t hWnd);
bool32 IsMenu(int64_t hMenu);
bool32 IsChild(int64_t hWndParent, int64_t hWnd);
bool32 IsZoomed(int64_t hWnd);
bool32 IsIconic(int64_t hWnd);

uintptr_t SetTimer(int64_t opt_hWnd, uintptr_t nIDEvent, uint32_t uElapseMs,
                   NtTimerProc lpTimerFunc);
int32_t KillTimer(int64_t hWnd, uintptr_t uIDEvent);

int64_t SetCapture(int64_t hWnd);
bool32 ReleaseCapture(void);
int16_t GetKeyState(int32_t nVirtKey);

int64_t CreateMenu(void);
int64_t CreatePopupMenu(void);
int64_t GetMenu(int64_t hWnd);
bool32 DestroyMenu(int64_t hMenu);
int64_t GetSystemMenu(int64_t hWnd, bool32 bRevert);
bool32 AppendMenu(int64_t hMenu, uint32_t mfFlags, uintptr_t uIDNewItem,
                  const char16_t *lpNewItem);
bool32 InsertMenu(int64_t hMenu, uint32_t uPosition, uint32_t uFlags,
                  uintptr_t uIDNewItem, const char16_t *lpNewItem);
bool32 TrackPopupMenu(int64_t hMenu, uint32_t uFlags, int32_t x, int32_t y,
                      int32_t nReserved, int64_t hWnd,
                      const struct NtRect *prcRect);

int WideCharToMultiByte(unsigned int CodePage, uint32_t dwFlags,
                        uint16_t *lpWideCharStr, int cchWideChar,
                        char *lpMultiByteStr, int cbMultiByte,
                        uint16_t *lpDefaultChar, int *lpUsedDefaultChar);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_WINDOWS_H_ */
