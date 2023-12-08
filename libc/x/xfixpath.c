/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/x/x.h"

static inline int IsAlpha(int c) {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

/**
 * Fixes $PATH environment variable on Windows.
 */
void xfixpath(void) {
  size_t i;
  char *p, *path;
  path = strdup(getenv("PATH"));
  if (strstr(path, "C:\\") && strstr(path, ";")) {

    // turn backslash into slash
    for (p = path; *p; ++p) {
      if (*p == '\\') *p = '/';
    }

    // turn c:/... into /c/...
    if (IsAlpha(path[0]) && path[1] == ':' && path[2] == '/') {
      path[1] = path[0];
      path[0] = '/';
    }
    for (p = path, i = 0; p[i]; ++i) {
      if (p[i + 0] == ';' && IsAlpha(p[i + 1]) && p[i + 2] == ':' &&
          p[i + 3] == '/') {
        p[i + 2] = p[i + 1];
        p[i + 1] = '/';
      }
    }

    // turn semicolon into colon
    for (p = path; *p; ++p) {
      if (*p == ';') *p = ':';
    }

    setenv("PATH", path, true);
  }
  free(path);
}
