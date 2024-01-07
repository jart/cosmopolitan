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
#include "libc/assert.h"
#include "libc/calls/createfileflags.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.internal.h"
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

  // implement no follow flag
  // you can't open symlinks; use readlink
  // this flag only applies to the final path component
  // if _O_NOFOLLOW_ANY is passed (-1 on NT) it'll be rejected later
  uint32_t fattr = GetFileAttributes(path16);
  if (flags & _O_NOFOLLOW) {
    if (fattr != -1u && (fattr & kNtFileAttributeReparsePoint)) {
      return eloop();
    }
    flags &= ~_O_NOFOLLOW;  // don't actually pass this to win32
  }

  // handle some obvious cases while we have the attributes
  // we should ideally resolve symlinks ourself before doing this
  if (fattr != -1u) {
    if (fattr & kNtFileAttributeDirectory) {
      if ((flags & O_ACCMODE) != O_RDONLY || (flags & _O_CREAT)) {
        // tried to open directory for writing. note that our
        // undocumented _O_TMPFILE support on windows requires that a
        // filename be passed, rather than a directory like linux.
        return eisdir();
      }
      // on posix, the o_directory flag is an advisory safeguard that
      // isn't required. on windows, it's mandatory for opening a dir
      flags |= _O_DIRECTORY;
    } else if (!(fattr & kNtFileAttributeReparsePoint)) {
      // we know for certain file isn't a directory
      if (flags & _O_DIRECTORY) {
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

static textwindows int sys_open_nt_file(int dirfd, const char *file,
                                        uint32_t flags, int32_t mode,
                                        size_t fd) {
  int64_t handle;
  if ((handle = sys_open_nt_impl(dirfd, file, flags, mode,
                                 kNtFileFlagOverlapped)) != -1) {
    g_fds.p[fd].handle = handle;
    g_fds.p[fd].kind = kFdFile;
    g_fds.p[fd].flags = flags;
    g_fds.p[fd].mode = mode;
    return fd;
  } else {
    return -1;
  }
}

static textwindows int sys_open_nt_special(int fd, int flags, int mode,
                                           int kind, const char16_t *name) {
  g_fds.p[fd].kind = kind;
  g_fds.p[fd].mode = mode;
  g_fds.p[fd].flags = flags;
  g_fds.p[fd].handle = CreateFile(name, kNtGenericRead | kNtGenericWrite,
                                  kNtFileShareRead | kNtFileShareWrite,
                                  &kNtIsInheritable, kNtOpenExisting, 0, 0);
  return fd;
}

static textwindows int sys_open_nt_dup(int fd, int flags, int mode, int oldfd) {
  int64_t handle;
  if (!__isfdopen(oldfd)) {
    return enoent();
  }
  if (DuplicateHandle(GetCurrentProcess(), g_fds.p[oldfd].handle,
                      GetCurrentProcess(), &handle, 0, true,
                      kNtDuplicateSameAccess)) {
    g_fds.p[fd] = g_fds.p[oldfd];
    g_fds.p[fd].handle = handle;
    g_fds.p[fd].mode = mode;
    if (!sys_fcntl_nt_setfl(fd, flags)) {
      return fd;
    } else {
      CloseHandle(handle);
      return -1;
    }
  } else {
    return __winerr();
  }
}

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

textwindows int sys_open_nt(int dirfd, const char *file, uint32_t flags,
                            int32_t mode) {
  ssize_t rc;
  int fd, oldfd;
  BLOCK_SIGNALS;
  __fds_lock();
  if (!(flags & _O_CREAT)) mode = 0;
  if ((rc = fd = __reservefd_unlocked(-1)) != -1) {
    if (startswith(file, "/dev/")) {
      if (!strcmp(file + 5, "tty")) {
        rc = sys_open_nt_special(fd, flags, mode, kFdConsole, u"CONIN$");
      } else if (!strcmp(file + 5, "null")) {
        rc = sys_open_nt_special(fd, flags, mode, kFdDevNull, u"NUL");
      } else if (!strcmp(file + 5, "stdin")) {
        rc = sys_open_nt_dup(fd, flags, mode, STDIN_FILENO);
      } else if (!strcmp(file + 5, "stdout")) {
        rc = sys_open_nt_dup(fd, flags, mode, STDOUT_FILENO);
      } else if (!strcmp(file + 5, "stderr")) {
        rc = sys_open_nt_dup(fd, flags, mode, STDERR_FILENO);
      } else if (startswith(file + 5, "fd/") &&
                 (oldfd = Atoi(file + 8)) != -1) {
        rc = sys_open_nt_dup(fd, flags, mode, oldfd);
      } else {
        rc = enoent();
      }
    } else {
      rc = sys_open_nt_file(dirfd, file, flags, mode, fd);
    }
    if (rc == -1) {
      __releasefd(fd);
    }
    __fds_unlock();
  }
  ALLOW_SIGNALS;
  return rc;
}
