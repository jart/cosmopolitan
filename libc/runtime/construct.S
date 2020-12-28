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
#include "libc/runtime/internal.h"
#include "libc/macros.h"
.text.startup
.source	__FILE__

/	Calls global initialization functions.
/
/	@param	r12 is argc
/	@param	r13 is argv
/	@param	r14 is environ
/	@param	r15 is auxv
_construct:
	push	%rbp
	mov	%rsp,%rbp
	ezlea	__init_array_start,ax		# static ctors in forward order
	.weak	__init_array_start		# could be called multiple times
	ezlea	__init_array_end,cx		# idempotency recommended
	.weak	__init_array_end		# @see ape/ape.lds
1:	cmp	%rax,%rcx
	je	2f
	push	%rax
	push	%rcx
	mov	%r12,%rdi
	mov	%r13,%rsi
	mov	%r14,%rdx
	mov	%r15,%rcx
	call	*(%rax)
	pop	%rcx
	pop	%rax
	add	$8,%rax
	jmp	1b
2:	pop	%rbp
	ret
	.endfn	_construct,globl
