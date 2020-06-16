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
/* clang-format off */

/	Searches for substring.
/
/	@param	rdi is NUL-terminated haystack string
/	@param	rsi is NUL-terminated needle string (16-byte aligned)
/	@return	rax is pointer to substring or NULL
/	@todo	10x faster than naïve but could be 100x faster
.macro	.strstr	mode:req
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	mov	%rdi,%rax
	xor	%ecx,%ecx
0:	mov	$-16,%rdx
1:	add	$16,%rdx
	movaps	(%rsi,%rdx),%xmm0
2:	add	%rcx,%rax
	lea	(%rax,%rdx),%rdi
	test	$15,%edi
	jnz	6f
	pcmpistri $\mode,(%rdi),%xmm0
3:	ja	2b	# !CF (no match) && !ZF (need NUL-term)
	jnc	4f	# !CF (no match) && ZF (NUL-terminator)
	jno	0b	# !OF ← CF && CX!=0 (matched at offset)
	jns	1b	# !SF ← NUL ∉ XMM1 (need to match more)
	jmp	5f	# youtu.be/nVk1DjMtLWs
4:	xor	%eax,%eax
5:	leave
	ret
6:	mov	%rdi,%r9			# same w/ pointer realign
	and	$15,%r9d
	mov	%edi,%r8d
	and	$0xfff,%r8d
	cmp	$0xff0,%r8d
	ja	8f
7:	pcmpistri $\mode,(%rdi),%xmm0
	cmova	%r9d,%ecx
	jmp	3b
8:	pcmpeqd	%xmm2,%xmm2			# handle danger memory
	mov	%rdi,%r8
	and	$-16,%r8
	movaps	(%r8),%xmm1
	movaps	%xmm1,-32(%rbp)
	movaps	%xmm2,-16(%rbp)
	pcmpistri $\mode,-32(%rbp,%r9),%xmm2
	jz	4b
	jmp	7b
.endm
