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

volatile int got_signal = 0;

void handler(int sig) {
  got_signal = 1;
}

int main() {
  pid_t pid;
  int status;

  signal(SIGUSR1, handler);

  if (kill(getpid(), 0) != 0)
    return 1;

  if (kill(getpid(), SIGUSR1) != 0)
    return 2;

  if (!got_signal)
    return 3;

  if (kill(99999, 0) == 0)
    return 4;

  if (errno != ESRCH)
    return 5;

  if (kill(getpid(), -1) == 0)
    return 6;

  if (errno != EINVAL)
    return 7;

  pid = fork();
  if (pid == 0) {
    pause();
    _exit(0);
  } else if (pid > 0) {
    usleep(1000);
    if (kill(pid, SIGTERM) != 0)
      return 8;

    if (waitpid(pid, &status, 0) != pid)
      return 9;

    if (!WIFSIGNALED(status) || WTERMSIG(status) != SIGTERM)
      return 10;
  } else {
    return 11;
  }

  return 0;
}
