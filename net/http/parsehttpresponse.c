/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/bits.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "net/http/http.h"

#define LIMIT (SHRT_MAX - 2)

enum { START, VERSION, STATUS, MESSAGE, HKEY, HSEP, HVAL, CR1, LF1, LF2 };

/**
 * Initializes HTTP response parser.
 */
void InitHttpResponse(struct HttpMessage *r) {
  memset(r, 0, sizeof(*r));
}

/**
 * Destroys HTTP response parser.
 */
void DestroyHttpResponse(struct HttpMessage *r) {
  if (r->xheaders.p) {
    free(r->xheaders.p);
    r->xheaders.p = NULL;
    r->xheaders.n = 0;
  }
}

/**
 * Parses HTTP response.
 */
int ParseHttpResponse(struct HttpMessage *r, const char *p, size_t n) {
  int c, h, i;
  struct HttpHeader *x;
  for (n = MIN(n, LIMIT); r->i < n; ++r->i) {
    c = p[r->i] & 0xff;
    switch (r->t) {
      case START:
        if (c == '\r' || c == '\n') break; /* RFC7230 § 3.5 */
        if (c != 'H') return ebadmsg();
        r->t = VERSION;
        r->a = r->i;
        break;
      case VERSION:
        if (c == ' ') {
          if (r->i - r->a == 8 &&
              (READ64BE(p + r->a) & 0xFFFFFFFFFF00FF00) == 0x485454502F002E00 &&
              isdigit(p[r->a + 5]) && isdigit(p[r->a + 7])) {
            r->version = (p[r->a + 5] - '0') * 10 + (p[r->a + 7] - '0');
            r->t = STATUS;
          } else {
            return ebadmsg();
          }
        }
        break;
      case STATUS:
        for (;;) {
          if (c == ' ' || c == '\r' || c == '\n') {
            if (r->status < 100) return ebadmsg();
            if (c == ' ') {
              r->a = r->i + 1;
              r->t = MESSAGE;
            } else {
              r->t = c == '\r' ? CR1 : LF1;
            }
            break;
          } else if ('0' <= c && c <= '9') {
            r->status *= 10;
            r->status += c - '0';
            if (r->status > 999) return ebadmsg();
          } else {
            return ebadmsg();
          }
          if (++r->i == n) break;
          c = p[r->i] & 0xff;
        }
        break;
      case MESSAGE:
        for (;;) {
          if (c == '\r' || c == '\n') {
            r->message.a = r->a;
            r->message.b = r->i;
            r->t = c == '\r' ? CR1 : LF1;
            break;
          } else if (c < 0x20 || (0x7F <= c && c < 0xA0)) {
            return ebadmsg();
          }
          if (++r->i == n) break;
          c = p[r->i] & 0xff;
        }
        break;
      case CR1:
        if (c != '\n') return ebadmsg();
        r->t = LF1;
        break;
      case LF1:
        if (c == '\r') {
          r->t = LF2;
          break;
        } else if (c == '\n') {
          return ++r->i;
        } else if (!kHttpToken[c]) {
          return ebadmsg(); /* RFC7230 § 3.2.4 */
        }
        r->k.a = r->i;
        r->t = HKEY;
        break;
      case HKEY:
        for (;;) {
          if (c == ':') {
            r->k.b = r->i;
            r->t = HSEP;
            break;
          } else if (!kHttpToken[c]) {
            return ebadmsg();
          }
          if (++r->i == n) break;
          c = p[r->i] & 0xff;
        }
        break;
      case HSEP:
        if (c == ' ' || c == '\t') break;
        r->a = r->i;
        r->t = HVAL;
        /* fallthrough */
      case HVAL:
        for (;;) {
          if (c == '\r' || c == '\n') {
            i = r->i;
            while (i > r->a && (p[i - 1] == ' ' || p[i - 1] == '\t')) --i;
            if ((h = GetHttpHeader(p + r->k.a, r->k.b - r->k.a)) != -1 &&
                (!r->headers[h].a || !kHttpRepeatable[h])) {
              r->headers[h].a = r->a;
              r->headers[h].b = i;
            } else if ((x = realloc(
                            r->xheaders.p,
                            (r->xheaders.n + 1) * sizeof(*r->xheaders.p)))) {
              x[r->xheaders.n].k = r->k;
              x[r->xheaders.n].v.a = r->a;
              x[r->xheaders.n].v.b = i;
              r->xheaders.p = x;
              ++r->xheaders.n;
            }
            r->t = c == '\r' ? CR1 : LF1;
            break;
          } else if ((c < 0x20 && c != '\t') || (0x7F <= c && c < 0xA0)) {
            return ebadmsg();
          }
          if (++r->i == n) break;
          c = p[r->i] & 0xff;
        }
        break;
      case LF2:
        if (c == '\n') {
          return ++r->i;
        }
        return ebadmsg();
      default:
        unreachable;
    }
  }
  if (r->i < LIMIT) {
    return 0;
  } else {
    return ebadmsg();
  }
}
