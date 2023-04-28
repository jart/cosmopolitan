/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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

feclearexcept:
	# maintain exceptions in the sse mxcsr, clear x87 exceptions
	mov	%edi,%ecx
	and	$0x3f,%ecx
	fnstsw	%ax
	test	%eax,%ecx
	jz	1f
	fnclex
1:	stmxcsr	-8(%rsp)
	and	$0x3f,%eax
	or	%eax,-8(%rsp)
	test	%ecx,-8(%rsp)
	jz	1f
	not	%ecx
	and	%ecx,-8(%rsp)
	ldmxcsr	-8(%rsp)
1:	xor	%eax,%eax
	ret
	.endfn	feclearexcept,globl

feraiseexcept:
	and	$0x3f,%edi
	stmxcsr	-8(%rsp)
	or	%edi,-8(%rsp)
	ldmxcsr	-8(%rsp)
	xor	%eax,%eax
	ret
	.endfn	feraiseexcept,globl

__fesetround:
	push	%rax
	xor	%eax,%eax
	mov	%edi,%ecx
	fnstcw	(%rsp)
	andb	$0xf3,1(%rsp)
	or	%ch,1(%rsp)
	fldcw	(%rsp)
	stmxcsr	(%rsp)
	shl	$3,%ch
	andb	$0x9f,1(%rsp)
	or	%ch,1(%rsp)
	ldmxcsr	(%rsp)
	pop	%rcx
	ret
	.endfn	__fesetround,globl,hidden

fegetround:
	push	%rax
	stmxcsr	(%rsp)
	pop	%rax
	shr	$3,%eax
	and	$0xc00,%eax
	ret
	.endfn	fegetround,globl

fegetenv:
	xor	%eax,%eax
	fnstenv	(%rdi)
	stmxcsr	28(%rdi)
	ret
	.endfn	fegetenv,globl

fesetenv:
	xor	%eax,%eax
	inc	%rdi
	jz	1f
	fldenv	-1(%rdi)
	ldmxcsr	27(%rdi)
	ret
1:	push	%rax
	push	%rax
	pushq	$0xffff
	pushq	$0x37f
	fldenv	(%rsp)
	pushq	$0x1f80
	ldmxcsr	(%rsp)
	add	$40,%rsp
	ret
	.endfn	fesetenv,globl

fetestexcept:
	and	$0x3f,%edi
	push	%rax
	stmxcsr	(%rsp)
	pop	%rsi
	fnstsw	%ax
	or	%esi,%eax
	and	%edi,%eax
	ret
	.endfn	fetestexcept,globl
