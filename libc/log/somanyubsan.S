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
#include "libc/macros.h"
.text.unlikely
.source	__FILE__

__ubsan_default_options:
	ret
	.endfn	__ubsan_default_options

__ubsan_on_report:
	ret
	.endfn	__ubsan_on_report,globl

__ubsan_get_current_report_data:
	xor	%eax,%eax
	ret
	.endfn	__ubsan_get_current_report_data,globl

__ubsan_handle_type_mismatch_abort:
	jmp	__ubsan_handle_type_mismatch
	.endfn	__ubsan_handle_type_mismatch_abort,globl

__ubsan_handle_float_cast_overflow_abort:
	jmp	__ubsan_handle_float_cast_overflow
	.endfn	__ubsan_handle_float_cast_overflow_abort,globl

__ubsan_handle_type_mismatch_v1:
__ubsan_handle_type_mismatch_v1_abort:
	jmp	___ubsan_handle_type_mismatch_v1
	.endfn	__ubsan_handle_type_mismatch_v1,globl
	.endfn	__ubsan_handle_type_mismatch_v1_abort,globl

__ubsan_handle_add_overflow_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_add_overflow_abort,globl
__ubsan_handle_add_overflow:
	loadstr	"add_overflow",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_add_overflow,globl

__ubsan_handle_alignment_assumption_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_alignment_assumption_abort,globl
__ubsan_handle_alignment_assumption:
	loadstr	"alignment_assumption",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_alignment_assumption,globl

__ubsan_handle_builtin_unreachable_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_builtin_unreachable_abort,globl
__ubsan_handle_builtin_unreachable:
	loadstr	"builtin_unreachable",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_builtin_unreachable,globl

__ubsan_handle_cfi_bad_type_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_cfi_bad_type_abort,globl
__ubsan_handle_cfi_bad_type:
	loadstr	"cfi_bad_type",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_cfi_bad_type,globl

__ubsan_handle_cfi_check_fail_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_cfi_check_fail_abort,globl
__ubsan_handle_cfi_check_fail:
	loadstr	"cfi_check_fail",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_cfi_check_fail,globl

__ubsan_handle_divrem_overflow_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_divrem_overflow_abort,globl
__ubsan_handle_divrem_overflow:
	loadstr	"divrem_overflow",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_divrem_overflow,globl

__ubsan_handle_dynamic_type_cache_miss_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_dynamic_type_cache_miss_abort,globl
__ubsan_handle_dynamic_type_cache_miss:
	loadstr	"dynamic_type_cache_miss",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_dynamic_type_cache_miss,globl

__ubsan_handle_function_type_mismatch_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_function_type_mismatch_abort,globl
__ubsan_handle_function_type_mismatch:
	loadstr	"function_type_mismatch",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_function_type_mismatch,globl

__ubsan_handle_implicit_conversion_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_implicit_conversion_abort,globl
__ubsan_handle_implicit_conversion:
	loadstr	"implicit_conversion",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_implicit_conversion,globl

__ubsan_handle_invalid_builtin_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_invalid_builtin_abort,globl
__ubsan_handle_invalid_builtin:
	loadstr	"invalid_builtin",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_invalid_builtin,globl

__ubsan_handle_load_invalid_value_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_load_invalid_value_abort,globl
__ubsan_handle_load_invalid_value:
	loadstr	"load_invalid_value (try checking for uninitialized variables)",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_load_invalid_value,globl

__ubsan_handle_missing_return_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_missing_return_abort,globl
__ubsan_handle_missing_return:
	loadstr	"missing_return",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_missing_return,globl

__ubsan_handle_mul_overflow_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_mul_overflow_abort,globl
__ubsan_handle_mul_overflow:
	loadstr	"mul_overflow",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_mul_overflow,globl

__ubsan_handle_negate_overflow_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_negate_overflow_abort,globl
__ubsan_handle_negate_overflow:
	loadstr	"negate_overflow",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_negate_overflow,globl

__ubsan_handle_nonnull_arg_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nonnull_arg_abort,globl
__ubsan_handle_nonnull_arg:
	loadstr	"nonnull_arg",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_nonnull_arg,globl

__ubsan_handle_nonnull_return_v1_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nonnull_return_v1_abort,globl
__ubsan_handle_nonnull_return_v1:
	loadstr	"nonnull_return_v1",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_nonnull_return_v1,globl

__ubsan_hop:
	jmp	__ubsan_abort
	.endfn	__ubsan_hop

__ubsan_handle_nullability_arg_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nullability_arg_abort,globl
__ubsan_handle_nullability_arg:
	loadstr	"nullability_arg",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_nullability_arg,globl

__ubsan_handle_nullability_return_v1_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nullability_return_v1_abort,globl
__ubsan_handle_nullability_return_v1:
	loadstr	"nullability_return_v1",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_nullability_return_v1,globl

__ubsan_handle_pointer_overflow_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_pointer_overflow_abort,globl
__ubsan_handle_pointer_overflow:
	loadstr	"pointer_overflow",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_pointer_overflow,globl

__ubsan_handle_shift_out_of_bounds_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_shift_out_of_bounds_abort,globl
__ubsan_handle_shift_out_of_bounds:
	loadstr	"shift_out_of_bounds",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_shift_out_of_bounds,globl

__ubsan_handle_sub_overflow_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_sub_overflow_abort,globl
__ubsan_handle_sub_overflow:
	loadstr	"sub_overflow",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_sub_overflow,globl

__ubsan_handle_vla_bound_not_positive_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_vla_bound_not_positive_abort,globl
__ubsan_handle_vla_bound_not_positive:
	loadstr	"vla_bound_not_positive",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_vla_bound_not_positive,globl

__ubsan_handle_nonnull_return_abort:
	nop
/	fallthrough
	.endfn	__ubsan_handle_nonnull_return_abort,globl
__ubsan_handle_nonnull_return:
	loadstr	"nonnull_return",si
	jmp	__ubsan_hop
	.endfn	__ubsan_handle_nonnull_return,globl

__ubsan_handle_out_of_bounds_abort:
	jmp	__ubsan_handle_out_of_bounds
	.endfn	__ubsan_handle_out_of_bounds_abort,globl

.previous
