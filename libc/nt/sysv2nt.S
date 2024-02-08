/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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
#ifdef __x86_64__
.section .text.windows,"ax",@progbits

//	Epilogues for calling functions w/ Microsoft x64 convention.
//
//	@param	%rax is address of windows x64 function to call
//	@param	%rsi is ignored
//	@param	%rdx,%rcx,%r8,%r9,stack are params (unlimited)
//	@param	%xmm0,%xmm1,%xmm2 are double params (limited to 3)
//	@return	is in %rax, %xmm0, or %st
//	@note	GCC 4.8+ and Clang can avoid this indirection
//	@note	thunk that jumps here must setup frame
//	@note	this is so much faster than __nt2sysv()
__sysv2nt14:
	pushq	72(%rbp)
	pushq	64(%rbp)
	.type	__sysv2nt14,@function
	.size	__sysv2nt14,.-__sysv2nt14
	.globl	__sysv2nt14
	.hidden	__sysv2nt14
__sysv2nt12:
	pushq	56(%rbp)
	pushq	48(%rbp)
	.type	__sysv2nt12,@function
	.size	__sysv2nt12,.-__sysv2nt12
	.globl	__sysv2nt12
	.hidden	__sysv2nt12
__sysv2nt10:
	pushq	40(%rbp)
	pushq	32(%rbp)
	.type	__sysv2nt10,@function
	.size	__sysv2nt10,.-__sysv2nt10
	.globl	__sysv2nt10
	.hidden	__sysv2nt10
__sysv2nt8:
	pushq	24(%rbp)
	pushq	16(%rbp)
	.type	__sysv2nt8,@function
	.size	__sysv2nt8,.-__sysv2nt8
	.globl	__sysv2nt8
	.hidden	__sysv2nt8
__sysv2nt6:
	push	%r9
	push	%r8
	.type	__sysv2nt6,@function
	.size	__sysv2nt6,.-__sysv2nt6
	.globl	__sysv2nt6
	.hidden	__sysv2nt6
__sysv2nt:
	mov	%rdx,%r8
	mov	%rcx,%r9
	mov	%rdi,%rcx
	mov	%rsi,%rdx
	sub	$32,%rsp
	call	*%rax
	leave
	ret
	.type	__sysv2nt,@function
	.size	__sysv2nt,.-__sysv2nt
	.globl	__sysv2nt
	.hidden	__sysv2nt

#endif /* __x86_64__ */
