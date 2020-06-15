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
#include "libc/escape/escape.h"
#include "libc/str/str.h"

asm(".ident\t\"\\n\\n\
NSSM (Public Domain)\\n\
Credit: Iain Patterson\n\
http://iain.cx/\"");

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
