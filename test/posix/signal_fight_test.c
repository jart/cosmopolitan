// Copyright 2024 Justine Alexandra Roberts Tunney
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

#include <pthread.h>
#include <signal.h>
#include <stdlib.h>

/**
 * @fileoverview Tests two threads killing each other won't deadlock.
 *
 * Our Windows implementation of signals has surprisingly high
 * throughput on this test. About 10x more signals get delivered than
 * any other OS and in the same amount of time. The only exception was
 * OpenBSD, which delivered a similar number of signals, but it took 10x
 * longer for the process to execute.
 */

#define ITERATIONS 10000

int gotsigs[2];
pthread_t threads[2];
pthread_t thread_ids[2];
pthread_barrier_t barrier;
pthread_barrier_t barrier2;

void sig_handler(int signo) {
  if (pthread_equal(pthread_self(), threads[0]))
    ++gotsigs[0];
  if (pthread_equal(pthread_self(), threads[1]))
    ++gotsigs[1];
}

void *thread_func(void *arg) {
  int idx = *(int *)arg;
  int other_idx = 1 - idx;

  thread_ids[idx] = pthread_self();

  int s = pthread_barrier_wait(&barrier);
  if (s != 0 && s != PTHREAD_BARRIER_SERIAL_THREAD)
    exit(1);

  pthread_t other_thread = thread_ids[other_idx];

  for (int i = 0; i < ITERATIONS; ++i)
    if (pthread_kill(other_thread, SIGUSR1))
      exit(2);

  s = pthread_barrier_wait(&barrier2);
  if (s != 0 && s != PTHREAD_BARRIER_SERIAL_THREAD)
    exit(1);

  return 0;
}

int main() {
  struct sigaction sa;
  sa.sa_handler = sig_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGUSR1, &sa, 0) == -1)
    exit(3);

  if (pthread_barrier_init(&barrier, 0, 2))
    exit(4);
  if (pthread_barrier_init(&barrier2, 0, 2))
    exit(4);

  int idx0 = 0, idx1 = 1;

  if (pthread_create(&threads[0], 0, thread_func, &idx0))
    exit(5);
  if (pthread_create(&threads[1], 0, thread_func, &idx1))
    exit(6);

  if (pthread_join(threads[0], 0))
    exit(7);
  if (pthread_join(threads[1], 0))
    exit(8);

  if (pthread_barrier_destroy(&barrier2))
    exit(9);
  if (pthread_barrier_destroy(&barrier))
    exit(9);

  if (!gotsigs[0])
    exit(10);
  if (!gotsigs[1])
    exit(11);

  return 0;
}
