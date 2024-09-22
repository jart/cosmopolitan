/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/createfileflags.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/intrin/fds.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/sock/internal.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

/**
 * Runs code that's common to read/write/pread/pwrite/etc on Windows.
 *
 * @return bytes exchanged, or -1 w/ errno, or -2 if operation failed
 *     and caller needs to do more work, examining the GetLastError()
 */
textwindows ssize_t
sys_readwrite_nt(int fd, void *data, size_t size, ssize_t offset,
                 int64_t handle, sigset_t waitmask,
                 bool32 ReadOrWriteFile(int64_t, void *, uint32_t, uint32_t *,
                                        struct NtOverlapped *)) {
  struct Fd *f = g_fds.p + fd;

  // pread() and pwrite() perform an implicit lseek() operation, so
  // similar to the lseek() system call, they too raise ESPIPE when
  // operating on a non-seekable file.
  bool pwriting = offset != -1;
  bool isdisk = f->kind == kFdFile && GetFileType(handle) == kNtFileTypeDisk;
  bool seekable = isdisk || f->kind == kFdDevNull || f->kind == kFdDevRandom;
  if (pwriting && !seekable)
    return espipe();

  // determine if we need to lock file descriptor
  bool locked = isdisk && !pwriting && f->cursor;

  for (;;) {
    int got_sig = 0;
    bool got_eagain = false;
    uint32_t other_error = 0;

    // create event handle for overlapped i/o
    intptr_t event;
    if (!(event = CreateEvent(0, 1, 0, 0)))
      return __winerr();

    // ensure iops are ordered across threads and processes if seeking
    if (locked)
      __cursor_lock(f->cursor);

    // when a file is opened in overlapped mode win32 requires that we
    // take over full responsibility for managing our own file pointer
    // which is fine, because the one win32 has was never very good in
    // the sense that it behaves so differently from linux, that using
    // win32 i/o required more compatibilty toil than doing it by hand
    if (!pwriting) {
      if (seekable && f->cursor) {
        offset = f->cursor->shared->pointer;
      } else {
        offset = 0;
      }
    }

    // initiate asynchronous i/o operation with win32
    struct NtOverlapped overlap = {.hEvent = event, .Pointer = offset};
    bool32 ok = ReadOrWriteFile(handle, data, size, 0, &overlap);
    if (!ok && GetLastError() == kNtErrorIoPending) {
      if (f->flags & _O_NONBLOCK) {
        // immediately back out of blocking i/o if non-blocking
        CancelIoEx(handle, &overlap);
        got_eagain = true;
      } else {
        // atomic block on i/o completion, signal, or cancel
        // it's not safe to acknowledge cancelation from here
        // it's not safe to call any signal handlers from here
        intptr_t sigev;
        if ((sigev = CreateEvent(0, 0, 0, 0))) {
          // installing semaphore before sig get makes wait atomic
          struct PosixThread *pt = _pthread_self();
          pt->pt_event = sigev;
          pt->pt_blkmask = waitmask;
          atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_EVENT,
                                memory_order_release);
          if (_is_canceled()) {
            CancelIoEx(handle, &overlap);
          } else if (_weaken(__sig_get) &&
                     (got_sig = _weaken(__sig_get)(waitmask))) {
            CancelIoEx(handle, &overlap);
          } else {
            intptr_t hands[] = {event, sigev};
            uint32_t wi = WaitForMultipleObjects(2, hands, 0, -1u);
            if (wi == 1) {  // event was signaled by signal enqueue
              CancelIoEx(handle, &overlap);
              if (_weaken(__sig_get))
                got_sig = _weaken(__sig_get)(waitmask);
            } else if (wi == -1u) {
              other_error = GetLastError();
              CancelIoEx(handle, &overlap);
            }
          }
          atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
          CloseHandle(sigev);
        } else {
          other_error = GetLastError();
          CancelIoEx(handle, &overlap);
        }
      }
      ok = true;
    }
    uint32_t exchanged = 0;
    if (ok)
      ok = GetOverlappedResult(handle, &overlap, &exchanged, true);
    uint32_t io_error = GetLastError();
    CloseHandle(event);

    // check if i/o completed
    // this could forseeably happen even if CancelIoEx was called
    if (ok) {
      if (!pwriting && seekable && f->cursor)
        f->cursor->shared->pointer = offset + exchanged;
      if (locked)
        __cursor_unlock(f->cursor);
      if (got_sig)  // swallow dequeued signal
        _weaken(__sig_relay)(got_sig, SI_KERNEL, waitmask);
      return exchanged;
    }

    // it's now safe to unlock cursor
    if (locked)
      __cursor_unlock(f->cursor);

    // check if i/o failed
    if (io_error != kNtErrorOperationAborted) {
      if (got_sig)  // swallow dequeued signal
        _weaken(__sig_relay)(got_sig, SI_KERNEL, waitmask);
      // read() and write() have different error paths
      SetLastError(io_error);
      return -2;
    }

    // the i/o operation was successfully canceled
    if (got_eagain)
      return eagain();

    // it's now reasonable to report semaphore creation error
    if (other_error) {
      errno = __dos2errno(other_error);
      return -1;
    }

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
    }

    // otherwise try the i/o operation again
  }
}

#endif /* __x86_64__ */
