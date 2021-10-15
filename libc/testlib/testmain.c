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
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"
#include "third_party/getopt/getopt.h"

#define USAGE \
  " [FLAGS]\n\
\n\
Flags:\n\
\n\
  -b         run benchmarks if tests pass\n\
  -h         show this information\n\
\n"

STATIC_YOINK("__die");
STATIC_YOINK("__get_symbol_by_addr");
STATIC_YOINK("testlib_quota_handlers");

static bool runbenchmarks_;

void PrintUsage(int rc, FILE *f) {
  fputs("Usage: ", f);
  fputs(program_invocation_name, f);
  fputs(USAGE, f);
  exit(rc);
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "?hbv")) != -1) {
    switch (opt) {
      case 'b':
        runbenchmarks_ = true;
        break;
      case 'v':
        ++__log_level;
        break;
      case '?':
      case 'h':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
}

/**
 * Generic test program main function.
 */
noasan int main(int argc, char *argv[]) {
  const char *comdbg;
  __log_level = kLogInfo;
  GetOpts(argc, argv);
  ShowCrashReports();
  g_testlib_shoulddebugbreak = IsDebuggerPresent(false);
  sys_getpid(); /* make strace easier to read */
  testlib_clearxmmregisters();
  testlib_runalltests();
  if (!g_testlib_failed && runbenchmarks_ && weaken(testlib_runallbenchmarks)) {
    weaken(testlib_runallbenchmarks)();
    if (!g_testlib_failed) {
      testlib_checkformemoryleaks();
    }
    if (!g_testlib_failed && IsRunningUnderMake()) {
      return 254; /* compile.com considers this 0 and propagates output */
    }
  } else if (!g_testlib_failed) {
    testlib_checkformemoryleaks();
  }
  exit(min(255, g_testlib_failed));
}
