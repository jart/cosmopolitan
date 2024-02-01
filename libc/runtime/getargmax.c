/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/consts/_posix.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/rlimit.h"

#define CTL_KERN    1
#define KERN_ARGMAX 8

/**
 * Returns expensive but more correct version of `ARG_MAX`.
 */
int __get_arg_max(void) {
  int res;
  if (IsLinux()) {
    // You might think that just returning a constant 128KiB (ARG_MAX)
    // would make sense, as this guy did:
    //
    //   https://lkml.org/lkml/2017/11/15/813...
    //
    //   I suspect a 128kB sysconf(_SC_ARG_MAX) is the sanest bet, simply
    //   because of that "conservative is better than aggressive".
    //
    //   Especially since _technically_ we're still limiting things to that
    //   128kB due to the single-string limit.
    //
    //                 Linus
    //
    // In practice that caused us trouble with toybox tests for xargs
    // edge cases. The tests assume that they can at least reach the
    // kernel's "minimum maximum" of 128KiB, but if we report 128KiB for
    // _SC_ARG_MAX and xargs starts subtracting the environment space
    // and so on from that, then xargs will think it's run out of space
    // when given 128KiB of data, which should always work. See this
    // thread for more:
    //
    // http://lists.landley.net/pipermail/toybox-landley.net/2019-November/011229.html
    //
    // So let's resign ourselves to tracking what the kernel actually
    // does. Right now (2019, Linux 5.3) that amounts to:
    uint64_t stacksz;
    stacksz = __get_rlimit(RLIMIT_STACK);
    res = MAX(MIN(stacksz / 4, 3 * (8 * 1024 * 1024) / 4), _ARG_MAX);
  } else if (IsBsd()) {
    res = __get_sysctl(CTL_KERN, KERN_ARGMAX);
  } else {
    res = _ARG_MAX;
  }
  return MAX(res, _POSIX_ARG_MAX);
}
