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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile int handler_called = 0;

void signal_handler(int sig) {
  sigset_t current_mask;
  handler_called = 1;

  if (sigprocmask(SIG_SETMASK, NULL, &current_mask) != 0) {
    _exit(10);
  }

  if (!sigismember(&current_mask, SIGUSR2)) {
    printf("BUG: SIGUSR2 not blocked in handler despite sa.sa_mask\n");
    _exit(11);
  }
}

int main() {
  struct sigaction sa;

  sa.sa_handler = signal_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGUSR2);

  if (sigaction(SIGUSR1, &sa, NULL) != 0)
    return 1;

  if (raise(SIGUSR1) != 0)
    return 2;

  if (!handler_called)
    return 3;

  return 0;
}
