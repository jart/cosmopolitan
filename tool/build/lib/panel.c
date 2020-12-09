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
#include "libc/bits/safemacros.internal.h"
#include "libc/fmt/conv.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/str/tpdecode.internal.h"
#include "libc/unicode/unicode.h"
#include "tool/build/lib/buffer.h"
#include "tool/build/lib/panel.h"

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
  size_t wrote;
  struct Buffer b, *l;
  int x, y, i, j, width;
  enum { kUtf8, kAnsi, kAnsiCsi } state;
  memset(&b, 0, sizeof(b));
  AppendStr(&b, "\e[H");
  for (y = 0; y < tyn; ++y) {
    if (y) AppendStr(&b, "\r\n");
    for (x = i = 0; i < pn; ++i) {
      if (p[i].top <= y && y < p[i].bottom) {
        j = state = 0;
        l = &p[i].lines[y - p[i].top];
        while (x < p[i].left) {
          AppendChar(&b, ' ');
          x += 1;
        }
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
                unreachable;
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
