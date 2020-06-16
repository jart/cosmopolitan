/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
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
.source	__FILE__

/	Compares NUL-terminated wchar_t strings.
/
/	@param	rdi first string
/	@param	rsi second string
/	@return	0 if equal, etc.
/	@note	wchar_t is a 32-bit signed type
wcscmp$k8:
	or	$-1,%rdx
/	fallthrough
	.endfn	wcscmp$k8,globl

/	Compares NUL-terminated wchar_t strings w/ limit.
/
/	@param	rdi first string
/	@param	rsi second string
/	@param	rdx max shorts
/	@return	0 if equal, etc.
/	@note	wchar_t is a 32-bit signed type
wcsncmp$k8:
	.leafprologue
	.profilable
	push	%rbx
	xor	%eax,%eax
	cmp	%rdi,%rsi
	je	3f
	or	$-1,%rcx
1:	add	$1,%rcx
	cmp	%rcx,%rdx
	je	3f
	mov	(%rsi,%rcx,4),%ebx
	cmp	(%rdi,%rcx,4),%ebx
	jne	2f
	test	%ebx,%ebx
	jnz	1b
2:	cmovl	.Lone(%rip),%eax
	cmovg	.Lneg1(%rip),%eax
3:	pop	%rbx
	.leafepilogue
	.endfn	wcsncmp$k8,globl

	.rodata.cst4
.Lone:	.long	1
	.endobj	.Lone
.Lneg1:	.long	-1
	.endobj	.Lneg1
	.previous
