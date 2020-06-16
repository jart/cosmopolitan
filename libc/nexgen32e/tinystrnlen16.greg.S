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

/	16-bit strnlen that's tiny and near optimal if data's tiny.
/
/	@param	RDI is char16_t *s
/	@param	RSI is size_t n
/	@param	EAX is unsigned length
/	@see	libc/nexgen32e/strsak16.S
tinystrnlen16:
	.leafprologue
	.profilable
	xor	%eax,%eax
1:	cmp	%esi,%eax
	jae	2f
	cmpw	$0,(%rdi,%rax,2)
	jz	2f
	inc	%eax
	jmp	1b
2:	.leafepilogue
	.endfn	tinystrnlen16,globl
	.source	__FILE__
