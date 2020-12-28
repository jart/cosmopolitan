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
#include "libc/str/str.h"

static textwindows bool shouldescapedos(const char16_t c) {
  if (c == u'"') return true;
  if (c == u'&') return true;
  if (c == u'%') return true;
  if (c == u'^') return true;
  if (c == u'<') return true;
  if (c == u'>') return true;
  if (c == u'|') return true;
  return false;
}

static textwindows bool shouldquotedos(const char16_t c) {
  if (c == u' ') return true;
  if (c == u'\t') return true;
  if (c == u'\n') return true;
  if (c == u'\v') return true;
  if (c == u'"') return true;
  if (c == u'*') return true;
  return shouldescapedos(c);
}

/**
 * Escapes command so DOS can run it.
 * @see Iain Patterson's NSSM for original code in public domain
 */
textwindows bool escapedos(char16_t *buffer, unsigned buflen,
                           const char16_t *unquoted, unsigned len) {
  unsigned i, j, n;
  if (len > buflen - 1) return false;
  bool escape = false;
  bool quotes = false;
  for (i = 0; i < len; i++) {
    if (shouldescapedos(unquoted[i])) {
      escape = quotes = true;
      break;
    }
    if (shouldquotedos(unquoted[i])) quotes = true;
  }
  if (!quotes) {
    memmove(buffer, unquoted, (len + 1) * sizeof(char16_t));
    return true;
  }
  /* "" */
  unsigned quoted_len = 2;
  if (escape) quoted_len += 2;
  for (i = 0;; i++) {
    n = 0;
    while (i != len && unquoted[i] == u'\\') {
      i++;
      n++;
    }
    if (i == len) {
      quoted_len += n * 2;
      break;
    } else if (unquoted[i] == u'"')
      quoted_len += n * 2 + 2;
    else
      quoted_len += n + 1;
    if (shouldescapedos(unquoted[i])) quoted_len += n;
  }
  if (quoted_len > buflen - 1) return false;
  char16_t *s = buffer;
  if (escape) *s++ = u'^';
  *s++ = u'"';
  for (i = 0;; i++) {
    n = 0;
    while (i != len && unquoted[i] == u'\\') {
      i++;
      n++;
    }
    if (i == len) {
      for (j = 0; j < n * 2; j++) {
        if (escape) *s++ = u'^';
        *s++ = u'\\';
      }
      break;
    } else if (unquoted[i] == u'"') {
      for (j = 0; j < n * 2 + 1; j++) {
        if (escape) *s++ = u'^';
        *s++ = u'\\';
      }
      if (escape && shouldescapedos(unquoted[i])) *s++ = u'^';
      *s++ = unquoted[i];
    } else {
      for (j = 0; j < n; j++) {
        if (escape) *s++ = u'^';
        *s++ = u'\\';
      }
      if (escape && shouldescapedos(unquoted[i])) *s++ = u'^';
      *s++ = unquoted[i];
    }
  }
  if (escape) *s++ = u'^';
  *s++ = u'"';
  *s++ = u'\0';
  return true;
}
