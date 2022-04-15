/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/sysv/consts/o.h"

// code size optimization
// <sync libc/sysv/consts.sh>
#define _O_APPEND     0x00000400  // kNtFileAppendData
#define _O_CREAT      0x00000040  // kNtOpenAlways
#define _O_EXCL       0x00000080  // kNtCreateNew
#define _O_TRUNC      0x00000200  // kNtCreateAlways
#define _O_DIRECTORY  0x00010000  // kNtFileFlagBackupSemantics
#define _O_TMPFILE    0x00410000  // AttributeTemporary|FlagDeleteOnClose
#define _O_DIRECT     0x00004000  // kNtFileFlagNoBuffering
#define _O_NDELAY     0x00000800  // kNtFileFlagWriteThrough
#define _O_RANDOM     0x80000000  // kNtFileFlagRandomAccess
#define _O_SEQUENTIAL 0x40000000  // kNtFileFlagSequentialScan
#define _O_COMPRESSED 0x20000000  // kNtFileAttributeCompressed
#define _O_INDEXED    0x10000000  // !kNtFileAttributeNotContentIndexed
#define _O_NONBLOCK   0x00000800
#define _O_CLOEXEC    0x00080000
// </sync libc/sysv/consts.sh>

textwindows int GetNtOpenFlags(int flags, int mode, uint32_t *out_perm,
                               uint32_t *out_share, uint32_t *out_disp,
                               uint32_t *out_attr) {
  uint32_t perm, share, disp, attr;

  switch (flags & O_ACCMODE) {
    case O_RDONLY:
      perm = kNtFileGenericRead | kNtGenericExecute;
      break;
    case O_WRONLY:
      perm = kNtFileGenericWrite | kNtGenericExecute;
      break;
    case O_RDWR:
      perm = kNtFileGenericRead | kNtFileGenericWrite | kNtGenericExecute;
      break;
    default:
      return -1;
  }
  if (flags & _O_APPEND) {
    perm = kNtFileAppendData;
  }

  if (flags & _O_EXCL) {
    share = kNtFileShareExclusive;
  } else {
    share = kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete;
  }

  if ((flags & _O_CREAT) && (flags & _O_EXCL)) {
    disp = kNtCreateNew;
  } else if ((flags & _O_CREAT) && (flags & _O_TRUNC)) {
    disp = kNtCreateAlways;
  } else if (flags & _O_CREAT) {
    disp = kNtOpenAlways;
  } else if (flags & _O_TRUNC) {
    disp = kNtTruncateExisting;
  } else {
    disp = kNtOpenExisting;
  }

  if ((flags & _O_TMPFILE) == _O_TMPFILE) {
    attr = kNtFileAttributeTemporary | kNtFileFlagDeleteOnClose;
  } else {
    attr = kNtFileAttributeNormal;
    if (flags & _O_DIRECTORY) {
      attr |= kNtFileFlagBackupSemantics;
    }
    if (~mode & 0200) {
      attr |= kNtFileAttributeReadonly;
    }
  }

  if (~flags & _O_INDEXED) attr |= kNtFileAttributeNotContentIndexed;
  if (flags & _O_COMPRESSED) attr |= kNtFileAttributeCompressed;
  if (flags & _O_SEQUENTIAL) attr |= kNtFileFlagSequentialScan;
  if (flags & _O_RANDOM) attr |= kNtFileFlagRandomAccess;
  if (flags & _O_DIRECT) attr |= kNtFileFlagNoBuffering;
  if (flags & _O_NDELAY) attr |= kNtFileFlagWriteThrough;

  if (out_perm) *out_perm = perm;
  if (out_share) *out_share = share;
  if (out_disp) *out_disp = disp;
  if (out_attr) *out_attr = attr;
  return 0;
}
