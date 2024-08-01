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
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

#define BENCH(ITERATIONS, WORK_PER_RUN, CODE)                                 \
  do {                                                                        \
    struct timespec start = timespec_real();                                  \
    for (int __i = 0; __i < ITERATIONS; ++__i) {                              \
      asm volatile("" ::: "memory");                                          \
      CODE;                                                                   \
    }                                                                         \
    long long work = (WORK_PER_RUN) * (ITERATIONS);                           \
    double nanos =                                                            \
        (timespec_tonanos(timespec_sub(timespec_real(), start)) + work - 1) / \
        (double)work;                                                         \
    kprintf("%'20ld ns %2dx %s\n", (long)nanos, (ITERATIONS), #CODE);         \
  } while (0)

void map_unmap_one_page(void) {
  void *p;
  if ((p = mmap(__maps_randaddr(), 1, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
    __builtin_trap();
  if (munmap(p, 1))
    __builtin_trap();
}

int main() {
  kprintf("\n");
  BENCH(1000, 1, map_unmap_one_page());

  // macos doesn't scale
  if (IsXnu())
    return 0;

  // create lots of sparse mappings to put
  // weight on __maps.maps. red-black tree
  int n = 10000;
  kprintf("%20s creating %d sparse maps...\n", "", n);
  for (int i = 0; i < n; ++i) {
    if (mmap(__maps_randaddr(), 1, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0) == MAP_FAILED)
      __builtin_trap();
  }

  BENCH(1000, 1, map_unmap_one_page());
}
