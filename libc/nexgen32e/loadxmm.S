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

/	Loads XMM registers from buffer.
/
/	@param	%rdi points to &(forcealign(16) uint8_t[256])[128]
/	@note	modern cpus have out-of-order execution engines
loadxmm:.leafprologue
	movaps	-0x80(%rdi),%xmm0
	movaps	-0x70(%rdi),%xmm1
	movaps	-0x60(%rdi),%xmm2
	movaps	-0x50(%rdi),%xmm3
	movaps	-0x40(%rdi),%xmm4
	movaps	-0x30(%rdi),%xmm5
	movaps	-0x20(%rdi),%xmm6
	movaps	-0x10(%rdi),%xmm7
	movaps	0x00(%rdi),%xmm8
	movaps	0x10(%rdi),%xmm9
	movaps	0x20(%rdi),%xmm10
	movaps	0x30(%rdi),%xmm11
	movaps	0x40(%rdi),%xmm12
	movaps	0x50(%rdi),%xmm13
	movaps	0x60(%rdi),%xmm14
	movaps	0x70(%rdi),%xmm15
	.leafepilogue
	.endfn	loadxmm,globl,hidden
	.source	__FILE__
