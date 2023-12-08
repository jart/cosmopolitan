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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/nt/files.h"
#include "libc/proc/ntspawn.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/utf16.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"

#define APPEND(c)     \
  do {                \
    if (k == 32766) { \
      return e2big(); \
    }                 \
    cmdline[k++] = c; \
  } while (0)

static bool NeedsQuotes(const char *s) {
  if (!*s) {
    return true;
  }
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

static bool LooksLikeCosmoDrivePath(const char *s) {
  return s[0] == '/' &&    //
         IsAlpha(s[1]) &&  //
         s[2] == '/';
}

// Converts System V argv to Windows-style command line.
//
// Escaping is performed and it's designed to round-trip with
// GetDosArgv() or GetDosArgv(). This function does NOT escape
// command interpreter syntax, e.g. $VAR (sh), %VAR% (cmd).
//
// @param cmdline is output buffer
// @param argv is an a NULL-terminated array of UTF-8 strings
// @return 0 on success, or -1 w/ errno
// @raise E2BIG if everything is too huge
// @see "Everyone quotes command line arguments the wrong way" MSDN
// @see libc/runtime/getdosargv.c
// @asyncsignalsafe
textwindows int mkntcmdline(char16_t cmdline[32767], char *const argv[]) {
  char *arg;
  int slashes, n;
  bool needsquote;
  size_t i, j, k, s;
  char argbuf[PATH_MAX];
  for (k = i = 0; argv[i]; ++i) {
    if (i) APPEND(u' ');
    if (LooksLikeCosmoDrivePath(argv[i]) &&
        strlcpy(argbuf, argv[i], PATH_MAX) < PATH_MAX) {
      mungentpath(argbuf);
      arg = argbuf;
    } else {
      arg = argv[i];
    }
    if ((needsquote = NeedsQuotes(arg))) {
      APPEND(u'"');
    }
    for (slashes = j = 0;;) {
      wint_t x = arg[j++] & 255;
      if (x >= 0300) {
        n = ThomPikeLen(x);
        x = ThomPikeByte(x);
        while (--n) {
          wint_t y;
          if ((y = arg[j++] & 255)) {
            x = ThomPikeMerge(x, y);
          } else {
            x = 0;
            break;
          }
        }
      }
      if (!x) break;
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
        uint32_t w = EncodeUtf16(x);
        do APPEND(w);
        while ((w >>= 16));
      }
    }
    for (s = 0; s < (slashes << needsquote); ++s) {
      APPEND(u'\\');
    }
    if (needsquote) {
      APPEND(u'"');
    }
  }
  cmdline[k] = 0;
  return 0;
}
