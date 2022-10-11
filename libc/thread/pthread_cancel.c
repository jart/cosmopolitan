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
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/thread.h"

/**
 * Cancels thread.
 *
 * This function currently isn't supported. In order to support this
 * function we'd need to redesign the system call interface, and add
 * bloat and complexity to every function that can return EINTR. You
 * might want to consider using `nsync_note` instead, which provides
 * much better cancellations because posix cancellations is a broken
 * design. If you need to cancel i/o operations, try doing this:
 *
 *     _Thread_local bool gotusr1;
 *     void OnUsr1(int sig) { gotusr1 = true; }
 *     struct sigaction sa = {.sa_handler = OnUsr1};
 *     sigaction(SIGUSR1, &sa, 0);
 *     pthread_kill(thread, SIGUSR1);
 *
 * The above code should successfully cancel a thread's blocking io
 * operations in most cases, e.g.
 *
 *     void *MyThread(void *arg) {
 *       sigset_t ss;
 *       sigfillset(&ss);
 *       sigdelset(&ss, SIGUSR1);
 *       sigprocmask(SIG_SETMASK, &ss, 0);
 *       while (!gotusr1) {
 *         char buf[512];
 *         ssize_t rc = read(0, buf, sizeof(buf));
 *         if (rc == -1 && errno == EINTR) continue;
 *         write(1, buf, rc);
 *       }
 *       return 0;
 *     }
 *
 * This has the same correctness issue as glibc, but it's usually
 * "good enough" if you only need cancellations to perform things
 * like server shutdown and socket options like `SO_RCVTIMEO` can
 * ensure it's even safer, since it can't possibly block forever.
 *
 * @see https://sourceware.org/bugzilla/show_bug.cgi?id=12683
 */
int pthread_cancel(pthread_t thread) {
  kprintf("error: pthread_cancel() isn't supported, please see the"
          " cosmopolitan libc documentation for further details\n");
  _Exit(1);
}
