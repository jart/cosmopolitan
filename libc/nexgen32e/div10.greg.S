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

/	Performs 128-bit div+mod by 10 without using div or mod.
/
/	If we didn't have this one-off function, our palandprintf()
/	implementation would cause nearly everything to need a soft
/	math library. It also somehow goes faster than 64-bit IDIV.
/
/	@param	rdi:rsi is the number
/	@param	rdx points to where remainder goes
/	@return	rax:rdx is result of division
/	@see	“Division by Invariant Integers using Multiplication”
/	@see	llog10() and div10int64() is a tiny bit faster
div10:	.leafprologue
	.profilable
	push	%rbx
	mov	%rdx,%r8
	test	%rsi,%rsi
	je	1f
	bsr	%rsi,%r10
	xor	$63,%r10d
	mov	$125,%r9d
	sub	%r10d,%r9d
	cmp	$64,%r9d
	jne	6f
	xor	%eax,%eax
	xor	%r11d,%r11d
	jmp	9f
1:	test	%r8,%r8
	je	3f
	movabs	$0xcccccccccccccccd,%rcx
	mov	%rdi,%rax
	mul	%rcx
	shr	$3,%rdx
	add	%edx,%edx
	lea	(%rdx,%rdx,4),%eax
	mov	%edi,%ecx
	sub	%eax,%ecx
	mov	%ecx,(%r8)
3:	movabs	$0xcccccccccccccccd,%rcx
	mov	%rdi,%rax
	mul	%rcx
	mov	%rdx,%rax
	shr	$3,%rax
	xor	%edi,%edi
	jmp	14f
6:	mov	%r9d,%ecx
	neg	%cl
	cmp	$62,%r10d
	jb	8f
	mov	%rdi,%rdx
	shl	%cl,%rdx
	mov	%rsi,%rax
	mov	%r9d,%ecx
	shr	%cl,%rax
	shrd	%cl,%rsi,%rdi
	xor	%r11d,%r11d
	mov	%rdi,%rsi
	mov	%rdx,%rdi
	jmp	9f
8:	mov	%rdi,%r11
	shl	%cl,%r11
	mov	%rsi,%rax
	shl	%cl,%rax
	mov	%r9d,%ecx
	shr	%cl,%rdi
	or	%rax,%rdi
	shr	%cl,%rsi
	xor	%eax,%eax
9:	add	$-125,%r10d
	xor	%ecx,%ecx
	mov	$9,%r9d
10:	shld	$1,%rsi,%rax
	shld	$1,%rdi,%rsi
	shld	$1,%r11,%rdi
	mov	%r11,%rdx
	add	%r11,%rdx
	mov	%rcx,%r11
	or	%rdx,%r11
	cmp	%rsi,%r9
	mov	$0,%ebx
	sbb	%rax,%rbx
	sar	$63,%rbx
	mov	%ebx,%ecx
	and	$1,%ecx
	and	$10,%ebx
	sub	%rbx,%rsi
	sbb	$0,%rax
	inc	%r10d
	jne	10b
	test	%r8,%r8
	je	13f
	mov	%esi,(%r8)
13:	lea	(%rcx,%r11,2),%rax
	shld	$1,%rdx,%rdi
14:	mov	%rdi,%rdx
	pop	%rbx
	.leafepilogue
	.endfn	div10,globl,hidden
	.source	__FILE__
