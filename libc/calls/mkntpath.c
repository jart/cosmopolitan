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
#include "libc/str/oldutf16.internal.h"
#include "libc/str/str.h"
#include "libc/str/tpdecode.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

textwindows static const char *FixNtMagicPath(const char *path,
                                              unsigned flags) {
  const struct NtMagicPaths *mp = &kNtMagicPaths;
  asm("" : "+r"(mp));
  if (path[0] != '/') return path;
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
  size_t i, n;
  path = FixNtMagicPath(path, flags);
  n = tprecode8to16(path16, PATH_MAX - 16, path).ax;
  if (n == PATH_MAX - 16 - 1) return enametoolong();
  for (i = 0; i < n; ++i) {
    if (path16[i] == '/') {
      path16[i] = '\\';
    }
  }
  return n;
}
