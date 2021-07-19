/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/macros.internal.h"

/	Computes 1024-bit product of 512-bit and 512-bit numbers.
/
/		Instructions:      262
/		Total Cycles:      114
/		Total uOps:        469
/		Dispatch Width:    6
/		uOps Per Cycle:    4.11
/		IPC:               2.30
/		Block RThroughput: 78.2
/
/	@param	rdi receives 16 quadword result
/	@param	rsi is left hand side which must have 8 quadwords
/	@param	rdx is right hand side which must have 8 quadwords
/	@note	words are host endian while array is little endian
/	@mayalias
Mul8x8Adx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	push	%r15
	push	%r14
	push	%r13
	push	%r12
	mov	%rdx,%r12
	push	%rbx
	sub	$64,%rsp
	mov	(%rdx),%rdx
	xor	%r13d,%r13d
	mulx	(%rsi),%rax,%rcx
	mov	%rdi,-48(%rbp)
	mov	%rax,-56(%rbp)
	mulx	8(%rsi),%rdx,%rax
	adox	%rdx,%rcx
	mov	(%r12),%rdx
	mulx	16(%rsi),%rdx,%rbx
	adox	%rdx,%rax
	mov	(%r12),%rdx
	mulx	24(%rsi),%rdx,%r11
	adox	%rdx,%rbx
	mov	(%r12),%rdx
	mulx	32(%rsi),%rdx,%r10
	adox	%rdx,%r11
	mov	(%r12),%rdx
	mulx	40(%rsi),%rdx,%r9
	adox	%rdx,%r10
	mov	(%r12),%rdx
	mulx	48(%rsi),%rdx,%r8
	adox	%rdx,%r9
	mov	(%r12),%rdx
	mulx	56(%rsi),%rdx,%rdi
	adox	%rdx,%r8
	adox	%r13,%rdi
	xor	%r13d,%r13d
	mov	8(%r12),%rdx
	mulx	(%rsi),%r15,%r14
	adox	%r15,%rcx
	adcx	%r14,%rax
	mov	%rcx,-64(%rbp)
	mulx	8(%rsi),%r14,%rcx
	adox	%r14,%rax
	adcx	%rcx,%rbx
	mulx	16(%rsi),%r14,%rcx
	adox	%r14,%rbx
	adcx	%rcx,%r11
	mulx	24(%rsi),%r14,%rcx
	adox	%r14,%r11
	adcx	%rcx,%r10
	mulx	32(%rsi),%r14,%rcx
	adox	%r14,%r10
	adcx	%rcx,%r9
	mulx	40(%rsi),%r14,%rcx
	adox	%r14,%r9
	adcx	%rcx,%r8
	mulx	48(%rsi),%r14,%rcx
	adox	%r14,%r8
	adcx	%rcx,%rdi
	mulx	56(%rsi),%rdx,%rcx
	adox	%rdx,%rdi
	adcx	%r13,%rcx
	mov	16(%r12),%rdx
	adox	%r13,%rcx
	mulx	(%rsi),%r15,%r14
	xor	%r13d,%r13d
	adox	%r15,%rax
	adcx	%r14,%rbx
	mov	%rax,-72(%rbp)
	mulx	8(%rsi),%r14,%rax
	adox	%r14,%rbx
	adcx	%rax,%r11
	mulx	16(%rsi),%r14,%rax
	adox	%r14,%r11
	adcx	%rax,%r10
	mulx	24(%rsi),%r14,%rax
	adox	%r14,%r10
	adcx	%rax,%r9
	mulx	32(%rsi),%r14,%rax
	adox	%r14,%r9
	adcx	%rax,%r8
	mulx	40(%rsi),%r14,%rax
	adox	%r14,%r8
	adcx	%rax,%rdi
	mulx	48(%rsi),%r14,%rax
	adox	%r14,%rdi
	adcx	%rax,%rcx
	mulx	56(%rsi),%rdx,%rax
	adox	%rdx,%rcx
	adcx	%r13,%rax
	adox	%r13,%rax
	xor	%r13d,%r13d
	mov	24(%r12),%rdx
	mulx	(%rsi),%r15,%r14
	adox	%r15,%rbx
	adcx	%r14,%r11
	mov	%rbx,-80(%rbp)
	mov	%r11,%r15
	mulx	8(%rsi),%r14,%rbx
	adox	%r14,%r15
	adcx	%rbx,%r10
	mulx	16(%rsi),%rbx,%r11
	adox	%rbx,%r10
	adcx	%r11,%r9
	mulx	24(%rsi),%rbx,%r11
	adox	%rbx,%r9
	adcx	%r11,%r8
	mulx	32(%rsi),%rbx,%r11
	adox	%rbx,%r8
	adcx	%r11,%rdi
	mulx	40(%rsi),%rbx,%r11
	adox	%rbx,%rdi
	adcx	%r11,%rcx
	mulx	48(%rsi),%rbx,%r11
	adox	%rbx,%rcx
	adcx	%r11,%rax
	mulx	56(%rsi),%rdx,%r11
	adox	%rdx,%rax
	adcx	%r13,%r11
	mov	32(%r12),%rdx
	adox	%r13,%r11
	xor	%ebx,%ebx
	mulx	(%rsi),%r14,%r13
	adox	%r14,%r15
	adcx	%r13,%r10
	mov	%r15,-88(%rbp)
	mulx	8(%rsi),%r14,%r13
	mov	%r10,%r15
	adcx	%r13,%r9
	adox	%r14,%r15
	mulx	16(%rsi),%r13,%r10
	adox	%r13,%r9
	adcx	%r10,%r8
	mulx	24(%rsi),%r13,%r10
	adcx	%r10,%rdi
	adox	%r13,%r8
	mulx	32(%rsi),%r13,%r10
	adox	%r13,%rdi
	adcx	%r10,%rcx
	mulx	40(%rsi),%r13,%r10
	adox	%r13,%rcx
	adcx	%r10,%rax
	mulx	48(%rsi),%r13,%r10
	adox	%r13,%rax
	adcx	%r10,%r11
	mulx	56(%rsi),%rdx,%r10
	adox	%rdx,%r11
	adcx	%rbx,%r10
	mov	40(%r12),%rdx
	adox	%rbx,%r10
	mulx	(%rsi),%r14,%r13
	xor	%ebx,%ebx
	adox	%r14,%r15
	mov	%r15,-96(%rbp)
	adcx	%r13,%r9
	mulx	8(%rsi),%r14,%r13
	mov	%r9,%r15
	adox	%r14,%r15
	adcx	%r13,%r8
	mulx	16(%rsi),%r13,%r9
	adox	%r13,%r8
	adcx	%r9,%rdi
	mulx	24(%rsi),%r13,%r9
	adox	%r13,%rdi
	adcx	%r9,%rcx
	mulx	32(%rsi),%r13,%r9
	adox	%r13,%rcx
	adcx	%r9,%rax
	mulx	40(%rsi),%r13,%r9
	adox	%r13,%rax
	adcx	%r9,%r11
	mulx	48(%rsi),%r13,%r9
	adox	%r13,%r11
	adcx	%r9,%r10
	mulx	56(%rsi),%rdx,%r9
	adox	%rdx,%r10
	adcx	%rbx,%r9
	adox	%rbx,%r9
	xor	%ebx,%ebx
	mov	48(%r12),%rdx
	mulx	(%rsi),%r14,%r13
	adox	%r14,%r15
	adcx	%r13,%r8
	mov	%r15,-104(%rbp)
	mulx	8(%rsi),%r14,%r13
	mov	%r8,%r15
	adcx	%r13,%rdi
	adox	%r14,%r15
	mulx	16(%rsi),%r13,%r8
	adox	%r13,%rdi
	adcx	%r8,%rcx
	mulx	24(%rsi),%r13,%r8
	adox	%r13,%rcx
	adcx	%r8,%rax
	mulx	32(%rsi),%r13,%r8
	adox	%r13,%rax
	adcx	%r8,%r11
	mulx	40(%rsi),%r13,%r8
	adox	%r13,%r11
	adcx	%r8,%r10
	mulx	48(%rsi),%r13,%r8
	adox	%r13,%r10
	adcx	%r8,%r9
	mulx	56(%rsi),%rdx,%r8
	adox	%rdx,%r9
	mov	56(%r12),%rdx
	adcx	%rbx,%r8
	mulx	(%rsi),%r13,%r12
	adox	%rbx,%r8
	xor	%ebx,%ebx
	adox	%r13,%r15
	adcx	%r12,%rdi
	mulx	8(%rsi),%r13,%r12
	adox	%r13,%rdi
	adcx	%r12,%rcx
	mulx	16(%rsi),%r13,%r12
	adox	%r13,%rcx
	adcx	%r12,%rax
	mulx	24(%rsi),%r13,%r12
	adox	%r13,%rax
	adcx	%r12,%r11
	mulx	32(%rsi),%r13,%r12
	adox	%r13,%r11
	adcx	%r12,%r10
	mulx	40(%rsi),%r13,%r12
	adox	%r13,%r10
	adcx	%r12,%r9
	mulx	48(%rsi),%r13,%r12
	mulx	56(%rsi),%rsi,%rdx
	adox	%r13,%r9
	adcx	%r12,%r8
	adox	%rsi,%r8
	adcx	%rbx,%rdx
	mov	-64(%rbp),%rsi
	adox	%rbx,%rdx
	mov	-48(%rbp),%rbx
	mov	-56(%rbp),%r14
	mov	%rsi,8(%rbx)
	mov	-72(%rbp),%rsi
	mov	%r14,(%rbx)
	mov	%rsi,16(%rbx)
	mov	-80(%rbp),%rsi
	mov	%rsi,24(%rbx)
	mov	-88(%rbp),%rsi
	mov	%rsi,32(%rbx)
	mov	-96(%rbp),%rsi
	mov	%rsi,40(%rbx)
	mov	-104(%rbp),%rsi
	mov	%r15,56(%rbx)
	mov	%rsi,48(%rbx)
	mov	%rdi,64(%rbx)
	mov	%rcx,72(%rbx)
	mov	%rax,80(%rbx)
	mov	%r11,88(%rbx)
	mov	%r10,96(%rbx)
	mov	%r9,104(%rbx)
	mov	%r8,112(%rbx)
	mov	%rdx,120(%rbx)
	add	$64,%rsp
	pop	%rbx
	pop	%r12
	pop	%r13
	pop	%r14
	pop	%r15
	pop	%rbp
	ret
	.endfn	Mul8x8Adx,globl

	.end
Timeline view:      0123456789          0123456789          0123456789          0123456789
Index     0123456789          0123456789          0123456789          0123456789
[0,0]     DeeER.    .    .    .    .    .    .    .    .    .    .    .    .    .    .   .   pushq	%r15
[0,1]     D==eeER   .    .    .    .    .    .    .    .    .    .    .    .    .    .   .   pushq	%r14
[0,2]     .D===eeER .    .    .    .    .    .    .    .    .    .    .    .    .    .   .   pushq	%r13
[0,3]     .D=====eeER    .    .    .    .    .    .    .    .    .    .    .    .    .   .   pushq	%r12
[0,4]     . DeE-----R    .    .    .    .    .    .    .    .    .    .    .    .    .   .   movq	%rdx, %r12
[0,5]     . D======eeER  .    .    .    .    .    .    .    .    .    .    .    .    .   .   pushq	%rbx
[0,6]     . D========eER .    .    .    .    .    .    .    .    .    .    .    .    .   .   subq	$64, %rsp
[0,7]     . DeeeeeE----R .    .    .    .    .    .    .    .    .    .    .    .    .   .   movq	(%rdx), %rdx
[0,8]     .  D---------R .    .    .    .    .    .    .    .    .    .    .    .    .   .   xorl	%r13d, %r13d
[0,9]     .  D====eeeeeeeeeER .    .    .    .    .    .    .    .    .    .    .    .   .   mulxq	(%rsi), %rax, %rcx
[0,10]    .  D======eE------R .    .    .    .    .    .    .    .    .    .    .    .   .   movq	%rdi, -48(%rbp)
[0,11]    .   D======eE-----R .    .    .    .    .    .    .    .    .    .    .    .   .   movq	%rax, -56(%rbp)
[0,12]    .   D====eeeeeeeeeER.    .    .    .    .    .    .    .    .    .    .    .   .   mulxq	8(%rsi), %rdx, %rax
[0,13]    .   D============eER.    .    .    .    .    .    .    .    .    .    .    .   .   adoxq	%rdx, %rcx
[0,14]    .    DeeeeeE-------R.    .    .    .    .    .    .    .    .    .    .    .   .   movq	(%r12), %rdx
[0,15]    .    D=====eeeeeeeeeER   .    .    .    .    .    .    .    .    .    .    .   .   mulxq	16(%rsi), %rdx, %rbx
[0,16]    .    D============eE-R   .    .    .    .    .    .    .    .    .    .    .   .   adoxq	%rdx, %rax
[0,17]    .    .DeeeeeE--------R   .    .    .    .    .    .    .    .    .    .    .   .   movq	(%r12), %rdx
[0,18]    .    .D=====eeeeeeeeeER  .    .    .    .    .    .    .    .    .    .    .   .   mulxq	24(%rsi), %rdx, %r11
[0,19]    .    .D=============eER  .    .    .    .    .    .    .    .    .    .    .   .   adoxq	%rdx, %rbx
[0,20]    .    . DeeeeeE--------R  .    .    .    .    .    .    .    .    .    .    .   .   movq	(%r12), %rdx
[0,21]    .    . D=====eeeeeeeeeER .    .    .    .    .    .    .    .    .    .    .   .   mulxq	32(%rsi), %rdx, %r10
[0,22]    .    . D=============eER .    .    .    .    .    .    .    .    .    .    .   .   adoxq	%rdx, %r11
[0,23]    .    .  DeeeeeE--------R .    .    .    .    .    .    .    .    .    .    .   .   movq	(%r12), %rdx
[0,24]    .    .  D=====eeeeeeeeeER.    .    .    .    .    .    .    .    .    .    .   .   mulxq	40(%rsi), %rdx, %r9
[0,25]    .    .  D=============eER.    .    .    .    .    .    .    .    .    .    .   .   adoxq	%rdx, %r10
[0,26]    .    .   DeeeeeE--------R.    .    .    .    .    .    .    .    .    .    .   .   movq	(%r12), %rdx
[0,27]    .    .   D=====eeeeeeeeeER    .    .    .    .    .    .    .    .    .    .   .   mulxq	48(%rsi), %rdx, %r8
[0,28]    .    .   D=============eER    .    .    .    .    .    .    .    .    .    .   .   adoxq	%rdx, %r9
[0,29]    .    .    DeeeeeE--------R    .    .    .    .    .    .    .    .    .    .   .   movq	(%r12), %rdx
[0,30]    .    .    D=====eeeeeeeeeER   .    .    .    .    .    .    .    .    .    .   .   mulxq	56(%rsi), %rdx, %rdi
[0,31]    .    .    D=============eER   .    .    .    .    .    .    .    .    .    .   .   adoxq	%rdx, %r8
[0,32]    .    .    .D=============eER  .    .    .    .    .    .    .    .    .    .   .   adoxq	%r13, %rdi
[0,33]    .    .    .D---------------R  .    .    .    .    .    .    .    .    .    .   .   xorl	%r13d, %r13d
[0,34]    .    .    .DeeeeeE---------R  .    .    .    .    .    .    .    .    .    .   .   movq	8(%r12), %rdx
[0,35]    .    .    . D====eeeeeeeeeER  .    .    .    .    .    .    .    .    .    .   .   mulxq	(%rsi), %r15, %r14
[0,36]    .    .    . D=======eE-----R  .    .    .    .    .    .    .    .    .    .   .   adoxq	%r15, %rcx
[0,37]    .    .    . D=============eER .    .    .    .    .    .    .    .    .    .   .   adcxq	%r14, %rax
[0,38]    .    .    .  D=======eE-----R .    .    .    .    .    .    .    .    .    .   .   movq	%rcx, -64(%rbp)
[0,39]    .    .    .  D====eeeeeeeeeER .    .    .    .    .    .    .    .    .    .   .   mulxq	8(%rsi), %r14, %rcx
[0,40]    .    .    .  D=============eER.    .    .    .    .    .    .    .    .    .   .   adoxq	%r14, %rax
[0,41]    .    .    .   D=============eER    .    .    .    .    .    .    .    .    .   .   adcxq	%rcx, %rbx
[0,42]    .    .    .   D====eeeeeeeeeE-R    .    .    .    .    .    .    .    .    .   .   mulxq	16(%rsi), %r14, %rcx
[0,43]    .    .    .   D==============eER   .    .    .    .    .    .    .    .    .   .   adoxq	%r14, %rbx
[0,44]    .    .    .    D==============eER  .    .    .    .    .    .    .    .    .   .   adcxq	%rcx, %r11
[0,45]    .    .    .    D====eeeeeeeeeE--R  .    .    .    .    .    .    .    .    .   .   mulxq	24(%rsi), %r14, %rcx
[0,46]    .    .    .    D===============eER .    .    .    .    .    .    .    .    .   .   adoxq	%r14, %r11
[0,47]    .    .    .    .D===============eER.    .    .    .    .    .    .    .    .   .   adcxq	%rcx, %r10
[0,48]    .    .    .    .D====eeeeeeeeeE---R.    .    .    .    .    .    .    .    .   .   mulxq	32(%rsi), %r14, %rcx
[0,49]    .    .    .    .D================eER    .    .    .    .    .    .    .    .   .   adoxq	%r14, %r10
[0,50]    .    .    .    . D================eER   .    .    .    .    .    .    .    .   .   adcxq	%rcx, %r9
[0,51]    .    .    .    . D====eeeeeeeeeE----R   .    .    .    .    .    .    .    .   .   mulxq	40(%rsi), %r14, %rcx
[0,52]    .    .    .    . D=================eER  .    .    .    .    .    .    .    .   .   adoxq	%r14, %r9
[0,53]    .    .    .    .  D=================eER .    .    .    .    .    .    .    .   .   adcxq	%rcx, %r8
[0,54]    .    .    .    .  D====eeeeeeeeeE-----R .    .    .    .    .    .    .    .   .   mulxq	48(%rsi), %r14, %rcx
[0,55]    .    .    .    .  D==================eER.    .    .    .    .    .    .    .   .   adoxq	%r14, %r8
[0,56]    .    .    .    .   D==================eER    .    .    .    .    .    .    .   .   adcxq	%rcx, %rdi
[0,57]    .    .    .    .   D====eeeeeeeeeE------R    .    .    .    .    .    .    .   .   mulxq	56(%rsi), %rdx, %rcx
[0,58]    .    .    .    .   D===================eER   .    .    .    .    .    .    .   .   adoxq	%rdx, %rdi
[0,59]    .    .    .    .    D===================eER  .    .    .    .    .    .    .   .   adcxq	%r13, %rcx
[0,60]    .    .    .    .    DeeeeeE---------------R  .    .    .    .    .    .    .   .   movq	16(%r12), %rdx
[0,61]    .    .    .    .    D====================eER .    .    .    .    .    .    .   .   adoxq	%r13, %rcx
[0,62]    .    .    .    .    .D====eeeeeeeeeE-------R .    .    .    .    .    .    .   .   mulxq	(%rsi), %r15, %r14
[0,63]    .    .    .    .    .D---------------------R .    .    .    .    .    .    .   .   xorl	%r13d, %r13d
[0,64]    .    .    .    .    .D=======eE------------R .    .    .    .    .    .    .   .   adoxq	%r15, %rax
[0,65]    .    .    .    .    . D============eE------R .    .    .    .    .    .    .   .   adcxq	%r14, %rbx
[0,66]    .    .    .    .    . D=======eE-----------R .    .    .    .    .    .    .   .   movq	%rax, -72(%rbp)
[0,67]    .    .    .    .    . D====eeeeeeeeeE------R .    .    .    .    .    .    .   .   mulxq	8(%rsi), %r14, %rax
[0,68]    .    .    .    .    .  D============eE-----R .    .    .    .    .    .    .   .   adoxq	%r14, %rbx
[0,69]    .    .    .    .    .  D=============eE----R .    .    .    .    .    .    .   .   adcxq	%rax, %r11
[0,70]    .    .    .    .    .  D====eeeeeeeeeE-----R .    .    .    .    .    .    .   .   mulxq	16(%rsi), %r14, %rax
[0,71]    .    .    .    .    .   D=============eE---R .    .    .    .    .    .    .   .   adoxq	%r14, %r11
[0,72]    .    .    .    .    .   D==============eE--R .    .    .    .    .    .    .   .   adcxq	%rax, %r10
[0,73]    .    .    .    .    .   D====eeeeeeeeeE----R .    .    .    .    .    .    .   .   mulxq	24(%rsi), %r14, %rax
[0,74]    .    .    .    .    .    D==============eE-R .    .    .    .    .    .    .   .   adoxq	%r14, %r10
[0,75]    .    .    .    .    .    D===============eER .    .    .    .    .    .    .   .   adcxq	%rax, %r9
[0,76]    .    .    .    .    .    D====eeeeeeeeeE---R .    .    .    .    .    .    .   .   mulxq	32(%rsi), %r14, %rax
[0,77]    .    .    .    .    .    .D===============eER.    .    .    .    .    .    .   .   adoxq	%r14, %r9
[0,78]    .    .    .    .    .    .D================eER    .    .    .    .    .    .   .   adcxq	%rax, %r8
[0,79]    .    .    .    .    .    .D====eeeeeeeeeE----R    .    .    .    .    .    .   .   mulxq	40(%rsi), %r14, %rax
[0,80]    .    .    .    .    .    . D================eER   .    .    .    .    .    .   .   adoxq	%r14, %r8
[0,81]    .    .    .    .    .    . D=================eER  .    .    .    .    .    .   .   adcxq	%rax, %rdi
[0,82]    .    .    .    .    .    . D====eeeeeeeeeE-----R  .    .    .    .    .    .   .   mulxq	48(%rsi), %r14, %rax
[0,83]    .    .    .    .    .    .  D=================eER .    .    .    .    .    .   .   adoxq	%r14, %rdi
[0,84]    .    .    .    .    .    .  D==================eER.    .    .    .    .    .   .   adcxq	%rax, %rcx
[0,85]    .    .    .    .    .    .  D====eeeeeeeeeE------R.    .    .    .    .    .   .   mulxq	56(%rsi), %rdx, %rax
[0,86]    .    .    .    .    .    .   D==================eER    .    .    .    .    .   .   adoxq	%rdx, %rcx
[0,87]    .    .    .    .    .    .   D===================eER   .    .    .    .    .   .   adcxq	%r13, %rax
[0,88]    .    .    .    .    .    .   D====================eER  .    .    .    .    .   .   adoxq	%r13, %rax
[0,89]    .    .    .    .    .    .   D----------------------R  .    .    .    .    .   .   xorl	%r13d, %r13d
[0,90]    .    .    .    .    .    .   DeeeeeE----------------R  .    .    .    .    .   .   movq	24(%r12), %rdx
[0,91]    .    .    .    .    .    .    D====eeeeeeeeeE-------R  .    .    .    .    .   .   mulxq	(%rsi), %r15, %r14
[0,92]    .    .    .    .    .    .    D===========eE--------R  .    .    .    .    .   .   adoxq	%r15, %rbx
[0,93]    .    .    .    .    .    .    D=============eE------R  .    .    .    .    .   .   adcxq	%r14, %r11
[0,94]    .    .    .    .    .    .    .D===========eE-------R  .    .    .    .    .   .   movq	%rbx, -80(%rbp)
[0,95]    .    .    .    .    .    .    .D=============eE-----R  .    .    .    .    .   .   movq	%r11, %r15
[0,96]    .    .    .    .    .    .    .D====eeeeeeeeeE------R  .    .    .    .    .   .   mulxq	8(%rsi), %r14, %rbx
[0,97]    .    .    .    .    .    .    . D=============eE----R  .    .    .    .    .   .   adoxq	%r14, %r15
[0,98]    .    .    .    .    .    .    . D==============eE---R  .    .    .    .    .   .   adcxq	%rbx, %r10
[0,99]    .    .    .    .    .    .    . D====eeeeeeeeeE-----R  .    .    .    .    .   .   mulxq	16(%rsi), %rbx, %r11
[0,100]   .    .    .    .    .    .    .  D==============eE--R  .    .    .    .    .   .   adoxq	%rbx, %r10
[0,101]   .    .    .    .    .    .    .  D===============eE-R  .    .    .    .    .   .   adcxq	%r11, %r9
[0,102]   .    .    .    .    .    .    .  D====eeeeeeeeeE----R  .    .    .    .    .   .   mulxq	24(%rsi), %rbx, %r11
[0,103]   .    .    .    .    .    .    .   D===============eER  .    .    .    .    .   .   adoxq	%rbx, %r9
[0,104]   .    .    .    .    .    .    .   D================eER .    .    .    .    .   .   adcxq	%r11, %r8
[0,105]   .    .    .    .    .    .    .   D====eeeeeeeeeE----R .    .    .    .    .   .   mulxq	32(%rsi), %rbx, %r11
[0,106]   .    .    .    .    .    .    .    D================eER.    .    .    .    .   .   adoxq	%rbx, %r8
[0,107]   .    .    .    .    .    .    .    D=================eER    .    .    .    .   .   adcxq	%r11, %rdi
[0,108]   .    .    .    .    .    .    .    D====eeeeeeeeeE-----R    .    .    .    .   .   mulxq	40(%rsi), %rbx, %r11
[0,109]   .    .    .    .    .    .    .    .D=================eER   .    .    .    .   .   adoxq	%rbx, %rdi
[0,110]   .    .    .    .    .    .    .    .D==================eER  .    .    .    .   .   adcxq	%r11, %rcx
[0,111]   .    .    .    .    .    .    .    .D====eeeeeeeeeE------R  .    .    .    .   .   mulxq	48(%rsi), %rbx, %r11
[0,112]   .    .    .    .    .    .    .    . D==================eER .    .    .    .   .   adoxq	%rbx, %rcx
[0,113]   .    .    .    .    .    .    .    . D===================eER.    .    .    .   .   adcxq	%r11, %rax
[0,114]   .    .    .    .    .    .    .    . D====eeeeeeeeeE-------R.    .    .    .   .   mulxq	56(%rsi), %rdx, %r11
[0,115]   .    .    .    .    .    .    .    .  D===================eER    .    .    .   .   adoxq	%rdx, %rax
[0,116]   .    .    .    .    .    .    .    .  D====================eER   .    .    .   .   adcxq	%r13, %r11
[0,117]   .    .    .    .    .    .    .    .  DeeeeeE----------------R   .    .    .   .   movq	32(%r12), %rdx
[0,118]   .    .    .    .    .    .    .    .  D=====================eER  .    .    .   .   adoxq	%r13, %r11
[0,119]   .    .    .    .    .    .    .    .  D=====E-----------------R  .    .    .   .   xorl	%ebx, %ebx
[0,120]   .    .    .    .    .    .    .    .   D====eeeeeeeeeE--------R  .    .    .   .   mulxq	(%rsi), %r14, %r13
[0,121]   .    .    .    .    .    .    .    .   D===========eE---------R  .    .    .   .   adoxq	%r14, %r15
[0,122]   .    .    .    .    .    .    .    .   D=============eE-------R  .    .    .   .   adcxq	%r13, %r10
[0,123]   .    .    .    .    .    .    .    .    D===========eE--------R  .    .    .   .   movq	%r15, -88(%rbp)
[0,124]   .    .    .    .    .    .    .    .    D====eeeeeeeeeE-------R  .    .    .   .   mulxq	8(%rsi), %r14, %r13
[0,125]   .    .    .    .    .    .    .    .    D=============eE------R  .    .    .   .   movq	%r10, %r15
[0,126]   .    .    .    .    .    .    .    .    .D============eE------R  .    .    .   .   adcxq	%r13, %r9
[0,127]   .    .    .    .    .    .    .    .    .D=============eE-----R  .    .    .   .   adoxq	%r14, %r15
[0,128]   .    .    .    .    .    .    .    .    .D====eeeeeeeeeE------R  .    .    .   .   mulxq	16(%rsi), %r13, %r10
[0,129]   .    .    .    .    .    .    .    .    . D=============eE----R  .    .    .   .   adoxq	%r13, %r9
[0,130]   .    .    .    .    .    .    .    .    . D==============eE---R  .    .    .   .   adcxq	%r10, %r8
[0,131]   .    .    .    .    .    .    .    .    . D====eeeeeeeeeE-----R  .    .    .   .   mulxq	24(%rsi), %r13, %r10
[0,132]   .    .    .    .    .    .    .    .    .  D==============eE--R  .    .    .   .   adcxq	%r10, %rdi
[0,133]   .    .    .    .    .    .    .    .    .  D===============eE-R  .    .    .   .   adoxq	%r13, %r8
[0,134]   .    .    .    .    .    .    .    .    .  D====eeeeeeeeeE----R  .    .    .   .   mulxq	32(%rsi), %r13, %r10
[0,135]   .    .    .    .    .    .    .    .    .   D===============eER  .    .    .   .   adoxq	%r13, %rdi
[0,136]   .    .    .    .    .    .    .    .    .   D================eER .    .    .   .   adcxq	%r10, %rcx
[0,137]   .    .    .    .    .    .    .    .    .   D====eeeeeeeeeE----R .    .    .   .   mulxq	40(%rsi), %r13, %r10
[0,138]   .    .    .    .    .    .    .    .    .    D================eER.    .    .   .   adoxq	%r13, %rcx
[0,139]   .    .    .    .    .    .    .    .    .    D=================eER    .    .   .   adcxq	%r10, %rax
[0,140]   .    .    .    .    .    .    .    .    .    D====eeeeeeeeeE-----R    .    .   .   mulxq	48(%rsi), %r13, %r10
[0,141]   .    .    .    .    .    .    .    .    .    .D=================eER   .    .   .   adoxq	%r13, %rax
[0,142]   .    .    .    .    .    .    .    .    .    .D==================eER  .    .   .   adcxq	%r10, %r11
[0,143]   .    .    .    .    .    .    .    .    .    .D====eeeeeeeeeE------R  .    .   .   mulxq	56(%rsi), %rdx, %r10
[0,144]   .    .    .    .    .    .    .    .    .    . D==================eER .    .   .   adoxq	%rdx, %r11
[0,145]   .    .    .    .    .    .    .    .    .    . D===================eER.    .   .   adcxq	%rbx, %r10
[0,146]   .    .    .    .    .    .    .    .    .    . DeeeeeE---------------R.    .   .   movq	40(%r12), %rdx
[0,147]   .    .    .    .    .    .    .    .    .    . D====================eER    .   .   adoxq	%rbx, %r10
[0,148]   .    .    .    .    .    .    .    .    .    .  D====eeeeeeeeeE-------R    .   .   mulxq	(%rsi), %r14, %r13
[0,149]   .    .    .    .    .    .    .    .    .    .  D---------------------R    .   .   xorl	%ebx, %ebx
[0,150]   .    .    .    .    .    .    .    .    .    .  D============eE-------R    .   .   adoxq	%r14, %r15
[0,151]   .    .    .    .    .    .    .    .    .    .   D============eE------R    .   .   movq	%r15, -96(%rbp)
[0,152]   .    .    .    .    .    .    .    .    .    .   D============eE------R    .   .   adcxq	%r13, %r9
[0,153]   .    .    .    .    .    .    .    .    .    .   D=====eeeeeeeeeE-----R    .   .   mulxq	8(%rsi), %r14, %r13
[0,154]   .    .    .    .    .    .    .    .    .    .    D============eE-----R    .   .   movq	%r9, %r15
[0,155]   .    .    .    .    .    .    .    .    .    .    D=============eE----R    .   .   adoxq	%r14, %r15
[0,156]   .    .    .    .    .    .    .    .    .    .    D==============eE---R    .   .   adcxq	%r13, %r8
[0,157]   .    .    .    .    .    .    .    .    .    .    .D====eeeeeeeeeE----R    .   .   mulxq	16(%rsi), %r13, %r9
[0,158]   .    .    .    .    .    .    .    .    .    .    .D==============eE--R    .   .   adoxq	%r13, %r8
[0,159]   .    .    .    .    .    .    .    .    .    .    .D===============eE-R    .   .   adcxq	%r9, %rdi
[0,160]   .    .    .    .    .    .    .    .    .    .    . D====eeeeeeeeeE---R    .   .   mulxq	24(%rsi), %r13, %r9
[0,161]   .    .    .    .    .    .    .    .    .    .    . D===============eER    .   .   adoxq	%r13, %rdi
[0,162]   .    .    .    .    .    .    .    .    .    .    . D================eER   .   .   adcxq	%r9, %rcx
[0,163]   .    .    .    .    .    .    .    .    .    .    .  D====eeeeeeeeeE---R   .   .   mulxq	32(%rsi), %r13, %r9
[0,164]   .    .    .    .    .    .    .    .    .    .    .  D================eER  .   .   adoxq	%r13, %rcx
[0,165]   .    .    .    .    .    .    .    .    .    .    .  D=================eER .   .   adcxq	%r9, %rax
[0,166]   .    .    .    .    .    .    .    .    .    .    .   D====eeeeeeeeeE----R .   .   mulxq	40(%rsi), %r13, %r9
[0,167]   .    .    .    .    .    .    .    .    .    .    .   D=================eER.   .   adoxq	%r13, %rax
[0,168]   .    .    .    .    .    .    .    .    .    .    .   D==================eER   .   adcxq	%r9, %r11
[0,169]   .    .    .    .    .    .    .    .    .    .    .    D====eeeeeeeeeE-----R   .   mulxq	48(%rsi), %r13, %r9
[0,170]   .    .    .    .    .    .    .    .    .    .    .    D==================eER  .   adoxq	%r13, %r11
[0,171]   .    .    .    .    .    .    .    .    .    .    .    D===================eER .   adcxq	%r9, %r10
[0,172]   .    .    .    .    .    .    .    .    .    .    .    .D====eeeeeeeeeE------R .   mulxq	56(%rsi), %rdx, %r9
[0,173]   .    .    .    .    .    .    .    .    .    .    .    .D===================eER.   adoxq	%rdx, %r10
[0,174]   .    .    .    .    .    .    .    .    .    .    .    .D====================eER   adcxq	%rbx, %r9
