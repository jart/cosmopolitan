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
#include "libc/calls/ntmagicpaths.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/systeminfo.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

static inline bool IsSlash(char c) {
  return c == '/' || c == '\\';
}

static inline int IsAlpha(int c) {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

textwindows static const char *FixNtMagicPath(const char *path,
                                              unsigned flags) {
  const struct NtMagicPaths *mp = &kNtMagicPaths;
  asm("" : "+r"(mp));
  if (!IsSlash(path[0])) return path;
  if (strcmp(path, mp->devtty) == 0) {
    if ((flags & O_ACCMODE) == O_RDONLY) {
      return mp->conin;
    } else if ((flags & O_ACCMODE) == O_WRONLY) {
      return mp->conout;
    }
  }
  if (strcmp(path, mp->devnull) == 0) return mp->nul;
  if (strcmp(path, mp->devstdin) == 0) return mp->conin;
  if (strcmp(path, mp->devstdout) == 0) return mp->conout;
  return path;
}

textwindows int __mkntpath(const char *path,
                           char16_t path16[hasatleast PATH_MAX]) {
  return __mkntpath2(path, path16, -1);
}

/**
 * Copies path for Windows NT.
 *
 * This function does the following chores:
 *
 * 1. Converting UTF-8 to UTF-16
 * 2. Replacing forward-slashes with backslashes
 * 3. Fixing drive letter paths, e.g. `/c/` → `c:\`
 * 4. Add `\\?\` prefix for paths exceeding 260 chars
 * 5. Remapping well-known paths, e.g. `/dev/null` → `NUL`
 *
 * @param flags is used by open()
 * @param path16 is shortened so caller can prefix, e.g. \\.\pipe\, and
 *     due to a plethora of special-cases throughout the Win32 API
 * @return short count excluding NUL on success, or -1 w/ errno
 * @error ENAMETOOLONG
 */
textwindows int __mkntpath2(const char *path,
                            char16_t path16[hasatleast PATH_MAX], int flags) {
  /*
   * 1. Need +1 for NUL-terminator
   * 2. Need +1 for UTF-16 overflow
   * 3. Need ≥2 for SetCurrentDirectory trailing slash requirement
   * 4. Need ≥13 for mkdir() i.e. 1+8+3+1, e.g. "\\ffffffff.xxx\0"
   *    which is an "8.3 filename" from the DOS days
   */
  const char *q;
  bool isdospath;
  char16_t c, *p;
  size_t i, j, n, m, x, z;
  if (!path) return efault();
  path = FixNtMagicPath(path, flags);
  p = path16;
  q = path;

  if (IsSlash(q[0]) && IsAlpha(q[1]) && IsSlash(q[2])) {
    z = MIN(32767, PATH_MAX);
    // turn "\c\foo" into "\\?\c:\foo"
    p[0] = '\\';
    p[1] = '\\';
    p[2] = '?';
    p[3] = '\\';
    p[4] = q[1];
    p[5] = ':';
    p[6] = '\\';
    p += 7;
    q += 3;
    z -= 7;
    x = 7;
  } else if (IsSlash(q[0]) && IsAlpha(q[1]) && !q[2]) {
    z = MIN(32767, PATH_MAX);
    // turn "\c" into "\\?\c:\"
    p[0] = '\\';
    p[1] = '\\';
    p[2] = '?';
    p[3] = '\\';
    p[4] = q[1];
    p[5] = ':';
    p[6] = '\\';
    p += 7;
    q += 2;
    z -= 7;
    x = 7;
  } else if (IsAlpha(q[0]) && q[1] == ':' && IsSlash(q[2])) {
    z = MIN(32767, PATH_MAX);
    // turn "c:\foo" into "\\?\c:\foo"
    p[0] = '\\';
    p[1] = '\\';
    p[2] = '?';
    p[3] = '\\';
    p[4] = q[0];
    p[5] = ':';
    p[6] = '\\';
    p += 7;
    q += 3;
    z -= 7;
    x = 7;
  } else if (IsSlash(q[0]) && IsSlash(q[1]) && q[2] == '?' && IsSlash(q[3])) {
    z = MIN(32767, PATH_MAX);
    x = 0;
  } else {
    z = MIN(260, PATH_MAX);
    x = 0;
  }

  // turn /tmp into GetTempPath()
  if (!x && IsSlash(q[0]) && q[1] == 't' && q[2] == 'm' && q[3] == 'p' &&
      (IsSlash(q[4]) || !q[4])) {
    m = GetTempPath(z, p);
    if (!q[4]) return m;
    q += 5;
    p += m;
    z -= m;
  } else {
    m = 0;
  }

  // turn utf-8 into utf-16
  n = tprecode8to16(p, z, q).ax;
  if (n >= z - 1) {
    STRACE("path too long for windows: %#s", path);
    return enametoolong();
  }

  // 1. turn `/` into `\`
  // 2. turn `\\` into `\` if not at beginning
  for (j = i = 0; i < n; ++i) {
    c = p[i];
    if (c == '/') {
      c = '\\';
    }
    if (j > 1 && c == '\\' && p[j - 1] == '\\') {
      continue;
    }
    p[j++] = c;
  }
  p[j] = 0;
  n = j;

  // our path is now stored at `path16` with length `n`
  n = x + m + n;

  // To avoid toil like this:
  //
  //     CMD.EXE was started with the above path as the current directory.
  //     UNC paths are not supported.  Defaulting to Windows directory.
  //     Access is denied.
  //
  // Remove \\?\ prefix if we're within 260 character limit.
  if (n > 4 && n < 260 &&   //
      path16[0] == '\\' &&  //
      path16[1] == '\\' &&  //
      path16[2] == '?' &&   //
      path16[3] == '\\') {
    memmove(path16, path16 + 4, (n - 4 + 1) * sizeof(char16_t));
    n -= 4;
  }

  return n;
}
