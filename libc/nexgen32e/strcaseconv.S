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
#include "libc/nexgen32e/macros.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/macros.h"

/	Mutates string to uppercase roman characters.
/
/	@param	RDI points to non-const NUL-terminated string
/	@return	RAX will be original RDI
/	@note	10x faster than C
strtoupper:
	mov	$'A-'a,%edx			# adding this uppers
	mov	$'a|'z<<8,%ecx			# uint8_t range a..z
	jmp	strcaseconv

/	Mutates string to lowercase roman characters.
/
/	@param	RDI points to non-const NUL-terminated string
/	@return	RAX will be original RDI
/	@note	10x faster than C
strtolower:
	mov	$'a-'A,%edx			# adding this lowers
	mov	$'A|'Z<<8,%ecx			# uint8_t range A..Z
/	𝑠𝑙𝑖𝑑𝑒

/	Support code for strtolower() and strtoupper().
/
/	@param	RDI points to non-const NUL-terminated string
/	@param	CL defines start of character range to mutate
/	@param	CH defines end of character range to mutate
/	@param	DL is added to each DIL ∈ [CL,CH]
/	@return	RAX will be original RDI
strcaseconv:
	.leafprologue
	.profilable
	mov	%rdi,%rsi
0:	testb	$15,%sil			# is it aligned?
#if X86_NEED(SSE4_2)
	jz	.Lsse4
#else
	jnz	1f
	testb	X86_HAVE(SSE4_2)+kCpuids(%rip)
	jnz	.Lsse4				# is it nehalem?
#endif
1:	lodsb					# AL = *RSI++
	test	%al,%al				# is it NUL?
	jz	3f
	cmp	%cl,%al				# is it in range?
	jb	1b
	cmp	%ch,%al
	ja	1b
	add	%dl,-1(%rsi)
	jmp	1b
.Lsse4:	movd	%ecx,%xmm1			# XMM1 = ['A,'Z,0,0,...]
	movd	%edx,%xmm2			# XMM2 = ['a-'A,'a-'A,...]
	pbroadcastb %xmm2
	xor	%ecx,%ecx
2:	movdqa	(%rsi,%rcx),%xmm3
	pcmpistrm $0b01000100,%xmm3,%xmm1	# →XMM0 8-bit byte mask
	pand	%xmm2,%xmm0			#  won't mask after NUL
	paddb	%xmm0,%xmm3
	movdqa	%xmm3,(%rsi,%rcx)
	lea	16(%rcx),%rcx
	jnz	2b				# PCMPISTRM found NUL
3:	mov	%rdi,%rax
	.leafepilogue
	.endfn	strcaseconv
	.endfn	strtolower,globl
	.endfn	strtoupper,globl
	.source	__FILE__
