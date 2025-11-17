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

void test1(void) {
  signal(SIGCHLD, SIG_IGN);
  if (!fork())
    _Exit(0);
  sleep(1);
  if (wait(0) != -1)
    exit(2);
  if (errno != ECHILD)
    exit(3);
}

void test2(void) {
  signal(SIGCHLD, SIG_IGN);
  if (!fork()) {
    sleep(1);
    _Exit(0);
  }
  if (wait(0) != -1)
    exit(2);
  if (errno != ECHILD)
    exit(3);
}

int main(void) {
  test1();
  test2();
}
