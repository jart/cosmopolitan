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
#include "libc/calls/calls.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/limits.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"

textwindows int sys_linkat_nt(int olddirfd, const char *oldpath, int newdirfd,
                              const char *newpath) {
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  struct {
    char16_t newpath16[PATH_MAX];
    char16_t oldpath16[PATH_MAX];
  } M;
  CheckLargeStackAllocation(&M, sizeof(M));
#pragma GCC pop_options
  if (__mkntpathat(olddirfd, oldpath, 0, M.oldpath16) != -1 &&
      __mkntpathat(newdirfd, newpath, 0, M.newpath16) != -1) {
    if (CreateHardLink(M.newpath16, M.oldpath16, NULL)) {
      return 0;
    } else {
      return __fix_enotdir3(__winerr(), M.newpath16, M.oldpath16);
    }
  } else {
    return -1;
  }
}
