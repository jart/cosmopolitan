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
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"

struct FindComBinary {
  bool once;
  const char *res;
  char buf[PATH_MAX];
};

static struct FindComBinary g_findcombinary;

/**
 * Returns path of binary without debug information, or null.
 *
 * @return path to non-debug binary, or -1 w/ errno
 */
const char *FindComBinary(void) {
  size_t len;
  const char *p;
  if (!g_findcombinary.once) {
    g_findcombinary.once = true;
    if ((p = GetProgramExecutableName()) &&
        (len = strlen(p)) < ARRAYLEN(g_findcombinary.buf)) {
      g_findcombinary.res = memcpy(g_findcombinary.buf, p, len + 1);
      if (len > 4 && memcmp(&g_findcombinary.buf[len - 4], ".dbg", 4) == 0) {
        g_findcombinary.buf[len - 4] = '\0';
      }
    }
  }
  return g_findcombinary.res;
}
