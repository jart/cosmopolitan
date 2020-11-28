/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/conv/conv.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/fileinfobyhandleclass.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/nt/struct/filecompressioninfo.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/s.h"

textwindows int fstat$nt(int64_t handle, struct stat *st) {
  int filetype;
  uint64_t actualsize;
  struct NtByHandleFileInformation wst;
  struct NtFileCompressionInfo fci;
  if (GetFileInformationByHandle(handle, &wst)) {
    memset(st, 0, sizeof(*st));
    filetype = GetFileType(handle);
    st->st_mode =
        (S_IRUSR | S_IXUSR |
         (!(wst.dwFileAttributes & kNtFileAttributeReadonly) ? S_IWUSR : 0) |
         ((wst.dwFileAttributes & kNtFileAttributeNormal) ? S_IFREG : 0) |
         ((wst.dwFileAttributes & kNtFileFlagOpenReparsePoint) ? S_IFLNK : 0) |
         ((wst.dwFileAttributes & kNtFileAttributeDirectory)
              ? S_IFDIR
              : (((filetype == kNtFileTypeDisk) ? S_IFBLK : 0) |
                 ((filetype == kNtFileTypeChar) ? S_IFCHR : 0) |
                 ((filetype == kNtFileTypePipe) ? S_IFIFO : 0))));
    st->st_atim = FileTimeToTimeSpec(wst.ftLastAccessFileTime);
    st->st_mtim = FileTimeToTimeSpec(wst.ftLastWriteFileTime);
    st->st_ctim = FileTimeToTimeSpec(wst.ftCreationFileTime);
    st->st_size = (uint64_t)wst.nFileSizeHigh << 32 | wst.nFileSizeLow;
    st->st_blksize = PAGESIZE;
    st->st_dev = wst.dwVolumeSerialNumber;
    st->st_ino = (uint64_t)wst.nFileIndexHigh << 32 | wst.nFileIndexLow;
    st->st_nlink = wst.nNumberOfLinks;
    if (GetFileInformationByHandleEx(handle, kNtFileCompressionInfo, &fci,
                                     sizeof(fci))) {
      actualsize = fci.CompressedFileSize;
    } else {
      actualsize = st->st_size;
    }
    st->st_blocks = roundup(actualsize, PAGESIZE) / 512;
    return 0;
  } else {
    return __winerr();
  }
}
