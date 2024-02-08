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
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/stat.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/errors.h"
#include "libc/nt/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/errfuns.h"

static int Atoi(const char *str) {
  int c;
  unsigned x = 0;
  if (!*str) return -1;
  while ((c = *str++)) {
    if ('0' <= c && c <= '9') {
      x *= 10;
      x += c - '0';
    } else {
      return -1;
    }
  }
  return x;
}

textwindows int sys_fstatat_nt(int dirfd, const char *path, struct stat *st,
                               int flags) {

  // handle special files
  if (startswith(path, "/dev/")) {
    int fd;
    if (!strcmp(path + 5, "tty")) {
      return sys_fstat_nt_special(kFdConsole, st);
    } else if (!strcmp(path + 5, "null")) {
      return sys_fstat_nt_special(kFdDevNull, st);
    } else if (!strcmp(path + 5, "stdin")) {
      return sys_fstat_nt(STDIN_FILENO, st);
    } else if (!strcmp(path + 5, "stdout")) {
      return sys_fstat_nt(STDOUT_FILENO, st);
    } else if (!strcmp(path + 5, "stderr")) {
      return sys_fstat_nt(STDERR_FILENO, st);
    } else if (startswith(path + 5, "fd/") && (fd = Atoi(path + 8)) != -1) {
      return sys_fstat_nt(fd, st);
    } else {
      return enoent();
    }
  }

  // convert path from utf-8 to utf-16
  uint16_t path16[PATH_MAX];
  if (__mkntpathat(dirfd, path, 0, path16) == -1) {
    return -1;
  }

  // open an actual file
  int rc;
  int64_t fh;
  int e = errno;
  uint32_t dwDesiredAccess = kNtFileGenericRead;
  BLOCK_SIGNALS;
TryAgain:
  if ((fh = CreateFile(
           path16, dwDesiredAccess,
           kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete, 0,
           kNtOpenExisting,
           kNtFileAttributeNormal | kNtFileFlagBackupSemantics |
               ((flags & AT_SYMLINK_NOFOLLOW) ? kNtFileFlagOpenReparsePoint
                                              : 0),
           0)) != -1) {
    rc = st ? sys_fstat_nt_handle(fh, path16, st) : 0;
    CloseHandle(fh);
  } else if (dwDesiredAccess == kNtFileGenericRead &&
             (GetLastError() == kNtErrorAccessDenied ||
              GetLastError() == kNtErrorSharingViolation)) {
    dwDesiredAccess = kNtFileReadAttributes;
    errno = e;
    goto TryAgain;
  } else {
    rc = __winerr();
  }
  ALLOW_SIGNALS;

  // mop up errors
  return __fix_enotdir(rc, path16);
}
