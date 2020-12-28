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
#include "libc/calls/internal.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/color.internal.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/math.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"

testonly void testlib_showerror(const char *file, int line, const char *func,
                                const char *method, const char *symbol,
                                const char *code, char *v1, char *v2) {
  /* TODO(jart): Pay off tech debt re duplication */
  getpid$sysv(); /* make strace easier to read */
  getpid$sysv();
  fprintf(stderr,
          "%s%s%s%s:%s:%d%s: %s() %s %s(%s)\n"
          "\t%s\n"
          "\t\t%s %s %s\n"
          "\t\t%s %s\n"
          "\t%s%s\n"
          "\t%s%s\n",
          RED2, "error", UNBOLD, BLUE1, file, line, RESET, method, "in", func,
          g_fixturename, code, "need", v1, symbol, " got", v2, SUBTLE,
          strerror(errno), program_invocation_name, RESET);
  free_s(&v1);
  free_s(&v2);
}
