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

#include <cosmo.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

volatile sig_atomic_t signal_count = 0;
volatile sig_atomic_t nested_signal = 0;

void safe_signal_handler(int sig) {
  // Only use async-signal-safe functions in signal handlers
  signal_count++;
}

void unsafe_signal_handler(int sig) {
  // This tests if unsafe functions are protected
  nested_signal = 1;
  // malloc() is not async-signal-safe, but we test if it's protected
  void *ptr = malloc(10);
  if (ptr)
    free(ptr);
}

int main() {
  struct sigaction sa;
  sigset_t mask, oldmask;
  pid_t pid;
  int status;

  if (IsNetbsd())
    return 0;  // TODO: why?

  // Test 1: Basic signal mask functionality
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);

  if (sigprocmask(SIG_BLOCK, &mask, &oldmask) == 0) {
    printf("Test 1 PASS: Signal masking works\n");
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
  } else {
    printf("Test 1 FAIL: Signal masking failed\n");
    return 1;
  }

  // Test 2: Signal handler installation and removal
  sa.sa_handler = safe_signal_handler;
  sa.sa_flags = SA_RESTART;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGUSR1, &sa, NULL) == 0) {
    printf("Test 2 PASS: Signal handler installation works\n");
  } else {
    printf("Test 2 FAIL: Signal handler installation failed\n");
    return 2;
  }

  // Test 3: Self-signal delivery
  signal_count = 0;
  if (kill(getpid(), SIGUSR1) == 0) {
    usleep(1000);  // Allow signal delivery
    if (signal_count == 1) {
      printf("Test 3 PASS: Self-signal delivery works\n");
    } else {
      printf("Test 3 FAIL: Signal not delivered (count=%d)\n", signal_count);
      return 3;
    }
  } else {
    printf("Test 3 FAIL: kill() failed\n");
    return 3;
  }

  // Test 4: Signal blocking and pending
  signal_count = 0;
  sigprocmask(SIG_BLOCK, &mask, NULL);
  kill(getpid(), SIGUSR1);  // Signal should be blocked

  if (signal_count == 0) {
    printf("Test 4a PASS: Signal properly blocked\n");
  } else {
    printf("Test 4a FAIL: Signal not blocked\n");
    return 4;
  }

  sigprocmask(SIG_UNBLOCK, &mask, NULL);
  usleep(1000);  // Allow signal delivery

  if (signal_count == 1) {
    printf("Test 4b PASS: Pending signal delivered after unblock\n");
  } else {
    printf("Test 4b FAIL: Pending signal not delivered\n");
    return 4;
  }

  // Test 5: sigpending() functionality
  sigset_t pending;
  signal_count = 0;
  sigprocmask(SIG_BLOCK, &mask, NULL);
  kill(getpid(), SIGUSR1);

  if (sigpending(&pending) == 0 && sigismember(&pending, SIGUSR1)) {
    printf("Test 5 PASS: sigpending() detects pending signal\n");
  } else {
    printf("Test 5 FAIL: sigpending() failed\n");
    return 5;
  }

  sigprocmask(SIG_UNBLOCK, &mask, NULL);
  usleep(1000);  // Allow delivery

  // Test 6: Signal coalescing (multiple identical signals)
  signal_count = 0;
  sigprocmask(SIG_BLOCK, &mask, NULL);

  // Send multiple identical signals - they should coalesce
  kill(getpid(), SIGUSR1);
  kill(getpid(), SIGUSR1);
  kill(getpid(), SIGUSR1);

  sigprocmask(SIG_UNBLOCK, &mask, NULL);
  usleep(1000);

  if (signal_count == 1) {
    printf("Test 6 PASS: Multiple signals coalesced correctly\n");
  } else {
    printf("Test 6 WARNING: Signal coalescing behavior: count=%d\n",
           signal_count);
  }

  // Test 7: Fork and signal inheritance
  signal_count = 0;
  pid = fork();
  if (pid == 0) {
    // Child process
    usleep(10000);       // Give parent time to send signal
    exit(signal_count);  // Exit with signal count
  } else if (pid > 0) {
    // Parent process
    usleep(1000);
    kill(pid, SIGUSR1);  // Send signal to child

    if (waitpid(pid, &status, 0) == pid && WIFEXITED(status)) {
      if (WEXITSTATUS(status) == 1) {
        printf("Test 7 PASS: Child inherited signal handler\n");
      } else {
        printf("Test 7 FAIL: Child signal handling failed (exit=%d)\n",
               WEXITSTATUS(status));
        return 7;
      }
    } else {
      printf("Test 7 FAIL: Child process failed\n");
      return 7;
    }
  } else {
    printf("Test 7 FAIL: fork() failed\n");
    return 7;
  }

  // Test 8: Async-signal-safety (we test that unsafe functions don't crash)
  sa.sa_handler = unsafe_signal_handler;
  sigaction(SIGUSR2, &sa, NULL);

  nested_signal = 0;
  kill(getpid(), SIGUSR2);
  usleep(1000);

  if (nested_signal == 1) {
    printf("Test 8 WARNING: Unsafe operations in signal handler completed\n");
  } else {
    printf("Test 8 FAIL: Signal handler not executed\n");
    return 8;
  }

  printf("All critical signal handling tests completed!\n");
  return 0;
}
