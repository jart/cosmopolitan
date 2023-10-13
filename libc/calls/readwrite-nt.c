/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

struct ReadwriteResources {
  int64_t handle;
  struct NtOverlapped *overlap;
};

static void UnwindReadwrite(void *arg) {
  uint32_t got;
  struct ReadwriteResources *rwc = arg;
  CancelIoEx(rwc->handle, rwc->overlap);
  GetOverlappedResult(rwc->handle, rwc->overlap, &got, true);
  CloseHandle(rwc->overlap->hEvent);
}

/**
 * Runs code that's common to read/write/pread/pwrite/etc on Windows.
 *
 * @return bytes exchanged, or -1 w/ errno, or -2 if operation failed
 *     and caller needs to do more work, examining the GetLastError()
 */
textwindows ssize_t
sys_readwrite_nt(int fd, void *data, size_t size, ssize_t offset,
                 int64_t handle, uint64_t waitmask,
                 bool32 ReadOrWriteFile(int64_t, void *, uint32_t, uint32_t *,
                                        struct NtOverlapped *)) {
  bool32 ok;
  uint64_t m;
  uint32_t exchanged;
  int olderror = errno;
  bool eagained = false;
  bool eintered = false;
  bool canceled = false;
  struct PosixThread *pt;
  struct Fd *f = g_fds.p + fd;

  // win32 i/o apis generally take 32-bit values thus we implicitly
  // truncate outrageously large sizes. linux actually does it too!
  size = MIN(size, 0x7ffff000);

  // pread() and pwrite() perform an implicit lseek() operation, so
  // similar to the lseek() system call, they too raise ESPIPE when
  // operating on a non-seekable file.
  bool pwriting = offset != -1;
  bool seekable = f->kind == kFdFile && GetFileType(handle) == kNtFileTypeDisk;
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

  // managing an overlapped i/o operation is tricky to do using just
  // imperative procedural logic. its design lends itself more to be
  // something that's abstracted in an object-oriented design, which
  // easily manages the unusual lifecycles requirements of the thing
  // the game here is to not return until win32 is done w/ `overlap`
  // next we need to allow signal handlers to re-enter this function
  // while we're performing a read in the same thread. this needs to
  // be thread safe too of course. read() / write() are cancelation
  // points so pthread_cancel() might teleport the execution here to
  // pthread_exit(), so we need cleanup handlers that pthread_exit()
  // can call, pushed onto the stack, so we don't leak win32 handles
  // or worse trash the thread stack containing `overlap` that win32
  // temporarily owns while the overlapped i/o op is being performed
  // we implement a non-blocking iop by optimistically performing io
  // and then aborting the operation if win32 says it needs to block
  // with cancelation points, we need to be able to raise eintr when
  // this thread is pre-empted to run a signal handler but only when
  // that signal handler wasn't installed using this SA_RESTART flag
  // in which case read() and write() will keep going afterwards. we
  // support a second kind of eintr in cosmo/musl which is ecanceled
  // and it's mission critical that it be relayed properly, since it
  // can only be returned by a single system call in a thread's life
  // another thing we do is check if any pending signals exist, then
  // running as many of them as possible before entering a wait call
  struct NtOverlapped overlap = {.hEvent = CreateEvent(0, 1, 0, 0),
                                 .Pointer = offset};
  struct ReadwriteResources rwc = {handle, &overlap};
  pthread_cleanup_push(UnwindReadwrite, &rwc);
  ok = ReadOrWriteFile(handle, data, size, 0, &overlap);
  if (!ok && GetLastError() == kNtErrorIoPending) {
  BlockingOperation:
    pt = _pthread_self();
    pt->pt_iohandle = handle;
    pt->pt_ioverlap = &overlap;
    pt->pt_flags |= PT_RESTARTABLE;
    atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_IO, memory_order_release);
    m = __sig_beginwait(waitmask);
    if (f->flags & O_NONBLOCK) {
      CancelIoEx(handle, &overlap);
      eagained = true;
    } else if (_check_cancel()) {
      CancelIoEx(handle, &overlap);
      canceled = true;
    } else if (_check_signal(true)) {
      CancelIoEx(handle, &overlap);
      eintered = true;
    } else {
      WaitForSingleObject(overlap.hEvent, -1u);
    }
    __sig_finishwait(m);
    atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_CPU,
                          memory_order_release);
    pt->pt_flags &= ~PT_RESTARTABLE;
    pt->pt_ioverlap = 0;
    pt->pt_iohandle = 0;
    ok = true;
  }
  if (ok) {
    bool32 should_wait = canceled || eagained;
    ok = GetOverlappedResult(handle, &overlap, &exchanged, should_wait);
    if (!ok && GetLastError() == kNtErrorIoIncomplete) {
      goto BlockingOperation;
    }
  }
  CloseHandle(overlap.hEvent);
  pthread_cleanup_pop(false);

  // if we acknowledged a pending masked mode cancelation request then
  // we must pass it to the caller immediately now that cleanup's done
  if (canceled) {
    return ecanceled();
  }

  // sudden success trumps interrupts and/or failed i/o abort attempts
  // plenty of code above might clobber errno, so we always restore it
  if (ok) {
    if (!pwriting && seekable) {
      f->pointer = offset + exchanged;
    }
    errno = olderror;
    return exchanged;
  }

  // if we backed out of the i/o operation intentionally ignore errors
  if (eagained) {
    return eagain();
  }

  // if another thread canceled our win32 i/o operation then we should
  // check and see if it was pthread_cancel() which committed the deed
  // in which case _check_cancel() can acknowledge the cancelation now
  // it's also fine to do nothing here; punt to next cancelation point
  if (GetLastError() == kNtErrorOperationAborted) {
    if (_check_cancel() == -1) return ecanceled();
    if (!eintered && _check_signal(false)) return eintr();
  }

  // if we chose to process a pending signal earlier then we preserve
  // that original error explicitly here even though aborted == eintr
  if (eintered) {
    return eintr();
  }

  // read() and write() have generally different error-handling paths
  return -2;
}

#endif /* __x86_64__ */
