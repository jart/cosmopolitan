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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nt/systeminfo.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

static struct {
  atomic_uint once;
  union {
    char path[PATH_MAX];
    char16_t path16[PATH_MAX / 2];
  };
} __tmpdir;

static inline int IsAlpha(int c) {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

static void __tmpdir_init(void) {
  int i;
  char *s;
  uint32_t n;

  if ((s = getenv("TMPDIR"))) {
    if (*s != '/') {
      if (__getcwd(__tmpdir.path, PATH_MAX) == -1) {
        goto GiveUp;
      }
      strlcat(__tmpdir.path, "/", sizeof(__tmpdir.path));
    }
    strlcat(__tmpdir.path, s, sizeof(__tmpdir.path));
    if (strlcat(__tmpdir.path, "/", sizeof(__tmpdir.path)) <
        PATH_MAX - NAME_MAX) {
      return;
    }
  }

GiveUp:
  if (IsWindows() &&
      ((n = GetTempPath(ARRAYLEN(__tmpdir.path16), __tmpdir.path16)) &&
       n < ARRAYLEN(__tmpdir.path16))) {
    // turn c:\foo\bar\ into c:/foo/bar/
    for (i = 0; i < n; ++i) {
      if (__tmpdir.path16[i] == '\\') {
        __tmpdir.path16[i] = '/';
      }
    }
    // turn c:/... into /c/...
    if (IsAlpha(__tmpdir.path16[0]) && __tmpdir.path16[1] == ':' &&
        __tmpdir.path16[2] == '/') {
      __tmpdir.path16[1] = __tmpdir.path16[0];
      __tmpdir.path16[0] = '/';
      __tmpdir.path16[2] = '/';
    }
    tprecode16to8(__tmpdir.path, sizeof(__tmpdir.path), __tmpdir.path16);
    return;
  }

  strcpy(__tmpdir.path, "/tmp/");
}

/**
 * Returns pretty good temporary directory.
 *
 * The order of precedence is:
 *
 *   - $TMPDIR/ is always favored if defined
 *   - GetTempPath(), for the New Technology
 *   - /tmp/ to make security scene go crazy
 *
 * This guarantees an absolute path with a trailing slash. The returned
 * value points to static memory with `PATH_MAX` bytes. The string will
 * be short enough that at least `NAME_MAX` bytes remain. This function
 * is thread safe so long as callers don't modified the returned memory
 */
char *__get_tmpdir(void) {
  cosmo_once(&__tmpdir.once, __tmpdir_init);
  return __tmpdir.path;
}
