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
#include "libc/alg/reverse.internal.h"
#include "libc/bits/pushpop.h"
#include "libc/calls/calls.h"
#include "libc/fmt/fmt.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/log/ubsan.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"

static char __ubsan_buf[256];

static const char kUbsanTypeCheckKinds[] = "\
load of\0\
store to\0\
reference binding to\0\
member access within\0\
member call on\0\
constructor call on\0\
downcast of\0\
downcast of\0\
upcast of\0\
cast to virtual base of\0\
\0";

static int __ubsan_bits(struct UbsanTypeDescriptor *t) {
  return 1 << (t->info >> 1);
}

static bool __ubsan_signed(struct UbsanTypeDescriptor *t) {
  return t->info & 1;
}

static bool __ubsan_negative(struct UbsanTypeDescriptor *t, uintptr_t x) {
  return __ubsan_signed(t) && (intptr_t)x < 0;
}

static size_t __ubsan_strlen(const char *s) {
  size_t n = 0;
  while (*s++) ++n;
  return n;
}

static char *__ubsan_stpcpy(char *d, const char *s) {
  size_t i;
  for (i = 0;; ++i) {
    if (!(d[i] = s[i])) {
      return d + i;
    }
  }
}

static char *__ubsan_poscpy(char *p, uintptr_t i) {
  int j = 0;
  do {
    p[j++] = i % 10 + '0';
    i /= 10;
  } while (i > 0);
  reverse(p, j);
  return p + j;
}

static char *__ubsan_intcpy(char *p, intptr_t i) {
  if (i >= 0) return __ubsan_poscpy(p, i);
  *p++ = '-';
  return __ubsan_poscpy(p, -i);
}

static char *__ubsan_hexcpy(char *p, uintptr_t x, int k) {
  while (k) *p++ = "0123456789abcdef"[(x >> (k -= 4)) & 15];
  return p;
}

static char *__ubsan_itpcpy(char *p, struct UbsanTypeDescriptor *t,
                            uintptr_t x) {
  if (__ubsan_signed(t)) {
    return __ubsan_intcpy(p, x);
  } else {
    return __ubsan_poscpy(p, x);
  }
}

static const char *__ubsan_dubnul(const char *s, unsigned i) {
  size_t n;
  while (i--) {
    if ((n = __ubsan_strlen(s))) {
      s += n + 1;
    } else {
      return NULL;
    }
  }
  return s;
}

static uintptr_t __ubsan_extend(struct UbsanTypeDescriptor *t, uintptr_t x) {
  int w;
  w = __ubsan_bits(t);
  if (w < sizeof(x) * CHAR_BIT) {
    x <<= sizeof(x) * CHAR_BIT - w;
    if (__ubsan_signed(t)) {
      x = (intptr_t)x >> w;
    } else {
      x >>= w;
    }
  }
  return x;
}

void __ubsan_abort(const struct UbsanSourceLocation *loc,
                   const char *description) {
  static bool once;
  if (!once) {
    once = true;
  } else {
    abort();
  }
  if (IsDebuggerPresent(false)) DebugBreak();
  __start_fatal(loc->file, loc->line);
  write(2, description, strlen(description));
  write(2, "\r\n", 2);
  __die();
}

void __ubsan_handle_shift_out_of_bounds(struct UbsanShiftOutOfBoundsInfo *info,
                                        uintptr_t lhs, uintptr_t rhs) {
  char *p;
  const char *s;
  lhs = __ubsan_extend(info->lhs_type, lhs);
  rhs = __ubsan_extend(info->rhs_type, rhs);
  if (__ubsan_negative(info->rhs_type, rhs)) {
    s = "shift exponent is negative";
  } else if (rhs >= __ubsan_bits(info->lhs_type)) {
    s = "shift exponent too large for type";
  } else if (__ubsan_negative(info->lhs_type, lhs)) {
    s = "left shift of negative value";
  } else if (__ubsan_signed(info->lhs_type)) {
    s = "signed left shift changed sign bit or overflowed";
  } else {
    s = "wut shift out of bounds";
  }
  p = __ubsan_buf;
  p = __ubsan_stpcpy(p, s), *p++ = ' ';
  p = __ubsan_itpcpy(p, info->lhs_type, lhs), *p++ = ' ';
  p = __ubsan_stpcpy(p, info->lhs_type->name), *p++ = ' ';
  p = __ubsan_itpcpy(p, info->rhs_type, rhs), *p++ = ' ';
  p = __ubsan_stpcpy(p, info->rhs_type->name);
  __ubsan_abort(&info->location, __ubsan_buf);
}

void __ubsan_handle_out_of_bounds(struct UbsanOutOfBoundsInfo *info,
                                  uintptr_t index) {
  char *p;
  p = __ubsan_buf;
  p = __ubsan_stpcpy(p, info->index_type->name);
  p = __ubsan_stpcpy(p, " index ");
  p = __ubsan_itpcpy(p, info->index_type, index);
  p = __ubsan_stpcpy(p, " into ");
  p = __ubsan_stpcpy(p, info->array_type->name);
  p = __ubsan_stpcpy(p, " out of bounds");
  __ubsan_abort(&info->location, __ubsan_buf);
}

void __ubsan_handle_type_mismatch(struct UbsanTypeMismatchInfo *info,
                                  uintptr_t pointer) {
  char *p;
  const char *kind;
  if (!pointer) __ubsan_abort(&info->location, "null pointer access");
  p = __ubsan_buf;
  kind = __ubsan_dubnul(kUbsanTypeCheckKinds, info->type_check_kind);
  if (info->alignment && (pointer & (info->alignment - 1))) {
    p = __ubsan_stpcpy(p, "unaligned ");
    p = __ubsan_stpcpy(p, kind), *p++ = ' ';
    p = __ubsan_stpcpy(p, info->type->name), *p++ = ' ', *p++ = '@';
    p = __ubsan_itpcpy(p, info->type, pointer);
    p = __ubsan_stpcpy(p, " align ");
    p = __ubsan_intcpy(p, info->alignment);
  } else {
    p = __ubsan_stpcpy(p, "insufficient size\r\n\t");
    p = __ubsan_stpcpy(p, kind);
    p = __ubsan_stpcpy(p, " address 0x");
    p = __ubsan_hexcpy(p, pointer, sizeof(pointer) * CHAR_BIT);
    p = __ubsan_stpcpy(p, " with insufficient space for object of type ");
    p = __ubsan_stpcpy(p, info->type->name);
  }
  __ubsan_abort(&info->location, __ubsan_buf);
}

void ___ubsan_handle_type_mismatch_v1(
    struct UbsanTypeMismatchInfoClang *type_mismatch, uintptr_t pointer) {
  struct UbsanTypeMismatchInfo mm;
  mm.location = type_mismatch->location;
  mm.type = type_mismatch->type;
  mm.alignment = 1u << type_mismatch->log_alignment;
  mm.type_check_kind = type_mismatch->type_check_kind;
  __ubsan_handle_type_mismatch(&mm, pointer);
}

void __ubsan_handle_float_cast_overflow(void *data_raw, void *from_raw) {
  struct UbsanFloatCastOverflowData *data =
      (struct UbsanFloatCastOverflowData *)data_raw;
#if __GNUC__ + 0 >= 6
  __ubsan_abort(&data->location, "float cast overflow");
#else
  const struct UbsanSourceLocation kUnknownLocation = {
      "<unknown file>",
      pushpop(0),
      pushpop(0),
  };
  __ubsan_abort(((void)data, &kUnknownLocation), "float cast overflow");
#endif
}
