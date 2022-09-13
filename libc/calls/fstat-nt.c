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
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/tpenc.h"
#include "libc/macros.internal.h"
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

static textwindows uint32_t GetSizeOfReparsePoint(int64_t fh) {
  wint_t x, y;
  const char16_t *p;
  uint32_t mem, i, n, z = 0;
  struct NtReparseDataBuffer *rdb;
  long buf[(sizeof(*rdb) + PATH_MAX * sizeof(char16_t)) / sizeof(long)];
  mem = sizeof(buf);
  rdb = (struct NtReparseDataBuffer *)buf;
  if (DeviceIoControl(fh, kNtFsctlGetReparsePoint, 0, 0, rdb, mem, &n, 0)) {
    i = 0;
    n = rdb->SymbolicLinkReparseBuffer.PrintNameLength / sizeof(char16_t);
    p = (char16_t *)((char *)rdb->SymbolicLinkReparseBuffer.PathBuffer +
                     rdb->SymbolicLinkReparseBuffer.PrintNameOffset);
    while (i < n) {
      x = p[i++] & 0xffff;
      if (!IsUcs2(x)) {
        if (i < n) {
          y = p[i++] & 0xffff;
          x = MergeUtf16(x, y);
        } else {
          x = 0xfffd;
        }
      }
      z += x < 0200 ? 1 : _bsrl(_tpenc(x)) >> 3;
    }
  } else {
    STRACE("%s failed %m", "GetSizeOfReparsePoint");
  }
  return z;
}

textwindows int sys_fstat_nt(int64_t handle, struct stat *st) {
  int filetype;
  uint64_t actualsize;
  struct NtFileCompressionInfo fci;
  struct NtByHandleFileInformation wst;
  if (!st) return efault();
  if ((filetype = GetFileType(handle))) {
    bzero(st, sizeof(*st));
    switch (filetype) {
      case kNtFileTypeChar:
        st->st_mode = S_IFCHR | 0644;
        break;
      case kNtFileTypePipe:
        st->st_mode = S_IFIFO | 0644;
        break;
      case kNtFileTypeDisk:
        if (GetFileInformationByHandle(handle, &wst)) {
          st->st_mode = 0555;
          st->st_flags = wst.dwFileAttributes;
          if (!(wst.dwFileAttributes & kNtFileAttributeReadonly)) {
            st->st_mode |= 0200;
          }
          if (wst.dwFileAttributes & kNtFileAttributeDirectory) {
            st->st_mode |= S_IFDIR;
          } else if (wst.dwFileAttributes & kNtFileAttributeReparsePoint) {
            st->st_mode |= S_IFLNK;
          } else {
            st->st_mode |= S_IFREG;
          }
          st->st_atim = FileTimeToTimeSpec(wst.ftLastAccessFileTime);
          st->st_mtim = FileTimeToTimeSpec(wst.ftLastWriteFileTime);
          st->st_ctim = FileTimeToTimeSpec(wst.ftCreationFileTime);
          st->st_birthtim = st->st_ctim;
          st->st_size = (uint64_t)wst.nFileSizeHigh << 32 | wst.nFileSizeLow;
          st->st_blksize = PAGESIZE;
          st->st_dev = wst.dwVolumeSerialNumber;
          st->st_rdev = 0;
          st->st_ino = (uint64_t)wst.nFileIndexHigh << 32 | wst.nFileIndexLow;
          st->st_nlink = wst.nNumberOfLinks;
          if (S_ISLNK(st->st_mode)) {
            if (!st->st_size) {
              st->st_size = GetSizeOfReparsePoint(handle);
            }
          } else {
            actualsize = st->st_size;
            if (S_ISREG(st->st_mode) &&
                GetFileInformationByHandleEx(handle, kNtFileCompressionInfo,
                                             &fci, sizeof(fci))) {
              actualsize = fci.CompressedFileSize;
            }
            st->st_blocks = ROUNDUP(actualsize, PAGESIZE) / 512;
          }
        } else {
          STRACE("%s failed %m", "GetFileInformationByHandle");
        }
        break;
      default:
        break;
    }
    return 0;
  } else {
    STRACE("%s failed %m", "GetFileType");
    return __winerr();
  }
}
