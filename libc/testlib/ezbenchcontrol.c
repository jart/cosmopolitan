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
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

static bool once;
static double g_ezbenchcontrol;

double __testlib_ezbenchcontrol(void) {
  char host[64];
  char ibuf[12];
  int Core, Tries, Interrupts;
  if (!once) {
    Tries = 0;
    do {
      __testlib_yield();
      Core = __testlib_getcore();
      Interrupts = __testlib_getinterrupts();
      g_ezbenchcontrol =
          BENCHLOOP(__startbench, __endbench, 128, donothing, (void)0);
    } while (++Tries < 10 && (__testlib_getcore() != Core &&
                              __testlib_getinterrupts() > Interrupts));
    if (Tries == 10) {
      tinyprint(2, "warning: failed to accurately benchmark control\n", NULL);
    }
    strcpy(host, "unknown");
    gethostname(host, 64);
    FormatInt32(ibuf, g_ezbenchcontrol);
    tinyprint(2, "benchmarks on ", host, " (", __describe_os(),
              "; overhead of ", ibuf, " cycles)\n", NULL);
    once = true;
  }
  return g_ezbenchcontrol;
}
