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

/	Compares NUL-terminated wchar_t strings, ignoring ASCII case.
/
/	@param	rdi first string
/	@param	rsi second string
/	@return	0 if equal, etc.
/	@note	wchar_t is a 32-bit signed type
wcscasecmp:
	or	$-1,%rdx
/	fallthrough
	.endfn	wcscasecmp,globl

/	Compares NUL-terminated wchar_t strings w/ limit ignoring ASCII case.
/
/	@param	rdi first string
/	@param	rsi second string
/	@param	rdx max shorts
/	@return	0 if equal, etc.
/	@note	wchar_t is a 32-bit signed type
wcsncasecmp:
	.leafprologue
	.profilable
	push	%rbx
	cmp	%rdi,%rsi
	je	3f
	lea	kToLower16(%rip),%r8
	or	$-1,%rcx
1:	add	$1,%rcx
	cmp	%rcx,%rdx
	je	3f
	mov	(%rsi,%rcx,4),%eax
	mov	%eax,%ebx
	and	$0x7f,%ebx
	cmp	%eax,%ebx
	cmove	(%r8,%rbx,2),%ax
	push	%rax
	mov	(%rdi,%rcx,4),%eax
	mov	%eax,%ebx
	and	$0x7f,%ebx
	cmp	%eax,%ebx
	cmove	(%r8,%rbx,2),%ax
	pop	%rbx
	cmp	%ebx,%eax
	jne	2f
	test	%eax,%eax
	jnz	1b
2:	sub	%ebx,%eax
	jmp	4f
3:	xor	%eax,%eax
4:	pop	%rbx
	.leafepilogue
	.endfn	wcsncasecmp,globl

	.yoink	__FILE__
