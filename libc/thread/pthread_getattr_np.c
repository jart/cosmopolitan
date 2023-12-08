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
#include "libc/calls/struct/rlimit.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/rlim.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

/**
 * Gets thread attributes.
 *
 * These attributes are copied from the ones supplied when
 * pthread_create() was called. However this function supplies
 * additional runtime information too:
 *
 * 1. The detached state. You can use pthread_attr_getdetachstate() on
 *    the `attr` result to see, for example, if a thread detached itself
 *    or some other thread detached it, after it was spawned.
 *
 * 2. The thread's stack. You can use pthread_attr_getstack() to see the
 *    address and size of the stack that was allocated by cosmo for your
 *    thread. This is useful for knowing where the stack is. It can also
 *    be useful If you explicitly configured a stack too, since we might
 *    have needed to slightly tune the address and size to meet platform
 *    requirements.
 *
 * 3. You can view changes pthread_create() may have made to the stack
 *    guard size by calling pthread_attr_getguardsize() on `attr`
 *
 * @param attr is output argument that receives attributes, which should
 *     find its way to pthread_attr_destroy() when it's done being used
 * @return 0 on success, or errno on error
 * @raise ENOMEM is listed as a possible result by LSB 5.0
 */
errno_t pthread_getattr_np(pthread_t thread, pthread_attr_t *attr) {
  struct PosixThread *pt = (struct PosixThread *)thread;
  memcpy(attr, &pt->pt_attr, sizeof(pt->pt_attr));
  switch (atomic_load_explicit(&pt->pt_status, memory_order_relaxed)) {
    case kPosixThreadJoinable:
    case kPosixThreadTerminated:
      attr->__detachstate = PTHREAD_CREATE_JOINABLE;
      break;
    case kPosixThreadDetached:
    case kPosixThreadZombie:
      attr->__detachstate = PTHREAD_CREATE_DETACHED;
      break;
    default:
      __builtin_unreachable();
  }
  if (!attr->__stacksize && (pt->pt_flags & PT_STATIC)) {
    __get_main_stack(&attr->__stackaddr, &attr->__stacksize,
                     &attr->__guardsize);
  }
  return 0;
}
