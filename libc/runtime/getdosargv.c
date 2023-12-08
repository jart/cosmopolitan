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
#include "libc/runtime/internal.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/str/utf16.h"

struct DosArgv {
  const char16_t *s;
  char *p;
  char *pe;
  wint_t wc;
};

textwindows void DecodeDosArgv(int ignore, struct DosArgv *st) {
  wint_t x, y;
  for (;;) {
    if (!(x = *st->s++)) break;
    if (!IsUcs2(x)) {
      if ((y = *st->s++)) {
        x = MergeUtf16(x, y);
      } else {
        x = 0;
      }
    }
    break;
  }
  st->wc = x;
}

static textwindows void AppendDosArgv(wint_t wc, struct DosArgv *st) {
  uint64_t w;
  w = tpenc(wc);
  do {
    if (st->p >= st->pe) break;
    *st->p++ = w & 0xff;
  } while (w >>= 8);
}

static textwindows int Count(int c, struct DosArgv *st) {
  int ignore, n = 0;
  asm("" : "=g"(ignore));
  while (st->wc == c) {
    DecodeDosArgv(ignore, st);
    n++;
  }
  return n;
}

// Tokenizes and transcodes Windows NT CLI args, thus avoiding
// CommandLineToArgv() schlepping in forty megs of dependencies.
//
// @param s is the command line string provided by the executive
// @param buf is where we'll store double-NUL-terminated decoded args
// @param size is how many bytes are available in buf
// @param argv is where we'll store the decoded arg pointer array, which
//     is guaranteed to be NULL-terminated if max>0
// @param max specifies the item capacity of argv, or 0 to do scanning
// @return number of args written, excluding the NULL-terminator; or,
//     if the output buffer wasn't passed, or was too short, then the
//     number of args that *would* have been written is returned; and
//     there are currently no failure conditions that would have this
//     return -1 since it doesn't do system calls
// @see test/libc/dosarg_test.c
// @see libc/runtime/ntspawn.c
// @note kudos to Simon Tatham for figuring out quoting behavior
textwindows int GetDosArgv(const char16_t *cmdline, char *buf, size_t size,
                           char **argv, size_t max) {
  bool inquote;
  int i, argc, slashes, quotes, ignore;
  static struct DosArgv st_;
  struct DosArgv *st = &st_;
  asm("" : "=g"(ignore));
  asm("" : "+r"(st));
  st->s = cmdline;
  st->p = buf;
  st->pe = buf + size;
  argc = 0;
  DecodeDosArgv(ignore, st);
  while (st->wc) {
    while (st->wc && (st->wc == ' ' || st->wc == '\t')) {
      DecodeDosArgv(ignore, st);
    }
    if (!st->wc) break;
    if (++argc < max) {
      argv[argc - 1] = st->p < st->pe ? st->p : NULL;
    }
    inquote = false;
    while (st->wc) {
      if (!inquote && (st->wc == ' ' || st->wc == '\t')) break;
      if (st->wc == '"' || st->wc == '\\') {
        slashes = Count('\\', st);
        quotes = Count('"', st);
        if (!quotes) {
          while (slashes--) {
            AppendDosArgv('\\', st);
          }
        } else {
          while (slashes >= 2) {
            AppendDosArgv('\\', st);
            slashes -= 2;
          }
          if (slashes) {
            AppendDosArgv('"', st);
            quotes--;
          }
          if (quotes > 0) {
            if (!inquote) quotes--;
            for (i = 3; i <= quotes + 1; i += 3) {
              AppendDosArgv('"', st);
            }
            inquote = (quotes % 3 == 0);
          }
        }
      } else {
        AppendDosArgv(st->wc, st);
        DecodeDosArgv(ignore, st);
      }
    }
    AppendDosArgv('\0', st);
  }
  AppendDosArgv('\0', st);
  if (size) buf[MIN(st->p - buf, size - 1)] = '\0';
  if (max) argv[MIN(argc, max - 1)] = NULL;
  return argc;
}
