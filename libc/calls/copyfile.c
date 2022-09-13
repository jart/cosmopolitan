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
#include "libc/calls/copyfile.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/filetime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/o.h"
#include "libc/time/time.h"

static textwindows int sys_copyfile_nt(const char *src, const char *dst,
                                       int flags) {
  int64_t fhsrc, fhdst;
  struct NtFileTime accessed, modified;
  char16_t src16[PATH_MAX], dst16[PATH_MAX];
  if (__mkntpath(src, src16) == -1) return -1;
  if (__mkntpath(dst, dst16) == -1) return -1;
  if (CopyFile(src16, dst16, !!(flags & COPYFILE_NOCLOBBER))) {
    if (flags & COPYFILE_PRESERVE_TIMESTAMPS) {
      fhsrc = CreateFile(src16, kNtFileReadAttributes, kNtFileShareRead, NULL,
                         kNtOpenExisting, kNtFileAttributeNormal, 0);
      fhdst = CreateFile(dst16, kNtFileWriteAttributes, kNtFileShareRead, NULL,
                         kNtOpenExisting, kNtFileAttributeNormal, 0);
      if (fhsrc != -1 && fhdst != -1) {
        GetFileTime(fhsrc, NULL, &accessed, &modified);
        SetFileTime(fhdst, NULL, &accessed, &modified);
      }
      CloseHandle(fhsrc);
      CloseHandle(fhdst);
    }
    return 0;
  } else {
    return __winerr();
  }
}

static int sys_copyfile(const char *src, const char *dst, int flags) {
  struct stat st;
  size_t remaining;
  ssize_t transferred;
  struct timespec amtime[2];
  int64_t inoffset, outoffset;
  int rc, srcfd, dstfd, oflags, omode;
  rc = -1;
  if ((srcfd = openat(AT_FDCWD, src, O_RDONLY, 0)) != -1) {
    if (fstat(srcfd, &st) != -1) {
      omode = st.st_mode & 0777;
      oflags = O_WRONLY | O_CREAT;
      if (flags & COPYFILE_NOCLOBBER) oflags |= O_EXCL;
      if ((dstfd = openat(AT_FDCWD, dst, oflags, omode)) != -1) {
        remaining = st.st_size;
        ftruncate(dstfd, remaining);
        inoffset = 0;
        outoffset = 0;
        while (remaining &&
               (transferred = copy_file_range(
                    srcfd, &inoffset, dstfd, &outoffset, remaining, 0)) != -1) {
          remaining -= transferred;
        }
        if (!remaining) {
          rc = 0;
          if (flags & COPYFILE_PRESERVE_TIMESTAMPS) {
            amtime[0] = st.st_atim;
            amtime[1] = st.st_mtim;
            utimensat(dstfd, NULL, amtime, 0);
          }
        }
        rc |= close(dstfd);
      }
    }
    rc |= close(srcfd);
  }
  return rc;
}

/**
 * Copies file.
 *
 * This implementation goes 2x faster than the `cp` command that comes
 * included with most systems since we use the newer copy_file_range()
 * system call rather than sendfile().
 *
 * @param flags may have COPYFILE_PRESERVE_TIMESTAMPS, COPYFILE_NOCLOBBER
 * @return 0 on success, or -1 w/ errno
 */
int _copyfile(const char *src, const char *dst, int flags) {
  if (!IsWindows() || _startswith(src, "/zip/") || _startswith(dst, "/zip/")) {
    return sys_copyfile(src, dst, flags);
  } else {
    return sys_copyfile_nt(src, dst, flags);
  }
}
