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
#include "libc/calls/internal.h"
#include "libc/calls/ntmagicpaths.internal.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/fsctl.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

static textwindows int64_t open$nt$impl(const char *file, uint32_t flags,
                                        int32_t mode) {
  uint32_t br;
  int64_t handle;
  char16_t file16[PATH_MAX];
  if (__mkntpath2(file, file16, flags) == -1) return -1;
  if ((handle = CreateFile(
           file16,
           (flags & 0xf000000f) | (/* this is needed if we mmap(rwx+cow)
                                      nt is choosy about open() access */
                                   kNtGenericExecute | kNtFileGenericWrite),
           (flags & O_EXCL)
               ? kNtFileShareExclusive
               : kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete,
           &kNtIsInheritable,
           (flags & O_CREAT) && (flags & O_EXCL)
               ? kNtCreateNew
               : (flags & O_CREAT) && (flags & O_TRUNC)
                     ? kNtCreateAlways
                     : (flags & O_CREAT)
                           ? kNtOpenAlways
                           : (flags & O_TRUNC) ? kNtTruncateExisting
                                               : kNtOpenExisting,
           /* TODO(jart): Should we just always set overlapped? */
           (/* note: content indexer demolishes unix-ey i/o performance */
            kNtFileAttributeNotContentIndexed | kNtFileAttributeNormal |
            (((flags & ((kNtFileFlagWriteThrough | kNtFileFlagOverlapped |
                         kNtFileFlagNoBuffering | kNtFileFlagRandomAccess) >>
                        8))
              << 8) |
             (flags & (kNtFileFlagSequentialScan | kNtFileFlagDeleteOnClose |
                       kNtFileFlagBackupSemantics | kNtFileFlagPosixSemantics |
                       kNtFileAttributeTemporary)))),
           0)) != -1) {
    if (flags & O_SPARSE) {
      /*
       * TODO(jart): Can all files be sparse files? That seems to be the
       *             way Linux behaves out of the box.
       * TODO(jart): Wow why does sparse wreak havoc?
       */
      DeviceIoControl(handle, kNtFsctlSetSparse, NULL, 0, NULL, 0, &br, NULL);
    }
  }
  return handle;
}

static textwindows ssize_t open$nt$console(const struct NtMagicPaths *mp,
                                           uint32_t flags, int32_t mode,
                                           size_t fd) {
  if (GetFileType(g_fds.p[STDIN_FILENO].handle) == kNtFileTypeChar &&
      GetFileType(g_fds.p[STDOUT_FILENO].handle) == kNtFileTypeChar) {
    g_fds.p[fd].handle = g_fds.p[STDIN_FILENO].handle;
    g_fds.p[fd].extra = g_fds.p[STDOUT_FILENO].handle;
  } else if ((g_fds.p[fd].handle = open$nt$impl(
                  mp->conin, (flags & ~O_ACCMODE) | O_RDONLY, mode)) != -1) {
    g_fds.p[fd].extra =
        open$nt$impl(mp->conout, (flags & ~O_ACCMODE) | O_WRONLY, mode);
    assert(g_fds.p[fd].extra != -1);
  } else {
    return __winerr();
  }
  g_fds.p[fd].kind = kFdConsole;
  g_fds.p[fd].flags = flags;
  return fd;
}

static textwindows ssize_t open$nt$file(const char *file, uint32_t flags,
                                        int32_t mode, size_t fd) {
  if ((g_fds.p[fd].handle = open$nt$impl(file, flags, mode)) != -1) {
    g_fds.p[fd].kind = kFdFile;
    g_fds.p[fd].flags = flags;
    return fd;
  } else if (GetLastError() == kNtErrorFileExists &&
             ((flags & O_CREAT) && (flags & O_TRUNC))) {
    return eisdir();
  } else {
    return __winerr();
  }
}

textwindows ssize_t open$nt(const char *file, uint32_t flags, int32_t mode) {
  size_t fd;
  if ((fd = __getemptyfd()) == -1) return -1;
  if ((flags & O_ACCMODE) == O_RDWR &&
      strcmp(file, kNtMagicPaths.devtty) == 0) {
    return open$nt$console(&kNtMagicPaths, flags, mode, fd);
  } else {
    return open$nt$file(file, flags, mode, fd);
  }
}
