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
#include "libc/bits/safemacros.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/fmt/conv.h"
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
  struct NtFileCompressionInfo fci;
  struct NtByHandleFileInformation wst;
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
