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
#include "libc/atomic.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rlimit.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/intrin/cxaatexit.h"
#include "libc/intrin/lockless.h"
#include "libc/intrin/rlimit.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/rlim.h"
#include "libc/sysv/consts/rlimit.h"

struct atomic_rlimit {
  atomic_ulong cur;
  atomic_ulong max;
  atomic_uint once;
  atomic_uint gen;
};

static struct atomic_rlimit __rlimit_stack;

static void __rlimit_stack_init(void) {
  struct rlimit rlim;
  if (IsWindows()) {
    rlim.rlim_cur = GetStaticStackSize();
    rlim.rlim_max = -1;  // RLIM_INFINITY in consts.sh
  } else {
    sys_getrlimit(RLIMIT_STACK, &rlim);
  }
  atomic_init(&__rlimit_stack.cur, rlim.rlim_cur);
  atomic_init(&__rlimit_stack.max, rlim.rlim_max);
}

struct rlimit __rlimit_stack_get(void) {
  unsigned gen;
  unsigned long cur, max;
  cosmo_once(&__rlimit_stack.once, __rlimit_stack_init);
  gen = lockless_read_begin(&__rlimit_stack.gen);
  do {
    cur = atomic_load_explicit(&__rlimit_stack.cur, memory_order_acquire);
    max = atomic_load_explicit(&__rlimit_stack.max, memory_order_acquire);
  } while (!lockless_read_end(&__rlimit_stack.gen, &gen));
  return (struct rlimit){cur, max};
}

void __rlimit_stack_set(struct rlimit rlim) {
  unsigned gen;
  unsigned long cur, max;
  cosmo_once(&__rlimit_stack.once, __rlimit_stack_init);
  __cxa_lock();
  cur = rlim.rlim_cur;
  max = rlim.rlim_max;
  gen = lockless_write_begin(&__rlimit_stack.gen);
  atomic_store_explicit(&__rlimit_stack.cur, cur, memory_order_release);
  atomic_store_explicit(&__rlimit_stack.max, max, memory_order_release);
  lockless_write_end(&__rlimit_stack.gen, gen);
  __cxa_unlock();
}
