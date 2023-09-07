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
#include "libc/atomic.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/struct/winsize.internal.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/nt/console.h"
#include "libc/nt/struct/consolescreenbufferinfoex.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"

#ifdef __x86_64__

static atomic_uint __win_winsize;

static textwindows unsigned __get_console_size(void) {
  for (int fd = 1; fd < 10; ++fd) {
    intptr_t hConsoleOutput;
    if (g_fds.p[fd].kind == kFdConsole) {
      hConsoleOutput = g_fds.p[fd].extra;
    } else {
      hConsoleOutput = g_fds.p[fd].handle;
    }
    struct NtConsoleScreenBufferInfoEx sr = {.cbSize = sizeof(sr)};
    if (GetConsoleScreenBufferInfoEx(hConsoleOutput, &sr)) {
      unsigned short yn = sr.srWindow.Bottom - sr.srWindow.Top + 1;
      unsigned short xn = sr.srWindow.Right - sr.srWindow.Left + 1;
      return (unsigned)yn << 16 | xn;
    }
  }
  return -1u;
}

textwindows void _check_sigwinch(void) {
  unsigned old = atomic_load_explicit(&__win_winsize, memory_order_acquire);
  if (old == -1u) return;
  unsigned neu = __get_console_size();
  old = atomic_exchange(&__win_winsize, neu);
  if (neu != old) {
    __sig_add(0, SIGWINCH, SI_KERNEL);
  }
}

__attribute__((__constructor__)) static void sigwinch_init(void) {
  if (!IsWindows()) return;
  unsigned ws = __get_console_size();
  atomic_store_explicit(&__win_winsize, ws, memory_order_release);
}

#endif /* __x86_64__ */
