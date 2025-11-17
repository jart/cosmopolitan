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
#include "ape/sections.internal.h"
#include "libc/atomic.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rlimit.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/pib.h"

__rarechange static struct {
  atomic_uint once;
  size_t size;
} __rlimit_stack;

static void __rlimit_stack_init(void) {
  size_t size;
  if (IsWindows() || IsMetal()) {
    size = GetStaticStackSize();
  } else {
    struct rlimit rlim;
    sys_getrlimit(RLIMIT_STACK, &rlim);
    size = rlim.rlim_cur;
  }
  __rlimit_stack.size = size;
}

size_t __rlimit_stack_get(void) {
  size_t size;
  if (__get_pib()->rlimit[RLIMIT_STACK].rlim_cur) {
    size = ~__get_pib()->rlimit[RLIMIT_STACK].rlim_cur;
  } else {
    cosmo_once(&__rlimit_stack.once, __rlimit_stack_init);
    size = __rlimit_stack.size;
  }
  if (size > 0x7fff0000)
    size = 0x7fff0000;
  return size;
}
