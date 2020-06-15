/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
#include "libc/macros.h"

/	Compares NUL-terminated strings w/o heavy-lifting.
/
/	@param	rdi is first non-null NUL-terminated string pointer
/	@param	rsi is second non-null NUL-terminated string pointer
/	@return	rax is <0, 0, or >0 based on uint8_t comparison
/	@clob	flags only
/	@asyncsignalsafe
tinystrcmp:
	.leafprologue
	push	%rcx
	push	%rdx
	xor	%ecx,%ecx
0:	movzbl	(%rdi,%rcx,1),%eax
	movzbl	(%rsi,%rcx,1),%edx
	test	%al,%al
	jz	1f
	cmp	%dl,%al
	jne	1f
	inc	%rcx
	jmp	0b
1:	sub	%edx,%eax
	pop	%rdx
	pop	%rcx
	.leafepilogue
	.endfn	tinystrcmp,globl
	.yoink	__FILE__
