/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/strace.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/pollfd.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/internal.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/pollfd.internal.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

#define POLL_INTERVAL_MS 10

// <sync libc/sysv/consts.sh>
#define POLLERR_    0x0001  // implied in events
#define POLLHUP_    0x0002  // implied in events
#define POLLNVAL_   0x0004  // implied in events
#define POLLIN_     0x0300
#define POLLRDNORM_ 0x0100
#define POLLRDBAND_ 0x0200
#define POLLOUT_    0x0010
#define POLLWRNORM_ 0x0010
#define POLLWRBAND_ 0x0020  // MSDN undocumented
#define POLLPRI_    0x0400  // MSDN unsupported
// </sync libc/sysv/consts.sh>

// Polls on the New Technology.
//
// This function is used to implement poll() and select(). You may poll
// on sockets, files and the console at the same time. We also poll for
// both signals and posix thread cancelation, while the poll is polling
static textwindows int sys_poll_nt_impl(struct pollfd *fds, uint64_t nfds,
                                        uint32_t *ms, sigset_t sigmask) {
  bool ok;
  uint64_t millis;
  uint32_t cm, avail, waitfor;
  struct sys_pollfd_nt pipefds[8];
  struct sys_pollfd_nt sockfds[64];
  int pipeindices[ARRAYLEN(pipefds)];
  int sockindices[ARRAYLEN(sockfds)];
  struct timespec started, deadline, remain, now;
  int i, rc, sn, pn, gotinvals, gotpipes, gotsocks;

  started = timespec_real();
  deadline = timespec_add(started, timespec_frommillis(ms ? *ms : -1u));

  // do the planning
  // we need to read static variables
  // we might need to spawn threads and open pipes
  __fds_lock();
  for (gotinvals = rc = sn = pn = i = 0; i < nfds; ++i) {
    if (fds[i].fd < 0)
      continue;
    if (__isfdopen(fds[i].fd)) {
      if (__isfdkind(fds[i].fd, kFdSocket)) {
        if (sn < ARRAYLEN(sockfds)) {
          // WSAPoll whines if we pass POLLNVAL, POLLHUP, or POLLERR.
          sockindices[sn] = i;
          sockfds[sn].handle = g_fds.p[fds[i].fd].handle;
          sockfds[sn].events =
              fds[i].events & (POLLRDNORM_ | POLLRDBAND_ | POLLWRNORM_);
          sockfds[sn].revents = 0;
          ++sn;
        } else {
          // too many socket fds
          rc = e2big();
          break;
        }
      } else if (pn < ARRAYLEN(pipefds)) {
        pipeindices[pn] = i;
        pipefds[pn].handle = g_fds.p[fds[i].fd].handle;
        pipefds[pn].events = 0;
        pipefds[pn].revents = 0;
        switch (g_fds.p[fds[i].fd].flags & O_ACCMODE) {
          case O_RDONLY:
            pipefds[pn].events = fds[i].events & POLLIN_;
            break;
          case O_WRONLY:
            pipefds[pn].events = fds[i].events & POLLOUT_;
            break;
          case O_RDWR:
            pipefds[pn].events = fds[i].events & (POLLIN_ | POLLOUT_);
            break;
          default:
            break;
        }
        ++pn;
      } else {
        // too many non-socket fds
        rc = e2big();
        break;
      }
    } else {
      ++gotinvals;
    }
  }
  __fds_unlock();
  if (rc)
    // failed to create a polling solution
    return rc;

  // perform the i/o and sleeping and looping
  for (;;) {
    // see if input is available on non-sockets
    for (gotpipes = i = 0; i < pn; ++i) {
      if (pipefds[i].events & POLLWRNORM_)
        // we have no way of polling if a non-socket is writeable yet
        // therefore we assume that if it can happen, it shall happen
        pipefds[i].revents |= POLLWRNORM_;
      if (GetFileType(pipefds[i].handle) == kNtFileTypePipe) {
        ok = PeekNamedPipe(pipefds[i].handle, 0, 0, 0, &avail, 0);
        POLLTRACE("PeekNamedPipe(%ld, 0, 0, 0, [%'u], 0) → {%hhhd, %d}",
                  pipefds[i].handle, avail, ok, GetLastError());
        if (ok) {
          if (avail)
            pipefds[i].revents |= POLLRDNORM_;
        } else if (GetLastError() == kNtErrorHandleEof ||
                   GetLastError() == kNtErrorBrokenPipe) {
          pipefds[i].revents &= ~POLLWRNORM_;
          pipefds[i].revents |= POLLHUP_;
        } else {
          pipefds[i].revents &= ~POLLWRNORM_;
          pipefds[i].revents |= POLLERR_;
        }
      } else if (GetConsoleMode(pipefds[i].handle, &cm)) {
        switch (CountConsoleInputBytes()) {
          case 0:
            break;
          case -1:
            pipefds[i].revents &= ~POLLWRNORM_;
            pipefds[i].revents |= POLLHUP_;
            break;
          default:
            pipefds[i].revents |= POLLRDNORM_;
            break;
        }
      } else {
        // we have no way of polling if a non-socket is readable yet
        // therefore we assume that if it can happen it shall happen
        pipefds[i].revents |= POLLRDNORM_;
      }
      if (!(pipefds[i].events & POLLRDNORM_))
        pipefds[i].revents &= ~POLLRDNORM_;
      if (pipefds[i].revents)
        ++gotpipes;
    }
    // if we haven't found any good results yet then here we
    // compute a small time slice we don't mind sleeping for
    if (sn) {
      if ((gotsocks = WSAPoll(sockfds, sn, 0)) == -1)
        return __winsockerr();
    } else {
      gotsocks = 0;
    }

    // add some artificial delay, which we use as an opportunity to also
    // check for pending signals, thread cancelation, etc.
    waitfor = 0;
    if (!gotinvals && !gotsocks && !gotpipes) {
      now = timespec_real();
      if (timespec_cmp(now, deadline) < 0) {
        remain = timespec_sub(deadline, now);
        millis = timespec_tomillis(remain);
        waitfor = MIN(millis, 0xffffffffu);
        waitfor = MIN(waitfor, POLL_INTERVAL_MS);
        if (waitfor) {
          POLLTRACE("poll() sleeping for %'d out of %'lu ms", waitfor,
                    timespec_tomillis(remain));
          if ((rc = _park_norestart(waitfor, sigmask)) == -1)
            return -1;  // eintr, ecanceled, etc.
        }
      }
    }

    // we gave all the sockets and all the named pipes a shot
    // if we found anything at all then it's time to end work
    if (gotinvals || gotpipes || gotsocks || !waitfor)
      break;
  }

  // the system call is going to succeed
  // it's now ok to start setting the output memory
  for (i = 0; i < nfds; ++i) {
    if (fds[i].fd < 0 || __isfdopen(fds[i].fd)) {
      fds[i].revents = 0;
    } else {
      fds[i].revents = POLLNVAL_;
    }
  }
  for (i = 0; i < pn; ++i)
    fds[pipeindices[i]].revents = pipefds[i].revents;
  for (i = 0; i < sn; ++i)
    fds[sockindices[i]].revents = sockfds[i].revents;

  // and finally return
  return gotinvals + gotpipes + gotsocks;
}

textwindows int sys_poll_nt(struct pollfd *fds, uint64_t nfds, uint32_t *ms,
                            const sigset_t *sigmask) {
  int rc;
  BLOCK_SIGNALS;
  rc = sys_poll_nt_impl(fds, nfds, ms, sigmask ? *sigmask : 0);
  ALLOW_SIGNALS;
  return rc;
}

#endif /* __x86_64__ */
