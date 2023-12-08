/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/internal.h"
#include "libc/calls/struct/fsid.h"
#include "libc/calls/struct/statfs.h"
#include "libc/calls/struct/statfs.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/fsinformationclass.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/files.h"
#include "libc/nt/ntdll.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/nt/struct/filefsfullsizeinformation.h"
#include "libc/nt/struct/iostatusblock.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_fstatfs_nt(int64_t handle, struct statfs *f) {
  uint64_t w;
  NtStatus st;
  uint32_t h, i, j;
  struct NtIoStatusBlock io;
  struct NtFileFsFullSizeInformation fs;
  char16_t VolumeNameBuffer[261];
  uint32_t VolumeSerialNumber;
  uint32_t MaximumComponentLength;
  uint32_t FileSystemFlags;
  char16_t FileSystemNameBuffer[261];
  if (!GetVolumeInformationByHandle(
          handle, VolumeNameBuffer, ARRAYLEN(VolumeNameBuffer),
          &VolumeSerialNumber, &MaximumComponentLength, &FileSystemFlags,
          FileSystemNameBuffer, ARRAYLEN(FileSystemNameBuffer))) {
    return __winerr();
  }
  st = NtQueryVolumeInformationFile(handle, &io, &fs, sizeof(fs),
                                    kNtFileFsFullSizeInformation);
  if (!NtSuccess(st)) {
    if (st == kNtStatusDllNotFound) return enosys();
    return eio();
  }
  for (h = j = i = 0; FileSystemNameBuffer[i]; i++) {
    w = tpenc(FileSystemNameBuffer[i]);
    do {
      if (j + 1 < sizeof(f->f_fstypename)) {
        h = ((unsigned)(w & 255) + h) * 0x9e3779b1u;
        f->f_fstypename[j++] = w;
      }
    } while ((w >>= 8));
  }
  f->f_fstypename[j] = 0;
  f->f_type = h;
  f->f_fsid = (fsid_t){{VolumeSerialNumber}};
  f->f_flags = FileSystemFlags;
  f->f_bsize = fs.BytesPerSector;
  f->f_bsize *= fs.SectorsPerAllocationUnit;
  f->f_frsize = f->f_bsize;
  f->f_blocks = fs.TotalAllocationUnits;
  f->f_bfree = fs.ActualAvailableAllocationUnits;
  f->f_bavail = fs.CallerAvailableAllocationUnits;
  f->f_files = INT64_MAX;
  f->f_ffree = INT64_MAX;
  f->f_namelen = 255;
  f->f_owner = 0;
  return 0;
}
