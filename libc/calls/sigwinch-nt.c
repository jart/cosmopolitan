/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/winsize.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/errno.h"
#include "libc/nt/struct/consolescreenbufferinfoex.h"
#include "libc/str/str.h"

static struct winsize __ws;

textwindows bool _check_sigwinch(struct Fd *fd) {
  int e;
  siginfo_t si;
  struct winsize ws, old;
  struct NtConsoleScreenBufferInfoEx sbinfo;
  old = __ws;
  if (old.ws_row != 0xffff) {
    e = errno;
    if (ioctl_tiocgwinsz_nt(fd, &ws) != -1) {
      if (old.ws_col != ws.ws_col || old.ws_row != ws.ws_row) {
        __ws = ws;
        if (old.ws_col | old.ws_row) {
          SYSDEBUG("SIGWINCH %hhu×%hhu → %hhu×%hhu", old.ws_col, old.ws_row,
                   ws.ws_col, ws.ws_row);
          if (__sighandrvas[SIGWINCH] >= kSigactionMinRva) {
            bzero(&si, sizeof(si));
            ((sigaction_f)(_base + __sighandrvas[SIGWINCH]))(SIGWINCH, &si, 0);
            return true;
          }
        }
      }
    } else {
      errno = e;
      if (!old.ws_row && !old.ws_col) {
        __ws.ws_row = 0xffff;
      }
    }
  }
  return false;
}
