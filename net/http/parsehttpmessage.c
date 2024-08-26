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
#include "libc/assert.h"
#include "libc/ctype.h"
#include "libc/limits.h"
#include "libc/mem/alg.h"
#include "libc/mem/arraylist.internal.h"
#include "libc/mem/mem.h"
#include "libc/serialize.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.h"
#include "libc/sysv/errfuns.h"
#include "libc/x/x.h"
#include "net/http/http.h"

/**
 * Initializes HTTP message parser.
 */
void InitHttpMessage(struct HttpMessage *r, int type) {
  unassert(type == kHttpRequest || type == kHttpResponse);
  bzero(r, sizeof(*r));
  r->type = type;
}

/**
 * Destroys HTTP message parser.
 */
void DestroyHttpMessage(struct HttpMessage *r) {
  if (r->xheaders.p) {
    free(r->xheaders.p);
    r->xheaders.p = NULL;
    r->xheaders.n = 0;
    r->xheaders.c = 0;
  }
}

/**
 * Resets http message parser state, so it can be re-used.
 *
 * This function amortizes the cost of malloc() in threads that process
 * multiple messages in a loop.
 *
 * @param r is assumed to have been passed to `InitHttpMessage` earlier
 */
void ResetHttpMessage(struct HttpMessage *r, int type) {
  unassert(type == kHttpRequest || type == kHttpResponse);
  unsigned c = r->xheaders.c;
  struct HttpHeader *p = r->xheaders.p;
  bzero(r, sizeof(*r));
  r->xheaders.c = c;
  r->xheaders.p = p;
  r->type = type;
}

/**
 * Parses HTTP request or response.
 *
 * This parser is responsible for determining the length of a message
 * and slicing the strings inside it. Performance is attained using
 * perfect hash tables. No memory allocation is performed for normal
 * messagesy. Line folding is forbidden. State persists across calls so
 * that fragmented messages can be handled efficiently. A limitation on
 * message size is imposed to make the header data structures smaller.
 *
 * This parser assumes ISO-8859-1 and guarantees no C0 or C1 control
 * codes are present in message fields, with the exception of tab.
 * Please note that fields like kHttpStateUri may use UTF-8 percent encoding.
 * This parser doesn't care if you choose ASA X3.4-1963 or MULTICS newlines.
 *
 * kHttpRepeatable defines which standard header fields are O(1) and
 * which ones may have comma entries spilled over into xheaders. For
 * most headers it's sufficient to simply check the static slice. If
 * r->headers[kHttpFoo].a is zero then the header is totally absent.
 *
 * This parser has linear complexity. Each character only needs to be
 * considered a single time. That's the case even if messages are
 * fragmented. If a message is valid but incomplete, this function will
 * return zero so that it can be resumed as soon as more data arrives.
 *
 * This parser takes about 400 nanoseconds to parse a 403 byte Chrome
 * HTTP request under MODE=rel on a Core i9 which is about three cycles
 * per byte or a gigabyte per second of throughput per core.
 *
 * @param p needs to have at least `c` bytes available
 * @param n is how many bytes have been received off the network so far
 * @param c is the capacity of `p` buffer; beyond `SHRT_MAX` is ignored
 * @return bytes on success, -1 on failure, 0 if more data must be read
 * @note messages can't exceed 2**15 bytes
 * @see HTTP/1.1 RFC2616 RFC2068
 * @see HTTP/1.0 RFC1945
 */
int ParseHttpMessage(struct HttpMessage *r, const char *p, size_t n, size_t c) {
  int h, i, ch;
  if (n > c)
    return einval();
  n = n > SHRT_MAX ? SHRT_MAX : n;
  c = c > SHRT_MAX ? SHRT_MAX : c;
  for (; r->i < n; ++r->i) {
    ch = p[r->i] & 255;
    switch (r->t) {
      case kHttpStateStart:
        if (ch == '\r' || ch == '\n')
          break;  // RFC7230 § 3.5
        if (!kHttpToken[ch])
          return ebadmsg();
        if (r->type == kHttpRequest) {
          r->t = kHttpStateMethod;
          r->method = kToUpper[ch];
          r->a = 8;
        } else {
          r->t = kHttpStateVersion;
          r->a = r->i;
        }
        break;
      case kHttpStateMethod:
        for (;;) {
          if (ch == ' ') {
            r->a = r->i + 1;
            r->t = kHttpStateUri;
            break;
          } else if (r->a == 64 || !kHttpToken[ch]) {
            return ebadmsg();
          }
          ch = kToUpper[ch];
          r->method |= (uint64_t)ch << r->a;
          r->a += 8;
          if (++r->i == n)
            break;
          ch = p[r->i] & 255;
        }
        break;
      case kHttpStateUri:
        for (;;) {
          if (ch == ' ' || ch == '\r' || ch == '\n') {
            if (r->i == r->a)
              return ebadmsg();
            r->uri.a = r->a;
            r->uri.b = r->i;
            if (ch == ' ') {
              r->a = r->i + 1;
              r->t = kHttpStateVersion;
            } else {
              r->version = 9;
              r->t = ch == '\r' ? kHttpStateCr : kHttpStateLf1;
            }
            break;
          } else if (ch < 0x20 || (0x7F <= ch && ch < 0xA0)) {
            return ebadmsg();
          }
          if (++r->i == n)
            break;
          ch = p[r->i] & 255;
        }
        break;
      case kHttpStateVersion:
        if (ch == ' ' || ch == '\r' || ch == '\n') {
          if (r->i - r->a == 8 &&
              (READ64BE(p + r->a) & 0xFFFFFFFFFF00FF00) == 0x485454502F002E00 &&
              isdigit(p[r->a + 5]) && isdigit(p[r->a + 7])) {
            r->version = (p[r->a + 5] - '0') * 10 + (p[r->a + 7] - '0');
            if (r->type == kHttpRequest) {
              r->t = ch == '\r' ? kHttpStateCr : kHttpStateLf1;
            } else {
              r->t = kHttpStateStatus;
            }
          } else {
            return ebadmsg();
          }
        }
        break;
      case kHttpStateStatus:
        for (;;) {
          if (ch == ' ' || ch == '\r' || ch == '\n') {
            if (r->status < 100)
              return ebadmsg();
            if (ch == ' ') {
              r->a = r->i + 1;
              r->t = kHttpStateMessage;
            } else {
              r->t = ch == '\r' ? kHttpStateCr : kHttpStateLf1;
            }
            break;
          } else if ('0' <= ch && ch <= '9') {
            r->status *= 10;
            r->status += ch - '0';
            if (r->status > 999)
              return ebadmsg();
          } else {
            return ebadmsg();
          }
          if (++r->i == n)
            break;
          ch = p[r->i] & 255;
        }
        break;
      case kHttpStateMessage:
        for (;;) {
          if (ch == '\r' || ch == '\n') {
            r->message.a = r->a;
            r->message.b = r->i;
            r->t = ch == '\r' ? kHttpStateCr : kHttpStateLf1;
            break;
          } else if (ch < 0x20 || (0x7F <= ch && ch < 0xA0)) {
            return ebadmsg();
          }
          if (++r->i == n)
            break;
          ch = p[r->i] & 255;
        }
        break;
      case kHttpStateCr:
        if (ch != '\n')
          return ebadmsg();
        r->t = kHttpStateLf1;
        break;
      case kHttpStateLf1:
        if (ch == '\r') {
          r->t = kHttpStateLf2;
          break;
        } else if (ch == '\n') {
          return ++r->i;
        } else if (!kHttpToken[ch]) {
          // 1. Forbid empty header name (RFC2616 §2.2)
          // 2. Forbid line folding (RFC7230 §3.2.4)
          return ebadmsg();
        }
        r->k.a = r->i;
        r->t = kHttpStateName;
        break;
      case kHttpStateName:
        for (;;) {
          if (ch == ':') {
            r->k.b = r->i;
            r->t = kHttpStateColon;
            break;
          } else if (!kHttpToken[ch]) {
            return ebadmsg();
          }
          if (++r->i == n)
            break;
          ch = p[r->i] & 255;
        }
        break;
      case kHttpStateColon:
        if (ch == ' ' || ch == '\t')
          break;
        r->a = r->i;
        r->t = kHttpStateValue;
        // fallthrough
      case kHttpStateValue:
        for (;;) {
          if (ch == '\r' || ch == '\n') {
            i = r->i;
            while (i > r->a && (p[i - 1] == ' ' || p[i - 1] == '\t'))
              --i;
            if ((h = GetHttpHeader(p + r->k.a, r->k.b - r->k.a)) != -1 &&
                (!r->headers[h].a || !kHttpRepeatable[h])) {
              r->headers[h].a = r->a;
              r->headers[h].b = i;
            } else {
              if (r->xheaders.n == r->xheaders.c) {
                unsigned c2;
                struct HttpHeader *p1, *p2;
                p1 = r->xheaders.p;
                c2 = r->xheaders.c;
                if (c2 == 0) {
                  c2 = 1;
                } else {
                  c2 = c2 * 2;
                }
                if ((p2 = realloc(p1, c2 * sizeof(*p1)))) {
                  r->xheaders.p = p2;
                  r->xheaders.c = c2;
                }
              }
              if (r->xheaders.n < r->xheaders.c) {
                r->xheaders.p[r->xheaders.n].k = r->k;
                r->xheaders.p[r->xheaders.n].v.a = r->a;
                r->xheaders.p[r->xheaders.n].v.b = i;
                r->xheaders.p = r->xheaders.p;
                ++r->xheaders.n;
              }
            }
            r->t = ch == '\r' ? kHttpStateCr : kHttpStateLf1;
            break;
          } else if ((ch < 0x20 && ch != '\t') || (0x7F <= ch && ch < 0xA0)) {
            return ebadmsg();
          }
          if (++r->i == n)
            break;
          ch = p[r->i] & 255;
        }
        break;
      case kHttpStateLf2:
        if (ch == '\n') {
          return ++r->i;
        }
        return ebadmsg();
      default:
        __builtin_unreachable();
    }
  }
  if (r->i < c)
    return 0;
  return ebadmsg();
}
