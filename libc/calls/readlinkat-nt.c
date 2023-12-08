/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/mem/alloca.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/fsctl.h"
#include "libc/nt/enum/io.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/reparsedatabuffer.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/str/utf16.h"
#include "libc/sysv/errfuns.h"

static textwindows ssize_t sys_readlinkat_nt_impl(int dirfd, const char *path,
                                                  char *buf, size_t bufsiz) {

  char16_t path16[PATH_MAX];
  if (__mkntpathat(dirfd, path, 0, path16) == -1) return -1;
  size_t len = strlen16(path16);
  bool must_be_directory = len > 1 && path16[len - 1] == '\\';
  if (must_be_directory) path16[--len] = 0;

  int64_t h;
  ssize_t rc;
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Walloca-larger-than="
  uint32_t mem = 6000;
  volatile char *memory = alloca(mem);
  CheckLargeStackAllocation((char *)memory, mem);
#pragma GCC pop_options
  struct NtReparseDataBuffer *rdb = (struct NtReparseDataBuffer *)memory;
  if ((h = CreateFile(path16, kNtFileReadAttributes,
                      kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete,
                      0, kNtOpenExisting,
                      kNtFileFlagOpenReparsePoint | kNtFileFlagBackupSemantics,
                      0)) != -1) {

    // if path had trailing slash, assert last component is directory
    if (must_be_directory) {
      struct NtByHandleFileInformation wst;
      if (GetFileType(h) != kNtFileTypeDisk ||
          (GetFileInformationByHandle(h, &wst) &&
           !(wst.dwFileAttributes & kNtFileAttributeDirectory))) {
        return enotdir();
      }
    }

    uint32_t bc;
    if (DeviceIoControl(h, kNtFsctlGetReparsePoint, 0, 0, rdb, mem, &bc, 0)) {
      if (rdb->ReparseTag == kNtIoReparseTagSymlink) {

        uint32_t i = 0;
        uint32_t j = 0;
        uint32_t n =
            rdb->SymbolicLinkReparseBuffer.PrintNameLength / sizeof(char16_t);
        char16_t *p =
            (char16_t *)((char *)rdb->SymbolicLinkReparseBuffer.PathBuffer +
                         rdb->SymbolicLinkReparseBuffer.PrintNameOffset);
        if (n >= 3 && isalpha(p[0]) && p[1] == ':' && p[2] == '\\') {
          p[1] = p[0];
          p[0] = '/';
          p[2] = '/';
        }
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

          uint64_t w;
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

textwindows ssize_t sys_readlinkat_nt(int dirfd, const char *path, char *buf,
                                      size_t bufsiz) {
  ssize_t rc;
  BLOCK_SIGNALS;
  rc = sys_readlinkat_nt_impl(dirfd, path, buf, bufsiz);
  ALLOW_SIGNALS;
  return rc;
}
