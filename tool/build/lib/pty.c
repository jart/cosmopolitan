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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/conv/conv.h"
#include "libc/conv/itoa.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/tpenc.h"
#include "libc/unicode/unicode.h"
#include "libc/x/x.h"
#include "tool/build/lib/pty.h"

/**
 * @fileoverview Pseudoteletypewriter
 *
 *   \t                                  TAB
 *   \a                                  BELL
 *   \r                                  CURSOR START
 *   \b                                  CURSOR LEFT
 *   \177                                CURSOR LEFT
 *   \n                                  CURSOR DOWN AND START IF OPOST
 *   \f                                  CURSOR DOWN AND START IF OPOST
 *   \v                                  CURSOR DOWN AND START IF OPOST
 *   \eD                                 CURSOR DOWN AND START
 *   \eE                                 CURSOR DOWN
 *   \eM                                 CURSOR UP
 *   \ec                                 FULL RESET
 *   \e7                                 SAVE CURSOR POSITION
 *   \e8                                 RESTORE CURSOR POSITION
 *   \e(0                                DEC SPECIAL GRAPHICS
 *   \e(B                                USAS X3.4-1967
 *   \e#5                                SINGLE WIDTH
 *   \e#6                                DOUBLE WIDTH
 *   \e#8                                SO MANY E
 *   \eZ → \e/Z                          REPORT IDENTITY
 *   \e[𝑛A                               CURSOR UP             [CLAMPED]
 *   \e[𝑛B                               CURSOR DOWN           [CLAMPED]
 *   \e[𝑛C                               CURSOR RIGHT          [CLAMPED]
 *   \e[𝑛D                               CURSOR LEFT           [CLAMPED]
 *   \e[𝑦;𝑥H                             SET CURSOR POSITION   [CLAMPED]
 *   \e[𝑥G                               SET CURSOR COLUMN     [CLAMPED]
 *   \e[𝑦d                               SET CURSOR ROW        [CLAMPED]
 *   \e[𝑛E                               CURSOR DOWN AND START [CLAMPED]
 *   \e[𝑛F                               CURSOR UP AND START   [CLAMPED]
 *   \e[𝑛S                               SCROLL UP
 *   \e[𝑛T                               SCROLL DOWN
 *   \e[𝑛@                               INSERT CELLS
 *   \e[𝑛P                               DELETE CELLS
 *   \e[𝑛L                               INSERT LINES
 *   \e[𝑛M                               DELETE LINES
 *   \e[J                                ERASE DISPLAY FORWARDS
 *   \e[1J                               ERASE DISPLAY BACKWARDS
 *   \e[2J                               ERASE DISPLAY
 *   \e[K                                ERASE LINE FORWARD
 *   \e[1K                               ERASE LINE BACKWARD
 *   \e[2K                               ERASE LINE
 *   \e[𝑛X                               ERASE CELLS
 *   \e[0m                               RESET
 *   \e[1m                               BOLD
 *   \e[2m                               FAINT
 *   \e[3m                               ITALIC
 *   \e[4m                               UNDER
 *   \e[5m                               BLINK
 *   \e[7m                               INVERT
 *   \e[8m                               CONCEAL
 *   \e[9m                               STRIKE
 *   \e[20m                              FRAKTUR
 *   \e[21m                              DUNDER
 *   \e[22m                              RESET BOLD & FAINT
 *   \e[23m                              RESET ITALIC & FRAKTUR
 *   \e[24m                              RESET UNDER & DUNDER
 *   \e[25m                              RESET BLINK
 *   \e[27m                              RESET INVERT
 *   \e[28m                              RESET CONCEAL
 *   \e[29m                              RESET STRIKE
 *   \e[39m                              RESET FOREGROUND
 *   \e[49m                              RESET BACKGROUND
 *   \e[30m                              BLACK FOREGROUND
 *   \e[31m                              RED FOREGROUND
 *   \e[32m                              GREEN FOREGROUND
 *   \e[33m                              YELLOW FOREGROUND
 *   \e[34m                              BLUE FOREGROUND
 *   \e[35m                              MAGENTA FOREGROUND
 *   \e[36m                              CYAN FOREGROUND
 *   \e[37m                              WHITE FOREGROUND
 *   \e[40m                              BLACK BACKGROUND
 *   \e[41m                              RED BACKGROUND
 *   \e[42m                              GREEN BACKGROUND
 *   \e[44m                              YELLOW BACKGROUND
 *   \e[44m                              BLUE BACKGROUND
 *   \e[45m                              MAGENTA BACKGROUND
 *   \e[46m                              CYAN BACKGROUND
 *   \e[47m                              WHITE BACKGROUND
 *   \e[90m                              BRIGHT BLACK FOREGROUND
 *   \e[91m                              BRIGHT RED FOREGROUND
 *   \e[92m                              BRIGHT GREEN FOREGROUND
 *   \e[93m                              BRIGHT YELLOW FOREGROUND
 *   \e[94m                              BRIGHT BLUE FOREGROUND
 *   \e[95m                              BRIGHT MAGENTA FOREGROUND
 *   \e[96m                              BRIGHT CYAN FOREGROUND
 *   \e[97m                              BRIGHT WHITE FOREGROUND
 *   \e[100m                             BRIGHT BLACK BACKGROUND
 *   \e[101m                             BRIGHT RED BACKGROUND
 *   \e[102m                             BRIGHT GREEN BACKGROUND
 *   \e[103m                             BRIGHT YELLOW BACKGROUND
 *   \e[104m                             BRIGHT BLUE BACKGROUND
 *   \e[105m                             BRIGHT MAGENTA BACKGROUND
 *   \e[106m                             BRIGHT CYAN BACKGROUND
 *   \e[107m                             BRIGHT WHITE BACKGROUND
 *   \e[38;5;𝑥m                          XTERM256 FOREGROUND
 *   \e[48;5;𝑥m                          XTERM256 BACKGROUND
 *   \e[38;2;𝑟;𝑔;𝑏m                      RGB FOREGROUND
 *   \e[48;2;𝑟;𝑔;𝑏m                      RGB BACKGROUND
 *   \e[?25h                             SHOW CURSOR
 *   \e[?25l                             HIDE CURSOR
 *   \e[s                                SAVE CURSOR POSITION
 *   \e[u                                RESTORE CURSOR POSITION
 *   \e[0q                               RESET LEDS
 *   \e[1q                               TURN ON FIRST LED
 *   \e[2q                               TURN ON SECOND LED
 *   \e[3q                               TURN ON THIRD LED
 *   \e[4q                               TURN ON FOURTH LED
 *   \e[c → \e[?1;0c                     REPORT DEVICE TYPE
 *   \e[5n → \e[0n                       REPORT DEVICE STATUS
 *   \e[6n → \e[𝑦;𝑥R                     REPORT CURSOR POSITION
 *   \e7\e[9979;9979H\e[6n\e8 → \e[𝑦;𝑥R  REPORT DISPLAY DIMENSIONS
 *
 * @see https://vt100.net/docs/vt220-rm/chapter4.html
 * @see https://invisible-island.net/xterm/
 * @see ANSI X3.64-1979
 * @see ISO/IEC 6429
 * @see FIPS-86
 * @see ECMA-48
 */

struct MachinePty *MachinePtyNew(void) {
  struct MachinePty *pty;
  pty = xcalloc(1, sizeof(struct MachinePty));
  MachinePtyResize(pty, 25, 80);
  MachinePtyFullReset(pty);
  MachinePtyErase(pty, 0, pty->yn * pty->xn);
  return pty;
}

static void MachinePtyFreePlanes(struct MachinePty *pty) {
  free(pty->wcs);
  free(pty->fgs);
  free(pty->bgs);
  free(pty->prs);
}

void MachinePtyFree(struct MachinePty *pty) {
  if (pty) {
    MachinePtyFreePlanes(pty);
    free(pty);
  }
}

static wchar_t *MachinePtyGetXlatAscii(void) {
  unsigned i;
  static bool once;
  static wchar_t xlat[128];
  if (!once) {
    for (i = 0; i < 128; ++i) {
      xlat[i] = i;
    }
    once = true;
  }
  return xlat;
}

static wchar_t *MachinePtyGetXlatLineDrawing(void) {
  unsigned i;
  static bool once;
  static wchar_t xlat[128];
  if (!once) {
    for (i = 0; i < 128; ++i) {
      if (0x5F <= i && i <= 0x7E) {
        xlat[i] = u" ◆▒␉␌␍␊°±␤␋┘┐┌└┼⎺⎻─⎼⎽├┤┴┬│≤≥π≠£·"[i - 0x5F];
      } else {
        xlat[i] = i;
      }
    }
    once = true;
  }
  return xlat;
}

static void MachinePtyXlatAlphabet(wchar_t xlat[128], int a, int b) {
  unsigned i;
  for (i = 0; i < 128; ++i) {
    if ('a' <= i && i <= 'z') {
      xlat[i] = i - 'a' + a;
    } else if ('A' <= i && i <= 'Z') {
      xlat[i] = i - 'A' + b;
    } else {
      xlat[i] = i;
    }
  }
}

static wchar_t *MachinePtyGetXlatItalic(void) {
  static bool once;
  static wchar_t xlat[128];
  if (!once) {
    MachinePtyXlatAlphabet(xlat, L'𝑎', L'𝐴');
    once = true;
  }
  return xlat;
}

static wchar_t *MachinePtyGetXlatBoldItalic(void) {
  static bool once;
  static wchar_t xlat[128];
  if (!once) {
    MachinePtyXlatAlphabet(xlat, L'𝒂', L'𝑨');
    once = true;
  }
  return xlat;
}

static wchar_t *MachinePtyGetXlatBoldFraktur(void) {
  static bool once;
  static wchar_t xlat[128];
  if (!once) {
    MachinePtyXlatAlphabet(xlat, L'𝖆', L'𝕬');
    once = true;
  }
  return xlat;
}

static wchar_t *MachinePtyGetXlatFraktur(void) {
  unsigned i;
  static bool once;
  static wchar_t xlat[128];
  if (!once) {
    for (i = 0; i < ARRAYLEN(xlat); ++i) {
      if ('A' <= i && i <= 'Z') {
        xlat[i] = L"𝔄𝔅ℭ𝔇𝔈𝔉𝔊ℌℑ𝔍𝔎𝔏𝔐𝔑𝔒𝔓𝔔ℜ𝔖𝔗𝔘𝔙𝔚𝔛𝔜ℨ"[i - 'A'];
      } else if ('a' <= i && i <= 'z') {
        xlat[i] = i - 'a' + L'𝔞';
      } else {
        xlat[i] = i;
      }
    }
    once = true;
  }
  return xlat;
}

static wchar_t *MachinePtyGetXlatDoubleWidth(void) {
  unsigned i;
  static bool once;
  static wchar_t xlat[128];
  if (!once) {
    for (i = 0; i < ARRAYLEN(xlat); ++i) {
      if ('!' <= i && i <= '~') {
        xlat[i] = -(i - '!' + L'！');
      } else {
        xlat[i] = i;
      }
    }
    once = true;
  }
  return xlat;
}

static wchar_t *MachinePtyGetXlatSgr(struct MachinePty *pty) {
  switch (!!(pty->pr & kMachinePtyFraktur) << 2 |
          !!(pty->pr & kMachinePtyItalic) << 1 |
          !!(pty->pr & kMachinePtyBold) << 0) {
    case 0b100:
    case 0b110:
      return MachinePtyGetXlatFraktur();
    case 0b101:
    case 0b111:
      return MachinePtyGetXlatBoldFraktur();
    case 0b011:
      return MachinePtyGetXlatBoldItalic();
    case 0b010:
      return MachinePtyGetXlatItalic();
    default:
      return MachinePtyGetXlatAscii();
  }
}

static void MachinePtySetXlat(struct MachinePty *pty, wchar_t *xlat) {
  pty->xlat = xlat;
  pty->pr &= ~(kMachinePtyItalic | kMachinePtyFraktur);
}

static void MachinePtySetCodepage(struct MachinePty *pty, char id) {
  unsigned i;
  switch (id) {
    default:
    case 'B':
      MachinePtySetXlat(pty, MachinePtyGetXlatAscii());
      break;
    case '0':
      MachinePtySetXlat(pty, MachinePtyGetXlatLineDrawing());
      break;
  }
}

void MachinePtyErase(struct MachinePty *pty, long dst, long n) {
  DCHECK_LE(dst + n, pty->yn * pty->xn);
  wmemset((void *)(pty->wcs + dst), ' ', n);
  wmemset((void *)(pty->prs + dst), 0, n);
}

void MachinePtyMemmove(struct MachinePty *pty, long dst, long src, long n) {
  DCHECK_LE(src + n, pty->yn * pty->xn);
  DCHECK_LE(dst + n, pty->yn * pty->xn);
  memmove(pty->wcs + dst, pty->wcs + src, n * 4);
  memmove(pty->fgs + dst, pty->fgs + src, n * 4);
  memmove(pty->bgs + dst, pty->bgs + src, n * 4);
  memmove(pty->prs + dst, pty->prs + src, n * 4);
}

void MachinePtyFullReset(struct MachinePty *pty) {
  pty->y = 0;
  pty->x = 0;
  pty->pr = 0;
  pty->u8 = 0;
  pty->n8 = 0;
  pty->conf = 0;
  pty->save = 0;
  pty->state = 0;
  pty->esc.i = 0;
  pty->input.i = 0;
  pty->xlat = MachinePtyGetXlatAscii();
  MachinePtyErase(pty, 0, pty->yn * pty->xn);
}

void MachinePtySetY(struct MachinePty *pty, int y) {
  pty->conf &= ~kMachinePtyRedzone;
  pty->y = MAX(0, MIN(pty->yn - 1, y));
}

void MachinePtySetX(struct MachinePty *pty, int x) {
  pty->conf &= ~kMachinePtyRedzone;
  pty->x = MAX(0, MIN(pty->xn - 1, x));
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
  MachinePtyFreePlanes(pty);
  pty->wcs = wcs;
  pty->fgs = fgs;
  pty->bgs = bgs;
  pty->prs = prs;
  pty->yn = yn;
  pty->xn = xn;
  MachinePtySetY(pty, pty->y);
  MachinePtySetX(pty, pty->x);
}

static void MachinePtyConcatInput(struct MachinePty *pty, const char *data,
                                  size_t n) {
  CONCAT(&pty->input.p, &pty->input.i, &pty->input.n, data, n);
}

static void MachinePtyScroll(struct MachinePty *pty) {
  MachinePtyMemmove(pty, 0, pty->xn, pty->xn * (pty->yn - 1));
  MachinePtyErase(pty, pty->xn * (pty->yn - 1), pty->xn);
}

static void MachinePtyReverse(struct MachinePty *pty) {
  MachinePtyMemmove(pty, pty->xn, 0, pty->xn * (pty->yn - 1));
  MachinePtyErase(pty, 0, pty->xn);
}

static void MachinePtyIndex(struct MachinePty *pty) {
  if (pty->y < pty->yn - 1) {
    ++pty->y;
  } else {
    MachinePtyScroll(pty);
  }
}

static void MachinePtyReverseIndex(struct MachinePty *pty) {
  if (pty->y) {
    --pty->y;
  } else {
    MachinePtyReverse(pty);
  }
}

static void MachinePtyCarriageReturn(struct MachinePty *pty) {
  MachinePtySetX(pty, 0);
}

static void MachinePtyNewline(struct MachinePty *pty) {
  MachinePtyIndex(pty);
  if (!(pty->conf & kMachinePtyNoopost)) {
    MachinePtyCarriageReturn(pty);
  }
}

static void MachinePtyAdvance(struct MachinePty *pty) {
  DCHECK_EQ(pty->xn - 1, pty->x);
  DCHECK(pty->conf & kMachinePtyRedzone);
  pty->conf &= ~kMachinePtyRedzone;
  pty->x = 0;
  if (pty->y < pty->yn - 1) {
    ++pty->y;
  } else {
    MachinePtyScroll(pty);
  }
}

static void MachinePtyWriteGlyph(struct MachinePty *pty, wint_t wc, int w) {
  uint32_t i;
  if (w < 1) wc = ' ', w = 1;
  if ((pty->conf & kMachinePtyRedzone) || pty->x + w > pty->xn) {
    MachinePtyAdvance(pty);
  }
  i = pty->y * pty->xn + pty->x;
  pty->wcs[i] = wc;
  if ((pty->prs[i] = pty->pr) & (kMachinePtyFg | kMachinePtyBg)) {
    pty->fgs[i] = pty->fg;
    pty->bgs[i] = pty->bg;
  }
  if ((pty->x += w) >= pty->xn) {
    pty->x = pty->xn - 1;
    pty->conf |= kMachinePtyRedzone;
  }
}

static void MachinePtyWriteTab(struct MachinePty *pty) {
  unsigned x, x2;
  if (pty->conf & kMachinePtyRedzone) {
    MachinePtyAdvance(pty);
  }
  x2 = MIN(pty->xn, ROUNDUP(pty->x + 1, 8));
  for (x = pty->x; x < x2; ++x) {
    pty->wcs[pty->y * pty->xn + x] = ' ';
  }
  if (x2 < pty->xn) {
    pty->x = x2;
  } else {
    pty->x = pty->xn - 1;
    pty->conf |= kMachinePtyRedzone;
  }
}

static int MachinePtyAtoi(const char *s, const char **e) {
  int i;
  for (i = 0; isdigit(*s); ++s) i *= 10, i += *s - '0';
  if (e) *e = s;
  return i;
}

static int MachinePtyGetMoveParam(struct MachinePty *pty) {
  int x = MachinePtyAtoi(pty->esc.s, NULL);
  if (x < 1) x = 1;
  return x;
}

static void MachinePtySetCursorPosition(struct MachinePty *pty) {
  int row, col;
  const char *s = pty->esc.s;
  row = max(1, MachinePtyAtoi(s, &s));
  if (*s == ';') ++s;
  col = max(1, MachinePtyAtoi(s, &s));
  MachinePtySetY(pty, row - 1);
  MachinePtySetX(pty, col - 1);
}

static void MachinePtySetCursorRow(struct MachinePty *pty) {
  MachinePtySetY(pty, MachinePtyGetMoveParam(pty) - 1);
}

static void MachinePtySetCursorColumn(struct MachinePty *pty) {
  MachinePtySetX(pty, MachinePtyGetMoveParam(pty) - 1);
}

static void MachinePtyMoveCursor(struct MachinePty *pty, int dy, int dx) {
  int n = MachinePtyGetMoveParam(pty);
  MachinePtySetY(pty, pty->y + dy * n);
  MachinePtySetX(pty, pty->x + dx * n);
}

static void MachinePtyScrollUp(struct MachinePty *pty) {
  int n = MachinePtyGetMoveParam(pty);
  while (n--) MachinePtyScroll(pty);
}

static void MachinePtyScrollDown(struct MachinePty *pty) {
  int n = MachinePtyGetMoveParam(pty);
  while (n--) MachinePtyReverse(pty);
}

static void MachinePtySetCursorStatus(struct MachinePty *pty, bool status) {
  if (status) {
    pty->conf &= ~kMachinePtyNocursor;
  } else {
    pty->conf |= kMachinePtyNocursor;
  }
}

static void MachinePtySetMode(struct MachinePty *pty, bool status) {
  const char *p = pty->esc.s;
  switch (*p++) {
    case '?':
      while (isdigit(*p)) {
        switch (MachinePtyAtoi(p, &p)) {
          case 25:
            MachinePtySetCursorStatus(pty, status);
            break;
          default:
            break;
        }
        if (*p == ';') {
          ++p;
        }
      }
      break;
    default:
      break;
  }
}

static void MachinePtySaveCursorPosition(struct MachinePty *pty) {
  pty->save = (pty->y & 0x7FFF) | (pty->x & 0x7FFF) << 16;
}

static void MachinePtyRestoreCursorPosition(struct MachinePty *pty) {
  MachinePtySetY(pty, (pty->save & 0x00007FFF) >> 000);
  MachinePtySetX(pty, (pty->save & 0x7FFF0000) >> 020);
}

static void MachinePtyEraseDisplay(struct MachinePty *pty) {
  switch (MachinePtyAtoi(pty->esc.s, NULL)) {
    case 0:
      MachinePtyErase(pty, pty->y * pty->xn + pty->x,
                      pty->yn * pty->xn - (pty->y * pty->xn + pty->x));
      break;
    case 1:
      MachinePtyErase(pty, 0, pty->y * pty->xn + pty->x);
      break;
    case 2:
    case 3:
      MachinePtyErase(pty, 0, pty->yn * pty->xn);
      break;
    default:
      break;
  }
}

static void MachinePtyEraseLine(struct MachinePty *pty) {
  switch (MachinePtyAtoi(pty->esc.s, NULL)) {
    case 0:
      MachinePtyErase(pty, pty->y * pty->xn + pty->x, pty->xn - pty->x);
      break;
    case 1:
      MachinePtyErase(pty, pty->y * pty->xn, pty->x);
      break;
    case 2:
      MachinePtyErase(pty, pty->y * pty->xn, pty->xn);
      break;
    default:
      break;
  }
}

static void MachinePtyEraseCells(struct MachinePty *pty) {
  int i, n, x;
  i = pty->y * pty->xn + pty->x;
  n = pty->yn * pty->xn;
  x = min(max(MachinePtyAtoi(pty->esc.s, NULL), 1), n - i);
  MachinePtyErase(pty, i, x);
}

static int MachinePtyArg1(struct MachinePty *pty) {
  return max(1, MachinePtyAtoi(pty->esc.s, NULL));
}

static void MachinePtyInsertCells(struct MachinePty *pty) {
  int n = min(pty->xn - pty->x, MachinePtyArg1(pty));
  MachinePtyMemmove(pty, pty->y * pty->xn + pty->x + n,
                    pty->y * pty->xn + pty->x, pty->xn - (pty->x + n));
  MachinePtyErase(pty, pty->y * pty->xn + pty->x, n);
}

static void MachinePtyInsertLines(struct MachinePty *pty) {
  int n = min(pty->yn - pty->y, MachinePtyArg1(pty));
  MachinePtyMemmove(pty, (pty->y + n) * pty->xn, pty->y * pty->xn,
                    (pty->yn - pty->y - n) * pty->xn);
  MachinePtyErase(pty, pty->y * pty->xn, n * pty->xn);
}

static void MachinePtyDeleteCells(struct MachinePty *pty) {
  int n = min(pty->xn - pty->x, MachinePtyArg1(pty));
  MachinePtyMemmove(pty, pty->y * pty->xn + pty->x,
                    pty->y * pty->xn + pty->x + n, pty->xn - (pty->x + n));
  MachinePtyErase(pty, pty->y * pty->xn + pty->x, n);
}

static void MachinePtyDeleteLines(struct MachinePty *pty) {
  int n = min(pty->yn - pty->y, MachinePtyArg1(pty));
  MachinePtyMemmove(pty, pty->y * pty->xn, (pty->y + n) * pty->xn,
                    (pty->yn - pty->y - n) * pty->xn);
  MachinePtyErase(pty, (pty->y + n) * pty->xn, n * pty->xn);
}

static void MachinePtyReportDeviceStatus(struct MachinePty *pty) {
  MachinePtyWriteInput(pty, "\e[0n", 4);
}

static void MachinePtyReportPreferredVtType(struct MachinePty *pty) {
  MachinePtyWriteInput(pty, "\e[?1;0c", 4);
}

static void MachinePtyReportPreferredVtIdentity(struct MachinePty *pty) {
  MachinePtyWriteInput(pty, "\e/Z", 4);
}

static void MachinePtyBell(struct MachinePty *pty) {
  pty->conf |= kMachinePtyBell;
}

static void MachinePtyLed(struct MachinePty *pty) {
  switch (MachinePtyAtoi(pty->esc.s, NULL)) {
    case 0:
      pty->conf &= ~kMachinePtyLed1;
      pty->conf &= ~kMachinePtyLed2;
      pty->conf &= ~kMachinePtyLed3;
      pty->conf &= ~kMachinePtyLed4;
      break;
    case 1:
      pty->conf |= kMachinePtyLed1;
      break;
    case 2:
      pty->conf |= kMachinePtyLed2;
      break;
    case 3:
      pty->conf |= kMachinePtyLed3;
      break;
    case 4:
      pty->conf |= kMachinePtyLed4;
      break;
    default:
      break;
  }
}

static void MachinePtyReportCursorPosition(struct MachinePty *pty) {
  char *p;
  char buf[2 + 10 + 1 + 10 + 1];
  p = buf;
  *p++ = '\e';
  *p++ = '[';
  p += uint64toarray_radix10((pty->y + 1) & 0x7fff, p);
  *p++ = ';';
  p += uint64toarray_radix10((pty->x + 1) & 0x7fff, p);
  *p++ = 'R';
  MachinePtyWriteInput(pty, buf, p - buf);
}

static void MachinePtyCsiN(struct MachinePty *pty) {
  switch (MachinePtyAtoi(pty->esc.s, NULL)) {
    case 5:
      MachinePtyReportDeviceStatus(pty);
      break;
    case 6:
      MachinePtyReportCursorPosition(pty);
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
              case 38:
                t = kSgrFg;
                break;
              case 48:
                t = kSgrBg;
                break;
              case 0:
                pty->pr = 0;
                pty->xlat = MachinePtyGetXlatSgr(pty);
                break;
              case 1:
                pty->pr |= kMachinePtyBold;
                pty->xlat = MachinePtyGetXlatSgr(pty);
                break;
              case 2:
                pty->pr |= kMachinePtyFaint;
                break;
              case 3:
                pty->pr |= kMachinePtyItalic;
                pty->xlat = MachinePtyGetXlatSgr(pty);
                break;
              case 4:
                pty->pr |= kMachinePtyUnder;
                break;
              case 5:
                pty->pr |= kMachinePtyBlink;
                break;
              case 7:
                pty->pr |= kMachinePtyFlip;
                break;
              case 8:
                pty->pr |= kMachinePtyConceal;
                break;
              case 9:
                pty->pr |= kMachinePtyStrike;
                break;
              case 20:
                pty->pr |= kMachinePtyFraktur;
                pty->xlat = MachinePtyGetXlatSgr(pty);
                break;
              case 21:
                pty->pr |= kMachinePtyUnder | kMachinePtyDunder;
                break;
              case 22:
                pty->pr &= ~(kMachinePtyFaint | kMachinePtyBold);
                pty->xlat = MachinePtyGetXlatSgr(pty);
                break;
              case 23:
                pty->pr &= ~kMachinePtyItalic;
                pty->xlat = MachinePtyGetXlatSgr(pty);
                break;
              case 24:
                pty->pr &= ~(kMachinePtyUnder | kMachinePtyDunder);
                break;
              case 25:
                pty->pr &= ~kMachinePtyBlink;
                break;
              case 27:
                pty->pr &= ~kMachinePtyFlip;
                break;
              case 28:
                pty->pr &= ~kMachinePtyConceal;
                break;
              case 29:
                pty->pr &= ~kMachinePtyStrike;
                break;
              case 39:
                pty->pr &= ~kMachinePtyFg;
                break;
              case 49:
                pty->pr &= ~kMachinePtyBg;
                break;
              case 90 ... 97:
                code[0] -= 90 - 30;
                code[0] += 8;
                /* fallthrough */
              case 30 ... 37:
                pty->fg = code[0] - 30;
                pty->pr |= kMachinePtyFg;
                pty->pr &= ~kMachinePtyTrue;
                break;
              case 100 ... 107:
                code[0] -= 100 - 40;
                code[0] += 8;
                /* fallthrough */
              case 40 ... 47:
                pty->bg = code[0] - 40;
                pty->pr |= kMachinePtyBg;
                pty->pr &= ~kMachinePtyTrue;
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

static void MachinePtyCsi(struct MachinePty *pty) {
  switch (pty->esc.s[pty->esc.i - 1]) {
    case 'f':
    case 'H':
      MachinePtySetCursorPosition(pty);
      break;
    case 'G':
      MachinePtySetCursorColumn(pty);
      break;
    case 'd':
      MachinePtySetCursorRow(pty);
      break;
    case 'F':
      pty->x = 0;
      /* fallthrough */
    case 'A':
      MachinePtyMoveCursor(pty, -1, +0);
      break;
    case 'E':
      pty->x = 0;
      /* fallthrough */
    case 'B':
      MachinePtyMoveCursor(pty, +1, +0);
      break;
    case 'C':
      MachinePtyMoveCursor(pty, +0, +1);
      break;
    case 'D':
      MachinePtyMoveCursor(pty, +0, -1);
      break;
    case 'S':
      MachinePtyScrollUp(pty);
      break;
    case 'T':
      MachinePtyScrollDown(pty);
      break;
    case '@':
      MachinePtyInsertCells(pty);
      break;
    case 'P':
      MachinePtyDeleteCells(pty);
      break;
    case 'L':
      MachinePtyInsertLines(pty);
      break;
    case 'M':
      MachinePtyDeleteLines(pty);
      break;
    case 'J':
      MachinePtyEraseDisplay(pty);
      break;
    case 'K':
      MachinePtyEraseLine(pty);
      break;
    case 'X':
      MachinePtyEraseCells(pty);
      break;
    case 's':
      MachinePtySaveCursorPosition(pty);
      break;
    case 'u':
      MachinePtyRestoreCursorPosition(pty);
      break;
    case 'n':
      MachinePtyCsiN(pty);
      break;
    case 'm':
      MachinePtySelectGraphicsRendition(pty);
      break;
    case 'h':
      MachinePtySetMode(pty, true);
      break;
    case 'l':
      MachinePtySetMode(pty, false);
      break;
    case 'c':
      MachinePtyReportPreferredVtType(pty);
      break;
    case 'q':
      MachinePtyLed(pty);
      break;
    default:
      break;
  }
}

static void MachinePtyScreenAlignmentDisplay(struct MachinePty *pty) {
  wmemset((void *)pty->wcs, 'E', pty->yn * pty->xn);
}

static void MachinePtyEscHash(struct MachinePty *pty) {
  switch (pty->esc.s[1]) {
    case '5':
      MachinePtySetXlat(pty, MachinePtyGetXlatAscii());
      break;
    case '6':
      MachinePtySetXlat(pty, MachinePtyGetXlatDoubleWidth());
      break;
    case '8':
      MachinePtyScreenAlignmentDisplay(pty);
      break;
    default:
      break;
  }
}

static void MachinePtyEsc(struct MachinePty *pty) {
  switch (pty->esc.s[0]) {
    case 'c':
      MachinePtyFullReset(pty);
      break;
    case '7':
      MachinePtySaveCursorPosition(pty);
      break;
    case '8':
      MachinePtyRestoreCursorPosition(pty);
      break;
    case 'E':
      pty->x = 0;
    case 'D':
      MachinePtyIndex(pty);
      break;
    case 'M':
      MachinePtyReverseIndex(pty);
      break;
    case 'Z':
      MachinePtyReportPreferredVtIdentity(pty);
      break;
    case '(':
      MachinePtySetCodepage(pty, pty->esc.s[1]);
      break;
    case '#':
      MachinePtyEscHash(pty);
      break;
    default:
      break;
  }
}

static void MachinePtyCntrl(struct MachinePty *pty, int c01) {
  switch (c01) {
    case '\a':
      MachinePtyBell(pty);
      break;
    case 0x85:
    case '\f':
    case '\v':
    case '\n':
      MachinePtyNewline(pty);
      break;
    case '\r':
      MachinePtyCarriageReturn(pty);
      break;
    case '\e':
      pty->state = kMachinePtyEsc;
      pty->esc.i = 0;
      break;
    case '\t':
      MachinePtyWriteTab(pty);
      break;
    case 0x7F:
    case '\b':
      pty->x = MAX(0, pty->x - 1);
      break;
    case 0x84:
      MachinePtyIndex(pty);
      break;
    case 0x8D:
      MachinePtyReverseIndex(pty);
      break;
    case 0x9B:
      pty->state = kMachinePtyCsi;
      break;
    default:
      break;
  }
}

static void MachinePtyEscAppend(struct MachinePty *pty, char c) {
  pty->esc.i = MIN(pty->esc.i + 1, ARRAYLEN(pty->esc.s) - 1);
  pty->esc.s[pty->esc.i - 1] = c;
  pty->esc.s[pty->esc.i - 0] = 0;
}

ssize_t MachinePtyWrite(struct MachinePty *pty, const void *data, size_t n) {
  int i;
  wchar_t wc;
  const uint8_t *p;
  for (p = data, i = 0; i < n; ++i) {
    switch (pty->state) {
      case kMachinePtyAscii:
        if (0x00 <= p[i] && p[i] <= 0x7F) {
          if (0x20 <= p[i] && p[i] <= 0x7E) {
            if ((wc = pty->xlat[p[i]]) >= 0) {
              MachinePtyWriteGlyph(pty, wc, 1);
            } else {
              MachinePtyWriteGlyph(pty, -wc, 2);
            }
          } else {
            MachinePtyCntrl(pty, p[i]);
          }
        } else if (!ThomPikeCont(p[i])) {
          pty->state = kMachinePtyUtf8;
          pty->u8 = ThomPikeByte(p[i]);
          pty->n8 = ThomPikeLen(p[i]) - 1;
        }
        break;
      case kMachinePtyUtf8:
        if (ThomPikeCont(p[i])) {
          pty->u8 = ThomPikeMerge(pty->u8, p[i]);
          if (--pty->n8) break;
        }
        wc = pty->u8;
        if ((0x00 <= wc && wc <= 0x1F) || (0x7F <= wc && wc <= 0x9F)) {
          MachinePtyCntrl(pty, wc);
        } else {
          MachinePtyWriteGlyph(pty, wc, wcwidth(wc));
        }
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
        unreachable;
    }
  }
  return n;
}

ssize_t MachinePtyWriteInput(struct MachinePty *pty, const void *data,
                             size_t n) {
  MachinePtyConcatInput(pty, data, n);
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

static char *MachinePtyEncodeRgb(char *p, int rgb) {
  *p++ = '2';
  *p++ = ';';
  p += uint64toarray_radix10((rgb & 0x0000ff) >> 000, p);
  *p++ = ';';
  p += uint64toarray_radix10((rgb & 0x00ff00) >> 010, p);
  *p++ = ';';
  p += uint64toarray_radix10((rgb & 0xff0000) >> 020, p);
  return p;
}

static char *MachinePtyEncodeXterm256(char *p, int xt) {
  *p++ = '5';
  *p++ = ';';
  p += uint64toarray_radix10(xt, p);
  return p;
}

char *MachinePtyEncodeStyle(char *p, uint32_t xr, uint32_t pr, uint32_t fg,
                            uint32_t bg) {
  *p++ = '\e';
  *p++ = '[';
  if (pr & (kMachinePtyBold | kMachinePtyFaint | kMachinePtyFlip |
            kMachinePtyUnder | kMachinePtyDunder | kMachinePtyBlink |
            kMachinePtyStrike | kMachinePtyFg | kMachinePtyBg)) {
    if (xr & (kMachinePtyBold | kMachinePtyFaint)) {
      if ((xr & (kMachinePtyBold | kMachinePtyFaint)) ^
          (pr & (kMachinePtyBold | kMachinePtyFaint))) {
        *p++ = '2';
        *p++ = '2';
        *p++ = ';';
      }
      if (pr & kMachinePtyBold) {
        *p++ = '1';
        *p++ = ';';
      }
      if (pr & kMachinePtyFaint) {
        *p++ = '2';
        *p++ = ';';
      }
    }
    if (xr & (kMachinePtyUnder | kMachinePtyDunder)) {
      if ((xr & (kMachinePtyUnder | kMachinePtyDunder)) ^
          (pr & (kMachinePtyUnder | kMachinePtyDunder))) {
        *p++ = '2';
        *p++ = '4';
        *p++ = ';';
      }
      if (pr & kMachinePtyUnder) {
        *p++ = '4';
        *p++ = ';';
      }
      if (pr & kMachinePtyDunder) {
        *p++ = '2';
        *p++ = '1';
        *p++ = ';';
      }
    }
    if (xr & (kMachinePtyFlip | kMachinePtyBlink | kMachinePtyStrike)) {
      if (xr & kMachinePtyFlip) {
        if (!(pr & kMachinePtyFlip)) *p++ = '2';
        *p++ = '7';
        *p++ = ';';
      }
      if (xr & kMachinePtyBlink) {
        if (!(pr & kMachinePtyBlink)) *p++ = '2';
        *p++ = '5';
        *p++ = ';';
      }
      if (xr & kMachinePtyStrike) {
        if (!(pr & kMachinePtyStrike)) *p++ = '2';
        *p++ = '9';
        *p++ = ';';
      }
    }
    if (xr & (kMachinePtyFg | kMachinePtyTrue)) {
      *p++ = '3';
      if (pr & kMachinePtyFg) {
        *p++ = '8';
        *p++ = ';';
        if (pr & kMachinePtyTrue) {
          p = MachinePtyEncodeRgb(p, fg);
        } else {
          p = MachinePtyEncodeXterm256(p, fg);
        }
      } else {
        *p++ = '9';
      }
      *p++ = ';';
    }
    if (xr & (kMachinePtyBg | kMachinePtyTrue)) {
      *p++ = '4';
      if (pr & kMachinePtyBg) {
        *p++ = '8';
        *p++ = ';';
        if (pr & kMachinePtyTrue) {
          p = MachinePtyEncodeRgb(p, bg);
        } else {
          p = MachinePtyEncodeXterm256(p, bg);
        }
      } else {
        *p++ = '9';
      }
      *p++ = ';';
    }
    DCHECK_EQ(';', p[-1]);
    p[-1] = 'm';
  } else {
    *p++ = '0';
    *p++ = 'm';
  }
  return p;
}

void MachinePtyAppendLine(struct MachinePty *pty, struct Buffer *buf,
                          unsigned y) {
  uint64_t u;
  size_t need;
  char *p, *pb;
  uint32_t i, j, w, wc, np, xp, pr, fg, bg, ci;
  if (y >= pty->yn) return;
  need = buf->i + pty->xn * 60;
  if (need > buf->n) {
    CHECK_NOTNULL((buf->p = realloc(buf->p, need)));
    buf->n = need;
  }
  i = y * pty->xn;
  j = (y + 1) * pty->xn;
  pb = buf->p + buf->i;
  ci = !(pty->conf & kMachinePtyNocursor) && y == pty->y ? i + pty->x : -1;
  for (pr = 0; i < j; i += w) {
    np = pty->prs[i];
    if (!(np & kMachinePtyConceal)) {
      wc = pty->wcs[i];
      DCHECK(!(0x00 <= wc && wc <= 0x1F));
      DCHECK(!(0x7F <= wc && wc <= 0x9F));
      if (0x20 <= wc && wc <= 0x7E) {
        u = wc;
        w = 1;
      } else {
        u = tpenc(wc);
        w = max(1, wcwidth(wc));
      }
    } else {
      u = ' ';
      w = 1;
    }
    if (i == ci) {
      if (u != ' ') {
        np ^= kMachinePtyFlip;
      } else {
        u = tpenc(u'▂');
        if (pty->conf & kMachinePtyBlinkcursor) {
          np |= kMachinePtyBlink;
        }
      }
    }
    fg = bg = -1;
    xp = pr ^ np;
    if (np & (kMachinePtyFg | kMachinePtyBg)) {
      if (np & kMachinePtyFg) {
        if (pty->fgs[i] != fg) xp |= kMachinePtyFg;
        fg = pty->fgs[i];
      }
      if (np & kMachinePtyBg) {
        if (pty->bgs[i] != bg) xp |= kMachinePtyBg;
        bg = pty->bgs[i];
      }
    }
    p = pb;
    if (xp) {
      pr = np;
      p = MachinePtyEncodeStyle(p, xp, pr, fg, bg);
    }
    do {
      *p++ = u & 0xFF;
      u >>= 8;
    } while (u);
    DCHECK_LE(p - pb, 60);
    pb = p;
  }
  DCHECK_LE(pb - buf->p, buf->n);
  buf->i = pb - buf->p;
}
