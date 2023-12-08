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
#include "libc/str/str.h"

/**
 * Returns nonzero if c is space character.
 *
 * We define this as invisible characters which move the cursor. That
 * means `\t\r\n\f\v` and unicodes whose category begins with `Z` but
 * not ogham since it's not invisible and non-breaking spaces neither
 * since they're not invisible to emacs users.
 */
int iswspace(wint_t c) {
  switch (c) {
    case '\t':    // CHARACTER TABULATION
    case '\n':    // LINE FEED
    case '\f':    // FORM FEED
    case '\v':    // LINE TABULATION
    case '\r':    // CARRIAGE RETURN
    case ' ':     // SPACE
    case 0x2000:  // EN QUAD (Zs)
    case 0x2001:  // EM QUAD (Zs)
    case 0x2002:  // EN SPACE (Zs)
    case 0x2003:  // EM SPACE (Zs)
    case 0x2004:  // THREE-PER-EM SPACE (Zs)
    case 0x2005:  // FOUR-PER-EM SPACE (Zs)
    case 0x2006:  // SIX-PER-EM SPACE (Zs)
    case 0x2007:  // FIGURE SPACE (Zs)
    case 0x2008:  // PUNCTUATION SPACE (Zs)
    case 0x2009:  // THIN SPACE (Zs)
    case 0x200a:  // HAIR SPACE (Zs)
    case 0x2028:  // LINE SEPARATOR (Zl)
    case 0x2029:  // PARAGRAPH SEPARATOR (Zp)
    case 0x205f:  // MEDIUM MATHEMATICAL SPACE (Zs)
    case 0x3000:  // IDEOGRAPHIC SPACE (Zs)
      return 1;
    default:
      return 0;
  }
}

__weak_reference(iswspace, iswspace_l);
