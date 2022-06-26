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
#include "libc/macros.internal.h"

	.rodata.cst4
__asan_option_detect_stack_use_after_return:
	.long	0
	.endobj	__asan_option_detect_stack_use_after_return,globl
	.previous

__asan_report_load1:
	push	$1
	jmp	1f
	.endfn	__asan_report_load1,globl
__asan_report_load2:
	push	$2
	jmp	1f
	.endfn	__asan_report_load2,globl
__asan_report_load4:
	push	$4
	jmp	1f
	.endfn	__asan_report_load4,globl
__asan_report_load8:
	push	$8
	jmp	1f
	.endfn	__asan_report_load8,globl
__asan_report_load16:
	push	$16
	jmp	1f
	.endfn	__asan_report_load16,globl
__asan_report_load32:
	push	$32
1:	pop	%rsi
0:	jmp	__asan_report_load
	.endfn	__asan_report_load32,globl
__asan_report_load_n:
	jmp	0b
	.endfn	__asan_report_load_n,globl

__asan_report_store1:
	push	$1
	jmp	1f
	.endfn	__asan_report_store1,globl
__asan_report_store2:
	push	$2
	jmp	1f
	.endfn	__asan_report_store2,globl
__asan_report_store4:
	push	$4
	jmp	1f
	.endfn	__asan_report_store4,globl
__asan_report_store8:
	push	$8
	jmp	1f
	.endfn	__asan_report_store8,globl
__asan_report_store16:
	push	$16
	jmp	1f
	.endfn	__asan_report_store16,globl
__asan_report_store32:
	push	$32
1:	pop	%rsi
0:	jmp	__asan_report_store
	.endfn	__asan_report_store32,globl
__asan_report_store_n:
	jmp	0b
	.endfn	__asan_report_store_n,globl

__asan_stack_free_0:
	push	$0
	jmp	1f
	.endfn	__asan_stack_free_0,globl
__asan_stack_free_1:
	push	$1
	jmp	1f
	.endfn	__asan_stack_free_1,globl
__asan_stack_free_2:
	push	$2
	jmp	1f
	.endfn	__asan_stack_free_2,globl
__asan_stack_free_3:
	push	$3
	jmp	1f
	.endfn	__asan_stack_free_3,globl
__asan_stack_free_4:
	push	$4
	jmp	1f
	.endfn	__asan_stack_free_4,globl
__asan_stack_free_5:
	push	$5
	.endfn	__asan_stack_free_5,globl
__asan_stack_free_hop:
1:	pop	%rdx
	jmp	__asan_stack_free
	.endfn	__asan_report_store_n,globl
__asan_stack_free_6:
	push	$6
	jmp	1b
	.endfn	__asan_stack_free_6,globl
__asan_stack_free_7:
	push	$7
	jmp	1b
	.endfn	__asan_stack_free_7,globl
__asan_stack_free_8:
	push	$8
	jmp	1b
	.endfn	__asan_stack_free_8,globl
__asan_stack_free_9:
	push	$9
	jmp	1b
	.endfn	__asan_stack_free_9,globl
__asan_stack_free_10:
	push	$10
	jmp	1b
	.endfn	__asan_stack_free_10,globl

__asan_stack_malloc_0:
	push	$0
	jmp	1f
	.endfn	__asan_stack_malloc_0,globl
__asan_stack_malloc_1:
	push	$1
	jmp	1f
	.endfn	__asan_stack_malloc_1,globl
__asan_stack_malloc_2:
	push	$2
	jmp	1f
	.endfn	__asan_stack_malloc_2,globl
__asan_stack_malloc_3:
	push	$3
	jmp	1f
	.endfn	__asan_stack_malloc_3,globl
__asan_stack_malloc_4:
	push	$4
	jmp	1f
	.endfn	__asan_stack_malloc_4,globl
__asan_stack_malloc_5:
	push	$5
	jmp	1f
	.endfn	__asan_stack_malloc_5,globl
__asan_stack_malloc_hop:
1:	pop	%rsi
	jmp	__asan_stack_malloc
	.endfn	__asan_report_store_n,globl
__asan_stack_malloc_6:
	push	$6
	jmp	1b
	.endfn	__asan_stack_malloc_6,globl
__asan_stack_malloc_7:
	push	$7
	jmp	1b
	.endfn	__asan_stack_malloc_7,globl
__asan_stack_malloc_8:
	push	$8
	jmp	1b
	.endfn	__asan_stack_malloc_8,globl
__asan_stack_malloc_9:
	push	$9
	jmp	1b
	.endfn	__asan_stack_malloc_9,globl
__asan_stack_malloc_10:
	push	$10
	jmp	1b
	.endfn	__asan_stack_malloc_10,globl

__asan_version_mismatch_check_v8:
	ret
	.endfn	__asan_version_mismatch_check_v8,globl

//	Initializes Address Sanitizer runtime earlier if linked.
	.init.start 303,_init_asan
	push	%rdi
	push	%rsi
	mov	%r12,%rdi
	mov	%r13,%rsi
	mov	%r14,%rdx
	mov	%r15,%rcx
	call	__asan_init
	pop	%rsi
	pop	%rdi
	.init.end 303,_init_asan

__asan_load1:
	push	%rbp
	mov	%rsp,%rbp
	ud2
	.endfn	__asan_load1,globl

__asan_load2:
	push	%rbp
	mov	%rsp,%rbp
	ud2
	.endfn	__asan_load2,globl

__asan_load4:
	push	%rbp
	mov	%rsp,%rbp
	ud2
	.endfn	__asan_load4,globl

__asan_load8:
	push	%rbp
	mov	%rsp,%rbp
	ud2
	.endfn	__asan_load8,globl

__asan_load16:
	push	%rbp
	mov	%rsp,%rbp
	ud2
	.endfn	__asan_load16,globl

__asan_load32:
	push	%rbp
	mov	%rsp,%rbp
	ud2
	.endfn	__asan_load32,globl

__asan_store1:
	push	%rbp
	mov	%rsp,%rbp
	ud2
	.endfn	__asan_store1,globl

__asan_store2:
	push	%rbp
	mov	%rsp,%rbp
	ud2
	.endfn	__asan_store2,globl

__asan_store4:
	push	%rbp
	mov	%rsp,%rbp
	ud2
	.endfn	__asan_store4,globl

__asan_store8:
	push	%rbp
	mov	%rsp,%rbp
	ud2
	.endfn	__asan_store8,globl

__asan_store16:
	push	%rbp
	mov	%rsp,%rbp
	ud2
	.endfn	__asan_store16,globl

__asan_store32:
	push	%rbp
	mov	%rsp,%rbp
	ud2
	.endfn	__asan_store32,globl
