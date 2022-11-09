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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/log/check.h"
#include "libc/log/internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/process.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/w.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/x/x.h"

static int x;
char g_testlib_olddir[PATH_MAX];
char g_testlib_tmpdir[PATH_MAX];
struct sigaction wanthandlers[31];
static pthread_mutex_t testlib_error_lock;

void testlib_finish(void) {
  if (g_testlib_failed) {
    fprintf(stderr, "%u / %u %s\n", g_testlib_failed, g_testlib_ran,
            "tests failed");
  }
}

void testlib_error_enter(const char *file, const char *func) {
  ftrace_enabled(-1);
  strace_enabled(-1);
  pthread_mutex_lock(&testlib_error_lock);
  if (!IsWindows()) sys_getpid(); /* make strace easier to read */
  if (!IsWindows()) sys_getpid();
  if (g_testlib_shoulddebugbreak) {
    DebugBreak();
  }
  testlib_showerror_file = file;
  testlib_showerror_func = func;
}

void testlib_error_leave(void) {
  strace_enabled(+1);
  ftrace_enabled(+1);
  pthread_mutex_unlock(&testlib_error_lock);
}

wontreturn void testlib_abort(void) {
  testlib_finish();
  _Exitr(MAX(1, MIN(255, g_testlib_failed)));
}

static void SetupTmpDir(void) {
  char *p = g_testlib_tmpdir;
  p = stpcpy(p, kTmpPath);
  p = stpcpy(p, program_invocation_short_name), *p++ = '.';
  p = FormatInt64(p, getpid()), *p++ = '.';
  p = FormatInt64(p, x++);
  p[0] = '\0';
  CHECK_NE(-1, makedirs(g_testlib_tmpdir, 0755), "%s", g_testlib_tmpdir);
  CHECK_NOTNULL(realpath(g_testlib_tmpdir, g_testlib_tmpdir), "%`'s",
                g_testlib_tmpdir);
  CHECK_NE(-1, chdir(g_testlib_tmpdir), "%s", g_testlib_tmpdir);
}

static void TearDownTmpDir(void) {
  CHECK_NE(-1, chdir(g_testlib_olddir));
  CHECK_NE(-1, rmrf(g_testlib_tmpdir), "%s", g_testlib_tmpdir);
}

static void DoNothing(int sig) {
  // function intentionally empty
}

static void CopySignalHandlers(void) {
#if 0
  int i;
  for (i = 0; i < ARRAYLEN(wanthandlers); ++i) {
    if (i + 1 == SIGKILL || i + 1 == SIGSTOP) continue;
    CHECK_EQ(0, sigaction(i + 1, 0, wanthandlers + i), "sig=%d", i + 1);
  }
#endif
}

static void CheckSignalHandler(int sig) {
#if 0
  int i;
  struct sigaction sa = {0};
  _unassert(0 <= sig - 1 && sig - 1 < ARRAYLEN(wanthandlers));
  CHECK_EQ(0, sigaction(sig, 0, &sa));
  CHECK_EQ(0, memcmp(wanthandlers + sig - 1, &sa, sizeof(sa)),
           "signal handler for %s was %p/%#x/%#x:%x "
           "but should have been restored to %p/%#x/%#x:%x",
           strsignal(sig), sa.sa_handler, sa.sa_flags, sa.sa_mask.__bits[0],
           sa.sa_mask.__bits[1], wanthandlers[sig - 1].sa_handler,
           wanthandlers[sig - 1].sa_flags,
           wanthandlers[sig - 1].sa_mask.__bits[0],
           wanthandlers[sig - 1].sa_mask.__bits[1]);
#endif
}

static void CheckForSignalHandlers(void) {
#if 0
  CheckSignalHandler(SIGINT);
  CheckSignalHandler(SIGQUIT);
  CheckSignalHandler(SIGCHLD);
  CheckSignalHandler(SIGFPE);
  CheckSignalHandler(SIGILL);
  CheckSignalHandler(SIGSEGV);
  CheckSignalHandler(SIGTRAP);
  CheckSignalHandler(SIGABRT);
  CheckSignalHandler(SIGBUS);
  CheckSignalHandler(SIGSYS);
  CheckSignalHandler(SIGWINCH);
#endif
}

static void CheckForFileDescriptors(void) {
#if 0
  // TODO: race condition on fd cleanup :'(
  int i;
  struct pollfd pfds[16];
  if (!_weaken(open) && !_weaken(socket)) return;
  for (i = 0; i < ARRAYLEN(pfds); ++i) {
    pfds[i].fd = i + 3;
    pfds[i].events = POLLIN;
  }
  if (poll(pfds, ARRAYLEN(pfds), 0) > 0) {
    for (i = 0; i < ARRAYLEN(pfds); ++i) {
      if (pfds[i].revents & POLLNVAL) continue;
      ++g_testlib_failed;
      fprintf(stderr, "error: test failed to close() fd %d\n", pfds[i].fd);
    }
  }
#endif
}

static void CheckForZombies(void) {
#if 0
  int e, pid;
  sigset_t ss, oldss;
  struct sigaction oldsa;
  struct sigaction ignore = {.sa_handler = DoNothing};
  if (!_weaken(fork)) return;
  for (;;) {
    if ((pid = wait(0)) == -1) {
      CHECK_EQ(ECHILD, errno);
      break;
    } else {
      ++g_testlib_failed;
      fprintf(stderr, "error: test failed to reap zombies %d\n", pid);
    }
  }
#endif
}

/**
 * Runs all test case functions in sorted order.
 */
void testlib_runtestcases(testfn_t *start, testfn_t *end, testfn_t warmup) {
  /*
   * getpid() calls are inserted to help visually see tests in traces
   * which can be performed on Linux, FreeBSD, OpenBSD, and XNU:
   *
   *     strace -f o/default/test.com |& less
   *     truss o/default/test.com |& less
   *     ktrace -f trace o/default/test.com </dev/null; kdump -f trace | less
   *     dtruss o/default/test.com |& less
   *
   * Test cases are iterable via a decentralized section. Your TEST()
   * macro inserts .testcase.SUITENAME sections into the binary which
   * the linker sorts into an array.
   *
   * @see ape/ape.lds
   */
  const testfn_t *fn;
  CopySignalHandlers();
  if (_weaken(testlib_enable_tmp_setup_teardown) ||
      _weaken(testlib_enable_tmp_setup_teardown_once)) {
    CHECK_NOTNULL(getcwd(g_testlib_olddir, sizeof(g_testlib_olddir)));
  }
  if (_weaken(testlib_enable_tmp_setup_teardown_once)) {
    SetupTmpDir();
  }
  if (_weaken(SetUpOnce)) _weaken(SetUpOnce)();
  for (x = 0, fn = start; fn != end; ++fn) {
    STRACE("");
    STRACE("# setting up %t", fn);
    if (_weaken(testlib_enable_tmp_setup_teardown)) SetupTmpDir();
    if (_weaken(SetUp)) _weaken(SetUp)();
    errno = 0;
    SetLastError(0);
    if (!IsWindows()) sys_getpid();
    if (warmup) warmup();
    testlib_clearxmmregisters();
    STRACE("");
    STRACE("# running test %t", fn);
    (*fn)();
    STRACE("");
    STRACE("# tearing down %t", fn);
    if (!IsWindows()) sys_getpid();
    if (_weaken(TearDown)) _weaken(TearDown)();
    if (_weaken(testlib_enable_tmp_setup_teardown)) TearDownTmpDir();
    CheckForFileDescriptors();
    CheckForSignalHandlers();
    CheckForZombies();
  }
  if (_weaken(TearDownOnce)) {
    _weaken(TearDownOnce)();
  }
  if (_weaken(testlib_enable_tmp_setup_teardown_once)) {
    TearDownTmpDir();
  }
}
