/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:4;tab-width:4;coding:utf-8   -*-│
│ vi: set et ft=c ts=4 sw=4 fenc=utf-8                                     :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│  PL_MPEG - MPEG1 Video decoder, MP2 Audio decoder, MPEG-PS demuxer           │
│  Dominic Szablewski - https://phoboslab.org                                  │
│                                                                              │
│  The MIT License(MIT)                                                        │
│  Copyright(c) 2019 Dominic Szablewski                                        │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files(the              │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and / or sell copies of the Software, and to        │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│    The above copyright notice and this permission notice shall be            │
│    included in all copies or substantial portions of the Software.           │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                       │
│  NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE       │
│  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN             │
│  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN           │
│  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE            │
│  SOFTWARE.                                                                   │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "dsp/mpeg/mpeg.h"
#include "libc/macros.internal.h"

asm(".ident\t\"\\n\\n\
PL_MPEG (MIT License)\\n\
Copyright(c) 2019 Dominic Szablewski\\n\
https://phoboslab.org\"");
asm(".include \"libc/disclaimer.inc\"");

/**
 * @see YCbCr2RGB() in tool/viz/lib/ycbcr2rgb.c
 */
void plm_frame_to_rgb(plm_frame_t *frame, uint8_t *rgb) {
  // Chroma values are the same for each block of 4 pixels, so we process
  // 2 lines at a time, 2 neighboring pixels each.
  int w = frame->y.width, w2 = w >> 1;
  int y_index1 = 0, y_index2 = w, y_next_2_lines = w + (w - frame->width);
  int c_index = 0, c_next_line = w2 - (frame->width >> 1);
  int rgb_index1 = 0, rgb_index2 = frame->width * 3,
      rgb_next_2_lines = frame->width * 3;
  int cols = frame->width >> 1, rows = frame->height >> 1;
  int ccb, ccr, r, g, b;
  uint8_t *y = frame->y.data, *cb = frame->cb.data, *cr = frame->cr.data;
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      ccb = cb[c_index];
      ccr = cr[c_index];
      c_index++;
      r = (ccr + ((ccr * 103) >> 8)) - 179;
      g = ((ccb * 88) >> 8) - 44 + ((ccr * 183) >> 8) - 91;
      b = (ccb + ((ccb * 198) >> 8)) - 227;
      // Line 1
      int y1 = y[y_index1++];
      int y2 = y[y_index1++];
      rgb[rgb_index1 + 0] = MAX(0, MIN(255, y1 + r));
      rgb[rgb_index1 + 1] = MAX(0, MIN(255, y1 - g));
      rgb[rgb_index1 + 2] = MAX(0, MIN(255, y1 + b));
      rgb[rgb_index1 + 3] = MAX(0, MIN(255, y2 + r));
      rgb[rgb_index1 + 4] = MAX(0, MIN(255, y2 - g));
      rgb[rgb_index1 + 5] = MAX(0, MIN(255, y2 + b));
      rgb_index1 += 6;
      // Line 2
      int y3 = y[y_index2++];
      int y4 = y[y_index2++];
      rgb[rgb_index2 + 0] = MAX(0, MIN(255, y3 + r));
      rgb[rgb_index2 + 1] = MAX(0, MIN(255, y3 - g));
      rgb[rgb_index2 + 2] = MAX(0, MIN(255, y3 + b));
      rgb[rgb_index2 + 3] = MAX(0, MIN(255, y4 + r));
      rgb[rgb_index2 + 4] = MAX(0, MIN(255, y4 - g));
      rgb[rgb_index2 + 5] = MAX(0, MIN(255, y4 + b));
      rgb_index2 += 6;
    }
    y_index1 += y_next_2_lines;
    y_index2 += y_next_2_lines;
    rgb_index1 += rgb_next_2_lines;
    rgb_index2 += rgb_next_2_lines;
    c_index += c_next_line;
  }
}
