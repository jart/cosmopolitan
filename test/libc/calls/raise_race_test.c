/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│                                                                              │
│ libc-test                                                                    │
│ Copyright © 2005-2013 libc-test AUTHORS                                      │
│                                                                              │
│ Permission is hereby granted, free of charge, to any person obtaining        │
│ a copy of this software and associated documentation files (the              │
│ "Software"), to deal in the Software without restriction, including          │
│ without limitation the rights to use, copy, modify, merge, publish,          │
│ distribute, sublicense, and/or sell copies of the Software, and to           │
│ permit persons to whom the Software is furnished to do so, subject to        │
│ the following conditions:                                                    │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY         │
│ CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,         │
│ TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE            │
│ SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                       │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

// libc-test/src/regression/raise-race.c
// commit: 370f78f2c80c64b7b0780a01e672494a26b5678e 2011-03-09
// commit: 0bed7e0acfd34e3fb63ca0e4d99b7592571355a9 2011-03-09
// raise should be robust against async fork in a signal handler
// [jart] i can't believe fork() is async-signal-safe

#define t_error(...)      \
  do {                    \
    kprintf(__VA_ARGS__); \
    ++t_status;           \
  } while (0)

static atomic_int c0;
static atomic_int c1;
static atomic_int child;
static atomic_int t_status;

static void handler0(int sig) {
  c0++;
}

static void handler1(int sig) {
  c1++;
  switch (fork()) {
    case 0:
      child = 1;
      break;
    case -1:
      t_error("fork failed: %s\n", strerror(errno));
    default:
      break;
  }
}

static void *start(void *arg) {
  int i, r, s;
  for (i = 0; i < 1000; i++) {
    r = raise(SIGRTMIN);
    if (r) t_error("raise failed: %s\n", strerror(errno));
  }
  if (c0 != 1000) {
    t_error("lost signals: got %d, wanted 1000 (ischild %d forks %d)\n", c0,
            child, c1);
  }
  if (child) _exit(t_status);
  /* make sure we got all pthread_kills, then wait the forked children */
  while (c1 < 100) donothing;
  for (i = 0; i < 100; i++) {
    r = wait(&s);
    if (r == -1) {
      t_error("wait failed: %s\n", strerror(errno));
    } else if (!WIFEXITED(s) || WTERMSIG(s)) {
      t_error("child failed: pid:%d status:%d sig:%s\n", r, s,
              strsignal(WTERMSIG(s)));
    }
  }
  return 0;
}

TEST(raise, test) {
  if (IsNetbsd()) return;   // why doesn't it work?
  if (IsOpenbsd()) return;  // no support for realtime signals yet
  if (IsXnu()) return;      // no support for realtime signals yet
  if (IsWindows()) return;  // TODO(jart): why does it exit 128+SIGRTMIN?
  void *p;
  int r, i, s;
  pthread_t t;
  if (signal(SIGRTMIN, handler0) == SIG_ERR)
    t_error("registering signal handler failed: %s\n", strerror(errno));
  if (signal(SIGRTMIN + 1, handler1) == SIG_ERR)
    t_error("registering signal handler failed: %s\n", strerror(errno));
  r = pthread_create(&t, 0, start, 0);
  if (r) t_error("pthread_create failed: %s\n", strerror(r));
  for (i = 0; i < 100; i++) {
    r = pthread_kill(t, SIGRTMIN + 1);
    if (r) t_error("phread_kill failed: %s\n", strerror(r));
  }
  r = pthread_join(t, &p);
  if (r) t_error("pthread_join failed: %s\n", strerror(r));
  ASSERT_EQ(0, t_status);
}
