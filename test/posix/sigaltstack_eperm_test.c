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
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ASSERT(x)                                                          \
  do {                                                                     \
    if (!(x)) {                                                            \
      fprintf(stderr, "%s:%d: assertion failed: %s\n", __FILE__, __LINE__, \
              #x);                                                         \
      exit(1);                                                             \
    }                                                                      \
  } while (0)

void onsig(int sig) {
  stack_t ss;
  ASSERT(!sigaltstack(0, &ss));
  ASSERT(ss.ss_size == SIGSTKSZ);
  ASSERT(ss.ss_flags == SS_ONSTACK);
  ASSERT(ss.ss_sp = malloc(SIGSTKSZ));
  ss.ss_size = SIGSTKSZ;
  ss.ss_flags = 0;
  ASSERT(sigaltstack(&ss, 0));
  ASSERT(errno == EPERM);
  _exit(0);
}

// TODO: figure out how to overflow stack without undefined behavior
#pragma GCC diagnostic ignored "-Waggressive-loop-optimizations"

int StackOverflow(int x) {
  return x ? StackOverflow(x) + StackOverflow(x + 1) : 0;
}

int main(int argc, char *argv[]) {

  stack_t ss;
  ASSERT(ss.ss_sp = malloc(SIGSTKSZ));
  ss.ss_size = SIGSTKSZ;
  ss.ss_flags = 0;
  ASSERT(!sigaltstack(&ss, 0));

  ASSERT(!sigaltstack(0, &ss));
  ASSERT(ss.ss_size == SIGSTKSZ);
  ASSERT(ss.ss_flags == 0);

  struct sigaction sa;
  sa.sa_handler = onsig;
  sa.sa_flags = SA_ONSTACK;
  ASSERT(!sigemptyset(&sa.sa_mask));
  ASSERT(!sigaction(SIGSEGV, &sa, 0));
  ASSERT(!sigaction(SIGBUS, &sa, 0));

  StackOverflow(1);
}
