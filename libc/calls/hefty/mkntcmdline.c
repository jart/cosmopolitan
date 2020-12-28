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
#include "libc/alg/arraylist2.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/hefty/ntspawn.h"
#include "libc/fmt/conv.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/hascharacter.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/oldutf16.internal.h"
#include "libc/str/str.h"
#include "libc/str/tpdecode.internal.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/errfuns.h"

#define APPENDCHAR(C)                                                      \
  do {                                                                     \
    if (mkntcmdline_append(&cmdline_p, &cmdline_i, &cmdline_n, C) == -1) { \
      goto error;                                                          \
    }                                                                      \
  } while (0)

static int mkntcmdline_append(char16_t **p, size_t *i, size_t *n, char16_t c) {
  return APPEND(p, i, n, &c);
}

/**
 * Converts System V argv to Windows-style command line.
 *
 * Escaping is performed and it's designed to round-trip with
 * GetDosArgv() or GetDosArgv(). This function does NOT escape
 * command interpreter syntax, e.g. $VAR (sh), %VAR% (cmd).
 *
 * @param argv is an a NULL-terminated array of UTF-8 strings
 * @return freshly allocated lpCommandLine or NULL w/ errno
 * @kudos Daniel Colascione for teaching how to quote
 * @see libc/runtime/dosargv.c
 */
hidden textwindows nodiscard char16_t *mkntcmdline(char *const argv[]) {
  wint_t wc;
  size_t i, j;
  char16_t cbuf[2];
  char16_t *cmdline_p = NULL;
  size_t cmdline_i = 0;
  size_t cmdline_n = 0;
  if (argv[0]) {
    for (i = 0; argv[i]; ++i) {
      if (i) APPENDCHAR(u' ');
      bool needsquote = !argv[i] || !!argv[i][strcspn(argv[i], " \t\n\v\"")];
      if (needsquote) APPENDCHAR(u'"');
      for (j = 0;;) {
        if (needsquote) {
          int slashes = 0;
          while (argv[i][j] && argv[i][j] == u'\\') slashes++, j++;
          slashes <<= 1;
          if (argv[i][j] == u'"') slashes++;
          while (slashes--) APPENDCHAR(u'\\');
        }
        if (!argv[i][j]) break;
        j += abs(tpdecode(&argv[i][j], &wc));
        if (CONCAT(&cmdline_p, &cmdline_i, &cmdline_n, cbuf,
                   abs(pututf16(cbuf, ARRAYLEN(cbuf), wc, false))) == -1) {
          goto error;
        }
      }
      if (needsquote) APPENDCHAR(u'"');
    }
    APPENDCHAR(u'\0');
    if (cmdline_i > CMD_MAX) {
      e2big();
      goto error;
    }
  } else {
    einval();
  }
  return cmdline_p;
error:
  free(cmdline_p);
  return NULL;
}
