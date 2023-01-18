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
#include "libc/calls/calls.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/log/check.h"
#include "libc/log/color.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/runtime/sysconf.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "third_party/dlmalloc/dlmalloc.h"
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
STATIC_YOINK("GetSymbolByAddr");
STATIC_YOINK("testlib_quota_handlers");
STATIC_YOINK("stack_usage_logging");

static bool runbenchmarks_;

void PrintUsage(int rc, FILE *f) {
  fputs("Usage: ", f);
  fputs(firstnonnull(program_invocation_name, "unknown"), f);
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

static void EmptySignalMask(void) {
  sigset_t ss;
  sigemptyset(&ss);
  sigprocmask(SIG_SETMASK, &ss, 0);
}

static void FixIrregularFds(void) {
  int e, i, fd, maxfds;
  struct rlimit rlim;
  struct pollfd *pfds;
  for (i = 0; i < 3; ++i) {
    if (fcntl(i, F_GETFL) == -1) {
      errno = 0;
      fd = open("/dev/null", O_RDWR);
      CHECK_NE(-1, fd);
      if (fd != i) {
        close(fd);
      }
    }
  }
  e = errno;
  if (!closefrom(3)) return;
  errno = e;
  if (IsWindows()) {
    maxfds = 64;
  } else {
    maxfds = 256;
    if (!getrlimit(RLIMIT_NOFILE, &rlim)) {
      maxfds = MIN(maxfds, (uint64_t)rlim.rlim_cur);
    }
  }
  pfds = malloc(maxfds * sizeof(struct pollfd));
  for (i = 0; i < maxfds; ++i) {
    pfds[i].fd = i + 3;
    pfds[i].events = POLLIN;
  }
  if (poll(pfds, maxfds, 0) != -1) {
    for (i = 0; i < maxfds; ++i) {
      if (pfds[i].revents & POLLNVAL) continue;
      CHECK_EQ(0, close(pfds[i].fd));
    }
  }
  free(pfds);
}

static void SetLimit(int resource, uint64_t soft, uint64_t hard) {
  struct rlimit old;
  struct rlimit lim = {soft, hard};
  if (resource == 127) return;
  if (setrlimit(resource, &lim) == -1) {
    if (!getrlimit(resource, &old)) {
      lim.rlim_max = MIN(hard, old.rlim_max);
      lim.rlim_cur = MIN(soft, lim.rlim_max);
      setrlimit(resource, &lim);
    }
  }
}

#pragma weak main

/**
 * Generic test program main function.
 */
noasan int main(int argc, char *argv[]) {
  unsigned cpus;
  const char *comdbg;
  __log_level = kLogInfo;
  GetOpts(argc, argv);
  setenv("GDB", "", true);
  GetSymbolTable();

  // normalize this process
  FixIrregularFds();
  EmptySignalMask();
  ShowCrashReports();

  // now get down to business
  g_testlib_shoulddebugbreak = IsDebuggerPresent(false);
  if (!IsWindows()) sys_getpid();  // make strace easier to read
  testlib_clearxmmregisters();
  testlib_runalltests();
  if (!g_testlib_failed && runbenchmarks_ &&
      _weaken(testlib_runallbenchmarks)) {
    _weaken(testlib_runallbenchmarks)();
    if (IsAsan() && !g_testlib_failed) {
      CheckForMemoryLeaks();
    }
    if (!g_testlib_failed && IsRunningUnderMake()) {
      return 254;  // compile.com considers this 0 and propagates output
    }
  } else if (IsAsan() && !g_testlib_failed) {
    CheckForMemoryLeaks();
  }

  // we're done!
  exit(min(255, g_testlib_failed));
}
