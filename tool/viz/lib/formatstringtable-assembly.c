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
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/conv/itoa.h"
#include "libc/runtime/gc.h"
#include "libc/str/str.h"
#include "libc/unicode/unicode.h"
#include "libc/x/x.h"
#include "tool/viz/lib/formatstringtable.h"

#define STREQ(A, B) (strcasecmp(A, B) == 0)

static int GetArrayAlignment(long yn, long xn, int w, int align) {
  /* abi guaranteed to 16 after which gcc / clang disagree */
  int i, r;
  r = i = align;
  while ((i *= 2) <= __BIGGEST_ALIGNMENT__) {
    if (yn * xn * w >= i) {
      r = i;
    }
  }
  return r;
}

static const char *GetStorageSpecifier(const char *type, int *out_width,
                                       int *out_align) {
  if (STREQ(type, "unsigned char") || STREQ(type, "char") ||
      STREQ(type, "signed char")) {
    *out_width = 1;
    *out_align = 1;
    return "byte";
  } else if (STREQ(type, "unsigned short") || STREQ(type, "short") ||
             STREQ(type, "signed short")) {
    *out_width = 2;
    *out_align = 2;
    return "short";
  } else if (STREQ(type, "unsigned int") || STREQ(type, "unsigned") ||
             STREQ(type, "int") || STREQ(type, "signed int")) {
    *out_width = 4;
    *out_align = 4;
    return "long";
  } else if (STREQ(type, "unsigned long") || STREQ(type, "unsigned") ||
             STREQ(type, "long") || STREQ(type, "signed long")) {
    *out_width = 8;
    *out_align = 8;
    return "quad";
  } else if (STREQ(type, "float")) {
    *out_width = 4;
    *out_align = 4;
    return "float";
  } else if (STREQ(type, "double")) {
    *out_width = 8;
    *out_align = 8;
    return "double";
  } else {
    *out_width = __BIGGEST_ALIGNMENT__;
    *out_align = __BIGGEST_ALIGNMENT__;
    return type;
  }
}

static void EmitSection(long yn, long xn, int w, int arrayalign, int emit(),
                        void *a) {
  char alignstr[20];
  uint64toarray_radix10(arrayalign, alignstr);
  if (arrayalign <= 8 && yn * xn * w == 8) {
    emit("\t.rodata.cst", a);
    emit("8\n", a);
  } else if (arrayalign <= 16 && yn * xn * w == 16) {
    emit("\t.rodata.cst", a);
    emit("16\n", a);
  } else if (arrayalign <= 32 && yn * xn * w == 32) {
    emit("\t.rodata.cst", a);
    emit("32\n", a);
  } else if (arrayalign <= 64 && yn * xn * w == 64) {
    emit("\t.rodata.cst", a);
    emit("64\n", a);
  } else {
    emit("\t.rodata\n", a);
    emit("\t.align\t", a);
    emit(alignstr, a);
    emit("\n", a);
  }
}

void *FormatStringTableAsAssembly(long yn, long xn, const char *const T[yn][xn],
                                  int emit(), void *a, const char *type,
                                  const char *name, const char *scope) {
  int w, align;
  const char *storage;
  char ynstr[20], xnstr[20];
  name = firstnonnull(name, "M");
  storage = GetStorageSpecifier(firstnonnull(type, "long"), &w, &align);
  uint64toarray_radix10(yn, ynstr);
  uint64toarray_radix10(xn, xnstr);
  EmitSection(yn, xn, w, GetArrayAlignment(yn, xn, w, align), emit, a);
  emit(name, a);
  emit(":", a);
  if (strwidth(name) >= 8) emit("\n", a);
  FormatStringTable(yn, xn, T, emit, a, gc(xstrcat("\t.", storage, "\t")), ",",
                    "\n");
  emit("\t.endobj\t", a);
  emit(name, a);
  emit(",", a);
  emit(firstnonnull(scope, "globl"), a);
  emit("\n\t.previous\n", a);
  return (/* unconst */ void *)T;
}
