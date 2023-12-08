/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/stdckdint.h"
#include "libc/str/str.h"
#include "libc/vga/vga.internal.h"

/**
 * @internal
 * @fileoverview Template for routines to support output in graphical video
 * modes for bare metal VGA.
 *
 * If KLOGTTY is undefined, this file expands to code that writes to a
 * separate off-screen canvas buffer — which should be allocated beforehand
 * — before drawing to the actual video memory.
 *
 * If KLOGTTY is defined, this file instead expands to code that can
 * implement an emergency console.  The graphics routines will directly write
 * to video memory.  This may result in slower output, but will work even if
 * we cannot allocate an off-screen canvas for whatever reason.
 *
 * @see libc/vga/tty.greg.c
 * @see libc/vga/tty-graph.c
 * @see libc/vga/tty-klog.greg.c
 */

#ifndef KLOGTTY

static COLOR MAPCOLOR(struct Tty *tty, TtyCanvasColor ic) {
  return ic;
}

static void DIRTY(struct Tty *tty, size_t gy1, size_t gx1, size_t gy2,
                  size_t gx2) {
  if (tty->updy1 > gy1) tty->updy1 = gy1;
  if (tty->updx1 > gx1) tty->updx1 = gx1;
  if (tty->updy2 < gy2) tty->updy2 = gy2;
  if (tty->updx2 < gx2) tty->updx2 = gx2;
}

static void RESETDIRTY(struct Tty *tty) {
  tty->updy1 = tty->updx1 = tty->updy2 = tty->updx2 = 0;
}

unrollloops void _TtyBgr565Update(struct Tty *tty) {
  size_t gy1 = tty->updy1, gy2 = tty->updy2, gx1 = tty->updx1, gx2 = tty->updx2,
         xsfb = tty->xsfb, xs = tty->xs;
  if (gy1 < gy2 && gx1 < gx2) {
    size_t yleft = gy2 - gy1, xleft;
    char *cplotter = tty->fb + gy1 * xsfb + gx1 * sizeof(TtyBgr565Color);
    const char *creader = tty->canvas + gy1 * xs + gx1 * sizeof(TtyCanvasColor);
    RESETDIRTY(tty);
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
  size_t gy1 = tty->updy1, gy2 = tty->updy2, gx1 = tty->updx1, gx2 = tty->updx2,
         xsfb = tty->xsfb, xs = tty->xs;
  if (gy1 < gy2 && gx1 < gx2) {
    size_t yleft = gy2 - gy1, xleft;
    char *cplotter = tty->fb + gy1 * xsfb + gx1 * sizeof(TtyBgr555Color);
    const char *creader = tty->canvas + gy1 * xs + gx1 * sizeof(TtyCanvasColor);
    RESETDIRTY(tty);
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
  size_t gy1 = tty->updy1, gy2 = tty->updy2, gx1 = tty->updx1, gx2 = tty->updx2,
         xsfb = tty->xsfb, xs = tty->xs;
  if (gy1 < gy2 && gx1 < gx2) {
    size_t yleft = gy2 - gy1, xleft;
    char *cplotter = tty->fb + gy1 * xsfb + gx1 * sizeof(TtyBgrxColor);
    const char *creader = tty->canvas + gy1 * xs + gx1 * sizeof(TtyCanvasColor);
    RESETDIRTY(tty);
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
  size_t gy1 = tty->updy1, gy2 = tty->updy2, gx1 = tty->updx1, gx2 = tty->updx2,
         xsfb = tty->xsfb, xs = tty->xs;
  if (gy1 < gy2 && gx1 < gx2) {
    size_t yleft = gy2 - gy1, xleft;
    char *cplotter = tty->fb + gy1 * xsfb + gx1 * sizeof(TtyRgbxColor);
    const char *creader = tty->canvas + gy1 * xs + gx1 * sizeof(TtyCanvasColor);
    RESETDIRTY(tty);
    while (yleft-- != 0) {
      TtyRgbxColor *plotter = (TtyRgbxColor *)cplotter;
      const TtyCanvasColor *reader = (const TtyCanvasColor *)creader;
      TtyCanvasColor ic;
      TtyRgbxColor oc;
      xleft = gx2 - gx1;
      while (xleft-- != 0) {
        ic.w = reader->w;
        ++reader;
        oc = (TtyRgbxColor){.rgb.r = ic.bgr.r,
                            .rgb.g = ic.bgr.g,
                            .rgb.b = ic.bgr.b,
                            .rgb.x = 0xff};
        plotter->w = oc.w;
        ++plotter;
      }
      cplotter += xsfb;
      creader += xs;
    }
  }
}

#else /* KLOGTTY */

static COLOR MAPCOLOR(struct Tty *tty, TtyCanvasColor ic) {
#if BPP == 16
  if (tty->type == PC_VIDEO_BGR565)
    return htole16(ic.bgr.b >> 3 | ic.bgr.g >> 2 << 5 | ic.bgr.r >> 3 << 11);
  else
    return htole16(ic.bgr.b >> 3 | ic.bgr.g >> 3 << 5 | ic.bgr.r >> 3 << 10);
#else
  if (tty->type == PC_VIDEO_BGRX8888)
    return ic.w;
  else {
    TtyRgbxColor oc = (TtyRgbxColor){
        .rgb.r = ic.bgr.r, .rgb.g = ic.bgr.g, .rgb.b = ic.bgr.b, .rgb.x = 0xff};
    return oc.w;
  }
#endif
}

static void DIRTY(struct Tty *tty, size_t gy1, size_t gx1, size_t gy2,
                  size_t gx2) {
}

static void RESETDIRTY(struct Tty *tty) {
}

void UPDATE(struct Tty *tty) {
}

#endif /* KLOGTTY */

static void DRAWBITMAP(struct Tty *tty, size_t gy, size_t gx, COLOR fg,
                       COLOR bg, const uint8_t *bitmap, size_t bm_ht,
                       size_t bm_wid) {
  size_t xs = tty->xs;
  char *cplotter = tty->canvas + gy * xs + gx * sizeof(COLOR);
  size_t yleft = bm_ht, xleft;
  while (yleft-- != 0) {
    COLOR *plotter = (COLOR *)cplotter;
    xleft = bm_wid;
    while (xleft >= 8) {
      uint8_t bits = *bitmap++;
      *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
      *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
      *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
      *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
      *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
      *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
      *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
      *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
      xleft -= 8;
    }
    if (xleft) {
      uint8_t bits = *bitmap++;
      switch (xleft) {
        default:
          *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
          /* fall through */
        case 6:
          *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
          /* fall through */
        case 5:
          *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
          /* fall through */
        case 4:
          *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
          /* fall through */
        case 3:
          *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
          /* fall through */
        case 2:
          *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
          /* fall through */
        case 1:
          *plotter++ = ckd_add(&bits, bits, bits) ? fg : bg;
      }
    }
    cplotter += xs;
  }
  DIRTY(tty, gy, gx, gy + bm_ht, gx + bm_wid);
}

static MAYUNROLLLOOPS void FILLRECT(struct Tty *tty, size_t gy, size_t gx,
                                    size_t fill_ht, size_t fill_wid, COLOR bg) {
  size_t xs = tty->xs;
  char *cplotter = tty->canvas + gy * xs + gx * sizeof(COLOR);
  size_t yleft = fill_ht;
  while (yleft-- != 0) {
    COLOR *plotter = (COLOR *)cplotter;
    size_t i;
    for (i = 0; i < fill_wid; ++i) *plotter++ = bg;
    cplotter += xs;
  }
  DIRTY(tty, gy, gx, gy + fill_ht, gx + fill_wid);
}

static void MOVERECT(struct Tty *tty, size_t dgy, size_t dgx, size_t sgy,
                     size_t sgx, size_t ht, size_t wid) {
  size_t xs = tty->xs, xm = wid * sizeof(COLOR);
  char *canvas = tty->canvas;
  DIRTY(tty, dgy, dgx, dgy + ht, dgx + wid);
  if (dgy < sgy) {
    while (ht-- != 0) {
      memmove(canvas + dgy * xs + dgx * sizeof(COLOR),
              canvas + sgy * xs + sgx * sizeof(COLOR), xm);
      ++dgy;
      ++sgy;
    }
  } else if (dgy > sgy) {
    while (ht-- != 0)
      memmove(canvas + (dgy + ht) * xs + dgx * sizeof(COLOR),
              canvas + (sgy + ht) * xs + sgx * sizeof(COLOR), xm);
  }
}

void DRAWCHAR(struct Tty *tty, size_t y, size_t x, wchar_t wc) {
  /* TODO: allow configuring a different font later. */
  const uint8_t *glyph;
  const size_t glyph_ht = ARRAYLEN(_vga_font_default_direct[0]);
  COLOR fg = MAPCOLOR(tty, tty->fg), bg = MAPCOLOR(tty, tty->bg);
  if ((tty->pr & kTtyFlip) != 0) fg = bg, bg = MAPCOLOR(tty, tty->fg);
  if (wc < L' ' || wc >= L' ' + ARRAYLEN(_vga_font_default_direct))
    glyph = _vga_font_default_direct[0];
  else
    glyph = _vga_font_default_direct[wc - L' '];
  if (glyph_ht >= VGA_ASSUME_CHAR_HEIGHT_PX)
    DRAWBITMAP(tty, y * tty->yc, x * tty->xc, fg, bg, glyph,
               VGA_ASSUME_CHAR_HEIGHT_PX, 8);
  else {
    /*
     * Glyph is not tall enough.  Draw it out, then pad the bottom of the
     * character cell with the background color.
     */
    DRAWBITMAP(tty, y * tty->yc, x * tty->xc, fg, bg, glyph, glyph_ht, 8);
    FILLRECT(tty, y * tty->yc + glyph_ht, x * tty->xc,
             VGA_ASSUME_CHAR_HEIGHT_PX - glyph_ht, 8, bg);
  }
}

void ERASELINECELLS(struct Tty *tty, size_t y, size_t x, size_t n) {
  size_t yc = tty->yc, xc = tty->xc;
  FILLRECT(tty, y * yc, x * xc, yc, n * xc, MAPCOLOR(tty, tty->bg));
}

void MOVELINECELLS(struct Tty *tty, size_t dsty, size_t dstx, size_t srcy,
                   size_t srcx, size_t n) {
  size_t yc = tty->yc, xc = tty->xc;
  MOVERECT(tty, dsty * yc, dstx * xc, srcy * yc, srcx * xc, yc, n * xc);
}
