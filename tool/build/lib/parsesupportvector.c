/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/fmt/conv.h"
#include "libc/str/str.h"
#include "tool/build/lib/lib.h"

bool ParseSupportVector(char *str, int *out_bits) {
  // you can supply a number, e.g. 123, 0x123, etc.
  char *endptr;
  int bits = strtol(str, &endptr, 0);
  if (!*endptr) {
    *out_bits = bits;
    return true;
  }
  // you can supply a string, e.g. -s linux+mac+bsd
  bits = 0;
  char *tok, *state;
  const char *sep = " ,+:/|";
  while ((tok = strtok_r(str, sep, &state))) {
    if (startswithi(tok, "_host")) {
      tok += 5;
    }
    if (!strcasecmp(tok, "linux")) {
      bits |= _HOSTLINUX;
    } else if (!strcasecmp(tok, "metal")) {
      bits |= _HOSTMETAL;
    } else if (!strcasecmp(tok, "windows") ||  //
               !strcasecmp(tok, "win") ||      //
               !strcasecmp(tok, "nt") ||       //
               !strcasecmp(tok, "pe")) {
      bits |= _HOSTWINDOWS;
    } else if (!strcasecmp(tok, "xnu") ||    //
               !strcasecmp(tok, "mac") ||    //
               !strcasecmp(tok, "macos") ||  //
               !strcasecmp(tok, "macho") ||  //
               !strcasecmp(tok, "darwin")) {
      bits |= _HOSTXNU;
    } else if (!strcasecmp(tok, "freebsd")) {
      bits |= _HOSTFREEBSD;
    } else if (!strcasecmp(tok, "openbsd")) {
      bits |= _HOSTOPENBSD;
    } else if (!strcasecmp(tok, "netbsd")) {
      bits |= _HOSTNETBSD;
    } else if (!strcasecmp(tok, "bsd")) {
      bits |= _HOSTFREEBSD | _HOSTOPENBSD | _HOSTNETBSD;
    } else if (!strcasecmp(tok, "elf")) {
      bits |=
          _HOSTMETAL | _HOSTLINUX | _HOSTFREEBSD | _HOSTOPENBSD | _HOSTNETBSD;
    } else if (!strcasecmp(tok, "unix")) {
      bits |= _HOSTLINUX | _HOSTFREEBSD | _HOSTOPENBSD | _HOSTNETBSD | _HOSTXNU;
    } else {
      return false;
    }
    str = 0;
  }
  *out_bits = bits;
  return true;
}
