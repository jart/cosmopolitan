/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/escape/escape.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/paland.inc"
#include "libc/fmt/palandprintf.h"
#include "libc/nexgen32e/tinystrlen.h"
#include "libc/str/internal.h"
#include "libc/str/str.h"
#include "libc/unicode/unicode.h"

forceinline unsigned long tpiencode(wint_t wc) {
  char buf[8];
  memset(buf, 0, sizeof(buf));
  tpencode(buf, sizeof(buf), wc, false);
  return read64le(buf);
}

forceinline int emitwc(int out(int, void *), void *arg, unsigned flags,
                       wint_t wc) {
  unsigned long pending;
  if (flags & FLAGS_QUOTE) {
    if (wc > 127) {
      pending = tpiencode(wc);
    } else {
      pending = cescapec(wc);
    }
  } else {
    pending = tpiencode(wc);
  }
  do {
    if (out(pending & 0xff, arg) == -1) return -1;
  } while ((pending >>= 8));
  return 0;
}

forceinline int emitquote(int out(int, void *), void *arg, unsigned flags,
                          char ch, unsigned char signbit) {
  if (flags & FLAGS_REPR) {
    if (signbit == 63) {
      if (out('L', arg) == -1) return -1;
    } else if (signbit == 15) {
      if (out('u', arg) == -1) return -1;
    }
    if (out(ch, arg) == -1) return -1;
  }
  return 0;
}

/**
 * Converts string to array.
 *
 * This function is used by palandprintf() to implement the %s and %c
 * directives. The content outputted to the array is always UTF-8, but
 * the input may be UTF-16 or UTF-32.
 */
int stoa(int out(int, void *), void *arg, void *data, unsigned long flags,
         unsigned long precision, unsigned long width, unsigned char signbit,
         unsigned char qchar) {
  char *p;
  wint_t wc;
  unsigned w, c;
  bool ignorenul;

  p = data;
  if (!p) {
    p = ((flags & FLAGS_REPR) ? "NULL" : "(null)");
    flags &= ~FLAGS_PRECISION;
    flags |= FLAGS_NOQUOTE;
    signbit = 0;
  } else {
    if (emitquote(out, arg, flags, qchar, signbit) == -1) return -1;
  }

  w = precision ? precision : -1;
  if (width) {
    if (signbit == 63) {
      if (weaken(wcsnwidth)) {
        w = weaken(wcsnwidth)((const wchar_t *)p, w);
      } else {
        w = tinywcsnlen((const wchar_t *)p, w);
      }
    } else if (signbit == 15) {
      if (weaken(strnwidth16)) {
        w = weaken(strnwidth16)((const char16_t *)p, w);
      } else {
        w = tinystrnlen16((const char16_t *)p, w);
      }
    } else if (weaken(strnwidth)) {
      w = weaken(strnwidth)(p, w);
    } else {
      w = strnlen(p, w);
    }
  }

  if (flags & FLAGS_PRECISION) {
    w = MIN(w, precision);
  }

  if (w < width && !(flags & FLAGS_LEFT)) {
    if (spacepad(out, arg, width - w) == -1) return -1;
  }

  ignorenul = (flags & FLAGS_PRECISION) && (flags & (FLAGS_HASH | FLAGS_QUOTE));
  for (; !(flags & FLAGS_PRECISION) || precision; --precision) {
    if (signbit == 15) {
      if ((wc = *(const char16_t *)p) || ignorenul) {
        if ((1 <= wc && wc <= 0xD7FF)) {
          p += sizeof(char16_t);
        } else if ((wc & UTF16_MASK) == UTF16_CONT) {
          p += sizeof(char16_t);
          continue;
        } else {
          char16_t buf[4] = {wc};
          if (!(flags & FLAGS_PRECISION) || precision > 1) {
            buf[1] = ((const char16_t *)p)[1];
          }
          p += max(1, getutf16((const char16_t *)p, &wc)) * sizeof(char16_t);
        }
      } else {
        break;
      }
    } else if (signbit == 63) {
      wc = *(const wint_t *)p;
      p += sizeof(wint_t);
      if (!wc) break;
    } else if (flags & FLAGS_HASH) {
      c = *p & 0xff;
      if (!c && !ignorenul) break;
      wc = (*weaken(kCp437))[c];
      p++;
    } else {
      if ((wc = *p & 0xff) || ignorenul) {
        if (1 <= wc && wc <= 0x7f) {
          ++p;
        } else if (iscont(wc & 0xff)) {
          ++p;
          continue;
        } else {
          char buf[8];
          memset(buf, 0, sizeof(buf));
          memcpy(buf, p,
                 !(flags & FLAGS_PRECISION) ? 7 : MIN(7, precision - 1));
          p += max(1, tpdecode(p, &wc));
        }
      } else {
        break;
      }
    }
    if (emitwc(out, arg, flags, wc) == -1) return -1;
  }

  if (w <= width && (flags & FLAGS_LEFT)) {
    if (spacepad(out, arg, width - w) == -1) return -1;
  }
  if (!(flags & FLAGS_NOQUOTE) && (flags & FLAGS_REPR)) {
    if (out(qchar, arg) == -1) return -1;
  }

  return 0;
}
