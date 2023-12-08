/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

int pfds[2];
pthread_cond_t cv;
pthread_mutex_t mu;
atomic_int gotcleanup;
char *wouldleak;
pthread_key_t key;
bool key_destructor_was_run;
atomic_int is_in_infinite_loop;

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void OnCleanup(void *arg) {
  gotcleanup = true;
}

void KeyDestructor(void *arg) {
  CheckStackIsAligned();
  ASSERT_EQ(31337, (intptr_t)arg);
  key_destructor_was_run = true;
}

void *CancelSelfWorkerDeferred(void *arg) {
  char buf[8];
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
  pthread_cleanup_push(OnCleanup, 0);
  pthread_cancel(pthread_self());
  read(pfds[0], buf, sizeof(buf));
  pthread_cleanup_pop(0);
  return 0;
}

TEST(pthread_cancel, self_deferred_waitsForCancelationPoint) {
  void *rc;
  pthread_t th;
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_EQ(0, pthread_create(&th, 0, CancelSelfWorkerDeferred, 0));
  ASSERT_EQ(0, pthread_join(th, &rc));
  ASSERT_EQ(PTHREAD_CANCELED, rc);
  ASSERT_TRUE(gotcleanup);
  ASSERT_SYS(0, 0, close(pfds[1]));
  ASSERT_SYS(0, 0, close(pfds[0]));
}
