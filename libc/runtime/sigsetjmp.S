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

//	Saves caller CPU state and signal mask.
//
//	@param	rdi points to jmp_buf
//	@param	esi if non-zero will cause mask to be saved
//	@return	eax 0 when set and !0 when longjmp'd
//	@returnstwice
sigsetjmp:
#ifdef __x86_64__
	test	%esi,%esi
	jz	setjmp
	popq	64(%rdi)
	mov	%rbx,72(%rdi)
	mov	%rdi,%rbx
	call	setjmp
	pushq	64(%rbx)
	mov	%rbx,%rdi
	mov	%eax,%esi
	mov	72(%rdi),%rbx
	jmp	__sigsetjmp_tail
#elif defined(__aarch64__)
	cbz	x1,setjmp
	str	x30,[x0,#176]
	str	x19,[x0,#176+8+8]
	mov	x19,x0
	bl	setjmp
	mov	w1,w0
	mov	x0,x19
	ldr	x30,[x0,#176]
	ldr	x19,[x0,#176+8+8]
	b	__sigsetjmp_tail
#else
#error "unsupported architecture"
#endif
	.hidden	__sigsetjmp_tail
	.endfn	sigsetjmp,globl
