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
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
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
  int sig;
  uint32_t exchanged;
  struct Fd *f = g_fds.p + fd;

  // win32 i/o apis generally take 32-bit values thus we implicitly
  // truncate outrageously large sizes. linux actually does it too!
  size = MIN(size, 0x7ffff000);

  // pread() and pwrite() perform an implicit lseek() operation, so
  // similar to the lseek() system call, they too raise ESPIPE when
  // operating on a non-seekable file.
  bool pwriting = offset != -1;
  bool seekable =
      (f->kind == kFdFile && GetFileType(handle) == kNtFileTypeDisk) ||
      f->kind == kFdDevNull;
  if (pwriting && !seekable) {
    return espipe();
  }

  // when a file is opened in overlapped mode win32 requires that we
  // take over full responsibility for managing our own file pointer
  // which is fine, because the one win32 has was never very good in
  // the sense that it behaves so differently from linux, that using
  // win32 i/o required more compatibilty toil than doing it by hand
  if (!pwriting) {
    if (seekable) {
      offset = f->pointer;
    } else {
      offset = 0;
    }
  }

RestartOperation:
  bool eagained = false;
  // check for signals and cancelation
  if (_check_cancel() == -1) return -1;  // ECANCELED
  if (_weaken(__sig_get) && (sig = _weaken(__sig_get)(waitmask))) {
    goto HandleInterrupt;
  }

  // signals have already been fully blocked by caller
  // perform i/o operation with atomic signal/cancel checking
  struct NtOverlapped overlap = {.hEvent = CreateEvent(0, 1, 0, 0),
                                 .Pointer = offset};
  bool32 ok = ReadOrWriteFile(handle, data, size, 0, &overlap);
  if (!ok && GetLastError() == kNtErrorIoPending) {
    // win32 says this i/o operation needs to block
    if (f->flags & _O_NONBLOCK) {
      // abort the i/o operation if file descriptor is in non-blocking mode
      CancelIoEx(handle, &overlap);
      eagained = true;
    } else {
      // wait until i/o either completes or is canceled by another thread
      // we avoid a race condition by having a second mask for unblocking
      struct PosixThread *pt;
      pt = _pthread_self();
      pt->pt_blkmask = waitmask;
      pt->pt_iohandle = handle;
      pt->pt_ioverlap = &overlap;
      atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_IO,
                            memory_order_release);
      WaitForSingleObject(overlap.hEvent, -1u);
      atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
    }
    ok = true;
  }
  if (ok) {
    ok = GetOverlappedResult(handle, &overlap, &exchanged, true);
  }
  CloseHandle(overlap.hEvent);

  // if i/o succeeded then return its result
  if (ok) {
    if (!pwriting && seekable) {
      f->pointer = offset + exchanged;
    }
    return exchanged;
  }

  // only raise EINTR or EAGAIN if I/O got canceled
  if (GetLastError() == kNtErrorOperationAborted) {
    // raise EAGAIN if it's due to O_NONBLOCK mmode
    if (eagained) {
      return eagain();
    }
    // otherwise it must be due to a kill() via __sig_cancel()
    if (_weaken(__sig_relay) && (sig = _weaken(__sig_get)(waitmask))) {
    HandleInterrupt:
      int handler_was_called = _weaken(__sig_relay)(sig, SI_KERNEL, waitmask);
      if (_check_cancel() == -1) return -1;  // possible if we SIGTHR'd
      // read() is @restartable unless non-SA_RESTART hands were called
      if (!(handler_was_called & SIG_HANDLED_NO_RESTART)) {
        goto RestartOperation;
      }
    }
    return eintr();
  }

  // read() and write() have generally different error-handling paths
  return -2;
}

#endif /* __x86_64__ */
