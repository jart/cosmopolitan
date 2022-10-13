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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/struct/winsize.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/struct/consolescreenbufferinfoex.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/tls.h"

static struct winsize __ws;

textwindows void _check_sigwinch(struct Fd *fd) {
  int e;
  siginfo_t si;
  struct winsize ws, old;
  struct NtConsoleScreenBufferInfoEx sbinfo;
  if (__tls_enabled && __threaded != gettid()) return;
  old = __ws;
  e = errno;
  if (old.ws_row != 0xffff) {
    if (ioctl_tiocgwinsz_nt(fd, &ws) != -1) {
      if (old.ws_col != ws.ws_col || old.ws_row != ws.ws_row) {
        __ws = ws;
        if (old.ws_col | old.ws_row) {
          __sig_add(0, SIGWINCH, SI_KERNEL);
        }
      }
    } else {
      if (!old.ws_row && !old.ws_col) {
        __ws.ws_row = 0xffff;
      }
    }
  }
  errno = e;
}
