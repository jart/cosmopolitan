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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/fds.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/pollfd.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/time.h"
#include "libc/nt/winsock.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/internal.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/pollfd.internal.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sicode.h"
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

textwindows static dontinline struct timespec sys_poll_nt_now(void) {
  uint64_t hectons;
  QueryUnbiasedInterruptTimePrecise(&hectons);
  return timespec_fromnanos(hectons * 100);
}

textwindows static int sys_poll_nt_sigcheck(sigset_t sigmask) {
  int sig, handler_was_called;
  if (_check_cancel() == -1)
    return -1;
  if (_weaken(__sig_get) && (sig = _weaken(__sig_get)(sigmask))) {
    handler_was_called = _weaken(__sig_relay)(sig, SI_KERNEL, sigmask);
    if (_check_cancel() == -1)
      return -1;
    if (handler_was_called)
      return eintr();
  }
  return 0;
}

// Polls on the New Technology.
//
// This function is used to implement poll() and select(). You may poll
// on sockets, files and the console at the same time. We also poll for
// both signals and posix thread cancelation, while the poll is polling
textwindows static int sys_poll_nt_impl(struct pollfd *fds, uint64_t nfds,
                                        uint32_t *ms, sigset_t sigmask) {
  bool ok;
  uint64_t millis;
  int fileindices[64];
  int sockindices[64];
  int64_t filehands[64];
  struct PosixThread *pt;
  int i, rc, ev, kind, gotsocks;
  struct sys_pollfd_nt sockfds[64];
  struct timespec deadline, remain, now;
  uint32_t cm, fi, wi, sn, pn, avail, waitfor, already_slept;

  waitfor = ms ? *ms : -1u;
  deadline = timespec_add(sys_poll_nt_now(), timespec_frommillis(waitfor));

  // ensure revents is cleared
  for (i = 0; i < nfds; ++i)
    fds[i].revents = 0;

  // divide files from sockets
  // check for invalid file descriptors
  __fds_lock();
  for (rc = sn = pn = i = 0; i < nfds; ++i) {
    if (fds[i].fd < 0)
      continue;
    if (__isfdopen(fds[i].fd)) {
      kind = g_fds.p[fds[i].fd].kind;
      if (kind == kFdSocket) {
        // we can use WSAPoll() for these fds
        if (sn < ARRAYLEN(sockfds)) {
          // WSAPoll whines if we pass POLLNVAL, POLLHUP, or POLLERR.
          sockindices[sn] = i;
          sockfds[sn].handle = g_fds.p[fds[i].fd].handle;
          sockfds[sn].events =
              fds[i].events & (POLLRDNORM_ | POLLRDBAND_ | POLLWRNORM_);
          sockfds[sn].revents = 0;
          ++sn;
        } else {
          // too many sockets
          rc = einval();
          break;
        }
      } else if (kind == kFdFile || kind == kFdConsole) {
        // we can use WaitForMultipleObjects() for these fds
        if (pn < ARRAYLEN(fileindices) - 1) {  // last slot for semaphore
          fileindices[pn] = i;
          filehands[pn] = g_fds.p[fds[i].fd].handle;
          ++pn;
        } else {
          // too many files
          rc = einval();
          break;
        }
      } else if (kind == kFdDevNull || kind == kFdDevRandom || kind == kFdZip) {
        // we can't wait on these kinds via win32
        if (fds[i].events & (POLLRDNORM_ | POLLWRNORM_)) {
          // the linux kernel does this irrespective of oflags
          fds[i].revents = fds[i].events & (POLLRDNORM_ | POLLWRNORM_);
        }
      } else {
        // unsupported file type
        fds[i].revents = POLLNVAL_;
      }
    } else {
      // file not open
      fds[i].revents = POLLNVAL_;
    }
    rc += !!fds[i].revents;
  }
  __fds_unlock();
  if (rc)
    return rc;

  // perform poll operation
  for (;;) {

    // check input status of pipes / consoles without blocking
    // this ensures any socket fds won't starve them of events
    // if a file handle is POLLOUT only, we just mark it ready
    for (i = 0; i < pn; ++i) {
      fi = fileindices[i];
      ev = fds[fi].events;
      ev &= POLLRDNORM_ | POLLWRNORM_;
      if ((g_fds.p[fds[fi].fd].flags & O_ACCMODE) == O_RDONLY)
        ev &= ~POLLWRNORM_;
      if ((g_fds.p[fds[fi].fd].flags & O_ACCMODE) == O_WRONLY)
        ev &= ~POLLRDNORM_;
      if ((ev & POLLWRNORM_) && !(ev & POLLRDNORM_)) {
        fds[fi].revents = fds[fi].events & (POLLRDNORM_ | POLLWRNORM_);
      } else if (GetFileType(filehands[i]) == kNtFileTypePipe) {
        ok = PeekNamedPipe(filehands[i], 0, 0, 0, &avail, 0);
        POLLTRACE("PeekNamedPipe(%ld, 0, 0, 0, [%'u], 0) → {%hhhd, %d}",
                  filehands[i], avail, ok, GetLastError());
        if (ok) {
          if (avail)
            fds[fi].revents = POLLRDNORM_;
        } else if (GetLastError() == kNtErrorHandleEof ||
                   GetLastError() == kNtErrorBrokenPipe) {
          fds[fi].revents = POLLHUP_;
        } else {
          fds[fi].revents = POLLERR_;
        }
      } else if (GetConsoleMode(filehands[i], &cm)) {
        switch (CountConsoleInputBytes()) {
          case 0:
            fds[fi].revents = fds[fi].events & POLLWRNORM_;
            break;
          case -1:
            fds[fi].revents = POLLHUP_;
            break;
          default:
            fds[fi].revents = fds[fi].events & (POLLRDNORM_ | POLLWRNORM_);
            break;
        }
      }
      rc += !!fds[fi].revents;
    }

    // determine how long to wait
    now = sys_poll_nt_now();
    if (timespec_cmp(now, deadline) < 0) {
      remain = timespec_sub(deadline, now);
      millis = timespec_tomillis(remain);
      waitfor = MIN(millis, 0xffffffffu);
      waitfor = MIN(waitfor, POLL_INTERVAL_MS);
    } else {
      waitfor = 0;  // we timed out
    }

    // check for events and/or readiness on sockets
    // we always do this due to issues with POLLOUT
    if (sn) {
      // if we need to wait, then we prefer to wait inside WSAPoll()
      // this ensures network events are received in ~10µs not ~10ms
      if (!rc && waitfor) {
        if (sys_poll_nt_sigcheck(sigmask))
          return -1;
        already_slept = waitfor;
      } else {
        already_slept = 0;
      }
      if ((gotsocks = WSAPoll(sockfds, sn, already_slept)) == -1)
        return __winsockerr();
      if (gotsocks) {
        for (i = 0; i < sn; ++i)
          if (sockfds[i].revents) {
            fds[sockindices[i]].revents = sockfds[i].revents;
            ++rc;
          }
      } else if (already_slept) {
        if (sys_poll_nt_sigcheck(sigmask))
          return -1;
      }
    } else {
      already_slept = 0;
    }

    // return if we observed events
    if (rc || !waitfor)
      break;

    // if nothing has happened and we haven't already waited in poll()
    // then we can wait on consoles, pipes, and signals simultaneously
    // this ensures low latency for apps like emacs which with no sock
    // here we shall actually report that something can be written too
    if (!already_slept) {
      if (sys_poll_nt_sigcheck(sigmask))
        return -1;
      pt = _pthread_self();
      filehands[pn] = pt->pt_semaphore = CreateSemaphore(0, 0, 1, 0);
      atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_SEM,
                            memory_order_release);
      wi = WaitForMultipleObjects(pn + 1, filehands, 0, waitfor);
      atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
      CloseHandle(filehands[pn]);
      if (wi == -1u) {
        // win32 wait failure
        return __winerr();
      } else if (wi == pn) {
        // our semaphore was signalled
        if (sys_poll_nt_sigcheck(sigmask))
          return -1;
      } else if ((wi ^ kNtWaitAbandoned) < pn) {
        // this is possibly because a process or thread was killed
        fds[fileindices[wi ^ kNtWaitAbandoned]].revents = POLLERR_;
        ++rc;
      } else if (wi < pn) {
        fi = fileindices[wi];
        // one of the handles we polled is ready for fi/o
        if (GetConsoleMode(filehands[wi], &cm)) {
          switch (CountConsoleInputBytes()) {
            case 0:
              // it's possible there was input and it was handled by the
              // ICANON reader, and therefore should not be reported yet
              if (fds[fi].events & POLLWRNORM_)
                fds[fi].revents = POLLWRNORM_;
              break;
            case -1:
              fds[fi].revents = POLLHUP_;
              break;
            default:
              fds[fi].revents = fds[fi].events & (POLLRDNORM_ | POLLWRNORM_);
              break;
          }
        } else if (GetFileType(filehands[wi]) == kNtFileTypePipe) {
          if ((fds[fi].events & POLLRDNORM_) &&
              (g_fds.p[fds[fi].fd].flags & O_ACCMODE) != O_WRONLY) {
            if (PeekNamedPipe(filehands[wi], 0, 0, 0, &avail, 0)) {
              fds[fi].revents = fds[fi].events & (POLLRDNORM_ | POLLWRNORM_);
            } else if (GetLastError() == kNtErrorHandleEof ||
                       GetLastError() == kNtErrorBrokenPipe) {
              fds[fi].revents = POLLHUP_;
            } else {
              fds[fi].revents = POLLERR_;
            }
          } else {
            fds[fi].revents = fds[fi].events & (POLLRDNORM_ | POLLWRNORM_);
          }
        } else {
          fds[fi].revents = fds[fi].events & (POLLRDNORM_ | POLLWRNORM_);
        }
        rc += !!fds[fi].revents;
      } else {
        // should only be possible on kNtWaitTimeout or semaphore abandoned
        // keep looping for events and we'll catch timeout when appropriate
        if (sys_poll_nt_sigcheck(sigmask))
          return -1;
      }
    }

    // once again, return if we observed events
    if (rc)
      break;
  }

  return rc;
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
