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
#include "libc/calls/ntspawn.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/bits.h"
#include "libc/macros.internal.h"
#include "libc/mem/alloca.h"
#include "libc/mem/arraylist2.internal.h"
#include "libc/mem/mem.h"
#include "libc/str/oldutf16.internal.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/tpdecode.internal.h"
#include "libc/str/utf16.h"
#include "libc/sysv/errfuns.h"

#define ToUpper(c) ((c) >= 'a' && (c) <= 'z' ? (c) - 'a' + 'A' : (c))

static inline int IsAlpha(int c) {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

static inline char *StrChr(char *s, int c) {
  for (;; ++s) {
    if ((*s & 255) == (c & 255)) return s;
    if (!*s) return 0;
  }
}

static textwindows inline int CompareStrings(const char *l, const char *r) {
  int a, b;
  size_t i = 0;
  while ((a = ToUpper(l[i] & 255)) == (b = ToUpper(r[i] & 255)) && r[i]) ++i;
  return a - b;
}

static textwindows void FixPath(char *path) {
  char *p;
  size_t i;

  // skip over variable name
  while (*path++) {
    if (path[-1] == '=') {
      break;
    }
  }

  // turn colon into semicolon
  // unless it already looks like a dos path
  for (p = path; *p; ++p) {
    if (p[0] == ':' && p[1] != '\\') {
      p[0] = ';';
    }
  }

  // turn \c\... into c:\...
  p = path;
  if ((p[0] == '/' | p[0] == '\\') && IsAlpha(p[1]) &&
      (p[2] == '/' || p[2] == '\\')) {
    p[0] = p[1];
    p[1] = ':';
  }
  for (; *p; ++p) {
    if (p[0] == ';' && (p[1] == '/' || p[1] == '\\') && IsAlpha(p[2]) &&
        (p[3] == '/' || p[3] == '\\')) {
      p[1] = p[2];
      p[2] = ':';
    }
  }

  // turn slash into backslash
  for (p = path; *p; ++p) {
    if (*p == '/') {
      *p = '\\';
    }
  }
}

static textwindows void InsertString(char **a, size_t i, char *s,
                                     char buf[ARG_MAX], size_t *bufi) {
  char *v;
  size_t j, k;

  // apply fixups to var=/c/...
  if ((v = StrChr(s, '=')) && v[1] == '/' && IsAlpha(v[2]) && v[3] == '/') {
    v = buf + *bufi;
    for (k = 0; s[k]; ++k) {
      if (*bufi + 1 < ARG_MAX) {
        buf[(*bufi)++] = s[k];
      }
    }
    if (*bufi < ARG_MAX) {
      buf[(*bufi)++] = 0;
      FixPath(v);
      s = v;
    }
  }

  // append to sorted list
  for (j = i; j > 0 && CompareStrings(s, a[j - 1]) < 0; --j) {
    a[j] = a[j - 1];
  }
  a[j] = s;
}

/**
 * Copies sorted environment variable block for Windows.
 *
 * This is designed to meet the requirements of CreateProcess().
 *
 * @param envvars receives sorted double-NUL terminated string list
 * @param envp is an a NULL-terminated array of UTF-8 strings
 * @param extravar is a VAR=val string we consider part of envp or NULL
 * @return 0 on success, or -1 w/ errno
 * @error E2BIG if total number of shorts exceeded ARG_MAX/2 (32767)
 */
textwindows int mkntenvblock(char16_t envvars[ARG_MAX / 2], char *const envp[],
                             const char *extravar, char buf[ARG_MAX]) {
  bool v;
  char *t;
  axdx_t rc;
  uint64_t w;
  char **vars;
  wint_t x, y;
  size_t i, j, k, n, m, bufi = 0;
  for (n = 0; envp[n];) n++;
  vars = alloca((n + 1) * sizeof(char *));
  for (i = 0; i < n; ++i) InsertString(vars, i, envp[i], buf, &bufi);
  if (extravar) InsertString(vars, n++, extravar, buf, &bufi);
  for (k = i = 0; i < n; ++i) {
    j = 0;
    v = false;
    do {
      x = vars[i][j++] & 0xff;
      if (x >= 0200) {
        if (x < 0300) continue;
        m = ThomPikeLen(x);
        x = ThomPikeByte(x);
        while (--m) {
          if ((y = vars[i][j++] & 0xff)) {
            x = ThomPikeMerge(x, y);
          } else {
            x = 0;
            break;
          }
        }
      }
      if (!v) {
        if (x != '=') {
          x = ToUpper(x);
        } else {
          v = true;
        }
      }
      w = EncodeUtf16(x);
      do {
        envvars[k++] = w & 0xffff;
        if (k == ARG_MAX / 2) {
          return e2big();
        }
      } while ((w >>= 16));
    } while (x);
  }
  envvars[k] = u'\0';
  return 0;
}
