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
#include "libc/bits/bits.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/spinlock.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/errors.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/pollfd.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/ntstdin.internal.h"
#include "libc/sock/yoink.inc"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

#undef STRACE        // too verbosen
#define STRACE(...)  // but don't want to delete

_Alignas(64) static char poll_lock;

/**
 * Polls on the New Technology.
 *
 * This function is used to implement poll() and select(). You may poll
 * on both sockets and files at the same time. We also poll for signals
 * while poll is polling.
 */
textwindows int sys_poll_nt(struct pollfd *fds, uint64_t nfds, uint64_t *ms) {
  bool ok;
  uint32_t avail;
  struct sys_pollfd_nt pipefds[8];
  struct sys_pollfd_nt sockfds[64];
  int pipeindices[ARRAYLEN(pipefds)];
  int sockindices[ARRAYLEN(sockfds)];
  int i, sn, pn, failed, gotpipes, gotsocks, waitfor;

  // check for interrupts early before doing work
  if (_check_interrupts(false, g_fds.p)) return eintr();

  // do the planning
  // we need to read static variables
  // we might need to spawn threads and open pipes
  _spinlock(&poll_lock);
  for (failed = sn = pn = i = 0; i < nfds; ++i) {
    if (fds[i].fd < 0) continue;
    if (__isfdopen(fds[i].fd)) {
      if (__isfdkind(fds[i].fd, kFdSocket)) {
        if (sn < ARRAYLEN(sockfds)) {
          sockindices[sn] = i;
          sockfds[sn].handle = g_fds.p[fds[i].fd].handle;
          sockfds[sn].events = fds[i].events & (POLLPRI | POLLIN | POLLOUT);
          sn += 1;
        } else {
          // too many socket fds
          failed = enomem();
          break;
        }
      } else if (fds[i].events & POLLIN) {
        if (!g_fds.p[fds[i].fd].worker) {
          if (!(g_fds.p[fds[i].fd].worker = NewNtStdinWorker(fds[i].fd))) {
            // failed to launch stdin worker
            failed = -1;
            break;
          }
        }
        if (pn < ARRAYLEN(pipefds)) {
          pipeindices[pn] = i;
          pipefds[pn].handle = g_fds.p[fds[i].fd].handle;
          pipefds[pn].events = fds[i].events & (POLLPRI | POLLIN | POLLOUT);
          pn += 1;
        } else {
          // too many non-socket fds
          failed = enomem();
          break;
        }
      } else {
        // non-sock w/o pollin
        failed = enotsock();
        break;
      }
    } else {
      // non-open file descriptor
      failed = einval();
      break;
    }
  }
  _spunlock(&poll_lock);
  if (failed) {
    // failed to create a polling solution
    return failed;
  }

  // perform the i/o and sleeping and looping
  for (;;) {
    // see if input is available on non-sockets
    for (gotpipes = i = 0; i < pn; ++i) {
      ok = PeekNamedPipe(pipefds[i].handle, 0, 0, 0, &avail, 0);
      STRACE("PeekNamedPipe(%ld, 0, 0, 0, [%'u], 0) → %hhhd% m",
             pipefds[i].handle, avail, ok);
      if (ok) {
        if (avail) {
          pipefds[i].revents = POLLIN;
          gotpipes += 1;
        } else {
          pipefds[i].revents = 0;
        }
      } else {
        pipefds[i].revents = POLLERR;
        gotpipes += 1;
      }
    }
    // if we haven't found any good results yet then here we
    // compute a small time slice we don't mind sleeping for
    waitfor = gotpipes ? 0 : MIN(__SIG_POLLING_INTERVAL_MS, *ms);
    if (sn) {
      // we need to poll the socket handles separately because
      // microsoft certainly loves to challenge us with coding
      // please note that winsock will fail if we pass zero fd
      STRACE("WSAPoll(%p, %u, %'d) out of %'lu", sockfds, sn, waitfor, *ms);
      if ((gotsocks = WSAPoll(sockfds, sn, waitfor)) == -1) {
        return __winsockerr();
      }
      *ms -= waitfor;
    } else {
      gotsocks = 0;
      if (!gotpipes && waitfor) {
        // if we've only got pipes and none of them are ready
        // then we'll just explicitly sleep for the time left
        STRACE("SleepEx(%'d, false) out of %'lu", waitfor, *ms);
        if (SleepEx(waitfor, true) == kNtWaitIoCompletion) {
          STRACE("IOCP TRIGGERED EINTR");
          return eintr();
        }
        *ms -= waitfor;
      }
    }
    // we gave all the sockets and all the named pipes a shot
    // if we found anything at all then it's time to end work
    if (gotpipes || gotsocks || *ms <= 0) {
      break;
    }
    // otherwise loop limitlessly for timeout to elapse while
    // checking for signal delivery interrupts, along the way
    if (_check_interrupts(false, g_fds.p)) {
      return eintr();
    }
  }

  // we got some
  // assemble the result
  for (i = 0; i < pn; ++i) {
    fds[pipeindices[i]].revents =
        pipefds[i].handle < 0 ? 0 : pipefds[i].revents;
  }
  for (i = 0; i < sn; ++i) {
    fds[sockindices[i]].revents =
        sockfds[i].handle < 0 ? 0 : sockfds[i].revents;
  }
  return gotpipes + gotsocks;
}

static textexit void __freefds_workers(void) {
  int i;
  STRACE("__freefds_workers()");
  for (i = g_fds.n; i--;) {
    if (g_fds.p[i].kind && g_fds.p[i].worker) {
      close(i);
    }
  }
}

static textstartup void __freefds_workers_init(void) {
  atexit(__freefds_workers);
}

const void *const __freefds_workers_ctor[] initarray = {
    __freefds_workers_init,
};
