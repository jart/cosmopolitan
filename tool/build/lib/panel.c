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
#include "tool/build/lib/panel.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/str/unicode.h"
#include "tool/build/lib/buffer.h"

static int tpdecode(const char *s, wint_t *out) {
  uint32_t wc, cb, need, msb, j, i = 0;
  if ((wc = s[i++] & 255) == -1) return -1;
  while ((wc & 0300) == 0200) {
    if ((wc = s[i++] & 255) == -1) return -1;
  }
  if (!(0 <= wc && wc <= 0x7F)) {
    msb = wc < 252 ? _bsr(~wc & 0xff) : 1;
    need = 7 - msb;
    wc &= ((1u << msb) - 1) | 0003;
    for (j = 1; j < need; ++j) {
      if ((cb = s[i++] & 255) == -1) return -1;
      if ((cb & 0300) == 0200) {
        wc = wc << 6 | (cb & 077);
      } else {
        if (out) *out = 0xFFFD;
        return -1;
      }
    }
  }
  if (out) *out = wc;
  return i;
}

/**
 * Renders panel div flex boxen inside terminal display for tui.
 *
 * You can use all the UNICODE and ANSI escape sequences you want.
 *
 * @param fd is file descriptor
 * @param pn is number of panels
 * @param p is panel list in logically sorted order
 * @param tyn is terminal height in cells
 * @param txn is terminal width in cells
 * @return -1 w/ errno if an error happened
 * @see nblack's notcurses project too!
 */
ssize_t PrintPanels(int fd, long pn, struct Panel *p, long tyn, long txn) {
  wint_t wc;
  ssize_t rc;
  struct Buffer b, *l;
  int x, y, i, j, width;
  enum { kUtf8, kAnsi, kAnsiCsi } state;
  bzero(&b, sizeof(b));
  AppendStr(&b, "\e[H");
  for (y = 0; y < tyn; ++y) {
    if (y) AppendFmt(&b, "\e[%dH", y + 1);
    for (x = i = 0; i < pn; ++i) {
      if (p[i].top <= y && y < p[i].bottom) {
        j = state = 0;
        l = &p[i].lines[y - p[i].top];
        while (x < p[i].left) {
          AppendChar(&b, ' ');
          x += 1;
        }
        AppendFmt(&b, "\e[%d;%dH", y + 1, x + 1);  // bsd utf-8 :(
        while (x < p[i].right || j < l->i) {
          wc = '\0';
          width = 0;
          if (j < l->i) {
            wc = l->p[j];
            switch (state) {
              case kUtf8:
                switch (wc & 0xff) {
                  case '\e':
                    state = kAnsi;
                    ++j;
                    break;
                  default:
                    j += abs(tpdecode(l->p + j, &wc));
                    if (x < p[i].right) {
                      width = max(0, wcwidth(wc));
                    } else {
                      wc = 0;
                    }
                    break;
                }
                break;
              case kAnsi:
                switch (wc & 0xff) {
                  case '[':
                    state = kAnsiCsi;
                    ++j;
                    break;
                  case '@':
                  case ']':
                  case '^':
                  case '_':
                  case '\\':
                  case 'A' ... 'Z':
                    state = kUtf8;
                    ++j;
                    break;
                  default:
                    state = kUtf8;
                    continue;
                }
                break;
              case kAnsiCsi:
                switch (wc & 0xff) {
                  case ':':
                  case ';':
                  case '<':
                  case '=':
                  case '>':
                  case '?':
                  case '0' ... '9':
                    ++j;
                    break;
                  case '`':
                  case '~':
                  case '^':
                  case '@':
                  case '[':
                  case ']':
                  case '{':
                  case '}':
                  case '_':
                  case '|':
                  case '\\':
                  case 'A' ... 'Z':
                  case 'a' ... 'z':
                    state = kUtf8;
                    ++j;
                    break;
                  default:
                    state = kUtf8;
                    continue;
                }
                break;
              default:
                __builtin_unreachable();
            }
            if (x > p[i].right) {
              break;
            }
          } else if (x < p[i].right) {
            wc = ' ';
            width = 1;
          }
          if (wc) {
            x += width;
            AppendWide(&b, wc);
          }
        }
      }
    }
  }
  rc = WriteBuffer(&b, fd);
  free(b.p);
  return rc;
}
