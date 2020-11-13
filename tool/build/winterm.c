/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/nt/dll.h"
#include "libc/nt/enum/color.h"
#include "libc/nt/enum/cw.h"
#include "libc/nt/enum/idc.h"
#include "libc/nt/enum/sw.h"
#include "libc/nt/enum/wm.h"
#include "libc/nt/enum/ws.h"
#include "libc/nt/events.h"
#include "libc/nt/paint.h"
#include "libc/nt/struct/msg.h"
#include "libc/nt/struct/paintstruct.h"
#include "libc/nt/struct/wndclass.h"
#include "libc/nt/windows.h"
#include "libc/runtime/missioncritical.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/winmain.h"
#include "libc/str/str.h"

/**
 * @fileoverview Incomplete Windows App.
 */

static const char16_t kClassName[] = u"Sample Window Class";

int64_t WindowProcSysv(int64_t hwnd, uint32_t uMsg, uint64_t wParam,
                       int64_t lParam) {
  int64_t hdc;
  struct NtPaintStruct ps;
  switch (uMsg) {
    case kNtWmDestroy:
      PostQuitMessage(0);
      return 0;
    case kNtWmPaint:
      hdc = BeginPaint(hwnd, &ps);
      FillRect(hdc, &ps.rcPaint, kNtColorWindow + 1);
      EndPaint(hwnd, &ps);
      return 0;
    default:
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}

int64_t WindowProcNt(int64_t, uint32_t, uint64_t, int64_t);
asm("WindowProcNt:\n\t"
    "lea\tWindowProcSysv(%rip),%rax\n\t"
    "jmp\t__nt2sysv");

int main(int argc, char *argv[]) {
  int64_t hwnd;
  struct NtMsg msg;
  struct NtWndClass wc;
  memset(&wc, 0, sizeof(wc));
  wc.lpfnWndProc = WindowProcNt;
  wc.hInstance = g_winmain.hInstance;
  wc.hCursor = LoadCursor(0, kNtIdcIbeam);
  wc.lpszClassName = kClassName;
  CHECK(RegisterClass(&wc));
  CHECK((hwnd = CreateWindowEx(0, kClassName, u"Learn to Program Windows",
                               kNtWsOverlappedwindow, kNtCwUsedefault,
                               kNtCwUsedefault, kNtCwUsedefault,
                               kNtCwUsedefault, 0, 0, g_winmain.hInstance, 0)));
  ShowWindow(hwnd, kNtSwNormal);
  while (GetMessage(&msg, 0, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return 0;
}
