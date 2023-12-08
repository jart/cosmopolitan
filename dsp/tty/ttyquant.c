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
#include "dsp/tty/internal.h"
#include "dsp/tty/quant.h"
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
