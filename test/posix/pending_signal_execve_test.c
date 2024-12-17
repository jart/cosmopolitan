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

#include <cosmo.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

sig_atomic_t gotsig;

void onsig(int sig) {
  gotsig = sig;
}

int main(int argc, char* argv[]) {
  sigset_t ss;
  sigfillset(&ss);
  sigprocmask(SIG_BLOCK, &ss, 0);
  if (argc >= 2 && !strcmp(argv[1], "childe")) {
    signal(SIGUSR1, onsig);
    sigemptyset(&ss);
    sigsuspend(&ss);
    if (gotsig != SIGUSR1)
      return 2;
  } else {
    int child;
    if ((child = fork()) == -1)
      return 2;
    if (!child) {
      execlp(argv[0], argv[0], "childe", NULL);
      _Exit(127);
    }
    if (IsNetbsd() || IsOpenbsd()) {
      // NetBSD has a bug where pending signals don't inherit across
      // execve, even though POSIX.1 literally says you must do this
      sleep(1);
    }
    if (kill(child, SIGUSR1))
      return 3;
    int ws;
    if (wait(&ws) != child)
      return 4;
    if (ws)
      return 5;
  }
}
