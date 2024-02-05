/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rlimit.internal.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/rlim.h"
#include "libc/sysv/consts/rlimit.h"

// Hack for guessing boundaries of _start()'s stack
//
// Every UNIX system in our support vector creates arg blocks like:
//
//     <HIGHEST-STACK-ADDRESS>
//     last environ string
//     ...
//     first environ string
//     ...
//     auxiliary value pointers
//     environ pointers
//     argument pointers
//     argument count
//     --- %rsp _start()
//     ...
//     ...
//     ... program's stack
//     ...
//     ...
//     <LOWEST-STACK-ADDRESS>
//
// The region of memory between highest and lowest can be computed
// across all supported platforms ±1 page accuracy as the distance
// between the last character of the last environ variable rounded
// up to the microprocessor page size (this computes the top addr)
// and the bottom is computed by subtracting RLIMIT_STACK rlim_cur
// It's simple but gets tricky if we consider environ can be empty
//
// This code always guesses correctly on Windows because WinMain()
// is written to allocate a stack ourself. Local testing on Linux,
// XNU, FreeBSD, OpenBSD, and NetBSD says that accuracy is ±1 page
// and that error rate applies to both beginning and end addresses

static char *__get_last(char **list) {
  char *res = 0;
  for (int i = 0; list[i]; ++i) {
    res = list[i];
  }
  return res;
}

static int __get_length(const char *s) {
  int n = 0;
  while (*s++) ++n;
  return n;
}

static uintptr_t __get_main_top(int pagesz) {
  uintptr_t top;
  const char *s;
  if ((s = __get_last(__envp)) || (s = __get_last(__argv))) {
    top = (uintptr_t)s + __get_length(s);
  } else {
    unsigned long *xp = __auxv;
    while (*xp) xp += 2;
    top = (uintptr_t)xp;
  }
  return ROUNDUP(top, pagesz);
}

static size_t __get_stack_size(int pagesz, uintptr_t start, uintptr_t top) {
  size_t size, max = 8 * 1024 * 1024;
  struct rlimit rlim = {RLIM_INFINITY};
  sys_getrlimit(RLIMIT_STACK, &rlim);
  if ((size = rlim.rlim_cur) > max) size = max;
  return MAX(ROUNDUP(size, pagesz), ROUNDUP(top - start, pagesz));
}

/**
 * Returns approximate boundaries of main thread stack.
 */
void __get_main_stack(void **out_addr, size_t *out_size, int *out_guardsize) {
  if (IsWindows()) {
    *out_addr = (void *)GetStaticStackAddr(0);
    *out_size = GetStaticStackSize();
    *out_guardsize = getauxval(AT_PAGESZ);
    return;
  }
  int pagesz = getauxval(AT_PAGESZ);
  uintptr_t start = (uintptr_t)__argv;
  uintptr_t top = __get_main_top(pagesz);
  uintptr_t bot = top - __get_stack_size(pagesz, start, top);
  uintptr_t vdso = getauxval(AT_SYSINFO_EHDR);
  if (vdso) {
    if (vdso > start && vdso < top) {
      top = vdso;
    } else if (vdso < start && vdso >= bot) {
      bot += vdso + pagesz * 2;
    }
  }
  unassert(bot < top);
  unassert(bot < start);
  unassert(top > start);
  *out_addr = (void *)bot;
  *out_size = top - bot;
  *out_guardsize = pagesz;
}
