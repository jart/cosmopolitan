/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/stdio/stdio.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

static bool once;
static int64_t g_ezbenchcontrol;

int64_t __testlib_ezbenchcontrol(void) {
  if (!once) {
    int Core, Tries, Interrupts;
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
      fputs("warning: failed to accurately benchmark control\n", stderr);
    }
    fprintf(stderr, "will subtract benchmark overhead of %ld cycles\n\n",
            g_ezbenchcontrol);
    once = true;
  }
  return g_ezbenchcontrol;
}
