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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/tpenc.h"
#include "libc/mem/alloca.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/fsctl.h"
#include "libc/nt/enum/io.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/reparsedatabuffer.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/str/utf16.h"
#include "libc/sysv/errfuns.h"

textwindows ssize_t sys_readlinkat_nt(int dirfd, const char *path, char *buf,
                                      size_t bufsiz) {
  int64_t h;
  ssize_t rc;
  uint64_t w;
  wint_t x, y;
  volatile char *memory;
  uint32_t i, j, n, mem;
  char16_t path16[PATH_MAX], *p;
  struct NtReparseDataBuffer *rdb;
  if (__mkntpathat(dirfd, path, 0, path16) == -1) return -1;
  mem = 16384;
  memory = alloca(mem);
  CheckLargeStackAllocation(memory, mem);
  rdb = (struct NtReparseDataBuffer *)memory;
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
        if (n >= 3 && isalpha(p[0]) && p[1] == ':' && p[2] == '\\') {
          p[1] = p[0];
          p[0] = '/';
          p[2] = '/';
        }
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
            w = _tpenc(x);
          }
          do {
            if (j < bufsiz) {
              buf[j++] = w;
            }
            w >>= 8;
          } while (w);
        }
        rc = j;
      } else {
        NTTRACE("sys_readlinkat_nt() should have kNtIoReparseTagSymlink");
        rc = einval();
      }
    } else {
      rc = -1;
    }
    CloseHandle(h);
  } else {
    rc = __fix_enotdir(-1, path16);
  }
  return rc;
}
