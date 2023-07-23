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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/wincrash.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

static textwindows ssize_t sys_read_nt_impl(struct Fd *fd, void *data,
                                            size_t size, int64_t offset) {

  // try to poll rather than block
  uint32_t avail;
  if (GetFileType(fd->handle) == kNtFileTypePipe) {
    for (;;) {
      if (!PeekNamedPipe(fd->handle, 0, 0, 0, &avail, 0)) break;
      if (avail) break;
      POLLTRACE("sys_read_nt polling");
      if (SleepEx(__SIG_POLLING_INTERVAL_MS, true) == kNtWaitIoCompletion) {
        POLLTRACE("IOCP EINTR");
      }
      if (fd->flags & O_NONBLOCK) {
        return eagain();
      }
      if (_check_interrupts(true, g_fds.p)) {
        POLLTRACE("sys_read_nt interrupted");
        return -1;
      }
    }
    POLLTRACE("sys_read_nt ready to read");
  }

  // perform the read i/o operation
  bool32 ok;
  uint32_t got;
  size = MIN(size, 0x7ffff000);
  if (offset == -1) {
    // perform simple blocking read
    ok = ReadFile(fd->handle, data, size, &got, 0);
  } else {
    // perform pread()-style read at particular file offset
    int64_t position;
    // save file pointer which windows clobbers, even for overlapped i/o
    if (!SetFilePointerEx(fd->handle, 0, &position, SEEK_CUR)) {
      return __winerr();  // fd probably isn't seekable?
    }
    struct NtOverlapped overlap = {0};
    overlap.Pointer = (void *)(uintptr_t)offset;
    ok = ReadFile(fd->handle, data, size, 0, &overlap);
    if (!ok && GetLastError() == kNtErrorIoPending) ok = true;
    if (ok) ok = GetOverlappedResult(fd->handle, &overlap, &got, true);
    // restore file pointer which windows clobbers, even on error
    _unassert(SetFilePointerEx(fd->handle, position, 0, SEEK_SET));
  }
  if (ok) {
    return got;
  }

  switch (GetLastError()) {
    case kNtErrorBrokenPipe:    // broken pipe
    case kNtErrorNoData:        // closing named pipe
    case kNtErrorHandleEof:     // pread read past EOF
      return 0;                 //
    case kNtErrorAccessDenied:  // read doesn't return EACCESS
      return ebadf();           //
    default:
      return __winerr();
  }
}

textwindows ssize_t sys_read_nt(struct Fd *fd, const struct iovec *iov,
                                size_t iovlen, int64_t opt_offset) {
  ssize_t rc;
  uint32_t size;
  size_t i, total;
  if (opt_offset < -1) return einval();
  if (_check_interrupts(true, fd)) return -1;
  while (iovlen && !iov[0].iov_len) iov++, iovlen--;
  if (iovlen) {
    for (total = i = 0; i < iovlen; ++i) {
      if (!iov[i].iov_len) continue;
      rc = sys_read_nt_impl(fd, iov[i].iov_base, iov[i].iov_len, opt_offset);
      if (rc == -1) return -1;
      total += rc;
      if (opt_offset != -1) opt_offset += rc;
      if (rc < iov[i].iov_len) break;
    }
    return total;
  } else {
    return sys_read_nt_impl(fd, NULL, 0, opt_offset);
  }
}
