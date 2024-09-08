/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/timespec.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/thread/thread.h"

#define ALLOCATIONS 1000

void *worker(void *arg) {
  void **ptrs = malloc(ALLOCATIONS * sizeof(void *));
  for (int i = 0; i < ALLOCATIONS; ++i)
    ptrs[i] = malloc(1);
  for (int i = 0; i < ALLOCATIONS; ++i)
    free(ptrs[i]);
  free(ptrs);
  return 0;
}

void test(int n) {
  struct timespec start = timespec_mono();
  pthread_t *th = malloc(sizeof(pthread_t) * n);
  for (int i = 0; i < n; ++i)
    pthread_create(th + i, 0, worker, 0);
  for (int i = 0; i < n; ++i)
    pthread_join(th[i], 0);
  free(th);
  struct timespec end = timespec_mono();
  printf("%2d threads * %d allocs = %ld us\n", n, ALLOCATIONS,
         timespec_tomicros(timespec_sub(end, start)));
}

int main(int argc, char *argv[]) {
  int n = __get_cpu_count();
  if (n < 8)
    n = 8;
  for (int i = 1; i <= n; ++i)
    test(i);
}
