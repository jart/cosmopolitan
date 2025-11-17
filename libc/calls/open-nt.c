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
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/ctype.h"
#include "libc/errno.h"
#include "libc/intrin/fds.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.h"
#include "libc/nt/console.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/fileinfobyhandleclass.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/filebasicinfo.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/pib.h"

textwindows static int sys_open_nt_atoi(const char *str) {
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

textwindows static bool sys_open_nt_ishiddenpath(const char16_t *p, int n) {
  int i = n;
  while (i) {
    if (p[i - 1] == '\\')
      break;
    --i;
  }
  if (i < n)
    if (p[i] == '.')
      return true;
  return false;
}

textwindows static intptr_t sys_open_nt_impl(int dirfd, const char *path,
                                             uint32_t flags, int32_t mode,
                                             uint32_t extra_attr) {

  // join(topath(dirfd), path) and translate from utf-8 to utf-16
  int path16len;
  char16_t path16[PATH_MAX];
  if ((path16len = __mkntpathat(dirfd, path, path16)) == -1)
    return -1;

  // implement no follow flag
  // you can't open symlinks; use readlink
  // this flag only applies to the final path component
  uint32_t fattr = GetFileAttributes(path16);
  if (flags & O_NOFOLLOW) {
    if (fattr != -1u && (fattr & kNtFileAttributeReparsePoint))
      return eloop();
    flags &= ~O_NOFOLLOW;  // don't actually pass this to win32
  }

  // handle some obvious cases while we have the attributes
  // we should ideally resolve symlinks ourself before doing this
  if (fattr != -1u) {
    if ((flags & O_CREAT) && (flags & O_EXCL))
      return eexist();
    // GetFileAttributes() doesn't follow symlinks, but win32 symlinks
    // have a directory status bit, which is set when the link is made
    if (fattr & kNtFileAttributeDirectory) {
      if ((flags & O_ACCMODE) != O_RDONLY || (flags & O_CREAT))
        // tried to open directory for writing. note that our
        // undocumented O_TMPFILE support on windows requires that a
        // filename be passed, rather than a directory like linux.
        return eisdir();
      // on posix, the o_directory flag is an advisory safeguard that
      // isn't required. on windows, it's mandatory for opening a dir
      flags |= O_DIRECTORY;
    } else {
      // we know for certain file isn't a directory
      if (flags & O_DIRECTORY)
        return enotdir();
    }
  }

  // translate posix flags to win32 flags
  uint32_t perm, share, disp, attr;
  if (GetNtOpenFlags(flags, mode, &perm, &share, &disp, &attr) == -1)
    return -1;

  // set hidden attribute on new files if last component starts with dot
  if (flags & O_CREAT)
    if (sys_open_nt_ishiddenpath(path16, path16len))
      attr |= kNtFileAttributeHidden;

  // the mode parameter should do nothing when file already exists
  if (fattr != -1u) {
    perm |= kNtGenericExecute;
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

  // optimistically request fchmod() permissions
  uint32_t extra_perm = kNtFileReadAttributes | kNtFileWriteAttributes |
                        kNtReadControl | kNtWriteDac;

  // open file, following symlinks
  int64_t hand = CreateFile(path16, perm | extra_perm, share, &kNtIsInheritable,
                            disp, attr | extra_attr, 0);

  // try again without greedy permissions if needed
  // our CreateFile() wrapper also retries without exec
  if (hand == -1)
    if (GetLastError() == kNtErrorAccessDenied)
      hand = CreateFile(path16, perm, share, &kNtIsInheritable, disp,
                        attr | extra_attr, 0);

  // set errno on error
  if (hand == -1)
    __winerr();

  // make it possible to create an unreadable file
  if (hand != -1)
    if (fattr == -1u)
      if (~mode & 0400)
        RestrictFileWin32(hand, kNtFileReadData);

  return __fix_enotdir(hand, path16);
}

textwindows static bool sys_open_nt_used_explicit_drive_letter(const char *p) {
  if (p[0] == '/' && isalpha(p[1]) && (!p[2] || p[2] == '/'))
    return true;
  if (isalpha(p[0]) && (!p[1] || p[1] == '/'))
    if (__get_pib()->cwd[0] == '/' && !__get_pib()->cwd[1])
      return true;
  return false;
}

textwindows static int sys_open_nt_file(int dirfd, const char *file,
                                        uint32_t flags, int32_t mode,
                                        size_t fd) {
  intptr_t handle;
  if ((handle = sys_open_nt_impl(dirfd, file, flags, mode,
                                 kNtFileFlagOverlapped)) != -1) {
    __get_pib()->fds.p[fd].handle = handle;
    __get_pib()->fds.p[fd].flags = flags;
    __get_pib()->fds.p[fd].mode = mode;
    __get_pib()->fds.p[fd].used_explicit_drive_letter =
        sys_open_nt_used_explicit_drive_letter(file);
    if (!(flags & O_APPEND) || (flags & O_ACCMODE) != O_WRONLY) {
      __fds_lock();
      __get_pib()->fds.p[fd].cursor = __cursor_new();
      __fds_unlock();
    }
    __get_pib()->fds.p[fd].kind = kFdFile;
    return fd;
  } else {
    return -1;
  }
}

textwindows static int sys_open_nt_special(int fd, int flags, int mode,
                                           int kind, const char16_t *name) {
  __get_pib()->fds.p[fd].handle =
      CreateFile(name, kNtGenericRead | kNtGenericWrite,
                 kNtFileShareRead | kNtFileShareWrite, &kNtIsInheritable,
                 kNtOpenExisting, 0, 0);
  __get_pib()->fds.p[fd].flags = flags;
  __get_pib()->fds.p[fd].mode = mode;
  __get_pib()->fds.p[fd].kind = kind;
  return fd;
}

textwindows static int sys_open_nt_no_handle(int fd, int flags, int mode,
                                             int kind) {
  __get_pib()->fds.p[fd].mode = mode;
  __get_pib()->fds.p[fd].flags = flags;
  __get_pib()->fds.p[fd].handle = -1;
  __get_pib()->fds.p[fd].kind = kind;
  return fd;
}

textwindows static int sys_open_nt_dup(int fd, int flags, int mode, int oldfd) {
  int64_t handle;
  if (!__isfdopen(oldfd))
    return enoent();
  if (DuplicateHandle(GetCurrentProcess(), __get_pib()->fds.p[oldfd].handle,
                      GetCurrentProcess(), &handle, 0, true,
                      kNtDuplicateSameAccess)) {
    __get_pib()->fds.p[fd] = __get_pib()->fds.p[oldfd];
    __get_pib()->fds.p[fd].handle = handle;
    __get_pib()->fds.p[fd].mode = mode;
    __cursor_ref(__get_pib()->fds.p[fd].cursor);
    sys_fcntl_nt_setfl(&__get_pib()->fds.p[fd], flags);
    return fd;
  } else {
    return __winerr();
  }
}

textwindows static int sys_open_nt_dispatch(int dirfd, const char *file,
                                            uint32_t flags, int32_t mode,
                                            int fd) {
  int oldfd;
  if (startswith(file, "/dev/")) {
    if (!strcmp(file + 5, "tty"))
      return sys_open_nt_special(fd, flags, mode, kFdConsole, u"CONIN$");
    if (!strcmp(file + 5, "null"))
      return sys_open_nt_special(fd, flags, mode, kFdDevNull, u"NUL");
    if (!strcmp(file + 5, "urandom") || !strcmp(file + 5, "random"))
      return sys_open_nt_no_handle(fd, flags, mode, kFdDevRandom);
    if (!strcmp(file + 5, "stdin"))
      return sys_open_nt_dup(fd, flags, mode, STDIN_FILENO);
    if (!strcmp(file + 5, "stdout"))
      return sys_open_nt_dup(fd, flags, mode, STDOUT_FILENO);
    if (!strcmp(file + 5, "stderr"))
      return sys_open_nt_dup(fd, flags, mode, STDERR_FILENO);
    if (startswith(file + 5, "fd/"))
      if ((oldfd = sys_open_nt_atoi(file + 8)) != -1)
        return sys_open_nt_dup(fd, flags, mode, oldfd);
  }
  return sys_open_nt_file(dirfd, file, flags, mode, fd);
}

textwindows int sys_open_nt(int dirfd, const char *file, uint32_t flags,
                            int32_t mode) {
  int newfd;
  mode &= ~__get_pib()->umask;
  if (!(flags & O_CREAT))
    mode = 0;
  if (mode & 07000)
    return eperm();
  BLOCK_SIGNALS;
  if ((newfd = __reservefd(-1)) != -1) {
    if (sys_open_nt_dispatch(dirfd, file, flags, mode, newfd) != -1) {
      __get_pib()->fds.p[newfd].was_created_during_vfork = __vforked;
    } else {
      __releasefd(newfd);
      newfd = -1;
    }
  }
  ALLOW_SIGNALS;
  return newfd;
}
