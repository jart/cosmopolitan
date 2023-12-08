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
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"

int pos;
int count;
int limit;
long data[1000];
pthread_mutex_t mu;
pthread_cond_t non_full;
pthread_cond_t non_empty;

int Put(long v, struct timespec *abs_deadline) {
  int err, added = 0, wake = 0;
  pthread_mutex_lock(&mu);
  while (count == limit) {
    if ((err = pthread_cond_timedwait(&non_full, &mu, abs_deadline))) {
      ASSERT_EQ(ETIMEDOUT, err);
      ASSERT_NE(NULL, abs_deadline);
    }
  }
  if (count != limit) {
    int i = pos + count;
    if (limit <= i) i -= limit;
    data[i] = v;
    if (count == 0) wake = 1;
    count++;
    added = 1;
  }
  pthread_mutex_unlock(&mu);
  if (wake) pthread_cond_broadcast(&non_empty);
  return added;
}

long Get(struct timespec *abs_deadline) {
  long err, v = 0;
  pthread_mutex_lock(&mu);
  while (!count) {
    if ((err = pthread_cond_timedwait(&non_empty, &mu, abs_deadline))) {
      ASSERT_EQ(ETIMEDOUT, err);
      ASSERT_NE(NULL, abs_deadline);
    }
  }
  if (count) {
    v = data[pos];
    data[pos] = 0;
    if (count == limit) {
      pthread_cond_broadcast(&non_full);
    }
    pos++;
    count--;
    if (pos == limit) {
      pos = 0;
    }
  }
  pthread_mutex_unlock(&mu);
  return v;
}

#define N 1000

void *Producer(void *arg) {
  for (int i = 0; i < N; i++) {
    ASSERT_EQ(1, Put((i + 1) * 3, 0));
  }
  return 0;
}

void *Consumer(void *arg) {
  for (int i = 0; i < N; i++) {
    ASSERT_EQ((i + 1) * 3, Get(0));
  }
  return 0;
}

TEST(cond, test) {
  pthread_t t;

  limit = 1;
  ASSERT_EQ(0, pthread_create(&t, 0, Producer, 0));
  Consumer(0);
  ASSERT_EQ(0, pthread_join(t, 0));

  limit = 10;
  ASSERT_EQ(0, pthread_create(&t, 0, Producer, 0));
  Consumer(0);
  ASSERT_EQ(0, pthread_join(t, 0));

  limit = 100;
  ASSERT_EQ(0, pthread_create(&t, 0, Producer, 0));
  Consumer(0);
  ASSERT_EQ(0, pthread_join(t, 0));

  limit = 1000;
  ASSERT_EQ(0, pthread_create(&t, 0, Producer, 0));
  Consumer(0);
  ASSERT_EQ(0, pthread_join(t, 0));
}
