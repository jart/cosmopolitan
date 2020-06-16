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

/	Duplicates chrominance samples horizontally, e.g.
/
/	    12345678--------
/	  → 1122334455667788
/
/	@param	%edi is size of %rsi array in bytes
/	@param	%rsi is char[edi/16][16] output and %rsi==%rdx is OK
/	@param	%rdx is char[edi/16][8] input
/	@return	%rax is %rsi
doublechrominance:
	.leafprologue
	.profilable
	shr	$1,%edi			# backwards algorithm
	jbe	1f			# do nothing if !n || n%2
	mov	%edi,%ecx
	shr	$4,%ecx
	shl	$4,%ecx
0:	cmp	%edi,%ecx
	je	0f
	dec	%edi
	movzbl	(%rdx,%rdi),%eax
	mov	%al,(%rsi,%rdi,2)
	mov	%al,1(%rsi,%rdi,2)
	jmp	0b
0:	sub	$8,%edi
	movq	(%rdx,%rdi),%xmm0
	punpcklbw %xmm0,%xmm0
	movdqu	%xmm0,(%rsi,%rdi,2)
	jnz	0b
1:	mov	%rsi,%rax
	.leafepilogue
	.endfn	doublechrominance,globl
	.source	__FILE__
