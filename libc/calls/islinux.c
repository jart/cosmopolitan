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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/pr.h"

#ifdef __x86_64__

static struct {
  atomic_uint once;
  bool res;
} __is_linux_2_6_23_data;

static bool __is_linux_2_6_23_impl(void) {
  int rc;
  if (IsGenuineBlink()) return true;
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(157), "D"(PR_GET_SECCOMP)
               : "rcx", "r11", "memory");
  return rc != -EINVAL;
}

static void __is_linux_2_6_23_init(void) {
  __is_linux_2_6_23_data.res = __is_linux_2_6_23_impl();
}

#endif /* x86 */

/**
 * Returns true if we're running on non-ancient Linux.
 * @note this function must only be called on Linux
 */
bool __is_linux_2_6_23(void) {
  unassert(IsLinux());  // should be checked by caller
#ifdef __x86_64__
  cosmo_once(&__is_linux_2_6_23_data.once, __is_linux_2_6_23_init);
  return __is_linux_2_6_23_data.res;
#else
  return true;
#endif
}
