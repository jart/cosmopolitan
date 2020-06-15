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

/	Returns nonzero if %dil ∈ [ \t\v\r\n].
/
/	@param	edi is character to test (treated as uint8_t)
/	@return	nonzero if character matches, otherwise 0
isspace:push	$1
	jmp	ctypep
	.endfn	isspace,globl

/	Returns nonzero if %dil ∈ [A-Za-z].
/
/	@param	edi is character to test (treated as uint8_t)
/	@return	nonzero if character matches, otherwise 0
isalpha:push	$2
	jmp	ctypep
	.endfn	isalpha,globl

/	Returns nonzero if %dil ∈ [0-9].
/
/	@param	edi is character to test (treated as uint8_t)
/	@return	nonzero if character matches, otherwise 0
isdigit:push	$4
	jmp	ctypep
	.endfn	isdigit,globl

/	Returns nonzero if %dil ∈ [0-9A-Za-z].
/
/	@param	edi is character to test (treated as uint8_t)
/	@return	nonzero if character matches, otherwise 0
isalnum:push	$6
	jmp	ctypep
	.endfn	isalnum,globl

/	Returns nonzero if %dil ∈ [0-9A-fa-f].
/
/	@param	edi is character to test (treated as uint8_t)
/	@return	nonzero if character matches, otherwise 0
isxdigit:push	$8
	jmp	ctypep
	.endfn	isxdigit,globl

/	Returns nonzero if %dil ∈ [ -~] a.k.a. [\x20-\x7e].
/
/	@param	edi is character to test (treated as uint8_t)
/	@return	nonzero if character matches, otherwise 0
isprint:push	$16
	jmp	ctypep
	.endfn	isprint,globl

/	Returns nonzero if %dil ∈ [a-z]
/
/	@param	edi is character to test (treated as uint8_t)
/	@return	nonzero if character matches, otherwise 0
islower:push	$32
	jmp	ctypep
	.endfn	islower,globl

/	Returns nonzero if %dil ∈ [A-Z]
/
/	@param	edi is character to test (treated as uint8_t)
/	@return	nonzero if character matches, otherwise 0
isupper:push	$64
	jmp	ctypep
	.endfn	isupper,globl

/	Returns nonzero if %dil ∈ [ \t]
/
/	@param	edi is character to test (treated as uint8_t)
/	@return	nonzero if character matches, otherwise 0
isblank:push	$-128
/	fallthrough
	.endfn	isblank,globl

ctypep:	pop	%rsi
	movzbl	%dil,%edi
	ezlea	kCtype,cx
	movzbl	(%rcx,%rdi),%eax
	and	%esi,%eax
	ret
	.endfn	ctypep
	.yoink	__FILE__
