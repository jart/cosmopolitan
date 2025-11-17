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

volatile int usr1_count = 0;
volatile int usr2_count = 0;

void usr1_handler(int sig) {
  sigset_t mask;
  usr1_count++;

  if (sigprocmask(SIG_SETMASK, NULL, &mask) != 0)
    _exit(10);

  if (!sigismember(&mask, SIGUSR2))
    _exit(11);

  raise(SIGUSR2);
}

void usr2_handler(int sig) {
  usr2_count++;
}

int main() {
  struct sigaction sa1, sa2;

  sa1.sa_handler = usr1_handler;
  sa1.sa_flags = 0;
  sigemptyset(&sa1.sa_mask);
  sigaddset(&sa1.sa_mask, SIGUSR2);

  sa2.sa_handler = usr2_handler;
  sa2.sa_flags = 0;
  sigemptyset(&sa2.sa_mask);

  if (sigaction(SIGUSR1, &sa1, NULL) != 0)
    return 1;

  if (sigaction(SIGUSR2, &sa2, NULL) != 0)
    return 2;

  if (raise(SIGUSR1) != 0)
    return 3;

  if (usr1_count != 1)
    return 4;

  if (usr2_count != 1)
    return 5;

  return 0;
}
