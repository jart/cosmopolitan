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
#include "libc/calls/strace.internal.h"
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
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

#define _O_APPEND     0x00000400 /* kNtFileAppendData */
#define _O_CREAT      0x00000040 /* kNtOpenAlways */
#define _O_EXCL       0x00000080 /* kNtCreateNew */
#define _O_TRUNC      0x00000200 /* kNtCreateAlways */
#define _O_DIRECTORY  0x00010000 /* kNtFileFlagBackupSemantics */
#define _O_TMPFILE    0x00410000 /* AttributeTemporary|FlagDeleteOnClose */
#define _O_DIRECT     0x00004000 /* kNtFileFlagNoBuffering */
#define _O_NDELAY     0x00000800 /* kNtFileFlagWriteThrough */
#define _O_RANDOM     0x80000000 /* kNtFileFlagRandomAccess */
#define _O_SEQUENTIAL 0x40000000 /* kNtFileFlagSequentialScan */
#define _O_COMPRESSED 0x20000000 /* kNtFileAttributeCompressed */
#define _O_INDEXED    0x10000000 /* !kNtFileAttributeNotContentIndexed */
#define _O_NONBLOCK   0x00000800
#define _O_CLOEXEC    0x00080000

static textwindows int64_t sys_open_nt_impl(int dirfd, const char *path,
                                            uint32_t flags, int32_t mode) {
  int64_t handle;
  uint32_t br, err;
  char16_t path16[PATH_MAX];
  uint32_t perm, share, disp, attr;
  if (__mkntpathat(dirfd, path, flags, path16) == -1) return -1;

  switch (flags & O_ACCMODE) {
    case O_RDONLY:
      perm = kNtFileGenericRead | kNtGenericExecute;
      break;
    case O_WRONLY:
      perm = kNtFileGenericWrite | kNtGenericExecute;
      break;
    case O_RDWR:
      perm = kNtFileGenericRead | kNtFileGenericWrite | kNtGenericExecute;
      break;
    default:
      unreachable;
  }

  if (flags & _O_EXCL) {
    share = kNtFileShareExclusive;
  } else {
    share = kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete;
  }

  if ((flags & _O_CREAT) && (flags & _O_EXCL)) {
    disp = kNtCreateNew;
  } else if ((flags & _O_CREAT) && (flags & _O_TRUNC)) {
    disp = kNtCreateAlways;
  } else if (flags & _O_CREAT) {
    disp = kNtOpenAlways;
  } else if (flags & _O_TRUNC) {
    disp = kNtTruncateExisting;
  } else {
    disp = kNtOpenExisting;
  }

  if ((flags & _O_TMPFILE) == _O_TMPFILE) {
    attr = kNtFileAttributeTemporary | kNtFileFlagDeleteOnClose;
  } else {
    attr = kNtFileAttributeNormal;
    if (flags & _O_DIRECTORY) attr |= kNtFileFlagBackupSemantics;
    if (~mode & 0200) {
      attr |= kNtFileAttributeReadonly;
      if (!IsTiny() && disp == kNtCreateAlways) {
        // iron out esoteric unix/win32 inconsistency (golang #38225)
        if ((handle = CreateFile(path16, perm, share, &kNtIsInheritable,
                                 kNtTruncateExisting, kNtFileAttributeNormal,
                                 0)) != -1 ||
            (errno != ENOENT && errno != ENOTDIR)) {
          return handle;
        }
      }
    }
  }
  flags |= kNtFileFlagOverlapped;
  if (~flags & _O_INDEXED) attr |= kNtFileAttributeNotContentIndexed;
  if (flags & _O_COMPRESSED) attr |= kNtFileAttributeCompressed;
  if (flags & _O_SEQUENTIAL) attr |= kNtFileFlagSequentialScan;
  if (flags & _O_RANDOM) attr |= kNtFileFlagRandomAccess;
  if (flags & _O_DIRECT) attr |= kNtFileFlagNoBuffering;
  if (flags & _O_NDELAY) attr |= kNtFileFlagWriteThrough;

  return CreateFile(path16, perm, share, &kNtIsInheritable, disp, attr, 0);
}

static textwindows ssize_t sys_open_nt_console(int dirfd,
                                               const struct NtMagicPaths *mp,
                                               uint32_t flags, int32_t mode,
                                               size_t fd) {
  if (GetFileType(g_fds.p[STDIN_FILENO].handle) == kNtFileTypeChar &&
      GetFileType(g_fds.p[STDOUT_FILENO].handle) == kNtFileTypeChar) {
    g_fds.p[fd].handle = g_fds.p[STDIN_FILENO].handle;
    g_fds.p[fd].extra = g_fds.p[STDOUT_FILENO].handle;
  } else if ((g_fds.p[fd].handle = sys_open_nt_impl(
                  dirfd, mp->conin, (flags & ~O_ACCMODE) | O_RDONLY, mode)) !=
             -1) {
    g_fds.p[fd].extra = sys_open_nt_impl(dirfd, mp->conout,
                                         (flags & ~O_ACCMODE) | O_WRONLY, mode);
    assert(g_fds.p[fd].extra != -1);
  } else {
    return -1;
  }
  g_fds.p[fd].kind = kFdConsole;
  g_fds.p[fd].flags = flags;
  return fd;
}

static textwindows ssize_t sys_open_nt_file(int dirfd, const char *file,
                                            uint32_t flags, int32_t mode,
                                            size_t fd) {
  if ((g_fds.p[fd].handle = sys_open_nt_impl(dirfd, file, flags, mode)) != -1) {
    g_fds.p[fd].kind = kFdFile;
    g_fds.p[fd].flags = flags;
    return fd;
  } else {
    return -1;
  }
}

textwindows ssize_t sys_open_nt(int dirfd, const char *file, uint32_t flags,
                                int32_t mode) {
  int fd;
  ssize_t rc;
  if ((fd = __reservefd()) == -1) return -1;
  if ((flags & O_ACCMODE) == O_RDWR && !strcmp(file, kNtMagicPaths.devtty)) {
    rc = sys_open_nt_console(dirfd, &kNtMagicPaths, flags, mode, fd);
  } else {
    rc = sys_open_nt_file(dirfd, file, flags, mode, fd);
  }
  if (rc == -1) {
    __releasefd(fd);
  }
  return rc;
}
