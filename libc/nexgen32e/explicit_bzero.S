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

/	Sets memory to zero w/ accompanying non-optimizing macro.
/
/	This is intended for security-conscious applications. This
/	implementation also hoses every register the abi allows. A
/	concomitant prototype (str.h) countermands compiler magic.
/
/	@param	rdi is dest
/	@param	rsi is the number of bytes to set
explicit_bzero:
	.leafprologue
	mov	%rsi,%rcx
	xor	%eax,%eax
	rep stosb
	xor	%edx,%edx
	xor	%edi,%edi
	xor	%esi,%esi
	xor	%r8,%r8
	xor	%r9,%r9
	xor	%r10,%r10
	xor	%r11,%r11
	xorps	%xmm0,%xmm0
	xorps	%xmm1,%xmm1
	xorps	%xmm2,%xmm2
	xorps	%xmm3,%xmm3
	xorps	%xmm4,%xmm4
	xorps	%xmm5,%xmm5
	.leafepilogue
	.endfn	explicit_bzero,globl
	.source	__FILE__
