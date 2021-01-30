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
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"

/**
 * Returns path of binary with the debug information, or null.
 *
 * @return path to debug binary, or -1 w/ errno
 */
const char *FindDebugBinary(void) {
  unsigned i, len;
  char buf[2][PATH_MAX];
  static char res[PATH_MAX];
  const char *bins[4], *pwd;
  bins[0] = program_invocation_name;
  bins[1] = (const char *)getauxval(AT_EXECFN);
  pwd = emptytonull(getenv("PWD"));
  for (i = 0; i < 2; ++i) {
    if (pwd && bins[i] && bins[i][0] != '/' && bins[i][0] != '\\' &&
        strlen(pwd) + 1 + strlen(bins[i]) + 1 <= ARRAYLEN(buf[0])) {
      strcpy(buf[i], pwd);
      strcat(buf[i], "/");
      strcat(buf[i], bins[i]);
      bins[i + 2] = buf[i];
    }
  }
  for (i = 0; i < 4; ++i) {
    if (!bins[i]) continue;
    len = strlen(bins[i]);
    memcpy(res, bins[i], len + 1);
    if (!endswith(res, ".dbg") && len + 3 + 1 <= ARRAYLEN(res)) {
      strcat(res, ".dbg");
    }
    if (fileexists(res)) {
      return res;
    }
  }
  errno = ENOENT;
  return NULL;
}
