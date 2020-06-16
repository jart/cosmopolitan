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
.source	__FILE__

/	Encodes Thompson-Pike varint.
/
/	@param	edi is int to encode
/	@return	rax is word-encoded byte buffer
/	@clob	edi,rax,flags
/	@note	invented on a napkin in a new jersey diner
tpenc:	.leafprologue
	.profilable
	push	%rcx
	push	%rdx
	mov	%edi,%edi
	xor	%eax,%eax
	cmp	$127,%edi
	jbe	3f
	bsr	%edi,%ecx
	ezlea	kTpenc,dx
	mov	-7*(1+1)(%rdx,%rcx,2),%ecx
1:	mov	%edi,%edx
	shr	$6,%edi
	and	$0b00111111,%dl
	or	$0b10000000,%al
	or	%dl,%al
	shl	$8,%rax
	dec	%cl
	jnz	1b
2:	or	%ch,%al
3:	or	%rdi,%rax
	pop	%rdx
	pop	%rcx
	.leafepilogue
	.endfn	tpenc,globl

	.rodata
kTpenc:	.rept	4			# MSB≤10 (0x7FF)
	.byte	1,0b11000000		# len,mark
	.endr
	.rept	5			# MSB≤15 (0xFFFF)
	.byte	2,0b11100000		# len,mark
	.endr
	.rept	5			# MSB≤20 (0x1FFFFF)
	.byte	3,0b11110000		# len,mark
	.endr
	.rept	5			# MSB≤25 (0x3FFFFFF)
	.byte	4,0b11111000		# len,mark
	.endr
	.rept	6			# MSB≤31 (0xffffffff)
	.byte	5,0b11111100		# len,mark
	.endr
	.endobj	kTpenc
