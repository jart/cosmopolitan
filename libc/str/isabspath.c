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
#include "libc/dce.h"
#include "libc/str/str.h"

/**
 * Returns true if pathname is absolute, e.g.
 *
 * - `/home/jart/foo.txt` is absolute
 * - `C:/Users/jart/foo.txt` is absolute on NT
 * - `C:\Users\jart\foo.txt` is absolute on NT
 * - `\??\C:\Users\jart\foo.txt` is absolute on NT
 * - `\\.\C:\Users\jart\foo.txt` is absolute on NT
 * - `/Users/jart/foo.txt` we consider it absolute enough on NT
 * - `\Users\jart\foo.txt` we consider it absolute enough on NT
 *
 * Please note that the recommended approach to using Cosmopolitan is to
 * not use absolute paths at all. If you do use absolute paths then it's
 * a good idea on Windows to stay within the C: drive. This is because
 * Cosmopolitan Libc doesn't create a virtual filesystem layer and
 * instead just replaces `\` characters with `/`.
 */
bool _isabspath(const char *p) {
  if (*p == '/') {
    return true;
  }
  if (IsWindows() &&
      (*p == '/' || *p == '\\' || (isalpha(p[0]) && p[1] == ':'))) {
    return true;
  }
  return false;
}
