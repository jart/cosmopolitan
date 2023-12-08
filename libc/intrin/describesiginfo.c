/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/siginfo.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/weaken.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"

#define N 300

#define append(...) i += ksnprintf(buf + i, N - i, __VA_ARGS__)

const char *(DescribeSiginfo)(char buf[N], int rc, const siginfo_t *si) {
  int i = 0;

  if (rc == -1) return "n/a";
  if (!si) return "NULL";
  if ((!IsAsan() && kisdangerous(si)) ||
      (IsAsan() && !__asan_is_valid(si, sizeof(*si)))) {
    ksnprintf(buf, N, "%p", si);
    return buf;
  }

  append("{.si_signo=%s, si_code=%s", strsignal(si->si_signo),
         DescribeSiCode(si->si_signo, si->si_code));

  if (si->si_errno) {
    append(", .si_errno=%s", DescribeErrno(si->si_errno));
  }

  if (si->si_signo == SIGILL ||   //
      si->si_signo == SIGFPE ||   //
      si->si_signo == SIGSEGV ||  //
      si->si_signo == SIGBUS ||   //
      si->si_signo == SIGTRAP) {
    append(", .si_addr=%p, .si_addr_lsb=%d, .si_pkey=%u", si->si_addr,
           si->si_addr_lsb, si->si_pkey);
  } else if (si->si_signo == SIGALRM) {
    append(", .si_timerid=%d, .si_overrun=%d", si->si_timerid, si->si_overrun);
  } else {
    if (si->si_pid) {
      append(", .si_pid=%d", si->si_pid);
    }
    if (si->si_uid) {
      append(", .si_uid=%d", si->si_uid);
    }
  }

  if (si->si_status) {
    append(", .si_status=%d", si->si_status);
  }

  if (si->si_utime) {
    append(", .si_utime=%d", si->si_utime);
  }

  if (si->si_stime) {
    append(", .si_stime=%d", si->si_stime);
  }

  append("}");

  return buf;
}
