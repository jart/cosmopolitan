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
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/futex.internal.h"

/**
 * Terminates current POSIX thread.
 *
 * For example, a thread could terminate early as follows:
 *
 *     pthread_exit((void *)123);
 *
 * The result value could then be obtained when joining the thread:
 *
 *     void *rc;
 *     pthread_join(id, &rc);
 *     assert((intptr_t)rc == 123);
 *
 * Under normal circumstances a thread can exit by simply returning from
 * the callback function that was supplied to pthread_create(). This may
 * be used if the thread wishes to exit at any other point in the thread
 * lifecycle, in which case this function is responsible for ensuring we
 * invoke _gc(), _defer(), and pthread_cleanup_push() callbacks, as well
 * as pthread_key_create() destructors.
 *
 * If the current thread is an orphaned thread, or is the main thread
 * when no other threads were created, then this will terminated your
 * process with an exit code of zero. It's not possible to supply a
 * non-zero exit status to wait4() via this function.
 *
 * Once a thread has exited, access to its stack memory is undefined.
 * The behavior of calling pthread_exit() from cleanup handlers and key
 * destructors is also undefined.
 *
 * @param rc is reported later to pthread_join()
 * @threadsafe
 * @noreturn
 */
wontreturn void pthread_exit(void *rc) {
  struct PosixThread *pt;
  struct _pthread_cleanup_buffer *cb;
  pt = (struct PosixThread *)__get_tls()->tib_pthread;
  pt->rc = rc;
  // the memory of pthread cleanup objects lives on the stack
  // so we need to harvest them before calling longjmp()
  while ((cb = pt->cleanup)) {
    pt->cleanup = cb->__prev;
    cb->__routine(cb->__arg);
  }
  if (~pt->flags & PT_MAINTHREAD) {
    // this thread was created by pthread_create()
    // garbage collector memory exists on a shadow stack. we don't need
    // to use _gclongjmp() since _pthread_ungarbage() will collect them
    // at the setjmp() site.
    longjmp(pt->exiter, 1);
  } else {
    // this is the main thread
    // release as much resources and possible and mark it terminated
    _pthread_cleanup(pt);
    // it's kind of irregular for a child thread to join the main thread
    // so we don't bother freeing the main thread's stack since it makes
    // this implementation so much simpler for example we want't to call
    // set_tid_address() upon every program startup which isn't possible
    // on non-linux platforms anyway.
    atomic_store_explicit(&__get_tls()->tib_tid, 0, memory_order_release);
    nsync_futex_wake_((int *)&__get_tls()->tib_tid, INT_MAX, !IsWindows());
    _Exit1(0);
  }
}
