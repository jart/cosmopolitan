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
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t cpu_signal_received = 0;
volatile sig_atomic_t io_signal_received = 0;

// Declare thread IDs
pthread_t cpu_thread_id;
pthread_t io_thread_id;

void signal_handler(int signum) {
  // Set the flag to indicate the signal was received
  if (pthread_equal(pthread_self(), cpu_thread_id)) {
    cpu_signal_received = 1;
  } else if (pthread_equal(pthread_self(), io_thread_id)) {
    io_signal_received = 1;
  }
}

void *cpu_bound_task(void *arg) {
  // Unblock SIGUSR1 for this thread
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  pthread_sigmask(SIG_UNBLOCK, &set, NULL);

  // Simulate CPU-bound work
  while (!cpu_signal_received) {
    // Busy-wait loop
  }
  return NULL;
}

void *io_bound_task(void *arg) {
  // Unblock SIGUSR1 for this thread
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  pthread_sigmask(SIG_UNBLOCK, &set, NULL);

  // Simulate I/O-bound work
  sleep(1);  // Use sleep to simulate waiting for I/O
  return NULL;
}

int main() {
  // Install the signal handler
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    // Failed to install signal handler
    exit(1);
  }

  // Block SIGUSR1 in the main thread
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
    // Failed to set signal mask
    exit(2);
  }

  // Create the CPU-bound and I/O-bound threads
  if (pthread_create(&cpu_thread_id, NULL, &cpu_bound_task, NULL) != 0) {
    // Failed to create the CPU-bound thread
    exit(3);
  }
  if (pthread_create(&io_thread_id, NULL, &io_bound_task, NULL) != 0) {
    // Failed to create the I/O-bound thread
    exit(4);
  }

  /* sleep(1); */

  // Raise the signal for both threads
  pthread_kill(cpu_thread_id, SIGUSR1);
  pthread_kill(io_thread_id, SIGUSR1);

  // Wait for both threads to finish
  pthread_join(cpu_thread_id, NULL);
  pthread_join(io_thread_id, NULL);

  // Check if both threads received the signal
  if (cpu_signal_received && io_signal_received) {
    // Both signals were successfully caught
    exit(0);
  } else {
    // One or both signals were not caught
    exit(5);
  }
}
