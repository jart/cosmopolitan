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

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

volatile int alarm_count = 0;
volatile int signal_received = 0;

void alarm_handler(int sig) {
  alarm_count++;
  signal_received = 1;
}

int main() {
  struct sigaction sa;
  sigset_t mask, oldmask;
  int i;

  sa.sa_handler = alarm_handler;
  sa.sa_flags = SA_RESTART;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGALRM, &sa, NULL) != 0)
    return 1;

  sigemptyset(&mask);
  sigaddset(&mask, SIGALRM);

  for (i = 0; i < 5; i++) {
    signal_received = 0;

    if (sigprocmask(SIG_BLOCK, &mask, &oldmask) != 0)
      return 2;

    alarm(1);

    usleep(500000);

    if (signal_received)
      return 3;

    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) != 0)
      return 4;

    usleep(600000);

    if (!signal_received)
      return 5;

    alarm(0);
  }

  if (alarm_count != 5)
    return 6;

  return 0;
}