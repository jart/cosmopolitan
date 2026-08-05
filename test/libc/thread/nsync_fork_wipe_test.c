/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/intrin/atomic.h"
#include "libc/calls/calls.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include <sys/wait.h>

// Reproducer for nsync_waiter_wipe_ bug:
// After fork(), nsync_waiter_wipe_() zeroes the word of every mutex
// that has a waiter, even if the surviving (forking) thread holds it.
// This causes nsync_mu_unlock to panic with:
//   "attempt to nsync_mu_unlock() an nsync_mu not held in write mode"

static pthread_mutex_t shared_mu = PTHREAD_MUTEX_INITIALIZER;
static atomic_int worker_waiting;

static void *worker_thread(void *arg) {
  atomic_store_explicit(&worker_waiting, 1, memory_order_release);
  // This will BLOCK because main thread holds shared_mu.
  // Blocking creates an nsync waiter with wipe_mu = &shared_mu.
  pthread_mutex_lock(&shared_mu);
  pthread_mutex_unlock(&shared_mu);
  return NULL;
}

TEST(nsync_fork_wipe, mutexHeldByForkerIsNotZeroed) {
  pthread_t th;
  pid_t pid;
  int status;

  atomic_store_explicit(&worker_waiting, 0, memory_order_relaxed);

  // Main thread acquires the mutex
  ASSERT_EQ(0, pthread_mutex_lock(&shared_mu));

  // Spawn a worker that will BLOCK trying to acquire the same mutex.
  // This creates an nsync waiter with wipe_mu pointing to shared_mu.
  ASSERT_EQ(0, pthread_create(&th, NULL, worker_thread, NULL));

  // Wait for worker to be contending on the mutex
  while (!atomic_load_explicit(&worker_waiting, memory_order_acquire))
    usleep(1000);
  // Give it a moment to actually enter the nsync wait
  usleep(50000);

  // Fork while holding shared_mu, with worker waiting on it.
  // In the child, nsync_waiter_wipe_() will zero shared_mu's word.
  pid = fork();
  ASSERT_NE(-1, pid);

  if (pid == 0) {
    // CHILD: shared_mu should still be held by us (the surviving thread).
    // This unlock should succeed, but nsync_waiter_wipe_ zeroed the word,
    // so nsync_mu_unlock will panic.
    int err = pthread_mutex_unlock(&shared_mu);
    _Exit(err ? 1 : 0);
  }

  // PARENT: clean up
  ASSERT_NE(-1, waitpid(pid, &status, 0));
  ASSERT_TRUE(WIFEXITED(status));
  EXPECT_EQ(0, WEXITSTATUS(status));

  // Let worker finish
  pthread_mutex_unlock(&shared_mu);
  pthread_join(th, NULL);
}
