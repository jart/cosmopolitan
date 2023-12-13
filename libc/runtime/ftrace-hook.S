/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
.privileged

ftrace_hook:
#ifdef __x86_64__

//	We need to save saved registers because we have some functions
//	like __errno_location which can be called from an inline asm()
//	statement. It's nice to have the flexibility anyway.

	cmpl	$0,__ftrace(%rip)
	jle	1f
	push	%rbp
	mov	%rsp,%rbp
	and	$-16,%rsp
	sub	$256,%rsp
	push	%rax
	push	%rbx
	push	%rcx
	push	%rdx
	push	%rdi
	push	%rsi
	push	%r8
	push	%r9
	push	%r10
	push	%r11
	push	%r12
	push	%r13
	push	%r14
	push	%r15
	call	__xmm_save
	call	ftracer
	call	__xmm_load
	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%r11
	pop	%r10
	pop	%r9
	pop	%r8
	pop	%rsi
	pop	%rdi
	pop	%rdx
	pop	%rcx
	pop	%rbx
	pop	%rax
	leave
1:	ret

#elif defined(__aarch64__)

	stp	x29,x30,[sp,-384]!
	mov	x29,sp
	stp	x0,x1,[sp,16]

	adrp	x0,__ftrace
	ldr	w0,[x0,#:lo12:__ftrace]
	cmp	w0,0
	ble	1f

	stp	x2,x3,[sp,32]
	stp	x4,x5,[sp,48]
	stp	x6,x7,[sp,64]
	stp	x8,x9,[sp,80]
	stp	x10,x11,[sp,96]
	stp	x12,x13,[sp,112]
	stp	x14,x15,[sp,128]
	stp	x16,x19,[sp,160]
	stp	x20,x21,[sp,176]
	stp	x22,x23,[sp,192]
	stp	x24,x25,[sp,208]
	stp	x26,x27,[sp,224]
	stp	x17,x28,[sp,240]
	stp	q0,q1,[sp,256]
	stp	q2,q3,[sp,288]
	stp	q4,q5,[sp,320]
	stp	q6,q7,[sp,352]

	bl	ftracer

	ldp	q6,q7,[sp,352]
	ldp	q4,q5,[sp,320]
	ldp	q2,q3,[sp,288]
	ldp	q0,q1,[sp,256]
	ldp	x17,x28,[sp,240]
	ldp	x26,x27,[sp,224]
	ldp	x24,x25,[sp,208]
	ldp	x22,x23,[sp,192]
	ldp	x20,x21,[sp,176]
	ldp	x16,x19,[sp,160]
	ldp	x14,x15,[sp,128]
	ldp	x12,x13,[sp,112]
	ldp	x10,x11,[sp,96]
	ldp	x8,x9,[sp,80]
	ldp	x6,x7,[sp,64]
	ldp	x4,x5,[sp,48]
	ldp	x2,x3,[sp,32]

1:	ldp	x0,x1,[sp,16]
	ldp	x29,x30,[sp],384
	ret

#endif /* __x86_64__ */
	.endfn	ftrace_hook,globl,hidden
