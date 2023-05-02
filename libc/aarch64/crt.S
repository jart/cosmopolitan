/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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

	.globl	_start
	.type	_start,%function
_start:	mov	x29,#0
	mov	x30,#0
	mov	x0,sp
	and	sp,x0,#-16
	b	_start_c
	.size	_start,.-_start

	.section .initprologue
	.global	_init
	.type	_init,%function
_init:	stp	x29,x30,[sp,-16]!
	mov	x29,sp
	.previous/*
	...
	decentralized content
	...
	*/.section .initepilogue
	ldp	x29,x30,[sp],#16
	ret

	.section .finiprologue
	.global	_fini
	.type	_fini,%function
_fini:	stp	x29,x30,[sp,-16]!
	mov	x29,sp
	.previous/*
	...
	decentralized content
	...
	*/.section .finiepilogue
	ldp	x29,x30,[sp],#16
	ret
