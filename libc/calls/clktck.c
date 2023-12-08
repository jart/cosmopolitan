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
#include "libc/runtime/clktck.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/intrin/getauxval.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"

struct clockinfo_netbsd {
  int32_t hz;       // number of clock ticks per second
  int32_t tick;     // µs per tick
  int32_t tickadj;  // skew rate for adjtime()
  int32_t stathz;   // statistics clock frequency
  int32_t profhz;   // profiling clock frequency
};

static int clk_tck;

static dontinline int __clk_tck_init(void) {
  int x;
  int cmd[2];
  size_t len;
  struct clockinfo_netbsd clock;
  if (IsWindows()) {
    // MSVC defines CLK_TCK as 1000 but 1ms is obviously not the
    // scheduling quantum Windows actually uses. If we define it
    // as 30 rather than 1000, then clock_nanosleep is much more
    // accurately able to predict the duration of its busy waits
    x = 30;
  } else if (IsXnu() || IsOpenbsd()) {
    x = 100;
  } else if (IsFreebsd()) {
    x = 128;
  } else if (IsNetbsd()) {
    cmd[0] = 1;   // CTL_KERN
    cmd[1] = 12;  // KERN_CLOCKRATE
    len = sizeof(clock);
    if (sys_sysctl(cmd, 2, &clock, &len, NULL, 0) != -1) {
      x = clock.hz;
    } else {
      x = -1;
    }
  } else {
    x = __getauxval(AT_CLKTCK).value;
  }
  if (x < 1) x = 100;
  clk_tck = x;
  return x;
}

/**
 * Returns system clock ticks per second.
 *
 * The returned value is memoized. This function is intended to be
 * used via the `CLK_TCK` macro wrapper.
 *
 * The returned value is always greater than zero. It's usually 100
 * hertz which means each clock tick is 10 milliseconds long.
 */
int __clk_tck(void) {
  if (clk_tck) {
    return clk_tck;
  } else {
    return __clk_tck_init();
  }
}
