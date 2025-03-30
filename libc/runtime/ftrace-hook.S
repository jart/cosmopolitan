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
#include "libc/macros.h"
.privileged

ftrace_hook:
#ifdef __x86_64__

//	save argument registers
//	we save %rax because __gc() takes it as an argument.
//	we save %r10 because it's used as a syscall argument.

	cmpl	$0,__ftrace(%rip)
	jle	1f
	.cfi_startproc
	push	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	mov	%rsp,%rbp
	.cfi_def_cfa_register %rbp
	and	$-16,%rsp
	sub	$128,%rsp
	movdqu	%xmm0,-0x80(%rbp)
	movdqu	%xmm1,-0x70(%rbp)
	movdqu	%xmm2,-0x60(%rbp)
	movdqu	%xmm3,-0x50(%rbp)
	movdqu	%xmm4,-0x40(%rbp)
	movdqu	%xmm5,-0x30(%rbp)
	movdqu	%xmm6,-0x20(%rbp)
	movdqu	%xmm7,-0x10(%rbp)
	push	%rax
	.cfi_offset %rax, -24
	push	%rcx
	.cfi_offset %rcx, -32
	push	%rdx
	.cfi_offset %rdx, -40
	push	%rdi
	.cfi_offset %rdi, -48
	push	%rsi
	.cfi_offset %rsi, -56
	push	%r8
	.cfi_offset %r8, -64
	push	%r9
	.cfi_offset %r9, -72
	push	%r10
	.cfi_offset %r10, -80
	call	ftracer
	movdqu	-0x80(%rbp),%xmm0
	movdqu	-0x70(%rbp),%xmm1
	movdqu	-0x60(%rbp),%xmm2
	movdqu	-0x50(%rbp),%xmm3
	movdqu	-0x40(%rbp),%xmm4
	movdqu	-0x30(%rbp),%xmm5
	movdqu	-0x20(%rbp),%xmm6
	movdqu	-0x10(%rbp),%xmm7
	pop	%r10
	pop	%r9
	pop	%r8
	pop	%rsi
	pop	%rdi
	pop	%rdx
	pop	%rcx
	pop	%rax
	leave
	.cfi_restore %rbp
	.cfi_def_cfa %rsp, 8
1:	ret
	.cfi_endproc

#elif defined(__aarch64__)

	stp	x29,x30,[sp,-384]!
	.cfi_startproc
	.cfi_def_cfa_offset 384
	.cfi_offset 29, -384	// x29 (fp) is saved at [sp - 384]
	.cfi_offset 30, -376	// x30 (lr) is saved at [sp - 376]
	mov	x29,sp
	.cfi_def_cfa_register 29
	stp	x0,x1,[sp,16]

	adrp	x0,__ftrace
	ldr	w0,[x0,#:lo12:__ftrace]
	cmp	w0,0
	ble	1f

	stp	x2,x3,[sp,32]
	.cfi_offset 2, -352
	.cfi_offset 3, -344
	stp	x4,x5,[sp,48]
	.cfi_offset 4, -336
	.cfi_offset 5, -328
	stp	x6,x7,[sp,64]
	.cfi_offset 6, -320
	.cfi_offset 7, -312
	stp	x8,x9,[sp,80]
	.cfi_offset 8, -304
	.cfi_offset 9, -296
	stp	x10,x11,[sp,96]
	.cfi_offset 10, -288
	.cfi_offset 11, -280
	stp	x12,x13,[sp,112]
	.cfi_offset 12, -272
	.cfi_offset 13, -264
	stp	x14,x15,[sp,128]
	.cfi_offset 14, -256
	.cfi_offset 15, -248
	stp	x16,x19,[sp,160]
	.cfi_offset 16, -224
	.cfi_offset 19, -216
	stp	x20,x21,[sp,176]
	.cfi_offset 20, -208
	.cfi_offset 21, -200
	stp	x22,x23,[sp,192]
	.cfi_offset 22, -192
	.cfi_offset 23, -184
	stp	x24,x25,[sp,208]
	.cfi_offset 24, -176
	.cfi_offset 25, -168
	stp	x26,x27,[sp,224]
	.cfi_offset 26, -160
	.cfi_offset 27, -152
	stp	x17,x28,[sp,240]
	.cfi_offset 17, -144
	.cfi_offset 28, -136
	// No CFI directives needed for FP registers
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
	.cfi_restore 29
	.cfi_restore 30
	.cfi_def_cfa 7, 0	// On some ARM systems the stack pointer is represented by register 7
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc

#endif /* __x86_64__ */
	.endfn	ftrace_hook,globl,hidden
