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
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/utf16.h"
#include "libc/sysv/errfuns.h"

#define APPEND(c)           \
  do {                      \
    cmdline[k++] = c;       \
    if (k == ARG_MAX / 2) { \
      return e2big();       \
    }                       \
  } while (0)

static bool NeedsQuotes(const char *s) {
  if (!*s) return true;
  do {
    switch (*s) {
      case '"':
      case ' ':
      case '\t':
      case '\v':
      case '\n':
        return true;
      default:
        break;
    }
  } while (*s++);
  return false;
}

static inline int IsAlpha(int c) {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

// Converts System V argv to Windows-style command line.
//
// Escaping is performed and it's designed to round-trip with
// GetDosArgv() or GetDosArgv(). This function does NOT escape
// command interpreter syntax, e.g. $VAR (sh), %VAR% (cmd).
//
// TODO(jart): this needs fuzzing and security review
//
// @param cmdline is output buffer
// @param argv is an a NULL-terminated array of UTF-8 strings
// @return 0 on success, or -1 w/ errno
// @raise E2BIG if everything is too huge
// @see "Everyone quotes command line arguments the wrong way" MSDN
// @see libc/runtime/getdosargv.c
textwindows int mkntcmdline(char16_t cmdline[ARG_MAX / 2], char *const argv[]) {
  uint64_t w;
  wint_t x, y;
  int slashes, n;
  bool needsquote;
  char16_t cbuf[2];
  char *ansiargv[2];
  size_t i, j, k, s;
  if (!argv[0]) {
    bzero(ansiargv, sizeof(ansiargv));
    argv = ansiargv;
  }
  for (k = i = 0; argv[i]; ++i) {
    if (i) APPEND(u' ');
    if ((needsquote = NeedsQuotes(argv[i]))) APPEND(u'"');
    for (slashes = j = 0;;) {
      x = argv[i][j++] & 255;
      if (x >= 0300) {
        n = ThomPikeLen(x);
        x = ThomPikeByte(x);
        while (--n) {
          if ((y = argv[i][j++] & 255)) {
            x = ThomPikeMerge(x, y);
          } else {
            x = 0;
            break;
          }
        }
      }
      if (!x) break;
      if (x == '/' || x == '\\') {
        if (!i) {
          // turn / into \ for first argv[i]
          x = '\\';
          // turn \c\... into c:\ for first argv[i]
          if (k == 2 && IsAlpha(cmdline[1]) && cmdline[0] == '\\') {
            cmdline[0] = cmdline[1];
            cmdline[1] = ':';
          }
        } else {
          // turn stuff like `less /c/...`
          //            into `less c:/...`
          // turn stuff like `more <"/c/..."`
          //            into `more <"c:/..."`
          if (k > 3 && IsAlpha(cmdline[k - 1]) &&
              (cmdline[k - 2] == '/' || cmdline[k - 2] == '\\') &&
              (cmdline[k - 3] == '"' || cmdline[k - 3] == ' ')) {
            cmdline[k - 2] = cmdline[k - 1];
            cmdline[k - 1] = ':';
          }
        }
      }
      if (x == '\\') {
        ++slashes;
      } else if (x == '"') {
        APPEND(u'"');
        APPEND(u'"');
        APPEND(u'"');
      } else {
        for (s = 0; s < slashes; ++s) {
          APPEND(u'\\');
        }
        slashes = 0;
        w = EncodeUtf16(x);
        do {
          APPEND(w);
        } while ((w >>= 16));
      }
    }
    for (s = 0; s < (slashes << needsquote); ++s) {
      APPEND(u'\\');
    }
    if (needsquote) {
      APPEND(u'"');
    }
  }
  cmdline[k] = u'\0';
  return 0;
}
