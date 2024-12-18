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

#include <cosmo.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

// tests that recursive mutexes are implemented atomically
//
// glibc fails this test
// musl passes this test
// cosmo only guarantees this in process-shared non-debug mode

atomic_bool done;
atomic_bool ready;
pthread_mutex_t lock;

void hand(int sig) {
  if (pthread_mutex_lock(&lock))
    _Exit(50);
  if (pthread_mutex_unlock(&lock))
    _Exit(51);
}

void* work(void* arg) {
  ready = true;
  while (!done) {
    if (pthread_mutex_lock(&lock))
      _Exit(60);
    if (pthread_mutex_unlock(&lock))
      _Exit(61);
  }
  return 0;
}

int main() {

  if (IsQemuUser()) {
    // qemu is believed to be the one at fault
    kprintf("mutex_async_signal_safety_test flakes on qemu\n");
    return 0;
  }

  if (IsModeDbg()) {
    // the deadlock detector gets in the way of our glorious spin lock
    kprintf("mutex_async_signal_safety_test not feasible in debug mode\n");
    return 0;
  }

  struct sigaction sa;
  sa.sa_handler = hand;
  sa.sa_flags = SA_NODEFER;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGUSR1, &sa, 0))
    _Exit(1);

  pthread_mutexattr_t attr;
  if (pthread_mutexattr_init(&attr))
    _Exit(2);
  if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE))
    _Exit(3);
  if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED))
    _Exit(3);
  if (pthread_mutex_init(&lock, &attr))
    _Exit(4);
  if (pthread_mutexattr_destroy(&attr))
    _Exit(5);

  pthread_t th;
  pthread_attr_t tattr;
  if (pthread_attr_init(&tattr))
    _Exit(6);
  if (pthread_attr_setstacksize(&tattr, 8 * 1024 * 1024))
    _Exit(7);
  if (pthread_attr_setguardsize(&tattr, 64 * 1024))
    _Exit(8);
  if (pthread_create(&th, &tattr, work, 0))
    _Exit(9);
  if (pthread_attr_destroy(&tattr))
    _Exit(10);
  for (;;)
    if (ready)
      break;

  for (int i = 0; i < 100; ++i) {
    if (pthread_kill(th, SIGUSR1))
      _Exit(11);
    if (pthread_kill(th, SIGUSR1))
      _Exit(12);
    usleep(1);
  }

  done = true;
  if (pthread_join(th, 0))
    _Exit(13);
  if (pthread_mutex_destroy(&lock))
    _Exit(14);
}
