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
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/winsize.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/struct/consolescreenbufferinfoex.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/errfuns.h"

textwindows int tcgetwinsize_nt(int fd, struct winsize *ws) {

  // The Linux man page doesn't list EBADF as an errno for this.
  if (!__isfdopen(fd)) {
    return enotty();
  }

  // Unlike _check_sigwinch() this is an API so be stricter.
  intptr_t hConsoleOutput;
  if (fd == STDIN_FILENO) {
    uint32_t dwMode;
    // WIN32 doesn't allow GetConsoleScreenBufferInfoEx(stdin)
    if (g_fds.p[STDOUT_FILENO].kind != kFdEmpty &&
        GetConsoleMode(g_fds.p[STDOUT_FILENO].handle, &dwMode)) {
      hConsoleOutput = g_fds.p[STDOUT_FILENO].handle;
    } else if (g_fds.p[STDERR_FILENO].kind != kFdEmpty &&
               GetConsoleMode(g_fds.p[STDERR_FILENO].handle, &dwMode)) {
      hConsoleOutput = g_fds.p[STDERR_FILENO].handle;
    } else {
      return enotty();
    }
  } else if (g_fds.p[fd].kind == kFdConsole) {
    hConsoleOutput = g_fds.p[fd].extra;
  } else {
    hConsoleOutput = g_fds.p[fd].handle;
  }

  // Query the console.
  struct NtConsoleScreenBufferInfoEx sr = {.cbSize = sizeof(sr)};
  if (GetConsoleScreenBufferInfoEx(hConsoleOutput, &sr)) {
    ws->ws_col = sr.srWindow.Right - sr.srWindow.Left + 1;
    ws->ws_row = sr.srWindow.Bottom - sr.srWindow.Top + 1;
    return 0;
  } else {
    return enotty();
  }
}
