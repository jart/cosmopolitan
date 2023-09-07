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
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/files.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

__msabi extern typeof(GetFileAttributes) *const __imp_GetFileAttributesW;

static textwindows int64_t sys_open_nt_impl(int dirfd, const char *path,
                                            uint32_t flags, int32_t mode,
                                            uint32_t extra_attr) {

  // join(topath(dirfd), path) and translate from utf-8 to utf-16
  char16_t path16[PATH_MAX];
  if (__mkntpathat(dirfd, path, flags, path16) == -1) {
    return kNtInvalidHandleValue;
  }

  // strip trailing slash
  size_t n = strlen16(path16);
  if (n > 1 && path16[n - 1] == '\\') {
    // path denormalization only goes so far. when a trailing / or /.
    // exists the kernel interprets that as having O_DIRECTORY intent
    // furthermore, windows will throw an error on unc paths with it!
    flags |= O_DIRECTORY;
    path16[--n] = 0;
  }

  // implement no follow flag
  // you can't open symlinks; use readlink
  // this flag only applies to the final path component
  // if O_NOFOLLOW_ANY is passed (-1 on NT) it'll be rejected later
  uint32_t fattr = __imp_GetFileAttributesW(path16);
  if (flags & O_NOFOLLOW) {
    if (fattr != -1u && (fattr & kNtFileAttributeReparsePoint)) {
      return eloop();
    }
    flags &= ~O_NOFOLLOW;  // don't actually pass this to win32
  }

  // handle some obvious cases while we have the attributes
  // we should ideally resolve symlinks ourself before doing this
  if (fattr != -1u) {
    if (fattr & kNtFileAttributeDirectory) {
      if ((flags & O_ACCMODE) != O_RDONLY || (flags & O_CREAT)) {
        // tried to open directory for writing. note that our
        // undocumented O_TMPFILE support on windows requires that a
        // filename be passed, rather than a directory like linux.
        return eisdir();
      }
      // on posix, the o_directory flag is an advisory safeguard that
      // isn't required. on windows, it's mandatory for opening a dir
      flags |= O_DIRECTORY;
    } else if (!(fattr & kNtFileAttributeReparsePoint)) {
      // we know for certain file isn't a directory
      if (flags & O_DIRECTORY) {
        return enotdir();
      }
    }
  }

  // translate posix flags to win32 flags
  uint32_t perm, share, disp, attr;
  if (GetNtOpenFlags(flags, mode, &perm, &share, &disp, &attr) == -1) {
    return kNtInvalidHandleValue;
  }

  if (fattr != -1u) {
    // "We have been asked to create a read-only file. "If the file
    //  already exists, the semantics of the Unix open system call is to
    //  preserve the existing permissions. If we pass CREATE_ALWAYS and
    //  FILE_ATTRIBUTE_READONLY to CreateFile, and the file already
    //  exists, CreateFile will change the file permissions. Avoid that to
    //  preserve the Unix semantics." -Quoth GoLang syscall_windows.go
    attr &= ~kNtFileAttributeReadonly;
  }

  // kNtTruncateExisting always returns kNtErrorInvalidParameter :'(
  if (disp == kNtTruncateExisting) {
    if (fattr != -1u) {
      disp = kNtCreateAlways;  // file exists (wish it could be more atomic)
    } else {
      return __fix_enotdir(enotdir(), path16);
    }
  }

  // We optimistically request some write permissions in O_RDONLY mode.
  // But that might prevent opening some files. So reactively back off.
  int extra_perm = 0;
  if ((flags & O_ACCMODE) == O_RDONLY) {
    extra_perm = kNtFileWriteAttributes | kNtFileWriteEa;
  }

  // open the file, following symlinks
  int e = errno;
  int64_t hand = CreateFile(path16, perm | extra_perm, share, &kNtIsInheritable,
                            disp, attr | extra_attr, 0);
  if (hand == -1 && errno == EACCES && (flags & O_ACCMODE) == O_RDONLY) {
    errno = e;
    hand = CreateFile(path16, perm, share, &kNtIsInheritable, disp,
                      attr | extra_attr, 0);
  }

  return __fix_enotdir(hand, path16);
}

static textwindows int sys_open_nt_console(int dirfd,
                                           const struct NtMagicPaths *mp,
                                           uint32_t flags, int32_t mode,
                                           size_t fd) {
  uint32_t cm;
  int input, output;
  if ((__isfdopen((input = STDIN_FILENO)) &&
       GetConsoleMode(g_fds.p[input].handle, &cm)) &&
      ((__isfdopen((output = STDOUT_FILENO)) &&
        GetConsoleMode(g_fds.p[output].handle, &cm)) ||
       (__isfdopen((output = STDERR_FILENO)) &&
        GetConsoleMode(g_fds.p[output].handle, &cm)))) {
    // this is an ugly hack that works for observed usage patterns
    g_fds.p[fd].handle = g_fds.p[input].handle;
    g_fds.p[fd].extra = g_fds.p[output].handle;
    g_fds.p[fd].dontclose = true;
    g_fds.p[input].dontclose = true;
    g_fds.p[output].dontclose = true;
  } else if ((g_fds.p[fd].handle = sys_open_nt_impl(
                  dirfd, mp->conin, (flags & ~O_ACCMODE) | O_RDONLY, mode,
                  kNtFileFlagOverlapped)) != -1) {
    g_fds.p[fd].extra = sys_open_nt_impl(
        dirfd, mp->conout, (flags & ~O_ACCMODE) | O_WRONLY, mode, 0);
    npassert(g_fds.p[fd].extra != -1);
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
  if ((g_fds.p[fd].handle = sys_open_nt_impl(dirfd, file, flags, mode, 0)) !=
      -1) {
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
    if (!strcmp(file, kNtMagicPaths.devtty)) {
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
