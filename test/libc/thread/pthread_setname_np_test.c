/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

void SetUpOnce(void) {

  if (IsXnu()) {
    tinyprint(2, program_invocation_short_name,
              ": no custom thread names on apple\n", NULL);
    exit(0);
  }

  if (IsWindows()) {
    tinyprint(2, program_invocation_short_name,
              ": no custom thread names on windows\n", NULL);
    exit(0);
  }

  if (IsLinux() || IsOpenbsd()) {
    char buf[512];
    if (pthread_getname_np(pthread_self(), buf, sizeof(buf)) == ENOSYS) {
      tinyprint(2, program_invocation_short_name,
                ": kernel too old for thread name support\n", NULL);
      exit(0);
    }
  }
}

static void *SetName(void *arg) {
  ASSERT_EQ(0, pthread_setname_np(pthread_self(), "justine"));
  return 0;
}

TEST(pthread_setname_np, SetName_SystemCallSucceeds) {
  pthread_t id;
  ASSERT_EQ(0, pthread_create(&id, 0, SetName, 0));
  ASSERT_EQ(0, pthread_join(id, 0));
}

static void *SetGetNameOfSelf(void *arg) {
  char me[16];
  ASSERT_EQ(0, pthread_setname_np(pthread_self(), "justine"));
  ASSERT_EQ(0, pthread_getname_np(pthread_self(), me, sizeof(me)));
  EXPECT_STREQ("justine", me);
  return 0;
}

TEST(pthread_setname_np, SetGetNameOfSelf) {
  pthread_t id;
  if (IsFreebsd()) {
    // no get thread name no freebsd (why??)
    return;
  }
  ASSERT_EQ(0, pthread_create(&id, 0, SetGetNameOfSelf, 0));
  ASSERT_EQ(0, pthread_join(id, 0));
}

atomic_char sync1, sync2;

static void *GetNameOfOtherThreadWorker(void *arg) {
  pthread_setname_np(pthread_self(), "justine");
  atomic_store(&sync1, 1);
  while (!atomic_load(&sync2)) pthread_yield();
  return 0;
}

TEST(pthread_setname_np, GetNameOfOtherThread) {
  char me[16];
  pthread_t id;
  if (IsFreebsd()) {
    // no get thread name no freebsd (why??)
    return;
  }
  ASSERT_EQ(0, pthread_create(&id, 0, GetNameOfOtherThreadWorker, 0));
  while (!atomic_load(&sync1)) pthread_yield();
  errno_t e = pthread_getname_np(id, me, sizeof(me));
  if (IsLinux() && e == ENOENT) goto GiveUp;  // bah old kernel
  if (IsLinux() && e == EACCES) goto GiveUp;  // meh landlock
  ASSERT_EQ(0, e);
  EXPECT_STREQ("justine", me);
  ASSERT_EQ(0, pthread_setname_np(id, "tunney"));
  ASSERT_EQ(0, pthread_getname_np(id, me, sizeof(me)));
  EXPECT_STREQ("tunney", me);
GiveUp:
  atomic_store(&sync2, 1);
  ASSERT_EQ(0, pthread_join(id, 0));
}
