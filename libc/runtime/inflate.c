/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "third_party/puff/puff.h"
#include "third_party/zlib/zlib.h"

/**
 * Decompresses raw deflate data.
 *
 * This uses puff by default since it has a 2kb footprint. If zlib
 * proper is linked, then we favor that instead, since it's faster.
 *
 * @param outsize needs to be known ahead of time by some other means
 * @return 0 on success or nonzero on failure
 */
int __inflate(void *out, size_t outsize, const void *in, size_t insize) {
  int rc;
  z_stream zs;
  if (_weaken(inflateInit2) &&  //
      _weaken(inflate) &&       //
      _weaken(inflateEnd) &&    //
      __runlevel >= RUNLEVEL_MALLOC) {
    zs.next_in = in;
    zs.avail_in = insize;
    zs.total_in = insize;
    zs.next_out = out;
    zs.avail_out = outsize;
    zs.total_out = outsize;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    if ((rc = _weaken(inflateInit2)(&zs, -MAX_WBITS)) == Z_OK &&
        (rc = _weaken(inflate)(&zs, Z_FINISH)) == Z_STREAM_END &&
        (rc = _weaken(inflateEnd)(&zs)) == Z_OK) {
      rc = 0;
    } else if (rc == Z_OK) {
      rc = Z_STREAM_END;  // coerce to nonzero
    }
  } else {
    rc = _puff(out, &outsize, in, &insize);
  }
  STRACE("inflate([%#.*hhs%s], %'zu, %#.*hhs%s, %'zu) → %d",
         (int)MIN(40, outsize), out, outsize > 40 ? "..." : "", outsize,
         (int)MIN(40, insize), in, insize > 40 ? "..." : "", insize, rc);
  return rc;
}
