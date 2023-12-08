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
#include "libc/intrin/likely.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/x/x.h"
#include "net/http/escape.h"
#include "net/http/url.h"

struct UrlParser {
  char *p, *q;
  const char *s;
  unsigned c, i, n, f;
};

static void EmitLatin1(char **p, int c) {
  (*p)[0] = 0300 | c >> 6;
  (*p)[1] = 0200 | (c & 077);
  *p += 2;
}

static bool EmitKey(struct UrlParser *u, struct UrlParams *h) {
  struct UrlParam *p;
  if ((p = realloc(h->p, ++h->n * sizeof(*h->p)))) {
    p[h->n - 1].key.p = u->q;
    p[h->n - 1].key.n = u->p - u->q;
    u->q = u->p;
    h->p = p;
    return true;
  } else {
    return false;
  }
}

static void EmitVal(struct UrlParser *u, struct UrlParams *h, bool t) {
  if (!t) {
    if (u->p > u->q || u->c != '?') {
      if (EmitKey(u, h)) {
        h->p[h->n - 1].val.p = NULL;
        h->p[h->n - 1].val.n = 0;
      }
    }
  } else {
    h->p[h->n - 1].val.p = u->q;
    h->p[h->n - 1].val.n = u->p - u->q;
    u->q = u->p;
  }
}

static void ParseEscape(struct UrlParser *u) {
  int a, b, c = '%';
  if (u->i + 2 <= u->n && ((a = kHexToInt[u->s[u->i + 0] & 255]) != -1 &&
                           (b = kHexToInt[u->s[u->i + 1] & 255]) != -1)) {
    c = a << 4 | b;
    u->i += 2;
  }
  *u->p++ = c;
}

static bool ParseScheme(struct UrlParser *u, struct Url *h) {
  while (u->i < u->n) {
    u->c = u->s[u->i++] & 255;
    if (u->c == '/') {
      if (u->i == 1 && u->i < u->n && u->s[u->i] == '/') {
        ++u->i;
        return true;
      } else {
        *u->p++ = '/';
        return false;
      }
    } else if (u->c == ':' && u->i > 1) {
      h->scheme.p = u->q;
      h->scheme.n = u->p - u->q;
      u->q = u->p;
      if (u->i < u->n && u->s[u->i] == '/') {
        if (u->i + 1 < u->n && u->s[u->i + 1] == '/') {
          u->i += 2;
          return true;
        } else {
          return false;
        }
      } else {
        u->f |= kUrlOpaque;
        return false;
      }
    } else if (u->c == '#' || u->c == '?') {
      h->path.p = u->q;
      h->path.n = u->p - u->q;
      u->q = u->p;
      return false;
    } else if (u->c == '%') {
      ParseEscape(u);
      return false;
    } else if (u->c >= 0200 && (u->f & kUrlLatin1)) {
      EmitLatin1(&u->p, u->c);
      return false;
    } else {
      *u->p++ = u->c;
      if (u->i == 1) {
        if (!(('A' <= u->c && u->c <= 'Z') || ('a' <= u->c && u->c <= 'z'))) {
          return false;
        }
      } else {
        if (!(('0' <= u->c && u->c <= '9') || ('A' <= u->c && u->c <= 'Z') ||
              ('a' <= u->c && u->c <= 'z') || u->c == '+' || u->c == '-' ||
              u->c == '.')) {
          return false;
        }
      }
    }
  }
  return false;
}

static void ParseAuthority(struct UrlParser *u, struct Url *h) {
  unsigned t = 1;
  const char *c = NULL;
  while (u->i < u->n) {
    u->c = u->s[u->i++] & 255;
    if (u->c == '/' || u->c == '#' || u->c == '?') {
      break;
    } else if (u->c == '[') {
      t = 0;
    } else if (u->c == ']') {
      t = 1;
    } else if (u->c == ':' && t > 0) {
      *u->p++ = ':';
      c = u->p;
      ++t;
    } else if (u->c == '@') {
      if (c) {
        h->user.p = u->q;
        h->user.n = c - 1 - u->q;
        h->pass.p = (char *)c;
        h->pass.n = u->p - c;
        c = NULL;
        t = 1;
      } else {
        h->user.p = u->q;
        h->user.n = u->p - u->q;
      }
      u->q = u->p;
    } else if (u->c == '%') {
      ParseEscape(u);
    } else if (u->c >= 0200 && (u->f & kUrlLatin1)) {
      EmitLatin1(&u->p, u->c);
    } else {
      *u->p++ = u->c;
    }
  }
  if (t == 2) {
    h->host.p = u->q;
    h->host.n = c - 1 - u->q;
    h->port.p = (char *)c;
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
  while (u->i < u->n) {
    u->c = u->s[u->i++] & 255;
    if (u->c == '#') {
      break;
    } else if (u->c == '?' && !(u->f & kUrlOpaque)) {
      break;
    } else if (u->c == '%') {
      ParseEscape(u);
    } else if (u->c >= 0200 && (u->f & kUrlLatin1)) {
      EmitLatin1(&u->p, u->c);
    } else {
      *u->p++ = u->c;
    }
  }
  h->p = u->q;
  h->n = u->p - u->q;
  u->q = u->p;
}

static void ParseQuery(struct UrlParser *u, struct UrlParams *h) {
  bool t = false;
  if (!h->p) h->p = malloc(0);
  while (u->i < u->n) {
    u->c = u->s[u->i++] & 255;
    if (u->c == '#') {
      break;
    } else if (u->c == '%') {
      ParseEscape(u);
    } else if (u->c == '+') {
      *u->p++ = (u->f & kUrlPlus) ? ' ' : '+';
    } else if (u->c == '&') {
      EmitVal(u, h, t);
      t = false;
    } else if (u->c == '=') {
      if (!t) {
        t = EmitKey(u, h);
      } else {
        *u->p++ = '=';
      }
    } else if (u->c >= 0200 && (u->f & kUrlLatin1)) {
      EmitLatin1(&u->p, u->c);
    } else {
      *u->p++ = u->c;
    }
  }
  EmitVal(u, h, t);
}

static void ParseFragment(struct UrlParser *u, struct UrlView *h) {
  while (u->i < u->n) {
    u->c = u->s[u->i++] & 255;
    if (u->c == '%') {
      ParseEscape(u);
    } else if (u->c >= 0200 && (u->f & kUrlLatin1)) {
      EmitLatin1(&u->p, u->c);
    } else {
      *u->p++ = u->c;
    }
  }
  h->p = u->q;
  h->n = u->p - u->q;
  u->q = u->p;
}

/**
 * Parses URL.
 *
 * This parser is charset agnostic. Percent encoded bytes are decoded
 * for all fields (with the exception of scheme). Returned values might
 * contain things like NUL characters, spaces, control codes, and
 * non-canonical encodings. Absent can be discerned from empty by
 * checking if the pointer is set.
 *
 * There's no failure condition for this routine. This is a permissive
 * parser. This doesn't normalize path segments like `.` or `..` so use
 * IsAcceptablePath() to check for those. No restrictions are imposed
 * beyond that which is strictly necessary for parsing. All the s
 * that is provided will be consumed to the one of the fields. Strict
 * conformance is enforced on some fields more than others, like scheme,
 * since it's the most non-deterministically defined field of them all.
 *
 * Please note this is a URL parser, not a URI parser. Which means we
 * support everything the URI spec says we should do except for the
 * things we won't do, like tokenizing path segments into an array
 * and then nesting another array beneath each of those for storing
 * semicolon parameters. So this parser won't make SIP easy. What it
 * can do is parse HTTP URLs and most URIs like s:opaque, better in
 * fact than most things which claim to be URI parsers.
 *
 * @param s is value like `/hi?x=y&z` or `http://a.example/hi#x`
 * @param n is byte length and -1 implies strlen
 * @param h is assumed to be uninitialized
 * @param f is flags which may have:
 *     - `FLAGS_PLUS` to turn `+` into space in query params
 *     - `FLAGS_LATIN1` to transcode ISO-8859-1 input into UTF-8
 * @return memory backing UrlView needing free (and h.params.p too)
 * @see URI Generic Syntax RFC3986 RFC2396
 * @see EncodeUrl()
 */
char *ParseUrl(const char *s, size_t n, struct Url *h, int f) {
  char *m;
  struct UrlParser u;
  if (n == -1) n = s ? strlen(s) : 0;
  u.i = 0;
  u.c = 0;
  u.s = s;
  u.n = n;
  u.f = f;
  bzero(h, sizeof(*h));
  if ((m = malloc((f & kUrlLatin1) ? u.n * 2 : u.n))) {
    u.q = u.p = m;
    if (ParseScheme(&u, h)) ParseAuthority(&u, h);
    if (u.c != '#' && u.c != '?') ParsePath(&u, &h->path);
    if (u.c == '?') ParseQuery(&u, &h->params);
    if (u.c == '#') ParseFragment(&u, &h->fragment);
  }
  return m;
}

/**
 * Parses HTTP POST key-value params.
 *
 * These are similar to the parameters found in a Request-URI, except
 * usually submitted via an HTTP POST request. We translate `+` into
 * space. The mime type is application/x-www-form-urlencoded.
 *
 * This parser is charset agnostic. Returned values might contain things
 * like NUL characters, NUL, control codes, and non-canonical encodings.
 * Absent can be discerned from empty by checking if the pointer is set.
 *
 * There's no failure condition for this routine. This is a permissive
 * parser that doesn't impose character restrictions beyond what is
 * necessary for parsing.
 *
 * @param s is value like `foo=bar&x=y&z`
 * @param n is byte length and -1 implies strlen
 * @param h must be zeroed by caller and this appends if reused
 * @return UrlView memory with same n needing free (h.p needs free too)
 */
char *ParseParams(const char *s, size_t n, struct UrlParams *h) {
  char *m;
  struct UrlParser u;
  if (n == -1) n = s ? strlen(s) : 0;
  u.i = 0;
  u.s = s;
  u.n = n;
  u.c = '?';
  u.f = kUrlPlus;
  if ((m = malloc(u.n))) {
    u.q = u.p = m;
    ParseQuery(&u, h);
  }
  return m;
}

/**
 * Parses HTTP Host header.
 *
 * The input is ISO-8859-1 which is transcoded to UTF-8. Therefore we
 * assume percent-encoded bytes are expressed as UTF-8. Returned values
 * might contain things like NUL characters, C0, and C1 control codes.
 * UTF-8 isn't checked for validity and may contain overlong values.
 * Absent can be discerned from empty by checking if the pointer is set.
 *
 * This function turns an HTTP header HOST[:PORT] into two strings, one
 * for host and the other for port. You may then call IsAcceptableHost()
 * and IsAcceptablePort() to see if they are valid values. After that a
 * function like sscanf() can be used to do the thing you likely thought
 * this function would do.
 *
 * This function doesn't initialize h since it's assumed this will be
 * called conditionally after ParseRequestUri() if the host is absent.
 * Fields unrelated to authority won't be impacted by this function.
 *
 * @param s is value like `127.0.0.1` or `foo.example:80`
 * @param n is byte length and -1 implies strlen
 * @param h is needs to be initialized by caller
 * @return memory backing UrlView needing free
 */
char *ParseHost(const char *s, size_t n, struct Url *h) {
  char *m;
  struct UrlParser u;
  if (n == -1) n = s ? strlen(s) : 0;
  u.i = 0;
  u.c = 0;
  u.s = s;
  u.n = n;
  u.f = kUrlLatin1;
  if ((m = malloc(u.n * 2))) {
    u.q = u.p = m;
    ParseAuthority(&u, h);
  }
  return m;
}
