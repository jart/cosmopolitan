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
#include "libc/errno.h"
#include "libc/runtime/stack.h"
#include "libc/thread/thread.h"

/**
 * Configures custom stack for thread.
 *
 * Normally you want to use pthread_attr_setstacksize() and
 * pthread_attr_setguardsize() to configure how pthread_create()
 * allocates stack memory for newly created threads. Cosmopolitan is
 * very good at managing stack memory. However if you still want to
 * allocate stack memory on your own, POSIX defines this function.
 *
 * Your `stackaddr` points to the byte at the very bottom of your stack.
 * You are responsible for this memory. Your POSIX threads runtime will
 * not free or unmap this allocation when the thread has terminated. If
 * `stackaddr` is null then `stacksize` is ignored and default behavior
 * is restored, i.e. pthread_create() will manage stack allocations.
 *
 * Your `stackaddr` could be created by malloc(). On OpenBSD,
 * pthread_create() will augment your custom allocation so it's
 * permissable by the kernel to use as a stack. You may also call
 * Cosmopolitan APIs such NewCosmoStack() and cosmo_stack_alloc().
 * Static memory can be used, but it won't reduce pthread footprint.
 *
 * @return 0 on success, or errno on error
 * @raise EINVAL if `stacksize` is less than `PTHREAD_STACK_MIN`
 * @see pthread_attr_setstacksize()
 */
errno_t pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr,
                              size_t stacksize) {
  if (!stackaddr) {
    attr->__stackaddr = 0;
    attr->__stacksize = GetStackSize();
    return 0;
  }
  if (stacksize < PTHREAD_STACK_MIN)
    return EINVAL;
  attr->__stackaddr = stackaddr;
  attr->__stacksize = stacksize;
  return 0;
}
