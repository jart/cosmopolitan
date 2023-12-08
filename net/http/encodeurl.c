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
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "net/http/escape.h"
#include "net/http/url.h"

static size_t DimensionUrl(struct Url *h) {
  size_t i, n;
  n = 0;
  n += h->scheme.n;
  n += 1;
  n += 2;
  n += h->user.n * 3;
  n += 1;
  n += h->pass.n * 3;
  n += 1;
  n += 1;
  n += h->host.n * 3;
  n += 1;
  n += 1;
  n += h->port.n * 3;
  n += 1;
  n += h->path.n * 3;
  n += 1;
  n += h->params.n;
  for (i = 0; i < h->params.n; ++i) {
    n += h->params.p[i].key.n * 3;
    n += 1;
    n += h->params.p[i].val.n * 3;
  }
  n += 1;
  n += h->fragment.n * 3;
  n += 1;
  return n;
}

static bool NeedsSquareBrackets(struct Url *h) {
  int c;
  size_t i;
  if (!memchr(h->host.p, ':', h->host.n)) return false;
  if (h->pass.p) return true;
  if (h->host.n >= 4 && h->host.p[0] == 'v' && h->host.p[2] == '.' &&
      kHexToInt[h->host.p[1] & 0xFF] != -1) {
    for (i = 3; i < h->host.n; ++i) {
      if (kEscapeIp[h->host.p[i] & 0xFF]) {
        return false;
      }
    }
  } else {
    for (i = 0; i < h->host.n; ++i) {
      c = h->host.p[i] & 0xFF;
      if (!(kHexToInt[c] || c == '.' || c == ':')) {
        return false;
      }
    }
  }
  return true;
}

/**
 * Encodes URL.
 *
 * @param z if not null receives string length of result
 * @return nul-terminated url string needing free
 * @see ParseUrl()
 */
char *EncodeUrl(struct Url *h, size_t *z) {
  size_t i, n;
  char *m, *p;
  if ((p = m = malloc(DimensionUrl(h)))) {
    if (h->scheme.n) {
      p = mempcpy(p, h->scheme.p, h->scheme.n);
      *p++ = ':';
    }
    if (h->host.p) {
      *p++ = '/';
      *p++ = '/';
      if (h->user.p) {
        p = EscapeUrlView(p, &h->user, kEscapeAuthority);
        if (h->pass.p) {
          *p++ = ':';
          p = EscapeUrlView(p, &h->pass, kEscapeAuthority);
        }
        *p++ = '@';
      }
      if (h->host.p) {
        if (NeedsSquareBrackets(h)) {
          *p++ = '[';
          p = EscapeUrlView(p, &h->host, kEscapeIp);
          *p++ = ']';
        } else {
          p = EscapeUrlView(p, &h->host, kEscapeAuthority);
        }
        if (h->port.p) {
          *p++ = ':';
          p = EscapeUrlView(p, &h->port, kEscapeAuthority);
        }
      }
      if (h->path.n && h->path.p[0] != '/') {
        *p++ = '/';
      }
    }
    p = EscapeUrlView(p, &h->path, kEscapePath);
    if (h->params.p) {
      *p++ = '?';
      for (i = 0; i < h->params.n; ++i) {
        if (i) *p++ = '&';
        p = EscapeUrlView(p, &h->params.p[i].key, kEscapeParam);
        if (h->params.p[i].val.p) {
          *p++ = '=';
          p = EscapeUrlView(p, &h->params.p[i].val, kEscapeParam);
        }
      }
    }
    if (h->fragment.p) {
      *p++ = '#';
      p = EscapeUrlView(p, &h->fragment, kEscapeFragment);
    }
    n = p - m;
    *p++ = '\0';
    if ((p = realloc(m, p - m))) m = p;
  } else {
    n = 0;
  }
  if (z) *z = n;
  return m;
}
