/*-*-mode:c++;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8-*-│
│ vi: set et ft=cpp ts=2 sts=2 sw=2 fenc=utf-8                             :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/mem/alg.h"
#include "libc/mem/leaks.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"

struct Resource {
  char *p;
  Resource() {
    p = new char;
  }
  ~Resource() {
    delete p;
  }
};

void Poke(char *p) {
  *p = 1;
}
void (*pPoke)(char *) = Poke;

void Throw(void) {
  throw 42;
}
void (*pThrow)(void) = Throw;

int cmp(const void *x, const void *y) {
  Resource r;
  pPoke(r.p);
  if (*r.p)
    pThrow();
  exit(5);
}

int A[3] = {3, 2, 1};
int Work(void) {
  Resource r;
  pPoke(r.p);
  qsort(A, 3, sizeof(int), cmp);
  return A[0];
}
int (*pWork)(void) = Work;

int main(int argc, char *argv[]) {
  try {
    Resource r;
    if (pWork() != 1)
      return 1;
    pPoke(r.p);
    if (r.p)
      return 2;
  } catch (int e) {
    if (e != 42)
      return 3;
  }
  CheckForMemoryLeaks();
}
