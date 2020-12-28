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
#include "libc/assert.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/sysv/errfuns.h"
#include "net/http/uri.h"

#define static

/* clang-format off */
%% machine uricspn;
%% write data;
/* clang-format on */

int uricspn(const char *data, size_t size) {
  int uricspn$avx(const char *, size_t) hidden;
  const char *p, *pe;
  int cs;

  assert(data || !size);
  assert(size <= 0x7ffff000);
  assert(size <= 0x7ffff000);

  if (X86_HAVE(AVX)) {
    return uricspn$avx(data, size);
  }

  p = data;
  pe = data + size;

  /* clang-format off */

  %%{
    mark = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")";
    reserved = ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | ",";
    unreserved = alnum | mark;
    uric = reserved | unreserved | "%";
    machina := uric*;
  }%%

  %% write init;
  cs = uricspn_en_machina;
  %% write exec;

  /* clang-format on */

  if (cs >= uricspn_first_final) {
    return p - data;
  } else {
    return einval();
  }
}
