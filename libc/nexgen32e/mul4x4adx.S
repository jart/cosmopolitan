/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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

//	Computes 512-bit product of 256-bit and 256-bit numbers.
//
//		Instructions:        88
//		Total Cycles:        36
//		Total uOps:         120
//		uOps Per Cycle:    3.33
//		IPC:               2.44
//		Block RThroughput: 20.0
//
//	@param	rdi receives 8 quadword result
//	@param	rsi is left hand side which must have 4 quadwords
//	@param	rdx is right hand side which must have 4 quadwords
//	@note	words are host endian while array is little endian
//	@mayalias
	.ftrace1
Mul4x4Adx:
	.ftrace2
	push	%rbp
	mov	%rsp,%rbp
	sub	$56,%rsp
	mov	%r15,-8(%rbp)
	mov	%r14,-16(%rbp)
	mov	%r13,-24(%rbp)
	mov	%r12,-32(%rbp)
	mov	%rbx,-40(%rbp)
	mov	%rdx,%r12
	mov	(%rdx),%rdx
	mov	(%rsi),%rax
	mov	16(%rsi),%r11
	mov	24(%rsi),%r10
	mulx	%rax,%rbx,%rax
	mov	%rbx,-48(%rbp)
	mov	8(%rsi),%rbx
	mulx	%rbx,%rdx,%rcx
	add	%rdx,%rax
	mov	(%r12),%rdx
	mulx	%r11,%rdx,%r9
	adc	%rdx,%rcx
	mov	(%r12),%rdx
	mulx	%r10,%rdx,%r8
	adc	%rdx,%r9
	adc	$0,%r8
	xor	%r13d,%r13d
	mov	(%rsi),%r14
	mov	8(%r12),%rdx
	mulx	%r14,%r14,%r15
	adox	%r14,%rax
	adcx	%r15,%rcx
	mov	%rax,-56(%rbp)
	mulx	%rbx,%r14,%rax
	adox	%r14,%rcx
	adcx	%rax,%r9
	mulx	%r11,%r14,%rax
	adox	%r14,%r9
	adcx	%rax,%r8
	mulx	%r10,%rdx,%rax
	adox	%rdx,%r8
	mov	16(%r12),%rdx
	adcx	%r13,%rax
	adox	%r13,%rax
	mov	(%rsi),%r13
	xor	%r15d,%r15d
	mulx	%r13,%r13,%r14
	adox	%r13,%rcx
	adcx	%r14,%r9
	mulx	%rbx,%r14,%r13
	adox	%r14,%r9
	adcx	%r13,%r8
	mulx	%r11,%r14,%r13
	adox	%r14,%r8
	adcx	%r13,%rax
	mov	(%rsi),%rsi
	mulx	%r10,%rdx,%r13
	adox	%rdx,%rax
	adcx	%r15,%r13
	mov	24(%r12),%rdx
	adox	%r15,%r13
	mulx	%rsi,%r12,%rsi
	xor	%r14d,%r14d
	adox	%r12,%r9
	adcx	%rsi,%r8
	mulx	%rbx,%rsi,%rbx
	adox	%rsi,%r8
	adcx	%rbx,%rax
	mulx	%r11,%r11,%rsi
	mov	-56(%rbp),%rbx
	mov	%rcx,16(%rdi)
	adcx	%rsi,%r13
	mov	-48(%rbp),%rsi
	mov	%rbx,8(%rdi)
	adox	%r11,%rax
	mov	%r9,24(%rdi)
	mov	%r8,32(%rdi)
	mov	%rax,40(%rdi)
	mulx	%r10,%rdx,%r10
	adox	%rdx,%r13
	adcx	%r14,%r10
	mov	%r13,48(%rdi)
	adox	%r14,%r10
	mov	%rsi,(%rdi)
	mov	%r10,56(%rdi)
	mov	-8(%rbp),%r15
	mov	-16(%rbp),%r14
	mov	-24(%rbp),%r13
	mov	-32(%rbp),%r12
	mov	-40(%rbp),%rbx
	leave
	ret
	.endfn	Mul4x4Adx,globl

	.end
TIMELINE VIEW       0123456789          012345
Index     0123456789          0123456789
[0,0]     DeER .    .    .    .    .    .    .   subq	$56, %rsp
[0,1]     DeER .    .    .    .    .    .    .   movq	%r15, -8(%rbp)
[0,2]     D=eER.    .    .    .    .    .    .   movq	%r14, -16(%rbp)
[0,3]     D==eER    .    .    .    .    .    .   movq	%r13, -24(%rbp)
[0,4]     D===eER   .    .    .    .    .    .   movq	%r12, -32(%rbp)
[0,5]     D====eER  .    .    .    .    .    .   movq	%rbx, -40(%rbp)
[0,6]     .DeE---R  .    .    .    .    .    .   movq	%rdx, %r12
[0,7]     .DeeeeeER .    .    .    .    .    .   movq	(%rdx), %rdx
[0,8]     .D=eeeeeER.    .    .    .    .    .   movq	(%rsi), %rax
[0,9]     .D=eeeeeER.    .    .    .    .    .   movq	16(%rsi), %r11
[0,10]    .D==eeeeeER    .    .    .    .    .   movq	24(%rsi), %r10
[0,11]    . D=====eeeeER .    .    .    .    .   mulxq	%rax, %rbx, %rax
[0,12]    . D========eER .    .    .    .    .   movq	%rbx, -48(%rbp)
[0,13]    . D=eeeeeE---R .    .    .    .    .   movq	8(%rsi), %rbx
[0,14]    .  D=====eeeeER.    .    .    .    .   mulxq	%rbx, %rdx, %rcx
[0,15]    .  D========eER.    .    .    .    .   addq	%rdx, %rax
[0,16]    .  D=eeeeeE---R.    .    .    .    .   movq	(%r12), %rdx
[0,17]    .   D=====eeeeER    .    .    .    .   mulxq	%r11, %rdx, %r9
[0,18]    .   D========eER    .    .    .    .   adcq	%rdx, %rcx
[0,19]    .   DeeeeeE----R    .    .    .    .   movq	(%r12), %rdx
[0,20]    .    D=====eeeeER   .    .    .    .   mulxq	%r10, %rdx, %r8
[0,21]    .    D========eER   .    .    .    .   adcq	%rdx, %r9
[0,22]    .    D=========eER  .    .    .    .   adcq	$0, %r8
[0,23]    .    D-----------R  .    .    .    .   xorl	%r13d, %r13d
[0,24]    .    .DeeeeeE----R  .    .    .    .   movq	(%rsi), %r14
[0,25]    .    .DeeeeeE----R  .    .    .    .   movq	8(%r12), %rdx
[0,26]    .    .D=====eeeeER  .    .    .    .   mulxq	%r14, %r14, %r15
[0,27]    .    .D========eER  .    .    .    .   adoxq	%r14, %rax
[0,28]    .    . D========eER .    .    .    .   adcxq	%r15, %rcx
[0,29]    .    . D========eER .    .    .    .   movq	%rax, -56(%rbp)
[0,30]    .    . D=====eeeeER .    .    .    .   mulxq	%rbx, %r14, %rax
[0,31]    .    . D=========eER.    .    .    .   adoxq	%r14, %rcx
[0,32]    .    .  D=========eER    .    .    .   adcxq	%rax, %r9
[0,33]    .    .  D=====eeeeE-R    .    .    .   mulxq	%r11, %r14, %rax
[0,34]    .    .  D==========eER   .    .    .   adoxq	%r14, %r9
[0,35]    .    .  D===========eER  .    .    .   adcxq	%rax, %r8
[0,36]    .    .   D=====eeeeE--R  .    .    .   mulxq	%r10, %rdx, %rax
[0,37]    .    .   D===========eER .    .    .   adoxq	%rdx, %r8
[0,38]    .    .   DeeeeeE-------R .    .    .   movq	16(%r12), %rdx
[0,39]    .    .   D============eER.    .    .   adcxq	%r13, %rax
[0,40]    .    .    D============eER    .    .   adoxq	%r13, %rax
[0,41]    .    .    DeeeeeE--------R    .    .   movq	(%rsi), %r13
[0,42]    .    .    D=====E--------R    .    .   xorl	%r15d, %r15d
[0,43]    .    .    D=====eeeeE----R    .    .   mulxq	%r13, %r13, %r14
[0,44]    .    .    .D=======eE----R    .    .   adoxq	%r13, %rcx
[0,45]    .    .    .D========eE---R    .    .   adcxq	%r14, %r9
[0,46]    .    .    .D=====eeeeE---R    .    .   mulxq	%rbx, %r14, %r13
[0,47]    .    .    .D=========eE--R    .    .   adoxq	%r14, %r9
[0,48]    .    .    . D=========eE-R    .    .   adcxq	%r13, %r8
[0,49]    .    .    . D=====eeeeE--R    .    .   mulxq	%r11, %r14, %r13
[0,50]    .    .    . D==========eER    .    .   adoxq	%r14, %r8
[0,51]    .    .    . D===========eER   .    .   adcxq	%r13, %rax
[0,52]    .    .    .  DeeeeeE------R   .    .   movq	(%rsi), %rsi
[0,53]    .    .    .  D=====eeeeE--R   .    .   mulxq	%r10, %rdx, %r13
[0,54]    .    .    .  D===========eER  .    .   adoxq	%rdx, %rax
[0,55]    .    .    .  D============eER .    .   adcxq	%r15, %r13
[0,56]    .    .    .   DeeeeeE-------R .    .   movq	24(%r12), %rdx
[0,57]    .    .    .   D============eER.    .   adoxq	%r15, %r13
[0,58]    .    .    .   D=====eeeeE----R.    .   mulxq	%rsi, %r12, %rsi
[0,59]    .    .    .   D======E-------R.    .   xorl	%r14d, %r14d
[0,60]    .    .    .    D========eE---R.    .   adoxq	%r12, %r9
[0,61]    .    .    .    D=========eE--R.    .   adcxq	%rsi, %r8
[0,62]    .    .    .    D=====eeeeE---R.    .   mulxq	%rbx, %rsi, %rbx
[0,63]    .    .    .    D==========eE-R.    .   adoxq	%rsi, %r8
[0,64]    .    .    .    .D==========eER.    .   adcxq	%rbx, %rax
[0,65]    .    .    .    .D=====eeeeE--R.    .   mulxq	%r11, %r11, %rsi
[0,66]    .    .    .    .DeeeeeE------R.    .   movq	-56(%rbp), %rbx
[0,67]    .    .    .    .D===eE-------R.    .   movq	%rcx, 16(%rdi)
[0,68]    .    .    .    . D==========eER    .   adcxq	%rsi, %r13
[0,69]    .    .    .    . DeeeeeE------R    .   movq	-48(%rbp), %rsi
[0,70]    .    .    .    . D====eE------R    .   movq	%rbx, 8(%rdi)
[0,71]    .    .    .    . D===========eER   .   adoxq	%r11, %rax
[0,72]    .    .    .    . D=======eE----R   .   movq	%r9, 24(%rdi)
[0,73]    .    .    .    . D=========eE--R   .   movq	%r8, 32(%rdi)
[0,74]    .    .    .    .  D===========eER  .   movq	%rax, 40(%rdi)
[0,75]    .    .    .    .  D====eeeeE----R  .   mulxq	%r10, %rdx, %r10
[0,76]    .    .    .    .  D===========eER  .   adoxq	%rdx, %r13
[0,77]    .    .    .    .  D============eER .   adcxq	%r14, %r10
[0,78]    .    .    .    .   D===========eER .   movq	%r13, 48(%rdi)
[0,79]    .    .    .    .   D============eER.   adoxq	%r14, %r10
[0,80]    .    .    .    .   D============eER.   movq	%rsi, (%rdi)
[0,81]    .    .    .    .   D=============eER   movq	%r10, 56(%rdi)
[0,82]    .    .    .    .   DeeeeeE---------R   movq	-8(%rbp), %r15
[0,83]    .    .    .    .   DeeeeeE---------R   movq	-16(%rbp), %r14
[0,84]    .    .    .    .    DeeeeeE--------R   movq	-24(%rbp), %r13
[0,85]    .    .    .    .    DeeeeeE--------R   movq	-32(%rbp), %r12
[0,86]    .    .    .    .    D=eeeeeE-------R   movq	-40(%rbp), %rbx
[0,87]    .    .    .    .    D===eE---------R   addq	$56, %rsp
