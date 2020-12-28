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
#include "libc/macros.h"
.privileged

/	Stores XMM registers to buffer.
/
/	@param	%rdi points to &(forcealign(16) uint8_t[256])[128]
/	@note	modern cpus have out-of-order execution engines
savexmm:.leafprologue
	movaps	%xmm0,-0x80(%rdi)
	movaps	%xmm1,-0x70(%rdi)
	movaps	%xmm2,-0x60(%rdi)
	movaps	%xmm3,-0x50(%rdi)
	movaps	%xmm4,-0x40(%rdi)
	movaps	%xmm5,-0x30(%rdi)
	movaps	%xmm6,-0x20(%rdi)
	movaps	%xmm7,-0x10(%rdi)
	movaps	%xmm8,0x00(%rdi)
	movaps	%xmm9,0x10(%rdi)
	movaps	%xmm10,0x20(%rdi)
	movaps	%xmm11,0x30(%rdi)
	movaps	%xmm12,0x40(%rdi)
	movaps	%xmm13,0x50(%rdi)
	movaps	%xmm14,0x60(%rdi)
	movaps	%xmm15,0x70(%rdi)
	.leafepilogue
	.endfn	savexmm,globl,hidden
	.source	__FILE__
