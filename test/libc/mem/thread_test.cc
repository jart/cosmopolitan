/*-*-mode:c++;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8-*-│
│ vi: set et ft=c++ ts=2 sts=2 sw=2 fenc=utf-8                             :vi │
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
#include "libc/thread/thread.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/fmt/itoa.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

//
// BEFORE ADDING TLS FREELIST
//
//     malloc multithreading torture test
//     with 192 threads and 10000 iterations
//     consumed 0.084721 wall and 0.141747 cpu seconds
//
// AFTER ADDING TLS FREELIST
//
//     malloc multithreading torture test
//     with 192 threads and 10000 iterations
//     consumed 0.035193 wall and 4.34012 cpu seconds
//

#define ITERATIONS 10000

void *Worker(void *arg) {
  char *thing[32] = {};
  for (int i = 0; i < ITERATIONS; ++i) {
    int r = rand();
    int j = r % ARRAYLEN(thing);
    if (thing[j]) {
      delete[] thing[j];
      thing[j] = 0;
    } else {
      thing[j] = new char[12 + ((r >> 8) % 32)];
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  int n = __get_cpu_count();
  pthread_t *t = new pthread_t[n];
  fprintf(stderr,
          "\n"
          "malloc multithreading torture test\n"
          "with %d threads and %d iterations\n",
          n, ITERATIONS);
  struct timespec t1 = timespec_real();
  for (int i = 0; i < n; ++i)
    unassert(!pthread_create(t + i, 0, Worker, 0));
  for (int i = 0; i < n; ++i)
    unassert(!pthread_join(t[i], 0));
  struct timespec t2 = timespec_real();
  fprintf(stderr, "consumed %g wall and %g cpu seconds\n",
          timespec_tomicros(timespec_sub(t2, t1)) * 1e-6,
          (double)clock() / CLOCKS_PER_SEC);
  delete[] t;
}
