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
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/files.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"

static textwindows int sys_open_nt_impl(int dirfd, const char *path,
                                        uint32_t flags, int32_t mode) {
  char16_t path16[PATH_MAX];
  uint32_t perm, share, disp, attr;
  if (__mkntpathat(dirfd, path, flags, path16) == -1) return -1;
  if (GetNtOpenFlags(flags, mode, &perm, &share, &disp, &attr) == -1) return -1;
  return __fix_enotdir(
      CreateFile(path16, perm, share, &kNtIsInheritable, disp, attr, 0),
      path16);
}

static textwindows int sys_open_nt_console(int dirfd,
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
    _npassert(g_fds.p[fd].extra != -1);
  } else {
    return -1;
  }
  g_fds.p[fd].kind = kFdConsole;
  g_fds.p[fd].flags = flags;
  g_fds.p[fd].mode = mode;
  return fd;
}

static textwindows int sys_open_nt_file(int dirfd, const char *file,
                                        uint32_t flags, int32_t mode,
                                        size_t fd) {
  if ((g_fds.p[fd].handle = sys_open_nt_impl(dirfd, file, flags, mode)) != -1) {
    g_fds.p[fd].kind = kFdFile;
    g_fds.p[fd].flags = flags;
    g_fds.p[fd].mode = mode;
    return fd;
  } else {
    return -1;
  }
}

textwindows int sys_open_nt(int dirfd, const char *file, uint32_t flags,
                            int32_t mode) {
  int fd;
  ssize_t rc;
  __fds_lock();
  if ((rc = fd = __reservefd_unlocked(-1)) != -1) {
    if ((flags & O_ACCMODE) == O_RDWR && !strcmp(file, kNtMagicPaths.devtty)) {
      rc = sys_open_nt_console(dirfd, &kNtMagicPaths, flags, mode, fd);
    } else {
      rc = sys_open_nt_file(dirfd, file, flags, mode, fd);
    }
    if (rc == -1) {
      __releasefd(fd);
    }
    __fds_unlock();
  }
  return rc;
}
