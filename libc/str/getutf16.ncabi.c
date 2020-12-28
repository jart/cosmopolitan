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
#include "libc/errno.h"
#include "libc/str/str.h"
#include "libc/str/utf16.h"

/* TODO: DELETE */

/**
 * Decodes UTF-16 character.
 *
 * @param s is a NUL-terminated string
 * @return number of bytes (NUL counts as 1) or -1 w/ errno
 * @note synchronization is performed to skip leading continuations;
 *     canonicalization and validation are performed to some extent
 * @todo delete
 */
forcealignargpointer unsigned(getutf16)(const char16_t *p, wint_t *wc) {
  unsigned skip = 0;
  while (IsUtf16Cont(p[skip])) skip++;
  if (IsUcs2(p[skip])) {
    *wc = p[skip];
    return skip + 1;
  } else if (!IsUtf16Cont(p[skip + 1])) {
    *wc = INVALID_CODEPOINT;
    return -1;
  } else {
    *wc = MergeUtf16(p[skip], p[skip + 1]);
    return skip + 2;
  }
}
