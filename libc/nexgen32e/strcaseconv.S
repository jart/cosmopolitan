/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
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
	.endfn	strtoupper,globl

/	Mutates string to lowercase roman characters.
/
/	@param	RDI points to non-const NUL-terminated string
/	@return	RAX will be original RDI
/	@note	10x faster than C
strtolower:
	mov	$'a-'A,%edx			# adding this lowers
	mov	$'A|'Z<<8,%ecx			# uint8_t range A..Z
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	strtolower,globl

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
	jb	0b
	cmp	%ch,%al
	ja	0b
	add	%dl,-1(%rsi)
	jmp	0b
.Lsse4:	movd	%ecx,%xmm1			# XMM1 = ['A,'Z,0,0,...]
	movd	%edx,%xmm2			# XMM2 = ['a-'A,'a-'A,...]
	pbroadcastb %xmm2
	xor	%ecx,%ecx
2:	movdqa	(%rsi,%rcx),%xmm3
/	              ┌─0:index of the LEAST significant, set, bit is used
/	              │   regardless of corresponding input element validity
/	              │   intres2 is returned in least significant bits of xmm0
/	              ├─1:index of the MOST significant, set, bit is used
/	              │   regardless of corresponding input element validity
/	              │   each bit of intres2 is expanded to byte/word
/	              │┌─0:negation of intres1 is for all 16 (8) bits
/	              │├─1:negation of intres1 is masked by reg/mem validity
/	              ││┌─intres1 is negated (1’s complement)
/	              │││┌─mode{equalany,ranges,equaleach,equalordered}
/	              ││││ ┌─issigned
/	              ││││ │┌─is16bit
/	             u│││├┐││
	pcmpistrm $0b01000100,%xmm3,%xmm1	# →XMM0 8-bit byte mask
	pand	%xmm2,%xmm0			#  won't mask after NUL
	paddb	%xmm0,%xmm3
	movdqa	%xmm3,(%rsi,%rcx)
	lea	16(%rcx),%rcx
	jnz	2b				# PCMPISTRM found NUL
3:	mov	%rdi,%rax
	.leafepilogue
	.endfn	strcaseconv
	.source	__FILE__
