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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/stat.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/alloca.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/fileinfobyhandleclass.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/fsctl.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/nt/struct/filecompressioninfo.h"
#include "libc/nt/struct/reparsedatabuffer.h"
#include "libc/str/str.h"
#include "libc/str/utf16.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"

static textwindows long GetSizeOfReparsePoint(int64_t fh) {
  uint32_t mem =
      sizeof(struct NtReparseDataBuffer) + PATH_MAX * sizeof(char16_t);
  void *buf = alloca(mem);
  uint32_t dwBytesReturned;
  struct NtReparseDataBuffer *rdb = (struct NtReparseDataBuffer *)buf;
  if (!DeviceIoControl(fh, kNtFsctlGetReparsePoint, 0, 0, rdb, mem,
                       &dwBytesReturned, 0)) {
    return -1;
  }
  const char16_t *p =
      (const char16_t *)((char *)rdb->SymbolicLinkReparseBuffer.PathBuffer +
                         rdb->SymbolicLinkReparseBuffer.PrintNameOffset);
  uint32_t n =
      rdb->SymbolicLinkReparseBuffer.PrintNameLength / sizeof(char16_t);
  uint32_t i = 0;
  uint32_t z = 0;
  while (i < n) {
    wint_t x = p[i++] & 0xffff;
    if (!IsUcs2(x)) {
      if (i < n) {
        wint_t y = p[i++] & 0xffff;
        x = MergeUtf16(x, y);
      } else {
        x = 0xfffd;
      }
    }
    if (x >= 0200) {
      z += _bsrl(tpenc(x)) >> 3;
    }
    ++z;
  }
  return z;
}

static textwindows int sys_fstat_nt_socket(int kind, struct stat *st) {
  bzero(st, sizeof(*st));
  st->st_blksize = 512;
  st->st_mode = S_IFSOCK | 0666;
  st->st_dev = 0x44444444;
  st->st_ino = kind;
  return 0;
}

textwindows int sys_fstat_nt_special(int kind, struct stat *st) {
  bzero(st, sizeof(*st));
  st->st_blksize = 512;
  st->st_mode = S_IFCHR | 0666;
  st->st_dev = 0x77777777;
  st->st_ino = kind;
  return 0;
}

textwindows int sys_fstat_nt(int fd, struct stat *st) {
  if (fd + 0u >= g_fds.n) return ebadf();
  switch (g_fds.p[fd].kind) {
    case kFdEmpty:
      return ebadf();
    case kFdConsole:
    case kFdDevNull:
      return sys_fstat_nt_special(g_fds.p[fd].kind, st);
    case kFdSocket:
      return sys_fstat_nt_socket(g_fds.p[fd].kind, st);
    default:
      return sys_fstat_nt_handle(g_fds.p[fd].handle, 0, st);
  }
}

textwindows int sys_fstat_nt_handle(int64_t handle, const char16_t *path,
                                    struct stat *out_st) {
  struct stat st = {0};

  // Always set st_blksize to avoid divide by zero issues.
  // The Linux kernel sets this for /dev/tty and similar too.
  // TODO(jart): GetVolumeInformationByHandle?
  st.st_blksize = 4096;
  st.st_gid = st.st_uid = sys_getuid_nt();

  // We'll use the "umask" to fake out the mode bits.
  uint32_t umask = atomic_load_explicit(&__umask, memory_order_acquire);

  switch (GetFileType(handle)) {
    case kNtFileTypeUnknown:
      break;
    case kNtFileTypeChar:
      st.st_mode = S_IFCHR | (0666 & ~umask);
      st.st_dev = 0x66666666;
      st.st_ino = handle;
      break;
    case kNtFileTypePipe:
      st.st_mode = S_IFIFO | (0666 & ~umask);
      st.st_dev = 0x55555555;
      st.st_ino = handle;
      break;
    case kNtFileTypeDisk: {
      struct NtByHandleFileInformation wst;
      if (!GetFileInformationByHandle(handle, &wst)) {
        return __winerr();
      }
      st.st_mode = 0444 & ~umask;
      if ((wst.dwFileAttributes & kNtFileAttributeDirectory) ||
          IsWindowsExecutable(handle, path)) {
        st.st_mode |= 0111 & ~umask;
      }
      st.st_flags = wst.dwFileAttributes;
      if (!(wst.dwFileAttributes & kNtFileAttributeReadonly)) {
        st.st_mode |= 0222 & ~umask;
      }
      if (wst.dwFileAttributes & kNtFileAttributeReparsePoint) {
        st.st_mode |= S_IFLNK;
      } else if (wst.dwFileAttributes & kNtFileAttributeDirectory) {
        st.st_mode |= S_IFDIR;
      } else {
        st.st_mode |= S_IFREG;
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
      st.st_dev = wst.dwVolumeSerialNumber;
      st.st_ino = (wst.nFileIndexHigh + 0ull) << 32 | wst.nFileIndexLow;
      st.st_nlink = wst.nNumberOfLinks;
      if (S_ISLNK(st.st_mode)) {
        if (!st.st_size) {
          long size = GetSizeOfReparsePoint(handle);
          if (size == -1) return -1;
          st.st_size = size;
        }
      } else {
        // st_size       = uncompressed size
        // st_blocks*512 = physical size
        uint64_t physicalsize;
        struct NtFileCompressionInfo fci;
        if (!(wst.dwFileAttributes &
              (kNtFileAttributeDirectory | kNtFileAttributeReparsePoint)) &&
            GetFileInformationByHandleEx(handle, kNtFileCompressionInfo, &fci,
                                         sizeof(fci))) {
          physicalsize = fci.CompressedFileSize;
        } else {
          physicalsize = st.st_size;
        }
        st.st_blocks = ROUNDUP(physicalsize, st.st_blksize) / 512;
      }
      break;
    }
    default:
      __builtin_unreachable();
  }

  memcpy(out_st, &st, sizeof(st));
  return 0;
}
