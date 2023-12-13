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
#include "libc/macros.internal.h"
.privileged

__xmm_save:
	lea	-128(%rbp),%rdi
	.leafprologue
	movdqu	%xmm0,-0x80(%rdi)
	movdqu	%xmm1,-0x70(%rdi)
	movdqu	%xmm2,-0x60(%rdi)
	movdqu	%xmm3,-0x50(%rdi)
	movdqu	%xmm4,-0x40(%rdi)
	movdqu	%xmm5,-0x30(%rdi)
	movdqu	%xmm6,-0x20(%rdi)
	movdqu	%xmm7,-0x10(%rdi)
	movdqu	%xmm8,0x00(%rdi)
	movdqu	%xmm9,0x10(%rdi)
	movdqu	%xmm10,0x20(%rdi)
	movdqu	%xmm11,0x30(%rdi)
	movdqu	%xmm12,0x40(%rdi)
	movdqu	%xmm13,0x50(%rdi)
	movdqu	%xmm14,0x60(%rdi)
	movdqu	%xmm15,0x70(%rdi)
	.leafepilogue
	.endfn	__xmm_save,globl,hidden

__xmm_load:
	lea	-128(%rbp),%rdi
	.leafprologue
	movdqu	-0x80(%rdi),%xmm0
	movdqu	-0x70(%rdi),%xmm1
	movdqu	-0x60(%rdi),%xmm2
	movdqu	-0x50(%rdi),%xmm3
	movdqu	-0x40(%rdi),%xmm4
	movdqu	-0x30(%rdi),%xmm5
	movdqu	-0x20(%rdi),%xmm6
	movdqu	-0x10(%rdi),%xmm7
	movdqu	0x00(%rdi),%xmm8
	movdqu	0x10(%rdi),%xmm9
	movdqu	0x20(%rdi),%xmm10
	movdqu	0x30(%rdi),%xmm11
	movdqu	0x40(%rdi),%xmm12
	movdqu	0x50(%rdi),%xmm13
	movdqu	0x60(%rdi),%xmm14
	movdqu	0x70(%rdi),%xmm15
	.leafepilogue
	.endfn	__xmm_load,globl,hidden
