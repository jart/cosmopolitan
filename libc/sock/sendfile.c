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
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

static textwindows ssize_t sendfile$linux2nt(int outfd, int infd,
                                             int64_t *inout_opt_inoffset,
                                             size_t uptobytes) {
  struct NtOverlapped Overlapped;
  struct NtOverlapped *lpOverlapped;
  if (!__isfdkind(outfd, kFdSocket) || !__isfdkind(outfd, kFdFile))
    return ebadf();
  if (inout_opt_inoffset) {
    memset(&Overlapped, 0, sizeof(Overlapped));
    Overlapped.Pointer = (void *)(intptr_t)(*inout_opt_inoffset);
    lpOverlapped = &Overlapped;
  } else {
    lpOverlapped = NULL;
  }
  /* TODO(jart): Fetch this on a per-socket basis via GUID. */
  if (TransmitFile(g_fds.p[outfd].handle, g_fds.p[infd].handle, uptobytes, 0,
                   lpOverlapped, NULL, 0)) {
    return uptobytes;
  } else {
    return __winsockerr();
  }
}

static ssize_t sendfile$linux2netflix(int outfd, int infd,
                                      int64_t *inout_opt_inoffset,
                                      size_t uptobytes) {
  int sendfile$netflix(int32_t infd, int32_t outfd, int64_t offset,
                       size_t nbytes, const void *opt_hdtr,
                       int64_t *out_opt_sbytes,
                       int32_t flags) asm("sendfile$sysv") hidden;
  int rc;
  int64_t offset, sbytes;
  if (inout_opt_inoffset) {
    offset = *inout_opt_inoffset;
  } else if ((offset = lseek$sysv(infd, 0, SEEK_CUR)) == -1) {
    return -1;
  }
  if ((rc = sendfile$netflix(infd, outfd, offset, uptobytes, NULL, &sbytes,
                             0)) != -1) {
    if (inout_opt_inoffset) *inout_opt_inoffset += sbytes;
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
  if (!uptobytes) return einval();
  if (uptobytes > 0x7ffffffe /* Microsoft's off-by-one */) return eoverflow();
  if (IsModeDbg() && uptobytes > 1) uptobytes >>= 1;
  if (IsLinux()) {
    return sendfile$sysv(outfd, infd, inout_opt_inoffset, uptobytes);
  } else if (IsFreebsd() || IsXnu()) {
    return sendfile$linux2netflix(outfd, infd, inout_opt_inoffset, uptobytes);
  } else if (IsWindows()) {
    return sendfile$linux2nt(outfd, infd, inout_opt_inoffset, uptobytes);
  } else {
    return copyfd(infd, inout_opt_inoffset, outfd, NULL, uptobytes, 0);
  }
}
