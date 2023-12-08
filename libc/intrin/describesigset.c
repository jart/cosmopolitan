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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/popcnt.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/sig.h"

#define N 128

#define append(...) o += ksnprintf(buf + o, N - o, __VA_ARGS__)

const char *(DescribeSigset)(char buf[N], int rc, const sigset_t *ss) {
  int olderr;
  bool gotsome;
  const char *s;
  int sig, o = 0;
  sigset_t sigset;

  if (rc == -1) return "n/a";
  if (!ss) return "NULL";
  if ((!IsAsan() && kisdangerous(ss)) ||
      (IsAsan() && !__asan_is_valid(ss, sizeof(*ss)))) {
    ksnprintf(buf, N, "%p", ss);
    return buf;
  }
  olderr = errno;

  if (sigcountset(ss) > 16) {
    append("~");
    sigemptyset(&sigset);
    for (sig = 1; sig <= _NSIG; ++sig) {
      if (!sigismember(ss, sig)) {
        sigaddset(&sigset, sig);
      }
    }
  } else {
    sigset = *ss;
  }

  append("{");
  gotsome = false;
  for (sig = 1; sig <= _NSIG; ++sig) {
    if (sigismember(&sigset, sig) > 0) {
      if (gotsome) {
        append(",");
      } else {
        gotsome = true;
      }
      s = strsignal(sig);
      if (s[0] == 'S' && s[1] == 'I' && s[2] == 'G') {
        s += 3;
      }
      append("%s", s);
    }
  }
  append("}");

  errno = olderr;
  return buf;
}
