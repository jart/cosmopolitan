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
#include "libc/bits/weaken.h"
#include "libc/fmt/paland.inc"
#include "libc/fmt/palandprintf.internal.h"
#include "libc/nexgen32e/tinystrlen.internal.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/tpenc.h"
#include "libc/str/utf16.h"
#include "libc/unicode/unicode.h"

typedef int (*emit_f)(int (*)(long, void *), void *, wint_t);

static int StoaEmitByte(int f(long, void *), void *a, wint_t c) {
  return f(c, a);
}

static int StoaEmitWordEncodedString(int f(long, void *), void *a, uint64_t w) {
  do {
    if (f(w & 0xff, a) == -1) {
      return -1;
    }
  } while ((w >>= 8));
  return 0;
}

static int StoaEmitUnicode(int f(long, void *), void *a, wint_t c) {
  if (isascii(c)) {
    return f(c, a);
  } else {
    return StoaEmitWordEncodedString(f, a, tpenc(c));
  }
}

static int StoaEmitQuoted(int f(long, void *), void *a, wint_t c) {
  if (isascii(c)) {
    return StoaEmitWordEncodedString(f, a, cescapec(c));
  } else {
    return StoaEmitWordEncodedString(f, a, tpenc(c));
  }
}

static int StoaEmitVisualized(int f(long, void *), void *a, wint_t c) {
  return StoaEmitUnicode(f, a, (*weaken(kCp437))[c]);
}

static int StoaEmitQuote(int out(long, void *), void *arg, unsigned flags,
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
 *
 * @see palandprintf()
 */
int stoa(int out(long, void *), void *arg, void *data, unsigned long flags,
         unsigned long precision, unsigned long width, unsigned char signbit,
         unsigned char qchar) {
  char *p;
  wint_t wc;
  unsigned n;
  emit_f emit;
  unsigned w, c, pad;
  bool justdobytes, ignorenul;

  p = data;
  if (!p) {
    p = ((flags & FLAGS_REPR) ? "NULL" : "(null)");
    flags &= ~FLAGS_PRECISION;
    flags |= FLAGS_NOQUOTE;
    signbit = 0;
  } else {
    if (StoaEmitQuote(out, arg, flags, qchar, signbit) == -1) return -1;
  }

  ignorenul = false;
  justdobytes = false;
  if (signbit == 15 || signbit == 63) {
    if (flags & FLAGS_QUOTE) {
      emit = StoaEmitQuoted;
      ignorenul = flags & FLAGS_PRECISION;
    } else {
      emit = StoaEmitUnicode;
    }
  } else if ((flags & FLAGS_HASH) && weaken(kCp437)) {
    justdobytes = true;
    emit = StoaEmitVisualized;
    ignorenul = flags & FLAGS_PRECISION;
  } else if (flags & FLAGS_QUOTE) {
    emit = StoaEmitQuoted;
    ignorenul = flags & FLAGS_PRECISION;
  } else {
    justdobytes = true;
    emit = StoaEmitByte;
  }

  if (!(flags & FLAGS_PRECISION)) precision = -1;
  if (!(flags & FLAGS_PRECISION) || !ignorenul) {
    if (signbit == 63) {
      precision = tinywcsnlen((const wchar_t *)p, precision);
    } else if (signbit == 15) {
      precision = tinystrnlen16((const char16_t *)p, precision);
    } else {
      precision = strnlen(p, precision);
    }
  }

  pad = 0;
  if (width) {
    w = precision;
    if (signbit == 63) {
      if (weaken(wcsnwidth)) {
        w = weaken(wcsnwidth)((const wchar_t *)p, precision);
      }
    } else if (signbit == 15) {
      if (weaken(strnwidth16)) {
        w = weaken(strnwidth16)((const char16_t *)p, precision);
      }
    } else if (weaken(strnwidth)) {
      w = weaken(strnwidth)(p, precision);
    }
    if (w < width) {
      pad = width - w;
    }
  }

  if (pad && !(flags & FLAGS_LEFT)) {
    if (spacepad(out, arg, pad) == -1) return -1;
  }

  if (justdobytes) {
    while (precision--) {
      wc = *p++ & 0xff;
      if (!wc && !ignorenul) break;
      if (emit(out, arg, wc) == -1) return -1;
    }
  } else {
    while (precision--) {
      if (signbit == 15) {
        wc = *(const char16_t *)p;
        if (!wc && !ignorenul) break;
        if (IsUcs2(wc)) {
          p += sizeof(char16_t);
        } else if (IsUtf16Cont(wc)) {
          p += sizeof(char16_t);
          continue;
        } else if (!precision) {
          break;
        } else {
          --precision;
          wc = MergeUtf16(wc, *(const char16_t *)p);
        }
      } else if (signbit == 63) {
        wc = *(const wint_t *)p;
        if (!wc && !ignorenul) break;
        p += sizeof(wint_t);
        if (!wc) break;
      } else {
        wc = *p++ & 0xff;
        if (!wc && !ignorenul) break;
        if (!isascii(wc)) {
          if (ThomPikeCont(wc)) continue;
          n = ThomPikeLen(wc) - 1;
          wc = ThomPikeByte(wc);
          if (n > precision) break;
          precision -= n;
          while (n--) {
            wc = ThomPikeMerge(wc, *p++);
          }
        }
      }
      if (emit(out, arg, wc) == -1) return -1;
    }
  }

  if (pad && (flags & FLAGS_LEFT)) {
    if (spacepad(out, arg, pad) == -1) return -1;
  }

  if (!(flags & FLAGS_NOQUOTE) && (flags & FLAGS_REPR)) {
    if (out(qchar, arg) == -1) return -1;
  }

  return 0;
}
