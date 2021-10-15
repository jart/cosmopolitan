/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/bits.h"
#include "libc/intrin/asan.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"

static bool once;
static bool hasleaks;

static noasan void CheckLeak(void *x, void *y, size_t n, void *a) {
  if (n) {
    if (IsAsan()) {
      if (__asan_get_heap_size(x)) {
        hasleaks = true;
      }
    } else {
      hasleaks = true;
    }
  }
}

static noasan void OnMemory(void *x, void *y, size_t n, void *a) {
  static int i;
  if (n) {
    if (++i < 20) {
      __printf("%p %,d bytes", x, n);
      if (IsAsan()) {
        __asan_print_trace(x);
      }
      __printf("\n");
    }
    if (i == 20) {
      __printf("etc. etc.\n");
    }
  }
}

static noasan bool HasLeaks(void) {
  malloc_inspect_all(CheckLeak, 0);
  return hasleaks;
}

/**
 * Tests for memory leaks.
 *
 * This function needs to call __cxa_finalize(). Therefore any runtime
 * services that depend on malloc() cannot be used, after calling this
 * function.
 */
noasan void testlib_checkformemoryleaks(void) {
  struct mallinfo mi;
  if (!cmpxchg(&once, false, true)) {
    __printf("testlib_checkformemoryleaks() may only be called once\n");
    exit(1);
  }
  __cxa_finalize(0);
  if (!IsAsan()) {
    /* TODO(jart): How can we make this work without ASAN? */
    return;
  }
  malloc_trim(0);
  if (HasLeaks()) {
    mi = mallinfo();
    __printf("\n"
             "UNFREED MEMORY\n"
             "%s\n"
             "max allocated space   %,*d\n"
             "total allocated space %,*d\n"
             "total free space      %,*d\n"
             "releasable space      %,*d\n"
             "mmaped space          %,*d\n"
             "non-mmapped space     %,*d\n"
             "\n",
             __argv[0], 16l, mi.usmblks, 16l, mi.uordblks, 16l, mi.fordblks,
             16l, mi.hblkhd, 16l, mi.keepcost, 16l, mi.arena);
    if (!IsAsan()) {
      __printf("# NOTE: Use `make -j8 MODE=dbg` for malloc() backtraces\n");
    }
    malloc_inspect_all(OnMemory, 0);
    __printf("\n");
    PrintMemoryIntervals(2, &_mmi);
    /* PrintSystemMappings(2); */
    /* PrintGarbage(); */
    _Exit(78);
  }
}
