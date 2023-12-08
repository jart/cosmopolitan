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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/getenv.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/aspect.internal.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"
#include "third_party/getopt/getopt.internal.h"

#define USAGE \
  " [FLAGS]\n\
\n\
Flags:\n\
\n\
  -b         run benchmarks if tests pass\n\
  -h         show this information\n\
\n"

static bool runbenchmarks_;

static void PrintUsage(int rc, int fd) {
  tinyprint(fd, "Usage: ", firstnonnull(program_invocation_name, "unknown"),
            USAGE, NULL);
  exit(rc);
}

static void GetOpts(int argc, char *argv[]) {
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
        PrintUsage(0, 1);
      default:
        PrintUsage(1, 2);
    }
  }
}

#pragma weak main

/**
 * Generic test program main function.
 */
dontasan int main(int argc, char *argv[]) {
  int fd;
  struct Dll *e;
  struct TestAspect *a;

  __log_level = kLogInfo;
  GetOpts(argc, argv);

  for (fd = 3; fd < 10; ++fd) {
    close(fd);
  }

#ifndef TINY
  setenv("GDB", "", true);
  GetSymbolTable();
#endif
  ShowCrashReports();

  // global setup
  errno = 0;
  STRACE("");
  STRACE("# setting up once");
  if (!IsWindows()) sys_getpid();
  testlib_clearxmmregisters();
  if (_weaken(SetUpOnce)) {
    _weaken(SetUpOnce)();
  }
  for (e = dll_first(testlib_aspects); e; e = dll_next(testlib_aspects, e)) {
    a = TESTASPECT_CONTAINER(e);
    if (a->once && a->setup) {
      a->setup(0);
    }
  }

  // run tests
  CheckStackIsAligned();
  testlib_runalltests();

  // run benchmarks
  if (!g_testlib_failed && runbenchmarks_ &&
      _weaken(testlib_runallbenchmarks)) {
    _weaken(testlib_runallbenchmarks)();
  }

  // global teardown
  STRACE("");
  STRACE("# tearing down once");
  for (e = dll_last(testlib_aspects); e; e = dll_prev(testlib_aspects, e)) {
    a = TESTASPECT_CONTAINER(e);
    if (a->once && a->teardown) {
      a->teardown(0);
    }
  }
  if (_weaken(TearDownOnce)) {
    _weaken(TearDownOnce)();
  }

  // make sure threads are in a good state
  if (_weaken(_pthread_decimate)) {
    _weaken(_pthread_decimate)();
  }
  if (_weaken(pthread_orphan_np) && !_weaken(pthread_orphan_np)()) {
    tinyprint(2, "error: tests ended with threads still active\n", NULL);
    _Exit(1);
  }

  // check for memory leaks
  if (IsAsan() && !g_testlib_failed) {
    CheckForMemoryLeaks();
  }

  // we're done!
  int status = MIN(255, g_testlib_failed);
  if (!status && IsRunningUnderMake()) {
    return 254;  // compile.com considers this 0 and propagates output
  } else if (!status && _weaken(pthread_exit)) {
    _weaken(pthread_exit)(0);
  } else {
    return status;
  }
}
