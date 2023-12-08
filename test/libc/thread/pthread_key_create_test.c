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
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

pthread_key_t mykey;
int destructor_calls;

void KeyDestructor(void *arg) {
  EXPECT_EQ(0, pthread_getspecific(mykey));
  ASSERT_EQ(31337, (intptr_t)arg);
  ++destructor_calls;
}

void *Worker(void *arg) {
  ASSERT_EQ(0, pthread_getspecific(mykey));
  ASSERT_EQ(0, pthread_setspecific(mykey, (void *)31337));
  ASSERT_EQ((void *)31337, pthread_getspecific(mykey));
  return 0;
}

TEST(pthread_key_create, test) {
  pthread_t th;
  destructor_calls = 0;
  ASSERT_EQ(0, pthread_key_create(&mykey, KeyDestructor));
  ASSERT_EQ(0, pthread_setspecific(mykey, (void *)666));
  ASSERT_EQ(0, pthread_create(&th, 0, Worker, 0));
  ASSERT_EQ(0, pthread_join(th, 0));
  ASSERT_EQ(1, destructor_calls);
  ASSERT_EQ((void *)666, pthread_getspecific(mykey));
  ASSERT_EQ(0, pthread_key_delete(mykey));
}

void KeyDestructorCraze(void *arg) {
  EXPECT_EQ(0, pthread_getspecific(mykey));
  ASSERT_EQ(31337, (intptr_t)arg);
  EXPECT_EQ(0, pthread_setspecific(mykey, (void *)31337));
  ++destructor_calls;
}

TEST(pthread_key_create, destructorKeepsSettingKey_willHalt) {
  pthread_t th;
  destructor_calls = 0;
  ASSERT_EQ(0, pthread_key_create(&mykey, KeyDestructorCraze));
  ASSERT_EQ(0, pthread_setspecific(mykey, (void *)666));
  ASSERT_EQ(0, pthread_create(&th, 0, Worker, 0));
  ASSERT_EQ(0, pthread_join(th, 0));
  ASSERT_EQ(PTHREAD_DESTRUCTOR_ITERATIONS, destructor_calls);
  ASSERT_EQ((void *)666, pthread_getspecific(mykey));
  ASSERT_EQ(0, pthread_key_delete(mykey));
}
