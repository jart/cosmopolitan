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
#include "libc/nexgen32e/kompressor.h"
#include "libc/nexgen32e/lz4.h"

const unsigned char *lz4check(const void *data) {
  const unsigned char *frame = data;
  if (LZ4_MAGIC(frame) == LZ4_MAGICNUMBER && LZ4_FRAME_VERSION(frame) == 1 &&
      LZ4_FRAME_BLOCKINDEPENDENCE(frame) == true &&
      LZ4_FRAME_BLOCKCONTENTSIZEFLAG(frame) == true &&
      LZ4_FRAME_RESERVED1(frame) == 0 && LZ4_FRAME_RESERVED2(frame) == 0 &&
      LZ4_FRAME_RESERVED3(frame) == 0) {
    return frame;
  } else {
    return NULL;
  }
}
