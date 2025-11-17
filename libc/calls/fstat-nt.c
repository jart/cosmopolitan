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
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/stat.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/cosmotime.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/intrin/fds.h"
#include "libc/macros.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/fileinfobyhandleclass.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/io.h"
#include "libc/nt/files.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/pib.h"

#define MAKEDEV(major, minor) ((uint64_t)(major) << 32 | (minor))

textwindows dontinline static int sys_fstat_nt_symsize(
    int64_t hSymlink, uint32_t *opt_out_ReparseTag) {
  char path8[PATH_MAX];
  return __readntsym(hSymlink, path8, opt_out_ReparseTag);
}

textwindows int sys_fstat_nt_char(int kind, struct stat *st) {
  bzero(st, sizeof(*st));
  st->st_blksize = 512;
  st->st_mode = S_IFCHR | 0666;
  st->st_dev = MAKEDEV(kind, 0);
  st->st_ino = 0;
  return 0;
}

textwindows int sys_fstat_nt(int fd, struct stat *st) {
  if (fd + 0u >= __get_pib()->fds.n)
    return ebadf();
  switch (__get_pib()->fds.p[fd].kind) {
    case kFdEmpty:
      return ebadf();
    case kFdConsole:
    case kFdDevNull:
    case kFdDevRandom:
      return sys_fstat_nt_char(__get_pib()->fds.p[fd].kind, st);
    case kFdSocket:
      bzero(st, sizeof(*st));
      st->st_blksize = 512;
      st->st_mode = S_IFSOCK | 0666;
      st->st_dev = MAKEDEV(kFdSocket, 0);
      st->st_ino = fd;
      return 0;
    default:
      // if the file was opened, then we know it's at least readable
      return sys_fstat_nt_handle(__get_pib()->fds.p[fd].handle, 0, st, 0444);
  }
}

textwindows int sys_fstat_nt_handle(int64_t handle, const char16_t *path,
                                    struct stat *out_st, int mode) {

  // Always set st_blksize to avoid divide by zero issues.
  // The Linux kernel sets this for /dev/tty and similar too.
  struct stat st = {0};
  st.st_blksize = 4096;
  st.st_gid = st.st_uid = sys_getuid_nt();

  // get file type
  uint32_t ft = GetFileType(handle);

  // get file information
  struct NtByHandleFileInformation wst;
  if (GetFileInformationByHandle(handle, &wst)) {
    st.st_mode = mode;
    if ((wst.dwFileAttributes & kNtFileAttributeDirectory) ||
        (ft == kNtFileTypeDisk && IsWindowsExecutable(handle, path)))
      st.st_mode |= 0111;
    st.st_flags = wst.dwFileAttributes;
    if (!(wst.dwFileAttributes & kNtFileAttributeReadonly))
      st.st_mode |= 0220;
    if (wst.dwFileAttributes & kNtFileAttributeReparsePoint) {
      st.st_mode |= S_IFLNK;
    } else if (wst.dwFileAttributes & kNtFileAttributeDirectory) {
      st.st_mode |= S_IFDIR;
    } else if (ft == kNtFileTypeDisk) {
      st.st_mode |= S_IFREG;
    } else if (ft == kNtFileTypeChar) {
      st.st_mode |= S_IFCHR;
    } else if (ft == kNtFileTypePipe) {
      st.st_mode |= S_IFIFO;
    }
    st.st_atim = FileTimeToTimeSpec(wst.ftLastAccessFileTime);
    st.st_mtim = FileTimeToTimeSpec(wst.ftLastWriteFileTime);
    st.st_birthtim = FileTimeToTimeSpec(wst.ftCreationFileTime);
    // compute time of last status change
    if (timespec_cmp(st.st_atim, st.st_mtim) > 0) {
      st.st_ctim = st.st_atim;
    } else {
      st.st_ctim = st.st_mtim;
    }
    st.st_size = (wst.nFileSizeHigh + 0ull) << 32 | wst.nFileSizeLow;
    st.st_dev = MAKEDEV(0, wst.dwVolumeSerialNumber);
    st.st_ino = (wst.nFileIndexHigh + 0ull) << 32 | wst.nFileIndexLow;
    st.st_nlink = wst.nNumberOfLinks;

    // if it's a symbolic link, then calculate the utf-8 length of its
    // content in a way that's consistent with readlinkat()
    if (S_ISLNK(st.st_mode) && !st.st_size) {
      int e = errno;
      uint32_t ReparseTag;
      int size = sys_fstat_nt_symsize(handle, &ReparseTag);
      if (size != -1) {
        st.st_size = size;
      } else if (errno == ENOLINK) {
        errno = e;
        if (ReparseTag == kNtIoReparseTagAfUnix) {
          // surprise! it's a unix domain named socket
          st.st_mode &= ~S_IFMT;
          st.st_mode |= S_IFSOCK;
        } else {
          // there's many other reparse tags. for example, ones exist
          // that let wsl create blk/fifo/chr files. but cosmopolitan
          // doesn't know how to use those and treats them as regular
          st.st_mode &= ~S_IFMT;
          st.st_mode |= S_IFREG;
        }
      } else {
        return -1;
      }
    }

    st.st_blocks = ROUNDUP(st.st_size, st.st_blksize) / 512;
  } else if (GetVolumeInformationByHandle(
                 handle, 0, 0, &wst.dwVolumeSerialNumber, 0, 0, 0, 0)) {
    st.st_dev = MAKEDEV(0, wst.dwVolumeSerialNumber);
    st.st_mode = S_IFDIR | 0555;
  } else if (ft == kNtFileTypeChar) {
    st.st_blksize = 512;
    st.st_mode = S_IFCHR | 0666;
    st.st_dev = 0x66666666;
    st.st_ino = handle;
  } else if (ft == kNtFileTypePipe) {
    st.st_blksize = 512;
    st.st_mode = S_IFIFO | 0666;
    st.st_dev = 0x55555555;
    st.st_ino = handle;
  } else {
    return __winerr();
  }

  memcpy(out_st, &st, sizeof(st));
  return 0;
}
