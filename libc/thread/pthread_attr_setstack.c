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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/thread/thread.h"

/**
 * Configures custom allocated stack for thread, e.g.
 *
 *     pthread_t id;
 *     pthread_attr_t attr;
 *     char *stk = NewCosmoStack();
 *     pthread_attr_init(&attr);
 *     pthread_attr_setstack(&attr, stk, GetStackSize());
 *     pthread_create(&id, &attr, func, 0);
 *     pthread_attr_destroy(&attr);
 *     pthread_join(id, 0);
 *     FreeCosmoStack(stk);
 *
 * Your stack must have at least `PTHREAD_STACK_MIN` bytes, which
 * Cosmpolitan Libc defines as `GetStackSize()`. It's a link-time
 * constant used by Actually Portable Executable that's 128 kb by
 * default. See libc/runtime/stack.h for docs on your stack limit
 * since the APE ELF phdrs are the one true source of truth here.
 *
 * Cosmpolitan Libc runtime magic (e.g. ftrace) and memory safety
 * (e.g. kprintf) assumes that stack sizes are two-powers and are
 * aligned to that two-power. Conformance isn't required since we
 * say caveat emptor to those who don't maintain these invariants
 * please consider using NewCosmoStack(), which is always perfect
 * or use `mmap(0, GetStackSize() << 1, ...)` for a bigger stack.
 *
 * Unlike pthread_attr_setstacksize(), this function permits just
 * about any parameters and will change the values and allocation
 * as needed to conform to the mandatory requirements of the host
 * operating system even if it doesn't meet the stricter needs of
 * Cosmopolitan Libc userspace libraries. For example with malloc
 * allocations, things like page size alignment, shall be handled
 * automatically for compatibility with existing codebases.
 *
 * The same stack shouldn't be used for two separate threads. Use
 * fresh stacks for each thread so that ASAN can be much happier.
 *
 * @param stackaddr is address of stack allocated by caller, and
 *     may be NULL in which case default behavior is restored
 * @param stacksize is size of caller allocated stack
 * @return 0 on success, or errno on error
 * @raise EINVAL if parameters were unacceptable
 * @see pthread_attr_setstacksize()
 */
errno_t pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr,
                              size_t stacksize) {
  if (!stackaddr) {
    attr->__stackaddr = 0;
    attr->__stacksize = 0;
    return 0;
  }
  if (stacksize < PTHREAD_STACK_MIN ||
      (IsAsan() && !__asan_is_valid(stackaddr, stacksize))) {
    return EINVAL;
  }
  attr->__stackaddr = stackaddr;
  attr->__stacksize = stacksize;
  return 0;
}
