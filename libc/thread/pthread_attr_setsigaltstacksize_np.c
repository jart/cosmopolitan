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
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/thread.h"

/**
 * Defines size of cosmo-owned signal stack for thread.
 *
 * The sigaltstack() function is useful for writing robust programs that
 * can recover from the occasional thread having a stack overflow rather
 * than having the entire process crash. To use it normally, sigaltstack
 * needs to be called at the start of each thread with a unique piece of
 * memory. However this is challenging to do *correctly* without support
 * from the POSIX threads runtime, since canceled or crashed threads may
 * need to execute on the signal stack during pthread_exit() which would
 * prevent a thread-local storage key destructor from free()'ing it.
 *
 * By default pthread_create() will not install a sigaltstack() on newly
 * created threads. If this function is called, on the attributes object
 * that gets passed to pthread_create(), then it'll use malloc() to make
 * a stack for the thread using the size you specify here. The threading
 * runtime will also free that memory safely after complete termination.
 *
 *     pthread_t id;
 *     pthread_attr_t attr;
 *     pthread_attr_init(&attr);
 *     pthread_attr_setguardsize(&attr, getpagesize());
 *     pthread_attr_setsigaltstacksize_np(&attr, stacksize);
 *     pthread_create(&id, &attr, func, 0);
 *     pthread_attr_destroy(&attr);
 *     pthread_join(id, 0);
 *
 * Try using a size of `sysconf(_SC_SIGSTKSZ)`. If you want the smallest
 * size possible, then `sysconf(_SC_MINSIGSTKSZ) + 2048` is probably the
 * smallest value that can reasonably expected to work with pthread_exit
 *
 *     struct sigaction sa;
 *     sigemptyset(&sa.sa_mask);
 *     sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
 *     sa.sa_sigaction = on_crash_signal;
 *     sigaction(SIGSEGV, &sa, 0);
 *
 * Please note that in order for this to work, your handlers for signals
 * such as SIGSEGV and SIGBUS need to use SA_ONSTACK in your sa_flags.
 *
 * @param stacksize contains stack size in bytes, or 0 to disable
 * @return 0 on success, or errno on error
 * @raise EINVAL if `stacksize` is less than `sysconf(_SC_MINSIGSTKSZ)`
 */
errno_t pthread_attr_setsigaltstacksize_np(pthread_attr_t *a,
                                           size_t stacksize) {
  if (stacksize) {
    if (stacksize > INT_MAX)
      return EINVAL;
    if (stacksize < __get_minsigstksz())
      return EINVAL;
  }
  a->__sigaltstacksize = stacksize;
  return 0;
}
