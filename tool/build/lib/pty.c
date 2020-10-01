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
#include "libc/alg/arraylist2.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/conv/conv.h"
#include "libc/conv/itoa.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/unicode/unicode.h"
#include "libc/x/x.h"
#include "tool/build/lib/pty.h"

struct MachinePty *MachinePtyNew(void) {
  struct MachinePty *pty;
  pty = xcalloc(1, sizeof(struct MachinePty));
  MachinePtyResize(pty, 25, 80);
  return pty;
}

void MachinePtyResize(struct MachinePty *pty, int yn, int xn) {
  unsigned y, ym, xm, y0;
  uint32_t *wcs, *fgs, *bgs, *prs;
  if (xn < 80) xn = 80;
  if (yn < 25) yn = 25;
  if (xn == pty->xn && yn == pty->yn) return;
  wcs = xcalloc(yn * xn, 4);
  fgs = xcalloc(yn * xn, 4);
  bgs = xcalloc(yn * xn, 4);
  prs = xcalloc(yn * xn, 4);
  y0 = yn > pty->y + 1 ? 0 : pty->y + 1 - yn;
  ym = MIN(yn, pty->yn) + y0;
  xm = MIN(xn, pty->xn);
  for (y = y0; y < ym; ++y) {
    memcpy(wcs + y * xn, pty->wcs + y * pty->xn, xm * 4);
    memcpy(fgs + y * xn, pty->fgs + y * pty->xn, xm * 4);
    memcpy(bgs + y * xn, pty->bgs + y * pty->xn, xm * 4);
    memcpy(prs + y * xn, pty->prs + y * pty->xn, xm * 4);
  }
  free(pty->wcs);
  free(pty->fgs);
  free(pty->bgs);
  free(pty->prs);
  pty->wcs = wcs;
  pty->fgs = fgs;
  pty->bgs = bgs;
  pty->prs = prs;
  pty->y = MIN(pty->y, yn - 1);
  pty->x = MIN(pty->x, xn - 1);
  pty->yn = yn;
  pty->xn = xn;
}

void MachinePtyFree(struct MachinePty *pty) {
  if (pty) {
    free(pty->wcs);
    free(pty->fgs);
    free(pty->bgs);
    free(pty->prs);
    free(pty);
  }
}

static void MachinePtyScrollPlane(struct MachinePty *pty, uint32_t *p) {
  memcpy(p, p + pty->xn, sizeof(p[0]) * pty->xn * (pty->yn - 1));
  memset(p + pty->xn * (pty->yn - 1), 0, sizeof(p[0]) * pty->xn);
}

static void MachinePtyScroll(struct MachinePty *pty) {
  MachinePtyScrollPlane(pty, pty->wcs);
  MachinePtyScrollPlane(pty, pty->fgs);
  MachinePtyScrollPlane(pty, pty->bgs);
  MachinePtyScrollPlane(pty, pty->prs);
}

static void MachinePtyNewline(struct MachinePty *pty) {
  pty->x = 0;
  if (++pty->y == pty->yn) {
    --pty->y;
    if (!(pty->conf & kMachinePtyNoopost)) {
      MachinePtyScroll(pty);
    }
  }
}

static void SetMachinePtyCell(struct MachinePty *pty, wint_t wc) {
  uint32_t w, i;
  if ((w = MAX(0, wcwidth(wc))) > 0) {
    i = pty->y * pty->xn + pty->x;
    pty->wcs[i] = wc;
    pty->fgs[i] = pty->fg;
    pty->bgs[i] = pty->bg;
    pty->prs[i] = pty->pr;
    if ((pty->x += w) >= pty->xn) {
      pty->x = 0;
      MachinePtyNewline(pty);
    }
  }
}

static void MachinePtyTab(struct MachinePty *pty) {
  unsigned x, x2;
  x2 = pty->x + ROUNDUP(pty->x + 1, 8);
  if (x2 >= pty->xn) x2 = pty->xn - 1;
  for (x = pty->x; x < x2; ++x) {
    pty->wcs[pty->y * pty->xn + x] = 0;
  }
  pty->x = x2;
}

static void MachinePtyCursorSet(struct MachinePty *pty, int y, int x) {
  pty->y = MAX(0, MIN(pty->yn - 1, y));
  pty->x = MAX(0, MIN(pty->xn - 1, x));
}

static void MachinePtyCursorMove(struct MachinePty *pty, int dy, int dx) {
  int n;
  if (pty->esc.i > 1) {
    n = atoi(pty->esc.s);
    dy *= n;
    dx *= n;
  }
  MachinePtyCursorSet(pty, pty->y + dy, pty->x + dx);
}

static void MachinePtyCursorPosition(struct MachinePty *pty) {
  int row, col;
  row = MAX(1, atoi(pty->esc.s));
  col = MAX(1, atoi((char *)firstnonnull(strchr(pty->esc.s, ';'), "x") + 1));
  MachinePtyCursorSet(pty, row - 1, col - 1);
}

static void MachinePtyEraseRange(struct MachinePty *pty, size_t i, size_t j) {
  size_t n;
  n = (j - i) * sizeof(uint32_t);
  memset(pty->wcs + i, 0, n);
  memset(pty->fgs + i, 0, n);
  memset(pty->bgs + i, 0, n);
  memset(pty->prs + i, 0, n);
}

static void MachinePtyEraseDisplay(struct MachinePty *pty) {
  switch (atoi(pty->esc.s)) {
    case 3:
    case 2:
      pty->y = 0;
      pty->x = 0;
    case 0:
      MachinePtyEraseRange(pty, pty->y * pty->xn + pty->x, pty->yn * pty->xn);
      break;
    case 1:
      MachinePtyEraseRange(pty, 0, pty->y * pty->xn + pty->x);
      break;
    default:
      break;
  }
}

static void MachinePtyEraseLine(struct MachinePty *pty) {
  switch (atoi(pty->esc.s)) {
    case 0:
      MachinePtyEraseRange(pty, pty->y * pty->xn + pty->x,
                           pty->y * pty->xn + pty->xn);
      break;
    case 1:
      MachinePtyEraseRange(pty, pty->y * pty->xn + pty->xn,
                           pty->y * pty->xn + pty->x);
      break;
    case 2:
      MachinePtyEraseRange(pty, pty->y * pty->xn, pty->y * pty->xn + pty->xn);
      break;
    default:
      break;
  }
}

static void MachinePtySelectGraphicsRendition(struct MachinePty *pty) {
  char *p, c;
  unsigned x;
  uint8_t code[4];
  enum {
    kSgr,
    kSgrFg = 010,
    kSgrFgTrue = 012,
    kSgrFgXterm = 015,
    kSgrBg = 020,
    kSgrBgTrue = 022,
    kSgrBgXterm = 025,
  } t;
  x = 0;
  t = kSgr;
  p = pty->esc.s;
  memset(code, 0, sizeof(code));
  for (;;) {
    c = *p++;
    switch (c) {
      case '\0':
        return;
      case '0' ... '9':
        x *= 10;
        x += c - '0';
        break;
      case ';':
      case 'm':
        code[code[3]] = x;
        x = 0;
        switch (t) {
          case kSgr:
            switch (code[0]) {
              case 0:
                pty->fg = 0;
                pty->bg = 0;
                pty->pr = 0;
                break;
              case 1:
                pty->pr |= kMachinePtyBold;
                break;
              case 21:
                pty->pr &= ~kMachinePtyBold;
                break;
              case 2:
                pty->pr |= kMachinePtyFaint;
                break;
              case 22:
                pty->pr &= ~kMachinePtyFaint;
                break;
              case 7:
                pty->pr |= kMachinePtyFlip;
                break;
              case 27:
                pty->pr &= ~kMachinePtyFlip;
                break;
              case 90 ... 97:
                code[0] -= 90 - 30;
                code[0] += 8;
              case 30 ... 37:
                pty->fg = code[0] - 30;
                pty->pr |= kMachinePtyFg;
                pty->pr &= ~kMachinePtyTrue;
                break;
              case 38:
                t = kSgrFg;
                break;
              case 39:
                pty->pr &= ~kMachinePtyFg;
                break;
              case 100 ... 107:
                code[0] -= 100 - 40;
                code[0] += 8;
              case 40 ... 47:
                pty->bg = code[0] - 40;
                pty->pr |= kMachinePtyBg;
                pty->pr &= ~kMachinePtyTrue;
                break;
              case 48:
                t = kSgrBg;
                break;
              case 49:
                pty->pr &= ~kMachinePtyBg;
                break;
              default:
                break;
            }
            break;
          case kSgrFg:
          case kSgrBg:
            switch (code[0]) {
              case 2:
              case 5:
                t += code[0];
                break;
              default:
                t = kSgr;
                break;
            }
            break;
          case kSgrFgTrue:
            if (++code[3] == 3) {
              code[3] = 0;
              t = kSgr;
              pty->fg = READ32LE(code);
              pty->pr |= kMachinePtyFg;
              pty->pr |= kMachinePtyTrue;
            }
            break;
          case kSgrBgTrue:
            if (++code[3] == 3) {
              code[3] = 0;
              t = kSgr;
              pty->bg = READ32LE(code);
              pty->pr |= kMachinePtyBg;
              pty->pr |= kMachinePtyTrue;
            }
            break;
          case kSgrFgXterm:
            t = kSgr;
            pty->fg = code[0];
            pty->pr |= kMachinePtyFg;
            pty->pr &= ~kMachinePtyTrue;
            break;
          case kSgrBgXterm:
            t = kSgr;
            pty->bg = code[0];
            pty->pr |= kMachinePtyBg;
            pty->pr &= ~kMachinePtyTrue;
            break;
          default:
            abort();
        }
        break;
      default:
        break;
    }
  }
}

static void MachinePtyHideCursor(struct MachinePty *pty) {
  pty->conf |= kMachinePtyNocursor;
}

static void MachinePtyShowCursor(struct MachinePty *pty) {
  pty->conf &= ~kMachinePtyNocursor;
}

static void MachinePtyReportCursorPosition(struct MachinePty *pty) {
  char *p;
  char buf[2 + 10 + 1 + 10 + 1];
  p = buf;
  *p++ = '\e';
  *p++ = '[';
  p += uint64toarray_radix10((pty->y + 1) & 0xffffffff, p);
  *p++ = ';';
  p += uint64toarray_radix10((pty->x + 1) & 0xffffffff, p);
  *p++ = 'R';
  CONCAT(&pty->input.p, &pty->input.i, &pty->input.n, buf, p - buf);
}

static void MachinePtyCsiN(struct MachinePty *pty) {
  switch (atoi(pty->esc.s)) {
    case 6:
      MachinePtyReportCursorPosition(pty);
      break;
    default:
      break;
  }
}

static void MachinePtyCsiScrollUp(struct MachinePty *pty) {
  int n;
  n = atoi(pty->esc.s);
  n = MAX(1, n);
  while (n--) {
    MachinePtyScroll(pty);
  }
}

static void MachinePtyCsi(struct MachinePty *pty) {
  switch (pty->esc.s[pty->esc.i - 1]) {
    case 'A':
      MachinePtyCursorMove(pty, -1, +0);
      break;
    case 'B':
      MachinePtyCursorMove(pty, +1, +0);
      break;
    case 'C':
      MachinePtyCursorMove(pty, +0, +1);
      break;
    case 'D':
      MachinePtyCursorMove(pty, +0, -1);
      break;
    case 'f':
    case 'H':
      MachinePtyCursorPosition(pty);
      break;
    case 'J':
      MachinePtyEraseDisplay(pty);
      break;
    case 'K':
      MachinePtyEraseLine(pty);
      break;
    case 'm':
      MachinePtySelectGraphicsRendition(pty);
      break;
    case 'n':
      MachinePtyCsiN(pty);
      break;
    case 'S':
      MachinePtyCsiScrollUp(pty);
      break;
    case 'l':
      if (strcmp(pty->esc.s, "?25l") == 0) {
        MachinePtyHideCursor(pty);
      }
      break;
    case 'h':
      if (strcmp(pty->esc.s, "?25h") == 0) {
        MachinePtyShowCursor(pty);
      }
      break;
    default:
      break;
  }
}

static void MachinePtyEsc(struct MachinePty *pty) {
}

static void MachinePtyEscAppend(struct MachinePty *pty, char c) {
  pty->esc.i = MIN(pty->esc.i + 1, ARRAYLEN(pty->esc.s) - 1);
  pty->esc.s[pty->esc.i - 1] = c;
  pty->esc.s[pty->esc.i - 0] = '\0';
}

ssize_t MachinePtyWrite(struct MachinePty *pty, const void *data, size_t n) {
  int i;
  const uint8_t *p;
  for (p = data, i = 0; i < n; ++i) {
    switch (pty->state) {
      case kMachinePtyAscii:
        if (p[i] < 0b10000000) {
          switch (p[i]) {
            case '\e':
              pty->state = kMachinePtyEsc;
              pty->esc.i = 0;
              break;
            case '\t':
              MachinePtyTab(pty);
              break;
            case '\r':
              pty->x = 0;
              break;
            case '\n':
              MachinePtyNewline(pty);
              break;
            case 0177:
            case '\b':
              pty->x = MAX(0, pty->x - 1);
              break;
            case '\f':
              break;
            case '\a':
              break;
            default:
              SetMachinePtyCell(pty, p[i]);
              break;
          }
        } else if (!ThomPikeCont(p[i])) {
          pty->state = kMachinePtyUtf8;
          pty->u8 = ThomPikeByte(p[i]);
          pty->n8 = ThomPikeLen(p[i]) - 1;
        }
        break;
      case kMachinePtyUtf8:
        if (ThomPikeCont(p[i])) {
          pty->u8 <<= 6;
          pty->u8 |= p[i] & 0b00111111;
          if (--pty->n8) {
            break;
          }
        }
        SetMachinePtyCell(pty, pty->u8);
        pty->state = kMachinePtyAscii;
        pty->u8 = 0;
        --i;
        break;
      case kMachinePtyEsc:
        if (p[i] == '[') {
          pty->state = kMachinePtyCsi;
        } else if (0x30 <= p[i] && p[i] <= 0x7e) {
          MachinePtyEscAppend(pty, p[i]);
          MachinePtyEsc(pty);
          pty->state = kMachinePtyAscii;
        } else if (0x20 <= p[i] && p[i] <= 0x2f) {
          MachinePtyEscAppend(pty, p[i]);
        } else {
          pty->state = kMachinePtyAscii;
        }
        break;
      case kMachinePtyCsi:
        MachinePtyEscAppend(pty, p[i]);
        switch (p[i]) {
          case ':':
          case ';':
          case '<':
          case '=':
          case '>':
          case '?':
          case '0' ... '9':
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
            MachinePtyCsi(pty);
            pty->state = kMachinePtyAscii;
            break;
          default:
            pty->state = kMachinePtyAscii;
            continue;
        }
        break;
      default:
        abort();
    }
  }
  return n;
}

ssize_t MachinePtyWriteInput(struct MachinePty *pty, const void *data,
                             size_t n) {
  const char *p = data;
  CONCAT(&pty->input.p, &pty->input.i, &pty->input.n, data, n);
  if (!(pty->conf & kMachinePtyNoecho)) {
    MachinePtyWrite(pty, data, n);
  }
  return n;
}

ssize_t MachinePtyRead(struct MachinePty *pty, void *buf, size_t size) {
  char *p;
  size_t n;
  n = MIN(size, pty->input.i);
  if (!(pty->conf & kMachinePtyNocanon)) {
    if ((p = memchr(pty->input.p, '\n', n))) {
      n = MIN(n, pty->input.p - p + 1);
    } else {
      n = 0;
    }
  }
  memcpy(buf, pty->input.p, n);
  memcpy(pty->input.p, pty->input.p + n, pty->input.i - n);
  pty->input.i -= n;
  return n;
}

void MachinePtyAppendLine(struct MachinePty *pty, struct Buffer *buf,
                          unsigned y) {
  bool atcursor;
  uint32_t x, i, fg, bg, pr, wc, w;
  if (y >= pty->yn) return;
  for (fg = bg = pr = x = 0; x < pty->xn; x += w) {
    i = y * pty->xn + x;
    wc = pty->wcs[i];
    w = MAX(0, wcwidth(wc));
    atcursor = y == pty->y && x == pty->x;
    if ((w && atcursor) ||
        (pty->prs[i] != pr || pty->fgs[i] != fg || pty->bgs[i] != bg)) {
      fg = pty->fgs[i];
      bg = pty->bgs[i];
      pr = pty->prs[i];
      AppendStr(buf, "\e[0");
      if (w && atcursor) {
        pr ^= kMachinePtyFlip;
      }
      if (pr & kMachinePtyBold) AppendStr(buf, ";1");
      if (pr & kMachinePtyFaint) AppendStr(buf, ";2");
      if (pr & kMachinePtyBlink) AppendStr(buf, ";5");
      if (pr & kMachinePtyFlip) AppendStr(buf, ";7");
      if (pr & kMachinePtyFg) {
        if (pr & kMachinePtyTrue) {
          AppendFmt(buf, ";38;2;%d;%d;%d", (fg & 0x0000ff) >> 000,
                    (fg & 0x00ff00) >> 010, (fg & 0xff0000) >> 020);
        } else {
          AppendFmt(buf, ";38;5;%d", fg);
        }
      }
      if (pr & kMachinePtyBg) {
        if (pr & kMachinePtyTrue) {
          AppendFmt(buf, ";48;2;%d;%d;%d", (bg & 0x0000ff) >> 000,
                    (bg & 0x00ff00) >> 010, (bg & 0xff0000) >> 020);
        } else {
          AppendFmt(buf, ";48;5;%d", bg);
        }
      }
      AppendStr(buf, "m");
    }
    if (w) {
      AppendWide(buf, wc);
    } else {
      w = 1;
      if (atcursor) {
        if (!(pty->conf & kMachinePtyNocursor)) {
          if (pty->conf & kMachinePtyBlinkcursor) {
            AppendStr(buf, "\e[5m");
          }
          AppendWide(buf, u'▂');
          if (pty->conf & kMachinePtyBlinkcursor) {
            AppendStr(buf, "\e[25m");
          }
        }
      } else {
        AppendChar(buf, ' ');
      }
    }
  }
  AppendStr(buf, "\e[0m");
}
