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
#include "libc/nexgen32e/x86feature.h"
#include "libc/macros.h"
.text.startup

/	Initializes jump table for memset() and memcpy().
/
/	@param	!ZF if required cpu vector extensions are available
/	@param	rdi is address of 64-bit jump table
/	@param	rsi is address of 8-bit jump initializers
/	@param	rdx is address of indirect branch
/	@param	ecx is size of jump table
memjmpinit:
	.leafprologue
	setnz	%r8b
	shl	%r8b
0:	xor	%eax,%eax
	lodsb
	add	%rdx,%rax
	stosq
	.loop	0b
	xor	%eax,%eax
	testb	X86_HAVE(ERMS)+kCpuids(%rip)
	setnz	%al
	or	%r8b,%al
	mov	(%rsi,%rax),%al
	add	%rdx,%rax
	stosq
	lodsq
	.leafepilogue
	.endfn	memjmpinit,globl,hidden
	.source	__FILE__
