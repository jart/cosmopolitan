/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/intrin/newbie.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "libc/vga/vga.internal.h"

/**
 * @fileoverview Routines to support output in graphical video modes for
 * bare metal VGA.
 *
 * @see libc/vga/tty.c
 */

static void TtyDirty(struct Tty *tty, size_t gy1, size_t gx1,
                                      size_t gy2, size_t gx2) {
  if (tty->updy1 > gy1) tty->updy1 = gy1;
  if (tty->updx1 > gx1) tty->updx1 = gx1;
  if (tty->updy2 < gy2) tty->updy2 = gy2;
  if (tty->updx2 < gx2) tty->updx2 = gx2;
}

static void TtyResetDirty(struct Tty *tty) {
  tty->updy1 = tty->updx1 = tty->updy2 = tty->updx2 = 0;
}

unrollloops void _TtyBgr565Update(struct Tty *tty) {
  size_t gy1 = tty->updy1, gy2 = tty->updy2,
         gx1 = tty->updx1, gx2 = tty->updx2,
         xsfb = tty->xsfb, xs = tty->xs;
  if (gy1 < gy2 && gx1 < gx2) {
    size_t yleft = gy2 - gy1, xleft;
    char *cplotter = tty->fb + gy1 * xsfb + gx1 * sizeof(TtyBgr565Color);
    const char *creader = tty->canvas + gy1 * xs
                                      + gx1 * sizeof(TtyCanvasColor);
    TtyResetDirty(tty);
    while (yleft-- != 0) {
      TtyBgr565Color *plotter = (TtyBgr565Color *)cplotter;
      const TtyCanvasColor *reader = (const TtyCanvasColor *)creader;
      TtyCanvasColor c;
      xleft = gx2 - gx1;
      while (xleft-- != 0) {
        uint16_t w;
        c.w = reader->w;
        ++reader;
        w = htole16(c.bgr.b >> 3 | c.bgr.g >> 2 << 5 | c.bgr.r >> 3 << 11);
        *plotter++ = (TtyBgr565Color){w};
      }
      cplotter += xsfb;
      creader += xs;
    }
  }
}

unrollloops void _TtyBgr555Update(struct Tty *tty) {
  size_t gy1 = tty->updy1, gy2 = tty->updy2,
         gx1 = tty->updx1, gx2 = tty->updx2,
         xsfb = tty->xsfb, xs = tty->xs;
  if (gy1 < gy2 && gx1 < gx2) {
    size_t yleft = gy2 - gy1, xleft;
    char *cplotter = tty->fb + gy1 * xsfb + gx1 * sizeof(TtyBgr555Color);
    const char *creader = tty->canvas + gy1 * xs
                                      + gx1 * sizeof(TtyCanvasColor);
    TtyResetDirty(tty);
    while (yleft-- != 0) {
      TtyBgr555Color *plotter = (TtyBgr555Color *)cplotter;
      const TtyCanvasColor *reader = (const TtyCanvasColor *)creader;
      TtyCanvasColor c;
      xleft = gx2 - gx1;
      while (xleft-- != 0) {
        uint16_t w;
        c.w = reader->w;
        ++reader;
        w = htole16(c.bgr.b >> 3 | c.bgr.g >> 3 << 5 | c.bgr.r >> 3 << 10);
        *plotter++ = (TtyBgr555Color){w};
      }
      cplotter += xsfb;
      creader += xs;
    }
  }
}

unrollloops void _TtyBgrxUpdate(struct Tty *tty) {
  size_t gy1 = tty->updy1, gy2 = tty->updy2,
         gx1 = tty->updx1, gx2 = tty->updx2,
         xsfb = tty->xsfb, xs = tty->xs;
  if (gy1 < gy2 && gx1 < gx2) {
    size_t yleft = gy2 - gy1, xleft;
    char *cplotter = tty->fb + gy1 * xsfb + gx1 * sizeof(TtyBgrxColor);
    const char *creader = tty->canvas + gy1 * xs
                                      + gx1 * sizeof(TtyCanvasColor);
    TtyResetDirty(tty);
    while (yleft-- != 0) {
      TtyBgrxColor *plotter = (TtyBgrxColor *)cplotter;
      const TtyCanvasColor *reader = (const TtyCanvasColor *)creader;
      xleft = gx2 - gx1;
      while (xleft-- != 0) {
        TtyCanvasColor c = *reader++;
        c.bgr.x = 0xff;
        *plotter++ = c;
      }
      cplotter += xsfb;
      creader += xs;
    }
  }
}

unrollloops void _TtyRgbxUpdate(struct Tty *tty) {
  size_t gy1 = tty->updy1, gy2 = tty->updy2,
         gx1 = tty->updx1, gx2 = tty->updx2,
         xsfb = tty->xsfb, xs = tty->xs;
  if (gy1 < gy2 && gx1 < gx2) {
    size_t yleft = gy2 - gy1, xleft;
    char *cplotter = tty->fb + gy1 * xsfb + gx1 * sizeof(TtyRgbxColor);
    const char *creader = tty->canvas + gy1 * xs
                                      + gx1 * sizeof(TtyCanvasColor);
    TtyResetDirty(tty);
    while (yleft-- != 0) {
      TtyRgbxColor *plotter = (TtyRgbxColor *)cplotter;
      const TtyCanvasColor *reader = (const TtyCanvasColor *)creader;
      TtyCanvasColor ic;
      TtyRgbxColor oc;
      xleft = gx2 - gx1;
      while (xleft-- != 0) {
        ic.w = reader->w;
        ++reader;
        oc = (TtyRgbxColor){.rgb.r = ic.bgr.r, .rgb.g = ic.bgr.g,
                            .rgb.b = ic.bgr.b, .rgb.x = 0xff};
        plotter->w = oc.w;
        ++plotter;
      }
      cplotter += xsfb;
      creader += xs;
    }
  }
}

static void TtyGraphDrawBitmap(struct Tty *tty, size_t gy, size_t gx,
                               TtyCanvasColor fg, TtyCanvasColor bg,
                               const uint8_t *bitmap,
                               size_t bm_ht, size_t bm_wid) {
  size_t xs = tty->xs;
  char *cplotter = tty->canvas + gy * xs
                               + gx * sizeof(TtyCanvasColor);
  size_t yleft = bm_ht, xleft;
  while (yleft-- != 0) {
    TtyCanvasColor *plotter = (TtyCanvasColor *)cplotter;
    xleft = bm_wid;
    while (xleft >= 8) {
      uint8_t bits = *bitmap++;
      *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
      *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
      *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
      *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
      *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
      *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
      *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
      *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
      xleft -= 8;
    }
    if (xleft) {
      uint8_t bits = *bitmap++;
      switch (xleft) {
        default:
          *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
          /* fall through */
        case 6:
          *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
          /* fall through */
        case 5:
          *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
          /* fall through */
        case 4:
          *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
          /* fall through */
        case 3:
          *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
          /* fall through */
        case 2:
          *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
          /* fall through */
        case 1:
          *plotter++ = __builtin_add_overflow(bits, bits, &bits) ? fg : bg;
      }
    }
    cplotter += xs;
  }
  TtyDirty(tty, gy, gx, gy + bm_ht, gx + bm_wid);
}

static unrollloops void TtyGraphFillRect(struct Tty *tty,
                                         size_t gy, size_t gx,
                                         size_t fill_ht, size_t fill_wid,
                                         TtyCanvasColor bg) {
  size_t xs = tty->xs;
  char *cplotter = tty->canvas + gy * xs + gx * sizeof(TtyCanvasColor);
  size_t yleft = fill_ht, xleft;
  while (yleft-- != 0) {
    TtyCanvasColor *plotter = (TtyCanvasColor *)cplotter;
    size_t i;
    for (i = 0; i < fill_wid; ++i)
      *plotter++ = bg;
    cplotter += xs;
  }
  TtyDirty(tty, gy, gx, gy + fill_ht, gx + fill_wid);
}

static void TtyGraphMoveRect(struct Tty *tty, size_t dgy, size_t dgx,
                             size_t sgy, size_t sgx, size_t ht, size_t wid) {
  size_t xs = tty->xs, xm = wid * sizeof(TtyCanvasColor);
  char *canvas = tty->canvas;
  TtyDirty(tty, dgy, dgx, dgy + ht, dgx + wid);
  if (dgy < sgy) {
    while (ht-- != 0) {
      memmove(canvas + dgy * xs + dgx * sizeof(TtyCanvasColor),
              canvas + sgy * xs + sgx * sizeof(TtyCanvasColor), xm);
      ++dgy;
      ++sgy;
    }
  } else if (dgy > sgy) {
    while (ht-- != 0)
      memmove(canvas + (dgy + ht) * xs + dgx * sizeof(TtyCanvasColor),
              canvas + (sgy + ht) * xs + sgx * sizeof(TtyCanvasColor), xm);
  }
}

void _TtyGraphDrawChar(struct Tty *tty, size_t y, size_t x, wchar_t wc) {
  /* TODO: allow configuring a different font later. */
  const uint8_t *glyph;
  const size_t glyph_ht = ARRAYLEN(_vga_font_default_direct[0]);
  TtyCanvasColor fg = tty->fg, bg = tty->bg;
  if ((tty->pr & kTtyFlip) != 0)
    fg = bg, bg = tty->fg;
  if (wc < L' ' || wc >= L' ' + ARRAYLEN(_vga_font_default_direct))
    glyph = _vga_font_default_direct[0];
  else
    glyph = _vga_font_default_direct[wc - L' '];
  if (glyph_ht >= VGA_ASSUME_CHAR_HEIGHT_PX)
    TtyGraphDrawBitmap(tty, y * tty->yc, x * tty->xc, fg, bg,
                       glyph, VGA_ASSUME_CHAR_HEIGHT_PX, 8);
  else {
    /*
     * Glyph is not tall enough.  Draw it out, then pad the bottom of the
     * character cell with the background color.
     */
    TtyGraphDrawBitmap(tty, y * tty->yc, x * tty->xc, fg, bg,
                       glyph, glyph_ht, 8);
    TtyGraphFillRect(tty, y * tty->yc + glyph_ht, x * tty->xc,
                     VGA_ASSUME_CHAR_HEIGHT_PX - glyph_ht, 8, bg);
  }
}

void _TtyGraphEraseLineCells(struct Tty *tty, size_t y, size_t x, size_t n) {
  size_t yc = tty->yc, xc = tty->xc;
  TtyGraphFillRect(tty, y * yc, x * xc, yc, n * xc, tty->bg);
}

void _TtyGraphMoveLineCells(struct Tty *tty, size_t dsty, size_t dstx,
                            size_t srcy, size_t srcx, size_t n) {
  size_t yc = tty->yc, xc = tty->xc;
  TtyGraphMoveRect(tty, dsty * yc, dstx * xc, srcy * yc, srcx * xc,
                   yc, n * xc);
}
