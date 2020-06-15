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

/	Multiplies matrices.
	.p2align 4
dgemm_:	push	%r15
	mov	%rdi,%r15
	push	%r14
	mov	%r8,%r14
	push	%r13
	mov	%rsi,%r13
	mov	$.LC1,%esi
	push	%r12
	push	%rbp
	push	%rbx
	sub	$248,%rsp
	mov	%rcx,8(%rsp)
	mov	352(%rsp),%rcx
	mov	%rdx,(%rsp)
	mov	$1,%edx
	mov	304(%rsp),%rbx
	mov	(%rcx),%ecx
	mov	344(%rsp),%rbp
	mov	%r9,24(%rsp)
	mov	%ecx,16(%rsp)
	mov	328(%rsp),%rcx
	mov	(%rcx),%ecx
	mov	%ecx,32(%rsp)
	mov	312(%rsp),%rcx
	mov	(%rcx),%ecx
	mov	%ecx,40(%rsp)
	mov	$1,%ecx
	call	lsame_
	mov	$1,%ecx
	mov	$1,%edx
	mov	%r13,%rdi
	mov	%eax,%r12d
	mov	$.LC1,%esi
	call	lsame_
	test	%r12d,%r12d
	mov	320(%rsp),%r11
	mov	%eax,%r8d
	je	.L2
	mov	(%rsp),%rax
	mov	(%rax),%r10d
	mov	%r10d,%r9d
	test	%r8d,%r8d
	jne	.L3
	movl	$0,236(%rsp)
	mov	8(%rsp),%rax
	mov	(%rax),%eax
	mov	%eax,48(%rsp)
.L103:	mov	$1,%ecx
	mov	$1,%edx
	mov	$.LC2,%esi
	mov	%r13,%rdi
	mov	%r11,320(%rsp)
	mov	%r8d,64(%rsp)
	mov	%r10d,56(%rsp)
	call	lsame_
	mov	56(%rsp),%r10d
	mov	64(%rsp),%r8d
	test	%eax,%eax
	mov	320(%rsp),%r11
	jne	.L156
	mov	$1,%ecx
	mov	$1,%edx
	mov	$.LC3,%esi
	mov	%r13,%rdi
	mov	%r11,320(%rsp)
	mov	%r8d,64(%rsp)
	mov	%r10d,56(%rsp)
	call	lsame_
	test	%eax,%eax
	jne	.L155
	movl	$2,236(%rsp)
	jmp	.L8
	.p2align 4,,10
	.p2align 3
.L2:	mov	%r11,320(%rsp)
	mov	(%r14),%r10d
	mov	%eax,64(%rsp)
	test	%eax,%eax
	je	.L5
	mov	$1,%ecx
	mov	$1,%edx
	mov	$.LC2,%esi
	mov	%r15,%rdi
	mov	%r10d,48(%rsp)
	mov	%r10d,56(%rsp)
	movl	$0,236(%rsp)
	call	lsame_
	mov	56(%rsp),%r10d
	mov	64(%rsp),%r8d
	test	%eax,%eax
	mov	320(%rsp),%r11
	je	.L145
.L156:	mov	(%rsp),%rax
	mov	(%rax),%r9d
.L6:	test	%r9d,%r9d
	js	.L160
.L9:	mov	8(%rsp),%rax
	mov	(%rax),%eax
	test	%eax,%eax
	js	.L161
	mov	(%r14),%r13d
	test	%r13d,%r13d
	js	.L162
	mov	312(%rsp),%rdi
	mov	$1,%edx
	test	%r10d,%r10d
	mov	%r10d,%ecx
	cmovle	%edx,%ecx
	cmp	%ecx,(%rdi)
	jge	.L12
	movl	$8,236(%rsp)
	jmp	.L8
.L3:	movl	$0,236(%rsp)
	mov	(%r14),%eax
	mov	%eax,48(%rsp)
	test	%r9d,%r9d
	jns	.L9
.L160:	movl	$3,236(%rsp)
.L8:	mov	$6,%edx
	lea	236(%rsp),%rsi
	mov	$.LC5,%edi
	call	xerbla_
.L1:	add	$248,%rsp
	pop	%rbx
	pop	%rbp
	pop	%r12
	pop	%r13
	pop	%r14
	pop	%r15
	ret
.L161:	movl	$4,236(%rsp)
	jmp	.L8
.L5:	mov	8(%rsp),%rax
	mov	$1,%ecx
	mov	$1,%edx
	mov	%r15,%rdi
	mov	$.LC2,%esi
	mov	%r10d,56(%rsp)
	movl	$0,236(%rsp)
	mov	(%rax),%eax
	mov	%eax,48(%rsp)
	call	lsame_
	mov	56(%rsp),%r10d
	mov	64(%rsp),%r8d
	test	%eax,%eax
	mov	320(%rsp),%r11
	jne	.L103
	mov	$1,%ecx
	mov	$1,%edx
	mov	$.LC3,%esi
	mov	%r15,%rdi
	mov	%r11,320(%rsp)
	mov	%r8d,64(%rsp)
	mov	%r10d,56(%rsp)
	call	lsame_
	mov	56(%rsp),%r10d
	mov	64(%rsp),%r8d
	test	%eax,%eax
	mov	320(%rsp),%r11
	jne	.L103
.L104:	movl	$1,236(%rsp)
	jmp	.L8
.L162:	movl	$5,236(%rsp)
	jmp	.L8
.L12:	mov	48(%rsp),%ecx
	mov	328(%rsp),%rdi
	test	%ecx,%ecx
	cmovle	%edx,%ecx
	cmp	%ecx,(%rdi)
	jl	.L163
	mov	352(%rsp),%rcx
	test	%r9d,%r9d
	cmovg	%r9d,%edx
	cmp	%edx,(%rcx)
	jge	.L14
	movl	$13,236(%rsp)
	jmp	.L8
.L145:	mov	$1,%ecx
	mov	$1,%edx
	mov	$.LC3,%esi
	mov	%r15,%rdi
	mov	%r11,320(%rsp)
	mov	%r8d,64(%rsp)
	mov	%r10d,56(%rsp)
	call	lsame_
	test	%eax,%eax
	je	.L104
.L155:	mov	(%rsp),%rax
	mov	56(%rsp),%r10d
	mov	64(%rsp),%r8d
	mov	320(%rsp),%r11
	mov	(%rax),%r9d
	jmp	.L6
.L163:	movl	$10,236(%rsp)
	jmp	.L8
	.p2align 4,,10
	.p2align 3
.L14:	test	%r9d,%r9d
	je	.L1
	test	%eax,%eax
	je	.L1
	movslq	16(%rsp),%rcx
	mov	$0,%edx
	pxor	%xmm3,%xmm3
	test	%rcx,%rcx
	cmovns	%rcx,%rdx
	mov	24(%rsp),%rcx
	movsd	(%rcx),%xmm2
	mov	%rdx,48(%rsp)
	ucomisd	%xmm3,%xmm2
	jp	.L119
	jne	.L119
	mov	336(%rsp),%rbx
	movsd	(%rbx),%xmm1
	ucomisd	.LC4(%rip),%xmm1
	jp	.L100
	je	.L1
.L100:	ucomisd	%xmm3,%xmm1
	jnp	.L164
.L21:	mov	48(%rsp),%rbx
	mov	%r9d,%r8d
	mov	%r9d,%r10d
	mov	%r9d,%r11d
	shrl	%r8d
	mov	%rbp,%rsi
	add	$1,%eax
	and	$-2,%r10d
	lea	0(,%rbx,8),%r12
	sal	$4,%r8
	or	$1,%r11d
	mov	$-1,%rcx
	mov	$1,%edi
	movddup	%xmm1,%xmm2
	.p2align 4,,10
	.p2align 3
.L28:	cmp	$1,%r9d
	je	.L106
	lea	(%rsi,%r8),%r13
	mov	%rsi,%rdx
	.p2align 4,,10
	.p2align 3
.L27:	movupd	(%rdx),%xmm0
	add	$16,%rdx
	mulpd	%xmm2,%xmm0
	movups	%xmm0,-16(%rdx)
	cmp	%rdx,%r13
	jne	.L27
	movslq	%r11d,%rdx
	cmp	%r9d,%r10d
	je	.L29
.L25:	add	%rcx,%rdx
	lea	0(%rbp,%rdx,8),%rdx
	movsd	(%rdx),%xmm0
	mulsd	%xmm1,%xmm0
	movsd	%xmm0,(%rdx)
.L29:	add	$1,%edi
	add	%rbx,%rcx
	add	%r12,%rsi
	cmp	%edi,%eax
	jne	.L28
	jmp	.L1
.L119:	test	%r13d,%r13d
	jne	.L20
	mov	336(%rsp),%rcx
	movsd	.LC4(%rip),%xmm0
	ucomisd	(%rcx),%xmm0
	jnp	.L165
.L20:	movslq	32(%rsp),%rcx
	xor	%edx,%edx
	test	%rcx,%rcx
	cmovs	%rdx,%rcx
	mov	%rcx,%rsi
	mov	%rcx,56(%rsp)
	notq	%rsi
	mov	%rsi,72(%rsp)
	movslq	40(%rsp),%rsi
	test	%rsi,%rsi
	cmovns	%rsi,%rdx
	mov	%rdx,%rsi
	mov	%rdx,(%rsp)
	mov	%rdx,%rdi
	notq	%rsi
	mov	%rsi,80(%rsp)
	test	%r8d,%r8d
	je	.L30
	test	%r12d,%r12d
	jne	.L166
	mov	48(%rsp),%rcx
	add	$1,%eax
	lea	-1(%r9),%edx
	mov	%r13d,%r8d
	mov	%eax,32(%rsp)
	lea	8(%rbp,%rdx,8),%r10
	notq	%rdx
	shrl	%r8d
	sal	$3,%rcx
	mov	$1,%r14d
	mov	%r13d,%ebp
	mov	%r13d,%r12d
	mov	%rcx,16(%rsp)
	mov	56(%rsp),%rcx
	mov	%r11,%rdi
	and	$-2,%ebp
	mov	336(%rsp),%rax
	sal	$4,%r8
	or	$1,%r12d
	lea	8(%rbx),%r15
	sal	$3,%rcx
	mov	%r14d,8(%rsp)
	mov	(%rsp),%r14
	mov	$-1,%rsi
	movsd	(%rax),%xmm5
	lea	0(,%rdx,8),%rax
	mov	%rcx,24(%rsp)
	mov	%rax,40(%rsp)
	.p2align 4,,10
	.p2align 3
.L64:	mov	40(%rsp),%rax
	mov	$-1,%rcx
	lea	(%rax,%r10),%r9
	.p2align 4,,10
	.p2align 3
.L63:	test	%r13d,%r13d
	je	.L111
	cmp	$1,%r13d
	je	.L112
	lea	(%r15,%rcx,8),%rdx
	xor	%eax,%eax
	pxor	%xmm4,%xmm4
	.p2align 4,,10
	.p2align 3
.L58:	movupd	(%rdx,%rax),%xmm0
	movupd	(%rdi,%rax),%xmm7
	add	$16,%rax
	mulpd	%xmm7,%xmm0
	addpd	%xmm0,%xmm4
	cmp	%r8,%rax
	jne	.L58
	movapd	%xmm4,%xmm1
	movslq	%r12d,%rax
	unpckhpd %xmm4,%xmm1
	addpd	%xmm4,%xmm1
	cmp	%ebp,%r13d
	je	.L55
.L56:	lea	(%rcx,%rax),%rdx
	add	%rsi,%rax
	movsd	(%rbx,%rdx,8),%xmm0
	mulsd	(%r11,%rax,8),%xmm0
	addsd	%xmm0,%xmm1
.L55:	mulsd	%xmm2,%xmm1
	ucomisd	%xmm3,%xmm5
	jp	.L60
	jne	.L60
.L157:	movsd	%xmm1,(%r9)
	add	$8,%r9
	add	%r14,%rcx
	cmp	%r10,%r9
	jne	.L63
	addl	$1,8(%rsp)
	add	16(%rsp),%r10
	mov	8(%rsp),%eax
	add	56(%rsp),%rsi
	add	24(%rsp),%rdi
	cmp	32(%rsp),%eax
	jne	.L64
	jmp	.L1
	.p2align 4,,10
	.p2align 3
.L60:	movsd	(%r9),%xmm0
	mulsd	%xmm5,%xmm0
	addsd	%xmm0,%xmm1
	jmp	.L157
	.p2align 4,,10
	.p2align 3
.L111:	movapd	%xmm3,%xmm1
	jmp	.L55
.L112:	mov	$1,%eax
	movapd	%xmm3,%xmm1
	jmp	.L56
.L30:	test	%r12d,%r12d
	jne	.L167
	sub	$1,%eax
	lea	-1(%r9),%esi
	mov	48(%rsp),%rcx
	movq	$1,64(%rsp)
	add	$2,%rax
	lea	8(%rbp,%rsi,8),%r14
	mov	56(%rsp),%rbp
	mov	%r11,16(%rsp)
	mov	%rax,88(%rsp)
	mov	336(%rsp),%rax
	sal	$3,%rcx
	mov	%rcx,80(%rsp)
	mov	%rbp,%rcx
	lea	0(,%rbp,8),%rdx
	movsd	(%rax),%xmm4
	mov	(%rsp),%rax
	sal	$4,%rcx
	sal	$3,%rax
	mov	%rax,8(%rsp)
	lea	-1(%r13),%eax
	mov	%eax,24(%rsp)
	mov	%r13d,%eax
	shrl	%eax
	sal	$4,%rax
	mov	%rax,40(%rsp)
	mov	%r13d,%eax
	and	$-2,%eax
	mov	%eax,32(%rsp)
	mov	%r13d,%eax
	or	$1,%eax
	mov	%eax,56(%rsp)
	mov	%rsi,%rax
	notq	%rax
	sal	$3,%rax
	mov	%rax,96(%rsp)
	.p2align 4,,10
	.p2align 3
.L98:	mov	96(%rsp),%rax
	mov	72(%rsp),%rdi
	mov	%rbx,%r10
	mov	$-1,%r9
	lea	(%rax,%r14),%rsi
	mov	64(%rsp),%rax
	mov	%rax,%r12
	add	%rbp,%rax
	add	%rdi,%rax
	add	%rdi,%r12
	mov	%rax,48(%rsp)
	.p2align 4,,10
	.p2align 3
.L97:	test	%r13d,%r13d
	je	.L117
	cmpl	$2,24(%rsp)
	jbe	.L118
	mov	40(%rsp),%rax
	mov	16(%rsp),%rdi
	pxor	%xmm1,%xmm1
	lea	(%rax,%r10),%r8
	mov	%r10,%rax
	.p2align 4,,10
	.p2align 3
.L92:	movsd	(%rdi),%xmm0
	movupd	(%rax),%xmm6
	add	$16,%rax
	movhpd	(%rdi,%rdx),%xmm0
	add	%rcx,%rdi
	mulpd	%xmm6,%xmm0
	addpd	%xmm0,%xmm1
	cmp	%r8,%rax
	jne	.L92
	movapd	%xmm1,%xmm0
	unpckhpd %xmm1,%xmm0
	addpd	%xmm0,%xmm1
	cmp	32(%rsp),%r13d
	je	.L89
	mov	56(%rsp),%eax
.L90:	movslq	%eax,%r8
	mov	%rbp,%rdi
	imul	%r8,%rdi
	add	%r9,%r8
	lea	(%r12,%rdi),%r15
	movsd	(%r11,%r15,8),%xmm0
	mulsd	(%rbx,%r8,8),%xmm0
	lea	1(%rax),%r8d
	addsd	%xmm0,%xmm1
	cmp	%r8d,%r13d
	jl	.L89
	add	%rbp,%rdi
	movslq	%r8d,%r8
	add	$2,%eax
	lea	(%r12,%rdi),%r15
	add	%r9,%r8
	movsd	(%r11,%r15,8),%xmm0
	mulsd	(%rbx,%r8,8),%xmm0
	addsd	%xmm0,%xmm1
	cmp	%eax,%r13d
	jl	.L89
	cltq
	add	48(%rsp),%rdi
	add	%r9,%rax
	movsd	(%r11,%rdi,8),%xmm0
	mulsd	(%rbx,%rax,8),%xmm0
	addsd	%xmm0,%xmm1
.L89:	mulsd	%xmm2,%xmm1
	ucomisd	%xmm3,%xmm4
	jp	.L94
	jne	.L94
.L158:	movsd	%xmm1,(%rsi)
	add	$8,%rsi
	add	(%rsp),%r9
	add	8(%rsp),%r10
	cmp	%r14,%rsi
	jne	.L97
	addq	$1,64(%rsp)
	add	80(%rsp),%r14
	addq	$8,16(%rsp)
	mov	64(%rsp),%rax
	cmp	88(%rsp),%rax
	jne	.L98
	jmp	.L1
	.p2align 4,,10
	.p2align 3
.L94:	movsd	(%rsi),%xmm0
	mulsd	%xmm4,%xmm0
	addsd	%xmm0,%xmm1
	jmp	.L158
	.p2align 4,,10
	.p2align 3
.L117:	movapd	%xmm3,%xmm1
	jmp	.L89
.L118:	movapd	%xmm3,%xmm1
	mov	$1,%eax
	jmp	.L90
.L164:	jne	.L21
	mov	48(%rsp),%r12
	lea	1(%rax),%r14d
	lea	-1(%r9),%eax
	mov	$1,%ebx
	lea	8(,%rax,8),%r13
	sal	$3,%r12
	.p2align 4,,10
	.p2align 3
.L24:	mov	%rbp,%rdi
	mov	%r13,%rdx
	xor	%esi,%esi
	add	$1,%ebx
	call	memset
	add	%r12,%rbp
	cmp	%ebx,%r14d
	jne	.L24
	jmp	.L1
.L167:	sub	$1,%eax
	mov	72(%rsp),%rsi
	mov	%r9d,%r15d
	mov	%rbp,%r12
	add	$2,%rax
	mov	336(%rsp),%rcx
	movq	$1,24(%rsp)
	shrl	%r15d
	mov	%rax,64(%rsp)
	mov	%r9d,%eax
	sal	$4,%r15
	movsd	.LC4(%rip),%xmm7
	and	$-2,%eax
	movsd	(%rcx),%xmm5
	mov	48(%rsp),%rcx
	mov	%r11,320(%rsp)
	mov	%eax,8(%rsp)
	mov	%r9d,%eax
	mov	$-1,%r14
	or	$1,%eax
	sal	$3,%rcx
	movddup	%xmm5,%xmm6
	mov	%eax,16(%rsp)
	lea	-1(%r9),%eax
	mov	%rcx,40(%rsp)
	mov	80(%rsp),%rcx
	mov	%eax,88(%rsp)
	lea	8(,%rax,8),%rax
	mov	%rax,208(%rsp)
	lea	(%rdx,%rdx),%rax
	add	%rax,%rcx
	mov	%rax,96(%rsp)
	mov	56(%rsp),%rax
	mov	%rcx,128(%rsp)
	mov	%rax,%rcx
	sal	$4,%rcx
	mov	%rcx,112(%rsp)
	mov	%rax,%rcx
	lea	(%rsi,%rax,2),%rax
	lea	(%r11,%rax,8),%rax
	mov	%rax,144(%rsp)
	mov	%rdx,%rax
	sal	$4,%rax
	mov	%rax,136(%rsp)
	mov	%rcx,%rax
	negq	%rax
	sal	$3,%rax
	mov	%rax,152(%rsp)
	lea	0(,%rdx,8),%rax
	mov	%rax,168(%rsp)
	mov	%r9d,%eax
	shr	$2,%eax
	sal	$5,%rax
	mov	%rax,104(%rsp)
	mov	%r9d,%eax
	and	$-4,%eax
	mov	%eax,200(%rsp)
	add	$1,%eax
	mov	%eax,192(%rsp)
	lea	0(,%rcx,8),%rax
	mov	%r13d,%ecx
	mov	%r9d,%r13d
	mov	%rax,216(%rsp)
	.p2align 4,,10
	.p2align 3
.L77:	ucomisd	%xmm3,%xmm5
	jnp	.L168
.L68:	ucomisd	%xmm7,%xmm5
	jp	.L123
	jne	.L123
.L71:	test	%ecx,%ecx
	je	.L69
	cmp	$2,%ecx
	jle	.L114
	mov	144(%rsp),%rax
	mov	24(%rsp),%rdi
	mov	%r15,160(%rsp)
	mov	$-1,%r10
	mov	128(%rsp),%rsi
	lea	(%rax,%rdi,8),%r11
	mov	104(%rsp),%rdi
	mov	%rbx,%rax
	mov	%rsi,32(%rsp)
	mov	$2,%esi
	mov	%esi,%r15d
.L82:	mov	152(%rsp),%rsi
	cmpl	$2,88(%rsp)
	movsd	(%r11,%rsi),%xmm10
	mulsd	%xmm2,%xmm10
	jbe	.L169
	movsd	(%r11),%xmm11
	mov	168(%rsp),%rsi
	movddup	%xmm10,%xmm9
	xor	%edx,%edx
	mulsd	%xmm2,%xmm11
	lea	(%rax,%rsi),%r8
	mov	%r12,%rsi
	movddup	%xmm11,%xmm8
	.p2align 4,,10
	.p2align 3
.L88:	movupd	16(%rax,%rdx),%xmm0
	movupd	16(%r8,%rdx),%xmm1
	add	$32,%rsi
	movupd	-16(%rsi),%xmm4
	mulpd	%xmm9,%xmm0
	mulpd	%xmm8,%xmm1
	addpd	%xmm4,%xmm0
	movupd	-32(%rsi),%xmm4
	addpd	%xmm1,%xmm0
	movupd	(%rax,%rdx),%xmm1
	mulpd	%xmm9,%xmm1
	movups	%xmm0,-16(%rsi)
	addpd	%xmm4,%xmm1
	movupd	(%r8,%rdx),%xmm4
	add	$32,%rdx
	mulpd	%xmm8,%xmm4
	addpd	%xmm4,%xmm1
	movups	%xmm1,-32(%rsi)
	cmp	%rdi,%rdx
	jne	.L88
	cmp	%r13d,200(%rsp)
	je	.L81
	mov	192(%rsp),%edx
.L79:	movslq	%edx,%rsi
	lea	(%r14,%rsi),%r8
	lea	(%r10,%rsi),%r9
	add	32(%rsp),%rsi
	movsd	(%rbx,%rsi,8),%xmm1
	movsd	(%rbx,%r9,8),%xmm0
	lea	0(%rbp,%r8,8),%r8
	lea	1(%rdx),%esi
	mulsd	%xmm11,%xmm1
	mulsd	%xmm10,%xmm0
	addsd	(%r8),%xmm0
	addsd	%xmm1,%xmm0
	movsd	%xmm0,(%r8)
	cmp	%esi,%r13d
	jl	.L81
	movslq	%esi,%rsi
	add	$2,%edx
	lea	(%r10,%rsi),%r9
	lea	(%r14,%rsi),%r8
	movsd	(%rbx,%r9,8),%xmm0
	mov	32(%rsp),%r9
	lea	0(%rbp,%r8,8),%r8
	mulsd	%xmm10,%xmm0
	add	%r9,%rsi
	movsd	(%rbx,%rsi,8),%xmm1
	addsd	(%r8),%xmm0
	mulsd	%xmm11,%xmm1
	addsd	%xmm1,%xmm0
	movsd	%xmm0,(%r8)
	cmp	%edx,%r13d
	jl	.L81
	movslq	%edx,%rdx
	lea	(%r14,%rdx),%rsi
	lea	(%r10,%rdx),%r8
	add	%r9,%rdx
	mulsd	(%rbx,%rdx,8),%xmm11
	lea	0(%rbp,%rsi,8),%rsi
	mulsd	(%rbx,%r8,8),%xmm10
	addsd	(%rsi),%xmm10
	addsd	%xmm10,%xmm11
	movsd	%xmm11,(%rsi)
.L81:	mov	96(%rsp),%rdx
	lea	1(%r15),%esi
	add	$2,%r15d
	add	%rdx,32(%rsp)
	add	112(%rsp),%r11
	add	136(%rsp),%rax
	add	%rdx,%r10
	cmp	%r15d,%ecx
	jg	.L82
	mov	160(%rsp),%r15
.L78:	mov	56(%rsp),%rdi
	movslq	%esi,%rdx
	mov	(%rsp),%r8
	lea	8(%rbx),%r10
	mov	72(%rsp),%rax
	add	24(%rsp),%rax
	imul	%rdx,%rdi
	mov	216(%rsp),%r9
	imul	%r8,%rdx
	add	80(%rsp),%rdx
	add	%rdi,%rax
	mov	320(%rsp),%rdi
	lea	(%rdi,%rax,8),%rdi
	.p2align 4,,10
	.p2align 3
.L86:	movsd	(%rdi),%xmm0
	mulsd	%xmm2,%xmm0
	cmp	$1,%r13d
	je	.L116
	lea	(%r10,%rdx,8),%r11
	movddup	%xmm0,%xmm4
	xor	%eax,%eax
	.p2align 4,,10
	.p2align 3
.L84:	movupd	(%r11,%rax),%xmm1
	movupd	(%r12,%rax),%xmm13
	mulpd	%xmm4,%xmm1
	addpd	%xmm13,%xmm1
	movups	%xmm1,(%r12,%rax)
	add	$16,%rax
	cmp	%r15,%rax
	jne	.L84
	cmp	8(%rsp),%r13d
	je	.L85
	movslq	16(%rsp),%rax
.L83:	lea	(%r14,%rax),%r11
	add	%rdx,%rax
	lea	0(%rbp,%r11,8),%r11
	mulsd	(%rbx,%rax,8),%xmm0
	addsd	(%r11),%xmm0
	movsd	%xmm0,(%r11)
.L85:	add	$1,%esi
	add	%r9,%rdi
	add	%r8,%rdx
	cmp	%esi,%ecx
	jge	.L86
.L69:	addq	$1,24(%rsp)
	add	48(%rsp),%r14
	mov	24(%rsp),%rax
	add	40(%rsp),%r12
	cmp	64(%rsp),%rax
	jne	.L77
	jmp	.L1
.L116:	mov	$1,%eax
	jmp	.L83
.L168:	jne	.L68
	mov	208(%rsp),%rdx
	xor	%esi,%esi
	mov	%r12,%rdi
	mov	%ecx,204(%rsp)
	movaps	%xmm6,176(%rsp)
	movsd	%xmm5,160(%rsp)
	movsd	%xmm2,32(%rsp)
	call	memset
	mov	.LC4(%rip),%rax
	movsd	32(%rsp),%xmm2
	pxor	%xmm3,%xmm3
	movsd	160(%rsp),%xmm5
	mov	204(%rsp),%ecx
	movapd	176(%rsp),%xmm6
	movq	%rax,%xmm7
	jmp	.L71
.L123:	cmp	$1,%r13d
	je	.L113
	lea	(%r15,%r12),%rdx
	mov	%r12,%rax
	.p2align 4,,10
	.p2align 3
.L75:	movupd	(%rax),%xmm0
	add	$16,%rax
	mulpd	%xmm6,%xmm0
	movups	%xmm0,-16(%rax)
	cmp	%rdx,%rax
	jne	.L75
	cmp	8(%rsp),%r13d
	je	.L71
	movslq	16(%rsp),%rax
.L73:	add	%r14,%rax
	lea	0(%rbp,%rax,8),%rax
	movsd	(%rax),%xmm0
	mulsd	%xmm5,%xmm0
	movsd	%xmm0,(%rax)
	jmp	.L71
.L166:	add	$1,%eax
	mov	%r9d,%r15d
	sal	$3,%rcx
	mov	%r9d,%r8d
	mov	%eax,32(%rsp)
	mov	%r9d,%eax
	shrl	%r15d
	mov	%rbp,%r12
	or	$1,%eax
	mov	%rcx,40(%rsp)
	sub	$8,%r11
	mov	%r13d,%ecx
	mov	%eax,8(%rsp)
	lea	-1(%r9),%eax
	sal	$4,%r15
	and	$-2,%r8d
	mov	%eax,96(%rsp)
	lea	8(,%rax,8),%rax
	mov	336(%rsp),%rdx
	mov	$-1,%r14
	mov	%rax,192(%rsp)
	lea	(%rdi,%rdi),%rax
	movsd	.LC4(%rip),%xmm7
	mov	%r9d,%r13d
	add	%rax,%rsi
	mov	%rax,88(%rsp)
	mov	%rdi,%rax
	movsd	(%rdx),%xmm5
	mov	48(%rsp),%rdx
	sal	$4,%rdi
	lea	0(,%rax,8),%rax
	mov	%rsi,72(%rsp)
	mov	%rax,152(%rsp)
	mov	%r9d,%eax
	movddup	%xmm5,%xmm6
	shr	$2,%eax
	sal	$3,%rdx
	mov	%rdi,104(%rsp)
	sal	$5,%rax
	mov	%rdx,56(%rsp)
	mov	%rax,64(%rsp)
	mov	%r9d,%eax
	and	$-4,%eax
	movl	$1,16(%rsp)
	mov	%eax,144(%rsp)
	add	$1,%eax
	mov	%eax,160(%rsp)
	.p2align 4,,10
	.p2align 3
.L43:	ucomisd	%xmm3,%xmm5
	jnp	.L170
.L34:	ucomisd	%xmm7,%xmm5
	jp	.L121
	jne	.L121
.L37:	test	%ecx,%ecx
	je	.L35
	cmp	$2,%ecx
	jle	.L108
	mov	72(%rsp),%rdi
	mov	%r15,128(%rsp)
	lea	16(%r11),%rsi
	mov	%rbx,%rax
	mov	$2,%edx
	mov	%r8d,112(%rsp)
	mov	$-1,%r10
	mov	%rdi,24(%rsp)
	mov	64(%rsp),%rdi
	mov	%edx,%r8d
	mov	%r11,136(%rsp)
	mov	%rsi,%r11
.L48:	movsd	-8(%r11),%xmm10
	cmpl	$2,96(%rsp)
	mulsd	%xmm2,%xmm10
	jbe	.L171
	movsd	(%r11),%xmm11
	mov	152(%rsp),%rsi
	movddup	%xmm10,%xmm9
	xor	%edx,%edx
	mulsd	%xmm2,%xmm11
	lea	(%rsi,%rax),%r9
	mov	%r12,%rsi
	movddup	%xmm11,%xmm8
	.p2align 4,,10
	.p2align 3
.L54:	movupd	16(%rax,%rdx),%xmm0
	movupd	16(%r9,%rdx),%xmm1
	add	$32,%rsi
	movupd	-16(%rsi),%xmm4
	mulpd	%xmm9,%xmm0
	mulpd	%xmm8,%xmm1
	addpd	%xmm4,%xmm0
	movupd	-32(%rsi),%xmm4
	addpd	%xmm1,%xmm0
	movupd	(%rax,%rdx),%xmm1
	mulpd	%xmm9,%xmm1
	movups	%xmm0,-16(%rsi)
	addpd	%xmm4,%xmm1
	movupd	(%r9,%rdx),%xmm4
	add	$32,%rdx
	mulpd	%xmm8,%xmm4
	addpd	%xmm4,%xmm1
	movups	%xmm1,-32(%rsi)
	cmp	%rdi,%rdx
	jne	.L54
	cmp	%r13d,144(%rsp)
	je	.L47
	mov	160(%rsp),%edx
.L45:	movslq	%edx,%rsi
	lea	(%r14,%rsi),%r9
	lea	(%r10,%rsi),%r15
	add	24(%rsp),%rsi
	movsd	(%rbx,%rsi,8),%xmm1
	movsd	(%rbx,%r15,8),%xmm0
	lea	0(%rbp,%r9,8),%r9
	lea	1(%rdx),%esi
	mulsd	%xmm11,%xmm1
	mulsd	%xmm10,%xmm0
	addsd	(%r9),%xmm0
	addsd	%xmm1,%xmm0
	movsd	%xmm0,(%r9)
	cmp	%esi,%r13d
	jl	.L47
	movslq	%esi,%rsi
	add	$2,%edx
	lea	(%r10,%rsi),%r15
	lea	(%r14,%rsi),%r9
	movsd	(%rbx,%r15,8),%xmm0
	mov	24(%rsp),%r15
	lea	0(%rbp,%r9,8),%r9
	mulsd	%xmm10,%xmm0
	add	%r15,%rsi
	movsd	(%rbx,%rsi,8),%xmm1
	addsd	(%r9),%xmm0
	mulsd	%xmm11,%xmm1
	addsd	%xmm1,%xmm0
	movsd	%xmm0,(%r9)
	cmp	%edx,%r13d
	jl	.L47
	movslq	%edx,%rdx
	lea	(%r14,%rdx),%rsi
	lea	(%r10,%rdx),%r9
	add	%r15,%rdx
	mulsd	(%rbx,%rdx,8),%xmm11
	lea	0(%rbp,%rsi,8),%rsi
	mulsd	(%rbx,%r9,8),%xmm10
	addsd	(%rsi),%xmm10
	addsd	%xmm10,%xmm11
	movsd	%xmm11,(%rsi)
.L47:	mov	88(%rsp),%rsi
	lea	1(%r8),%edx
	add	$2,%r8d
	add	%rsi,24(%rsp)
	add	104(%rsp),%rax
	add	$16,%r11
	add	%rsi,%r10
	cmp	%r8d,%ecx
	jg	.L48
	mov	128(%rsp),%r15
	mov	112(%rsp),%r8d
	mov	136(%rsp),%r11
.L44:	mov	(%rsp),%r9
	movslq	%edx,%rdx
	lea	8(%rbx),%rdi
	mov	%r9,%rsi
	imul	%rdx,%rsi
	add	80(%rsp),%rsi
	.p2align 4,,10
	.p2align 3
.L52:	movsd	(%r11,%rdx,8),%xmm0
	mulsd	%xmm2,%xmm0
	cmp	$1,%r13d
	je	.L110
	lea	(%rdi,%rsi,8),%r10
	movddup	%xmm0,%xmm4
	xor	%eax,%eax
	.p2align 4,,10
	.p2align 3
.L50:	movupd	(%r10,%rax),%xmm1
	movupd	(%r12,%rax),%xmm12
	mulpd	%xmm4,%xmm1
	addpd	%xmm12,%xmm1
	movups	%xmm1,(%r12,%rax)
	add	$16,%rax
	cmp	%rax,%r15
	jne	.L50
	cmp	%r8d,%r13d
	je	.L51
	movslq	8(%rsp),%rax
.L49:	lea	(%r14,%rax),%r10
	add	%rsi,%rax
	lea	0(%rbp,%r10,8),%r10
	mulsd	(%rbx,%rax,8),%xmm0
	addsd	(%r10),%xmm0
	movsd	%xmm0,(%r10)
.L51:	add	$1,%rdx
	add	%r9,%rsi
	cmp	%edx,%ecx
	jge	.L52
.L35:	addl	$1,16(%rsp)
	add	48(%rsp),%r14
	mov	16(%rsp),%eax
	add	56(%rsp),%r12
	add	40(%rsp),%r11
	cmp	32(%rsp),%eax
	jne	.L43
	jmp	.L1
.L110:	mov	$1,%eax
	jmp	.L49
.L170:	jne	.L34
	mov	192(%rsp),%rdx
	xor	%esi,%esi
	mov	%r12,%rdi
	mov	%r11,168(%rsp)
	mov	%r8d,200(%rsp)
	mov	%ecx,136(%rsp)
	movaps	%xmm6,112(%rsp)
	movsd	%xmm5,128(%rsp)
	movsd	%xmm2,24(%rsp)
	call	memset
	mov	.LC4(%rip),%rax
	pxor	%xmm3,%xmm3
	movsd	24(%rsp),%xmm2
	movsd	128(%rsp),%xmm5
	movapd	112(%rsp),%xmm6
	mov	136(%rsp),%ecx
	mov	200(%rsp),%r8d
	movq	%rax,%xmm7
	mov	168(%rsp),%r11
	jmp	.L37
.L121:	cmp	$1,%r13d
	je	.L107
	lea	(%r15,%r12),%rdx
	mov	%r12,%rax
	.p2align 4,,10
	.p2align 3
.L41:	movupd	(%rax),%xmm0
	add	$16,%rax
	mulpd	%xmm6,%xmm0
	movups	%xmm0,-16(%rax)
	cmp	%rax,%rdx
	jne	.L41
	cmp	%r8d,%r13d
	je	.L37
	movslq	8(%rsp),%rax
.L39:	add	%r14,%rax
	lea	0(%rbp,%rax,8),%rax
	movsd	(%rax),%xmm0
	mulsd	%xmm5,%xmm0
	movsd	%xmm0,(%rax)
	jmp	.L37
.L108:	mov	$1,%edx
	jmp	.L44
.L114:	mov	$1,%esi
	jmp	.L78
.L106:	mov	$1,%edx
	jmp	.L25
.L171:	movsd	(%r11),%xmm11
	mov	$1,%edx
	mulsd	%xmm2,%xmm11
	jmp	.L45
.L169:	movsd	(%r11),%xmm11
	mov	$1,%edx
	mulsd	%xmm2,%xmm11
	jmp	.L79
.L107:	mov	$1,%eax
	jmp	.L39
.L113:	mov	$1,%eax
	jmp	.L73
.L165:	je	.L1
	jmp	.L20
	.endfn	dgemm_,globl

	.rodata.cst8
.LC4:	.double	1

	.rodata.str1.1
.LC1:	.string	"N"
.LC2:	.string	"C"
.LC3:	.string	"T"
.LC5:	.string	"DGEMM "
