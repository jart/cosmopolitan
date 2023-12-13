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
#ifndef COSMOPOLITAN_LIBC_NT_NTDLLIMPORT_H_
#define COSMOPOLITAN_LIBC_NT_NTDLLIMPORT_H_
#include "ape/relocations.h"
#include "libc/macros.internal.h"
#ifdef __ASSEMBLER__
/* clang-format off */

.macro	.ntimp	fn:req name:req
#ifdef __x86_64__
	.yoink	_init_ntdll
	.initbss 202,_init_ntdll.\fn
__imp_\fn:
	.quad	0
	.endobj	__imp_\fn,globl,hidden
	.previous
	.initro 202,_init_ntdll.2.\fn
	.quad	RVA(.L\fn)
	.previous
	.section .rodata.str1.1,"aSM",@progbits,1
.L\fn:
	.asciz	"\fn"
	.previous
#elif defined(__aarch64__)
	.section .data.nt.\fn,"aw",@progbits
	.globl	__imp_\fn
	.balign	8
__imp_\fn:
	.quad	\name
	.weak	\name
#endif
.endm

/* clang-format on */
#endif /* __ASSEMBLER__ */
#endif /* COSMOPOLITAN_LIBC_NT_NTDLLIMPORT_H_ */
