/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/bits.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/fsctl.h"
#include "libc/nt/enum/io.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/reparsedatabuffer.h"
#include "libc/str/tpenc.h"
#include "libc/str/utf16.h"
#include "libc/sysv/errfuns.h"

static textwindows ssize_t sys_readlinkat_nt_error(void) {
  uint32_t e;
  e = GetLastError();
  SYSDEBUG("sys_readlinkat_nt() error %d", e);
  switch (e) {
    case kNtErrorNotAReparsePoint:
      return einval();
    default:
      errno = e;
      return -1;
  }
}

textwindows ssize_t sys_readlinkat_nt(int dirfd, const char *path, char *buf,
                                      size_t bufsiz) {
  int64_t h;
  ssize_t rc;
  uint64_t w;
  wint_t x, y;
  void *freeme;
  uint32_t e, i, j, n, mem;
  char16_t path16[PATH_MAX], *p;
  struct NtReparseDataBuffer *rdb;
  if (__mkntpathat(dirfd, path, 0, path16) == -1) {
    SYSDEBUG("sys_readlinkat_nt() failed b/c __mkntpathat() failed");
    return -1;
  }
  if (weaken(malloc)) {
    mem = 16384;
    rdb = weaken(malloc)(mem);
    freeme = rdb;
  } else if (bufsiz >= sizeof(struct NtReparseDataBuffer) + 16) {
    mem = bufsiz;
    rdb = (struct NtReparseDataBuffer *)buf;
    freeme = 0;
  } else {
    SYSDEBUG("sys_readlinkat_nt() needs bigger buffer malloc() to be yoinked");
    return enomem();
  }
  if ((h = CreateFile(path16, 0, 0, 0, kNtOpenExisting,
                      kNtFileFlagOpenReparsePoint | kNtFileFlagBackupSemantics,
                      0)) != -1) {
    if (DeviceIoControl(h, kNtFsctlGetReparsePoint, 0, 0, rdb, mem, &n, 0)) {
      if (rdb->ReparseTag == kNtIoReparseTagSymlink) {
        i = 0;
        j = 0;
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
          if (x < 0200) {
            if (x == '\\') {
              x = '/';
            }
            w = x;
          } else {
            w = tpenc(x);
          }
          do {
            if (j < bufsiz) {
              buf[j++] = w;
            }
            w >>= 8;
          } while (w);
        }
        if (freeme || (intptr_t)(buf + j) <= (intptr_t)(p + i)) {
          rc = j;
        } else {
          SYSDEBUG("sys_readlinkat_nt() too many astral codepoints");
          rc = enametoolong();
        }
      } else {
        SYSDEBUG("sys_readlinkat_nt() should have kNtIoReparseTagSymlink");
        rc = einval();
      }
    } else {
      SYSDEBUG("DeviceIoControl(kNtFsctlGetReparsePoint) failed");
      rc = sys_readlinkat_nt_error();
    }
    CloseHandle(h);
  } else {
    SYSDEBUG("CreateFile(kNtFileFlagOpenReparsePoint) failed");
    rc = sys_readlinkat_nt_error();
  }
  if (freeme && weaken(free)) {
    weaken(free)(freeme);
  }
  return rc;
}
