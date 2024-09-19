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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/struct/pollfd.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#ifdef __x86_64__

textwindows ssize_t
__winsock_block(int64_t handle, uint32_t flags, bool nonblock,
                uint32_t srwtimeout, sigset_t waitmask,
                int StartSocketOp(int64_t handle, struct NtOverlapped *overlap,
                                  uint32_t *flags, void *arg),
                void *arg) {

  // convert relative to absolute timeout
  struct timespec deadline;
  if (srwtimeout) {
    deadline = timespec_add(sys_clock_gettime_monotonic_nt(),
                            timespec_frommillis(srwtimeout));
  } else {
    deadline = timespec_max;
  }

  for (;;) {
    int got_sig = 0;
    bool got_cancel = false;
    bool got_eagain = false;
    uint32_t other_error = 0;

    // create event handle for overlapped i/o
    intptr_t event;
    if (!(event = WSACreateEvent()))
      return __winsockerr();

    struct NtOverlapped overlap = {.hEvent = event};
    bool32 ok = !StartSocketOp(handle, &overlap, &flags, arg);
    if (!ok && WSAGetLastError() == kNtErrorIoPending) {
      if (nonblock) {
        // send() and sendto() shall not pass O_NONBLOCK along to here
        // because winsock has a bug that causes CancelIoEx() to cause
        // WSAGetOverlappedResult() to report errors when it succeeded
        CancelIoEx(handle, &overlap);
        got_eagain = true;
      } else {
        // atomic block on i/o completion, signal, or cancel
        // it's not safe to acknowledge cancelation from here
        // it's not safe to call any signal handlers from here
        intptr_t sev;
        if ((sev = CreateEvent(0, 0, 0, 0))) {
          // installing semaphore before sig get makes wait atomic
          struct PosixThread *pt = _pthread_self();
          pt->pt_event = sev;
          pt->pt_blkmask = waitmask;
          atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_EVENT,
                                memory_order_release);
          if (_is_canceled()) {
            got_cancel = true;
            CancelIoEx(handle, &overlap);
          } else if (_weaken(__sig_get) &&
                     (got_sig = _weaken(__sig_get)(waitmask))) {
            CancelIoEx(handle, &overlap);
          } else {
            struct timespec now = sys_clock_gettime_monotonic_nt();
            struct timespec remain = timespec_subz(deadline, now);
            int64_t millis = timespec_tomillis(remain);
            uint32_t waitms = MIN(millis, 0xffffffffu);
            intptr_t hands[] = {event, sev};
            uint32_t wi = WSAWaitForMultipleEvents(2, hands, 0, waitms, 0);
            if (wi == 1) {  // semaphore was signaled by signal enqueue
              CancelIoEx(handle, &overlap);
              if (_weaken(__sig_get))
                got_sig = _weaken(__sig_get)(waitmask);
            } else if (wi == kNtWaitTimeout) {
              CancelIoEx(handle, &overlap);
              got_eagain = true;
            } else if (wi == -1u) {
              other_error = WSAGetLastError();
              CancelIoEx(handle, &overlap);
            }
          }
          atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
          CloseHandle(sev);
        } else {
          other_error = GetLastError();
          CancelIoEx(handle, &overlap);
        }
      }
      ok = true;
    }
    uint32_t exchanged = 0;
    if (ok)
      ok = WSAGetOverlappedResult(handle, &overlap, &exchanged, true, &flags);
    uint32_t io_error = WSAGetLastError();
    WSACloseEvent(event);

    // check if i/o completed
    // this could forseeably happen even if CancelIoEx was called
    if (ok) {
      if (got_sig)  // swallow dequeued signal
        _weaken(__sig_relay)(got_sig, SI_KERNEL, waitmask);
      return exchanged;
    }

    // check if i/o failed
    if (io_error != kNtErrorOperationAborted) {
      if (got_sig)  // swallow dequeued signal
        _weaken(__sig_relay)(got_sig, SI_KERNEL, waitmask);
      errno = __dos2errno(io_error);
      return -1;
    }

    // it's now reasonable to report semaphore creation error
    if (other_error) {
      unassert(!got_sig);
      errno = __dos2errno(other_error);
      return -1;
    }

    // check for non-block cancel or timeout
    if (got_eagain && !got_sig && !got_cancel)
      return eagain();

    // check for thread cancelation and acknowledge
    if (_check_cancel() == -1)
      return -1;

    // if signal module has been linked, then
    if (_weaken(__sig_get)) {

      // gobble up all unmasked pending signals
      // it's now safe to recurse into signal handlers
      int handler_was_called = 0;
      do {
        if (got_sig)
          handler_was_called |=
              _weaken(__sig_relay)(got_sig, SI_KERNEL, waitmask);
      } while ((got_sig = _weaken(__sig_get)(waitmask)));

      // check if SIGTHR handler was called
      if (_check_cancel() == -1)
        return -1;

      // check if signal handler without SA_RESTART was called
      if (handler_was_called & SIG_HANDLED_NO_RESTART)
        return eintr();

      // emulates linux behavior of having timeouts @norestart
      if (handler_was_called & SIG_HANDLED_SA_RESTART)
        if (srwtimeout)
          return eintr();
    }

    // otherwise try the i/o operation again
  }
}

#endif /* __x86_64__ */
