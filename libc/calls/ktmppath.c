/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/nt/systeminfo.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

/**
 * RII constant holding temporary file directory.
 *
 * The order of precedence is:
 *
 *   - $TMPDIR/
 *   - GetTempPath()
 *   - /tmp/
 *
 * This guarantees trailing slash.
 * We also guarantee `kTmpPath` won't be longer than `PATH_MAX / 2`.
 */
char kTmpPath[PATH_MAX];

static inline int IsAlpha(int c) {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

__attribute__((__constructor__)) static void kTmpPathInit(void) {
  int i;
  char *s;
  uint32_t n;
  char16_t path16[PATH_MAX];

  if ((s = getenv("TMPDIR")) && (n = strlen(s)) < PATH_MAX / 2) {
    memcpy(kTmpPath, s, n);
    if (n && kTmpPath[n - 1] != '/') {
      kTmpPath[n + 0] = '/';
      kTmpPath[n + 1] = 0;
    }
    return;
  }

  if (IsWindows() &&
      ((n = GetTempPath(ARRAYLEN(path16), path16)) && n < ARRAYLEN(path16))) {
    // turn c:\foo\bar\ into c:/foo/bar/
    for (i = 0; i < n; ++i) {
      if (path16[i] == '\\') {
        path16[i] = '/';
      }
    }
    // turn c:/... into /c/...
    if (IsAlpha(path16[0]) && path16[1] == ':' && path16[2] == '/') {
      path16[1] = path16[0];
      path16[0] = '/';
      path16[2] = '/';
    }
    tprecode16to8(kTmpPath, sizeof(kTmpPath), path16);
    return;
  }

  strcpy(kTmpPath, "/tmp/");
}
