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
#include "libc/calls/calls.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/pushpop.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/log/color.internal.h"
#include "libc/log/internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/mem/reverse.internal.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/nr.h"

#define kUbsanKindInt     0
#define kUbsanKindFloat   1
#define kUbsanKindUnknown 0xffff

typedef void __ubsan_die_f(void);

struct UbsanSourceLocation {
  const char *file;
  uint32_t line;
  uint32_t column;
};

struct UbsanTypeDescriptor {
  uint16_t kind; /* int,float,... */
  uint16_t info; /* if int bit 0 if signed, remaining bits are log2(sizeof*8) */
  char name[];
};

struct UbsanTypeMismatchInfo {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
  uintptr_t alignment;
  uint8_t type_check_kind;
};

struct UbsanTypeMismatchInfoClang {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
  unsigned char log_alignment; /* https://reviews.llvm.org/D28244 */
  uint8_t type_check_kind;
};

struct UbsanOutOfBoundsInfo {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *array_type;
  struct UbsanTypeDescriptor *index_type;
};

struct UbsanUnreachableData {
  struct UbsanSourceLocation location;
};

struct UbsanVlaBoundData {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
};

struct UbsanNonnullArgData {
  struct UbsanSourceLocation location;
  struct UbsanSourceLocation attr_location;
};

struct UbsanCfiBadIcallData {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
};

struct UbsanNonnullReturnData {
  struct UbsanSourceLocation location;
  struct UbsanSourceLocation attr_location;
};

struct UbsanFunctionTypeMismatchData {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
};

struct UbsanInvalidValueData {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
};

struct UbsanOverflowData {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
};

struct UbsanDynamicTypeCacheMissData {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
  void *TypeInfo;
  unsigned char TypeCheckKind;
};

struct UbsanFloatCastOverflowData {
#if __GNUC__ + 0 >= 6
  struct UbsanSourceLocation location;
#endif
  struct UbsanTypeDescriptor *from_type;
  struct UbsanTypeDescriptor *to_type;
};

struct UbsanOutOfBoundsData {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *arraytype;
  struct UbsanTypeDescriptor *index_type;
};

struct UbsanShiftOutOfBoundsInfo {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *lhs_type;
  struct UbsanTypeDescriptor *rhs_type;
};

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

uintptr_t __ubsan_vptr_type_cache[128];

static int __ubsan_bits(struct UbsanTypeDescriptor *t) {
  return 1 << (t->info >> 1);
}

static bool __ubsan_signed(struct UbsanTypeDescriptor *t) {
  return t->info & 1;
}

static bool __ubsan_negative(struct UbsanTypeDescriptor *t, uintptr_t x) {
  return __ubsan_signed(t) && (intptr_t)x < 0;
}

static char *__ubsan_itpcpy(char *p, struct UbsanTypeDescriptor *t,
                            uintptr_t x) {
  if (__ubsan_signed(t)) {
    return __intcpy(p, x);
  } else {
    return __uintcpy(p, x);
  }
}

static size_t __ubsan_strlen(const char *s) {
  size_t i = 0;
  while (s[i]) ++i;
  return i;
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

static wontreturn void __ubsan_unreachable(void) {
  for (;;) abort();
}

static void __ubsan_exit(void) {
  kprintf("your ubsan runtime needs\n"
          "\t__static_yoink(\"__die\");\n"
          "in order to show you backtraces\n");
  _Exit(99);
}

static char *__ubsan_stpcpy(char *d, const char *s) {
  size_t i;
  for (i = 0;; ++i) {
    if (!(d[i] = s[i])) {
      return d + i;
    }
  }
}

__wur static __ubsan_die_f *__ubsan_die(void) {
  if (_weaken(__die)) {
    return _weaken(__die);
  } else {
    return __ubsan_exit;
  }
}

static void __ubsan_warning(const struct UbsanSourceLocation *loc,
                            const char *description) {
  kprintf("%s:%d: %subsan warning: %s is undefined behavior%s\n", loc->file,
          loc->line, SUBTLE, description, RESET);
}

__wur __ubsan_die_f *__ubsan_abort(const struct UbsanSourceLocation *loc,
                                   const char *description) {
  kprintf("\n%s:%d: %subsan error%s: %s (tid %d)\n", loc->file, loc->line, RED2,
          RESET, description, gettid());
  return __ubsan_die();
}

static const char *__ubsan_describe_shift(
    struct UbsanShiftOutOfBoundsInfo *info, uintptr_t lhs, uintptr_t rhs) {
  if (__ubsan_negative(info->rhs_type, rhs)) {
    return "negative shift exponent";
  } else if (rhs >= __ubsan_bits(info->lhs_type)) {
    return "shift exponent too large for type";
  } else if (__ubsan_negative(info->lhs_type, lhs)) {
    return "left shift of negative value";
  } else if (__ubsan_signed(info->lhs_type)) {
    return "signed left shift changed sign bit or overflowed";
  } else {
    return "wut shift out of bounds";
  }
}

static char *__ubsan_describe_shift_out_of_bounds(
    char buf[512], struct UbsanShiftOutOfBoundsInfo *info, uintptr_t lhs,
    uintptr_t rhs) {
  char *p = buf;
  lhs = __ubsan_extend(info->lhs_type, lhs);
  rhs = __ubsan_extend(info->rhs_type, rhs);
  p = __ubsan_stpcpy(p, __ubsan_describe_shift(info, lhs, rhs)), *p++ = ' ';
  p = __ubsan_itpcpy(p, info->lhs_type, lhs), *p++ = ' ';
  p = __ubsan_stpcpy(p, info->lhs_type->name), *p++ = ' ';
  p = __ubsan_itpcpy(p, info->rhs_type, rhs), *p++ = ' ';
  p = __ubsan_stpcpy(p, info->rhs_type->name);
  return buf;
}

void __ubsan_handle_shift_out_of_bounds(struct UbsanShiftOutOfBoundsInfo *info,
                                        uintptr_t lhs, uintptr_t rhs) {
  char buf[512];
  __ubsan_warning(&info->location,
                  __ubsan_describe_shift_out_of_bounds(buf, info, lhs, rhs));
}

void __ubsan_handle_shift_out_of_bounds_abort(
    struct UbsanShiftOutOfBoundsInfo *info, uintptr_t lhs, uintptr_t rhs) {
  char buf[512];
  __ubsan_abort(&info->location,
                __ubsan_describe_shift_out_of_bounds(buf, info, lhs, rhs))();
  __ubsan_unreachable();
}

void __ubsan_handle_out_of_bounds(struct UbsanOutOfBoundsInfo *info,
                                  uintptr_t index) {
  char buf[512], *p = buf;
  p = __ubsan_stpcpy(p, info->index_type->name);
  p = __ubsan_stpcpy(p, " index ");
  p = __ubsan_itpcpy(p, info->index_type, index);
  p = __ubsan_stpcpy(p, " into ");
  p = __ubsan_stpcpy(p, info->array_type->name);
  p = __ubsan_stpcpy(p, " out of bounds");
  __ubsan_abort(&info->location, buf)();
  __ubsan_unreachable();
}

void __ubsan_handle_out_of_bounds_abort(struct UbsanOutOfBoundsInfo *info,
                                        uintptr_t index) {
  __ubsan_handle_out_of_bounds(info, index);
}

static __ubsan_die_f *__ubsan_type_mismatch_handler(
    struct UbsanTypeMismatchInfo *info, uintptr_t pointer) {
  const char *kind;
  char buf[512], *p = buf;
  if (!pointer) return __ubsan_abort(&info->location, "null pointer access");
  kind = __ubsan_dubnul(kUbsanTypeCheckKinds, info->type_check_kind);
  if (info->alignment && (pointer & (info->alignment - 1))) {
    p = __ubsan_stpcpy(p, "unaligned ");
    p = __ubsan_stpcpy(p, kind), *p++ = ' ';
    p = __ubsan_stpcpy(p, info->type->name), *p++ = ' ', *p++ = '@';
    p = __ubsan_itpcpy(p, info->type, pointer);
    p = __ubsan_stpcpy(p, " align ");
    p = __intcpy(p, info->alignment);
  } else {
    p = __ubsan_stpcpy(p, "insufficient size ");
    p = __ubsan_stpcpy(p, kind);
    p = __ubsan_stpcpy(p, " address 0x");
    p = __fixcpy(p, pointer, sizeof(pointer) * CHAR_BIT);
    p = __ubsan_stpcpy(p, " with insufficient space for object of type ");
    p = __ubsan_stpcpy(p, info->type->name);
  }
  return __ubsan_abort(&info->location, buf);
}

void __ubsan_handle_type_mismatch(struct UbsanTypeMismatchInfo *info,
                                  uintptr_t pointer) {
  __ubsan_type_mismatch_handler(info, pointer)();
  __ubsan_unreachable();
}

void __ubsan_handle_type_mismatch_abort(struct UbsanTypeMismatchInfo *info,
                                        uintptr_t pointer) {
  __ubsan_type_mismatch_handler(info, pointer)();
  __ubsan_unreachable();
}

static __ubsan_die_f *__ubsan_type_mismatch_v1_handler(
    struct UbsanTypeMismatchInfoClang *type_mismatch, uintptr_t pointer) {
  struct UbsanTypeMismatchInfo mm;
  mm.location = type_mismatch->location;
  mm.type = type_mismatch->type;
  mm.alignment = 1u << type_mismatch->log_alignment;
  mm.type_check_kind = type_mismatch->type_check_kind;
  return __ubsan_type_mismatch_handler(&mm, pointer);
}

void __ubsan_handle_type_mismatch_v1(
    struct UbsanTypeMismatchInfoClang *type_mismatch, uintptr_t pointer) {
  __ubsan_type_mismatch_v1_handler(type_mismatch, pointer)();
  __ubsan_unreachable();
}

void __ubsan_handle_type_mismatch_v1_abort(
    struct UbsanTypeMismatchInfoClang *type_mismatch, uintptr_t pointer) {
  __ubsan_type_mismatch_v1_handler(type_mismatch, pointer)();
  __ubsan_unreachable();
}

void __ubsan_handle_float_cast_overflow(void *data_raw, void *from_raw) {
  struct UbsanFloatCastOverflowData *data =
      (struct UbsanFloatCastOverflowData *)data_raw;
#if __GNUC__ + 0 >= 6
  __ubsan_abort(&data->location, "float cast overflow")();
  __ubsan_unreachable();
#else
  const struct UbsanSourceLocation kUnknownLocation = {
      "<unknown file>",
      pushpop(0),
      pushpop(0),
  };
  __ubsan_abort(((void)data, &kUnknownLocation), "float cast overflow")();
  __ubsan_unreachable();
#endif
}

void __ubsan_handle_float_cast_overflow_abort(void *data_raw, void *from_raw) {
  __ubsan_handle_float_cast_overflow(data_raw, from_raw);
}

void __ubsan_handle_add_overflow(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "add overflow")();
  __ubsan_unreachable();
}

void __ubsan_handle_add_overflow_abort(const struct UbsanSourceLocation *loc) {
  __ubsan_handle_add_overflow(loc);
}

void __ubsan_handle_alignment_assumption(
    const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "alignment assumption")();
  __ubsan_unreachable();
}

void __ubsan_handle_alignment_assumption_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_alignment_assumption(loc);
}

void __ubsan_handle_builtin_unreachable(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "builtin unreachable")();
  __ubsan_unreachable();
}

void __ubsan_handle_builtin_unreachable_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_builtin_unreachable(loc);
}

void __ubsan_handle_cfi_bad_type(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "cfi bad type")();
  __ubsan_unreachable();
}

void __ubsan_handle_cfi_bad_type_abort(const struct UbsanSourceLocation *loc) {
  __ubsan_handle_cfi_bad_type(loc);
}

void __ubsan_handle_cfi_check_fail(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "cfi check fail")();
  __ubsan_unreachable();
}

void __ubsan_handle_cfi_check_fail_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_cfi_check_fail(loc);
}

void __ubsan_handle_divrem_overflow(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "divrem overflow")();
  __ubsan_unreachable();
}

void __ubsan_handle_divrem_overflow_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_divrem_overflow(loc);
}

static bool HandleDynamicTypeCacheMiss(
    struct UbsanDynamicTypeCacheMissData *data, uintptr_t ptr, uintptr_t hash) {
  return false;  // TODO: implement me
}

void __ubsan_handle_dynamic_type_cache_miss(
    struct UbsanDynamicTypeCacheMissData *data, uintptr_t ptr, uintptr_t hash) {
  HandleDynamicTypeCacheMiss(data, ptr, hash);
}

void __ubsan_handle_dynamic_type_cache_miss_abort(
    struct UbsanDynamicTypeCacheMissData *data, uintptr_t ptr, uintptr_t hash) {
  if (HandleDynamicTypeCacheMiss(data, ptr, hash)) {
    __ubsan_abort(&data->location, "dynamic type cache miss")();
    __ubsan_unreachable();
  }
}

void __ubsan_handle_function_type_mismatch(
    const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "function type mismatch")();
  __ubsan_unreachable();
}

void __ubsan_handle_function_type_mismatch_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_function_type_mismatch(loc);
}

void __ubsan_handle_implicit_conversion(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "implicit conversion")();
  __ubsan_unreachable();
}

void __ubsan_handle_implicit_conversion_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_implicit_conversion(loc);
}

void __ubsan_handle_invalid_builtin(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "invalid builtin")();
  __ubsan_unreachable();
}

void __ubsan_handle_invalid_builtin_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_invalid_builtin(loc);
}

void __ubsan_handle_load_invalid_value(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "load invalid value (uninitialized? bool∌[01]?)")();
  __ubsan_unreachable();
}

void __ubsan_handle_load_invalid_value_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_load_invalid_value(loc);
}

void __ubsan_handle_missing_return(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "missing return")();
  __ubsan_unreachable();
}

void __ubsan_handle_missing_return_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_missing_return(loc);
}

void __ubsan_handle_mul_overflow(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "multiply overflow")();
  __ubsan_unreachable();
}

void __ubsan_handle_mul_overflow_abort(const struct UbsanSourceLocation *loc) {
  __ubsan_handle_mul_overflow(loc);
}

void __ubsan_handle_negate_overflow(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "negate overflow")();
  __ubsan_unreachable();
}

void __ubsan_handle_negate_overflow_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_negate_overflow(loc);
}

void __ubsan_handle_nonnull_arg(const struct UbsanSourceLocation *loc) {
  __ubsan_warning(loc, "null argument here");
}

void __ubsan_handle_nonnull_arg_abort(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "nonnull argument")();
  __ubsan_unreachable();
}

void __ubsan_handle_nonnull_return_v1(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "non-null return (v1)")();
  __ubsan_unreachable();
}

void __ubsan_handle_nonnull_return_v1_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "non-null return (v1)")();
  __ubsan_unreachable();
}

void __ubsan_handle_nullability_arg(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "nullability arg")();
  __ubsan_unreachable();
}

void __ubsan_handle_nullability_arg_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_nullability_arg(loc);
}

void __ubsan_handle_nullability_return_v1(
    const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "nullability return (v1)")();
  __ubsan_unreachable();
}

void __ubsan_handle_nullability_return_v1_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_nullability_return_v1(loc);
}

void __ubsan_handle_pointer_overflow(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "pointer overflow")();
  __ubsan_unreachable();
}

void __ubsan_handle_pointer_overflow_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_pointer_overflow(loc);
}

void __ubsan_handle_sub_overflow(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "subtract overflow")();
  __ubsan_unreachable();
}

void __ubsan_handle_sub_overflow_abort(const struct UbsanSourceLocation *loc) {
  __ubsan_handle_sub_overflow(loc);
}

void __ubsan_handle_vla_bound_not_positive(
    const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "vla bound not positive")();
  __ubsan_unreachable();
}

void __ubsan_handle_vla_bound_not_positive_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_vla_bound_not_positive(loc);
}

void __ubsan_handle_nonnull_return(const struct UbsanSourceLocation *loc) {
  __ubsan_abort(loc, "nonnull return")();
  __ubsan_unreachable();
}

void __ubsan_handle_nonnull_return_abort(
    const struct UbsanSourceLocation *loc) {
  __ubsan_handle_nonnull_return(loc);
}

void __ubsan_default_options(void) {
}

void __ubsan_on_report(void) {
}

void *__ubsan_get_current_report_data(void) {
  return 0;
}

static textstartup void ubsan_init() {
  STRACE(" _   _ ____ ____    _    _   _");
  STRACE("| | | | __ ) ___|  / \\  | \\ | |");
  STRACE("| | | |  _ \\___ \\ / _ \\ |  \\| |");
  STRACE("| |_| | |_) |__) / ___ \\| |\\  |");
  STRACE(" \\___/|____/____/_/   \\_\\_| \\_|");
  STRACE("cosmopolitan behavior module initialized");
}

const void *const ubsan_ctor[] initarray = {
    ubsan_init,
};
