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
#include "libc/calls/metalfile.internal.h"
#include "libc/calls/struct/metastat.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/nt/enum/fileinfobyhandleclass.h"
#include "libc/nt/files.h"
#include "libc/nt/struct/filestandardinformation.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Determines size of open file.
 *
 * This function is equivalent to:
 *
 *     struct stat st;
 *     !fstat(fd, &st) ? st.st_size : -1
 *
 * Except faster on BSD/Windows and a much smaller link size.
 *
 * @return file byte length, or -1 w/ errno
 * @asyncsignalsafe
 */
ssize_t getfiledescriptorsize(int fd) {
  int e;
  ssize_t rc;
  union metastat st;
  struct NtFileStandardInformation info;
  e = errno;
  if (__isfdkind(fd, kFdZip)) {
    if (_weaken(__zipos_fstat)(
            (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle, &st.cosmo) !=
        -1) {
      rc = st.cosmo.st_size;
    } else {
      rc = -1;
    }
  } else if (IsMetal()) {
    if (fd < 0 || fd >= g_fds.n) {
      rc = ebadf();
    } else if (g_fds.p[fd].kind != kFdFile) {
      rc = eacces();
    } else {
      struct MetalFile *state = (struct MetalFile *)g_fds.p[fd].handle;
      rc = state->size;
    }
  } else if (!IsWindows()) {
    if (!__sys_fstat(fd, &st)) {
      rc = METASTAT(st, st_size);
    } else {
      rc = -1;
    }
  } else if (__isfdopen(fd)) {
    if (GetFileInformationByHandleEx(g_fds.p[fd].handle, kNtFileStandardInfo,
                                     &info, sizeof(info))) {
      rc = info.EndOfFile;
    } else {
      rc = ebadf();
    }
  } else {
    rc = ebadf();
  }
  STRACE("getfiledescriptorsize(%d) → %'zd% m", fd, rc);
  return rc;
}
