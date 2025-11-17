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
#include <stdlib.h>
#include <unistd.h>

volatile int signal_count = 0;

void signal_handler(int sig) {
  signal_count++;
}

int main() {
  struct sigaction sa;
  sigset_t mask;

  sa.sa_handler = signal_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGUSR2);

  if (sigaction(SIGUSR1, &sa, NULL) != 0)
    return 1;

  if (sigaction(SIGUSR2, &sa, NULL) != 0)
    return 2;

  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR2);

  if (sigprocmask(SIG_BLOCK, &mask, NULL) != 0)
    return 3;

  if (raise(SIGUSR2) != 0)
    return 4;

  if (signal_count != 0)
    return 5;

  if (sigprocmask(SIG_UNBLOCK, &mask, NULL) != 0)
    return 6;

  if (signal_count != 1)
    return 7;

  return 0;
}
