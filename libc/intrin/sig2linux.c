/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/sysv/consts/sig.h"

__privileged int __sig2linux(int e) {
  if (IsLinux() || IsWindows())
    return e;
  static const char map[33] = {
      [0] = 0,           //
      [1] = SIGHUP,      //
      [2] = SIGINT,      //
      [3] = SIGQUIT,     //
      [4] = SIGILL,      //
      [5] = SIGTRAP,     //
      [6] = SIGABRT,     //
      [7] = SIGTHR,      // SIGEMT/SIGPOLL (BSD), SIGBUS (Linux)
      [8] = SIGFPE,      //
      [9] = SIGKILL,     //
      [10] = SIGBUS,     //
      [11] = SIGSEGV,    //
      [12] = SIGSYS,     //
      [13] = SIGPIPE,    //
      [14] = SIGALRM,    //
      [15] = SIGTERM,    //
      [16] = SIGURG,     //
      [17] = SIGSTOP,    //
      [18] = SIGTSTP,    //
      [19] = SIGCONT,    //
      [20] = SIGCHLD,    //
      [21] = SIGTTIN,    //
      [22] = SIGTTOU,    //
      [23] = 23,         // SIGIO (BSD), SIGURG (Linux)
      [24] = SIGXCPU,    //
      [25] = SIGXFSZ,    //
      [26] = SIGVTALRM,  //
      [27] = SIGPROF,    //
      [28] = SIGWINCH,   //
      [29] = 29,         // SIGINFO (BSD), SIGIO (Linux)
      [30] = SIGUSR1,    //
      [31] = SIGUSR2,    //
      [32] = 32,         // SIGPWR (NetBSD) SIGTHR (FreeBSD, OpenBSD)
  };
  if (0 <= e && e <= 32)
    return map[e];
  return e;
}
