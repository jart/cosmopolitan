/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
#include "libc/macros.h"
.privileged
.yoink	__FILE__

__asan_load1:
	push	$1
	jmp	OnLoad
	.endfn	__asan_load1,globl
__asan_load2:
	push	$2
	jmp	OnLoad
	.endfn	__asan_load2,globl
__asan_load4:
	push	$4
	jmp	OnLoad
	.endfn	__asan_load4,globl
__asan_load8:
	push	$8
	jmp	OnLoad
	.endfn	__asan_load8,globl
__asan_load16:
	push	$16
	jmp	OnLoad
	.endfn	__asan_load16,globl
__asan_load32:
	push	$32
/	fallthrough
	.endfn	__asan_load32,globl
OnLoad:	pop	%rsi
	.globl	__asan_loadN
	.weak	__asan_loadN
	ezlea	__asan_loadN,ax
	jmp	OnAsan
	.endfn	OnStore

__asan_store1:
	push	$1
	jmp	OnStore
	.endfn	__asan_store1,globl
__asan_store2:
	push	$2
	jmp	OnStore
	.endfn	__asan_store2,globl
__asan_store4:
	push	$4
	jmp	OnStore
	.endfn	__asan_store4,globl
__asan_store8:
	push	$8
	jmp	OnStore
	.endfn	__asan_store8,globl
__asan_store16:
	push	$16
	jmp	OnStore
	.endfn	__asan_store16,globl
__asan_store32:
	push	$32
/	fallthrough
	.endfn	__asan_store32,globl
OnStore:pop	%rsi
	.globl	__asan_storeN
	.weak	__asan_storeN
	ezlea	__asan_storeN,ax
	jmp	OnAsan
	.endfn	OnStore

__asan_report_load1:
	push	$1
	jmp	OnReportLoad
	.endfn	__asan_report_load1,globl
__asan_report_load2:
	push	$2
	jmp	OnReportLoad
	.endfn	__asan_report_load2,globl
__asan_report_load4:
	push	$4
	jmp	OnReportLoad
	.endfn	__asan_report_load4,globl
__asan_report_load8:
	push	$8
	jmp	OnReportLoad
	.endfn	__asan_report_load8,globl
__asan_report_load16:
	push	$16
/	fallthrough
	.endfn	__asan_report_load16,globl
OnReportLoad:
	pop	%rsi
	.globl	__asan_report_load_n
	.weak	__asan_report_load_n
	ezlea	__asan_report_load_n,ax
	jmp	OnAsan
	.endfn	OnReportLoad

__asan_report_store1:
	push	$1
	jmp	ReportStore
	.endfn	__asan_report_store1,globl
__asan_report_store2:
	push	$2
	jmp	ReportStore
	.endfn	__asan_report_store2,globl
__asan_report_store4:
	push	$4
	jmp	ReportStore
	.endfn	__asan_report_store4,globl
__asan_report_store8:
	push	$8
	jmp	ReportStore
	.endfn	__asan_report_store8,globl
__asan_report_store16:
	push	$16
	jmp	ReportStore
	.endfn	__asan_report_store16,globl
__asan_report_store32:
	push	$32
/	fallthrough
	.endfn	__asan_report_store32,globl
ReportStore:
	pop	%rsi
	.globl	__asan_report_store_n
	.weak	__asan_report_store_n
	ezlea	__asan_report_store_n,ax
/	fallthrough
	.endfn	ReportStore

OnAsan:	test	%rax,%rax
	jz	1f
	jmp	*%rax
1:	ret
	.endfn	OnAsan

__asan_stack_free_0:
	push	$0
/	fallthrough
	.endfn	__asan_stack_free_0,globl
OnStackFree:
	pop	%rdx
	.globl	__asan_stack_free
	.weak	__asan_stack_free
	ezlea	__asan_stack_free,ax
	jmp	OnAsan
	.endfn	OnStackFree
__asan_stack_free_1:
	push	$1
	jmp	OnStackFree
	.endfn	__asan_stack_free_1,globl
__asan_stack_free_2:
	push	$2
	jmp	OnStackFree
	.endfn	__asan_stack_free_2,globl
__asan_stack_free_3:
	push	$3
	jmp	OnStackFree
	.endfn	__asan_stack_free_3,globl
__asan_stack_free_4:
	push	$4
	jmp	OnStackFree
	.endfn	__asan_stack_free_4,globl
__asan_stack_free_5:
	push	$5
	jmp	OnStackFree
	.endfn	__asan_stack_free_5,globl
__asan_stack_free_6:
	push	$6
	jmp	OnStackFree
	.endfn	__asan_stack_free_6,globl
__asan_stack_free_7:
	push	$7
	jmp	OnStackFree
	.endfn	__asan_stack_free_7,globl
__asan_stack_free_8:
	push	$8
	jmp	OnStackFree
	.endfn	__asan_stack_free_8,globl
__asan_stack_free_9:
	push	$9
	jmp	OnStackFree
	.endfn	__asan_stack_free_9,globl
__asan_stack_free_10:
	push	$10
	jmp	OnStackFree
	.endfn	__asan_stack_free_10,globl

__asan_stack_malloc_0:
	push	$0
/	fallthrough
	.endfn	__asan_stack_malloc_0,globl
OnStackMalloc:
	pop	%rsi
	.globl	__asan_stack_malloc
	.weak	__asan_stack_malloc
	ezlea	__asan_stack_malloc,ax
	jmp	OnAsan
	.endfn	OnStackMalloc
__asan_stack_malloc_1:
	push	$1
	jmp	OnStackMalloc
	.endfn	__asan_stack_malloc_1,globl
__asan_stack_malloc_2:
	push	$2
	jmp	OnStackMalloc
	.endfn	__asan_stack_malloc_2,globl
__asan_stack_malloc_3:
	push	$3
	jmp	OnStackMalloc
	.endfn	__asan_stack_malloc_3,globl
__asan_stack_malloc_4:
	push	$4
	jmp	OnStackMalloc
	.endfn	__asan_stack_malloc_4,globl
__asan_stack_malloc_5:
	push	$5
	jmp	OnStackMalloc
	.endfn	__asan_stack_malloc_5,globl
__asan_stack_malloc_6:
	push	$6
	jmp	OnStackMalloc
	.endfn	__asan_stack_malloc_6,globl
__asan_stack_malloc_7:
	push	$7
	jmp	OnStackMalloc
	.endfn	__asan_stack_malloc_7,globl
__asan_stack_malloc_8:
	push	$8
	jmp	OnStackMalloc
	.endfn	__asan_stack_malloc_8,globl
__asan_stack_malloc_9:
	push	$9
	jmp	OnStackMalloc
	.endfn	__asan_stack_malloc_9,globl
__asan_stack_malloc_10:
	push	$10
	jmp	OnStackMalloc
	.endfn	__asan_stack_malloc_10,globl

	.rodata.cst4
__asan_option_detect_stack_use_after_return:
	.long	1
	.endobj	__asan_option_detect_stack_use_after_return,globl
	.previous
