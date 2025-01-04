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
#include "libc/calls/struct/cpuset.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/getenv.h"
#include "libc/intrin/safemacros.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/ubsan.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/leaks.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/aspect.internal.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"
#include "third_party/getopt/getopt.internal.h"

#pragma weak main

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

static int rando(void) {
  return _rand64() & INT_MAX;
}

static void limit_process_to_single_cpu(void) {
  extern int disable_limit_process_to_single_cpu;
  if (_weaken(disable_limit_process_to_single_cpu))
    return;
  if (!(IsLinux() || IsFreebsd() || IsNetbsd() || IsWindows()))
    return;
  if (IsFreebsd() && getuid())
    return;
  cpu_set_t legal;
  if (sched_getaffinity(0, sizeof(cpu_set_t), &legal) == -1) {
    perror("sched_setaffinity failed");
    exit(1);
  }
  int count = CPU_COUNT(&legal);
  cpu_set_t newset;
  CPU_ZERO(&newset);
  bool done = false;
  while (!done) {
    for (int i = 0; i < CPU_SETSIZE; ++i) {
      if (CPU_ISSET(i, &legal) && !(rando() % count)) {
        CPU_SET(rando() % count, &newset);
        done = true;
        break;
      }
    }
  }
  if (sched_setaffinity(0, sizeof(cpu_set_t), &newset) == -1) {
    perror("sched_setaffinity failed");
    exit(1);
  }
}

/**
 * Generic test program main function.
 */
int main(int argc, char *argv[]) {
  int fd;
  struct Dll *e;
  struct TestAspect *a;

  // some settings
  __ubsan_strict = true;
  __log_level = kLogInfo;

  if (errno) {
    tinyprint(2, "error: the errno variable was contaminated by constructors\n",
              NULL);
    return 1;
  }

  // // this sometimes helps tease out mt bugs
  // limit_process_to_single_cpu();

  // test huge pointers by enabling pml5t
  if (rando() % 2) {
    errno_t e = errno;
    mmap((char *)0x80000000000000, 1, PROT_NONE,  //
         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    errno = e;
  }

  GetOpts(argc, argv);

  int oe = errno;
  for (fd = 3; fd < 100; ++fd)
    close(fd);
  errno = oe;

#ifndef TINY
  setenv("GDB", "", true);
  GetSymbolTable();
#endif
  ShowCrashReports();

  // global setup
  errno = 0;
  STRACE("");
  STRACE("# setting up once");
  if (!IsWindows())
    sys_getpid();
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
  if (_weaken(TearDownOnce))
    _weaken(TearDownOnce)();

  // make sure threads are in a good state
  if (_weaken(_pthread_decimate))
    _weaken(_pthread_decimate)(kPosixThreadZombie);
  if (_weaken(pthread_orphan_np) && !_weaken(pthread_orphan_np)()) {
    tinyprint(2, "error: tests ended with threads still active\n", NULL);
    _Exit(1);
  }

  // check for memory leaks
  AssertNoLocksAreHeld();
  if (!g_testlib_failed)
    CheckForMemoryLeaks();

  // we're done!
  int status = MIN(255, g_testlib_failed);
  if (!status && IsRunningUnderMake()) {
    return 254;  // compile considers this 0 and propagates output
  } else if (!status && _weaken(pthread_exit)) {
    _weaken(pthread_exit)(0);
  } else {
    return status;
  }
}
