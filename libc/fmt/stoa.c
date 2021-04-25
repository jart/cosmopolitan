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
#include "libc/bits/safemacros.internal.h"
#include "libc/bits/weaken.h"
#include "libc/fmt/fmts.h"
#include "libc/fmt/internal.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/nexgen32e/tinystrlen.internal.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/tpenc.h"
#include "libc/str/utf16.h"
#include "libc/unicode/unicode.h"

typedef int (*out_f)(const char *, void *, size_t);
typedef int (*emit_f)(out_f, void *, uint64_t);

static int __fmt_stoa_byte(out_f out, void *a, uint64_t c) {
  char buf[1] = {c};
  return out(buf, a, 1);
}

static int __fmt_stoa_wide(out_f out, void *a, uint64_t w) {
  char buf[8];
  if (!isascii(w)) w = tpenc(w);
  WRITE64LE(buf, w);
  return out(buf, a, w ? (bsr(w) >> 3) + 1 : 1);
}

static int __fmt_stoa_bing(out_f out, void *a, uint64_t w) {
  char buf[8];
  w = tpenc((*weaken(kCp437))[w & 0xFF]);
  WRITE64LE(buf, w);
  return out(buf, a, w ? (bsr(w) >> 3) + 1 : 1);
}

static int __fmt_stoa_quoted(out_f out, void *a, uint64_t w) {
  char buf[8];
  if (isascii(w)) {
    w = cescapec(w);
  } else {
    w = tpenc(w);
  }
  WRITE64LE(buf, w);
  return out(buf, a, w ? (bsr(w) >> 3) + 1 : 1);
}

static int __fmt_stoa_quote(out_f out, void *arg, unsigned flags, char ch,
                            unsigned char signbit) {
  if (flags & FLAGS_REPR) {
    if (signbit == 63) {
      if (out("L", arg, 1) == -1) return -1;
    } else if (signbit == 15) {
      if (out("u", arg, 1) == -1) return -1;
    }
    if (out(&ch, arg, 1) == -1) return -1;
  }
  return 0;
}

/**
 * Converts string to array.
 *
 * This is used by __fmt() to implement the %s and %c directives. The
 * content outputted to the array is always UTF-8, but the input may be
 * UTF-16 or UTF-32.
 *
 * @see __fmt()
 */
int __fmt_stoa(int out(const char *, void *, size_t), void *arg, void *data,
               unsigned long flags, unsigned long precision,
               unsigned long width, unsigned char signbit,
               unsigned char qchar) {
  wint_t wc;
  unsigned n;
  emit_f emit;
  char *p, buf[1];
  unsigned w, c, pad;
  bool justdobytes, ignorenul;

  p = data;
  if (!p) {
    p = ((flags & FLAGS_REPR) ? "NULL" : "(null)");
    signbit = 0;
    flags |= FLAGS_NOQUOTE;
    if (flags & FLAGS_PRECISION) {
      precision = min(strlen(p), precision);
    }
  } else {
    if (__fmt_stoa_quote(out, arg, flags, qchar, signbit) == -1) return -1;
  }

  ignorenul = false;
  justdobytes = false;
  if (signbit == 15 || signbit == 63) {
    if (flags & FLAGS_QUOTE) {
      emit = __fmt_stoa_quoted;
      ignorenul = flags & FLAGS_PRECISION;
    } else {
      emit = __fmt_stoa_wide;
    }
  } else if ((flags & FLAGS_HASH) && weaken(kCp437)) {
    justdobytes = true;
    emit = __fmt_stoa_bing;
    ignorenul = flags & FLAGS_PRECISION;
  } else if (flags & FLAGS_QUOTE) {
    emit = __fmt_stoa_quoted;
    ignorenul = flags & FLAGS_PRECISION;
  } else {
    justdobytes = true;
    emit = __fmt_stoa_byte;
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
        w = weaken(wcsnwidth)((const wchar_t *)p, precision, 0);
      }
    } else if (signbit == 15) {
      if (weaken(strnwidth16)) {
        w = weaken(strnwidth16)((const char16_t *)p, precision, 0);
      }
    } else if (weaken(strnwidth)) {
      w = weaken(strnwidth)(p, precision, 0);
    }
    if (w < width) {
      pad = width - w;
    }
  }

  if (pad && !(flags & FLAGS_LEFT)) {
    if (__fmt_pad(out, arg, pad) == -1) return -1;
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
    if (__fmt_pad(out, arg, pad) == -1) return -1;
  }

  if (!(flags & FLAGS_NOQUOTE) && (flags & FLAGS_REPR)) {
    buf[0] = qchar;
    if (out(buf, arg, 1) == -1) return -1;
  }

  return 0;
}
