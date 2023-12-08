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
 * @see _mapanon(), lz4check()
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
