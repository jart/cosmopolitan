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
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "libc/x/x.h"
#include "net/http/http.h"

enum ParseHttpRequestState {
  METHOD,
  URI,
  VERSION,
  HKEY,
  HSEP,
  HVAL,
  CR1,
  LF1,
  LF2
};

/**
 * Parses HTTP request header.
 */
int ParseHttpRequest(struct HttpRequest *req, const char *p, size_t n) {
  int a, h, c, i, x;
  enum ParseHttpRequestState t;
  memset(req, 0, sizeof(*req));
  a = h = 0;
  t = METHOD;
  if (n > SHRT_MAX - 1) n = SHRT_MAX - 1;
  for (i = 0; i < n; ++i) {
    c = p[i] & 0xFF;
    switch (t) {
      case METHOD:
        if (c == '\r' || c == '\n') break; /* RFC7230 § 3.5 */
        if (c == ' ') {
          if (!i) return ebadmsg();
          if ((x = GetHttpMethod(p, i)) == -1) return ebadmsg();
          req->method = x;
          req->uri.a = i + 1;
          t = URI;
        }
        break;
      case URI:
        if (c == ' ') {
          req->uri.b = i;
          req->version.a = i + 1;
          if (req->uri.a == req->uri.b) return ebadmsg();
          t = VERSION;
        }
        break;
      case VERSION:
        if (c == '\r' || c == '\n') {
          req->version.b = i;
          t = c == '\r' ? CR1 : LF1;
        }
        break;
      case CR1:
        if (c != '\n') return ebadmsg();
        t = LF1;
        break;
      case LF1:
        if (c == '\r') {
          t = LF2;
          break;
        } else if (c == '\n') {
          return 0;
        } else if (c == ' ' || c == '\t') { /* line folding!!! */
          return eprotonosupport();         /* RFC7230 § 3.2.4 */
        }
        a = i;
        t = HKEY;
        /* εpsilon transition */
      case HKEY:
        if (c == ':') {
          h = GetHttpHeader(p + a, i - a);
          t = HSEP;
        }
        break;
      case HSEP:
        if (c == ' ' || c == '\t') break;
        a = i;
        t = HVAL;
        /* εpsilon transition */
      case HVAL:
        if (c == '\r' || c == '\n') {
          if (h != -1) {
            req->headers[h].a = a;
            req->headers[h].b = i;
          }
          t = c == '\r' ? CR1 : LF1;
        }
        break;
      case LF2:
        if (c == '\n') {
          req->length = i + 1;
          return i + 1;
        }
        return ebadmsg();
      default:
        unreachable;
    }
  }
  return 0;
}
