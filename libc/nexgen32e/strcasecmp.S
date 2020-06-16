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
#include "libc/macros.h"

/	Compares NUL-terminated strings, ignoring ASCII case.
/
/	@param	rdi first string
/	@param	rsi second string
/	@return	0 if equal, etc.
/	@note	char is treated as unsigned
strcasecmp:
	or	$-1,%rdx
/	fallthrough
	.endfn	strcasecmp,globl

/	Compares NUL-terminated strings w/ limit ignoring ASCII case.
/
/	@param	rdi first string
/	@param	rsi second string
/	@param	rdx max bytes
/	@return	0 if equal, etc.
/	@note	char is treated as unsigned
strncasecmp:
	.leafprologue
	.profilable
	push	%rbx
	cmp	%rdi,%rsi
	je	3f
	ezlea	kToLower,bx
	or	$-1,%rcx
1:	add	$1,%rcx
	cmp	%rcx,%rdx
	je	3f
	movzbl	(%rdi,%rcx),%r8d
	movzbl	(%rsi,%rcx),%eax
	xlat
	xchg	%r8d,%eax
	xlat
	cmp	%r8b,%al
	jne	2f
	test	%al,%al
	jnz	1b
2:	sub	%r8d,%eax
	jmp	4f
3:	xor	%eax,%eax
4:	pop	%rbx
	.leafepilogue
	.endfn	strncasecmp,globl
	.source	__FILE__
