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

/	Compares NUL-terminated char16_t strings.
/
/	@param	rdi first string
/	@param	rsi second string
/	@return	0 if equal, etc.
/	@note	char16_t is an unsigned type
/	@asyncsignalsafe
strcmp16$k8:
	or	$-1,%rdx
/	fallthrough
	.endfn	strcmp16$k8,globl,hidden

/	Compares NUL-terminated char16_t strings w/ limit.
/
/	@param	rdi first string
/	@param	rsi second string
/	@param	rdx max shorts
/	@return	0 if equal, etc.
/	@note	char16_t is an unsigned type
/	@asyncsignalsafe
strncmp16$k8:
	.leafprologue
	.profilable
	push	%rbx
	xor	%eax,%eax
	xor	%ebx,%ebx
	xor	%ecx,%ecx
	cmp	%rdi,%rsi
	je	1f
	test	%rdx,%rdx
	jz	1f
0:	inc	%rcx
	movzwl	-2(%rdi,%rcx,2),%eax
	movzwl	-2(%rsi,%rcx,2),%ebx
	cmp	%rcx,%rdx
	je	1f
	cmp	%ebx,%eax
	jne	1f
	test	%eax,%eax
	jne	0b
1:	sub	%ebx,%eax
	pop	%rbx
	.leafepilogue
	.endfn	strncmp16$k8,globl,hidden
	.source	__FILE__
