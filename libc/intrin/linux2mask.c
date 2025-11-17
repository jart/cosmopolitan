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
#include "libc/calls/struct/sigset.internal.h"
#include "libc/dce.h"
#include "libc/sysv/consts/sig.h"

#define SIGBUS_BSD  10
#define SIGUSR1_BSD 30
#define SIGUSR2_BSD 31
#define SIGCHLD_BSD 20
#define SIGCONT_BSD 19
#define SIGSTOP_BSD 17
#define SIGTSTP_BSD 18
#define SIGURG_BSD  16
#define SIGSYS_BSD  12
#define SIGTHR_BSD  7

__privileged sigset_t __linux2mask(sigset_t m) {
  if (IsLinux() || IsWindows())
    return m;
  sigset_t r = m & ~((1ull << (SIGBUS - 1)) |       //
                     (1ull << (SIGBUS_BSD - 1)) |   //
                     (1ull << (SIGUSR1 - 1)) |      //
                     (1ull << (SIGUSR1_BSD - 1)) |  //
                     (1ull << (SIGUSR2 - 1)) |      //
                     (1ull << (SIGUSR2_BSD - 1)) |  //
                     (1ull << (SIGCHLD - 1)) |      //
                     (1ull << (SIGCHLD_BSD - 1)) |  //
                     (1ull << (SIGCONT - 1)) |      //
                     (1ull << (SIGCONT_BSD - 1)) |  //
                     (1ull << (SIGSTOP - 1)) |      //
                     (1ull << (SIGSTOP_BSD - 1)) |  //
                     (1ull << (SIGTSTP - 1)) |      //
                     (1ull << (SIGTSTP_BSD - 1)) |  //
                     (1ull << (SIGURG - 1)) |       //
                     (1ull << (SIGURG_BSD - 1)) |   //
                     (1ull << (SIGSYS - 1)) |       //
                     (1ull << (SIGSYS_BSD - 1)) |   //
                     (1ull << (SIGTHR - 1)) |       //
                     (1ull << (SIGTHR_BSD - 1)));
  r |= ((m & (1ull << (SIGBUS - 1))) >> (SIGBUS - 1)) << (SIGBUS_BSD - 1);
  r |= ((m & (1ull << (SIGUSR1 - 1))) >> (SIGUSR1 - 1)) << (SIGUSR1_BSD - 1);
  r |= ((m & (1ull << (SIGUSR2 - 1))) >> (SIGUSR2 - 1)) << (SIGUSR2_BSD - 1);
  r |= ((m & (1ull << (SIGCHLD - 1))) >> (SIGCHLD - 1)) << (SIGCHLD_BSD - 1);
  r |= ((m & (1ull << (SIGCONT - 1))) >> (SIGCONT - 1)) << (SIGCONT_BSD - 1);
  r |= ((m & (1ull << (SIGSTOP - 1))) >> (SIGSTOP - 1)) << (SIGSTOP_BSD - 1);
  r |= ((m & (1ull << (SIGTSTP - 1))) >> (SIGTSTP - 1)) << (SIGTSTP_BSD - 1);
  r |= ((m & (1ull << (SIGURG - 1))) >> (SIGURG - 1)) << (SIGURG_BSD - 1);
  r |= ((m & (1ull << (SIGSYS - 1))) >> (SIGSYS - 1)) << (SIGSYS_BSD - 1);
  r |= ((m & (1ull << (SIGTHR - 1))) >> (SIGTHR - 1)) << (SIGTHR_BSD - 1);
  return r;
}
