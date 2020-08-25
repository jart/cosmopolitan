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
#include "dsp/tty/internal.h"
#include "dsp/tty/quant.h"
#include "libc/bits/initializer.h"
#include "libc/dce.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

struct TtyQuant g_ttyquant_;

/**
 * Chooses xterm quantization mode.
 */
textstartup void ttyquantsetup(enum TtyQuantizationAlgorithm alg,
                               enum TtyQuantizationChannels chans,
                               enum TtyBlocksSelection blocks) {
  switch (alg) {
    case kTtyQuantAnsi:
      TTYQUANT()->rgb2tty = rgb2ansi_;
      TTYQUANT()->rgb2ttyf = rgb2ttyf2i_;
      TTYQUANT()->tty2rgb = tty2rgb_;
      TTYQUANT()->tty2rgbf = tty2rgbf_;
      TTYQUANT()->setbg = setbg16_;
      TTYQUANT()->setfg = setfg16_;
      TTYQUANT()->setbgfg = setbgfg16_;
      TTYQUANT()->min = 0;
      TTYQUANT()->max = 16;
      break;
    case kTtyQuantTrue:
      TTYQUANT()->rgb2tty = rgb2xterm24_;
      TTYQUANT()->rgb2ttyf = rgb2tty24f_;
      TTYQUANT()->tty2rgb = tty2rgb24_;
      TTYQUANT()->tty2rgbf = tty2rgbf24_;
      TTYQUANT()->setbg = setbg24_;
      TTYQUANT()->setfg = setfg24_;
      TTYQUANT()->setbgfg = setbgfg24_;
      TTYQUANT()->min = 16;
      TTYQUANT()->max = 256;
      break;
    case kTtyQuantXterm256:
      TTYQUANT()->rgb2tty = rgb2ansi_;
      TTYQUANT()->rgb2ttyf = rgb2ttyf2i_;
      TTYQUANT()->tty2rgb = tty2rgb_;
      TTYQUANT()->tty2rgbf = tty2rgbf_;
      TTYQUANT()->setbg = setbg256_;
      TTYQUANT()->setfg = setfg256_;
      TTYQUANT()->setbgfg = setbgfg256_;
      TTYQUANT()->min = 16;
      TTYQUANT()->max = 256;
      break;
    default:
      abort();
  }
  TTYQUANT()->chans = chans;
  TTYQUANT()->alg = alg;
  TTYQUANT()->blocks = blocks;
}

textstartup void ttyquant_init(void) {
  ttyquantsetup(kTtyQuantXterm256, kTtyQuantRgb, kTtyBlocksUnicode);
}

const void *const ttyquant_init_ctor[] initarray = {ttyquant_init};
