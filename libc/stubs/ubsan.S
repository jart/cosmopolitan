/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
#include "ape/macros.internal.h"
.real
.code16 # ∩ .code32 ∩ .code64
.source	__FILE__

/**
 * @fileoverview Overridable stubs for synthetic sanitizer calls.
 */

__ubsan_default_options:
	ret
	.endfn	__ubsan_default_options,weak

__ubsan_get_current_report_data:
	ret
	.endfn	__ubsan_get_current_report_data,weak

__ubsan_handle_add_overflow:
	nop
/	fallthrough
	.endfn	__ubsan_handle_add_overflow,weak

__ubsan_handle_add_overflow_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_add_overflow_abort,weak

__ubsan_handle_alignment_assumption:
	nop
/	fallthrough
	.endfn	__ubsan_handle_alignment_assumption,weak

__ubsan_handle_alignment_assumption_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_alignment_assumption_abort,weak

__ubsan_handle_builtin_unreachable:
	nop
/	fallthrough
	.endfn	__ubsan_handle_builtin_unreachable,weak

__ubsan_handle_builtin_unreachable_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_builtin_unreachable_abort,weak

__ubsan_handle_cfi_bad_type:
	nop
/	fallthrough
	.endfn	__ubsan_handle_cfi_bad_type,weak

__ubsan_handle_cfi_bad_type_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_cfi_bad_type_abort,weak

__ubsan_handle_cfi_check_fail:
	nop
/	fallthrough
	.endfn	__ubsan_handle_cfi_check_fail,weak

__ubsan_handle_cfi_check_fail_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_cfi_check_fail_abort,weak

__ubsan_handle_divrem_overflow:
	nop
/	fallthrough
	.endfn	__ubsan_handle_divrem_overflow,weak

__ubsan_handle_divrem_overflow_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_divrem_overflow_abort,weak

__ubsan_handle_dynamic_type_cache_miss:
	nop
/	fallthrough
	.endfn	__ubsan_handle_dynamic_type_cache_miss,weak

__ubsan_handle_dynamic_type_cache_miss_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_dynamic_type_cache_miss_abort,weak

__ubsan_handle_float_cast_overflow:
	nop
/	fallthrough
	.endfn	__ubsan_handle_float_cast_overflow,weak

__ubsan_handle_float_cast_overflow_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_float_cast_overflow_abort,weak

__ubsan_handle_function_type_mismatch:
	nop
/	fallthrough
	.endfn	__ubsan_handle_function_type_mismatch,weak

__ubsan_handle_function_type_mismatch_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_function_type_mismatch_abort,weak

__ubsan_handle_implicit_conversion:
	nop
/	fallthrough
	.endfn	__ubsan_handle_implicit_conversion,weak

__ubsan_handle_implicit_conversion_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_implicit_conversion_abort,weak

__ubsan_handle_invalid_builtin:
	nop
/	fallthrough
	.endfn	__ubsan_handle_invalid_builtin,weak

__ubsan_handle_invalid_builtin_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_invalid_builtin_abort,weak

__ubsan_handle_load_invalid_value:
	nop
/	fallthrough
	.endfn	__ubsan_handle_load_invalid_value,weak

__ubsan_handle_load_invalid_value_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_load_invalid_value_abort,weak

__ubsan_handle_missing_return:
	nop
/	fallthrough
	.endfn	__ubsan_handle_missing_return,weak

__ubsan_handle_missing_return_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_missing_return_abort,weak

__ubsan_handle_mul_overflow:
	nop
/	fallthrough
	.endfn	__ubsan_handle_mul_overflow,weak

__ubsan_handle_mul_overflow_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_mul_overflow_abort,weak

__ubsan_handle_negate_overflow:
	nop
/	fallthrough
	.endfn	__ubsan_handle_negate_overflow,weak

__ubsan_handle_negate_overflow_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_negate_overflow_abort,weak

__ubsan_handle_nonnull_arg:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nonnull_arg,weak

__ubsan_handle_nonnull_arg_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nonnull_arg_abort,weak

__ubsan_handle_nonnull_return:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nonnull_return,weak

__ubsan_handle_nonnull_return_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nonnull_return_abort,weak

__ubsan_handle_nonnull_return_v1:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nonnull_return_v1,weak

__ubsan_handle_nonnull_return_v1_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nonnull_return_v1_abort,weak

__ubsan_handle_nullability_arg:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nullability_arg,weak

__ubsan_handle_nullability_arg_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nullability_arg_abort,weak

__ubsan_handle_nullability_return_v1:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nullability_return_v1,weak

__ubsan_handle_nullability_return_v1_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nullability_return_v1_abort,weak

__ubsan_handle_out_of_bounds:
	nop
/	fallthrough
	.endfn	__ubsan_handle_out_of_bounds,weak

__ubsan_handle_out_of_bounds_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_out_of_bounds_abort,weak

__ubsan_handle_pointer_overflow:
	nop
/	fallthrough
	.endfn	__ubsan_handle_pointer_overflow,weak

__ubsan_handle_pointer_overflow_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_pointer_overflow_abort,weak

__ubsan_handle_shift_out_of_bounds:
	nop
/	fallthrough
	.endfn	__ubsan_handle_shift_out_of_bounds,weak

__ubsan_handle_shift_out_of_bounds_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_shift_out_of_bounds_abort,weak

__ubsan_handle_sub_overflow:
	nop
/	fallthrough
	.endfn	__ubsan_handle_sub_overflow,weak

__ubsan_handle_sub_overflow_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_sub_overflow_abort,weak

__ubsan_handle_type_mismatch:
	nop
/	fallthrough
	.endfn	__ubsan_handle_type_mismatch,weak

__ubsan_handle_type_mismatch_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_type_mismatch_abort,weak

__ubsan_handle_type_mismatch_v1:
	nop
/	fallthrough
	.endfn	__ubsan_handle_type_mismatch_v1,weak

__ubsan_handle_type_mismatch_v1_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_type_mismatch_v1_abort,weak

__ubsan_handle_vla_bound_not_positive:
	nop
/	fallthrough
	.endfn	__ubsan_handle_vla_bound_not_positive,weak

__ubsan_handle_vla_bound_not_positive_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_vla_bound_not_positive_abort,weak

__ubsan_abort_stub:
	push	%bp
	mov	%sp,%bp
1:	int3
	jmp	1b
	.endfn	__ubsan_abort_stub
