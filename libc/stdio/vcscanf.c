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
#include "libc/ctype.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdckdint.h"
#include "libc/str/str.h"
#include "libc/str/tab.h"
#include "libc/str/thompike.h"
#include "libc/str/unicode.h"
#include "libc/str/utf16.h"
#include "libc/wctype.h"

#define READ             \
  ({                     \
    int c;               \
    if (width) {         \
      --width;           \
      c = callback(arg); \
      if (c != -1)       \
        ++consumed;      \
    } else {             \
      c = -1;            \
    }                    \
    c;                   \
  })

#define BUFFER                \
  ({                          \
    int c = READ;             \
    if (c != -1)              \
      if (!Buffer(&fpbuf, c)) \
        goto OutOfMemory;     \
    c;                        \
  })

#define UNBUFFER              \
  ({                          \
    if (c != -1) {            \
      --consumed;             \
      unget(c, arg);          \
      fpbuf.p[--fpbuf.n] = 0; \
    }                         \
  })

struct Buf {
  int n;
  int c;
  char *p;
};

static bool Buffer(struct Buf *b, char c) {
  if (b->n == b->c) {
    int c;
    char *p;
    size_t t;
    if (ckd_add(&c, b->c, 1) ||           //
        ckd_add(&c, c, c >> 1) ||         //
        ckd_mul(&t, c, sizeof(*b->p)) ||  //
        !(p = realloc(b->p, t)))
      return false;
    b->c = c;
    b->p = p;
  }
  b->p[b->n++] = c;
  return true;
}

struct Range {
  wint_t a;
  unsigned n;
};

struct Set {
  bool v;
  bool u;
  short n;
  short c;
  struct Range *p;
};

static bool CharsetHas(const struct Set *cs, wint_t wc) {
  for (int i = 0; i < cs->n; ++i)
    if (wc - cs->p[i].a <= cs->p[i].n)
      return !cs->v;
  return cs->v;
}

static bool CharsetAdd(struct Set *cs, wint_t wc) {
  if (cs->n == cs->c) {
    short c;
    size_t b;
    struct Range *p;
    if (ckd_add(&c, cs->c, 1) ||           //
        ckd_add(&c, c, c >> 1) ||          //
        ckd_mul(&b, c, sizeof(*cs->p)) ||  //
        !(p = realloc(cs->p, b)))
      return false;
    cs->c = c;
    cs->p = p;
  }
  cs->u |= !isascii(wc);
  cs->p[cs->n].a = wc;
  cs->p[cs->n].n = 0;
  ++cs->n;
  return true;
}

static int IsSpace(int c) {
  return c == ' ' ||   //
         c == '\t' ||  //
         c == '\r' ||  //
         c == '\n' ||  //
         c == '\f' ||  //
         c == '\v';
}

static void ConsumeWhitespace(int callback(void *), int unget(int, void *),
                              void *arg, unsigned long *consumed) {
  int c;
  for (;;) {
    if ((c = callback(arg)) == -1)
      break;
    if (IsSpace(c)) {
      ++*consumed;
    } else {
      unget(c, arg);
      break;
    }
  }
}

/**
 * Decodes string.
 *
 * This function implements scanf(), sscanf(), etc. for cosmopolitan.
 *
 * All standard features are supported, except for positional argument
 * references (e.g. `%n$`) specified by POSIX.1-2001 (but not ISO C99).
 * This implementation behaves more similarly to glibc than musl when it
 * comes to the handling of erroneous inputs.
 *
 * Like glibc, the `%b` specifier may be used to parse binary numbers,
 * and the '%S' specifier means `%ls`.
 *
 * You may use `%jjd` or `%Ld` to parse `int128_t`.
 *
 * You may use `%hs` to write `char16_t` strings.
 *
 * You may use `%y` to parse base36 integers.
 */
int __vcscanf(int callback(void *),           //
              int unget(int, void *),         //
              wint_t ungetw(wint_t, void *),  //
              void *arg,                      //
              const char *fmt,                //
              va_list va) {
  unsigned long consumed = 0;
  struct Set charset = {0};
  struct Buf fpbuf = {0};
  int items = 0;
  int fc;
  int c;

  for (;;) {
    switch ((fc = *fmt++ & 255)) {
      case '\0':
        goto Done;

      default: {
      NonDirectiveCharacter:
        if ((c = callback(arg)) == -1)
          goto Eof;
        ++consumed;
        if (c != fc)
          goto Mismatch;
        break;
      }

      case ' ':
      case '\t':
      case '\n':
      case '\r':
      case '\v':
        ConsumeWhitespace(callback, unget, arg, &consumed);
        break;

      case '%': {
        union {
          float f;
          double d;
          long double l;
        } fp;
        char *buf;
        size_t j, cap;
        uint128_t number;
        char base_prefix;
        unsigned width = -1;
        unsigned char bits = 32;
        unsigned char charbytes = 1;
        unsigned char base = 10;
        char thousands_sep = 0;
        bool ismalloc = false;
        bool rawmode = false;
        bool isneg = false;
        bool discard = false;
        bool gotsome = false;
        bool match = false;

        for (;;) {
          switch ((fc = *fmt++ & 255)) {
            case '%':  // %% → %
              goto NonDirectiveCharacter;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
              if (width == -1)
                width = 0;
              width *= 10;
              width += fc - '0';
              break;

            case '*':
              discard = true;
              break;

            case 'm':
              ismalloc = true;
              break;

            case '\'':
              thousands_sep = ',';
              break;

            case 'c':  // character
              rawmode = true;
              if (width == -1)
                width = 1;
              goto DecodeString;

            case 'S':  // wide string
              charbytes = sizeof(wchar_t);
              // fallthrough

            case 's':  // string
              ConsumeWhitespace(callback, unget, arg, &consumed);
              goto DecodeString;

            case 'j':  // j=64-bit jj=128-bit
              if (bits < 64) {
                bits = 64;
              } else {
                bits = 128;
              }
              break;

            case 'l':  // long
              charbytes = sizeof(wchar_t);
              bits = 64;
              break;

            case 'q':
            case 'L':  // longer
              bits = 128;
              break;

            case 't':  // ptrdiff_t
            case 'Z':  // size_t
            case 'z':  // size_t
              bits = 64;
              break;

            case 'h':  // short and char
              charbytes = sizeof(char16_t);
              if (bits == 32) {
                bits = 16;
              } else {
                bits = 8;
              }
              break;

            case 'b':  // binary
              base = 2;
              base_prefix = 'b';
              goto ConsumeBasePrefix;

            case 'p':  // pointer
              bits = 64;
              // fallthrough

            case 'x':
            case 'X':  // hexadecimal
              base = 16;
              base_prefix = 'x';
            ConsumeBasePrefix:
              for (;;) {
                c = READ;
                if (IsSpace(c)) {
                  ++width;
                } else {
                  break;
                }
              }
              if (c == '+' || (isneg = c == '-'))
                c = READ;
              if (c == '0') {
                gotsome = true;
                c = READ;
                if (tolower(c) == base_prefix)
                  c = READ;
              }
              goto DecodeNumber;

            case 'o':  // octal
              base = 8;
              goto HandleNumber;

            case 'y':  // base36
              base = 36;
              goto HandleNumber;

            case 'd':  // signed decimal
            case 'u':  // unsigned decimal
              base = 10;
            HandleNumber:
              for (;;) {
                c = READ;
                if (IsSpace(c)) {
                  ++width;
                } else {
                  break;
                }
              }
              if (c == '+' || (isneg = c == '-'))
                c = READ;
              if (c == -1 && !width)
                goto Mismatch;
              goto DecodeNumber;

            case 'i':  // flexidecimal
              for (;;) {
                c = READ;
                if (IsSpace(c)) {
                  ++width;
                } else {
                  break;
                }
              }
              if (c == '+' || (isneg = c == '-'))
                c = READ;
              if (c == -1 && !width)
                goto Mismatch;
              if (c == '0') {
                gotsome = true;
                c = READ;
                if (c == 'x' || c == 'X') {
                  base = 16;
                  c = READ;
                } else if (c == 'b' || c == 'B') {
                  base = 2;
                  c = READ;
                } else {
                  base = 8;
                }
              } else {
                base = 10;
              }
              goto DecodeNumber;

            case 'a':
            case 'A':
            case 'e':
            case 'E':
            case 'f':
            case 'F':
            case 'g':
            case 'G':  // floating point number
              goto DecodeFloat;

            case 'n':  // report
              number = consumed;
              goto WriteNumber;

            case '[': {  // charset
              charset.v = 0;
              charset.u = 0;
              charset.n = 0;
              if (*fmt == '^') {
                ++fmt;
                charset.v = true;
              }
              if (*fmt == ']') {
                ++fmt;
                if (!CharsetAdd(&charset, L']'))
                  goto OutOfMemory;
              }
              int t = 0;
              for (;;) {
                wint_t x;
                if (!(x = *fmt++ & 255))
                  goto InvalidArgument;
                if (x == ']')
                  break;
                if (x == '-' && t) {
                  t = 2;
                  continue;
                }
                if (x >= 0300) {
                  int n = ThomPikeLen(x);
                  x = ThomPikeByte(x);
                  while (--n) {
                    wint_t y = *fmt++ & 255;
                    if (!ThomPikeCont(y))
                      goto InvalidArgument;
                    x = ThomPikeMerge(x, y);
                  }
                  charset.u = true;
                }
                if (t == 2) {
                  x -= charset.p[charset.n - 1].a;
                  charset.p[charset.n - 1].n = x;
                  t = 0;
                } else {
                  if (!CharsetAdd(&charset, x))
                    goto OutOfMemory;
                  t = 1;
                }
              }
              match = true;
              goto DecodeString;
            }

            default:
              goto InvalidArgument;
          }
        }

      DecodeNumber:
        number = 0;
        if (!gotsome) {
          if (c == -1) {
            if (width) {
              goto Eof;
            } else {
              goto Mismatch;
            }
          }
          if (!(1 <= kBase36[c] && kBase36[c] <= base))
            goto Mismatch;
        }
        while (c != -1) {
          char diglet = kBase36[c];
          if (1 <= diglet && diglet <= base) {
            number *= base;
            number += diglet - 1;
          } else if (thousands_sep && c == thousands_sep) {
            // do nothing
          } else {
            --consumed;
            unget(c, arg);
            break;
          }
          c = READ;
        }
        if (!discard) {
          ++items;
          if (isneg)
            number = -number;
        WriteNumber:
          switch (bits) {
            case 128:
              *va_arg(va, uint128_t *) = number;
              break;
            case 64:
              *va_arg(va, uint64_t *) = number;
              break;
            default:
              *va_arg(va, uint32_t *) = number;
              break;
            case 16:
              *va_arg(va, uint16_t *) = number;
              break;
            case 8:
              *va_arg(va, uint8_t *) = number;
              break;
          }
        }
        continue;

      DecodeString:
        if (discard) {
          buf = 0;
          cap = 0;
          ismalloc = false;
        } else if (ismalloc) {
          cap = 16;
          if (!(buf = malloc(cap * charbytes)))
            goto OutOfMemory;
        } else {
          if (!(buf = va_arg(va, char *)))
            goto Mismatch;
          cap = width + 1;
        }
        j = 0;
        // when the output string is utf-16 or utf-32, then we need to
        // decode the input string as utf-8. however, when we write to
        // a byte array output, we try very hard to avoid interpreting
        // the input as utf-8 since we don't know if the user is using
        // scanf to read binary. the only time we force a roundtrip in
        // utf-8 parser is if the user has specified a character class
        // containing non-ascii characters specified in the format str
        bool should_decode_utf8 = charbytes > 1 || (match && charset.u);
        for (;;) {
          wint_t a, b;
          int n, l = 0;
          if (!width)
            goto DecodeStringEof;
          if ((c = callback(arg)) == -1)
            goto DecodeStringEof;
          ++consumed;
          ++l;
          if (should_decode_utf8 && c >= 0300) {
            a = ThomPikeByte(c);
            n = ThomPikeLen(c) - 1;
            do {
              if ((b = callback(arg)) == -1)
                goto DecodeStringIllegalSequence;
              ++consumed;
              ++l;
              if (!ThomPikeCont(b))
                goto DecodeStringIllegalSequence;
              a = ThomPikeMerge(a, b);
            } while (--n);
            c = a;
          }
          --width;
          if (match) {
            if (!CharsetHas(&charset, c)) {
              if (should_decode_utf8) {
                ungetw(c, arg);
              } else {
                unget(c, arg);
              }
              consumed -= l;
              goto DecodeStringMismatch;
            }
          } else if (!rawmode) {
            if (IsSpace(c)) {
              unget(c, arg);
              consumed -= l;
              goto DecodeStringMismatch;
            }
          }
          if (ismalloc && j + 8 >= cap) {
            char *buf2;
            cap += 8;
            cap += cap >> 1;
            if (!(buf2 = realloc(buf, cap * charbytes)))
              goto DecodeStringEof;
            buf = buf2;
          }
          if (buf) {
            if (charbytes == 1) {
              if (!should_decode_utf8) {
                buf[j++] = c;
              } else {
                uint64_t w = tpenc(c);
                do {
                  buf[j++] = w;
                } while ((w >>= 8));
              }
            } else if (charbytes == sizeof(char16_t)) {
              if (c < 0x10000u) {
                ((char16_t *)buf)[j++] = c;
              } else if (c <= 0x10FFFDu) {
                ((char16_t *)buf)[j++] = ((c - 0x10000) >> 10) + 0xD800;
                ((char16_t *)buf)[j++] = ((c - 0x10000) & 1023) + 0xDC00;
              } else {
                goto DecodeStringIllegalSequence;
              }
            } else {
              ((wchar_t *)buf)[j++] = c;
            }
          }
        }
      DecodeStringFinished:
        if (buf) {
          if (!rawmode) {
            switch (charbytes) {
              case sizeof(char):
                buf[j] = 0;
                break;
              case sizeof(char16_t):
                ((char16_t *)buf)[j] = 0;
                break;
              case sizeof(wchar_t):
                ((wchar_t *)buf)[j] = 0;
                break;
              default:
                __builtin_unreachable();
            }
          }
          ++items;
        }
        if (ismalloc) {
          realloc_in_place(buf, (j + 1) * charbytes);
          *va_arg(va, char **) = buf;
        }
        buf = 0;
        break;
      DecodeStringEof:
        if (discard)
          break;
        if (!j) {
          if (ismalloc)
            free(buf);
          buf = 0;
          goto Eof;
        }
        goto DecodeStringFinished;
      DecodeStringMismatch:
        if (discard)
          break;
        if (!j) {
          if (ismalloc)
            free(buf);
          buf = 0;
          goto Mismatch;
        }
        goto DecodeStringFinished;
      DecodeStringIllegalSequence:
        errno = EILSEQ;
        if (ismalloc)
          free(buf);
        buf = 0;
        goto Mismatch;

      DecodeFloat:
        for (;;) {
          c = READ;
          if (IsSpace(c)) {
            ++width;
          } else {
            break;
          }
        }
        fpbuf.n = 0;
        if (c != -1)
          if (!Buffer(&fpbuf, c))
            goto OutOfMemory;
        if (c == '+' || c == '-')
          c = BUFFER;
        if (c == -1 && !width)
          goto Mismatch;
        bool hexadecimal = false;
        if (c == '0') {
          c = BUFFER;
          if (c == 'x' || c == 'X') {
            c = BUFFER;
            hexadecimal = true;
            goto BufferFloat;
          } else if (c == -1) {
            goto GotFloat;
          } else {
            goto BufferFloat;
          }
        } else if (c == 'n' || c == 'N') {
          c = BUFFER;
          if (c == 'a' || c == 'A') {
            c = BUFFER;
            if (c == 'n' || c == 'N') {
              c = BUFFER;
              if (c == '(') {
                c = BUFFER;
                do {
                  bool isdigit = c >= '0' && c <= '9';
                  bool isletter =
                      (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
                  if (!(c == '_' || isdigit || isletter))
                    goto Done;
                } while ((c = BUFFER) != -1 && c != ')');
                if (c == ')')
                  c = READ;
                goto GotFloat;
              } else {
                UNBUFFER;
                goto GotFloat;
              }
            } else {
              goto Done;
            }
          } else {
            goto Done;
          }
        } else if (c == 'i' || c == 'I') {
          c = BUFFER;
          if (c == 'n' || c == 'N') {
            c = BUFFER;
            if (c == 'f' || c == 'F') {
              c = BUFFER;
              if (c == 'i' || c == 'I') {
                c = BUFFER;
                if (c == 'n' || c == 'N') {
                  c = BUFFER;
                  if (c == 'i' || c == 'I') {
                    c = BUFFER;
                    if (c == 't' || c == 'T') {
                      c = BUFFER;
                      if (c == 'y' || c == 'Y') {
                        c = BUFFER;
                      } else {
                        goto Done;
                      }
                    } else {
                      goto Done;
                    }
                  } else {
                    goto Done;
                  }
                } else {
                  goto Done;
                }
              } else {
                UNBUFFER;
                goto GotFloat;
              }
            } else {
              goto Done;
            }
          } else {
            goto Done;
          }
        }

      BufferFloat:;
        enum { INTEGER, FRACTIONAL, SIGN, EXPONENT } state = INTEGER;
        int exponent_start = -1;     // track where exponent parsing started
        bool did_backtrack = false;  // track if we backtracked
        do {
          bool isdecdigit = c >= '0' && c <= '9';
          bool ishexdigit = (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
          bool ispoint = c == '.' || c == ',';
          bool isdecexp = c == 'e' || c == 'E';
          bool ishexp = c == 'p' || c == 'P';
          bool issign = c == '+' || c == '-';
          switch (state) {
            case INTEGER:
            case FRACTIONAL:
              if (isdecdigit || (hexadecimal && ishexdigit)) {
                goto Continue;
              } else if (state == INTEGER && ispoint) {
                state = FRACTIONAL;
                goto Continue;
              } else if (isdecexp || (hexadecimal && ishexp)) {
                // mark position of exponent marker
                exponent_start = fpbuf.n - 1;
                state = SIGN;
                goto Continue;
              } else {
                goto Break;
              }
            case SIGN:
              if (issign) {
                state = EXPONENT;
                goto Continue;
              }
              state = EXPONENT;
              // fallthrough
            case EXPONENT:
              if (isdecdigit) {
                goto Continue;
              } else {
                // if we never got any digits in the exponent, we need
                // to decide whether to backtrack
                if (exponent_start >= 0 && fpbuf.n > exponent_start) {
                  // check if we have any digits after the exponent
                  // marker and optional sign
                  bool has_exponent_digits = false;
                  bool has_sign = false;
                  int start_check = exponent_start + 1;
                  // Check for optional sign
                  if (start_check < fpbuf.n && (fpbuf.p[start_check] == '+' ||
                                                fpbuf.p[start_check] == '-')) {
                    has_sign = true;
                    start_check++;
                  }
                  for (int i = start_check; i < fpbuf.n; i++) {
                    if (fpbuf.p[i] >= '0' && fpbuf.p[i] <= '9') {
                      has_exponent_digits = true;
                      break;
                    }
                  }
                  // handle incomplete exponents to match glibc behavior
                  if (!has_exponent_digits) {
                    if (has_sign) {
                      // if we have a sign but no digits (like "p+"),
                      // append "0" to make it "p+0"
                      if (!Buffer(&fpbuf, '0'))
                        goto OutOfMemory;
                    } else {
                      // if we have no sign and no digits (like just
                      // "p"), backtrack
                      fpbuf.n = exponent_start;
                      if (fpbuf.n >= 0)
                        fpbuf.p[fpbuf.n] = 0;
                      did_backtrack = true;
                    }
                  }
                }
                goto Break;
              }
            default:
              goto Break;
          }
        Continue:
          continue;
        Break:
          if (!did_backtrack) {
            UNBUFFER;
          } else {
            // if we backtracked, we still need to decrement consumed to
            // account for the invalid character we read but didn't keep
            if (c != -1) {
              --consumed;
              unget(c, arg);
            }
          }
          break;
        } while ((c = BUFFER) != -1);

        // if we exited the loop due to end of input, we still need to
        // handle incomplete exponent cases
        if (c == -1 && (state == EXPONENT || state == SIGN)) {
          if (exponent_start >= 0 && fpbuf.n > exponent_start) {
            // check if we have any digits after the exponent marker and
            // optional sign
            bool has_exponent_digits = false;
            bool has_sign = false;
            int start_check = exponent_start + 1;
            // Check for optional sign
            if (start_check < fpbuf.n &&
                (fpbuf.p[start_check] == '+' || fpbuf.p[start_check] == '-')) {
              has_sign = true;
              start_check++;
            }
            for (int i = start_check; i < fpbuf.n; i++) {
              if (fpbuf.p[i] >= '0' && fpbuf.p[i] <= '9') {
                has_exponent_digits = true;
                break;
              }
            }
            // handle incomplete exponents to match glibc behavior
            if (!has_exponent_digits) {
              if (has_sign) {
                // if we have a sign but no digits (like "e+" or "p+"),
                // append "0" to make it "e+0" or "p+0"
                if (!Buffer(&fpbuf, '0'))
                  goto OutOfMemory;
              } else {
                // if we have no sign and no digits (like just "e" or
                // "p"), append "0" to make it "e0" or "p0"
                if (!Buffer(&fpbuf, '0'))
                  goto OutOfMemory;
              }
            }
          }
        }

      GotFloat: {
        char *ep;
        if (!fpbuf.n) {
          if (c == -1) {
            if (width) {
              goto Eof;
            } else {
              goto Mismatch;
            }
          }
          goto Mismatch;
        }
        if (!Buffer(&fpbuf, 0))
          goto OutOfMemory;
        switch (bits) {
          case 32:
            fp.f = strtof(fpbuf.p, &ep);
            break;
          case 64:
            fp.d = strtod(fpbuf.p, &ep);
            break;
          case 128:
            fp.l = strtold(fpbuf.p, &ep);
            break;
          default:
            goto InvalidArgument;
        }
        if (ep != fpbuf.p + fpbuf.n - 1)
          goto Mismatch;
        if (!discard) {
          ++items;
          switch (bits) {
            case 32:
              *va_arg(va, float *) = fp.f;
              break;
            case 64:
              *va_arg(va, double *) = fp.d;
              break;
            case 128:
              *va_arg(va, long double *) = fp.l;
              break;
            default:
              goto InvalidArgument;
          }
        }
        continue;
      }
      }
    }
  }

Done:
Mismatch:
  if (charset.p)
    free(charset.p);
  if (fpbuf.p)
    free(fpbuf.p);
  return items;

Eof:
  if (!items)
    items = -1;
  goto Done;

OutOfMemory:
  errno = ENOMEM;
  goto Eof;

InvalidArgument:
  errno = EINVAL;
  goto Eof;
}
