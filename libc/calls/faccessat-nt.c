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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/limits.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/pib.h"

static int Atoi(const char *str) {
  int c;
  unsigned x = 0;
  if (!*str)
    return -1;
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

static int sys_faccessat_nt_dev(int amode, bool readable, bool writable) {
  if (amode == F_OK)
    return 0;
  if (amode == X_OK)
    return eacces();
  if (amode == R_OK)
    if (!readable)
      return eacces();
  if (amode == W_OK)
    if (!writable)
      return eacces();
  return 0;
}

textwindows static int sys_faccessat_nt_impl(const char16_t *path16, int amode,
                                             int flags) {

  // create access mask
  uint32_t dwDesiredAccess;
  if (amode == F_OK) {
    dwDesiredAccess = kNtFileReadAttributes;
  } else {
    // W or X implies R
    dwDesiredAccess = kNtGenericRead;
    if (amode & W_OK)
      dwDesiredAccess |= kNtGenericWrite;
  }

  // open file
  intptr_t hFile;
  if ((hFile = CreateFile(
           path16, dwDesiredAccess,
           kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete, 0,
           kNtOpenExisting,
           kNtFileAttributeNormal | kNtFileFlagBackupSemantics |
               ((flags & AT_SYMLINK_NOFOLLOW) ? kNtFileFlagOpenReparsePoint
                                              : 0),
           0)) == -1)
    return __winerr();

  // check for write and execute access
  int rc = 0;
  if (amode & (W_OK | X_OK)) {
    struct NtByHandleFileInformation wst;
    if (!GetFileInformationByHandle(hFile, &wst))
      rc = __winerr();
    // handle read-only files
    if (!rc)
      if (amode & W_OK)
        if (wst.dwFileAttributes & kNtFileAttributeReadonly)
          rc = eacces();
    // executables, directories, and symlinks are always executable
    if (!rc)
      if (amode & X_OK)
        if (!(wst.dwFileAttributes & kNtFileAttributeReparsePoint) &&
            !(wst.dwFileAttributes & kNtFileAttributeDirectory) &&
            !IsWindowsExecutable(hFile, path16))
          rc = eacces();
  }

  // close file
  CloseHandle(hFile);

  // return result
  return rc;
}

textwindows int sys_faccessat_nt(int dirfd, const char *path, int amode,
                                 int flags) {

  // validate arguments
  if (amode & ~(R_OK | W_OK | X_OK))
    return einval();
  if (flags & ~(AT_EACCESS | AT_SYMLINK_NOFOLLOW))
    return einval();

  // handle special files
  if (startswith(path, "/dev/")) {
    int fd;
    if (!strcmp(path + 5, "tty")) {
      return sys_faccessat_nt_dev(amode, true, true);
    } else if (!strcmp(path + 5, "null")) {
      return sys_faccessat_nt_dev(amode, true, true);
    } else if (!strcmp(path + 5, "random") || !strcmp(path + 5, "urandom")) {
      return sys_faccessat_nt_dev(amode, true, false);
    } else if (!strcmp(path + 5, "stdin")) {
      return sys_faccessat_nt_dev(amode, true, false);
    } else if (!strcmp(path + 5, "stdout")) {
      return sys_faccessat_nt_dev(amode, false, true);
    } else if (!strcmp(path + 5, "stderr")) {
      return sys_faccessat_nt_dev(amode, false, true);
    } else if (startswith(path + 5, "fd/") && (fd = Atoi(path + 8)) != -1) {
      return sys_faccessat_nt_dev(
          amode, (__get_pib()->fds.p[fd].flags & O_ACCMODE) != O_WRONLY,
          (__get_pib()->fds.p[fd].flags & O_ACCMODE) != O_RDONLY);
    }
  }

  char16_t path16[PATH_MAX];
  if (__mkntpathat(dirfd, path, path16) == -1)
    return -1;

  int rc;
  BLOCK_SIGNALS;
  rc = __fix_enotdir(sys_faccessat_nt_impl(path16, amode, flags), path16);
  ALLOW_SIGNALS;
  return rc;
}
