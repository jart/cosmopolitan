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
#include "libc/calls/internal.h"
#include "libc/calls/ntmagicpaths.internal.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/nt/systeminfo.h"
#include "libc/str/oldutf16.internal.h"
#include "libc/str/str.h"
#include "libc/str/tpdecode.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

static inline bool IsSlash(char c) {
  return c == '/' || c == '\\';
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
                           char16_t path16[hasatleast PATH_MAX - 16]) {
  return __mkntpath2(path, path16, -1);
}

/**
 * Copies path for Windows NT.
 *
 * This entails (1) UTF-8 to UTF-16 conversion; (2) replacing
 * forward-slashes with backslashes; and (3) remapping several
 * well-known paths (e.g. /dev/null → NUL) for convenience.
 *
 * @param flags is used by open()
 * @param path16 is shortened so caller can prefix, e.g. \\.\pipe\, and
 *     due to a plethora of special-cases throughout the Win32 API
 * @return short count excluding NUL on success, or -1 w/ errno
 * @error ENAMETOOLONG
 */
textwindows int __mkntpath2(const char *path,
                            char16_t path16[hasatleast PATH_MAX - 16],
                            int flags) {
  /*
   * 1. Reserve +1 for NUL-terminator
   * 2. Reserve +1 for UTF-16 overflow
   * 3. Reserve ≥2 for SetCurrentDirectory trailing slash requirement
   * 4. Reserve ≥10 for CreateNamedPipe "\\.\pipe\" prefix requirement
   * 5. Reserve ≥13 for mkdir() i.e. 1+8+3+1, e.g. "\\ffffffff.xxx\0"
   */
  char *q;
  char16_t *p;
  size_t i, n, m, z;
  if (!path) return efault();
  path = FixNtMagicPath(path, flags);
  p = path16;
  q = path;
  z = PATH_MAX - 16;
  if (IsSlash(q[0]) && q[1] == 't' && q[2] == 'm' && q[3] == 'p' &&
      (IsSlash(q[4]) || !q[4])) {
    m = GetTempPath(z, p);
    if (!q[4]) return m;
    q += 5;
    p += m;
    z -= m;
  } else {
    m = 0;
  }
  n = tprecode8to16(p, z, q).ax;
  if (n == z - 1) {
    SYSDEBUG("path too long for windows: %s", path);
    return enametoolong();
  }
  for (i = 0; i < n; ++i) {
    if (p[i] == '/') {
      p[i] = '\\';
    }
  }
  return m + n;
}
