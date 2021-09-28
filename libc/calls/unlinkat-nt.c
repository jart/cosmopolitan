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
#include "libc/calls/internal.h"
#include "libc/errno.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/io.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/win32fileattributedata.h"
#include "libc/nt/struct/win32finddata.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/at.h"

static textwindows bool IsDirectorySymlink(const char16_t *path) {
  int64_t h;
  struct NtWin32FindData data;
  struct NtWin32FileAttributeData info;
  if (GetFileAttributesEx(path, 0, &info) &&
      ((info.dwFileAttributes & kNtFileAttributeDirectory) &&
       (info.dwFileAttributes & kNtFileAttributeReparsePoint)) &&
      (h = FindFirstFile(path, &data)) != -1) {
    FindClose(h);
    return data.dwReserved0 == kNtIoReparseTagSymlink ||
           data.dwReserved0 == kNtIoReparseTagMountPoint;
  } else {
    return false;
  }
}

static textwindows int sys_rmdir_nt(const char16_t *path) {
  int e, ms;
  for (ms = 1;; ms *= 2) {
    if (RemoveDirectory(path)) return 0;
    /*
     * Files can linger, for absolutely no reason.
     * Possibly some Windows Defender bug on Win7.
     * Sleep for up to one second w/ expo backoff.
     * Alternative is use Microsoft internal APIs.
     * Never could have imagined it'd be this bad.
     */
    if ((e = GetLastError()) == kNtErrorDirNotEmpty && ms <= 512) {
      Sleep(ms);
      continue;
    } else {
      break;
    }
  }
  errno = e;
  return -1;
}

static textwindows int sys_unlink_nt(const char16_t *path) {
  if (IsDirectorySymlink(path)) return sys_rmdir_nt(path);
  return DeleteFile(path) ? 0 : __winerr();
}

textwindows int sys_unlinkat_nt(int dirfd, const char *path, int flags) {
  uint16_t path16[PATH_MAX];
  if (__mkntpathat(dirfd, path, 0, path16) == -1) return -1;
  if (flags & AT_REMOVEDIR) {
    return sys_rmdir_nt(path16);
  } else {
    return sys_unlink_nt(path16);
  }
}
