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
#include "libc/alg/alg.h"
#include "libc/alg/arraylist.h"
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
 * Parses req line and headers of HTTP req.
 *
 * This parser is very lax. All decoding is ISO-8859-1. A 1mB upper
 * bound on memory is enforced.
 */
int parsehttprequest(struct HttpRequest *req, FILE *f) {
  enum ParseHttpRequestState state = METHOD;
  int toto = 0;
  clearhttprequest(req);
  while (true) {
    char ch;
    {
      int c = fgetc(f);
      if (c == -1) {
        if (toto && !ferror(f)) {
          return ebadmsg(); /* RFC7230 § 3.4 */
        }
        return -1;
      }
      ch = (unsigned char)c;
    }
    if (++toto == UINT16_MAX) {
      return ebadmsg(); /* RFC7230 § 3.2.5 */
    }
    switch (state) {
      case METHOD:
        if (ch == '\r' || ch == '\n') break; /* RFC7230 § 3.5 */
        if (ch == ' ') {
          if (req->method.i == 0) return ebadmsg();
          state = URI;
        } else {
          append(&req->method, &ch);
        }
        break;
      case URI:
        if (ch == ' ') {
          if (req->uri.i == 0) return ebadmsg();
          state = VERSION;
        } else {
          append(&req->uri, &ch);
        }
        break;
      case VERSION:
        if (ch == '\r' || ch == '\n') {
          state = ch == '\r' ? CR1 : LF1;
        } else {
          append(&req->version, &ch);
        }
        break;
      case CR1:
        if (ch != '\n') return ebadmsg();
        state = LF1;
        break;
      case LF1:
        if (ch == '\r') {
          state = LF2;
          break;
        } else if (ch == '\n') {
          return 0;
        } else if (ch == ' ' || ch == '\t') { /* line folding!!! */
          return eprotonosupport();           /* RFC7230 § 3.2.4 */
        }
        state = HKEY;
        /* εpsilon transition */
      case HKEY:
        if (ch == ':') state = HSEP;
        ch = tolower(ch);
        append(&req->scratch, &ch);
        break;
      case HSEP:
        if (ch == ' ' || ch == '\t') break;
        state = HVAL;
        /* εpsilon transition */
      case HVAL:
        if (ch == '\r' || ch == '\n') {
          state = ch == '\r' ? CR1 : LF1;
          ch = '\0';
        }
        append(&req->scratch, &ch);
        if (!ch) {
          critbit0_insert(&req->headers, req->scratch.p);
          req->scratch.i = 0;
        }
        break;
      case LF2:
        if (ch == '\n') return 0;
        return ebadmsg();
      default:
        unreachable;
    }
  }
}
