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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/fds.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/pollfd.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/time.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#ifdef __x86_64__

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

textwindows static uint32_t sys_poll_nt_waitms(struct timespec deadline) {
  struct timespec now = sys_clock_gettime_monotonic_nt();
  if (timespec_cmp(now, deadline) < 0) {
    struct timespec remain = timespec_sub(deadline, now);
    int64_t millis = timespec_tomillis(remain);
    uint32_t waitfor = MIN(millis, 0xffffffffu);
    return MIN(waitfor, POLL_INTERVAL_MS);
  } else {
    return 0;  // we timed out
  }
}

// Polls on the New Technology.
//
// This function is used to implement poll() and select(). You may poll
// on sockets, files and the console at the same time. We also poll for
// both signals and posix thread cancelation, while the poll is polling
textwindows static int sys_poll_nt_actual(struct pollfd *fds, uint64_t nfds,
                                          struct timespec deadline,
                                          sigset_t waitmask) {
  int fileindices[64];
  int sockindices[64];
  int64_t filehands[64];
  struct PosixThread *pt;
  int i, rc, ev, kind, gotsocks;
  struct sys_pollfd_nt sockfds[64];
  uint32_t cm, fi, sn, pn, avail, waitfor, already_slept;

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
        if (pn < ARRAYLEN(fileindices) - 1) {  // last slot for signal event
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
  if (rc == -1)
    return rc;

  // perform poll operation
  for (;;) {

    // check input status of pipes / consoles without blocking
    // this ensures any socket fds won't starve them of events
    // we can't poll file handles, so we just mark those ready
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
        if (PeekNamedPipe(filehands[i], 0, 0, 0, &avail, 0)) {
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
      } else {
        fds[fi].revents = fds[fi].events & (POLLRDNORM_ | POLLWRNORM_);
      }
      rc += !!fds[fi].revents;
    }

    // determine how long to wait
    waitfor = sys_poll_nt_waitms(deadline);

    // check for events and/or readiness on sockets
    // we always do this due to issues with POLLOUT
    if (sn) {
      // if we need to wait, then we prefer to wait inside WSAPoll()
      // this ensures network events are received in ~10µs not ~10ms
      if (!rc && waitfor) {
        if (__sigcheck(waitmask, false))
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
        if (__sigcheck(waitmask, false))
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
      intptr_t sigev;
      if (!(sigev = CreateEvent(0, 0, 0, 0)))
        return __winerr();
      filehands[pn] = sigev;
      pt = _pthread_self();
      pt->pt_event = sigev;
      pt->pt_blkmask = waitmask;
      atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_EVENT,
                            memory_order_release);
      //!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!//
      int sig = 0;
      uint32_t wi = pn;
      if (!_is_canceled() &&
          !(_weaken(__sig_get) && (sig = _weaken(__sig_get)(waitmask))))
        wi = WaitForMultipleObjects(pn + 1, filehands, 0, waitfor);
      //!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!/!//
      atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
      CloseHandle(sigev);
      if (wi == -1u)
        // win32 wait failure
        return __winerr();
      if (wi == pn) {
        // our signal event was signalled
        int handler_was_called = 0;
        if (sig)
          handler_was_called = _weaken(__sig_relay)(sig, SI_KERNEL, waitmask);
        if (_check_cancel() == -1)
          return -1;
        if (handler_was_called)
          return eintr();
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
      }
    }

    // once again, return if we observed events
    if (rc)
      break;
  }

  return rc;
}

textwindows static int sys_poll_nt_impl(struct pollfd *fds, uint64_t nfds,
                                        struct timespec deadline,
                                        const sigset_t waitmask) {
  int i, n, rc, got = 0;
  struct timespec now, next, target;

  // we normally don't check for signals until we decide to wait, since
  // it's nice to have functions like write() be unlikely to EINTR, but
  // ppoll is a function where users are surely thinking about signals,
  // since ppoll actually allows them to block signals everywhere else.
  if (__sigcheck(waitmask, false))
    return -1;

  // fast path
  if (nfds <= 63)
    return sys_poll_nt_actual(fds, nfds, deadline, waitmask);

  // clumsy path
  for (;;) {
    for (i = 0; i < nfds; i += 64) {
      n = nfds - i;
      n = n > 64 ? 64 : n;
      rc = sys_poll_nt_actual(fds + i, n, timespec_zero, waitmask);
      if (rc == -1)
        return -1;
      got += rc;
    }
    if (got)
      return got;
    now = sys_clock_gettime_monotonic_nt();
    if (timespec_cmp(now, deadline) >= 0)
      return 0;
    next = timespec_add(now, timespec_frommillis(POLL_INTERVAL_MS));
    if (timespec_cmp(next, deadline) >= 0) {
      target = deadline;
    } else {
      target = next;
    }
    if (_park_norestart(target, waitmask) == -1)
      return -1;
  }
}

textwindows int sys_poll_nt(struct pollfd *fds, uint64_t nfds,
                            const struct timespec *relative,
                            const sigset_t *sigmask) {
  int rc;
  struct timespec now, timeout, deadline;
  BLOCK_SIGNALS;
  now = relative ? sys_clock_gettime_monotonic_nt() : timespec_zero;
  timeout = relative ? *relative : timespec_max;
  deadline = timespec_add(now, timeout);
  rc = sys_poll_nt_impl(fds, nfds, deadline, sigmask ? *sigmask : _SigMask);
  ALLOW_SIGNALS;
  return rc;
}

#endif /* __x86_64__ */
