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
#include <sys/wait.h>
#include <unistd.h>

volatile int signal_received = 0;

void signal_handler(int sig) {
  signal_received = 1;
}

int main() {
  struct sigaction sa;
  sigset_t mask, oldmask, suspmask;
  pid_t pid;

  sa.sa_handler = signal_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGUSR1, &sa, NULL) != 0)
    return 1;

  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);

  if (sigprocmask(SIG_BLOCK, &mask, &oldmask) != 0)
    return 2;

  pid = fork();
  if (pid == 0) {
    usleep(5000);
    kill(getppid(), SIGUSR1);
    return 0;
  } else if (pid > 0) {
    sigemptyset(&suspmask);

    if (sigsuspend(&suspmask) != -1 || errno != EINTR)
      return 3;

    if (!signal_received)
      return 4;

    wait(NULL);

    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) != 0)
      return 5;
  } else {
    return 6;
  }

  return 0;
}