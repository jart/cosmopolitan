// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

volatile int ready = 0;
volatile int waiting = 0;
volatile int signaled = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *waiter_thread(void *arg) {
  pthread_mutex_lock(&mutex);
  waiting = 1;

  while (!ready) {
    if (pthread_cond_wait(&cond, &mutex) != 0) {
      pthread_mutex_unlock(&mutex);
      return (void *)1;
    }
  }

  pthread_mutex_unlock(&mutex);
  return (void *)0;
}

void *signaler_thread(void *arg) {
  usleep(1000);

  pthread_mutex_lock(&mutex);
  ready = 1;
  signaled = 1;
  pthread_mutex_unlock(&mutex);

  if (pthread_cond_signal(&cond) != 0)
    return (void *)2;

  return (void *)0;
}

int main() {
  pthread_t waiter, signaler;
  void *waiter_result, *signaler_result;
  int i;

  for (i = 0; i < 10; i++) {
    ready = 0;
    waiting = 0;
    signaled = 0;

    if (pthread_create(&waiter, NULL, waiter_thread, NULL) != 0)
      return 1;

    while (!waiting)
      usleep(100);

    if (pthread_create(&signaler, NULL, signaler_thread, NULL) != 0)
      return 2;

    if (pthread_join(waiter, &waiter_result) != 0)
      return 3;

    if (pthread_join(signaler, &signaler_result) != 0)
      return 4;

    if (waiter_result != (void *)0)
      return 5;

    if (signaler_result != (void *)0)
      return 6;

    if (!ready || !signaled)
      return 7;
  }

  return 0;
}