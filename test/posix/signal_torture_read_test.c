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
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @fileoverview i/o signal handling torture test
 *
 * This test tries to trigger race conditions in the kernel's read()
 * implementation, by sending a massive amount of SA_RESTART signals
 * which cause churn in its internal code, and finally an interrupt.
 * This should reveal if the kernel code that checks for any pending
 * signals before blocking on i/o happens non-atomically. Expect the
 * test to hang indefinitely in such cases.
 *
 * "This flag affects the behavior of interruptible functions; that is,
 *  those specified to fail with errno set to EINTR. If set, and a
 *  function specified as interruptible is interrupted by this signal,
 *  the function shall restart and shall not fail with EINTR unless
 *  otherwise specified. If an interruptible function which uses a
 *  timeout is restarted, the duration of the timeout following the
 *  restart is set to an unspecified value that does not exceed the
 *  original timeout value. If the flag is not set, interruptible
 *  functions interrupted by this signal shall fail with errno set to
 *  EINTR." —Quoth IEEE Std 1003.1-2017 (POSIX.1) on SA_RESTART
 *
 * Every OS except Windows fails this test.
 *
 * @see sys_readwrite_nt()
 */

#define COUNT 1000

volatile sig_atomic_t got_sigusr1;
volatile sig_atomic_t got_sigusr2;
volatile sig_atomic_t thread_ready;
volatile sig_atomic_t read_interrupted;

void sigusr1_handler(int signo) {
  ++got_sigusr1;
  // printf("got %d sigusr1\n", got_sigusr1);
}

void sigusr2_handler(int signo) {
  ++got_sigusr2;
  // printf("got %d sigusr2\n", got_sigusr2);
}

void setup_signal_handlers() {
  struct sigaction sa;

  // Set up SIGUSR1 handler with SA_RESTART
  sa.sa_handler = sigusr1_handler;
  sa.sa_flags = SA_RESTART;  // Signal handler with SA_RESTART
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGUSR1, &sa, NULL) == -1)
    exit(97);

  // Set up SIGUSR2 handler without SA_RESTART
  sa.sa_handler = sigusr2_handler;
  sa.sa_flags = 0;  // Signal handler without SA_RESTART
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGUSR2, &sa, NULL) == -1)
    exit(98);
}

void block_signals() {
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  sigaddset(&set, SIGUSR2);
  if (pthread_sigmask(SIG_BLOCK, &set, 0))
    exit(99);
}

void *thread_func(void *arg) {
  int *pipefd = (int *)arg;
  char buf[1];
  ssize_t ret;

  // Unblock SIGUSR1 and SIGUSR2 in this thread
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  sigaddset(&set, SIGUSR2);
  if (pthread_sigmask(SIG_UNBLOCK, &set, 0))
    exit(100);

  // Indicate that the thread is ready
  thread_ready = 1;

  // Call read() on the pipe
  ret = read(pipefd[0], buf, 1);
  if (ret == -1) {
    if (errno == EINTR) {
      read_interrupted = 1;
      // printf("read interrupted\n");
    } else {
      perror("read");
      exit(78);
    }
  } else {
    exit(77);
  }

  return NULL;
}

int main() {
  int pipefd[2];
  pthread_t thread;

  // this test exposes bugs in macos
  if (IsXnu())
    return 0;

  // this test exposes bugs in linux
  if (IsLinux())
    return 0;

  // this test exposes bugs in netbsd
  if (IsNetbsd())
    return 0;

  // this test exposes bugs in freebsd
  if (IsFreebsd())
    return 0;

  // this test exposes bugs in openbsd
  if (IsOpenbsd())
    return 0;

  ShowCrashReports();

  // Block SIGUSR1 and SIGUSR2 in the main thread
  block_signals();

  // Set up signal handlers
  setup_signal_handlers();

  // Create a pipe
  if (pipe(pipefd) == -1)
    exit(95);

  // Create a thread
  if (pthread_create(&thread, NULL, thread_func, pipefd) != 0)
    exit(90);

  // Wait until the thread is ready
  while (!thread_ready)
    if (pthread_yield_np())
      exit(101);

  // Send SIGUSR1 signals
  // This will cause read() to restart internally
  for (int i = 0; i < COUNT; i++) {
    if (pthread_kill(thread, SIGUSR1) != 0)
      exit(91);
    if (i % (COUNT / 10) == 0)
      usleep(1);
  }

  // Send SIGUSR2 to the thread
  // This will trigger an EINTR
  fflush(stdout);
  if (pthread_kill(thread, SIGUSR2))
    exit(92);

  // Join the thread
  if (pthread_join(thread, NULL))
    exit(93);

  // Close the pipe
  close(pipefd[0]);
  close(pipefd[1]);

  // Check if read() was interrupted by EINTR
  if (!read_interrupted)
    exit(94);

  if (!got_sigusr1)
    exit(60);
  if (!got_sigusr2)
    exit(61);

  // printf("got %d got_sigusr1\n", got_sigusr1);

  CheckForMemoryLeaks();
  return 0;
}
