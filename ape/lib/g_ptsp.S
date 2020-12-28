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
#include "ape/lib/pc.h"
#include "ape/config.h"
#include "ape/macros.internal.h"
#include "ape/notice.inc"
.section .real,"ax",@progbits
.source	__FILE__
.code16

	.globl	g_ptsp
	.hidden	g_ptsp
	.type	g_ptsp,@object
	.size	g_ptsp,XLM_PAGE_TABLE_STACK_POINTER_SIZE
	g_ptsp = ape.xlm + XLM_PAGE_TABLE_STACK_POINTER

	.globl	g_ptsp_xlm
	.hidden	g_ptsp_xlm
	.type	g_ptsp_xlm,@object
	.size	g_ptsp_xlm,XLM_PAGE_TABLE_STACK_POINTER_SIZE
	g_ptsp_xlm = XLM(PAGE_TABLE_STACK_POINTER)
