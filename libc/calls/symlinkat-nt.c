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
#include "libc/calls/internal.h"
#include "libc/errno.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/tokeninformationclass.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/privilege.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/luid.h"
#include "libc/nt/struct/tokenprivileges.h"
#include "libc/sysv/errfuns.h"

static bool g_can_symlink;

textwindows int sys_symlinkat_nt(const char *target, int newdirfd,
                                 const char *linkpath) {
  uint32_t flags;
  char16_t target16[PATH_MAX];
  char16_t linkpath16[PATH_MAX];
  if (!g_can_symlink) return eacces();
  flags = isdirectory(target) ? kNtSymbolicLinkFlagDirectory : 0;
  if (__mkntpathat(newdirfd, linkpath, 0, linkpath16) == -1) return -1;
  if (__mkntpath(target, target16) == -1) return -1;
  if (CreateSymbolicLink(linkpath16, target16, flags)) {
    return 0;
  } else {
    return __winerr();
  }
}

static textstartup bool EnableSymlink(void) {
  int64_t tok;
  struct NtLuid id;
  struct NtTokenPrivileges tp;
  if (!OpenProcessToken(GetCurrentProcess(), kNtTokenAllAccess, &tok)) return 0;
  if (!LookupPrivilegeValue(0, u"SeCreateSymbolicLinkPrivilege", &id)) return 0;
  tp.PrivilegeCount = 1;
  tp.Privileges[0].Luid = id;
  tp.Privileges[0].Attributes = kNtSePrivilegeEnabled;
  if (!AdjustTokenPrivileges(tok, 0, &tp, sizeof(tp), 0, 0)) return 0;
  return GetLastError() != kNtErrorNotAllAssigned;
}

static textstartup void g_can_symlink_init() {
  g_can_symlink = EnableSymlink();
}

const void *const g_can_symlink_ctor[] initarray = {
    g_can_symlink_init,
};
