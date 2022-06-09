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
#include "libc/bits/popcnt.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/str/str.h"

const char *DescribeSigset(char *buf, size_t bufsize, int rc,
                           const sigset_t *ss) {
  bool gotsome;
  int i, n, sig;
  sigset_t sigset;

  if (rc == -1) return "n/a";
  if (!ss) return "NULL";
  if ((!IsAsan() && kisdangerous(ss)) ||
      (IsAsan() && !__asan_is_valid(ss, sizeof(*ss)))) {
    ksnprintf(buf, sizeof(buf), "%p", ss);
    return buf;
  }

  i = 0;
  n = bufsize;
  sigset = *ss;
  gotsome = false;
  if (popcnt(sigset.__bits[0]) + popcnt(sigset.__bits[1]) > 64) {
    i += ksnprintf(buf + i, n - i, "~");
    sigset.__bits[0] = ~sigset.__bits[0];
    sigset.__bits[1] = ~sigset.__bits[1];
  }
  i += ksnprintf(buf + i, n - i, "{");
  for (sig = 1; sig < 128; ++sig) {
    if (sigismember(&sigset, sig)) {
      if (gotsome) {
        sig += ksnprintf(buf + sig, n - sig, ", ");
      } else {
        gotsome = true;
      }
      sig += ksnprintf(buf + sig, n - sig, "%s", strsignal(sig));
    }
  }
  i += ksnprintf(buf + i, n - i, "}");

  return buf;
}
