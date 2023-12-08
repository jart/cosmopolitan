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
#include "ape/sections.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/stack.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"
#include "libc/thread/tls2.internal.h"

/**
 * Computes safer buffer size for alloca().
 *
 * @param want defines an upper limit on the return value
 * @param extraspace is how much stack caller needs that isn't buffer
 * @return number of bytes to use for your buffer, or negative if the
 *     allocation would likely cause a stack overflow
 */
privileged long __get_safe_size(long want, long extraspace) {
  if (!__tls_enabled) return want;
  struct PosixThread *pt;
  struct CosmoTib *tib = __get_tls_privileged();
  if (!IsAutoFrame((uintptr_t)tib >> 16) &&
      !(__executable_start <= (const unsigned char *)tib &&
        (const unsigned char *)tib < _end)) {
    return want;
  }
  long bottom, sp = GetStackPointer();
  if ((char *)sp >= tib->tib_sigstack_addr &&
      (char *)sp <= tib->tib_sigstack_addr + tib->tib_sigstack_size) {
    bottom = (long)tib->tib_sigstack_addr;
  } else if ((pt = (struct PosixThread *)tib->tib_pthread) &&
             pt->pt_attr.__stacksize) {
    bottom = (long)pt->pt_attr.__stackaddr + pt->pt_attr.__guardsize;
  } else {
    return want;
  }
  long size = sp - bottom - extraspace;
  if (size > want) size = want;
  return size;
}
