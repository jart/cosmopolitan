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
  fprintf(stderr, "%s\r\n", description);
  __die();
  unreachable;
}

void __ubsan_handle_out_of_bounds(struct UbsanOutOfBoundsInfo *info,
                                  uintptr_t index) {
  snprintf(__ubsan_buf, sizeof(__ubsan_buf),
           "%s index %,lu into %s out of bounds", info->index_type->name, index,
           info->array_type->name);
  __ubsan_abort(&info->location, __ubsan_buf);
  unreachable;
}

void __ubsan_handle_type_mismatch(struct UbsanTypeMismatchInfo *type_mismatch,
                                  uintptr_t pointer) {
  struct UbsanSourceLocation *loc = &type_mismatch->location;
  const char *description;
  const char *kind = IndexDoubleNulString(kUbsanTypeCheckKinds,
                                          type_mismatch->type_check_kind);
  if (pointer == 0) {
    description = "null pointer access";
  } else if (type_mismatch->alignment != 0 &&
             (pointer & (type_mismatch->alignment - 1))) {
    description = __ubsan_buf;
    snprintf(__ubsan_buf, sizeof(__ubsan_buf), "%s %s %s @%p %s %d",
             "unaligned", kind, type_mismatch->type->name, pointer, "align",
             type_mismatch->alignment);
  } else {
    description = __ubsan_buf;
    snprintf(__ubsan_buf, sizeof(__ubsan_buf), "%s\r\n\t%s %s %p %s %s",
             "insufficient size", kind, "address", pointer,
             "with insufficient space for object of type",
             type_mismatch->type->name);
  }
  __ubsan_abort(loc, description);
  unreachable;
}

void ___ubsan_handle_type_mismatch_v1(
    struct UbsanTypeMismatchInfoClang *type_mismatch, uintptr_t pointer) {
  struct UbsanTypeMismatchInfo mm;
  mm.location = type_mismatch->location;
  mm.type = type_mismatch->type;
  mm.alignment = 1u << type_mismatch->log_alignment;
  mm.type_check_kind = type_mismatch->type_check_kind;
  __ubsan_handle_type_mismatch(&mm, pointer);
  unreachable;
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
  unreachable;
}
