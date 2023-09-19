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
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/str/tpdecodecb.internal.h"
#include "libc/str/utf16.h"
#include "libc/sysv/errfuns.h"

#define READ                 \
  ({                         \
    int c = callback(arg);   \
    if (c != -1) ++consumed; \
    c;                       \
  })

/**
 * String / file / stream decoder.
 *
 * This scanf implementation is able to tokenize strings containing
 * 8-bit through 128-bit integers (with validation), floating point
 * numbers, etc. It can also be used to convert UTF-8 to UTF-16/32.
 *
 *   - `%d`  parses integer
 *   - `%ms` parses string allocating buffer assigning pointer
 *
 * @param callback supplies UTF-8 characters using -1 sentinel
 * @param fmt is a computer program embedded inside a c string, written
 *     in a domain-specific programming language that, by design, lacks
 *     Turing-completeness
 * @param va points to the variadic argument state
 * @see libc/fmt/pflink.h (dynamic memory is not a requirement)
 */
int __vcscanf(int callback(void *),    //
              int unget(int, void *),  //
              void *arg,               //
              const char *fmt,         //
              va_list va) {
  struct FreeMe {
    struct FreeMe *next;
    void *ptr;
  } *freeme = NULL;
  const unsigned char *p = (const unsigned char *)fmt;
  int *n_ptr;
  int items = 0;
  int consumed = 0;
  unsigned i = 0;
  int c = READ;
  for (;;) {
    switch (p[i++]) {
      case '\0':
        if (c != -1 && unget) {
          unget(c, arg);
        }
        goto Done;
      case ' ':
      case '\t':
      case '\n':
      case '\r':
      case '\v':
        while (isspace(c)) {
          c = READ;
        }
        break;
      case '%': {
        uint128_t number;
        void *buf;
        size_t bufsize;
        unsigned width = 0;
        unsigned char bits = 32;
        unsigned char charbytes = sizeof(char);
        unsigned char diglet;
        unsigned char base;
        unsigned char prefix;
        bool rawmode = false;
        bool issigned = false;
        bool ismalloc = false;
        bool isneg = false;
        bool thousands = false;
        bool discard = false;
        for (;;) {
          switch (p[i++]) {
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
              width *= 10;
              width += p[i - 1] - '0';
              break;
            case '*':
              discard = true;
              break;
            case 'm':
              ismalloc = true;
              break;
            case 'c':
              rawmode = true;
              if (!width) width = 1;
              // fallthrough
            case 's':
              while (isspace(c)) {
                c = READ;
              }
              goto DecodeString;
            case '\'':
              thousands = true;
              break;
            case 'j':  // j=64-bit jj=128-bit
              if (bits < 64) {
                bits = 64;
              } else {
                bits = 128;
              }
              break;
            case 'l':  // long
            case 'L':  // loooong
              charbytes = sizeof(wchar_t);
              // fallthrough
            case 't':  // ptrdiff_t
            case 'Z':  // size_t
            case 'z':  // size_t
              bits = 64;
              break;
            case 'h':  // short and char
              charbytes = sizeof(char16_t);
              bits >>= 1;
              break;
            case 'b':  // binary
              base = 2;
              prefix = 'b';
              goto ConsumeBasePrefix;
            case 'p':  // pointer (NexGen32e)
              bits = 48;
              // fallthrough
            case 'x':
            case 'X':  // hexadecimal
              base = 16;
              prefix = 'x';
              goto ConsumeBasePrefix;
            case 'o':  // octal
              base = 8;
              goto HandleNumber;
            case 'n':
              goto ReportConsumed;
            case 'd':  // decimal
              issigned = true;
              // fallthrough
            case 'u':
              base = 10;
            HandleNumber:
              while (isspace(c)) {
                c = READ;
              }
              if (c == '+' || (isneg = c == '-')) {
                c = READ;
              }
              goto DecodeNumber;
            case 'i':  // flexidecimal
              while (isspace(c)) {
                c = READ;
              }
              if (c == '+' || (isneg = c == '-')) {
                c = READ;
              }
              if (c == '0') {
                c = READ;
                if (c == 'x' || c == 'X') {
                  c = READ;
                  base = 16;
                } else if (c == 'b' || c == 'B') {
                  c = READ;
                  base = 2;
                } else if ('0' <= c && c <= '7') {
                  base = 8;
                } else {
                  number = 0;
                  goto GotNumber;
                }
              } else {
                base = 10;
              }
              goto DecodeNumber;
            default:
              items = einval();
              goto Done;
          }
        }
      ConsumeBasePrefix:
        while (isspace(c)) {
          c = READ;
        }
        if (c == '+' || (isneg = c == '-')) {
          c = READ;
        }
        if (c == '0') {
          c = READ;
          if (c == prefix || c == prefix + ('a' - 'A')) {
            c = READ;
          } else if (c == -1) {
            number = 0;
            goto GotNumber;
          }
        }
      DecodeNumber:
        if (c != -1 && (1 <= kBase36[(unsigned char)c] &&
                        kBase36[(unsigned char)c] <= base)) {
          number = 0;
          width = !width ? bits : width;
          do {
            diglet = kBase36[(unsigned char)c];
            if (1 <= diglet && diglet <= base) {
              width -= 1;
              number *= base;
              number += diglet - 1;
            } else if (thousands && diglet == ',') {
              // ignore
            } else {
              break;
            }
          } while ((c = READ) != -1 && width > 0);
        GotNumber:
          if (!discard) {
            uint128_t bane = (uint128_t)1 << (bits - 1);
            if (!(number & ~((bane - 1) | (issigned ? 0 : bane))) ||
                (issigned && number == bane)) {
              ++items;
            } else {
              items = erange();
              goto Done;
            }
            if (issigned && isneg) {
              number = ~number + 1;
            }
            void *out = va_arg(va, void *);
            switch (bits) {
              case sizeof(uint128_t) * CHAR_BIT:
                *(uint128_t *)out = number;
                break;
              case 48:
              case 64:
                *(uint64_t *)out = (uint64_t)number;
                break;
              case 32:
                *(uint32_t *)out = (uint32_t)number;
                break;
              case 16:
                *(uint16_t *)out = (uint16_t)number;
                break;
              case 8:
              default:
                *(uint8_t *)out = (uint8_t)number;
                break;
            }
          } else if (!items && c == -1) {
            items = -1;
            goto Done;
          }
        } else if (c == -1 && !items) {
          items = -1;
          goto Done;
        } else {
          if (c != -1 && unget) {
            unget(c, arg);
          }
          goto Done;
        }
        continue;
      ReportConsumed:
        n_ptr = va_arg(va, int *);
        *n_ptr = consumed - 1;  // minus lookahead
        continue;
      DecodeString:
        bufsize = !width ? 32 : rawmode ? width : width + 1;
        if (discard) {
          buf = NULL;
        } else if (ismalloc) {
          buf = malloc(bufsize * charbytes);
          struct FreeMe *entry;
          if (buf && (entry = calloc(1, sizeof(struct FreeMe)))) {
            entry->ptr = buf;
            entry->next = freeme;
            freeme = entry;
          }
        } else {
          buf = va_arg(va, void *);
        }
        if (buf) {
          size_t j = 0;
          for (;;) {
            if (ismalloc && !width && j + 2 + 1 >= bufsize &&
                !__grow(&buf, &bufsize, charbytes, 0)) {
              width = bufsize - 1;
            }
            if (c != -1 && j + !rawmode < bufsize && (rawmode || !isspace(c))) {
              if (charbytes == 1) {
                ((unsigned char *)buf)[j++] = (unsigned char)c;
                c = READ;
              } else if (tpdecodecb((wint_t *)&c, c, (void *)callback, arg) !=
                         -1) {
                if (charbytes == sizeof(char16_t)) {
                  unsigned w = EncodeUtf16(c);
                  do {
                    if ((j + 1) * 2 < bufsize) {
                      ((char16_t *)buf)[j++] = w;
                    }
                  } while ((w >>= 16));
                } else {
                  ((wchar_t *)buf)[j++] = (wchar_t)c;
                }
                c = READ;
              }
            } else {
              if (!j && c == -1 && !items) {
                items = -1;
                goto Done;
              } else if (!rawmode && j < bufsize) {
                if (charbytes == sizeof(char)) {
                  ((unsigned char *)buf)[j] = '\0';
                } else if (charbytes == sizeof(char16_t)) {
                  ((char16_t *)buf)[j] = u'\0';
                } else if (charbytes == sizeof(wchar_t)) {
                  ((wchar_t *)buf)[j] = L'\0';
                }
              }
              break;
            }
          }
          ++items;
          if (ismalloc) {
            *va_arg(va, char **) = buf;
          }
        } else {
          do {
            if (isspace(c)) break;
          } while ((c = READ) != -1);
        }
        break;
      }
      default:
      NonDirectiveCharacter:
        c = (c == p[i - 1]) ? READ : -1;
        break;
    }
  }
Done:
  while (freeme) {
    struct FreeMe *entry = freeme;
    freeme = entry->next;
    if (items == -1) free(entry->ptr);
    free(entry);
  }
  return items;
}
