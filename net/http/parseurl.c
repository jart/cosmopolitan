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
#include "libc/bits/likely.h"
#include "libc/limits.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "net/http/url.h"

struct UrlParser {
  int i;
  int c;
  const char *data;
  int size;
  bool isform;
  bool islatin1;
  char *p;
  char *q;
};

static const signed char kHexToInt[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x00
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x10
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x20
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  -1, -1, -1, -1, -1, -1,  // 0x30
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x40
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x50
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x60
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x70
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x80
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x90
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xa0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xb0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xc0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xd0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xe0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xf0
};

static void EmitLatin1(struct UrlParser *u, int c) {
  u->p[0] = 0300 | c >> 6;
  u->p[1] = 0200 | c & 077;
  u->p += 2;
}

static void EmitKey(struct UrlParser *u, struct UrlParams *h) {
  h->p = xrealloc(h->p, ++h->n * sizeof(*h->p));
  h->p[h->n - 1].key.p = u->q;
  h->p[h->n - 1].key.n = u->p - u->q;
  u->q = u->p;
}

static void EmitVal(struct UrlParser *u, struct UrlParams *h, bool t) {
  if (!t) {
    if (u->p > u->q) {
      EmitKey(u, h);
      h->p[h->n - 1].val.p = NULL;
      h->p[h->n - 1].val.n = SIZE_MAX;
    }
  } else {
    h->p[h->n - 1].val.p = u->q;
    h->p[h->n - 1].val.n = u->p - u->q;
    u->q = u->p;
  }
}

static void ParseEscape(struct UrlParser *u) {
  int a, b;
  if (u->i + 2 <= u->size &&
      ((a = kHexToInt[u->data[u->i + 0] & 0xff]) != -1 &&
       (b = kHexToInt[u->data[u->i + 1] & 0xff]) != -1)) {
    u->c = a << 4 | b;
    u->i += 2;
  }
  *u->p++ = u->c;
}

static bool ParseScheme(struct UrlParser *u, struct Url *h) {
  while (u->i < u->size) {
    u->c = u->data[u->i++] & 0xff;
    if (u->c == '/') {
      if (u->i == 1 && u->i < u->size && u->data[u->i] == '/') {
        ++u->i;
        return true;
      } else {
        *u->p++ = u->c;
        return false;
      }
    } else if (u->c == ':') {
      h->scheme.p = u->q;
      h->scheme.n = u->p - u->q;
      u->q = u->p;
      if (u->i + 2 <= u->size &&
          (u->data[u->i + 1] == '/' && u->data[u->i + 1] == '/')) {
        u->i += 2;
        return true;
      } else {
        return false;
      }
    } else if (u->c == '#' || u->c == '?') {
      h->path.p = u->q;
      h->path.n = u->p - u->q;
      u->q = u->p;
      return false;
    } else if (u->c == '%') {
      ParseEscape(u);
    } else if (u->c >= 0200 && u->islatin1) {
      EmitLatin1(u, u->c);
    } else {
      *u->p++ = u->c;
    }
  }
  return false;
}

static void ParseAuthority(struct UrlParser *u, struct Url *h) {
  bool b = false;
  const char *c = NULL;
  while (u->i < u->size) {
    u->c = u->data[u->i++] & 0xff;
    if (u->c == '/' || u->c == '#' || u->c == '?') {
      break;
    } else if (u->c == '[') {
      b = true;
    } else if (u->c == ']') {
      b = false;
    } else if (u->c == ':' && !b) {
      c = u->p;
    } else if (u->c == '@') {
      if (c) {
        h->user.p = u->q;
        h->user.n = c - u->q;
        h->pass.p = c;
        h->pass.n = u->p - c;
        c = NULL;
      } else {
        h->user.p = u->q;
        h->user.n = u->p - u->q;
      }
      u->q = u->p;
    } else if (u->c == '%') {
      ParseEscape(u);
    } else if (u->c >= 0200 && u->islatin1) {
      EmitLatin1(u, u->c);
    } else {
      *u->p++ = u->c;
    }
  }
  if (c) {
    h->host.p = u->q;
    h->host.n = c - u->q;
    h->port.p = c;
    h->port.n = u->p - c;
    c = NULL;
  } else {
    h->host.p = u->q;
    h->host.n = u->p - u->q;
  }
  u->q = u->p;
  if (u->c == '/') {
    *u->p++ = u->c;
  }
}

static void ParsePath(struct UrlParser *u, struct UrlView *h) {
  while (u->i < u->size) {
    u->c = u->data[u->i++] & 0xff;
    if (u->c == '#' || u->c == '?') {
      break;
    } else if (u->c == '%') {
      ParseEscape(u);
    } else if (u->c >= 0200 && u->islatin1) {
      EmitLatin1(u, u->c);
    } else {
      *u->p++ = u->c;
    }
  }
  h->p = u->q;
  h->n = u->p - u->q;
  u->q = u->p;
}

static void ParseKeyValues(struct UrlParser *u, struct UrlParams *h) {
  bool t = false;
  while (u->i < u->size) {
    u->c = u->data[u->i++] & 0xff;
    if (u->c == '#') {
      break;
    } else if (u->c == '%') {
      ParseEscape(u);
    } else if (u->c == '+') {
      *u->p++ = u->isform ? ' ' : '+';
    } else if (u->c == '&') {
      EmitVal(u, h, t);
      t = false;
    } else if (u->c == '=') {
      if (!t) {
        if (u->p > u->q) {
          EmitKey(u, h);
          t = true;
        }
      } else {
        *u->p++ = '=';
      }
    } else if (u->c >= 0200 && u->islatin1) {
      EmitLatin1(u, u->c);
    } else {
      *u->p++ = u->c;
    }
  }
  EmitVal(u, h, t);
}

static void ParseFragment(struct UrlParser *u, struct UrlView *h) {
  while (u->i < u->size) {
    u->c = u->data[u->i++] & 0xff;
    if (u->c == '%') {
      ParseEscape(u);
    } else if (u->c >= 0200 && u->islatin1) {
      EmitLatin1(u, u->c);
    } else {
      *u->p++ = u->c;
    }
  }
  h->p = u->q;
  h->n = u->p - u->q;
  u->q = u->p;
}

static char *ParseUrlImpl(const char *data, size_t size, struct Url *h,
                          bool latin1) {
  char *m;
  struct UrlParser u;
  if (size == -1) size = data ? strlen(data) : 0;
  u.i = 0;
  u.c = 0;
  u.isform = false;
  u.islatin1 = latin1;
  u.data = data;
  u.size = size;
  memset(h, 0, sizeof(*h));
  u.q = u.p = m = xmalloc(u.size * 2);
  if (ParseScheme(&u, h)) ParseAuthority(&u, h);
  if (u.c != '#' && u.c != '?') ParsePath(&u, &h->path);
  if (u.c == '?') ParseKeyValues(&u, &h->params);
  if (u.c == '#') ParseFragment(&u, &h->fragment);
  return xrealloc(m, u.p - m);
}

/**
 * Parses URL.
 *
 * There's no failure condition for this routine. This is a permissive
 * parser that doesn't impose character restrictions beyond what is
 * necessary for parsing. This doesn't normalize path segments like `.`
 * or `..`. Use IsAcceptablePath() to check for those.
 *
 * This parser is charset agnostic. Returned values might contain things
 * like NUL characters, control codes, and non-canonical encodings.
 *
 * This parser doesn't support the ability to accurately parse path
 * segments which contain percent-encoded slash. There's also no support
 * for semicolon parameters at the moment.
 *
 * @param data is value like `/hi?x=y&z` or `http://a.example/hi#x`
 * @param size is byte length and -1 implies strlen
 * @param h is assumed to be uninitialized
 * @return memory backing UrlView needing free (and h.params.p too)
 */
char *ParseUrl(const char *data, size_t size, struct Url *h) {
  return ParseUrlImpl(data, size, h, false);
}

/**
 * Parses HTTP Request-URI.
 *
 * The input is ISO-8859-1 which is transcoded to UTF-8. Therefore we
 * assume percent-encoded bytes are expressed as UTF-8. Returned values
 * might contain things like NUL characters, C0, and C1 control codes.
 * UTF-8 isn't checked for validity and may contain overlong values.
 *
 * There's no failure condition for this routine. This is a permissive
 * parser that doesn't impose character restrictions beyond what is
 * necessary for parsing. This doesn't normalize path segments like `.`
 * or `..`. Use IsAcceptablePath() to check for those.
 *
 * This parser doesn't support the ability to accurately parse path
 * segments which contain percent-encoded slash.
 *
 * @param data is value like `/hi?x=y&z` or `http://a.example/hi#x`
 * @param size is byte length and -1 implies strlen
 * @param h is assumed to be uninitialized
 * @return memory backing UrlView needing free (and h.params.p too)
 */
char *ParseRequestUri(const char *data, size_t size, struct Url *h) {
  return ParseUrlImpl(data, size, h, true);
}

/**
 * Parses HTTP POST key-value params.
 *
 * These are similar to the parameters found in a Request-URI. The main
 * difference is that `+` is translated into space here. The mime type
 * for this is application/x-www-form-urlencoded.
 *
 * This parser is charset agnostic. Returned values might contain things
 * like NUL characters, control codes, and non-canonical encodings.
 *
 * There's no failure condition for this routine. This is a permissive
 * parser that doesn't impose character restrictions beyond what is
 * necessary for parsing.
 *
 * @param data is value like `foo=bar&x=y&z`
 * @param size is byte length and -1 implies strlen
 * @param h must be zeroed by caller and this appends if reused
 * @return UrlView memory with same size needing free (h.p needs free too)
 */
char *ParseParams(const char *data, size_t size, struct UrlParams *h) {
  char *m;
  struct UrlParser u;
  if (size == -1) size = data ? strlen(data) : 0;
  u.i = 0;
  u.c = 0;
  u.isform = true;
  u.islatin1 = false;
  u.data = data;
  u.size = size;
  u.q = u.p = m = xmalloc(u.size);
  ParseKeyValues(&u, h);
  return m;
}
