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
#include "libc/ctype.h"
#include "libc/errno.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/sysv/errfuns.h"

/**
 * Reads utf-8 encoded multi-byte character from stream.
 *
 * - When the end of stream is encountered, WEOF is returned, the stream
 *   status is set so feof() will return true, and `errno` isn't changed
 *
 * - When an i/o error happens, `errno` is set, and the stream status is
 *   changed so that ferror() will be true, then WEOF is returned
 *
 * - When an invalid character is encountered, WEOF is returned, `errno`
 *   is set to `EILSEQ`, and the stream error state is *not* changed. An
 *   exception however is made when an incomplete multi-byte sequence is
 *   encountered at the end of stream, in which case feof() becomes true
 *   and the `errno` will be set to `EILSEQ` and WEOF is still returned.
 *
 * @return wide character, otherwise WEOF
 * @raise EAGAIN on `O_NONBLOCK` file descriptors when char isn't ready
 * @raise EILSEQ on malformed utf-8 sequence
 * @raise EINTR if signal handler was called
 * @cancelationpoint
 */
wint_t fgetwc_unlocked(FILE *f) {
  int c, n, i;
  wint_t b, x, y;
  unsigned char save[8];

  // read first byte
  if (f->beg < f->end) {
    b = f->buf[f->beg++] & 255;
  } else if ((c = fgetc_unlocked(f)) != EOF) {
    b = c;
  } else {
    return WEOF;
  }
  if (isascii(b))
    return b;
  if (b < 0300)
    return eilseq();
  save[(i = 0)] = b;

  // decode remaining bytes of multi-byte sequence
  n = ThomPikeLen(b);
  x = ThomPikeByte(b);
  while (--n) {
    if ((c = fgetc_unlocked(f)) == EOF) {
      switch (f->state) {
        case EOF:
          // incomplete sequence due to eof
          errno = EILSEQ;
          return WEOF;
        case EINTR:
        case EAGAIN:
          while (i)
            ungetc_unlocked(save[--i], f);
          return WEOF;
        default:
          return WEOF;
      }
    }
    y = save[i++] = c;
    if (ThomPikeCont(y)) {
      x = ThomPikeMerge(x, y);
    } else {
      // incomplete sequence due to new sequence starting
      ungetc_unlocked(c, f);
      errno = EILSEQ;
      return WEOF;
    }
  }
  if (x == WEOF)
    errno = EILSEQ;
  return x;
}

__strong_reference(fgetwc_unlocked, getwc_unlocked);
