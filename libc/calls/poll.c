/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/strace.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/likely.h"
#include "libc/macros.internal.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/pollfd.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Waits for something to happen on multiple file descriptors at once.
 *
 * Warning: XNU has an inconsistency with other platforms. If you have
 * pollfds with fd≥0 and none of the meaningful events flags are added
 * e.g. POLLIN then XNU won't check for POLLNVAL. This matters because
 * one of the use-cases for poll() is quickly checking for open files.
 *
 * Note: Polling works best on Windows for sockets. We're able to poll
 * input on named pipes. But for anything that isn't a socket, or pipe
 * with POLLIN, (e.g. regular file) then POLLIN/POLLOUT are always set
 * into revents if they're requested, provided they were opened with a
 * mode that permits reading and/or writing.
 *
 * Note: Windows has a limit of 64 file descriptors and ENOMEM with -1
 * is returned if that limit is exceeded. In practice the limit is not
 * this low. For example, pollfds with fd<0 don't count. So the caller
 * could flip the sign bit with a short timeout, to poll a larger set.
 *
 * @param fds[𝑖].fd should be a socket, input pipe, or conosle input
 *     and if it's a negative number then the entry is ignored
 * @param fds[𝑖].events flags can have POLLIN, POLLOUT, POLLPRI,
 *     POLLRDNORM, POLLWRNORM, POLLRDBAND, POLLWRBAND as well as
 *     POLLERR, POLLHUP, and POLLNVAL although the latter are
 *     always implied (assuming fd≥0) so they're ignored here
 * @param timeout_ms if 0 means don't wait and -1 means wait forever
 * @return number of items fds whose revents field has been set to
 *     nonzero to describe its events, or 0 if the timeout elapsed,
 *     or -1 w/ errno
 * @return fds[𝑖].revents is always zero initializaed and then will
 *     be populated with POLL{IN,OUT,PRI,HUP,ERR,NVAL} if something
 *     was determined about the file descriptor
 * @asyncsignalsafe
 * @threadsafe
 * @norestart
 */
int poll(struct pollfd *fds, size_t nfds, int timeout_ms) {
  int i, rc;
  uint64_t millis;

  if (IsAsan() && !__asan_is_valid(fds, nfds * sizeof(struct pollfd))) {
    rc = efault();
  } else if (!IsWindows()) {
    if (!IsMetal()) {
      rc = sys_poll(fds, nfds, timeout_ms);
    } else {
      rc = sys_poll_metal(fds, nfds, timeout_ms);
    }
  } else {
    millis = timeout_ms;
    rc = sys_poll_nt(fds, nfds, &millis, 0);
  }

#if defined(SYSDEBUG) && _POLLTRACE
  if (UNLIKELY(__strace > 0)) {
    kprintf(STRACE_PROLOGUE "poll(");
    if ((!IsAsan() && kisdangerous(fds)) ||
        (IsAsan() && !__asan_is_valid(fds, nfds * sizeof(struct pollfd)))) {
      kprintf("%p", fds);
    } else {
      kprintf("[{");
      for (i = 0; i < MIN(5, nfds); ++i) {
        kprintf("%s{%d, %s, %s}", i ? ", " : "", fds[i].fd,
                DescribePollFlags(fds[i].events),
                DescribePollFlags(fds[i].revents));
      }
      kprintf("%s}]", i == 5 ? "..." : "");
    }
    kprintf(", %'zu, %'d) → %d% lm\n", nfds, timeout_ms, rc);
  }
#endif

  return rc;
}
