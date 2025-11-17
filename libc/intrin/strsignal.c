/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"

/**
 * Returns string describing signal code.
 */
__privileged char *strsignal(int sig) {
  static const char kSignalNames[33][10] = {
      [0] = "0",                  // 0
      [SIGHUP] = "SIGHUP",        // 1
      [SIGINT] = "SIGINT",        // 2
      [SIGQUIT] = "SIGQUIT",      // 3
      [SIGILL] = "SIGILL",        // 4
      [SIGTRAP] = "SIGTRAP",      // 5
      [SIGABRT] = "SIGABRT",      // 6
      [SIGBUS] = "SIGBUS",        // 7
      [SIGFPE] = "SIGFPE",        // 8
      [SIGKILL] = "SIGKILL",      // 9
      [SIGUSR1] = "SIGUSR1",      // 10
      [SIGSEGV] = "SIGSEGV",      // 11
      [SIGUSR2] = "SIGUSR2",      // 12
      [SIGPIPE] = "SIGPIPE",      // 13
      [SIGALRM] = "SIGALRM",      // 14
      [SIGTERM] = "SIGTERM",      // 15
      [16] = "SIG16",             // 16 SIGSTKFLT
      [SIGCHLD] = "SIGCHLD",      // 17
      [SIGCONT] = "SIGCONT",      // 18
      [SIGSTOP] = "SIGSTOP",      // 19
      [SIGTSTP] = "SIGTSTP",      // 20
      [SIGTTIN] = "SIGTTIN",      // 21
      [SIGTTOU] = "SIGTTOU",      // 22
      [SIGURG] = "SIGURG",        // 23
      [SIGXCPU] = "SIGXCPU",      // 24
      [SIGXFSZ] = "SIGXFSZ",      // 25
      [SIGVTALRM] = "SIGVTALRM",  // 26
      [SIGPROF] = "SIGPROF",      // 27
      [SIGWINCH] = "SIGWINCH",    // 28
      [29] = "SIG29",             // 29 SIGPOLL
      [30] = "SIG30",             // 30 SIGPWR
      [SIGSYS] = "SIGSYS",        // 31
      [SIGTHR] = "SIGTHR",        // 32
  };
  if (0 <= sig && sig <= 32)
    return (char *)kSignalNames[sig];
  return "SIG???";
}
