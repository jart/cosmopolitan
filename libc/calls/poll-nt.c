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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/pollfd.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/pollfd.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

/*
 * Polls on the New Technology.
 *
 * This function is used to implement poll() and select(). You may poll
 * on both sockets and files at the same time. We also poll for signals
 * while poll is polling.
 */
textwindows int sys_poll_nt(struct pollfd *fds, uint64_t nfds, uint64_t *ms,
                            const sigset_t *sigmask) {
  bool ok;
  uint32_t avail;
  sigset_t oldmask;
  struct sys_pollfd_nt pipefds[8];
  struct sys_pollfd_nt sockfds[64];
  int pipeindices[ARRAYLEN(pipefds)];
  int sockindices[ARRAYLEN(sockfds)];
  int i, rc, sn, pn, gotinvals, gotpipes, gotsocks, waitfor;

  // check for interrupts early before doing work
  if (sigmask) {
    __sig_mask(SIG_SETMASK, sigmask, &oldmask);
  }
  if ((rc = _check_interrupts(false, g_fds.p))) {
    goto ReturnPath;
  }

  // do the planning
  // we need to read static variables
  // we might need to spawn threads and open pipes
  __fds_lock();
  for (gotinvals = rc = sn = pn = i = 0; i < nfds; ++i) {
    if (fds[i].fd < 0) continue;
    if (__isfdopen(fds[i].fd)) {
      if (__isfdkind(fds[i].fd, kFdSocket)) {
        if (sn < ARRAYLEN(sockfds)) {
          // the magnums for POLLIN/OUT/PRI on NT include the other ones too
          // we need to clear ones like POLLNVAL or else WSAPoll shall whine
          sockindices[sn] = i;
          sockfds[sn].handle = g_fds.p[fds[i].fd].handle;
          sockfds[sn].events = fds[i].events & (POLLPRI | POLLIN | POLLOUT);
          sockfds[sn].revents = 0;
          ++sn;
        } else {
          // too many socket fds
          rc = enomem();
          break;
        }
      } else if (pn < ARRAYLEN(pipefds)) {
        pipeindices[pn] = i;
        pipefds[pn].handle = g_fds.p[fds[i].fd].handle;
        pipefds[pn].events = 0;
        pipefds[pn].revents = 0;
        switch (g_fds.p[fds[i].fd].flags & O_ACCMODE) {
          case O_RDONLY:
            pipefds[pn].events = fds[i].events & POLLIN;
            break;
          case O_WRONLY:
            pipefds[pn].events = fds[i].events & POLLOUT;
            break;
          case O_RDWR:
            pipefds[pn].events = fds[i].events & (POLLIN | POLLOUT);
            break;
          default:
            unreachable;
        }
        ++pn;
      } else {
        // too many non-socket fds
        rc = enomem();
        break;
      }
    } else {
      ++gotinvals;
    }
  }
  __fds_unlock();
  if (rc) {
    // failed to create a polling solution
    goto ReturnPath;
  }

  // perform the i/o and sleeping and looping
  for (;;) {
    // see if input is available on non-sockets
    for (gotpipes = i = 0; i < pn; ++i) {
      if (pipefds[i].events & POLLOUT) {
        // we have no way of polling if a non-socket is writeable yet
        // therefore we assume that if it can happen, it shall happen
        pipefds[i].revents = POLLOUT;
      }
      if (pipefds[i].events & POLLIN) {
        if (GetFileType(pipefds[i].handle) == kNtFileTypePipe) {
          ok = PeekNamedPipe(pipefds[i].handle, 0, 0, 0, &avail, 0);
          POLLTRACE("PeekNamedPipe(%ld, 0, 0, 0, [%'u], 0) → %hhhd% m",
                    pipefds[i].handle, avail, ok);
          if (ok) {
            if (avail) {
              pipefds[i].revents = POLLIN;
            }
          } else {
            pipefds[i].revents = POLLERR;
          }
        } else {
          // we have no way of polling if a non-socket is readable yet
          // therefore we assume that if it can happen it shall happen
          pipefds[i].revents = POLLIN;
        }
      }
      if (pipefds[i].revents) {
        ++gotpipes;
      }
    }
    // if we haven't found any good results yet then here we
    // compute a small time slice we don't mind sleeping for
    waitfor = gotinvals || gotpipes ? 0 : MIN(__SIG_POLLING_INTERVAL_MS, *ms);
    if (sn) {
      // we need to poll the socket handles separately because
      // microsoft certainly loves to challenge us with coding
      // please note that winsock will fail if we pass zero fd
#if _NTTRACE
      POLLTRACE("WSAPoll(%p, %u, %'d) out of %'lu", sockfds, sn, waitfor, *ms);
#endif
      if ((gotsocks = WSAPoll(sockfds, sn, waitfor)) == -1) {
        rc = __winsockerr();
        goto ReturnPath;
      }
      *ms -= waitfor;
    } else {
      gotsocks = 0;
      if (!gotinvals && !gotpipes && waitfor) {
        // if we've only got pipes and none of them are ready
        // then we'll just explicitly sleep for the time left
        POLLTRACE("SleepEx(%'d, false) out of %'lu", waitfor, *ms);
        if (SleepEx(__SIG_POLLING_INTERVAL_MS, true) == kNtWaitIoCompletion) {
          POLLTRACE("IOCP EINTR");
        } else {
          *ms -= waitfor;
        }
      }
    }
    // we gave all the sockets and all the named pipes a shot
    // if we found anything at all then it's time to end work
    if (gotinvals || gotpipes || gotsocks || *ms <= 0) {
      break;
    }
    // otherwise loop limitlessly for timeout to elapse while
    // checking for signal delivery interrupts, along the way
    if ((rc = _check_interrupts(false, g_fds.p))) {
      goto ReturnPath;
    }
  }

  // the system call is going to succeed
  // it's now ok to start setting the output memory
  for (i = 0; i < nfds; ++i) {
    if (fds[i].fd < 0 || __isfdopen(fds[i].fd)) {
      fds[i].revents = 0;
    } else {
      fds[i].revents = POLLNVAL;
    }
  }
  for (i = 0; i < pn; ++i) {
    fds[pipeindices[i]].revents = pipefds[i].revents;
  }
  for (i = 0; i < sn; ++i) {
    fds[sockindices[i]].revents = sockfds[i].revents;
  }

  // and finally return
  rc = gotinvals + gotpipes + gotsocks;

ReturnPath:
  if (sigmask) {
    __sig_mask(SIG_SETMASK, &oldmask, 0);
  }
  return rc;
}
