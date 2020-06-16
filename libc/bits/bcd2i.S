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

/	Converts binary-coded-decimal to integer.
/
/	@param	rdi is the string copied into a word
bcd2i:	.leafprologue
	.profilable
	test	%rdi,%rdi
	je	2f
	mov	$1,%ecx
	xor	%eax,%eax
1:	mov	%edi,%edx
	and	$15,%edx
	imul	%rcx,%rdx
	add	%rdx,%rax
	add	%rcx,%rcx
	lea	(%rcx,%rcx,4),%rcx
	shr	$4,%rdi
	jne	1b
	ret
2:	xor	%eax,%eax
	.leafepilogue
	.endfn	bcd2i,globl
