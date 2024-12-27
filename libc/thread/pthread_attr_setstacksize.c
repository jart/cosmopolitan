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
#include "libc/thread/thread.h"

/**
 * Specifies minimum stack size for thread, e.g.
 *
 *     pthread_t th;
 *     pthread_attr_t attr;
 *     pthread_attr_init(&attr);
 *     pthread_attr_setguardsize(&attr, 4096);
 *     pthread_attr_setstacksize(&attr, 61440);
 *     pthread_create(&th, &attr, thfunc, arg);
 *     pthread_attr_destroy(&attr);
 *
 * On Linux, if you're not using `cosmocc -mtiny`, and you're not using
 * cosmo_dlopen(), and guard size is nonzero, then `MAP_GROWSDOWN` will
 * be used to create your stack memory. This helps minimize virtual
 * memory consumption. Please note this is only possible if `stacksize`
 * is no larger than the current `RLIMIT_STACK`, otherwise the runtime
 * will map your stack using plain old mmap().
 *
 * Non-custom stacks may be recycled by the cosmo runtime. You can
 * control this behavior by calling cosmo_stack_setmaxstacks(). It's
 * useful for both tuning performance and hardening security. See also
 * pthread_attr_setguardsize() which is important for security too.
 *
 * @param stacksize contains stack size in bytes
 * @return 0 on success, or errno on error
 * @raise EINVAL if `stacksize` is less than `PTHREAD_STACK_MIN`
 */
errno_t pthread_attr_setstacksize(pthread_attr_t *a, size_t stacksize) {
  if (stacksize < PTHREAD_STACK_MIN)
    return EINVAL;
  a->__stacksize = stacksize;
  return 0;
}
