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
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/thread/thread.h"

/**
 * Initializes pthread attributes.
 *
 * @return 0 on success, or errno on error
 * @see pthread_attr_setdetachstate()
 * @see pthread_attr_setsigmask_np()
 * @see pthread_attr_setstack()
 * @see pthread_attr_setstacksize()
 * @see pthread_attr_setguardsize()
 * @see pthread_attr_setschedparam()
 * @see pthread_attr_setschedpolicy()
 * @see pthread_attr_setinheritsched()
 * @see pthread_attr_setscope()
 */
errno_t pthread_attr_init(pthread_attr_t *attr) {
  *attr = (pthread_attr_t){
      .__stacksize = GetStackSize(),
      .__guardsize = getauxval(AT_PAGESZ),
  };
  return 0;
}
