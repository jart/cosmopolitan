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
#include "libc/errno.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/mu.h"

int pos;
int count;
int limit;
long data[1000];
nsync_mu mu;
nsync_cv non_full;
nsync_cv non_empty;

int Put(long v, nsync_time abs_deadline) {
  int err, added = 0, wake = 0;
  nsync_mu_lock(&mu);
  while (count == limit) {
    if ((err = nsync_cv_wait_with_deadline(&non_full, &mu, abs_deadline, 0))) {
      ASSERT_EQ(ETIMEDOUT, err);
      ASSERT_NE(0, nsync_time_cmp(nsync_time_no_deadline, abs_deadline));
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
  nsync_mu_unlock(&mu);
  if (wake) nsync_cv_broadcast(&non_empty);
  return added;
}

long Get(nsync_time abs_deadline) {
  long err, v = 0;
  nsync_mu_lock(&mu);
  while (!count) {
    if ((err = nsync_cv_wait_with_deadline(&non_empty, &mu, abs_deadline, 0))) {
      ASSERT_EQ(ETIMEDOUT, err);
      ASSERT_NE(0, nsync_time_cmp(nsync_time_no_deadline, abs_deadline));
    }
  }
  if (count) {
    v = data[pos];
    data[pos] = 0;
    if (count == limit) {
      nsync_cv_broadcast(&non_full);
    }
    pos++;
    count--;
    if (pos == limit) {
      pos = 0;
    }
  }
  nsync_mu_unlock(&mu);
  return v;
}

#define N 10000

void *Producer(void *arg) {
  for (int i = 0; i < N; i++) {
    ASSERT_EQ(1, Put((i + 1) * 3, nsync_time_no_deadline));
  }
  return 0;
}

void *Consumer(void *arg) {
  for (int i = 0; i < N; i++) {
    ASSERT_EQ((i + 1) * 3, Get(nsync_time_no_deadline));
  }
  return 0;
}

TEST(cond, test) {
  pthread_t t;
  nsync_mu_init(&mu);

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
