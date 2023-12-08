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
#include "libc/fmt/bing.internal.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/runtime/pc.internal.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/unicode.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "libc/vga/vga.internal.h"

#ifdef __x86_64__

/**
 * @internal
 * @fileoverview ECMA-48 / VT100 video terminal implementation for bare
 * metal VGA.
 *
 * The code is mainly a pared-down version of the implementation of a
 * pseudo-tty in tool/build/lib/pty.c.
 *
 * @see tool/build/lib/pty.c
 */

#define DEFAULT_FG \
  ((TtyCanvasColor){.bgr.r = 0xaa, .bgr.g = 0xaa, .bgr.b = 0xaa, .bgr.x = 0xff})
#define DEFAULT_BG \
  ((TtyCanvasColor){.bgr.r = 0, .bgr.g = 0, .bgr.b = 0, .bgr.x = 0xff})
#define CRTPORT 0x3d4

static void SetYn(struct Tty *tty, unsigned short yn) {
  tty->yn = yn;
}

static void SetXn(struct Tty *tty, unsigned short xn) {
  tty->xn = xn;
}

static void SetYp(struct Tty *tty, unsigned short yp) {
  tty->yp = yp;
}

static void SetXp(struct Tty *tty, unsigned short xp) {
  tty->xp = xp;
}

static void SetXsFb(struct Tty *tty, unsigned short xsfb) {
  tty->xsfb = xsfb;
}

static bool SetWcs(struct Tty *tty, unsigned init_flags) {
#ifdef VGA_USE_WCS
  struct DirectMap dm;
  if (!(init_flags & kTtyAllocWcs)) return false;
  dm = sys_mmap_metal(NULL, Yn(tty) * Xn(tty) * sizeof(wchar_t),
                      PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1,
                      0);
  if (dm.addr == (void *)-1) {
    tty->wcs = NULL;
    return false;
  }
  tty->wcs = dm.addr;
  return true;
#else
  return false;
#endif
}

static size_t Yn(struct Tty *tty) {
  return tty->yn;
}

static size_t Xn(struct Tty *tty) {
  return tty->xn;
}

static wchar_t *Wcs(struct Tty *tty) {
#ifdef VGA_USE_WCS
  return tty->wcs;
#else
  return NULL;
#endif
}

static void TtyNoopUpdate(struct Tty *);
static void TtyTextDrawChar(struct Tty *, size_t, size_t, wchar_t);
static void TtyTextEraseLineCells(struct Tty *, size_t, size_t, size_t);
static void TtyTextMoveLineCells(struct Tty *, size_t, size_t, size_t, size_t,
                                 size_t);

static void TtySetType(struct Tty *tty, unsigned char type,
                       unsigned init_flags) {
  tty->type = type;
  if ((init_flags & kTtyKlog) == 0) {
    switch (type) {
      case PC_VIDEO_BGR565:
        tty->update = _TtyBgr565Update;
        goto graphics_mode;
      case PC_VIDEO_BGR555:
        tty->update = _TtyBgr555Update;
        goto graphics_mode;
      case PC_VIDEO_BGRX8888:
        tty->update = _TtyBgrxUpdate;
        goto graphics_mode;
      case PC_VIDEO_RGBX8888:
        tty->update = _TtyRgbxUpdate;
      graphics_mode:
        tty->drawchar = _TtyGraphDrawChar;
        tty->eraselinecells = _TtyGraphEraseLineCells;
        tty->movelinecells = _TtyGraphMoveLineCells;
        break;
      default:
        tty->update = TtyNoopUpdate;
        tty->drawchar = TtyTextDrawChar;
        tty->eraselinecells = TtyTextEraseLineCells;
        tty->movelinecells = TtyTextMoveLineCells;
    }
  } else {
    switch (type) {
      case PC_VIDEO_BGR565:
      case PC_VIDEO_BGR555:
        tty->update = _TtyKlog16Update;
        tty->drawchar = _TtyKlog16DrawChar;
        tty->eraselinecells = _TtyKlog16EraseLineCells;
        tty->movelinecells = _TtyKlog16MoveLineCells;
        break;
      case PC_VIDEO_BGRX8888:
      case PC_VIDEO_RGBX8888:
        tty->update = _TtyKlog32Update;
        tty->drawchar = _TtyKlog32DrawChar;
        tty->eraselinecells = _TtyKlog32EraseLineCells;
        tty->movelinecells = _TtyKlog32MoveLineCells;
        break;
      default:
        tty->update = TtyNoopUpdate;
        tty->drawchar = TtyTextDrawChar;
        tty->eraselinecells = TtyTextEraseLineCells;
        tty->movelinecells = TtyTextMoveLineCells;
    }
  }
}

void _StartTty(struct Tty *tty, unsigned char type, unsigned short yp,
               unsigned short xp, unsigned short xsfb, unsigned short starty,
               unsigned short startx, unsigned char yc, unsigned char xc,
               void *fb, unsigned init_flags) {
  unsigned short yn, xn, xs = xp * sizeof(TtyCanvasColor);
  struct DirectMap dm;
  bzero(tty, sizeof(struct Tty));
  SetYp(tty, yp);
  SetXp(tty, xp);
  SetXsFb(tty, xsfb);
  if (type == PC_VIDEO_TEXT) {
    yn = yp;
    xn = xp;
    tty->canvas = fb;
  } else {
    yn = yp / yc;
    xn = xp / xc;
    if ((init_flags & kTtyKlog) != 0) {
      tty->canvas = fb;
      xs = xsfb;
    } else {
      dm = sys_mmap_metal(NULL, (size_t)yp * xs, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      if (dm.addr == (void *)-1) {
        /*
         * We are a bit low on memory.  Try to go on anyway, & initialize
         * our tty as an emergency console.
         */
        init_flags |= kTtyKlog;
        tty->canvas = fb;
        xs = xsfb;
      } else
        tty->canvas = dm.addr;
    }
  }
  SetYn(tty, yn);
  SetXn(tty, xn);
  tty->yc = yc;
  tty->xc = xc;
  tty->fb = fb;
  if (starty >= yn) starty = yn - 1;
  if (startx >= xn) startx = xn - 1;
  tty->y = starty;
  tty->x = startx;
  if ((init_flags & kTtyKlog) != 0) init_flags &= ~kTtyAllocWcs;
  tty->xs = xs;
  TtySetType(tty, type, init_flags);
  if (SetWcs(tty, init_flags)) {
    wchar_t *wcs = Wcs(tty);
    size_t n = (size_t)yn * xn, i;
    if (type == PC_VIDEO_TEXT) {
      struct VgaTextCharCell *ccs = fb;
      for (i = 0; i < n; ++i) wcs[i] = bing(ccs[i].ch, 0);
    } else
      wmemset(wcs, L' ', n);
  }
  _TtyResetOutputMode(tty);
}

static wchar_t *GetXlatAscii(void) {
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

static wchar_t *GetXlatLineDrawing(void) {
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

static wchar_t *GetXlatItalic(void) {
  /* Unimplemented.  Simply output normal non-italicized characters for now. */
  return GetXlatAscii();
}

static wchar_t *GetXlatBoldItalic(void) {
  /*
   * Unimplemented.  Simply output high-intensity non-italicized characters
   * for now.
   */
  return GetXlatAscii();
}

static wchar_t *GetXlatBoldFraktur(void) {
  /* Unimplemented. */
  return GetXlatAscii();
}

static wchar_t *GetXlatFraktur(void) {
  /* Unimplemented. */
  return GetXlatAscii();
}

static wchar_t *GetXlatDoubleWidth(void) {
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

static wchar_t *GetXlatSgr(struct Tty *tty) {
  switch (!!(tty->pr & kTtyFraktur) << 2 | !!(tty->pr & kTtyItalic) << 1 |
          !!(tty->pr & kTtyBold) << 0) {
    case 0b100:
    case 0b110:
      return GetXlatFraktur();
    case 0b101:
    case 0b111:
      return GetXlatBoldFraktur();
    case 0b011:
      return GetXlatBoldItalic();
    case 0b010:
      return GetXlatItalic();
    default:
      return GetXlatAscii();
  }
}

static void TtySetXlat(struct Tty *tty, wchar_t *xlat) {
  tty->xlat = xlat;
  tty->pr &= ~(kTtyItalic | kTtyFraktur);
}

static void TtySetCodepage(struct Tty *tty, char id) {
  switch (id) {
    default:
    case 'B':
      TtySetXlat(tty, GetXlatAscii());
      break;
    case '0':
      TtySetXlat(tty, GetXlatLineDrawing());
      break;
  }
}

/**
 * @internal
 * Map the given direct color value to one of the 16 basic foreground colors
 * or one of the 16 background colors.
 *
 * @see drivers/tty/vt/vt.c in Linux 5.9.14 source code
 */
static uint8_t TtyGetTextColor(TtyCanvasColor color) {
  uint8_t r = color.bgr.r, g = color.bgr.g, b = color.bgr.b;
  uint8_t hue = 0, max = MAX(MAX(r, g), b);
  if (r > max / 2) hue |= 4;
  if (g > max / 2) hue |= 2;
  if (b > max / 2) hue |= 1;
  if (hue == 7 && max <= 0x55)
    hue = 8;
  else if (max > 0xaa)
    hue |= 8;
  return hue;
}

/**
 * @internal
 * Map the currently active foreground & background colors & terminal
 * configuration to a VGA text character attribute byte.
 *
 * @see VGA_USE_BLINK macro (libc/vga/vga.internal.h)
 * @see drivers/tty/vt/vt.c in Linux 5.9.14 source code
 */
static uint8_t TtyGetTextAttr(struct Tty *tty) {
  uint8_t fg = TtyGetTextColor(tty->fg), bg = TtyGetTextColor(tty->bg);
  uint8_t attr;
  if ((tty->pr & kTtyFlip) == 0)
    attr = fg | bg << 4;
  else
    attr = bg | fg << 4;
#ifdef VGA_USE_BLINK
  /*
   * If blinking is enabled, we can only have the 8 dark background color
   * codes (0 to 7).  Simply map any bright background color (8 to 15) to
   * its corresponding dark color, & call it a day.  This is a bit more
   * simplistic than what Linux does, but should be enough.
   */
  attr &= ~0x80;
  if ((tty->pr & kTtyBlink) != 0) attr |= 0x80;
#endif
  if ((tty->pr & kTtyBold) != 0) attr |= 0x08;
  return attr;
}

static void TtyNoopUpdate(struct Tty *tty) {
}

static void TtyTextDrawChar(struct Tty *tty, size_t y, size_t x, wchar_t wc) {
  uint8_t attr = TtyGetTextAttr(tty);
  struct VgaTextCharCell *ccs = (struct VgaTextCharCell *)tty->canvas;
  size_t i = tty->y * Xn(tty) + tty->x;
  int c = unbing(wc);
  if (c == -1) c = 0xFE;
  ccs[i] = (struct VgaTextCharCell){c, attr};
}

static void TtyTextEraseLineCells(struct Tty *tty, size_t dsty, size_t dstx,
                                  size_t n) {
  uint8_t attr = TtyGetTextAttr(tty);
  struct VgaTextCharCell *ccs = (struct VgaTextCharCell *)tty->canvas;
  size_t dst = dsty * Xn(tty) + dstx, i;
  for (i = 0; i < n; ++i) ccs[dst + i] = (struct VgaTextCharCell){' ', attr};
}

void _TtyEraseLineCells(struct Tty *tty, size_t dsty, size_t dstx, size_t n) {
  tty->eraselinecells(tty, dsty, dstx, n);
  if (Wcs(tty)) {
    size_t i = dsty * Xn(tty) + dstx;
    wmemset(Wcs(tty) + i, L' ', n);
  }
}

void _TtyEraseLines(struct Tty *tty, size_t dsty, size_t n) {
  size_t xn = Xn(tty), y;
  y = dsty;
  while (n-- != 0) {
    _TtyEraseLineCells(tty, y, 0, xn);
    ++y;
  }
}

static void TtyTextMoveLineCells(struct Tty *tty, size_t dsty, size_t dstx,
                                 size_t srcy, size_t srcx, size_t n) {
  size_t xn = Xn(tty);
  size_t dst = dsty * xn + dstx, src = srcy * xn + srcx;
  struct VgaTextCharCell *ccs = (struct VgaTextCharCell *)tty->canvas;
  memmove(ccs + dst, ccs + src, n * sizeof(struct VgaTextCharCell));
}

void _TtyMoveLineCells(struct Tty *tty, size_t dsty, size_t dstx, size_t srcy,
                       size_t srcx, size_t n) {
  size_t xn = Xn(tty);
  size_t dst = dsty * xn + dstx, src = srcy * xn + srcx;
  tty->movelinecells(tty, dsty, dstx, srcy, srcx, n);
  if (Wcs(tty)) wmemmove(Wcs(tty) + dst, Wcs(tty) + src, n);
}

void _TtyMoveLines(struct Tty *tty, size_t dsty, size_t srcy, size_t n) {
  size_t xn = Xn(tty);
  if (dsty < srcy) {
    while (n-- != 0) {
      _TtyMoveLineCells(tty, dsty, 0, srcy, 0, xn);
      ++dsty;
      ++srcy;
    }
  } else if (dsty > srcy) {
    while (n-- != 0) _TtyMoveLineCells(tty, dsty + n, 0, srcy + n, 0, xn);
  }
}

void _TtyResetOutputMode(struct Tty *tty) {
  tty->updy1 = tty->updx1 = tty->updy2 = tty->updx2 = 0;
  tty->pr = 0;
  tty->fg = DEFAULT_FG;
  tty->bg = DEFAULT_BG;
  tty->u8 = 0;
  tty->n8 = 0;
  tty->conf = 0;
  tty->savey = 0;
  tty->savex = 0;
  tty->state = 0;
  tty->esc.i = 0;
  tty->input.i = 0;
  tty->xlat = GetXlatAscii();
}

void _TtyFullReset(struct Tty *tty) {
  _TtyResetOutputMode(tty);
  tty->y = 0;
  tty->x = 0;
  _TtyEraseLines(tty, 0, Yn(tty));
}

void _TtySetY(struct Tty *tty, unsigned short y) {
  tty->conf &= ~kTtyRedzone;
  tty->y = MAX(0, MIN(Yn(tty) - 1, y));
}

void _TtySetX(struct Tty *tty, unsigned short x) {
  tty->conf &= ~kTtyRedzone;
  tty->x = MAX(0, MIN(Xn(tty) - 1, x));
}

static void TtyScroll(struct Tty *tty) {
  _TtyMoveLines(tty, 0, 1, Yn(tty) - 1);
  _TtyEraseLines(tty, Yn(tty) - 1, 1);
}

static void TtyReverse(struct Tty *tty) {
  _TtyMoveLines(tty, 1, 0, Yn(tty) - 1);
  _TtyEraseLines(tty, 0, 1);
}

static void TtyIndex(struct Tty *tty) {
  if (tty->y < Yn(tty) - 1) {
    ++tty->y;
  } else {
    TtyScroll(tty);
  }
}

static void TtyReverseIndex(struct Tty *tty) {
  if (tty->y) {
    --tty->y;
  } else {
    TtyReverse(tty);
  }
}

static void TtyCarriageReturn(struct Tty *tty) {
  _TtySetX(tty, 0);
}

static void TtyNewline(struct Tty *tty) {
  TtyIndex(tty);
  if (!(tty->conf & kTtyNoopost)) {
    TtyCarriageReturn(tty);
  }
}

static void TtyAdvance(struct Tty *tty) {
  tty->conf &= ~kTtyRedzone;
  tty->x = 0;
  if (tty->y < Yn(tty) - 1) {
    ++tty->y;
  } else {
    TtyScroll(tty);
  }
}

static void TtyWriteGlyph(struct Tty *tty, wint_t wc, int w) {
  if (w < 1) wc = L' ', w = 1;
  if ((tty->conf & kTtyRedzone) || tty->x + w > Xn(tty)) {
    TtyAdvance(tty);
  }
  tty->drawchar(tty, tty->y, tty->x, wc);
  if (Wcs(tty)) Wcs(tty)[tty->y * Xn(tty) + tty->x] = wc;
  if ((tty->x += w) >= Xn(tty)) {
    tty->x = Xn(tty) - 1;
    tty->conf |= kTtyRedzone;
  }
}

static void TtyWriteTab(struct Tty *tty) {
  unsigned x, x2;
  if (tty->conf & kTtyRedzone) {
    TtyAdvance(tty);
  }
  x2 = MIN(Xn(tty), ROUNDUP(tty->x + 1, 8));
  for (x = tty->x; x < x2; ++x) {
    tty->eraselinecells(tty, tty->y, x, 1);
  }
  if (Wcs(tty)) {
    for (x = tty->x; x < x2; ++x) {
      Wcs(tty)[tty->y * Xn(tty) + x] = L' ';
    }
  }
  if (x2 < Xn(tty)) {
    tty->x = x2;
  } else {
    tty->x = Xn(tty) - 1;
    tty->conf |= kTtyRedzone;
  }
}

int TtyAtoi(const char *s, const char **e) {
  int i;
  for (i = 0; isdigit(*s); ++s) i *= 10, i += *s - '0';
  if (e) *e = s;
  return i;
}

static int TtyGetMoveParam(struct Tty *tty) {
  int x = TtyAtoi(tty->esc.s, NULL);
  if (x < 1) x = 1;
  return x;
}

static void TtySetCursorPosition(struct Tty *tty) {
  int row, col;
  const char *s = tty->esc.s;
  row = max(1, TtyAtoi(s, &s));
  if (*s == ';') ++s;
  col = max(1, TtyAtoi(s, &s));
  _TtySetY(tty, row - 1);
  _TtySetX(tty, col - 1);
}

static void TtySetCursorRow(struct Tty *tty) {
  _TtySetY(tty, TtyGetMoveParam(tty) - 1);
}

static void TtySetCursorColumn(struct Tty *tty) {
  _TtySetX(tty, TtyGetMoveParam(tty) - 1);
}

static void TtyMoveCursor(struct Tty *tty, int dy, int dx) {
  int n = TtyGetMoveParam(tty);
  _TtySetY(tty, tty->y + dy * n);
  _TtySetX(tty, tty->x + dx * n);
}

static void TtyScrollUp(struct Tty *tty) {
  int n = TtyGetMoveParam(tty);
  while (n--) TtyScroll(tty);
}

static void TtyScrollDown(struct Tty *tty) {
  int n = TtyGetMoveParam(tty);
  while (n--) TtyReverse(tty);
}

static void TtySetCursorStatus(struct Tty *tty, bool status) {
  if (status) {
    tty->conf &= ~kTtyNocursor;
  } else {
    tty->conf |= kTtyNocursor;
  }
}

static void TtySetMode(struct Tty *tty, bool status) {
  const char *p = tty->esc.s;
  switch (*p++) {
    case '?':
      while (isdigit(*p)) {
        switch (TtyAtoi(p, &p)) {
          case 25:
            TtySetCursorStatus(tty, status);
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

static void TtySaveCursorPosition(struct Tty *tty) {
  tty->savey = tty->y;
  tty->savex = tty->x;
}

static void TtyRestoreCursorPosition(struct Tty *tty) {
  _TtySetY(tty, tty->savey);
  _TtySetX(tty, tty->savex);
}

static void TtyEraseDisplay(struct Tty *tty) {
  switch (TtyAtoi(tty->esc.s, NULL)) {
    case 0:
      _TtyEraseLineCells(tty, tty->y, tty->x, Xn(tty) - tty->x);
      _TtyEraseLines(tty, tty->y + 1, Yn(tty) - tty->y - 1);
      break;
    case 1:
      _TtyEraseLines(tty, 0, tty->y);
      _TtyEraseLineCells(tty, tty->y, 0, tty->x);
      break;
    case 2:
    case 3:
      _TtyEraseLines(tty, 0, Yn(tty));
      break;
    default:
      break;
  }
}

static void TtyEraseLine(struct Tty *tty) {
  switch (TtyAtoi(tty->esc.s, NULL)) {
    case 0:
      _TtyEraseLineCells(tty, tty->y, tty->x, Xn(tty) - tty->x);
      break;
    case 1:
      _TtyEraseLineCells(tty, tty->y, 0, tty->x);
      break;
    case 2:
      _TtyEraseLines(tty, tty->y, 1);
      break;
    default:
      break;
  }
}

static void TtyEraseCells(struct Tty *tty) {
  int yn, xn, yi, xi, left;
  yn = Yn(tty);
  xn = Xn(tty);
  yi = tty->y;
  xi = tty->x;
  left = min(max(TtyAtoi(tty->esc.s, NULL), 1), yn * xn - (yi * xn + xi));
  while (left) {
    if (left >= xn - xi) {
      _TtyEraseLineCells(tty, yi, xi, xn - xi);
      left -= xn - xi;
      ++yi;
      xi = 0;
    } else {
      _TtyEraseLineCells(tty, yi, xi, left);
      left = 0;
    }
  }
}

static int TtyArg1(struct Tty *tty) {
  return max(1, TtyAtoi(tty->esc.s, NULL));
}

static void TtyInsertCells(struct Tty *tty) {
  int n = min(Xn(tty) - tty->x, TtyArg1(tty));
  _TtyMoveLineCells(tty, tty->y, tty->x + n, tty->y, tty->x,
                    Xn(tty) - (tty->x + n));
  _TtyEraseLineCells(tty, tty->y, tty->x, n);
}

static void TtyInsertLines(struct Tty *tty) {
  int n = min(Yn(tty) - tty->y, TtyArg1(tty));
  _TtyMoveLines(tty, tty->y + n, tty->y, Yn(tty) - tty->y - n);
  _TtyEraseLines(tty, tty->y, n);
}

static void TtyDeleteCells(struct Tty *tty) {
  int n = min(Xn(tty) - tty->x, TtyArg1(tty));
  _TtyMoveLineCells(tty, tty->y, tty->x, tty->y, tty->x + n,
                    Xn(tty) - (tty->x + n));
  _TtyEraseLineCells(tty, tty->y, tty->x, n);
}

static void TtyDeleteLines(struct Tty *tty) {
  int n = min(Yn(tty) - tty->y, TtyArg1(tty));
  _TtyMoveLines(tty, tty->y, tty->y + n, Yn(tty) - tty->y - n);
  _TtyEraseLines(tty, tty->y + n, n);
}

static void TtyReportDeviceStatus(struct Tty *tty) {
  static const char report[] = "\e[0n";
  _TtyWriteInput(tty, report, strlen(report));
}

static void TtyReportPreferredVtType(struct Tty *tty) {
  static const char report[] = "\e[?1;0c";
  _TtyWriteInput(tty, report, strlen(report));
}

static void TtyReportPreferredVtIdentity(struct Tty *tty) {
  static const char report[] = "\e/Z";
  _TtyWriteInput(tty, report, strlen(report));
}

static void TtyBell(struct Tty *tty) {
  tty->conf |= kTtyBell;
}

static void TtyLed(struct Tty *tty) {
  switch (TtyAtoi(tty->esc.s, NULL)) {
    case 0:
      tty->conf &= ~kTtyLed1;
      tty->conf &= ~kTtyLed2;
      tty->conf &= ~kTtyLed3;
      tty->conf &= ~kTtyLed4;
      break;
    case 1:
      tty->conf |= kTtyLed1;
      break;
    case 2:
      tty->conf |= kTtyLed2;
      break;
    case 3:
      tty->conf |= kTtyLed3;
      break;
    case 4:
      tty->conf |= kTtyLed4;
      break;
    default:
      break;
  }
}

static void TtyReportCursorPosition(struct Tty *tty) {
  char *p;
  char buf[2 + 10 + 1 + 10 + 1];
  p = buf;
  *p++ = '\e';
  *p++ = '[';
  p = FormatInt32(p, (tty->y + 1) & 0x7fff);
  *p++ = ';';
  p = FormatInt32(p, (tty->x + 1) & 0x7fff);
  *p++ = 'R';
  _TtyWriteInput(tty, buf, p - buf);
}

static void TtyCsiN(struct Tty *tty) {
  switch (TtyAtoi(tty->esc.s, NULL)) {
    case 5:
      TtyReportDeviceStatus(tty);
      break;
    case 6:
      TtyReportCursorPosition(tty);
      break;
    default:
      break;
  }
}

/**
 * @internal
 * Create a direct color pixel value.
 */
static TtyCanvasColor TtyMapTrueColor(uint8_t r, uint8_t g, uint8_t b) {
  return (TtyCanvasColor){.bgr.r = r, .bgr.g = g, .bgr.b = b, .bgr.x = 0xff};
}

/**
 * @internal
 * Map the given 256-color code to a direct color.
 */
static TtyCanvasColor TtyMapXtermColor(uint8_t color) {
  uint8_t r, g, b;
  if (color < 8) {
    r = (color & 1) ? 0xaa : 0;
    g = (color & 2) ? 0xaa : 0;
    b = (color & 4) ? 0xaa : 0;
  } else if (color < 16) {
    r = (color & 1) ? 0xff : 0x55;
    g = (color & 2) ? 0xff : 0x55;
    b = (color & 4) ? 0xff : 0x55;
  } else if (color < 232) {
    color -= 16;
    b = color % 6 * 0x55 / 2;
    color /= 6;
    g = color % 6 * 0x55 / 2;
    color /= 6;
    r = color * 0x55 / 2;
  } else
    r = g = b = (unsigned)color * 10 - 2312;
  return TtyMapTrueColor(r, g, b);
}

/**
 * @internal
 * Map the given ECMA-48 / VT100 SGR color code to a direct color.
 */
static TtyCanvasColor TtyMapEcma48Color(uint8_t color) {
  return TtyMapXtermColor(color % 16);
}

static void TtySelectGraphicsRendition(struct Tty *tty) {
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
  p = tty->esc.s;
  bzero(code, sizeof(code));
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
                tty->pr = 0;
                tty->fg = DEFAULT_FG;
                tty->bg = DEFAULT_BG;
                tty->xlat = GetXlatSgr(tty);
                break;
              case 1:
                tty->pr |= kTtyBold;
                tty->xlat = GetXlatSgr(tty);
                break;
              case 2:
                tty->pr |= kTtyFaint;
                break;
              case 3:
                tty->pr |= kTtyItalic;
                tty->xlat = GetXlatSgr(tty);
                break;
              case 4:
                tty->pr |= kTtyUnder;
                break;
              case 5:
                tty->pr |= kTtyBlink;
                break;
              case 7:
                tty->pr |= kTtyFlip;
                break;
              case 8:
                tty->pr |= kTtyConceal;
                break;
              case 9:
                tty->pr |= kTtyStrike;
                break;
              case 20:
                tty->pr |= kTtyFraktur;
                tty->xlat = GetXlatSgr(tty);
                break;
              case 21:
                tty->pr |= kTtyUnder | kTtyDunder;
                break;
              case 22:
                tty->pr &= ~(kTtyFaint | kTtyBold);
                tty->xlat = GetXlatSgr(tty);
                break;
              case 23:
                tty->pr &= ~kTtyItalic;
                tty->xlat = GetXlatSgr(tty);
                break;
              case 24:
                tty->pr &= ~(kTtyUnder | kTtyDunder);
                break;
              case 25:
                tty->pr &= ~kTtyBlink;
                break;
              case 27:
                tty->pr &= ~kTtyFlip;
                break;
              case 28:
                tty->pr &= ~kTtyConceal;
                break;
              case 29:
                tty->pr &= ~kTtyStrike;
                break;
              case 39:
                tty->fg = DEFAULT_FG;
                tty->pr &= ~kTtyFg;
                break;
              case 49:
                tty->bg = DEFAULT_BG;
                tty->pr &= ~kTtyBg;
                break;
              case 90 ... 97:
                code[0] -= 90 - 30;
                code[0] += 8;
                /* fallthrough */
              case 30 ... 37:
                tty->fg = TtyMapEcma48Color(code[0] - 30);
                tty->pr |= kTtyFg;
                tty->pr &= ~kTtyTrue;
                break;
              case 100 ... 107:
                code[0] -= 100 - 40;
                code[0] += 8;
                /* fallthrough */
              case 40 ... 47:
                tty->bg = TtyMapEcma48Color(code[0] - 40);
                tty->pr |= kTtyBg;
                tty->pr &= ~kTtyTrue;
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
              tty->fg = TtyMapTrueColor(code[0], code[1], code[2]);
              tty->pr |= kTtyFg;
              tty->pr |= kTtyTrue;
            }
            break;
          case kSgrBgTrue:
            if (++code[3] == 3) {
              code[3] = 0;
              t = kSgr;
              tty->bg = TtyMapTrueColor(code[0], code[1], code[2]);
              tty->pr |= kTtyBg;
              tty->pr |= kTtyTrue;
            }
            break;
          case kSgrFgXterm:
            t = kSgr;
            tty->fg = TtyMapXtermColor(code[0]);
            tty->pr |= kTtyFg;
            tty->pr &= ~kTtyTrue;
            break;
          case kSgrBgXterm:
            t = kSgr;
            tty->bg = TtyMapXtermColor(code[0]);
            tty->pr |= kTtyBg;
            tty->pr &= ~kTtyTrue;
            break;
          default:
            __builtin_trap();
        }
        break;
      default:
        break;
    }
  }
}

static void TtyCsi(struct Tty *tty) {
  switch (tty->esc.s[tty->esc.i - 1]) {
    case 'f':
    case 'H':
      TtySetCursorPosition(tty);
      break;
    case 'G':
      TtySetCursorColumn(tty);
      break;
    case 'd':
      TtySetCursorRow(tty);
      break;
    case 'F':
      tty->x = 0;
      /* fallthrough */
    case 'A':
      TtyMoveCursor(tty, -1, +0);
      break;
    case 'E':
      tty->x = 0;
      /* fallthrough */
    case 'B':
      TtyMoveCursor(tty, +1, +0);
      break;
    case 'C':
      TtyMoveCursor(tty, +0, +1);
      break;
    case 'D':
      TtyMoveCursor(tty, +0, -1);
      break;
    case 'S':
      TtyScrollUp(tty);
      break;
    case 'T':
      TtyScrollDown(tty);
      break;
    case '@':
      TtyInsertCells(tty);
      break;
    case 'P':
      TtyDeleteCells(tty);
      break;
    case 'L':
      TtyInsertLines(tty);
      break;
    case 'M':
      TtyDeleteLines(tty);
      break;
    case 'J':
      TtyEraseDisplay(tty);
      break;
    case 'K':
      TtyEraseLine(tty);
      break;
    case 'X':
      TtyEraseCells(tty);
      break;
    case 's':
      TtySaveCursorPosition(tty);
      break;
    case 'u':
      TtyRestoreCursorPosition(tty);
      break;
    case 'n':
      TtyCsiN(tty);
      break;
    case 'm':
      TtySelectGraphicsRendition(tty);
      break;
    case 'h':
      TtySetMode(tty, true);
      break;
    case 'l':
      TtySetMode(tty, false);
      break;
    case 'c':
      TtyReportPreferredVtType(tty);
      break;
    case 'q':
      TtyLed(tty);
      break;
    default:
      break;
  }
}

static void TtyScreenAlignmentDisplay(struct Tty *tty) {
  size_t yn = Yn(tty), xn = Xn(tty), y, x;
  for (y = 0; y < yn; ++y)
    for (x = 0; x < xn; ++x) tty->drawchar(tty, y, x, 'E');
  if (Wcs(tty)) wmemset(Wcs(tty), L'E', yn * xn);
}

static void TtyEscHash(struct Tty *tty) {
  switch (tty->esc.s[1]) {
    case '5':
      TtySetXlat(tty, GetXlatAscii());
      break;
    case '6':
      TtySetXlat(tty, GetXlatDoubleWidth());
      break;
    case '8':
      TtyScreenAlignmentDisplay(tty);
      break;
    default:
      break;
  }
}

static void TtyEsc(struct Tty *tty) {
  switch (tty->esc.s[0]) {
    case 'c':
      _TtyFullReset(tty);
      break;
    case '7':
      TtySaveCursorPosition(tty);
      break;
    case '8':
      TtyRestoreCursorPosition(tty);
      break;
    case 'E':
      tty->x = 0;
    case 'D':
      TtyIndex(tty);
      break;
    case 'M':
      TtyReverseIndex(tty);
      break;
    case 'Z':
      TtyReportPreferredVtIdentity(tty);
      break;
    case '(':
      TtySetCodepage(tty, tty->esc.s[1]);
      break;
    case '#':
      TtyEscHash(tty);
      break;
    default:
      break;
  }
}

static void TtyCntrl(struct Tty *tty, int c01) {
  switch (c01) {
    case '\a':
      TtyBell(tty);
      break;
    case 0x85:
    case '\f':
    case '\v':
    case '\n':
      TtyNewline(tty);
      break;
    case '\r':
      TtyCarriageReturn(tty);
      break;
    case '\e':
      tty->state = kTtyEsc;
      tty->esc.i = 0;
      break;
    case '\t':
      TtyWriteTab(tty);
      break;
    case 0x7F:
    case '\b':
      tty->x = MAX(0, tty->x - 1);
      break;
    case 0x84:
      TtyIndex(tty);
      break;
    case 0x8D:
      TtyReverseIndex(tty);
      break;
    case 0x9B:
      tty->state = kTtyCsi;
      break;
    default:
      break;
  }
}

static void TtyEscAppend(struct Tty *tty, char c) {
  tty->esc.i = MIN(tty->esc.i + 1, ARRAYLEN(tty->esc.s) - 1);
  tty->esc.s[tty->esc.i - 1] = c;
  tty->esc.s[tty->esc.i - 0] = 0;
}

static void TtyUpdate(struct Tty *tty) {
  if (tty->type == PC_VIDEO_TEXT) {
    unsigned char start = tty->yc - 2, end = tty->yc - 1;
    unsigned short pos = tty->y * Xn(tty) + tty->x;
    if ((tty->conf & kTtyNocursor)) start |= 1 << 5;
    outb(CRTPORT, 0x0A);
    outb(CRTPORT + 1, start);
    outb(CRTPORT, 0x0B);
    outb(CRTPORT + 1, end);
    outb(CRTPORT, 0x0E);
    outb(CRTPORT + 1, (unsigned char)(pos >> 8));
    outb(CRTPORT, 0x0F);
    outb(CRTPORT + 1, (unsigned char)pos);
  } else
    tty->update(tty);
}

ssize_t _TtyWrite(struct Tty *tty, const void *data, size_t n) {
  int i;
  wchar_t wc;
  const uint8_t *p;
  for (p = data, i = 0; i < n; ++i) {
    switch (tty->state) {
      case kTtyAscii:
        if (0x00 <= p[i] && p[i] <= 0x7F) {
          if (0x20 <= p[i] && p[i] <= 0x7E) {
            if ((wc = tty->xlat[p[i]]) >= 0) {
              TtyWriteGlyph(tty, wc, 1);
            } else {
              TtyWriteGlyph(tty, -wc, 2);
            }
          } else {
            TtyCntrl(tty, p[i]);
          }
        } else if (!ThomPikeCont(p[i])) {
          tty->state = kTtyUtf8;
          tty->u8 = ThomPikeByte(p[i]);
          tty->n8 = ThomPikeLen(p[i]) - 1;
        }
        break;
      case kTtyUtf8:
        if (ThomPikeCont(p[i])) {
          tty->u8 = ThomPikeMerge(tty->u8, p[i]);
          if (--tty->n8) break;
        }
        wc = tty->u8;
        if ((0x00 <= wc && wc <= 0x1F) || (0x7F <= wc && wc <= 0x9F)) {
          TtyCntrl(tty, wc);
        } else {
          TtyWriteGlyph(tty, wc, wcwidth(wc));
        }
        tty->state = kTtyAscii;
        tty->u8 = 0;
        --i;
        break;
      case kTtyEsc:
        if (p[i] == '[') {
          tty->state = kTtyCsi;
        } else if (0x30 <= p[i] && p[i] <= 0x7E) {
          TtyEscAppend(tty, p[i]);
          TtyEsc(tty);
          tty->state = kTtyAscii;
        } else if (0x20 <= p[i] && p[i] <= 0x2F) {
          TtyEscAppend(tty, p[i]);
        } else {
          tty->state = kTtyAscii;
        }
        break;
      case kTtyCsi:
        TtyEscAppend(tty, p[i]);
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
            TtyCsi(tty);
            tty->state = kTtyAscii;
            break;
          default:
            tty->state = kTtyAscii;
            continue;
        }
        break;
      default:
        __builtin_unreachable();
    }
  }
  TtyUpdate(tty);
  return n;
}

ssize_t _TtyWriteInput(struct Tty *tty, const void *data, size_t n) {
  int c;
  bool cr;
  char *p;
  size_t i, j;
  const char *q;
  q = data;
  p = tty->input.p;
  i = tty->input.i;
  cr = i && p[i - 1] == '\r';
  for (j = 0; j < n && i < sizeof tty->input.p; ++j) {
    c = q[j] & 255;
    if (c == '\r') {
      cr = true;
    } else if (cr) {
      if (c != '\n') {
        p[i++] = '\n';
      }
      cr = false;
    }
    p[i++] = c;
  }
  if (cr && i < sizeof tty->input.p) {
    p[i++] = '\n';
  }
#ifdef VGA_PERSNICKETY_STATUS
  if (!(tty->conf & kTtyNoecho)) {
    _TtyWrite(tty, p + tty->input.i, i - tty->input.i);
  }
#endif
  tty->input.i = i;
  return n;
}

ssize_t _TtyRead(struct Tty *tty, void *buf, size_t size) {
  size_t n;
  n = MIN(size, tty->input.i);
#ifdef VGA_PERSNICKETY_STATUS
  if (!(tty->conf & kTtyNocanon)) {
    if ((p = memchr(tty->input.p, '\n', n))) {
      n = MIN(n, tty->input.p - p + 1);
    } else {
      n = 0;
    }
  }
#endif
  memcpy(buf, tty->input.p, n);
  memcpy(tty->input.p, tty->input.p + n, tty->input.i - n);
  tty->input.i -= n;
  return n;
}

#endif /* __x86_64__ */
