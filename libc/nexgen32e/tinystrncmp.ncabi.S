/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
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

/	Compares strings w/ limit & no-clobber greg abi.
/
/	@param	%rdi is first string
/	@param	%rsi is second string
/	@param	%rdx is max length
/	@return	<0, 0, or >0 depending on comparison
/	@clob	flags only
/	@asyncsignalsafe
tinystrncmp:
	.leafprologue
	push	%rbx
	push	%rcx
	xor	%eax,%eax
	xor	%ebx,%ebx
	xor	%ecx,%ecx
	test	%edx,%edx
	jz	2f
	cmp	%rdi,%rsi
	je	2f
0:	cmp	%edx,%ecx
	jae	1f
	movzbl	(%rdi,%rcx,1),%eax
	movzbl	(%rsi,%rcx,1),%ebx
	test	%al,%al
	jz	1f
	cmp	%bl,%al
	jne	1f
	inc	%ecx
	jmp	0b
1:	sub	%ebx,%eax
2:	pop	%rcx
	pop	%rbx
	.leafepilogue
	.endfn	tinystrncmp,globl
	.source	__FILE__
