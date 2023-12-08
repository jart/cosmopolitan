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

volatile sig_atomic_t signal_handled_count;

void reentrant_signal_handler(int signum) {

  // waste stack memory to test raise() won't recurse
  volatile char burn_stack[3500];
  burn_stack[3000] = 0;
  burn_stack[0] = 3;

  // Increment the count to indicate the signal was handled
  if (++signal_handled_count == 10000) return;

  // Re-raise the signal to test reentrancy
  raise(signum | burn_stack[3000]);
}

void *child_thread_func(void *arg) {
  // Send SIGUSR2 to the main thread
  pthread_kill(*((pthread_t *)arg), SIGUSR2);
  return NULL;
}

int main() {
  pthread_t main_thread_id = pthread_self();
  pthread_t child_thread;
  struct sigaction sa;

  // Install the signal handler for SIGUSR2
  sa.sa_handler = reentrant_signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGUSR2, &sa, NULL) == -1) {
    exit(1);  // Failed to install signal handler
  }

  // Create the child thread
  if (pthread_create(&child_thread, NULL, &child_thread_func,
                     &main_thread_id) != 0) {
    exit(2);  // Failed to create child thread
  }

  // Wait for the signal to be handled
  while (signal_handled_count < 10000) {
    // Busy wait
  }

  // Wait for child thread to finish
  pthread_join(child_thread, NULL);

  // Check if the signal was handled reentrantly
  if (signal_handled_count == 10000) {
    exit(0);  // Success
  } else {
    exit(3);  // The signal was not handled twice as expected
  }
}
