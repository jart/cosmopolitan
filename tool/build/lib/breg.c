/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
