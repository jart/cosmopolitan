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

// overflow the stack and catch the segmentation fault using
// sigaltstack() and then verify that the thread keeps crashing since
// we've done nothing to rehabilitate the program

int g_count;

void onsig(int sig) {
  if (g_count++ == 100)
    _Exit(0);
}

int StackOverflow(volatile int x) {
  return x ? StackOverflow(x) + StackOverflow(x + 1) : 0;
}

int main(int argc, char *argv[]) {

  stack_t ss;
  ss.ss_sp = malloc(SIGSTKSZ);
  ss.ss_size = SIGSTKSZ;
  ss.ss_flags = 0;
  if (sigaltstack(&ss, 0))
    exit(2);

  struct sigaction sa;
  sa.sa_handler = onsig;
  sa.sa_flags = SA_ONSTACK;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGSEGV, &sa, 0))
    exit(3);
  if (sigaction(SIGBUS, &sa, 0))  // needed for freebsd
    exit(4);

  return StackOverflow(1);
}
