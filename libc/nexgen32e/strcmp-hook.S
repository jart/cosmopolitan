/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/nexgen32e/x86feature.h"
#include "libc/dce.h"
#include "libc/macros.h"

/	Dispatches to fastest strcmp() implementation.
/
/	@param	rdi is first non-null NUL-terminated string pointer
/	@param	rsi is second non-null NUL-terminated string pointer
/	@return	rax is <0, 0, or >0 based on uint8_t comparison
/	@asyncsignalsafe
	.initbss 300,_init_strcmp
hook$strcmp:
	.quad	0
	.endobj	hook$strcmp,globl,hidden
	.previous

	.init.start 300,_init_strcmp
#if !IsTiny()
	ezlea	strcmp$avx,ax
#if !X86_NEED(AVX)
	ezlea	tinystrcmp,dx
	testb	X86_HAVE(AVX)+kCpuids(%rip)
	cmovz	%rdx,%rax
#endif /* AVX */
#else
	ezlea	tinystrcmp,ax
#endif /* TINY */
	stosq
	.init.end 300,_init_strcmp
	.source	__FILE__
