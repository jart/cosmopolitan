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
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/errno.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/consts/posix.h"
#include "libc/sysv/errno.h"
#include "libc/sysv/pib.h"

textwindows static errno_t sys_posix_fadvise_nt_impl(int fd, uint64_t offset,
                                                     uint64_t len, int advice) {
  int64_t h1, h2;
  int flags, mode;
  uint32_t perm, share, attr, extra_attr = 0;

  if ((int64_t)len < 0)
    return EINVAL;
  if (!__isfdkind(fd, kFdFile))
    return EBADF;
  h1 = __get_pib()->fds.p[fd].handle;
  mode = __get_pib()->fds.p[fd].mode;
  flags = __get_pib()->fds.p[fd].flags;

  switch (advice) {
    case POSIX_FADV_NORMAL:
      break;
    case POSIX_FADV_RANDOM:
      extra_attr = kNtFileFlagRandomAccess;
      break;
    case POSIX_FADV_SEQUENTIAL:
      extra_attr = kNtFileFlagSequentialScan;
      break;
    case POSIX_FADV_NOREUSE:
    case POSIX_FADV_DONTNEED:
      return 0;
    default:
      return EINVAL;
  }

  if (GetFileType(h1) == kNtFileTypePipe)
    return ESPIPE;

  int e = errno;
  if (GetNtOpenFlags(flags, mode, &perm, &share, 0, &attr) == -1) {
    errno_t err = errno;
    errno = e;
    return err;  // TODO: Why does this fail with coreutils cp -R?
  }
  attr |= extra_attr;

  // MSDN says only these are allowed, otherwise it returns EINVAL.
  attr &= kNtFileFlagBackupSemantics | kNtFileFlagDeleteOnClose |
          kNtFileFlagNoBuffering | kNtFileFlagOpenNoRecall |
          kNtFileFlagOpenReparsePoint | kNtFileFlagOverlapped |
          kNtFileFlagPosixSemantics | kNtFileFlagRandomAccess |
          kNtFileFlagSequentialScan | kNtFileFlagWriteThrough;

  if ((h2 = ReOpenFile(h1, perm, share, attr | kNtFileFlagOverlapped)) == -1)
    return __errno_windows2linux(GetLastError());

  if (h2 != h1)
    CloseHandle(h1);

  __get_pib()->fds.p[fd].handle = h2;
  __get_pib()->fds.p[fd].flags = flags;
  return 0;
}

textwindows errno_t sys_posix_fadvise_nt(int fd, uint64_t offset, uint64_t len,
                                         int advice) {
  errno_t err;
  BLOCK_SIGNALS;
  err = sys_posix_fadvise_nt_impl(fd, offset, len, advice);
  ALLOW_SIGNALS;
  return err;
}
