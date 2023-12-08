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
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/check.h"
#include "libc/log/color.internal.h"
#include "libc/log/internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * Handles failure of CHECK_xx() macros.
 */
relegated void __check_fail(const char *suffix,   //
                            const char *opstr,    //
                            uint64_t want,        //
                            const char *wantstr,  //
                            uint64_t got,         //
                            const char *gotstr,   //
                            const char *file,     //
                            int line,             //
                            const char *fmt,      //
                            ...) {
  size_t i;
  va_list va;
  char hostname[32];
  strace_enabled(-1);
  ftrace_enabled(-1);
  __start_fatal(file, line);
  __stpcpy(hostname, "unknown");
  gethostname(hostname, sizeof(hostname));
  kprintf("check failed on %s pid %d\n"
          "\tCHECK_%^s(%s, %s);\n"
          "\t\t → %p (%s)\n"
          "\t\t%s %p (%s)\n",       //
          hostname, getpid(),       //
          suffix, wantstr, gotstr,  //
          want, wantstr,            //
          opstr, got, gotstr);
  if (!isempty(fmt)) {
    kprintf("\t");
    va_start(va, fmt);
    kvprintf(fmt, va);
    va_end(va);
    kprintf("\n");
  }
  kprintf("\t%s\n\t%s%s", strerror(errno), SUBTLE, program_invocation_name);
  for (i = 1; i < __argc; ++i) {
    kprintf(" %s", __argv[i]);
  }
  kprintf("%s\n", RESET);
  __die();
}
