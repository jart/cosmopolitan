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

//	Computes 768-bit product of 384-bit and 384-bit numbers.
//
//		Instructions:       152
//		Total Cycles:        65
//		Total uOps:         260
//		uOps Per Cycle:    4.00
//		IPC:               2.34
//		Block RThroughput: 43.3
//
//	@param	rdi receives 8 quadword result
//	@param	rsi is left hand side which must have 4 quadwords
//	@param	rdx is right hand side which must have 4 quadwords
//	@note	words are host endian while array is little endian
//	@mayalias
	.ftrace1
Mul6x6Adx:
	.ftrace2
	push	%rbp
	mov	%rsp,%rbp
	sub	$64,%rsp
	mov	%r15,-8(%rbp)
	mov	%r14,-16(%rbp)
	mov	%r13,-24(%rbp)
	mov	%r12,-32(%rbp)
	mov	%rbx,-40(%rbp)
	mov	%rdx,%rbx
	mov	(%rdx),%rdx
	mulx	(%rsi),%rcx,%rax
	mulx	8(%rsi),%rdx,%r12
	mov	%rcx,-48(%rbp)
	add	%rdx,%rax
	mov	(%rbx),%rdx
	mulx	16(%rsi),%rdx,%r15
	adc	%rdx,%r12
	mov	(%rbx),%rdx
	mulx	24(%rsi),%rdx,%r10
	adc	%rdx,%r15
	mov	(%rbx),%rdx
	mulx	32(%rsi),%rdx,%r9
	adc	%rdx,%r10
	mov	(%rbx),%rdx
	mulx	40(%rsi),%rdx,%rcx
	adc	%rdx,%r9
	mov	8(%rbx),%rdx
	adc	$0,%rcx
	mulx	(%rsi),%r13,%r11
	xor	%r8d,%r8d
	adox	%r13,%rax
	adcx	%r11,%r12
	mov	%rax,-56(%rbp)
	mulx	8(%rsi),%r11,%rax
	adox	%r11,%r12
	adcx	%rax,%r15
	mov	%r12,%r14
	mulx	16(%rsi),%r11,%rax
	adox	%r11,%r15
	adcx	%rax,%r10
	mulx	24(%rsi),%r11,%rax
	adox	%r11,%r10
	adcx	%rax,%r9
	mulx	32(%rsi),%r11,%rax
	adox	%r11,%r9
	adcx	%rax,%rcx
	mulx	40(%rsi),%rdx,%rax
	adox	%rdx,%rcx
	adcx	%r8,%rax
	mov	16(%rbx),%rdx
	adox	%r8,%rax
	mulx	(%rsi),%r13,%r8
	xor	%r11d,%r11d
	adox	%r13,%r14
	mov	%r14,-64(%rbp)
	adcx	%r8,%r15
	mulx	8(%rsi),%r12,%r8
	adox	%r12,%r15
	adcx	%r8,%r10
	mulx	16(%rsi),%r12,%r8
	adox	%r12,%r10
	adcx	%r8,%r9
	mulx	24(%rsi),%r12,%r8
	adox	%r12,%r9
	adcx	%r8,%rcx
	mulx	32(%rsi),%r12,%r8
	adox	%r12,%rcx
	adcx	%r8,%rax
	mulx	40(%rsi),%rdx,%r8
	adox	%rdx,%rax
	adcx	%r11,%r8
	mov	24(%rbx),%rdx
	adox	%r11,%r8
	mulx	(%rsi),%r13,%r11
	xor	%r12d,%r12d
	adox	%r13,%r15
	adcx	%r11,%r10
	mulx	8(%rsi),%r13,%r11
	adox	%r13,%r10
	adcx	%r11,%r9
	mulx	16(%rsi),%r13,%r11
	adox	%r13,%r9
	adcx	%r11,%rcx
	mulx	24(%rsi),%r13,%r11
	adox	%r13,%rcx
	adcx	%r11,%rax
	mulx	32(%rsi),%r13,%r11
	adox	%r13,%rax
	adcx	%r11,%r8
	mulx	40(%rsi),%rdx,%r11
	adox	%rdx,%r8
	mov	32(%rbx),%rdx
	adcx	%r12,%r11
	mulx	(%rsi),%r14,%r13
	adox	%r12,%r11
	xor	%r12d,%r12d
	adox	%r14,%r10
	adcx	%r13,%r9
	mulx	8(%rsi),%r14,%r13
	adox	%r14,%r9
	adcx	%r13,%rcx
	mulx	16(%rsi),%r14,%r13
	adox	%r14,%rcx
	adcx	%r13,%rax
	mulx	24(%rsi),%r14,%r13
	adox	%r14,%rax
	adcx	%r13,%r8
	mulx	32(%rsi),%r14,%r13
	adox	%r14,%r8
	adcx	%r13,%r11
	mulx	40(%rsi),%rdx,%r13
	adox	%rdx,%r11
	adcx	%r12,%r13
	mov	40(%rbx),%rdx
	adox	%r12,%r13
	mulx	(%rsi),%r14,%rbx
	xor	%r12d,%r12d
	adox	%r14,%r9
	adcx	%rbx,%rcx
	mulx	8(%rsi),%r14,%rbx
	adox	%r14,%rcx
	adcx	%rbx,%rax
	mulx	16(%rsi),%r14,%rbx
	adox	%r14,%rax
	adcx	%rbx,%r8
	mulx	24(%rsi),%r14,%rbx
	adox	%r14,%r8
	adcx	%rbx,%r11
	mulx	32(%rsi),%r14,%rbx
	mulx	40(%rsi),%rsi,%rdx
	adox	%r14,%r11
	adcx	%rbx,%r13
	adox	%rsi,%r13
	adcx	%r12,%rdx
	adox	%r12,%rdx
	mov	-48(%rbp),%rsi
	mov	-56(%rbp),%rbx
	mov	-64(%rbp),%r14
	mov	%rsi,(%rdi)
	mov	%rbx,8(%rdi)
	mov	%r14,16(%rdi)
	mov	%r15,24(%rdi)
	mov	%r10,32(%rdi)
	mov	%r9,40(%rdi)
	mov	%rcx,48(%rdi)
	mov	%rax,56(%rdi)
	mov	%r8,64(%rdi)
	mov	%r11,72(%rdi)
	mov	%r13,80(%rdi)
	mov	%rdx,88(%rdi)
	mov	-8(%rbp),%r15
	mov	-16(%rbp),%r14
	mov	-24(%rbp),%r13
	mov	-32(%rbp),%r12
	mov	-40(%rbp),%rbx
	leave
	ret
	.endfn	Mul6x6Adx,globl

	.end
SIMULATION          0123456789          0123456789          0123456789
Index     0123456789          0123456789          0123456789          01234
[0,0]     DeER .    .    .    .    .    .    .    .    .    .    .    .   .   movq	%r15, -8(%rbp)
[0,1]     D=eER.    .    .    .    .    .    .    .    .    .    .    .   .   movq	%r14, -16(%rbp)
[0,2]     D==eER    .    .    .    .    .    .    .    .    .    .    .   .   movq	%r13, -24(%rbp)
[0,3]     D===eER   .    .    .    .    .    .    .    .    .    .    .   .   movq	%r12, -32(%rbp)
[0,4]     D====eER  .    .    .    .    .    .    .    .    .    .    .   .   movq	%rbx, -40(%rbp)
[0,5]     DeE----R  .    .    .    .    .    .    .    .    .    .    .   .   movq	%rdx, %rbx
[0,6]     .DeeeeeER .    .    .    .    .    .    .    .    .    .    .   .   movq	(%rdx), %rdx
[0,7]     .D=====eeeeeeeeeER  .    .    .    .    .    .    .    .    .   .   mulxq	(%rsi), %rcx, %rax
[0,8]     . D=====eeeeeeeeeER .    .    .    .    .    .    .    .    .   .   mulxq	8(%rsi), %rdx, %r12
[0,9]     . D=======eE------R .    .    .    .    .    .    .    .    .   .   movq	%rcx, -48(%rbp)
[0,10]    . D=============eER .    .    .    .    .    .    .    .    .   .   addq	%rdx, %rax
[0,11]    .  DeeeeeE--------R .    .    .    .    .    .    .    .    .   .   movq	(%rbx), %rdx
[0,12]    .  D=====eeeeeeeeeER.    .    .    .    .    .    .    .    .   .   mulxq	16(%rsi), %rdx, %r15
[0,13]    .  D=============eER.    .    .    .    .    .    .    .    .   .   adcq	%rdx, %r12
[0,14]    .   DeeeeeE--------R.    .    .    .    .    .    .    .    .   .   movq	(%rbx), %rdx
[0,15]    .   D=====eeeeeeeeeER    .    .    .    .    .    .    .    .   .   mulxq	24(%rsi), %rdx, %r10
[0,16]    .   D=============eER    .    .    .    .    .    .    .    .   .   adcq	%rdx, %r15
[0,17]    .    DeeeeeE--------R    .    .    .    .    .    .    .    .   .   movq	(%rbx), %rdx
[0,18]    .    D=====eeeeeeeeeER   .    .    .    .    .    .    .    .   .   mulxq	32(%rsi), %rdx, %r9
[0,19]    .    D=============eER   .    .    .    .    .    .    .    .   .   adcq	%rdx, %r10
[0,20]    .    .DeeeeeE--------R   .    .    .    .    .    .    .    .   .   movq	(%rbx), %rdx
[0,21]    .    .D=====eeeeeeeeeER  .    .    .    .    .    .    .    .   .   mulxq	40(%rsi), %rdx, %rcx
[0,22]    .    .D=============eER  .    .    .    .    .    .    .    .   .   adcq	%rdx, %r9
[0,23]    .    . DeeeeeE--------R  .    .    .    .    .    .    .    .   .   movq	8(%rbx), %rdx
[0,24]    .    . D=============eER .    .    .    .    .    .    .    .   .   adcq	$0, %rcx
[0,25]    .    . D=====eeeeeeeeeER .    .    .    .    .    .    .    .   .   mulxq	(%rsi), %r13, %r11
[0,26]    .    .  D--------------R .    .    .    .    .    .    .    .   .   xorl	%r8d, %r8d
[0,27]    .    .  D========eE----R .    .    .    .    .    .    .    .   .   adoxq	%r13, %rax
[0,28]    .    .  D=============eER.    .    .    .    .    .    .    .   .   adcxq	%r11, %r12
[0,29]    .    .  D=========eE----R.    .    .    .    .    .    .    .   .   movq	%rax, -56(%rbp)
[0,30]    .    .   D====eeeeeeeeeER.    .    .    .    .    .    .    .   .   mulxq	8(%rsi), %r11, %rax
[0,31]    .    .   D=============eER    .    .    .    .    .    .    .   .   adoxq	%r11, %r12
[0,32]    .    .   D==============eER   .    .    .    .    .    .    .   .   adcxq	%rax, %r15
[0,33]    .    .    D=============eER   .    .    .    .    .    .    .   .   movq	%r12, %r14
[0,34]    .    .    D====eeeeeeeeeE-R   .    .    .    .    .    .    .   .   mulxq	16(%rsi), %r11, %rax
[0,35]    .    .    D==============eER  .    .    .    .    .    .    .   .   adoxq	%r11, %r15
[0,36]    .    .    .D==============eER .    .    .    .    .    .    .   .   adcxq	%rax, %r10
[0,37]    .    .    .D====eeeeeeeeeE--R .    .    .    .    .    .    .   .   mulxq	24(%rsi), %r11, %rax
[0,38]    .    .    .D===============eER.    .    .    .    .    .    .   .   adoxq	%r11, %r10
[0,39]    .    .    . D===============eER    .    .    .    .    .    .   .   adcxq	%rax, %r9
[0,40]    .    .    . D====eeeeeeeeeE---R    .    .    .    .    .    .   .   mulxq	32(%rsi), %r11, %rax
[0,41]    .    .    . D================eER   .    .    .    .    .    .   .   adoxq	%r11, %r9
[0,42]    .    .    .  D================eER  .    .    .    .    .    .   .   adcxq	%rax, %rcx
[0,43]    .    .    .  D====eeeeeeeeeE----R  .    .    .    .    .    .   .   mulxq	40(%rsi), %rdx, %rax
[0,44]    .    .    .  D=================eER .    .    .    .    .    .   .   adoxq	%rdx, %rcx
[0,45]    .    .    .   D=================eER.    .    .    .    .    .   .   adcxq	%r8, %rax
[0,46]    .    .    .   DeeeeeE-------------R.    .    .    .    .    .   .   movq	16(%rbx), %rdx
[0,47]    .    .    .   D==================eER    .    .    .    .    .   .   adoxq	%r8, %rax
[0,48]    .    .    .    D====eeeeeeeeeE-----R    .    .    .    .    .   .   mulxq	(%rsi), %r13, %r8
[0,49]    .    .    .    D====E--------------R    .    .    .    .    .   .   xorl	%r11d, %r11d
[0,50]    .    .    .    D=========eE--------R    .    .    .    .    .   .   adoxq	%r13, %r14
[0,51]    .    .    .    .D=========eE-------R    .    .    .    .    .   .   movq	%r14, -64(%rbp)
[0,52]    .    .    .    .D============eE----R    .    .    .    .    .   .   adcxq	%r8, %r15
[0,53]    .    .    .    .D====eeeeeeeeeE----R    .    .    .    .    .   .   mulxq	8(%rsi), %r12, %r8
[0,54]    .    .    .    . D============eE---R    .    .    .    .    .   .   adoxq	%r12, %r15
[0,55]    .    .    .    . D=============eE--R    .    .    .    .    .   .   adcxq	%r8, %r10
[0,56]    .    .    .    . D====eeeeeeeeeE---R    .    .    .    .    .   .   mulxq	16(%rsi), %r12, %r8
[0,57]    .    .    .    .  D=============eE-R    .    .    .    .    .   .   adoxq	%r12, %r10
[0,58]    .    .    .    .  D==============eER    .    .    .    .    .   .   adcxq	%r8, %r9
[0,59]    .    .    .    .  D====eeeeeeeeeE--R    .    .    .    .    .   .   mulxq	24(%rsi), %r12, %r8
[0,60]    .    .    .    .   D==============eER   .    .    .    .    .   .   adoxq	%r12, %r9
[0,61]    .    .    .    .   D===============eER  .    .    .    .    .   .   adcxq	%r8, %rcx
[0,62]    .    .    .    .   D====eeeeeeeeeE---R  .    .    .    .    .   .   mulxq	32(%rsi), %r12, %r8
[0,63]    .    .    .    .    D===============eER .    .    .    .    .   .   adoxq	%r12, %rcx
[0,64]    .    .    .    .    D================eER.    .    .    .    .   .   adcxq	%r8, %rax
[0,65]    .    .    .    .    D====eeeeeeeeeE----R.    .    .    .    .   .   mulxq	40(%rsi), %rdx, %r8
[0,66]    .    .    .    .    .D================eER    .    .    .    .   .   adoxq	%rdx, %rax
[0,67]    .    .    .    .    .D=================eER   .    .    .    .   .   adcxq	%r11, %r8
[0,68]    .    .    .    .    .DeeeeeE-------------R   .    .    .    .   .   movq	24(%rbx), %rdx
[0,69]    .    .    .    .    .D==================eER  .    .    .    .   .   adoxq	%r11, %r8
[0,70]    .    .    .    .    . D====eeeeeeeeeE-----R  .    .    .    .   .   mulxq	(%rsi), %r13, %r11
[0,71]    .    .    .    .    . D====E--------------R  .    .    .    .   .   xorl	%r12d, %r12d
[0,72]    .    .    .    .    . D===========eE------R  .    .    .    .   .   adoxq	%r13, %r15
[0,73]    .    .    .    .    .  D============eE----R  .    .    .    .   .   adcxq	%r11, %r10
[0,74]    .    .    .    .    .  D====eeeeeeeeeE----R  .    .    .    .   .   mulxq	8(%rsi), %r13, %r11
[0,75]    .    .    .    .    .  D=============eE---R  .    .    .    .   .   adoxq	%r13, %r10
[0,76]    .    .    .    .    .   D=============eE--R  .    .    .    .   .   adcxq	%r11, %r9
[0,77]    .    .    .    .    .   D====eeeeeeeeeE---R  .    .    .    .   .   mulxq	16(%rsi), %r13, %r11
[0,78]    .    .    .    .    .   D==============eE-R  .    .    .    .   .   adoxq	%r13, %r9
[0,79]    .    .    .    .    .    D==============eER  .    .    .    .   .   adcxq	%r11, %rcx
[0,80]    .    .    .    .    .    D====eeeeeeeeeE--R  .    .    .    .   .   mulxq	24(%rsi), %r13, %r11
[0,81]    .    .    .    .    .    D===============eER .    .    .    .   .   adoxq	%r13, %rcx
[0,82]    .    .    .    .    .    .D===============eER.    .    .    .   .   adcxq	%r11, %rax
[0,83]    .    .    .    .    .    .D====eeeeeeeeeE---R.    .    .    .   .   mulxq	32(%rsi), %r13, %r11
[0,84]    .    .    .    .    .    .D================eER    .    .    .   .   adoxq	%r13, %rax
[0,85]    .    .    .    .    .    . D================eER   .    .    .   .   adcxq	%r11, %r8
[0,86]    .    .    .    .    .    . D====eeeeeeeeeE----R   .    .    .   .   mulxq	40(%rsi), %rdx, %r11
[0,87]    .    .    .    .    .    . D=================eER  .    .    .   .   adoxq	%rdx, %r8
[0,88]    .    .    .    .    .    .  DeeeeeE------------R  .    .    .   .   movq	32(%rbx), %rdx
[0,89]    .    .    .    .    .    .  D=================eER .    .    .   .   adcxq	%r12, %r11
[0,90]    .    .    .    .    .    .  D=====eeeeeeeeeE----R .    .    .   .   mulxq	(%rsi), %r14, %r13
[0,91]    .    .    .    .    .    .   D=================eER.    .    .   .   adoxq	%r12, %r11
[0,92]    .    .    .    .    .    .   D-------------------R.    .    .   .   xorl	%r12d, %r12d
[0,93]    .    .    .    .    .    .   D===========eE------R.    .    .   .   adoxq	%r14, %r10
[0,94]    .    .    .    .    .    .   D=============eE----R.    .    .   .   adcxq	%r13, %r9
[0,95]    .    .    .    .    .    .    D====eeeeeeeeeE----R.    .    .   .   mulxq	8(%rsi), %r14, %r13
[0,96]    .    .    .    .    .    .    D=============eE---R.    .    .   .   adoxq	%r14, %r9
[0,97]    .    .    .    .    .    .    D==============eE--R.    .    .   .   adcxq	%r13, %rcx
[0,98]    .    .    .    .    .    .    .D====eeeeeeeeeE---R.    .    .   .   mulxq	16(%rsi), %r14, %r13
[0,99]    .    .    .    .    .    .    .D==============eE-R.    .    .   .   adoxq	%r14, %rcx
[0,100]   .    .    .    .    .    .    .D===============eER.    .    .   .   adcxq	%r13, %rax
[0,101]   .    .    .    .    .    .    . D====eeeeeeeeeE--R.    .    .   .   mulxq	24(%rsi), %r14, %r13
[0,102]   .    .    .    .    .    .    . D===============eER    .    .   .   adoxq	%r14, %rax
[0,103]   .    .    .    .    .    .    . D================eER   .    .   .   adcxq	%r13, %r8
[0,104]   .    .    .    .    .    .    .  D====eeeeeeeeeE---R   .    .   .   mulxq	32(%rsi), %r14, %r13
[0,105]   .    .    .    .    .    .    .  D================eER  .    .   .   adoxq	%r14, %r8
[0,106]   .    .    .    .    .    .    .  D=================eER .    .   .   adcxq	%r13, %r11
[0,107]   .    .    .    .    .    .    .   D====eeeeeeeeeE----R .    .   .   mulxq	40(%rsi), %rdx, %r13
[0,108]   .    .    .    .    .    .    .   D=================eER.    .   .   adoxq	%rdx, %r11
[0,109]   .    .    .    .    .    .    .   D==================eER    .   .   adcxq	%r12, %r13
[0,110]   .    .    .    .    .    .    .    DeeeeeE-------------R    .   .   movq	40(%rbx), %rdx
[0,111]   .    .    .    .    .    .    .    D==================eER   .   .   adoxq	%r12, %r13
[0,112]   .    .    .    .    .    .    .    D=====eeeeeeeeeE-----R   .   .   mulxq	(%rsi), %r14, %rbx
[0,113]   .    .    .    .    .    .    .    .D-------------------R   .   .   xorl	%r12d, %r12d
[0,114]   .    .    .    .    .    .    .    .D===========eE------R   .   .   adoxq	%r14, %r9
[0,115]   .    .    .    .    .    .    .    .D=============eE----R   .   .   adcxq	%rbx, %rcx
[0,116]   .    .    .    .    .    .    .    . D====eeeeeeeeeE----R   .   .   mulxq	8(%rsi), %r14, %rbx
[0,117]   .    .    .    .    .    .    .    . D=============eE---R   .   .   adoxq	%r14, %rcx
[0,118]   .    .    .    .    .    .    .    . D==============eE--R   .   .   adcxq	%rbx, %rax
[0,119]   .    .    .    .    .    .    .    .  D====eeeeeeeeeE---R   .   .   mulxq	16(%rsi), %r14, %rbx
[0,120]   .    .    .    .    .    .    .    .  D==============eE-R   .   .   adoxq	%r14, %rax
[0,121]   .    .    .    .    .    .    .    .  D===============eER   .   .   adcxq	%rbx, %r8
[0,122]   .    .    .    .    .    .    .    .   D====eeeeeeeeeE--R   .   .   mulxq	24(%rsi), %r14, %rbx
[0,123]   .    .    .    .    .    .    .    .   D===============eER  .   .   adoxq	%r14, %r8
[0,124]   .    .    .    .    .    .    .    .   D================eER .   .   adcxq	%rbx, %r11
[0,125]   .    .    .    .    .    .    .    .    D====eeeeeeeeeE---R .   .   mulxq	32(%rsi), %r14, %rbx
[0,126]   .    .    .    .    .    .    .    .    .D====eeeeeeeeeE--R .   .   mulxq	40(%rsi), %rsi, %rdx
[0,127]   .    .    .    .    .    .    .    .    .D===============eER.   .   adoxq	%r14, %r11
[0,128]   .    .    .    .    .    .    .    .    .D================eER   .   adcxq	%rbx, %r13
[0,129]   .    .    .    .    .    .    .    .    . D================eER  .   adoxq	%rsi, %r13
[0,130]   .    .    .    .    .    .    .    .    . D=================eER .   adcxq	%r12, %rdx
[0,131]   .    .    .    .    .    .    .    .    . D==================eER.   adoxq	%r12, %rdx
[0,132]   .    .    .    .    .    .    .    .    . DeeeeeE--------------R.   movq	-48(%rbp), %rsi
[0,133]   .    .    .    .    .    .    .    .    . D=eeeeeE-------------R.   movq	-56(%rbp), %rbx
[0,134]   .    .    .    .    .    .    .    .    . D==eeeeeE------------R.   movq	-64(%rbp), %r14
[0,135]   .    .    .    .    .    .    .    .    .  D====eE-------------R.   movq	%rsi, (%rdi)
[0,136]   .    .    .    .    .    .    .    .    .  D=====eE------------R.   movq	%rbx, 8(%rdi)
[0,137]   .    .    .    .    .    .    .    .    .  D======eE-----------R.   movq	%r14, 16(%rdi)
[0,138]   .    .    .    .    .    .    .    .    .  D=======eE----------R.   movq	%r15, 24(%rdi)
[0,139]   .    .    .    .    .    .    .    .    .  D========eE---------R.   movq	%r10, 32(%rdi)
[0,140]   .    .    .    .    .    .    .    .    .  D=========eE--------R.   movq	%r9, 40(%rdi)
[0,141]   .    .    .    .    .    .    .    .    .   D=========eE-------R.   movq	%rcx, 48(%rdi)
[0,142]   .    .    .    .    .    .    .    .    .   D==========eE------R.   movq	%rax, 56(%rdi)
[0,143]   .    .    .    .    .    .    .    .    .   D===========eE-----R.   movq	%r8, 64(%rdi)
[0,144]   .    .    .    .    .    .    .    .    .   D=============eE---R.   movq	%r11, 72(%rdi)
[0,145]   .    .    .    .    .    .    .    .    .   D===============eE-R.   movq	%r13, 80(%rdi)
[0,146]   .    .    .    .    .    .    .    .    .   D=================eER   movq	%rdx, 88(%rdi)
[0,147]   .    .    .    .    .    .    .    .    .    DeeeeeE------------R   movq	-8(%rbp), %r15
[0,148]   .    .    .    .    .    .    .    .    .    D=eeeeeE-----------R   movq	-16(%rbp), %r14
[0,149]   .    .    .    .    .    .    .    .    .    D=eeeeeE-----------R   movq	-24(%rbp), %r13
[0,150]   .    .    .    .    .    .    .    .    .    D==eeeeeE----------R   movq	-32(%rbp), %r12
[0,151]   .    .    .    .    .    .    .    .    .    D==eeeeeE----------R   movq	-40(%rbp), %rbx
