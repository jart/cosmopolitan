/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/serialize.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/sysv/errfuns.h"

#ifdef __x86_64__

static bool __watch_busy;
static void *__watch_addr;
static size_t __watch_size;
static char __watch_last[4096];

void __watch_hook(void);

static dontinstrument inline void Copy(char *p, char *q, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    p[i] = q[i];
  }
}

static dontinstrument inline int Cmp(char *p, char *q, size_t n) {
  if (n == 8) return READ64LE(p) != READ64LE(q);
  if (n == 4) return READ32LE(p) != READ32LE(q);
  for (; n; ++p, ++q, --n) {
    if (*p != *q) {
      return 1;
    }
  }
  return 0;
}

dontinstrument void __watcher(void) {
  if (__watch_busy) return;
  __watch_busy = true;
  if (Cmp(__watch_last, __watch_addr, __watch_size)) {
    kprintf("watchpoint %p changed:\n"
            "%#.*hhs\n"
            "%#.*hhs\n",
            __watch_addr,                //
            __watch_size, __watch_last,  //
            __watch_size, __watch_addr);
    Copy(__watch_last, __watch_addr, __watch_size);
    ShowBacktrace(2, __builtin_frame_address(0));
  }
  __watch_busy = false;
}

/**
 * Watches memory address for changes.
 *
 * It's useful for situations when ASAN and GDB both aren't working.
 */
int __watch(void *addr, size_t size) {
  static bool once;
  if (__watch_busy) ebusy();
  if (size > sizeof(__watch_last)) return einval();
  if (!once) {
    if (!GetSymbolTable()) return -1;
    if (__hook(__watch_hook, GetSymbolTable()) == -1) return -1;
    once = true;
  }
  __watch_addr = addr;
  __watch_size = size;
  Copy(__watch_last, __watch_addr, __watch_size);
  return 0;
}

#endif /* __x86_64__ */
