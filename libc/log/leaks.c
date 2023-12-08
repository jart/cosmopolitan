/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/cxxabi.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/cxaatexit.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"

__static_yoink("GetSymbolByAddr");

#define MAXLEAKS 1000

static bool once;
static bool hasleaks;

static dontasan void CheckLeak(void *x, void *y, size_t n, void *a) {
  if (n) {
    if (IsAsan()) {
      if (__asan_get_heap_size(x) && !__asan_is_leaky(x)) {
        hasleaks = true;
      }
    } else {
      hasleaks = true;
    }
  }
}

static dontasan void OnMemory(void *x, void *y, size_t n, void *a) {
  static int i;
  if (n) {
    if (MAXLEAKS) {
      if (i < MAXLEAKS) {
        ++i;
        kprintf("%p %,lu bytes [dlmalloc]", x, n);
        if (__asan_is_leaky(x)) {
          kprintf(" [leaky]");
        }
        __asan_print_trace(x);
        kprintf("\n");
      } else if (i == MAXLEAKS) {
        ++i;
        kprintf("etc. etc.\n");
      }
    }
  }
}

static dontasan bool HasLeaks(void) {
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
dontasan void CheckForMemoryLeaks(void) {
  struct mallinfo mi;
  if (!IsAsan()) return;  // we need traces to exclude leaky
  if (!GetSymbolTable()) {
    kprintf("CheckForMemoryLeaks() needs the symbol table\n");
    return;
  }
  if (!_cmpxchg(&once, false, true)) {
    kprintf("CheckForMemoryLeaks() may only be called once\n");
    exit(0);
  }
  _pthread_unwind(_pthread_self());
  __cxa_thread_finalize();
  _pthread_unkey(__get_tls());
  _pthread_ungarbage();
  __cxa_finalize(0);
  STRACE("checking for memory leaks% m");
  if (!IsAsan()) {
    /* TODO(jart): How can we make this work without ASAN? */
    return;
  }
  malloc_trim(0);
  if (HasLeaks()) {
    mi = mallinfo();
    kprintf("\n"
            "UNFREED MEMORY\n"
            "%s\n"
            "max allocated space   %,*d\n"
            "total allocated space %,*d\n"
            "total free space      %,*d\n"
            "releasable space      %,*d\n"
            "mmaped space          %,*d\n"
            "non-mmapped space     %,*d\n"
            "\n",
            __argv[0], 16l, mi.usmblks, 16l, mi.uordblks, 16l, mi.fordblks, 16l,
            mi.hblkhd, 16l, mi.keepcost, 16l, mi.arena);
    if (!IsAsan()) {
      kprintf("# NOTE: Use `make -j8 MODE=dbg` for malloc() backtraces\n");
    }
    malloc_inspect_all(OnMemory, 0);
    kprintf("\n");
    /* __print_maps(); */
    /* PrintSystemMappings(2); */
    /* PrintGarbage(); */
    _Exit(78);
  }
}
