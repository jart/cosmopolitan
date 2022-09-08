/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/pthread.h"
#include "libc/macros.internal.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

#define MAP_ANON_OPENBSD  0x1000
#define MAP_STACK_OPENBSD 0x4000

/**
 * Configures custom allocated stack for thread, e.g.
 *
 *     pthread_t id;
 *     pthread_attr_t attr;
 *     char *stk = _mapstack();
 *     pthread_attr_init(&attr);
 *     pthread_attr_setstack(&attr, stk, GetStackSize());
 *     pthread_create(&id, &attr, func, 0);
 *     pthread_attr_destroy(&attr);
 *     pthread_join(id, 0);
 *     _freestack(stk);
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
 * please consider using _mapstack() which always does it perfect
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
 * @param stackaddr is address of stack allocated by caller, and
 *     may be NULL in which case default behavior is restored
 * @param stacksize is size of caller allocated stack
 * @return 0 on success, or errno on error
 * @raise EINVAL if parameters were unacceptable
 * @see pthread_attr_setstacksize()
 */
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr,
                          size_t stacksize) {
  if (!stackaddr) {
    attr->stackaddr = 0;
    attr->stacksize = 0;
    return 0;
  }
  if (stacksize < PTHREAD_STACK_MIN ||
      (IsAsan() && !__asan_is_valid(stackaddr, stacksize))) {
    return EINVAL;
  }
  if (IsOpenbsd()) {
    // OpenBSD: Only permits RSP to occupy memory that's been explicitly
    // defined as stack memory. We need to squeeze the provided interval
    // in order to successfully call mmap(), which will return EINVAL if
    // these calculations should overflow.
    size_t n;
    int e, rc;
    uintptr_t x, y;
    n = stacksize;
    x = (uintptr_t)stackaddr;
    y = ROUNDUP(x, PAGESIZE);
    n -= y - x;
    n = ROUNDDOWN(n, PAGESIZE);
    stackaddr = (void *)y;
    stacksize = n;
    e = errno;
    if (__sys_mmap(stackaddr, stacksize, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANON_OPENBSD | MAP_STACK_OPENBSD, -1, 0,
                   0) == MAP_FAILED) {
      rc = errno;
      errno = e;
      return rc;
    }
  }
  attr->stackaddr = stackaddr;
  attr->stacksize = stacksize;
  return 0;
}
