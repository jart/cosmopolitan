/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/sysv/errfuns.h"
#include "net/http/escape.h"
#include "net/http/http.h"

/**
 * Removes chunk transfer encoding from message body in place.
 *
 * @param p is modified in place
 * @param l receives content length if not null
 * @return bytes processed, 0 if need more data, or -1 on failure
 */
ssize_t Unchunk(struct HttpUnchunker *u, char *p, size_t n, size_t *l) {
  int c, h;
  size_t s;
  while (u->i < n) {
    c = p[u->i++] & 255;
    switch (u->t) {
      case kHttpStateChunkStart:
        if ((u->m = kHexToInt[c]) == -1) return ebadmsg();
        u->t = kHttpStateChunkSize;
        break;
      case kHttpStateChunkSize:
        if ((h = kHexToInt[c]) != -1) {
          u->m *= 16;
          u->m += h;
          if (u->m >= 0x0000010000000000) return ebadmsg();
          break;
        }
        u->t = kHttpStateChunkExt;
        /* fallthrough */
      case kHttpStateChunkExt:
        if (c == '\r') {
          u->t = kHttpStateChunkLf1;
          break;
        } else if (c != '\n') {
          break;
        }
        /* fallthrough */
      case kHttpStateChunkLf1:
        if (c != '\n') return ebadmsg();
        u->t = u->m ? kHttpStateChunk : kHttpStateTrailerStart;
        break;
      case kHttpStateChunk:
        p[u->j++] = c, --u->m;
        s = MIN(u->m, n - u->i);
        memmove(p + u->j, p + u->i, s);
        u->i += s;
        u->j += s;
        u->m -= s;
        if (!u->m) u->t = kHttpStateChunkCr2;
        break;
      case kHttpStateChunkCr2:
        if (c == '\r') {
          u->t = kHttpStateChunkLf2;
          break;
        }
        /* fallthrough */
      case kHttpStateChunkLf2:
        if (c != '\n') return ebadmsg();
        u->t = kHttpStateChunkStart;
        break;
      case kHttpStateTrailerStart:
        if (c == '\r') {
          u->t = kHttpStateTrailerLf2;
          break;
        } else if (c == '\n') {
          goto Finished;
        }
        u->t = kHttpStateTrailer;
        /* fallthrough */
      case kHttpStateTrailer:
        if (c == '\r') {
          u->t = kHttpStateTrailerLf1;
          break;
        } else if (c != '\n') {
          break;
        }
        /* fallthrough */
      case kHttpStateTrailerLf1:
        if (c != '\n') return ebadmsg();
        u->t = kHttpStateTrailerStart;
        break;
      case kHttpStateTrailerLf2:
        if (c != '\n') return ebadmsg();
      Finished:
        if (l) *l = u->j;
        if (u->j < n) p[u->j] = 0;
        return u->i;
        break;
      default:
        __builtin_unreachable();
    }
  }
  return 0;
}
