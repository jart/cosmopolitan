/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
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
