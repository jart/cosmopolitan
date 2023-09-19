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
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/cosmo.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/struct/consolescreenbufferinfoex.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/tls.h"
#include "libc/thread/tls2.internal.h"
#ifdef __x86_64__

intptr_t __sigwinch_thread;
static unsigned __sigwinch_size;
static atomic_uint __sigwinch_once;
static struct CosmoTib __sigwinch_tls;

static textwindows unsigned __get_console_size(void) {
  unsigned res = -1u;
  __fds_lock();
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
      res = (unsigned)yn << 16 | xn;
      break;
    }
  }
  __fds_unlock();
  return res;
}

static textwindows dontinstrument uint32_t __sigwinch_worker(void *arg) {
  __bootstrap_tls(&__sigwinch_tls, __builtin_frame_address(0));
  for (;;) {
    unsigned old = __sigwinch_size;
    unsigned neu = __get_console_size();
    if (neu != old) {
      __sigwinch_size = neu;
      __sig_generate(SIGWINCH, SI_KERNEL);
    }
    SleepEx(25, false);
  }
  return 0;
}

static textwindows void __sigwinch_init(void) {
  __enable_threads();
  __sigwinch_size = __get_console_size();
  __sigwinch_thread = CreateThread(0, 65536, __sigwinch_worker, 0,
                                   kNtStackSizeParamIsAReservation, 0);
}

textwindows void _init_sigwinch(void) {
  cosmo_once(&__sigwinch_once, __sigwinch_init);
}

#endif /* __x86_64__ */
