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
#include "libc/assert.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/limits.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/symboliclink.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

struct Memory {
  char16_t target[PATH_MAX];
  char16_t linkpath[PATH_MAX];
  char16_t resolved[PATH_MAX];
};

textwindows static void *sys_symlinkat_nt_malloc(size_t size) {
  return HeapAlloc(GetProcessHeap(), 0, size);
}

textwindows static void sys_symlinkat_nt_free(void *ptr) {
  HeapFree(GetProcessHeap(), 0, ptr);
}

textwindows static uint32_t sys_symlinkat_nt_flags(const char16_t *target,
                                                   bool bLooksLikeDir) {
  uint32_t attrs;
  if ((attrs = GetFileAttributes(target)) != -1u) {
    if (attrs & kNtFileAttributeDirectory)
      return kNtSymbolicLinkFlagDirectory |
             kNtSymbolicLinkFlagAllowUnprivilegedCreate;
  } else {
    // broken symlinks aren't right, but it's legal
    if (bLooksLikeDir)
      return kNtSymbolicLinkFlagDirectory |
             kNtSymbolicLinkFlagAllowUnprivilegedCreate;
  }
  return kNtSymbolicLinkFlagAllowUnprivilegedCreate;
}

textwindows static int sys_symlinkat_nt_impl2(const char *target, int newdirfd,
                                              const char *linkpath,
                                              struct Memory *m) {

  // convert link path to dos path
  int linklen;
  if ((linklen = __mkntpathat(newdirfd, linkpath, m->linkpath)) == -1)
    return -1;

  // figure out target
  uint32_t dwFlags;
  int targetlen = strlen(target);
  bool bLooksLikeDir = targetlen && target[targetlen - 1] == '/';
  if (target[0] == '/') {
    // if target is an absolute path, we can avoid doing work
    if ((targetlen = __mkntpath(target, m->target)) == -1)
      return -1;
    dwFlags = sys_symlinkat_nt_flags(m->target, bLooksLikeDir);
  } else {
    // do simple translation of relative target
    targetlen = tprecode8to16(m->target, PATH_MAX, target).ax;
    if (targetlen >= PATH_MAX)
      return enametoolong();
    for (size_t i = 0; i < targetlen; ++i)
      if (m->target[i] == '/')
        m->target[i] = '\\';
    // check if dirname(link)/target is directory
    int resolvelen = linklen;
    // this will turn c:\foo into c:\ but c:\ will stay the same
    while (m->linkpath[resolvelen - 1] != '\\')
      --resolvelen;
    --resolvelen;
    memcpy(m->resolved, m->linkpath, resolvelen * 2);
    m->resolved[resolvelen++] = '\\';
    if (resolvelen + targetlen + 1 > PATH_MAX)
      return enametoolong();
    memcpy(m->resolved + resolvelen, m->target, targetlen * 2);
    m->resolved[resolvelen += targetlen] = 0;
    dwFlags = sys_symlinkat_nt_flags(m->resolved, bLooksLikeDir);
  }

  // create symbolic link
  if (!CreateSymbolicLink(m->linkpath, m->target, dwFlags))
    return __fix_enotdir(__winerr(), m->linkpath);

  // we're done
  return 0;
}

textwindows static int sys_symlinkat_nt_impl1(const char *target, int newdirfd,
                                              const char *linkpath) {

  // create temporary memory
  struct Memory *m;
  if (!(m = sys_symlinkat_nt_malloc(sizeof(struct Memory))))
    return enomem();

  // call implementation
  int rc = sys_symlinkat_nt_impl2(target, newdirfd, linkpath, m);
  sys_symlinkat_nt_free(m);
  return rc;
}

textwindows int sys_symlinkat_nt(const char *target, int newdirfd,
                                 const char *linkpath) {
  int rc;
  BLOCK_SIGNALS;
  rc = sys_symlinkat_nt_impl1(target, newdirfd, linkpath);
  ALLOW_SIGNALS;
  return rc;
}
