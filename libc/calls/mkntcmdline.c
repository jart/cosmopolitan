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

/**
 * Converts System V argv to Windows-style command line.
 *
 * Escaping is performed and it's designed to round-trip with
 * GetDosArgv() or GetDosArgv(). This function does NOT escape
 * command interpreter syntax, e.g. $VAR (sh), %VAR% (cmd).
 *
 * @param cmdline is output buffer
 * @param prog is used as argv[0]
 * @param argv is an a NULL-terminated array of UTF-8 strings
 * @return freshly allocated lpCommandLine or NULL w/ errno
 * @kudos Daniel Colascione for teaching how to quote
 * @see libc/runtime/dosargv.c
 */
textwindows int mkntcmdline(char16_t cmdline[ARG_MAX], const char *prog,
                            char *const argv[]) {
  char *arg;
  uint64_t w;
  wint_t x, y;
  int slashes, n;
  size_t i, j, k;
  bool needsquote;
  char16_t cbuf[2];
  for (arg = prog, k = i = 0; arg; arg = argv[++i]) {
    if (i) {
      cmdline[k++] = u' ';
      if (k == ARG_MAX) return e2big();
    }
    needsquote = !arg[0] || arg[strcspn(arg, " \t\n\v\"")];
    if (needsquote) {
      cmdline[k++] = u'"';
      if (k == ARG_MAX) return e2big();
    }
    for (j = 0;;) {
      if (needsquote) {
        slashes = 0;
        while (arg[j] && arg[j] == '\\') slashes++, j++;
        slashes <<= 1;
        if (arg[j] == '"') slashes++;
        while (slashes--) {
          cmdline[k++] = u'\\';
          if (k == ARG_MAX) return e2big();
        }
      }
      x = arg[j++] & 0xff;
      if (x >= 0300) {
        n = ThomPikeLen(x);
        x = ThomPikeByte(x);
        while (--n) {
          if ((y = arg[j++] & 0xff)) {
            x = ThomPikeMerge(x, y);
          } else {
            x = 0;
            break;
          }
        }
      }
      if (!x) break;
      if (!i && x == '/') x = '\\';
      w = EncodeUtf16(x);
      do {
        cmdline[k++] = w;
        if (k == ARG_MAX) return e2big();
      } while ((w >>= 16));
    }
    if (needsquote) {
      cmdline[k++] = u'"';
      if (k == ARG_MAX) return e2big();
    }
  }
  cmdline[k] = u'\0';
  return 0;
}
