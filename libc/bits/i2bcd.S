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

/	Converts integer to binary-coded decimal.
i2bcd:	.leafprologue
	.profilable
	test	%rdi,%rdi
	je	2f
	mov	%rdi,%rsi
	xor	%r9d,%r9d
	mov	$0xcccccccccccccccd,%r8
	xor	%ecx,%ecx
1:	mov	%rsi,%rax
	mul	%r8
	shr	$3,%rdx
	lea	(%rdx,%rdx),%rax
	lea	(%rax,%rax,4),%rax
	mov	%rsi,%rdi
	sub	%rax,%rdi
	shl	%cl,%rdi
	add	%rdi,%r9
	add	$4,%rcx
	cmp	$9,%rsi
	mov	%rdx,%rsi
	ja	1b
	jmp	3f
2:	xor	%r9d,%r9d
3:	mov	%r9,%rax
	.leafepilogue
	.endfn	i2bcd,globl
