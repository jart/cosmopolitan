// Copyright 2024 Justine Alexandra Roberts Tunney
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

volatile int gotsig;

void OnSig(int sig) {
  gotsig = sig;
}

void test_sa_resethand_raise(void) {
  struct sigaction sa;
  sa.sa_handler = OnSig;
  sa.sa_flags = SA_RESETHAND;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGUSR1, &sa, 0))
    exit(1);
  if (sigaction(SIGUSR1, 0, &sa))
    exit(2);
  if (sa.sa_handler != OnSig)
    exit(3);
  if (raise(SIGUSR1))
    exit(4);
  if (gotsig != SIGUSR1)
    exit(5);
  if (sigaction(SIGUSR1, 0, &sa))
    exit(6);
  if (sa.sa_handler != SIG_DFL)
    exit(7);
}

void test_sa_resethand_pause(void) {
  struct sigaction sa;
  sa.sa_handler = OnSig;
  sa.sa_flags = SA_RESETHAND;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGALRM, &sa, 0))
    exit(10);
  ualarm(10000, 0);
  if (pause() != -1 || errno != EINTR)
    exit(11);
  if (gotsig != SIGALRM)
    exit(12);
  if (sigaction(SIGALRM, 0, &sa))
    exit(13);
  if (sa.sa_handler != SIG_DFL)
    exit(14);
}

void test_sa_resethand_read(void) {
  struct sigaction sa;
  sa.sa_handler = OnSig;
  sa.sa_flags = SA_RESETHAND;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGALRM, &sa, 0))
    exit(20);
  int fds[2];
  if (pipe(fds))
    exit(21);
  ualarm(10000, 0);
  if (read(fds[0], (char[]){0}, 1) != -1 || errno != EINTR)
    exit(22);
  if (gotsig != SIGALRM)
    exit(23);
  if (sigaction(SIGALRM, 0, &sa))
    exit(24);
  if (sa.sa_handler != SIG_DFL)
    exit(25);
}

int main() {
  test_sa_resethand_raise();
  test_sa_resethand_pause();
  test_sa_resethand_read();
}
