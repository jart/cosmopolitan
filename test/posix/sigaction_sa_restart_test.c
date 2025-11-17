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

volatile int signal_count = 0;

void signal_handler(int sig) {
  signal_count++;
}

int main() {
  struct sigaction sa, old_sa;
  int pipefd[2];

  sa.sa_handler = signal_handler;
  sa.sa_flags = SA_RESTART;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGALRM, &sa, &old_sa) != 0)
    return 1;

  if (pipe(pipefd) != 0)
    return 2;

  pid_t pid = fork();
  if (pid == -1)
    return 3;

  if (pid == 0) {
    usleep(10000);
    kill(getppid(), SIGALRM);
    usleep(10000);
    write(pipefd[1], "x", 1);
    return 0;
  }

  char buf;
  int result = read(pipefd[0], &buf, 1);

  if (result != 1)
    return 4;

  if (signal_count != 1)
    return 5;

  wait(NULL);

  if (sigaction(SIGALRM, &old_sa, NULL) != 0)
    return 6;

  return 0;
}
