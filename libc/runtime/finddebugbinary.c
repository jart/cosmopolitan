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
  static char buf[PATH_MAX];
  if (buf[0]) return &buf[0];
  const char *const trybins[] = {program_invocation_name,
                                 (const char *)getauxval(AT_EXECFN)};
  for (unsigned i = 0; i < ARRAYLEN(trybins); ++i) {
    const char *res = trybins[i];
    unsigned len = strlen(res);
    if (4 < len && len < sizeof(buf) - 5) {
      if (strcmp(res + len - 4, ".dbg") == 0) return res;
      /* try suffixing extension, e.g. .com → .com.dbg */
      memcpy(mempcpy(buf, res, len), ".dbg", 5);
      if (fileexists(buf)) return &buf[0];
      buf[0] = '\0';
    }
  }
  errno = ENOENT;
  return NULL;
}
