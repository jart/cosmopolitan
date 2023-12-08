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
#include "libc/cosmo.h"
#include "libc/errno.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/symboliclink.h"
#include "libc/nt/enum/tokeninformationclass.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/privilege.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/luid.h"
#include "libc/nt/struct/tokenprivileges.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

static struct {
  _Atomic(uint32_t) once;
  _Bool allowed;
} g_winlink;

static textwindows void InitializeWinlink(void) {
  int64_t tok;
  struct NtLuid id;
  struct NtTokenPrivileges tp;
  if (!OpenProcessToken(GetCurrentProcess(), kNtTokenAllAccess, &tok)) return;
  if (!LookupPrivilegeValue(0, u"SeCreateSymbolicLinkPrivilege", &id)) return;
  tp.PrivilegeCount = 1;
  tp.Privileges[0].Luid = id;
  tp.Privileges[0].Attributes = kNtSePrivilegeEnabled;
  if (!AdjustTokenPrivileges(tok, 0, &tp, sizeof(tp), 0, 0)) return;
  g_winlink.allowed = GetLastError() != kNtErrorNotAllAssigned;
}

textwindows int sys_symlinkat_nt(const char *target, int newdirfd,
                                 const char *linkpath) {
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  struct {
    char16_t target16[PATH_MAX];
    char16_t linkpath16[PATH_MAX];
  } M;
  CheckLargeStackAllocation(&M, sizeof(M));
#pragma GCC pop_options
  int targetlen;
  uint32_t attrs, flags;

  // convert the paths
  if (__mkntpathat(newdirfd, linkpath, 0, M.linkpath16) == -1) return -1;
  if ((targetlen = __mkntpath(target, M.target16)) == -1) return -1;

  // determine if we need directory flag
  if ((attrs = GetFileAttributes(M.target16)) != -1u) {
    if (attrs & kNtFileAttributeDirectory) {
      flags = kNtSymbolicLinkFlagDirectory;
    } else {
      flags = 0;
    }
  } else {
    // win32 needs to know if it's a directory of a file symlink, but
    // that's hard to determine if we're creating a broken symlink so
    // we'll fall back to checking for trailing slash
    if (targetlen && M.target16[targetlen - 1] == '\\') {
      flags = kNtSymbolicLinkFlagDirectory;
    } else {
      flags = 0;
    }
  }

  // windows only lets administrators do this
  // even then we're required to ask for permission
  cosmo_once(&g_winlink.once, InitializeWinlink);
  if (!g_winlink.allowed) {
    return eperm();
  }

  // we can now proceed
  if (CreateSymbolicLink(M.linkpath16, M.target16, flags)) {
    return 0;
  } else {
    return __fix_enotdir(-1, M.linkpath16);
  }
}
