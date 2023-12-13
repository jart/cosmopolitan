/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│                                                                              │
│  Musl Libc                                                                   │
│  Copyright © 2005-2014 Rich Felker, et al.                                   │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.internal.h"

//	Clears floating point exception status, e.g.
//
//	    feclearexcept(FE_ALL_EXCEPT);
//
//	@param excepts may bitwise-or the following:
//		- `FE_INVALID`
//		- `FE_DIVBYZERO`
//		- `FE_OVERFLOW`
//		- `FE_UNDERFLOW`
//		- `FE_INEXACT`
//		- `FE_ALL_EXCEPT` (all of the above)
//	@return	0 on success, or nonzero on error
	.ftrace1
feclearexcept:
	.ftrace2
#ifdef __x86_64__
//	maintain exceptions in the sse mxcsr, clear x87 exceptions
	mov	%edi,%ecx
	and	$0x3f,%ecx
#ifndef NOX87
	fnstsw	%ax
	test	%eax,%ecx
	jz	1f
	fnclex
1:
#endif
	stmxcsr	-8(%rsp)
	and	$0x3f,%eax
	or	%eax,-8(%rsp)
	test	%ecx,-8(%rsp)
	jz	1f
	not	%ecx
	and	%ecx,-8(%rsp)
	ldmxcsr	-8(%rsp)
1:	xor	%eax,%eax
	ret
#elif defined(__aarch64__)
	and	w0,w0,#0x1f
	mrs	x1,fpsr
	bic	w1,w1,w0
	msr	fpsr,x1
	mov	w0,#0
	ret
#else
#error "unsupported architecture"
#endif
	.endfn	feclearexcept,globl

//	Checks for floating point exception.
//
//	This function may be used to check the thread-local
//	floating-point exception status bits, e.g.
//
//	    feclearexcept(FE_ALL_EXCEPT);
//	    volatile double x = 0, y = 1 / x;
//	    assert(fetestexcept(FE_ALL_EXCEPT) == FE_DIVBYZERO);
//
	.ftrace1
//	@param excepts may bitwise-or the following:
//		- `FE_INVALID`
//		- `FE_DIVBYZERO`
//		- `FE_OVERFLOW`
//		- `FE_UNDERFLOW`
//		- `FE_INEXACT`
//		- `FE_ALL_EXCEPT` (all of the above)
//	@return	mask of which exception status codes are currently set,
//		or zero if there aren't any floating point exceptions
	.ftrace1
fetestexcept:
	.ftrace2
#ifdef __x86_64__
	and	$0x3f,%edi
	push	$0
	stmxcsr	(%rsp)
#ifdef NOX87
	pop	%rax
#else
	pop	%rsi
	fnstsw	%ax
	or	%esi,%eax
	and	%edi,%eax
#endif
	ret
#elif defined(__aarch64__)
	and	w0,w0,#0x1f
	mrs	x1,fpsr
	and	w0,w0,w1
	ret
#endif
	.endfn	fetestexcept,globl

	.ftrace1
feraiseexcept:
	.ftrace2
#ifdef __x86_64__
	and	$0x3f,%edi
	stmxcsr	-8(%rsp)
	or	%edi,-8(%rsp)
	ldmxcsr	-8(%rsp)
	xor	%eax,%eax
	ret
#elif defined(__aarch64__)
	and	w0,w0,#0x1f
	mrs	x1,fpsr
	orr	w1,w1,w0
	msr	fpsr,x1
	mov	w0,#0
	ret
#endif
	.endfn	feraiseexcept,globl

	.ftrace1
__fesetround:
	.ftrace2
#ifdef __x86_64__
	push	%rax
	xor	%eax,%eax
	mov	%edi,%ecx
#ifndef NOX87
	fnstcw	(%rsp)
	andb	$0xf3,1(%rsp)
	or	%ch,1(%rsp)
	fldcw	(%rsp)
#endif
	stmxcsr	(%rsp)
	shl	$3,%ch
	andb	$0x9f,1(%rsp)
	or	%ch,1(%rsp)
	ldmxcsr	(%rsp)
	pop	%rcx
	ret
#elif defined(__aarch64__)
	mrs	x1,fpcr
	bic	w1,w1,#0xc00000
	orr	w1,w1,w0
	msr	fpcr,x1
	mov	w0,#0
	ret
#endif
	.endfn	__fesetround,globl,hidden

	.ftrace1
fegetround:
	.ftrace2
#ifdef __x86_64__
	push	%rax
	stmxcsr	(%rsp)
	pop	%rax
	shr	$3,%eax
	and	$0xc00,%eax
	ret
#elif defined(__aarch64__)
	mrs	x0,fpcr
	and	w0,w0,#0xc00000
	ret
#endif
	.endfn	fegetround,globl

	.ftrace1
fegetenv:
	.ftrace2
#ifdef __x86_64__
	xor	%eax,%eax
#ifndef NOX87
	fnstenv	(%rdi)
#endif
	stmxcsr	28(%rdi)
	ret
#elif defined(__aarch64__)
	mrs	x1,fpcr
	mrs	x2,fpsr
	stp	w1,w2,[x0]
	mov	w0,#0
	ret
#endif
	.endfn	fegetenv,globl

	.ftrace1
fesetenv:
	.ftrace2
#ifdef __x86_64__
	xor	%eax,%eax
	inc	%rdi
	jz	1f
#ifndef NOX87
	fldenv	-1(%rdi)
#endif
	ldmxcsr	27(%rdi)
	ret
1:	push	%rax
	push	%rax
	pushq	$0xffff
	pushq	$0x37f
#ifndef NOX87
	fldenv	(%rsp)
#endif
	pushq	$0x1f80
	ldmxcsr	(%rsp)
	add	$40,%rsp
	ret
#elif defined(__aarch64__)
	mov	x1,#0
	mov	x2,#0
	cmn	x0,#1
	b.eq	1f
	ldp	w1,w2,[x0]
1:	msr	fpcr,x1
	msr	fpsr,x2
	mov	w0,#0
	ret
#endif
	.endfn	fesetenv,globl
