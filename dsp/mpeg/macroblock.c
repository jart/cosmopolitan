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
#include "dsp/mpeg/video.h"
#include "libc/log/check.h"

forceinline void plm_video_process_macroblock(plm_video_t *self,
                                              uint8_t *restrict d,
                                              uint8_t *restrict s, int motion_h,
                                              int motion_v, bool interpolate,
                                              unsigned BW) {
  unsigned si, di, max_address;
  int y, x, dest_scan, source_scan, dw, hp, vp, odd_h, odd_v;
  dw = self->mb_width * BW;
  hp = motion_h >> 1;
  vp = motion_v >> 1;
  odd_h = (motion_h & 1) == 1;
  odd_v = (motion_v & 1) == 1;
  si = ((self->mb_row * BW) + vp) * dw + (self->mb_col * BW) + hp;
  di = (self->mb_row * dw + self->mb_col) * BW;
  max_address = (dw * (self->mb_height * BW - BW + 1) - BW);
  if (si > max_address || di > max_address) return;
  d += di;
  s += si;
  switch (((interpolate << 2) | (odd_h << 1) | (odd_v)) & 7) {
    case 0:
      dest_scan = dw - BW;
      source_scan = dw - BW;
      for (y = 0; y < BW; y++) {
        for (x = 0; x < BW; x++) {
          *d++ = *s++;
        }
        s += source_scan;
        d += dest_scan;
      }
      break;
    case 1:
      dest_scan = dw - BW;
      source_scan = dw - BW;
      for (y = 0; y < BW; y++) {
        for (x = 0; x < BW; x++) {
          *d++ = (s[0] + s[dw] + 1) >> 1;
          s++;
        }
        s += source_scan;
        d += dest_scan;
      }
      break;
    case 2:
      dest_scan = dw - BW;
      source_scan = dw - BW;
      for (y = 0; y < BW; y++) {
        for (x = 0; x < BW; x++) {
          *d++ = (s[0] + s[1] + 1) >> 1;
          s++;
        }
        s += source_scan;
        d += dest_scan;
      }
      break;
    case 3:
      dest_scan = dw - BW;
      source_scan = dw - BW;
      for (y = 0; y < BW; y++) {
        for (x = 0; x < BW; x++) {
          *d++ = (s[0] + s[1] + s[dw] + s[dw + 1] + 2) >> 2;
          s++;
        }
        s += source_scan;
        d += dest_scan;
      }
      break;
    case 4:
      dest_scan = dw - BW;
      source_scan = dw - BW;
      for (y = 0; y < BW; y++) {
        for (x = 0; x < BW; x++) {
          d[0] = (d[0] + (s[0]) + 1) >> 1;
          d++;
          s++;
        }
        s += source_scan;
        d += dest_scan;
      }
      break;
    case 5:
      dest_scan = dw - BW;
      source_scan = dw - BW;
      for (y = 0; y < BW; y++) {
        for (x = 0; x < BW; x++) {
          d[0] = (d[0] + ((s[0] + s[dw] + 1) >> 1) + 1) >> 1;
          d++;
          s++;
        }
        s += source_scan;
        d += dest_scan;
      }
      break;
    case 6:
      dest_scan = dw - BW;
      source_scan = dw - BW;
      for (y = 0; y < BW; y++) {
        for (x = 0; x < BW; x++) {
          d[0] = (d[0] + ((s[0] + s[1] + 1) >> 1) + 1) >> 1;
          d++;
          s++;
        }
        s += source_scan;
        d += dest_scan;
      }
      break;
    case 7:
      dest_scan = dw - BW;
      source_scan = dw - BW;
      for (y = 0; y < BW; y++) {
        for (x = 0; x < BW; x++) {
          d[0] = (d[0] + ((s[0] + s[1] + s[dw] + s[dw + 1] + 2) >> 2) + 1) >> 1;
          d++;
          s++;
        }
        s += source_scan;
        d += dest_scan;
      }
      break;
    default:
      break;
  }
}

void plm_video_process_macroblock_8(plm_video_t *self, uint8_t *restrict d,
                                    uint8_t *restrict s, int motion_h,
                                    int motion_v, bool interpolate) {
  DCHECK_ALIGNED(8, d);
  DCHECK_ALIGNED(8, s);
  plm_video_process_macroblock(self, d, s, motion_h, motion_v, interpolate, 8);
}

void plm_video_process_macroblock_16(plm_video_t *self, uint8_t *restrict d,
                                     uint8_t *restrict s, int motion_h,
                                     int motion_v, bool interpolate) {
  DCHECK_ALIGNED(16, d);
  DCHECK_ALIGNED(16, s);
  plm_video_process_macroblock(self, d, s, motion_h, motion_v, interpolate, 16);
}
