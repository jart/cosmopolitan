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
#include "libc/bits/safemacros.h"
#include "libc/calls/internal.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/testlib/testlib.h"
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

static bool runbenchmarks_;

static testonly void PrintUsage(int rc, FILE *f) {
  fputs("Usage: ", f);
  fputs(program_invocation_name, f);
  fputs(USAGE, f);
  exit(rc);
}

static testonly void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "?hbv")) != -1) {
    switch (opt) {
      case 'b':
        runbenchmarks_ = true;
        break;
      case 'v':
        ++g_loglevel;
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
testonly int main(int argc, char *argv[]) {
  g_loglevel = kLogInfo;
  GetOpts(argc, argv);
  showcrashreports();
  g_testlib_shoulddebugbreak = IsDebuggerPresent(false);
  getpid$sysv(); /* make strace easier to read */
  testlib_clearxmmregisters();
  testlib_runalltests();
  if (!g_testlib_failed && runbenchmarks_ && weaken(testlib_runallbenchmarks)) {
    weaken(testlib_runallbenchmarks)();
  }
  return min(255, g_testlib_failed);
}
