/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/time/time.h"

static _Thread_local int tdata = 31337;
static _Thread_local int tbss;

static void *ReturnArg(void *arg) {
  return arg;
}

TEST(cthread_create, testJoinDeadlock) {
  ASSERT_SYS(0, EDEADLK, cthread_join(cthread_self(), 0));
}

TEST(cthread_create, testCreateReturnJoin) {
  if (IsOpenbsd()) return;  // TODO(jart): we've getting flakes
  void *exitcode;
  cthread_t thread;
  ASSERT_EQ(0, cthread_create(&thread, 0, ReturnArg, ReturnArg));
  ASSERT_EQ(0, cthread_join(thread, &exitcode));
  ASSERT_EQ(ReturnArg, exitcode);
}

static void *ExitArg(void *arg) {
  cthread_exit(arg);
}

TEST(cthread_create, testCreateExitJoin) {
  if (IsOpenbsd()) return;  // TODO(jart): we've getting flakes
  void *exitcode;
  cthread_t thread;
  ASSERT_EQ(0, cthread_create(&thread, 0, ExitArg, (void *)-31337));
  ASSERT_EQ(0, cthread_join(thread, &exitcode));
  ASSERT_EQ((void *)-31337, exitcode);
}

TEST(gcctls, size) {
  if (IsXnu()) return;      // TODO(jart): codemorph
  if (IsWindows()) return;  // TODO(jart): codemorph
  if (IsOpenbsd()) return;  // TODO(jart): we've getting flakes
  // schlep in .zip section too
  // make sure executable isn't too huge
  size_t size;
  int64_t x = 0;
  gmtime(&x);
  ASSERT_LT((uintptr_t)_tls_size, 8192);
  size = GetFileSize(GetProgramExecutableName());
  if (IsTiny()) {
    ASSERT_LT(size, 200 * 1024);
  } else if (IsModeDbg() || IsAsan()) {
    ASSERT_LT(size, 4 * 1024 * 1024);
  } else {
    ASSERT_LT(size, 500 * 1024);
  }
}

static void *TlsWorker(void *arg) {
  ASSERT_EQ(31337, tdata);
  ASSERT_EQ(0, tbss);
  return 0;
}

TEST(gcctls, worksAndIsNonInheritable) {
  if (IsXnu()) return;      // TODO(jart): codemorph
  if (IsWindows()) return;  // TODO(jart): codemorph
  if (IsOpenbsd()) return;  // TODO(jart): we've getting flakes
  void *exitcode;
  cthread_t thread;
  ASSERT_EQ(tdata, 31337);
  ASSERT_EQ(tbss, 0);
  tdata = 1337;
  tbss = 1337;
  ASSERT_EQ(0, cthread_create(&thread, 0, TlsWorker, (void *)-31337));
  ASSERT_EQ(0, cthread_join(thread, &exitcode));
  ASSERT_EQ(NULL, exitcode);
}
