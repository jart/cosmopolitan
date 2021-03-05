#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/dce.h"
#include "libc/nexgen32e/nt2sysv.h"
#include "libc/nt/dll.h"
#include "libc/nt/enum/cw.h"
#include "libc/nt/enum/mb.h"
#include "libc/nt/enum/sw.h"
#include "libc/nt/enum/wm.h"
#include "libc/nt/enum/ws.h"
#include "libc/nt/events.h"
#include "libc/nt/messagebox.h"
#include "libc/nt/struct/msg.h"
#include "libc/nt/struct/wndclass.h"
#include "libc/nt/windows.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * @fileoverview GUI Example
 *
 * Cosmopolitan has had a razor sharp focus on making sockets and stdio
 * work well across platforms. When you call a function like read(), it
 * will just work on platforms like Windows too. In order to do that we
 * need to support the WIN32 API, which you can use, but please note it
 * isn't polyfilled on other platforms in the same way as POSIX APIs.
 */

static const char16_t kClassName[] = u"cosmopolitan";

static int64_t WindowProc(int64_t hwnd, uint32_t uMsg, uint64_t wParam,
                          int64_t lParam) {
  switch (uMsg) {
    case kNtWmDestroy:
      PostQuitMessage(0);
      return 0;
    default:
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}

static void Gui(void) {
  int64_t h;
  struct NtMsg msg;
  struct NtWndClass wc;
  memset(&wc, 0, sizeof(wc));
  wc.lpfnWndProc = NT2SYSV(WindowProc);
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpszClassName = kClassName;
  RegisterClass(&wc);
  h = CreateWindowEx(0, kClassName, u"Hello Cosmopolitan",
                     kNtWsOverlappedwindow, kNtCwUsedefault, kNtCwUsedefault,
                     kNtCwUsedefault, kNtCwUsedefault, 0, 0, wc.hInstance, 0);
  ShowWindow(h, kNtSwNormal);
  /*
   * NOTE: Cosmopolitan linker script is hard-coded to change the
   *       subsystem from CUI to GUI when GetMessage() is linked.
   */
  while (GetMessage(&msg, 0, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

int main(int argc, char *argv[]) {
  if (!IsWindows()) {
    fputs("Sorry! This GUI demo only runs on Windows\n", stderr);
    fputs("https://github.com/jart/cosmopolitan/issues/57\n", stderr);
    return 1;
  }
  Gui();
}
