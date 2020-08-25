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
#include "libc/str/str.h"

/**
 * Decompresses LZ4 file.
 *
 * We assume (1) the file is mmap()'d or was copied into into memory
 * beforehand; and (2) folks handling untrustworthy data shall place
 * 64kb of guard pages on the ends of each buffer, see mapanon(). We
 * don't intend to support XXHASH; we recommend folks needing checks
 * against data corruption consider crc32c(), or gzip since it's the
 * best at file recovery. Dictionaries are supported; by convention,
 * they are passed in the ≤64kb bytes preceding src.
 *
 * @return pointer to end of decoded data, similar to mempcpy()
 * @see mapanon(), lz4check()
 */
void *lz4decode(void *dest, const void *src) {
  const unsigned char *frame, *block;
  frame = (const unsigned char *)src;
  for (block = frame + LZ4_FRAME_HEADERSIZE(frame); !LZ4_BLOCK_ISEOF(block);
       block += LZ4_BLOCK_SIZE(frame, block)) {
    if (LZ4_BLOCK_ISCOMPRESSED(block)) {
      dest = lz4cpy(dest, LZ4_BLOCK_DATA(block), LZ4_BLOCK_DATASIZE(block));
    } else {
      dest = mempcpy(dest, LZ4_BLOCK_DATA(block), LZ4_BLOCK_DATASIZE(block));
    }
  }
  return dest;
}
