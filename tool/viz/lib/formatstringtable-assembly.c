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
#include "libc/fmt/itoa.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/mem/gc.h"
#include "libc/str/str.h"
#include "libc/str/strwidth.h"
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
  char alignstr[21];
  FormatUint32(alignstr, arrayalign);
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
  char ynstr[21], xnstr[21];
  name = firstnonnull(name, "M");
  storage = GetStorageSpecifier(firstnonnull(type, "long"), &w, &align);
  FormatUint64(ynstr, yn);
  FormatUint64(xnstr, xn);
  EmitSection(yn, xn, w, GetArrayAlignment(yn, xn, w, align), emit, a);
  emit(name, a);
  emit(":", a);
  if (strwidth(name, 0) >= 8) emit("\n", a);
  FormatStringTable(yn, xn, T, emit, a, gc(xstrcat("\t.", storage, "\t")), ",",
                    "\n");
  emit("\t.endobj\t", a);
  emit(name, a);
  emit(",", a);
  emit(firstnonnull(scope, "globl"), a);
  emit("\n\t.previous\n", a);
  return (/* unconst */ void *)T;
}
