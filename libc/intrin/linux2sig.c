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

int __linux2sig(int e) {
  if (IsLinux() || IsWindows())
    return e;
  static const char map[33] = {
      [0] = 0,           //
      [SIGHUP] = 1,      //
      [SIGINT] = 2,      //
      [SIGQUIT] = 3,     //
      [SIGILL] = 4,      //
      [SIGTRAP] = 5,     //
      [SIGABRT] = 6,     //
      [SIGBUS] = 10,     //
      [SIGFPE] = 8,      //
      [SIGKILL] = 9,     //
      [SIGUSR1] = 30,    //
      [SIGSEGV] = 11,    //
      [SIGUSR2] = 31,    //
      [SIGPIPE] = 13,    //
      [SIGALRM] = 14,    //
      [SIGTERM] = 15,    //
      [16] = 16,         //
      [SIGCHLD] = 20,    //
      [SIGCONT] = 19,    //
      [SIGSTOP] = 17,    //
      [SIGTSTP] = 18,    //
      [SIGTTIN] = 21,    //
      [SIGTTOU] = 22,    //
      [SIGURG] = 16,     //
      [SIGXCPU] = 24,    //
      [SIGXFSZ] = 25,    //
      [SIGVTALRM] = 26,  //
      [SIGPROF] = 27,    //
      [SIGWINCH] = 28,   //
      [29] = 29,         //
      [30] = 30,         //
      [SIGSYS] = 12,     //
      [SIGTHR] = 7,      //
  };
  if (0 <= e && e <= 32)
    return map[e];
  return e;
}
