/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/struct/consolescreenbufferinfoex.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

textwindows int ioctl$tiocgwinsz$nt(int fd, struct winsize *ws) {
  int i, fds[3];
  uint32_t mode;
  struct NtConsoleScreenBufferInfoEx sbinfo;
  fds[0] = fd, fds[1] = 1, fds[2] = 0;
  for (i = 0; i < ARRAYLEN(fds); ++i) {
    if (__isfdkind(fds[i], kFdFile) || __isfdkind(fds[i], kFdConsole)) {
      if (GetConsoleMode(g_fds.p[fds[i]].handle, &mode)) {
        memset(&sbinfo, 0, sizeof(sbinfo));
        sbinfo.cbSize = sizeof(sbinfo);
        if (GetConsoleScreenBufferInfoEx(g_fds.p[fds[i]].handle, &sbinfo)) {
          ws->ws_col = sbinfo.srWindow.Right - sbinfo.srWindow.Left + 1;
          ws->ws_row = sbinfo.srWindow.Bottom - sbinfo.srWindow.Top + 1;
          ws->ws_xpixel = 0;
          ws->ws_ypixel = 0;
          return 0;
        } else if (g_ntstartupinfo.dwFlags & kNtStartfUsecountchars) {
          ws->ws_col = g_ntstartupinfo.dwXCountChars;
          ws->ws_row = g_ntstartupinfo.dwYCountChars;
          ws->ws_xpixel = 0;
          ws->ws_ypixel = 0;
          return 0;
        } else {
          __winerr();
        }
      } else {
        enotty();
      }
    } else {
      ebadf();
    }
  }
  return -1;
}
