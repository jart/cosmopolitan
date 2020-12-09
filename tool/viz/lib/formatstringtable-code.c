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
#include "libc/bits/safemacros.internal.h"
#include "libc/fmt/itoa.h"
#include "tool/viz/lib/formatstringtable.h"

void *FormatStringTableAsCode(long yn, long xn, const char *const T[yn][xn],
                              int emit(), void *arg, const char *type,
                              const char *name, const char *ignored) {
  char ynstr[20], xnstr[20];
  uint64toarray_radix10(yn, ynstr);
  uint64toarray_radix10(xn, xnstr);
  emit(type, arg);
  emit(" ", arg);
  emit(firstnonnull(name, "M"), arg);
  emit("[", arg);
  emit(ynstr, arg);
  emit("][", arg);
  emit(xnstr, arg);
  emit("] = {", arg);
  FormatStringTable(yn, xn, T, emit, arg, "\n    {", ", ", "},");
  emit("\n};\n", arg);
  return (/* unconst */ void *)T;
}
