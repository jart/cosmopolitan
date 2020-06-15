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
