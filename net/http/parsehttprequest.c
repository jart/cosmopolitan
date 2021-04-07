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
#include "libc/alg/alg.h"
#include "libc/alg/arraylist.internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "libc/x/x.h"
#include "net/http/http.h"

#define LIMIT (SHRT_MAX - 2)

enum { START, METHOD, URI, VERSION, HKEY, HSEP, HVAL, CR1, LF1, LF2 };

/**
 * Initializes HTTP request parser.
 */
void InitHttpRequest(struct HttpRequest *r) {
  memset(r, 0, sizeof(*r));
}

/**
 * Destroys HTTP request parser.
 */
void DestroyHttpRequest(struct HttpRequest *r) {
  if (r->xheaders.p) {
    free(r->xheaders.p);
    r->xheaders.p = NULL;
    r->xheaders.n = 0;
  }
}

/**
 * Parses HTTP request.
 *
 * This parser is responsible for determining the length of a message
 * and slicing the strings inside it. Performance is attained using
 * perfect hash tables. No memory allocation is performed for normal
 * messages. Line folding is forbidden. State persists across calls so
 * that fragmented messages can be handled efficiently. A limitation on
 * message size is imposed to make the header data structures smaller.
 * All other things are permissive to the greatest extent possible.
 * Further functions are provided for the interpretation, validation,
 * and sanitization of various fields.
 *
 * @note we assume p points to a buffer that has >=SHRT_MAX bytes
 */
int ParseHttpRequest(struct HttpRequest *r, const char *p, size_t n) {
  int c, h, i;
  struct HttpRequestHeader *x;
  for (n = MIN(n, LIMIT); r->i < n; ++r->i) {
    c = p[r->i] & 0xff;
    switch (r->t) {
      case START:
        if (c == '\r' || c == '\n') {
          ++r->a; /* RFC7230 § 3.5 */
          break;
        }
        r->t = METHOD;
        /* fallthrough */
      case METHOD:
        for (;;) {
          if (c == ' ') {
            if ((r->method = GetHttpMethod(p + r->a, r->i - r->a)) != -1) {
              r->uri.a = r->i + 1;
              r->t = URI;
            } else {
              return ebadmsg();
            }
            break;
          } else if (!('A' <= c && c <= 'Z')) {
            return ebadmsg();
          }
          if (++r->i == n) break;
          c = p[r->i] & 0xff;
        }
        break;
      case URI:
        for (;;) {
          if (c == ' ' || c == '\r' || c == '\n') {
            if (r->i == r->uri.a) return ebadmsg();
            r->uri.b = r->i;
            if (c == ' ') {
              r->version.a = r->i + 1;
              r->t = VERSION;
            } else if (c == '\r') {
              r->t = CR1;
            } else {
              r->t = LF1;
            }
            break;
          }
          if (++r->i == n) break;
          c = p[r->i] & 0xff;
        }
        break;
      case VERSION:
        if (c == '\r' || c == '\n') {
          r->version.b = r->i;
          r->t = c == '\r' ? CR1 : LF1;
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
        } else if (c == ':') {
          return ebadmsg();
        } else if (c == ' ' || c == '\t') {
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
            if ((h = GetHttpHeader(p + r->k.a, r->k.b - r->k.a)) != -1) {
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
