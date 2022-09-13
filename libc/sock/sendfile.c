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
#include "libc/intrin/strace.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/sendfile.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

// sendfile() isn't specified as raising eintr
static textwindows int SendfileBlock(int64_t handle,
                                     struct NtOverlapped *overlapped) {
  uint32_t i, got, flags = 0;
  if (WSAGetLastError() != kNtErrorIoPending &&
      WSAGetLastError() != WSAEINPROGRESS) {
    NTTRACE("TransmitFile failed %lm");
    return __winsockerr();
  }
  for (;;) {
    i = WSAWaitForMultipleEvents(1, &overlapped->hEvent, true,
                                 __SIG_POLLING_INTERVAL_MS, true);
    if (i == kNtWaitFailed) {
      NTTRACE("WSAWaitForMultipleEvents failed %lm");
      return __winsockerr();
    } else if (i == kNtWaitTimeout || i == kNtWaitIoCompletion) {
      _check_interrupts(true, g_fds.p);
#if _NTTRACE
      POLLTRACE("WSAWaitForMultipleEvents...");
#endif
    } else {
      break;
    }
  }
  if (!WSAGetOverlappedResult(handle, overlapped, &got, false, &flags)) {
    NTTRACE("WSAGetOverlappedResult failed %lm");
    return __winsockerr();
  }
  return got;
}

static textwindows ssize_t sendfile_linux2nt(int outfd, int infd,
                                             int64_t *inout_opt_inoffset,
                                             size_t uptobytes) {
  ssize_t rc;
  int64_t offset;
  struct NtOverlapped overlapped;
  if (!__isfdkind(outfd, kFdSocket)) return ebadf();
  if (!__isfdkind(infd, kFdFile)) return ebadf();
  if (inout_opt_inoffset) {
    offset = *inout_opt_inoffset;
  } else if (!SetFilePointerEx(g_fds.p[infd].handle, 0, &offset, SEEK_CUR)) {
    return __winerr();
  }
  bzero(&overlapped, sizeof(overlapped));
  overlapped.Pointer = (void *)(intptr_t)offset;
  overlapped.hEvent = WSACreateEvent();
  if (TransmitFile(g_fds.p[outfd].handle, g_fds.p[infd].handle, uptobytes, 0,
                   &overlapped, 0, 0)) {
    rc = uptobytes;
  } else {
    rc = SendfileBlock(g_fds.p[outfd].handle, &overlapped);
  }
  if (rc != -1 && inout_opt_inoffset) {
    *inout_opt_inoffset = offset + rc;
  }
  WSACloseEvent(overlapped.hEvent);
  return rc;
}

static ssize_t sendfile_linux2bsd(int outfd, int infd,
                                  int64_t *inout_opt_inoffset,
                                  size_t uptobytes) {
  int rc;
  int64_t offset, sbytes;
  if (inout_opt_inoffset) {
    offset = *inout_opt_inoffset;
  } else if ((offset = lseek(infd, 0, SEEK_CUR)) == -1) {
    return -1;
  }
  if (IsFreebsd()) {
    rc = sys_sendfile_freebsd(infd, outfd, offset, uptobytes, 0, &sbytes, 0);
  } else {
    sbytes = uptobytes;
    rc = sys_sendfile_xnu(infd, outfd, offset, &sbytes, 0, 0);
  }
  if (rc != -1) {
    if (inout_opt_inoffset) {
      *inout_opt_inoffset += sbytes;
    }
    return sbytes;
  } else {
    return -1;
  }
}

/**
 * Transfers data from file to network.
 *
 * @param outfd needs to be a socket
 * @param infd needs to be a file
 * @param inout_opt_inoffset may be specified for pread()-like behavior
 * @param uptobytes is usually the number of bytes remaining in file; it
 *     can't exceed INT_MAX-1; some platforms block until everything's
 *     sent, whereas others won't; zero isn't allowed
 * @return number of bytes transmitted which may be fewer than requested
 * @see copy_file_range() for file ↔ file
 * @see splice() for fd ↔ pipe
 */
ssize_t sendfile(int outfd, int infd, int64_t *inout_opt_inoffset,
                 size_t uptobytes) {
  int rc;
  if (!uptobytes) {
    rc = einval();
  } else if (IsAsan() && inout_opt_inoffset &&
             !__asan_is_valid(inout_opt_inoffset,
                              sizeof(*inout_opt_inoffset))) {
    rc = efault();
  } else if (uptobytes > 0x7ffffffe /* Microsoft's off-by-one */) {
    rc = eoverflow();
  } else if (IsLinux()) {
    rc = sys_sendfile(outfd, infd, inout_opt_inoffset, uptobytes);
  } else if (IsFreebsd() || IsXnu()) {
    rc = sendfile_linux2bsd(outfd, infd, inout_opt_inoffset, uptobytes);
  } else if (IsWindows()) {
    rc = sendfile_linux2nt(outfd, infd, inout_opt_inoffset, uptobytes);
  } else {
    rc = copyfd(infd, inout_opt_inoffset, outfd, NULL, uptobytes, 0);
  }
  STRACE("sendfile(%d, %d, %p, %'zu) → %ld% m", outfd, infd, inout_opt_inoffset,
         uptobytes, rc);
  return rc;
}
