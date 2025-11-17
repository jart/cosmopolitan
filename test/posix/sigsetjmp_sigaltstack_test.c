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

#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>

sigjmp_buf jb;

void onsig(int sig) {
  siglongjmp(jb, 1);
}

void poke(char *p) {
  *p = 1;
}

void (*pPoke)(char *) = poke;

int main(void) {

  stack_t ss;
  ss.ss_sp = malloc(SIGSTKSZ);
  ss.ss_size = SIGSTKSZ;
  ss.ss_flags = 0;
  if (sigaltstack(&ss, 0))
    exit(1);

  struct sigaction sa;
  sa.sa_handler = onsig;
  sa.sa_flags = SA_ONSTACK;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGSEGV, &sa, 0))
    exit(2);

  if (!sigsetjmp(jb, 1)) {
    pPoke((void *)1);
    exit(3);
  }

  if (sigaltstack(0, &ss))
    exit(4);
  if (ss.ss_flags)
    exit(5);
}
