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
#include "libc/zip.h"

/* TODO(jart): DELETE */

/**
 * Locates End Of Central Directory record in ZIP file.
 *
 * We search backwards for the End Of Central Directory Record magnum.
 * The ZIP spec says this can be anywhere in the last 64kb. We go all
 * the way since .com.dbg binaries will have lots of DWARF stuff after
 * the embedded .com binary. As such, we sanity check the other fields
 * too to make sure the record seems legit and it's not just a random
 * occurrence of the magnum.
 *
 * @param p points to file memory
 * @param n is byte size of file
 */
uint8_t *zipfindcentraldir(const uint8_t *p, size_t n) {
  size_t i;
  if (n >= kZipCdirHdrMinSize) {
    i = n - kZipCdirHdrMinSize;
    do {
      if (ZIP_CDIR_MAGIC(p + i) == kZipCdirHdrMagic && IsZipCdir32(p, n, i)) {
        return (/*unconst*/ uint8_t *)(p + i);
      }
    } while (i--);
  }
  return NULL;
}
