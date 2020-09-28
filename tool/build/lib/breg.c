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
#include "tool/build/lib/modrm.h"

/**
 * Byte register offsets.
 *
 * for (i = 0; i < 2; ++i) {      // rex
 *   for (j = 0; j < 2; ++j) {    // rexb, or rexr
 *     for (k = 0; k < 8; ++k) {  // reg, rm, or srm
 *       kByteReg[i << 4 | j << 3 | k] =
 *           i ? (j << 3 | k) * 8 : (k & 0b11) * 8 + ((k & 0b100) >> 2);
 *     }
 *   }
 * }
 */
const uint8_t kByteReg[32] = {0x00, 0x08, 0x10, 0x18, 0x01, 0x09, 0x11, 0x19,
                              0x00, 0x08, 0x10, 0x18, 0x01, 0x09, 0x11, 0x19,
                              0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38,
                              0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78};
