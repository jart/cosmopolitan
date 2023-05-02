	.text
	.globl	__popcountdi2
	.section	.text._ZN3hwy6N_SSE26detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE26detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, @function
_ZN3hwy6N_SSE26detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0:
.LFB18781:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	movq	%rcx, %r14
	pushq	%r13
	.cfi_offset 13, -40
	movq	%rsi, %r13
	pushq	%r12
	.cfi_offset 12, -48
	movq	%rdi, %r12
	pushq	%rbx
	subq	$88, %rsp
	.cfi_offset 3, -56
	movq	%rdx, -120(%rbp)
	movaps	%xmm0, -80(%rbp)
	movaps	%xmm1, -64(%rbp)
	movaps	%xmm0, -112(%rbp)
	movaps	%xmm1, -96(%rbp)
	cmpq	$3, %rsi
	jbe	.L32
	movl	$4, %r15d
	xorl	%ebx, %ebx
	jmp	.L11
	.p2align 4,,10
	.p2align 3
.L3:
	movdqa	-80(%rbp), %xmm5
	movmskps	%xmm1, %edi
	movups	%xmm5, (%r12,%rbx,4)
	call	__popcountdi2@PLT
	cltq
	addq	%rax, %rbx
	leaq	4(%r15), %rax
	cmpq	%r13, %rax
	ja	.L88
	movq	%rax, %r15
.L11:
	movdqu	-16(%r12,%r15,4), %xmm1
	movdqu	-16(%r12,%r15,4), %xmm0
	leaq	-4(%r15), %rdx
	pcmpeqd	-96(%rbp), %xmm0
	pcmpeqd	-112(%rbp), %xmm1
	movdqa	%xmm0, %xmm2
	por	%xmm1, %xmm0
	movmskps	%xmm0, %eax
	cmpl	$15, %eax
	je	.L3
	pcmpeqd	%xmm0, %xmm0
	pxor	%xmm0, %xmm2
	pandn	%xmm2, %xmm1
	movmskps	%xmm1, %eax
	rep bsfl	%eax, %eax
	cltq
	addq	%rdx, %rax
	movd	(%r12,%rax,4), %xmm3
	movq	-120(%rbp), %rax
	pshufd	$0, %xmm3, %xmm0
	movaps	%xmm0, (%rax)
	leaq	4(%rbx), %rax
	cmpq	%rdx, %rax
	ja	.L4
	.p2align 4,,10
	.p2align 3
.L5:
	movdqa	-64(%rbp), %xmm4
	movq	%rax, %rbx
	movups	%xmm4, -16(%r12,%rax,4)
	addq	$4, %rax
	cmpq	%rdx, %rax
	jbe	.L5
.L4:
	subq	%rbx, %rdx
	leaq	0(,%rbx,4), %rcx
	movd	%edx, %xmm3
	pshufd	$0, %xmm3, %xmm0
	pcmpgtd	.LC0(%rip), %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L6
	movdqa	-64(%rbp), %xmm3
	movd	%xmm3, (%r12,%rbx,4)
.L6:
	pshufd	$85, %xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L7
	pshufd	$85, -64(%rbp), %xmm1
	movd	%xmm1, 4(%r12,%rcx)
.L7:
	movdqa	%xmm0, %xmm1
	punpckhdq	%xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L8
	movdqa	-64(%rbp), %xmm3
	movdqa	%xmm3, %xmm1
	punpckhdq	%xmm3, %xmm1
	movd	%xmm1, 8(%r12,%rcx)
.L8:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L89
.L21:
	addq	$88, %rsp
	xorl	%eax, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L89:
	.cfi_restore_state
	pshufd	$255, -64(%rbp), %xmm0
	movd	%xmm0, 12(%r12,%rcx)
	jmp	.L21
	.p2align 4,,10
	.p2align 3
.L88:
	movq	%r13, %r8
	leaq	0(,%r15,4), %rsi
	leaq	0(,%rbx,4), %r9
	subq	%r15, %r8
.L2:
	testq	%r8, %r8
	je	.L15
	leaq	0(,%r8,4), %rdx
	addq	%r12, %rsi
	movq	%r14, %rdi
	movq	%r9, -112(%rbp)
	movq	%r8, -96(%rbp)
	call	memcpy@PLT
	movq	-96(%rbp), %r8
	movq	-112(%rbp), %r9
.L15:
	movd	%r8d, %xmm3
	movdqa	(%r14), %xmm2
	movdqa	-80(%rbp), %xmm1
	pshufd	$0, %xmm3, %xmm0
	movdqa	.LC0(%rip), %xmm3
	pcmpeqd	%xmm2, %xmm1
	pcmpeqd	-64(%rbp), %xmm2
	pcmpgtd	%xmm3, %xmm0
	movdqa	%xmm0, %xmm5
	pand	%xmm1, %xmm5
	por	%xmm2, %xmm1
	pcmpeqd	%xmm2, %xmm2
	movdqa	%xmm2, %xmm4
	pxor	%xmm0, %xmm4
	por	%xmm4, %xmm1
	movmskps	%xmm1, %eax
	cmpl	$15, %eax
	jne	.L90
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L22
	movdqa	-80(%rbp), %xmm4
	movd	%xmm4, (%r12,%r9)
.L22:
	pshufd	$85, %xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L23
	pshufd	$85, -80(%rbp), %xmm1
	movd	%xmm1, 4(%r12,%r9)
.L23:
	movdqa	%xmm0, %xmm1
	punpckhdq	%xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L24
	movdqa	-80(%rbp), %xmm7
	movdqa	%xmm7, %xmm1
	punpckhdq	%xmm7, %xmm1
	movd	%xmm1, 8(%r12,%r9)
.L24:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L91
.L25:
	movmskps	%xmm5, %edi
	call	__popcountdi2@PLT
	movdqa	.LC0(%rip), %xmm3
	movslq	%eax, %rdx
	addq	%rbx, %rdx
	leaq	4(%rdx), %rax
	cmpq	%rax, %r13
	jb	.L26
	.p2align 4,,10
	.p2align 3
.L27:
	movdqa	-64(%rbp), %xmm2
	movq	%rax, %rdx
	movups	%xmm2, -16(%r12,%rax,4)
	addq	$4, %rax
	cmpq	%rax, %r13
	jnb	.L27
.L26:
	subq	%rdx, %r13
	leaq	0(,%rdx,4), %rcx
	movd	%r13d, %xmm4
	pshufd	$0, %xmm4, %xmm0
	pcmpgtd	%xmm3, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L28
	movdqa	-64(%rbp), %xmm3
	movd	%xmm3, (%r12,%rdx,4)
.L28:
	pshufd	$85, %xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L29
	pshufd	$85, -64(%rbp), %xmm1
	movd	%xmm1, 4(%r12,%rcx)
.L29:
	movdqa	%xmm0, %xmm1
	punpckhdq	%xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L30
	movdqa	-64(%rbp), %xmm3
	movdqa	%xmm3, %xmm1
	punpckhdq	%xmm3, %xmm1
	movd	%xmm1, 8(%r12,%rcx)
.L30:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L31
	pshufd	$255, -64(%rbp), %xmm0
	movd	%xmm0, 12(%r12,%rcx)
.L31:
	addq	$88, %rsp
	movl	$1, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L91:
	.cfi_restore_state
	pshufd	$255, -80(%rbp), %xmm0
	movd	%xmm0, 12(%r12,%r9)
	jmp	.L25
.L32:
	movq	%rsi, %r8
	xorl	%r9d, %r9d
	xorl	%esi, %esi
	xorl	%ebx, %ebx
	xorl	%r15d, %r15d
	jmp	.L2
.L90:
	pxor	%xmm2, %xmm1
	movmskps	%xmm1, %eax
	rep bsfl	%eax, %eax
	cltq
	addq	%r15, %rax
	movd	(%r12,%rax,4), %xmm4
	movq	-120(%rbp), %rax
	pshufd	$0, %xmm4, %xmm0
	movaps	%xmm0, (%rax)
	leaq	4(%rbx), %rax
	cmpq	%rax, %r15
	jb	.L16
	.p2align 4,,10
	.p2align 3
.L17:
	movdqa	-64(%rbp), %xmm6
	movq	%rax, %rbx
	movups	%xmm6, -16(%r12,%rax,4)
	leaq	4(%rax), %rax
	cmpq	%r15, %rax
	jbe	.L17
	leaq	0(,%rbx,4), %r9
.L16:
	movq	%r15, %rcx
	subq	%rbx, %rcx
	movd	%ecx, %xmm4
	pshufd	$0, %xmm4, %xmm0
	pcmpgtd	%xmm3, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L18
	movdqa	-64(%rbp), %xmm3
	movd	%xmm3, (%r12,%r9)
.L18:
	pshufd	$85, %xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L19
	pshufd	$85, -64(%rbp), %xmm1
	movd	%xmm1, 4(%r12,%r9)
.L19:
	movdqa	%xmm0, %xmm1
	punpckhdq	%xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L20
	movdqa	-64(%rbp), %xmm3
	movdqa	%xmm3, %xmm1
	punpckhdq	%xmm3, %xmm1
	movd	%xmm1, 8(%r12,%r9)
.L20:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L21
	pshufd	$255, -64(%rbp), %xmm0
	movd	%xmm0, 12(%r12,%r9)
	jmp	.L21
	.cfi_endproc
.LFE18781:
	.size	_ZN3hwy6N_SSE26detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, .-_ZN3hwy6N_SSE26detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	.section	.text._ZN3hwy6N_SSE26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0, @function
_ZN3hwy6N_SSE26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0:
.LFB18782:
	.cfi_startproc
	cmpq	%rdx, %rsi
	jbe	.L92
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r10
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rax, %rsi
	jbe	.L92
	movl	(%rdi,%rdx,4), %r11d
	movd	%r11d, %xmm6
	pshufd	$0, %xmm6, %xmm0
	jmp	.L95
	.p2align 4,,10
	.p2align 3
.L96:
	cmpq	%rcx, %rsi
	jbe	.L92
	movq	%rdx, %rax
.L101:
	movd	(%rdi,%r10,8), %xmm5
	pshufd	$0, %xmm5, %xmm1
	pcmpgtd	%xmm3, %xmm1
	movmskps	%xmm1, %r8d
	andl	$1, %r8d
	jne	.L98
.L97:
	cmpq	%rdx, %rax
	je	.L92
	leaq	(%rdi,%rax,4), %rdx
	movl	(%rdx), %ecx
	movl	%ecx, (%r9)
	movl	%r11d, (%rdx)
	cmpq	%rax, %rsi
	jbe	.L105
	movq	%rax, %rdx
.L99:
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r10
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rsi, %rax
	jnb	.L92
.L95:
	movd	(%rdi,%rax,4), %xmm4
	leaq	(%rdi,%rdx,4), %r9
	movdqa	%xmm0, %xmm3
	pshufd	$0, %xmm4, %xmm1
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm0, %xmm2
	movmskps	%xmm2, %r8d
	andl	$1, %r8d
	je	.L96
	cmpq	%rcx, %rsi
	jbe	.L97
	movdqa	%xmm1, %xmm3
	jmp	.L101
	.p2align 4,,10
	.p2align 3
.L98:
	cmpq	%rcx, %rdx
	je	.L106
	leaq	(%rdi,%rcx,4), %rax
	movl	(%rax), %edx
	movl	%edx, (%r9)
	movq	%rcx, %rdx
	movl	%r11d, (%rax)
	jmp	.L99
	.p2align 4,,10
	.p2align 3
.L92:
	ret
	.p2align 4,,10
	.p2align 3
.L105:
	ret
.L106:
	ret
	.cfi_endproc
.LFE18782:
	.size	_ZN3hwy6N_SSE26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0, .-_ZN3hwy6N_SSE26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	.section	.text._ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0, @function
_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0:
.LFB18783:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %rax
	salq	$2, %rax
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	.cfi_offset 15, -24
	leaq	(%rdi,%rax), %r15
	pushq	%r14
	.cfi_offset 14, -32
	leaq	(%r15,%rax), %r14
	pushq	%r13
	.cfi_offset 13, -40
	leaq	(%r14,%rax), %r13
	pushq	%r12
	.cfi_offset 12, -48
	leaq	0(%r13,%rax), %r12
	pushq	%rbx
	.cfi_offset 3, -56
	leaq	(%r12,%rax), %rbx
	leaq	(%rbx,%rax), %r11
	leaq	(%r11,%rax), %r10
	subq	$240, %rsp
	leaq	(%r10,%rax), %r9
	leaq	(%r9,%rax), %r8
	movq	%rdi, -264(%rbp)
	movq	%rsi, -240(%rbp)
	movdqu	(%r15), %xmm6
	movdqu	(%rdi), %xmm12
	leaq	(%r8,%rax), %rdi
	leaq	(%rdi,%rax), %rsi
	movdqu	0(%r13), %xmm5
	movdqu	(%r14), %xmm14
	movdqa	%xmm6, %xmm8
	leaq	(%rsi,%rax), %rcx
	movdqu	(%rbx), %xmm3
	movdqu	(%r12), %xmm11
	pcmpgtd	%xmm12, %xmm8
	leaq	(%rcx,%rax), %rdx
	movdqu	(%r10), %xmm2
	movdqu	(%r11), %xmm10
	movdqu	(%rdx), %xmm0
	movdqu	(%rsi), %xmm4
	movq	%rdx, -248(%rbp)
	addq	%rax, %rdx
	movdqu	(%rdx), %xmm15
	movdqu	(%r8), %xmm1
	addq	%rdx, %rax
	movq	%rdx, -256(%rbp)
	movdqa	%xmm8, %xmm13
	movdqu	(%r9), %xmm7
	movdqu	(%rdi), %xmm9
	pandn	%xmm6, %xmm13
	movaps	%xmm15, -112(%rbp)
	pand	%xmm8, %xmm6
	movdqa	%xmm13, %xmm15
	movdqa	%xmm12, %xmm13
	pand	%xmm8, %xmm13
	por	%xmm15, %xmm13
	movdqa	%xmm8, %xmm15
	pandn	%xmm12, %xmm15
	movdqa	%xmm5, %xmm12
	pcmpgtd	%xmm14, %xmm12
	por	%xmm15, %xmm6
	movdqa	%xmm12, %xmm8
	pandn	%xmm5, %xmm8
	pand	%xmm12, %xmm5
	movdqa	%xmm8, %xmm15
	movdqa	%xmm14, %xmm8
	pand	%xmm12, %xmm8
	por	%xmm15, %xmm8
	movdqa	%xmm12, %xmm15
	pandn	%xmm14, %xmm15
	movdqa	%xmm3, %xmm14
	pcmpgtd	%xmm11, %xmm14
	por	%xmm15, %xmm5
	movdqa	%xmm14, %xmm12
	pandn	%xmm3, %xmm12
	pand	%xmm14, %xmm3
	movdqa	%xmm12, %xmm15
	movdqa	%xmm11, %xmm12
	pand	%xmm14, %xmm12
	por	%xmm15, %xmm12
	movdqa	%xmm14, %xmm15
	pandn	%xmm11, %xmm15
	movdqa	%xmm2, %xmm11
	pcmpgtd	%xmm10, %xmm11
	por	%xmm15, %xmm3
	movaps	%xmm3, -64(%rbp)
	movdqa	%xmm10, %xmm3
	movdqa	%xmm11, %xmm14
	pand	%xmm11, %xmm3
	pandn	%xmm2, %xmm14
	pand	%xmm11, %xmm2
	por	%xmm14, %xmm3
	movdqa	%xmm11, %xmm14
	pandn	%xmm10, %xmm14
	movdqa	%xmm1, %xmm10
	pcmpgtd	%xmm7, %xmm10
	por	%xmm14, %xmm2
	movdqa	%xmm10, %xmm11
	pandn	%xmm1, %xmm11
	pand	%xmm10, %xmm1
	movdqa	%xmm11, %xmm14
	movdqa	%xmm7, %xmm11
	pand	%xmm10, %xmm11
	por	%xmm14, %xmm11
	movdqa	%xmm10, %xmm14
	pandn	%xmm7, %xmm14
	movdqa	%xmm4, %xmm7
	pcmpgtd	%xmm9, %xmm7
	por	%xmm14, %xmm1
	movaps	%xmm1, -80(%rbp)
	movdqa	%xmm7, %xmm10
	movdqa	%xmm7, %xmm1
	movdqa	%xmm9, %xmm7
	pandn	%xmm4, %xmm10
	pand	%xmm1, %xmm7
	pand	%xmm1, %xmm4
	por	%xmm10, %xmm7
	movdqa	%xmm1, %xmm10
	movdqa	%xmm0, %xmm1
	pandn	%xmm9, %xmm10
	por	%xmm10, %xmm4
	movdqu	(%rcx), %xmm10
	pcmpgtd	%xmm10, %xmm1
	movdqu	(%rcx), %xmm10
	movdqu	(%rcx), %xmm15
	movdqa	%xmm1, %xmm9
	pand	%xmm1, %xmm10
	pandn	%xmm0, %xmm9
	pand	%xmm1, %xmm0
	por	%xmm9, %xmm10
	movdqa	%xmm1, %xmm9
	movdqu	(%rax), %xmm1
	pandn	%xmm15, %xmm9
	movdqa	-112(%rbp), %xmm15
	por	%xmm9, %xmm0
	pcmpgtd	%xmm15, %xmm1
	movaps	%xmm0, -96(%rbp)
	movdqu	(%rax), %xmm0
	movdqa	%xmm1, %xmm9
	pandn	%xmm0, %xmm9
	movdqa	%xmm15, %xmm0
	pand	%xmm1, %xmm0
	por	%xmm9, %xmm0
	movdqa	%xmm1, %xmm9
	pandn	%xmm15, %xmm9
	movdqu	(%rax), %xmm15
	pand	%xmm15, %xmm1
	movdqa	%xmm13, %xmm15
	por	%xmm9, %xmm1
	movdqa	%xmm8, %xmm9
	pcmpgtd	%xmm13, %xmm9
	movdqa	%xmm9, %xmm14
	pandn	%xmm8, %xmm9
	pand	%xmm14, %xmm15
	pand	%xmm14, %xmm8
	por	%xmm15, %xmm9
	movdqa	%xmm14, %xmm15
	movdqa	%xmm6, %xmm14
	pandn	%xmm13, %xmm15
	movdqa	%xmm5, %xmm13
	pcmpgtd	%xmm6, %xmm13
	por	%xmm8, %xmm15
	movdqa	%xmm13, %xmm8
	pand	%xmm13, %xmm14
	pandn	%xmm5, %xmm8
	pand	%xmm13, %xmm5
	por	%xmm14, %xmm8
	movdqa	%xmm13, %xmm14
	pandn	%xmm6, %xmm14
	movdqa	%xmm3, %xmm6
	pcmpgtd	%xmm12, %xmm6
	por	%xmm14, %xmm5
	movdqa	-64(%rbp), %xmm14
	movaps	%xmm5, -112(%rbp)
	movdqa	%xmm12, %xmm5
	movdqa	%xmm6, %xmm13
	pand	%xmm6, %xmm5
	pandn	%xmm3, %xmm13
	pand	%xmm6, %xmm3
	por	%xmm13, %xmm5
	movdqa	%xmm6, %xmm13
	movdqa	%xmm14, %xmm6
	pandn	%xmm12, %xmm13
	movdqa	%xmm2, %xmm12
	pcmpgtd	%xmm14, %xmm12
	por	%xmm13, %xmm3
	movdqa	%xmm12, %xmm13
	pand	%xmm12, %xmm6
	pandn	%xmm2, %xmm13
	pand	%xmm12, %xmm2
	por	%xmm13, %xmm6
	movdqa	%xmm12, %xmm13
	pandn	%xmm14, %xmm13
	movdqa	%xmm11, %xmm14
	por	%xmm13, %xmm2
	movdqa	%xmm7, %xmm13
	pcmpgtd	%xmm11, %xmm13
	movdqa	%xmm13, %xmm12
	pand	%xmm13, %xmm14
	pandn	%xmm7, %xmm12
	pand	%xmm13, %xmm7
	por	%xmm14, %xmm12
	movdqa	%xmm13, %xmm14
	pandn	%xmm11, %xmm14
	movdqa	%xmm4, %xmm11
	por	%xmm14, %xmm7
	movdqa	-80(%rbp), %xmm14
	movaps	%xmm7, -128(%rbp)
	pcmpgtd	%xmm14, %xmm11
	movdqa	%xmm14, %xmm13
	movdqa	%xmm11, %xmm7
	pand	%xmm11, %xmm13
	pandn	%xmm4, %xmm7
	pand	%xmm11, %xmm4
	por	%xmm13, %xmm7
	movdqa	%xmm11, %xmm13
	movdqa	%xmm0, %xmm11
	pcmpgtd	%xmm10, %xmm11
	pandn	%xmm14, %xmm13
	movdqa	-96(%rbp), %xmm14
	por	%xmm13, %xmm4
	movaps	%xmm4, -80(%rbp)
	movdqa	%xmm10, %xmm4
	movdqa	%xmm11, %xmm13
	pand	%xmm11, %xmm4
	pandn	%xmm0, %xmm13
	pand	%xmm11, %xmm0
	por	%xmm13, %xmm4
	movdqa	%xmm11, %xmm13
	movdqa	%xmm1, %xmm11
	pcmpgtd	%xmm14, %xmm11
	pandn	%xmm10, %xmm13
	movdqa	%xmm14, %xmm10
	por	%xmm13, %xmm0
	movdqa	%xmm11, %xmm13
	pand	%xmm11, %xmm10
	pandn	%xmm1, %xmm13
	pand	%xmm11, %xmm1
	por	%xmm13, %xmm10
	movdqa	%xmm11, %xmm13
	pandn	%xmm14, %xmm13
	movdqa	%xmm9, %xmm14
	por	%xmm13, %xmm1
	movdqa	%xmm5, %xmm13
	pcmpgtd	%xmm9, %xmm13
	movdqa	%xmm13, %xmm11
	pand	%xmm13, %xmm14
	pandn	%xmm5, %xmm11
	pand	%xmm13, %xmm5
	por	%xmm14, %xmm11
	movdqa	%xmm13, %xmm14
	movdqa	%xmm8, %xmm13
	pandn	%xmm9, %xmm14
	movdqa	%xmm6, %xmm9
	pcmpgtd	%xmm8, %xmm9
	por	%xmm5, %xmm14
	movdqa	%xmm9, %xmm5
	pand	%xmm9, %xmm13
	pandn	%xmm6, %xmm5
	pand	%xmm9, %xmm6
	por	%xmm13, %xmm5
	movdqa	%xmm9, %xmm13
	movdqa	%xmm15, %xmm9
	pandn	%xmm8, %xmm13
	movdqa	%xmm3, %xmm8
	pcmpgtd	%xmm15, %xmm8
	por	%xmm13, %xmm6
	movaps	%xmm6, -96(%rbp)
	movdqa	%xmm8, %xmm6
	pand	%xmm8, %xmm9
	pandn	%xmm3, %xmm6
	pand	%xmm8, %xmm3
	por	%xmm9, %xmm6
	movdqa	%xmm8, %xmm9
	movdqa	%xmm2, %xmm8
	pandn	%xmm15, %xmm9
	movdqa	-112(%rbp), %xmm15
	por	%xmm9, %xmm3
	pcmpgtd	%xmm15, %xmm8
	movaps	%xmm3, -144(%rbp)
	movdqa	%xmm15, %xmm9
	movdqa	%xmm8, %xmm3
	pand	%xmm8, %xmm9
	pandn	%xmm2, %xmm3
	pand	%xmm8, %xmm2
	por	%xmm9, %xmm3
	movdqa	%xmm8, %xmm9
	movdqa	%xmm12, %xmm8
	pandn	%xmm15, %xmm9
	movdqa	-128(%rbp), %xmm15
	por	%xmm9, %xmm2
	movaps	%xmm2, -64(%rbp)
	movdqa	%xmm4, %xmm2
	pcmpgtd	%xmm12, %xmm2
	movdqa	%xmm2, %xmm9
	pand	%xmm2, %xmm8
	pandn	%xmm4, %xmm9
	pand	%xmm2, %xmm4
	por	%xmm9, %xmm8
	movdqa	%xmm2, %xmm9
	movdqa	%xmm10, %xmm2
	pcmpgtd	%xmm7, %xmm2
	pandn	%xmm12, %xmm9
	por	%xmm9, %xmm4
	movdqa	%xmm7, %xmm9
	movdqa	%xmm2, %xmm12
	pand	%xmm2, %xmm9
	pandn	%xmm10, %xmm12
	pand	%xmm2, %xmm10
	por	%xmm12, %xmm9
	movdqa	%xmm2, %xmm12
	movdqa	%xmm15, %xmm2
	pandn	%xmm7, %xmm12
	movdqa	%xmm0, %xmm7
	pcmpgtd	%xmm15, %xmm7
	por	%xmm12, %xmm10
	movdqa	%xmm7, %xmm12
	pand	%xmm7, %xmm2
	pandn	%xmm0, %xmm12
	pand	%xmm7, %xmm0
	por	%xmm12, %xmm2
	movdqa	%xmm7, %xmm12
	pandn	%xmm15, %xmm12
	movdqa	-80(%rbp), %xmm15
	por	%xmm12, %xmm0
	movdqa	%xmm1, %xmm12
	pcmpgtd	%xmm15, %xmm12
	movdqa	%xmm12, %xmm7
	pandn	%xmm1, %xmm7
	pand	%xmm12, %xmm1
	movdqa	%xmm7, %xmm13
	movdqa	%xmm15, %xmm7
	pand	%xmm12, %xmm7
	por	%xmm13, %xmm7
	movdqa	%xmm12, %xmm13
	movdqa	%xmm8, %xmm12
	pcmpgtd	%xmm11, %xmm12
	pandn	%xmm15, %xmm13
	por	%xmm13, %xmm1
	movdqa	%xmm12, %xmm15
	pandn	%xmm8, %xmm15
	pand	%xmm12, %xmm8
	movdqa	%xmm15, %xmm13
	movdqa	%xmm11, %xmm15
	pand	%xmm12, %xmm15
	por	%xmm13, %xmm15
	movaps	%xmm15, -112(%rbp)
	movdqa	%xmm12, %xmm15
	movdqa	%xmm5, %xmm12
	pandn	%xmm11, %xmm15
	movdqa	%xmm15, %xmm13
	movdqa	%xmm8, %xmm15
	movdqa	%xmm9, %xmm8
	pcmpgtd	%xmm5, %xmm8
	por	%xmm13, %xmm15
	movdqa	-96(%rbp), %xmm13
	movdqa	%xmm8, %xmm11
	pand	%xmm8, %xmm12
	pandn	%xmm9, %xmm11
	pand	%xmm8, %xmm9
	por	%xmm11, %xmm12
	movdqa	%xmm8, %xmm11
	movdqa	%xmm2, %xmm8
	pcmpgtd	%xmm6, %xmm8
	pandn	%xmm5, %xmm11
	movdqa	%xmm6, %xmm5
	movaps	%xmm12, -128(%rbp)
	por	%xmm11, %xmm9
	movdqa	%xmm8, %xmm11
	pand	%xmm8, %xmm5
	pandn	%xmm2, %xmm11
	pand	%xmm8, %xmm2
	por	%xmm11, %xmm5
	movdqa	%xmm8, %xmm11
	movdqa	%xmm7, %xmm8
	pcmpgtd	%xmm3, %xmm8
	pandn	%xmm6, %xmm11
	movdqa	%xmm3, %xmm6
	movaps	%xmm5, -80(%rbp)
	por	%xmm11, %xmm2
	movdqa	%xmm8, %xmm11
	pand	%xmm8, %xmm6
	pandn	%xmm7, %xmm11
	pand	%xmm8, %xmm7
	por	%xmm11, %xmm6
	movdqa	%xmm8, %xmm11
	movdqa	%xmm4, %xmm8
	pcmpgtd	%xmm14, %xmm8
	pandn	%xmm3, %xmm11
	movdqa	%xmm14, %xmm3
	por	%xmm11, %xmm7
	movdqa	%xmm8, %xmm11
	pand	%xmm8, %xmm3
	pandn	%xmm4, %xmm11
	pand	%xmm8, %xmm4
	por	%xmm11, %xmm3
	movdqa	%xmm8, %xmm11
	movdqa	%xmm13, %xmm8
	pandn	%xmm14, %xmm11
	movdqa	-144(%rbp), %xmm14
	por	%xmm11, %xmm4
	movdqa	%xmm10, %xmm11
	pcmpgtd	%xmm13, %xmm11
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm8
	pandn	%xmm10, %xmm12
	pand	%xmm11, %xmm10
	por	%xmm12, %xmm8
	movdqa	%xmm11, %xmm12
	movdqa	%xmm0, %xmm11
	pcmpgtd	%xmm14, %xmm11
	pandn	%xmm13, %xmm12
	por	%xmm12, %xmm10
	movdqa	%xmm14, %xmm12
	movdqa	%xmm11, %xmm13
	pand	%xmm11, %xmm12
	pandn	%xmm0, %xmm13
	pand	%xmm11, %xmm0
	por	%xmm13, %xmm12
	movdqa	%xmm11, %xmm13
	pandn	%xmm14, %xmm13
	movdqa	-64(%rbp), %xmm14
	por	%xmm13, %xmm0
	movdqa	%xmm1, %xmm13
	pcmpgtd	%xmm14, %xmm13
	movdqa	%xmm13, %xmm11
	movdqa	%xmm13, %xmm5
	pandn	-64(%rbp), %xmm5
	pandn	%xmm1, %xmm11
	pand	%xmm13, %xmm1
	pand	%xmm13, %xmm14
	por	%xmm5, %xmm1
	por	%xmm14, %xmm11
	movdqa	%xmm8, %xmm13
	movaps	%xmm1, -192(%rbp)
	movdqa	%xmm2, %xmm1
	pcmpgtd	%xmm8, %xmm1
	movdqa	%xmm1, %xmm14
	pand	%xmm1, %xmm13
	pandn	%xmm2, %xmm14
	pand	%xmm1, %xmm2
	por	%xmm14, %xmm13
	movdqa	%xmm1, %xmm14
	movdqa	%xmm12, %xmm1
	pandn	%xmm8, %xmm14
	movdqa	%xmm9, %xmm8
	pcmpgtd	%xmm12, %xmm8
	por	%xmm14, %xmm2
	movdqa	%xmm8, %xmm14
	pand	%xmm8, %xmm1
	pandn	%xmm9, %xmm14
	pand	%xmm8, %xmm9
	por	%xmm14, %xmm1
	movdqa	%xmm8, %xmm14
	movdqa	%xmm4, %xmm8
	pcmpgtd	%xmm6, %xmm8
	pandn	%xmm12, %xmm14
	por	%xmm14, %xmm9
	movdqa	%xmm6, %xmm14
	movdqa	%xmm8, %xmm5
	pand	%xmm8, %xmm14
	pandn	%xmm4, %xmm5
	pand	%xmm8, %xmm4
	por	%xmm5, %xmm14
	movdqa	%xmm8, %xmm5
	movdqa	%xmm11, %xmm8
	pandn	%xmm6, %xmm5
	movdqa	%xmm7, %xmm6
	pcmpgtd	%xmm11, %xmm6
	por	%xmm5, %xmm4
	movdqa	%xmm6, %xmm5
	pand	%xmm6, %xmm8
	pandn	%xmm7, %xmm5
	pand	%xmm6, %xmm7
	por	%xmm5, %xmm8
	movdqa	%xmm6, %xmm5
	movdqa	%xmm10, %xmm6
	pandn	%xmm11, %xmm5
	movdqa	%xmm0, %xmm11
	pcmpgtd	%xmm10, %xmm11
	por	%xmm5, %xmm7
	movdqa	%xmm11, %xmm5
	pand	%xmm11, %xmm6
	pandn	%xmm0, %xmm5
	pand	%xmm11, %xmm0
	por	%xmm5, %xmm6
	movdqa	%xmm11, %xmm5
	movdqa	%xmm15, %xmm11
	pcmpgtd	%xmm3, %xmm11
	pandn	%xmm10, %xmm5
	movdqa	%xmm3, %xmm10
	por	%xmm5, %xmm0
	movaps	%xmm0, -64(%rbp)
	movdqa	-128(%rbp), %xmm0
	movdqa	%xmm11, %xmm5
	pand	%xmm11, %xmm10
	pandn	%xmm15, %xmm5
	pand	%xmm11, %xmm15
	por	%xmm5, %xmm10
	movdqa	%xmm11, %xmm5
	pandn	%xmm3, %xmm5
	movdqa	%xmm0, %xmm3
	por	%xmm5, %xmm15
	movdqa	-80(%rbp), %xmm5
	movdqa	%xmm5, %xmm11
	pcmpgtd	%xmm0, %xmm11
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm3
	pandn	%xmm5, %xmm12
	movdqa	%xmm11, %xmm5
	pandn	%xmm0, %xmm5
	por	%xmm12, %xmm3
	movdqa	%xmm5, %xmm12
	movdqa	-80(%rbp), %xmm5
	movdqa	%xmm3, %xmm0
	pand	%xmm11, %xmm5
	movdqa	%xmm10, %xmm11
	pcmpgtd	%xmm3, %xmm11
	por	%xmm12, %xmm5
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm0
	pandn	%xmm10, %xmm12
	pand	%xmm11, %xmm10
	por	%xmm0, %xmm12
	movdqa	-64(%rbp), %xmm0
	movaps	%xmm12, -128(%rbp)
	movdqa	%xmm11, %xmm12
	movdqa	%xmm6, %xmm11
	pcmpgtd	%xmm8, %xmm11
	pandn	%xmm3, %xmm12
	movdqa	%xmm8, %xmm3
	por	%xmm12, %xmm10
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm3
	pandn	%xmm6, %xmm12
	pand	%xmm11, %xmm6
	por	%xmm12, %xmm3
	movdqa	%xmm11, %xmm12
	movdqa	%xmm15, %xmm11
	pcmpgtd	%xmm5, %xmm11
	pandn	%xmm8, %xmm12
	movdqa	%xmm5, %xmm8
	por	%xmm12, %xmm6
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm8
	pandn	%xmm15, %xmm12
	pand	%xmm11, %xmm15
	por	%xmm12, %xmm8
	movdqa	%xmm11, %xmm12
	movdqa	%xmm7, %xmm11
	pandn	%xmm5, %xmm12
	movdqa	%xmm0, %xmm5
	pcmpgtd	%xmm7, %xmm5
	por	%xmm12, %xmm15
	movdqa	%xmm5, %xmm12
	pand	%xmm5, %xmm11
	pandn	%xmm0, %xmm12
	pand	%xmm5, %xmm0
	por	%xmm12, %xmm11
	movdqa	%xmm5, %xmm12
	pandn	%xmm7, %xmm12
	movdqa	%xmm8, %xmm7
	por	%xmm12, %xmm0
	movdqa	%xmm1, %xmm12
	movaps	%xmm0, -208(%rbp)
	movdqa	%xmm10, %xmm0
	pcmpgtd	%xmm13, %xmm12
	cmpq	$1, -240(%rbp)
	pcmpgtd	%xmm8, %xmm0
	movdqa	%xmm0, %xmm5
	pand	%xmm0, %xmm7
	pandn	%xmm10, %xmm5
	pand	%xmm0, %xmm10
	por	%xmm5, %xmm7
	movdqa	%xmm0, %xmm5
	movdqa	%xmm12, %xmm0
	pandn	%xmm8, %xmm5
	pandn	%xmm1, %xmm0
	pand	%xmm12, %xmm1
	movaps	%xmm7, -144(%rbp)
	por	%xmm5, %xmm10
	movdqa	%xmm13, %xmm5
	movdqa	%xmm9, %xmm7
	pand	%xmm12, %xmm5
	movdqa	%xmm11, %xmm8
	por	%xmm0, %xmm5
	movdqa	%xmm12, %xmm0
	pandn	%xmm13, %xmm0
	movdqa	%xmm6, %xmm13
	por	%xmm0, %xmm1
	pcmpgtd	%xmm11, %xmm13
	movdqa	%xmm2, %xmm0
	pcmpgtd	%xmm9, %xmm0
	movdqa	%xmm1, %xmm12
	pand	%xmm13, %xmm8
	movdqa	%xmm0, %xmm1
	pand	%xmm0, %xmm7
	pandn	%xmm2, %xmm1
	pand	%xmm0, %xmm2
	por	%xmm1, %xmm7
	movdqa	%xmm0, %xmm1
	movdqa	%xmm13, %xmm0
	pandn	%xmm9, %xmm1
	pandn	%xmm6, %xmm0
	movdqa	%xmm14, %xmm9
	por	%xmm1, %xmm2
	movdqa	%xmm15, %xmm1
	por	%xmm0, %xmm8
	pcmpgtd	%xmm14, %xmm1
	movdqa	%xmm13, %xmm0
	pand	%xmm6, %xmm13
	pandn	%xmm11, %xmm0
	movdqa	%xmm3, %xmm6
	movdqa	%xmm7, %xmm11
	por	%xmm0, %xmm13
	movdqa	%xmm1, %xmm0
	pand	%xmm1, %xmm9
	pandn	%xmm15, %xmm0
	pand	%xmm1, %xmm15
	por	%xmm0, %xmm9
	movdqa	%xmm1, %xmm0
	pandn	%xmm14, %xmm0
	movdqa	%xmm9, %xmm14
	por	%xmm0, %xmm15
	movdqa	%xmm4, %xmm0
	pcmpgtd	%xmm3, %xmm0
	movdqa	%xmm0, %xmm1
	pand	%xmm0, %xmm6
	pandn	%xmm4, %xmm1
	pand	%xmm0, %xmm4
	por	%xmm1, %xmm6
	movdqa	%xmm0, %xmm1
	movdqa	%xmm5, %xmm0
	pcmpgtd	%xmm9, %xmm0
	pcmpgtd	%xmm6, %xmm11
	pandn	%xmm3, %xmm1
	por	%xmm1, %xmm4
	movdqa	%xmm12, %xmm3
	movdqa	%xmm0, %xmm1
	pand	%xmm0, %xmm14
	pandn	%xmm5, %xmm1
	pand	%xmm0, %xmm5
	por	%xmm1, %xmm14
	movdqa	%xmm0, %xmm1
	pandn	%xmm9, %xmm1
	movdqa	%xmm6, %xmm9
	por	%xmm1, %xmm5
	movdqa	%xmm15, %xmm1
	pand	%xmm11, %xmm9
	pcmpgtd	%xmm12, %xmm1
	movdqa	%xmm1, %xmm0
	pand	%xmm1, %xmm3
	pandn	%xmm15, %xmm0
	por	%xmm0, %xmm3
	movdqa	%xmm1, %xmm0
	pand	%xmm15, %xmm1
	pandn	%xmm12, %xmm0
	por	%xmm0, %xmm1
	movdqa	%xmm11, %xmm0
	pandn	%xmm7, %xmm0
	pand	%xmm11, %xmm7
	por	%xmm0, %xmm9
	movdqa	%xmm11, %xmm0
	pandn	%xmm6, %xmm0
	por	%xmm0, %xmm7
	movdqa	%xmm4, %xmm0
	pcmpgtd	%xmm2, %xmm0
	movdqa	%xmm7, %xmm11
	movdqa	%xmm2, %xmm7
	movdqa	%xmm0, %xmm6
	movdqa	%xmm0, %xmm12
	pand	%xmm0, %xmm7
	pandn	%xmm2, %xmm6
	movdqa	%xmm10, %xmm2
	pand	%xmm4, %xmm0
	pcmpgtd	%xmm14, %xmm2
	por	%xmm6, %xmm0
	pandn	%xmm4, %xmm12
	movdqa	%xmm14, %xmm6
	por	%xmm12, %xmm7
	movdqa	%xmm2, %xmm4
	pand	%xmm2, %xmm6
	pandn	%xmm10, %xmm4
	pand	%xmm2, %xmm10
	por	%xmm4, %xmm6
	movdqa	%xmm2, %xmm4
	movdqa	%xmm3, %xmm2
	pcmpgtd	%xmm5, %xmm2
	pandn	%xmm14, %xmm4
	movaps	%xmm6, -160(%rbp)
	movdqa	%xmm5, %xmm6
	por	%xmm4, %xmm10
	movaps	%xmm10, -64(%rbp)
	movdqa	%xmm8, %xmm10
	movdqa	%xmm2, %xmm4
	pand	%xmm2, %xmm6
	pandn	%xmm3, %xmm4
	pand	%xmm2, %xmm3
	por	%xmm4, %xmm6
	movdqa	%xmm2, %xmm4
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm9, %xmm2
	pandn	%xmm5, %xmm4
	movaps	%xmm6, -80(%rbp)
	movdqa	%xmm9, %xmm5
	por	%xmm4, %xmm3
	movdqa	%xmm7, %xmm6
	pcmpgtd	%xmm11, %xmm6
	movdqa	%xmm2, %xmm4
	pand	%xmm2, %xmm5
	pandn	%xmm1, %xmm4
	pand	%xmm2, %xmm1
	por	%xmm4, %xmm5
	movdqa	%xmm2, %xmm4
	movdqa	%xmm11, %xmm2
	pandn	%xmm9, %xmm4
	pand	%xmm6, %xmm2
	por	%xmm4, %xmm1
	movdqa	%xmm6, %xmm4
	pandn	%xmm7, %xmm4
	pand	%xmm6, %xmm7
	por	%xmm4, %xmm2
	movdqa	%xmm6, %xmm4
	pandn	%xmm11, %xmm4
	movdqa	%xmm2, %xmm9
	por	%xmm4, %xmm7
	pcmpgtd	%xmm1, %xmm9
	movdqa	%xmm0, %xmm4
	pcmpgtd	%xmm8, %xmm4
	movdqa	%xmm4, %xmm6
	pand	%xmm4, %xmm10
	pandn	%xmm0, %xmm6
	pand	%xmm4, %xmm0
	por	%xmm6, %xmm10
	movdqa	%xmm4, %xmm6
	movdqa	%xmm5, %xmm4
	pcmpgtd	%xmm3, %xmm4
	pandn	%xmm8, %xmm6
	movdqa	%xmm3, %xmm8
	por	%xmm6, %xmm0
	movdqa	%xmm4, %xmm6
	pand	%xmm4, %xmm8
	pandn	%xmm5, %xmm6
	pand	%xmm4, %xmm5
	por	%xmm6, %xmm8
	movdqa	%xmm4, %xmm6
	movdqa	%xmm9, %xmm4
	pandn	%xmm3, %xmm6
	movdqa	%xmm1, %xmm3
	pandn	%xmm2, %xmm4
	movaps	%xmm8, -96(%rbp)
	pand	%xmm9, %xmm3
	por	%xmm6, %xmm5
	pand	%xmm9, %xmm2
	por	%xmm4, %xmm3
	movdqa	%xmm9, %xmm4
	movdqa	%xmm5, %xmm12
	pandn	%xmm1, %xmm4
	por	%xmm4, %xmm2
	jbe	.L112
	movdqa	-112(%rbp), %xmm5
	pshufd	$177, %xmm13, %xmm14
	pshufd	$177, -192(%rbp), %xmm13
	movdqa	%xmm13, %xmm8
	pshufd	$177, %xmm3, %xmm6
	pshufd	$177, %xmm0, %xmm0
	pshufd	$177, %xmm7, %xmm7
	pshufd	$177, -208(%rbp), %xmm15
	pcmpgtd	%xmm5, %xmm8
	movaps	%xmm6, -176(%rbp)
	movdqa	%xmm5, %xmm4
	movdqa	%xmm15, %xmm9
	movdqa	-160(%rbp), %xmm3
	pshufd	$177, %xmm10, %xmm10
	pshufd	$177, %xmm2, %xmm2
	movdqa	%xmm8, %xmm6
	movdqa	%xmm8, %xmm1
	pand	%xmm8, %xmm4
	pandn	%xmm5, %xmm6
	movdqa	-128(%rbp), %xmm5
	pandn	%xmm13, %xmm1
	pand	%xmm13, %xmm8
	por	%xmm1, %xmm4
	movaps	%xmm6, -208(%rbp)
	movdqa	-144(%rbp), %xmm6
	pcmpgtd	%xmm5, %xmm9
	movdqa	%xmm5, %xmm11
	movaps	%xmm4, -192(%rbp)
	movdqa	%xmm6, %xmm4
	movdqa	%xmm9, %xmm1
	pand	%xmm9, %xmm11
	pandn	%xmm15, %xmm1
	por	%xmm1, %xmm11
	movdqa	%xmm9, %xmm1
	pand	%xmm15, %xmm9
	pandn	%xmm5, %xmm1
	movdqa	%xmm14, %xmm5
	pcmpgtd	%xmm6, %xmm5
	movaps	%xmm1, -224(%rbp)
	movdqa	%xmm5, %xmm1
	pand	%xmm5, %xmm4
	pandn	%xmm14, %xmm1
	por	%xmm1, %xmm4
	movdqa	%xmm5, %xmm1
	pand	%xmm14, %xmm5
	pandn	%xmm6, %xmm1
	movaps	%xmm4, -112(%rbp)
	movdqa	%xmm3, %xmm6
	movaps	%xmm1, -288(%rbp)
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm3, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm6
	pandn	%xmm0, %xmm4
	por	%xmm4, %xmm6
	movdqa	%xmm1, %xmm4
	pand	%xmm0, %xmm1
	pandn	%xmm3, %xmm4
	movaps	%xmm6, -128(%rbp)
	movdqa	-64(%rbp), %xmm6
	movaps	%xmm4, -304(%rbp)
	movdqa	%xmm10, %xmm4
	por	-304(%rbp), %xmm1
	pcmpgtd	%xmm6, %xmm4
	pshufd	$177, %xmm1, %xmm1
	movdqa	%xmm4, %xmm3
	pandn	%xmm10, %xmm3
	pand	%xmm4, %xmm10
	movaps	%xmm3, -320(%rbp)
	movdqa	%xmm4, %xmm3
	pand	-64(%rbp), %xmm4
	por	-320(%rbp), %xmm4
	pandn	%xmm6, %xmm3
	por	%xmm3, %xmm10
	movdqa	%xmm7, %xmm3
	pshufd	$177, %xmm4, %xmm4
	movaps	%xmm10, -144(%rbp)
	movdqa	-80(%rbp), %xmm10
	pcmpgtd	%xmm10, %xmm3
	movdqa	%xmm3, %xmm6
	pandn	%xmm7, %xmm3
	movaps	%xmm3, -336(%rbp)
	movdqa	%xmm6, %xmm3
	pand	%xmm6, %xmm7
	pand	-80(%rbp), %xmm6
	pandn	%xmm10, %xmm3
	por	%xmm3, %xmm7
	movdqa	%xmm2, %xmm3
	movaps	%xmm7, -160(%rbp)
	movdqa	-96(%rbp), %xmm7
	pcmpgtd	%xmm7, %xmm3
	movdqa	%xmm3, %xmm10
	pandn	%xmm2, %xmm3
	movaps	%xmm3, -352(%rbp)
	movdqa	%xmm10, %xmm3
	pand	%xmm10, %xmm2
	pandn	%xmm7, %xmm3
	movdqa	-176(%rbp), %xmm7
	por	%xmm3, %xmm2
	pcmpgtd	%xmm12, %xmm7
	movdqa	%xmm7, %xmm3
	pandn	-176(%rbp), %xmm3
	movaps	%xmm3, -368(%rbp)
	movdqa	%xmm7, %xmm3
	pandn	%xmm12, %xmm3
	movaps	%xmm3, -384(%rbp)
	movdqa	-176(%rbp), %xmm3
	pand	%xmm7, %xmm3
	pand	%xmm12, %xmm7
	por	-384(%rbp), %xmm3
	por	-336(%rbp), %xmm6
	por	-368(%rbp), %xmm7
	movdqa	-192(%rbp), %xmm13
	pand	-96(%rbp), %xmm10
	pshufd	$177, %xmm7, %xmm7
	pshufd	$177, %xmm6, %xmm6
	por	-352(%rbp), %xmm10
	movdqa	%xmm7, %xmm0
	por	-288(%rbp), %xmm5
	por	-224(%rbp), %xmm9
	pcmpgtd	%xmm13, %xmm0
	pshufd	$177, %xmm10, %xmm14
	por	-208(%rbp), %xmm8
	pshufd	$177, %xmm9, %xmm15
	movdqa	%xmm13, %xmm9
	movaps	%xmm14, -64(%rbp)
	pshufd	$177, %xmm5, %xmm5
	pshufd	$177, %xmm8, %xmm12
	movdqa	%xmm3, %xmm8
	movdqa	%xmm0, %xmm10
	pandn	%xmm7, %xmm0
	pand	%xmm10, %xmm9
	por	%xmm0, %xmm9
	movdqa	%xmm10, %xmm0
	pand	%xmm7, %xmm10
	pandn	%xmm13, %xmm0
	movdqa	%xmm12, %xmm13
	pcmpgtd	%xmm3, %xmm13
	movaps	%xmm0, -80(%rbp)
	movdqa	%xmm13, %xmm0
	pand	%xmm13, %xmm8
	pandn	%xmm12, %xmm0
	por	%xmm0, %xmm8
	movdqa	%xmm13, %xmm0
	pand	%xmm12, %xmm13
	pandn	%xmm3, %xmm0
	movaps	%xmm8, -224(%rbp)
	movdqa	%xmm11, %xmm8
	movaps	%xmm0, -304(%rbp)
	movdqa	%xmm14, %xmm0
	pcmpgtd	%xmm11, %xmm0
	movdqa	%xmm0, %xmm3
	pandn	-64(%rbp), %xmm3
	pand	%xmm0, %xmm8
	por	%xmm3, %xmm8
	movdqa	%xmm0, %xmm3
	pand	-64(%rbp), %xmm0
	pandn	%xmm11, %xmm3
	movdqa	%xmm2, %xmm11
	movaps	%xmm8, -96(%rbp)
	movaps	%xmm3, -320(%rbp)
	movdqa	%xmm15, %xmm3
	por	-320(%rbp), %xmm0
	pcmpgtd	%xmm2, %xmm3
	pshufd	$177, %xmm0, %xmm0
	movdqa	%xmm3, %xmm14
	pand	%xmm3, %xmm11
	pandn	%xmm15, %xmm14
	por	%xmm14, %xmm11
	movdqa	%xmm3, %xmm14
	pand	%xmm15, %xmm3
	movaps	%xmm11, -176(%rbp)
	movdqa	-112(%rbp), %xmm11
	pandn	%xmm2, %xmm14
	movdqa	%xmm6, %xmm2
	movaps	%xmm14, -336(%rbp)
	pcmpgtd	%xmm11, %xmm2
	movdqa	%xmm2, %xmm14
	movdqa	%xmm2, %xmm8
	pandn	%xmm6, %xmm14
	pand	%xmm2, %xmm6
	pandn	%xmm11, %xmm8
	movaps	%xmm14, -352(%rbp)
	movdqa	%xmm6, %xmm14
	movdqa	%xmm5, %xmm6
	pand	-112(%rbp), %xmm2
	por	%xmm8, %xmm14
	por	-352(%rbp), %xmm2
	movdqa	-160(%rbp), %xmm8
	movaps	%xmm14, -192(%rbp)
	pcmpgtd	%xmm8, %xmm6
	pshufd	$177, %xmm2, %xmm2
	movdqa	%xmm6, %xmm14
	pandn	%xmm5, %xmm6
	movaps	%xmm6, -368(%rbp)
	movdqa	%xmm14, %xmm6
	pand	%xmm14, %xmm5
	pandn	%xmm8, %xmm6
	por	%xmm6, %xmm5
	movdqa	-128(%rbp), %xmm6
	movaps	%xmm5, -208(%rbp)
	movdqa	%xmm4, %xmm5
	pcmpgtd	%xmm6, %xmm5
	movdqa	%xmm5, %xmm8
	pandn	%xmm4, %xmm5
	movdqa	%xmm5, %xmm11
	movdqa	%xmm8, %xmm5
	pand	%xmm8, %xmm4
	pandn	%xmm6, %xmm5
	pand	-128(%rbp), %xmm8
	por	%xmm5, %xmm4
	movdqa	%xmm1, %xmm5
	pcmpgtd	-144(%rbp), %xmm5
	movaps	%xmm4, -288(%rbp)
	por	%xmm11, %xmm8
	pshufd	$177, %xmm8, %xmm8
	movdqa	%xmm5, %xmm6
	movdqa	%xmm5, %xmm4
	pandn	-144(%rbp), %xmm4
	pandn	%xmm1, %xmm6
	por	-80(%rbp), %xmm10
	pand	%xmm5, %xmm1
	movdqa	-288(%rbp), %xmm15
	pand	-144(%rbp), %xmm5
	por	%xmm4, %xmm1
	por	-304(%rbp), %xmm13
	pshufd	$177, %xmm10, %xmm11
	movdqa	%xmm15, %xmm4
	por	-336(%rbp), %xmm3
	por	%xmm6, %xmm5
	pshufd	$177, %xmm13, %xmm10
	movdqa	-96(%rbp), %xmm6
	movaps	%xmm11, -128(%rbp)
	pshufd	$177, %xmm5, %xmm7
	movaps	%xmm10, -80(%rbp)
	movdqa	%xmm9, %xmm10
	movdqa	-192(%rbp), %xmm5
	movaps	%xmm7, -64(%rbp)
	movdqa	%xmm8, %xmm7
	pshufd	$177, %xmm3, %xmm3
	pand	-160(%rbp), %xmm14
	por	-368(%rbp), %xmm14
	pcmpgtd	%xmm9, %xmm7
	pshufd	$177, %xmm14, %xmm14
	movdqa	%xmm7, %xmm13
	pand	%xmm7, %xmm10
	pandn	%xmm8, %xmm13
	por	%xmm13, %xmm10
	movdqa	%xmm7, %xmm13
	pand	%xmm8, %xmm7
	pandn	%xmm9, %xmm13
	movdqa	%xmm2, %xmm9
	movdqa	%xmm10, %xmm8
	pcmpgtd	%xmm6, %xmm9
	movaps	%xmm13, -304(%rbp)
	movdqa	%xmm9, %xmm13
	pandn	%xmm2, %xmm9
	movdqa	%xmm13, %xmm12
	pand	%xmm13, %xmm2
	movaps	%xmm9, -320(%rbp)
	pandn	%xmm6, %xmm12
	por	%xmm12, %xmm2
	movdqa	%xmm11, %xmm12
	pcmpgtd	%xmm15, %xmm12
	movdqa	%xmm12, %xmm6
	pandn	-128(%rbp), %xmm12
	pand	%xmm6, %xmm4
	movdqa	%xmm4, %xmm9
	por	%xmm12, %xmm9
	movdqa	%xmm6, %xmm12
	pandn	%xmm15, %xmm12
	movdqa	-224(%rbp), %xmm15
	movaps	%xmm12, -288(%rbp)
	movdqa	%xmm0, %xmm12
	pcmpgtd	%xmm5, %xmm12
	movdqa	%xmm12, %xmm4
	pandn	%xmm0, %xmm4
	pand	%xmm12, %xmm0
	movaps	%xmm4, -336(%rbp)
	movdqa	%xmm12, %xmm4
	pandn	%xmm5, %xmm4
	por	%xmm4, %xmm0
	movdqa	-64(%rbp), %xmm4
	movaps	%xmm0, -144(%rbp)
	movdqa	%xmm4, %xmm11
	pcmpgtd	%xmm15, %xmm11
	movdqa	%xmm11, %xmm5
	pandn	-64(%rbp), %xmm11
	movdqa	%xmm11, %xmm4
	movdqa	%xmm15, %xmm11
	pand	%xmm5, %xmm11
	por	%xmm4, %xmm11
	movdqa	%xmm5, %xmm4
	pandn	%xmm15, %xmm4
	movaps	%xmm11, -160(%rbp)
	movdqa	-176(%rbp), %xmm15
	movdqa	%xmm14, %xmm11
	movaps	%xmm4, -352(%rbp)
	pcmpgtd	%xmm15, %xmm11
	movdqa	%xmm11, %xmm4
	pandn	%xmm14, %xmm4
	pand	%xmm11, %xmm14
	movaps	%xmm4, -368(%rbp)
	movdqa	%xmm11, %xmm4
	pandn	%xmm15, %xmm4
	movdqa	-80(%rbp), %xmm15
	por	%xmm4, %xmm14
	movdqa	%xmm15, %xmm4
	movaps	%xmm14, -112(%rbp)
	movdqa	%xmm1, %xmm15
	pcmpgtd	%xmm1, %xmm4
	movdqa	%xmm4, %xmm14
	pandn	-80(%rbp), %xmm14
	pand	%xmm4, %xmm15
	por	%xmm14, %xmm15
	movdqa	%xmm4, %xmm14
	pandn	%xmm1, %xmm14
	movdqa	-208(%rbp), %xmm1
	movaps	%xmm14, -384(%rbp)
	movdqa	%xmm3, %xmm14
	pcmpgtd	%xmm1, %xmm14
	movdqa	%xmm14, %xmm0
	pandn	%xmm3, %xmm0
	pand	%xmm14, %xmm3
	movaps	%xmm0, -400(%rbp)
	movdqa	%xmm14, %xmm0
	pandn	%xmm1, %xmm0
	por	%xmm0, %xmm3
	movaps	%xmm3, -224(%rbp)
	movdqa	-96(%rbp), %xmm3
	movdqa	-208(%rbp), %xmm1
	pand	-80(%rbp), %xmm4
	pand	-64(%rbp), %xmm5
	pand	%xmm13, %xmm3
	por	-384(%rbp), %xmm4
	por	-320(%rbp), %xmm3
	movdqa	-400(%rbp), %xmm13
	pand	%xmm14, %xmm1
	por	-304(%rbp), %xmm7
	pshufd	$177, %xmm3, %xmm3
	pand	-128(%rbp), %xmm6
	pand	-192(%rbp), %xmm12
	por	%xmm1, %xmm13
	pshufd	$177, %xmm4, %xmm1
	pshufd	$177, %xmm7, %xmm7
	movdqa	-144(%rbp), %xmm4
	movaps	%xmm1, -64(%rbp)
	movdqa	%xmm3, %xmm1
	por	-336(%rbp), %xmm12
	por	-288(%rbp), %xmm6
	pcmpgtd	%xmm10, %xmm1
	por	-352(%rbp), %xmm5
	pand	-176(%rbp), %xmm11
	pshufd	$177, %xmm12, %xmm12
	pshufd	$177, %xmm6, %xmm6
	por	-368(%rbp), %xmm11
	pshufd	$177, %xmm5, %xmm5
	pshufd	$177, %xmm13, %xmm13
	movdqa	%xmm1, %xmm0
	pand	%xmm1, %xmm8
	pshufd	$177, %xmm11, %xmm11
	pandn	%xmm3, %xmm0
	por	%xmm0, %xmm8
	movdqa	%xmm1, %xmm0
	pand	%xmm3, %xmm1
	pandn	%xmm10, %xmm0
	movdqa	%xmm7, %xmm10
	pcmpgtd	%xmm2, %xmm10
	por	%xmm0, %xmm1
	movdqa	%xmm2, %xmm0
	movdqa	%xmm10, %xmm3
	pand	%xmm10, %xmm0
	pandn	%xmm7, %xmm3
	por	%xmm0, %xmm3
	movdqa	%xmm10, %xmm0
	pand	%xmm7, %xmm10
	pandn	%xmm2, %xmm0
	movdqa	%xmm12, %xmm2
	pcmpgtd	%xmm9, %xmm2
	movdqa	%xmm0, %xmm14
	movdqa	%xmm4, %xmm0
	por	%xmm10, %xmm14
	movdqa	%xmm9, %xmm10
	movdqa	%xmm2, %xmm7
	pand	%xmm2, %xmm10
	pandn	%xmm12, %xmm7
	por	%xmm7, %xmm10
	movdqa	%xmm2, %xmm7
	pand	%xmm12, %xmm2
	pandn	%xmm9, %xmm7
	por	%xmm7, %xmm2
	movdqa	%xmm2, %xmm12
	movdqa	%xmm6, %xmm2
	pcmpgtd	%xmm4, %xmm2
	pand	%xmm2, %xmm0
	movdqa	%xmm2, %xmm7
	pandn	%xmm6, %xmm7
	movdqa	%xmm0, %xmm9
	movdqa	%xmm11, %xmm0
	por	%xmm7, %xmm9
	movdqa	%xmm2, %xmm7
	pand	%xmm6, %xmm2
	pandn	%xmm4, %xmm7
	movdqa	-160(%rbp), %xmm4
	por	%xmm2, %xmm7
	pcmpgtd	%xmm4, %xmm0
	movdqa	%xmm4, %xmm6
	movdqa	%xmm0, %xmm2
	pand	%xmm0, %xmm6
	pandn	%xmm11, %xmm2
	por	%xmm2, %xmm6
	movdqa	%xmm0, %xmm2
	pand	%xmm11, %xmm0
	pandn	%xmm4, %xmm2
	movaps	%xmm6, -288(%rbp)
	movdqa	-112(%rbp), %xmm6
	por	%xmm2, %xmm0
	movdqa	%xmm0, %xmm11
	movdqa	%xmm5, %xmm0
	pcmpgtd	%xmm6, %xmm0
	movdqa	%xmm0, %xmm2
	pand	%xmm0, %xmm6
	movdqa	%xmm0, %xmm4
	pandn	%xmm5, %xmm2
	pand	%xmm5, %xmm0
	movdqa	%xmm13, %xmm5
	pcmpgtd	%xmm15, %xmm5
	pandn	-112(%rbp), %xmm4
	por	%xmm2, %xmm6
	por	%xmm4, %xmm0
	movaps	%xmm0, -304(%rbp)
	movdqa	%xmm5, %xmm2
	movdqa	%xmm5, %xmm0
	movdqa	%xmm15, %xmm5
	pandn	%xmm13, %xmm2
	pand	%xmm0, %xmm5
	por	%xmm2, %xmm5
	movdqa	%xmm0, %xmm2
	pand	%xmm13, %xmm0
	movaps	%xmm5, -320(%rbp)
	movdqa	-64(%rbp), %xmm5
	pandn	%xmm15, %xmm2
	movdqa	-224(%rbp), %xmm15
	movdqa	%xmm0, %xmm13
	movdqa	%xmm5, %xmm4
	por	%xmm2, %xmm13
	pcmpgtd	%xmm15, %xmm4
	movdqa	%xmm4, %xmm2
	movdqa	%xmm4, %xmm0
	movdqa	%xmm5, %xmm4
	pandn	%xmm5, %xmm2
	movdqa	%xmm15, %xmm5
	pand	%xmm0, %xmm5
	por	%xmm2, %xmm5
	movdqa	%xmm0, %xmm2
	pand	%xmm4, %xmm0
	pandn	%xmm15, %xmm2
	movdqa	%xmm0, %xmm15
	pshufd	$177, %xmm8, %xmm0
	movaps	%xmm5, -336(%rbp)
	por	%xmm2, %xmm15
	movdqa	%xmm0, %xmm2
	pcmpgtd	%xmm8, %xmm2
	movdqa	%xmm2, %xmm4
	movdqa	%xmm2, %xmm5
	pandn	%xmm0, %xmm4
	pandn	%xmm8, %xmm5
	pand	%xmm2, %xmm0
	pand	%xmm2, %xmm8
	por	%xmm5, %xmm0
	por	%xmm4, %xmm8
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm8, %xmm8
	punpckldq	%xmm0, %xmm8
	pshufd	$177, %xmm1, %xmm0
	movdqa	%xmm0, %xmm2
	movaps	%xmm8, -352(%rbp)
	pcmpgtd	%xmm1, %xmm2
	movaps	%xmm8, -112(%rbp)
	movdqa	%xmm2, %xmm4
	movdqa	%xmm2, %xmm8
	pandn	%xmm0, %xmm4
	pandn	%xmm1, %xmm8
	pand	%xmm2, %xmm0
	pand	%xmm2, %xmm1
	por	%xmm8, %xmm0
	por	%xmm4, %xmm1
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm1, %xmm1
	punpckldq	%xmm0, %xmm1
	pshufd	$177, %xmm3, %xmm0
	movaps	%xmm1, -368(%rbp)
	movaps	%xmm1, -128(%rbp)
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm3, %xmm1
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm4
	pandn	%xmm0, %xmm2
	pandn	%xmm3, %xmm4
	pand	%xmm1, %xmm0
	pand	%xmm1, %xmm3
	por	%xmm4, %xmm0
	por	%xmm2, %xmm3
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm3, %xmm3
	punpckldq	%xmm0, %xmm3
	pshufd	$177, %xmm14, %xmm0
	movdqa	%xmm0, %xmm1
	movaps	%xmm3, -384(%rbp)
	pcmpgtd	%xmm14, %xmm1
	movaps	%xmm3, -144(%rbp)
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm14, %xmm3
	pand	%xmm1, %xmm0
	pand	%xmm1, %xmm14
	por	%xmm3, %xmm0
	por	%xmm2, %xmm14
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm14, %xmm14
	punpckldq	%xmm0, %xmm14
	pshufd	$177, %xmm10, %xmm0
	movdqa	%xmm0, %xmm1
	movaps	%xmm14, -160(%rbp)
	cmpq	$3, -240(%rbp)
	pcmpgtd	%xmm10, %xmm1
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm10, %xmm3
	pand	%xmm1, %xmm0
	pand	%xmm1, %xmm10
	por	%xmm3, %xmm0
	por	%xmm2, %xmm10
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm10, %xmm10
	punpckldq	%xmm0, %xmm10
	pshufd	$177, %xmm12, %xmm0
	movdqa	%xmm0, %xmm1
	movaps	%xmm10, -176(%rbp)
	pcmpgtd	%xmm12, %xmm1
	movaps	%xmm10, -64(%rbp)
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm12, %xmm3
	pand	%xmm1, %xmm0
	pand	%xmm1, %xmm12
	por	%xmm3, %xmm0
	por	%xmm2, %xmm12
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm12, %xmm12
	punpckldq	%xmm0, %xmm12
	pshufd	$177, %xmm9, %xmm0
	movdqa	%xmm0, %xmm1
	movaps	%xmm12, -192(%rbp)
	pcmpgtd	%xmm9, %xmm1
	movaps	%xmm12, -80(%rbp)
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm9, %xmm3
	pand	%xmm1, %xmm0
	pand	%xmm1, %xmm9
	por	%xmm3, %xmm0
	por	%xmm2, %xmm9
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm9, %xmm9
	punpckldq	%xmm0, %xmm9
	pshufd	$177, %xmm7, %xmm0
	movdqa	%xmm0, %xmm1
	movaps	%xmm9, -208(%rbp)
	pcmpgtd	%xmm7, %xmm1
	movaps	%xmm9, -96(%rbp)
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm7, %xmm3
	pand	%xmm1, %xmm7
	por	%xmm2, %xmm7
	pand	%xmm1, %xmm0
	pshufd	$136, %xmm7, %xmm7
	por	%xmm3, %xmm0
	movdqa	%xmm7, %xmm3
	movdqa	-288(%rbp), %xmm7
	pshufd	$221, %xmm0, %xmm0
	punpckldq	%xmm0, %xmm3
	pshufd	$177, %xmm7, %xmm0
	movaps	%xmm3, -224(%rbp)
	movdqa	%xmm3, %xmm12
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm7, %xmm1
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm7, %xmm3
	pand	%xmm1, %xmm0
	pand	%xmm7, %xmm1
	por	%xmm3, %xmm0
	por	%xmm2, %xmm1
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm1, %xmm1
	punpckldq	%xmm0, %xmm1
	pshufd	$177, %xmm11, %xmm0
	movdqa	%xmm1, %xmm8
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm11, %xmm1
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm11, %xmm3
	pand	%xmm1, %xmm11
	pand	%xmm1, %xmm0
	por	%xmm2, %xmm11
	por	%xmm3, %xmm0
	pshufd	$136, %xmm11, %xmm11
	pshufd	$221, %xmm0, %xmm0
	movdqa	%xmm11, %xmm7
	punpckldq	%xmm0, %xmm7
	pshufd	$177, %xmm6, %xmm0
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm6, %xmm1
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm6, %xmm3
	pand	%xmm1, %xmm6
	por	%xmm2, %xmm6
	pand	%xmm1, %xmm0
	pshufd	$136, %xmm6, %xmm6
	por	%xmm3, %xmm0
	movdqa	%xmm6, %xmm10
	movdqa	-304(%rbp), %xmm6
	pshufd	$221, %xmm0, %xmm0
	punpckldq	%xmm0, %xmm10
	pshufd	$177, %xmm6, %xmm0
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm6, %xmm1
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm6, %xmm3
	pand	%xmm1, %xmm0
	pand	%xmm6, %xmm1
	por	%xmm3, %xmm0
	movdqa	-320(%rbp), %xmm6
	por	%xmm2, %xmm1
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm1, %xmm1
	punpckldq	%xmm0, %xmm1
	movdqa	%xmm1, %xmm5
	pshufd	$177, %xmm6, %xmm1
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm6, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm3
	pandn	%xmm1, %xmm2
	pandn	%xmm6, %xmm3
	pand	%xmm0, %xmm1
	pand	%xmm6, %xmm0
	por	%xmm3, %xmm1
	movdqa	-336(%rbp), %xmm6
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm13, %xmm1
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm13, %xmm2
	movdqa	%xmm2, %xmm3
	movdqa	%xmm2, %xmm4
	pandn	%xmm1, %xmm3
	pandn	%xmm13, %xmm4
	pand	%xmm2, %xmm1
	pand	%xmm2, %xmm13
	por	%xmm4, %xmm1
	pshufd	$177, %xmm6, %xmm2
	por	%xmm3, %xmm13
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm13, %xmm13
	punpckldq	%xmm1, %xmm13
	movdqa	%xmm2, %xmm1
	pcmpgtd	%xmm6, %xmm1
	movdqa	%xmm1, %xmm3
	movdqa	%xmm1, %xmm4
	pandn	%xmm2, %xmm3
	pandn	%xmm6, %xmm4
	pand	%xmm1, %xmm2
	pand	%xmm6, %xmm1
	por	%xmm4, %xmm2
	por	%xmm3, %xmm1
	pshufd	$221, %xmm2, %xmm2
	pshufd	$177, %xmm15, %xmm3
	pshufd	$136, %xmm1, %xmm1
	punpckldq	%xmm2, %xmm1
	movdqa	%xmm3, %xmm2
	pcmpgtd	%xmm15, %xmm2
	movdqa	%xmm2, %xmm4
	movdqa	%xmm2, %xmm6
	pandn	%xmm3, %xmm4
	pandn	%xmm15, %xmm6
	pand	%xmm2, %xmm3
	pand	%xmm2, %xmm15
	por	%xmm6, %xmm3
	por	%xmm4, %xmm15
	pshufd	$221, %xmm3, %xmm3
	pshufd	$136, %xmm15, %xmm2
	punpckldq	%xmm3, %xmm2
	jbe	.L113
	pshufd	$27, %xmm2, %xmm2
	pshufd	$27, %xmm13, %xmm3
	pshufd	$27, %xmm1, %xmm4
	movdqa	-352(%rbp), %xmm15
	movdqa	%xmm2, %xmm9
	pshufd	$27, %xmm10, %xmm6
	movdqa	%xmm4, %xmm10
	movaps	%xmm4, -144(%rbp)
	pcmpgtd	%xmm15, %xmm9
	movdqa	%xmm15, %xmm13
	movdqa	%xmm3, %xmm4
	movaps	%xmm3, -64(%rbp)
	pshufd	$27, %xmm0, %xmm0
	pshufd	$27, %xmm5, %xmm5
	pshufd	$27, %xmm7, %xmm7
	pshufd	$27, %xmm8, %xmm8
	movdqa	%xmm9, %xmm1
	pand	%xmm9, %xmm13
	pandn	%xmm2, %xmm1
	por	%xmm1, %xmm13
	movdqa	%xmm9, %xmm1
	pand	%xmm2, %xmm9
	pandn	%xmm15, %xmm1
	movdqa	-368(%rbp), %xmm15
	movaps	%xmm1, -240(%rbp)
	pcmpgtd	%xmm15, %xmm10
	movdqa	%xmm15, %xmm12
	movdqa	%xmm10, %xmm11
	movdqa	%xmm10, %xmm1
	pand	%xmm10, %xmm12
	pandn	%xmm15, %xmm11
	pandn	-144(%rbp), %xmm1
	movaps	%xmm11, -288(%rbp)
	movdqa	-384(%rbp), %xmm11
	por	%xmm1, %xmm12
	pcmpgtd	%xmm11, %xmm4
	movdqa	%xmm11, %xmm15
	movdqa	%xmm4, %xmm1
	pandn	-64(%rbp), %xmm1
	pand	%xmm4, %xmm15
	por	%xmm1, %xmm15
	movdqa	%xmm4, %xmm1
	pandn	%xmm11, %xmm1
	movaps	%xmm15, -80(%rbp)
	movaps	%xmm1, -304(%rbp)
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm14, %xmm1
	movdqa	%xmm1, %xmm15
	movdqa	%xmm1, %xmm11
	pandn	%xmm0, %xmm15
	pand	%xmm14, %xmm11
	por	%xmm15, %xmm11
	movdqa	%xmm1, %xmm15
	pand	%xmm0, %xmm1
	pandn	%xmm14, %xmm15
	movdqa	%xmm5, %xmm14
	movaps	%xmm11, -96(%rbp)
	movaps	%xmm15, -320(%rbp)
	por	-320(%rbp), %xmm1
	movdqa	-176(%rbp), %xmm15
	pcmpgtd	%xmm15, %xmm14
	pshufd	$27, %xmm1, %xmm1
	movdqa	-192(%rbp), %xmm15
	movdqa	%xmm14, %xmm3
	pandn	%xmm5, %xmm14
	movdqa	%xmm3, %xmm11
	pand	%xmm3, %xmm5
	pandn	-176(%rbp), %xmm11
	movaps	%xmm14, -336(%rbp)
	pand	-176(%rbp), %xmm3
	por	-336(%rbp), %xmm3
	por	%xmm11, %xmm5
	movaps	%xmm5, -112(%rbp)
	movdqa	%xmm6, %xmm5
	pcmpgtd	%xmm15, %xmm5
	movdqa	-224(%rbp), %xmm15
	movdqa	%xmm5, %xmm11
	pandn	%xmm6, %xmm11
	pand	%xmm5, %xmm6
	movaps	%xmm11, -352(%rbp)
	movdqa	%xmm5, %xmm11
	pandn	-192(%rbp), %xmm11
	pand	-192(%rbp), %xmm5
	por	-352(%rbp), %xmm5
	por	%xmm11, %xmm6
	movdqa	%xmm7, %xmm11
	movaps	%xmm6, -128(%rbp)
	movdqa	-208(%rbp), %xmm6
	pshufd	$27, %xmm5, %xmm5
	pcmpgtd	%xmm6, %xmm11
	movdqa	%xmm8, %xmm6
	pcmpgtd	%xmm15, %xmm6
	pshufd	$27, %xmm3, %xmm15
	movdqa	%xmm11, %xmm14
	pandn	%xmm7, %xmm14
	pand	%xmm11, %xmm7
	movaps	%xmm14, -368(%rbp)
	movdqa	%xmm11, %xmm14
	pandn	-208(%rbp), %xmm14
	por	%xmm14, %xmm7
	movdqa	%xmm6, %xmm14
	pandn	%xmm8, %xmm14
	movaps	%xmm7, -160(%rbp)
	pand	%xmm6, %xmm8
	movdqa	%xmm6, %xmm7
	pandn	-224(%rbp), %xmm7
	pand	-64(%rbp), %xmm4
	pand	-224(%rbp), %xmm6
	pand	-144(%rbp), %xmm10
	por	-288(%rbp), %xmm10
	por	%xmm7, %xmm8
	por	-240(%rbp), %xmm9
	por	%xmm14, %xmm6
	por	-304(%rbp), %xmm4
	pand	-208(%rbp), %xmm11
	pshufd	$27, %xmm6, %xmm6
	pshufd	$27, %xmm10, %xmm3
	pshufd	$27, %xmm9, %xmm2
	movdqa	-160(%rbp), %xmm14
	movdqa	%xmm6, %xmm10
	movdqa	%xmm13, %xmm9
	movaps	%xmm2, -208(%rbp)
	por	-368(%rbp), %xmm11
	pcmpgtd	%xmm13, %xmm10
	movaps	%xmm3, -64(%rbp)
	pshufd	$27, %xmm4, %xmm4
	pshufd	$27, %xmm11, %xmm11
	movdqa	%xmm10, %xmm7
	movdqa	%xmm10, %xmm0
	pand	%xmm10, %xmm9
	pandn	%xmm13, %xmm7
	movdqa	%xmm2, %xmm13
	pandn	%xmm6, %xmm0
	pcmpgtd	%xmm8, %xmm13
	por	%xmm0, %xmm9
	movdqa	%xmm8, %xmm2
	movaps	%xmm7, -288(%rbp)
	movdqa	%xmm14, %xmm7
	movaps	%xmm9, -224(%rbp)
	pand	%xmm6, %xmm10
	movdqa	%xmm13, %xmm0
	pandn	-208(%rbp), %xmm0
	pand	%xmm13, %xmm2
	por	%xmm0, %xmm2
	movdqa	%xmm13, %xmm0
	pandn	%xmm8, %xmm0
	movaps	%xmm2, -240(%rbp)
	movaps	%xmm0, -304(%rbp)
	movdqa	%xmm11, %xmm0
	pcmpgtd	%xmm12, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm8
	pandn	%xmm11, %xmm3
	pandn	%xmm12, %xmm8
	movdqa	%xmm3, %xmm2
	movdqa	%xmm12, %xmm3
	movdqa	-64(%rbp), %xmm12
	movaps	%xmm8, -320(%rbp)
	pand	%xmm0, %xmm3
	movdqa	-128(%rbp), %xmm8
	pand	%xmm11, %xmm0
	por	%xmm2, %xmm3
	movaps	%xmm3, -144(%rbp)
	movdqa	%xmm12, %xmm3
	pcmpgtd	%xmm14, %xmm3
	pand	%xmm3, %xmm7
	movdqa	%xmm3, %xmm2
	pandn	-64(%rbp), %xmm2
	movdqa	%xmm7, %xmm12
	movdqa	%xmm3, %xmm7
	pandn	%xmm14, %xmm7
	movdqa	-80(%rbp), %xmm14
	por	%xmm2, %xmm12
	movdqa	%xmm5, %xmm2
	movaps	%xmm12, -160(%rbp)
	pcmpgtd	%xmm14, %xmm2
	movaps	%xmm7, -336(%rbp)
	movdqa	%xmm2, %xmm12
	movdqa	%xmm2, %xmm7
	pandn	%xmm5, %xmm12
	pandn	%xmm14, %xmm7
	pand	%xmm2, %xmm5
	por	%xmm7, %xmm5
	movdqa	%xmm4, %xmm7
	pand	-80(%rbp), %xmm2
	pcmpgtd	%xmm8, %xmm7
	movaps	%xmm5, -176(%rbp)
	por	%xmm12, %xmm2
	pshufd	$27, %xmm2, %xmm2
	movdqa	%xmm7, %xmm5
	pandn	%xmm4, %xmm5
	pand	%xmm7, %xmm4
	movaps	%xmm5, -352(%rbp)
	movdqa	%xmm7, %xmm5
	movdqa	%xmm4, %xmm14
	movdqa	%xmm15, %xmm4
	pandn	%xmm8, %xmm5
	por	%xmm5, %xmm14
	movdqa	-96(%rbp), %xmm5
	movaps	%xmm14, -192(%rbp)
	pcmpgtd	%xmm5, %xmm4
	movdqa	%xmm4, %xmm8
	pandn	%xmm15, %xmm4
	movdqa	%xmm8, %xmm14
	pand	%xmm8, %xmm15
	pand	-96(%rbp), %xmm8
	pandn	%xmm5, %xmm14
	movdqa	%xmm1, %xmm5
	por	%xmm14, %xmm15
	movdqa	-112(%rbp), %xmm14
	por	%xmm4, %xmm8
	pshufd	$27, %xmm8, %xmm8
	pcmpgtd	%xmm14, %xmm5
	movdqa	%xmm5, %xmm9
	pandn	%xmm1, %xmm9
	pand	%xmm5, %xmm1
	movaps	%xmm9, -368(%rbp)
	por	-320(%rbp), %xmm0
	movdqa	%xmm5, %xmm9
	movdqa	-128(%rbp), %xmm4
	pand	-112(%rbp), %xmm5
	pandn	%xmm14, %xmm9
	pand	-64(%rbp), %xmm3
	pand	%xmm7, %xmm4
	por	-368(%rbp), %xmm5
	pand	-208(%rbp), %xmm13
	por	-304(%rbp), %xmm13
	por	%xmm9, %xmm1
	movdqa	-224(%rbp), %xmm6
	pshufd	$27, %xmm0, %xmm0
	pshufd	$27, %xmm5, %xmm7
	por	-336(%rbp), %xmm3
	por	-288(%rbp), %xmm10
	movdqa	%xmm7, %xmm14
	pshufd	$27, %xmm13, %xmm7
	movdqa	%xmm2, %xmm13
	movaps	%xmm7, -64(%rbp)
	movdqa	%xmm8, %xmm7
	pshufd	$27, %xmm10, %xmm10
	por	-352(%rbp), %xmm4
	pcmpgtd	%xmm6, %xmm7
	movdqa	%xmm10, %xmm11
	movdqa	%xmm6, %xmm10
	movaps	%xmm14, -224(%rbp)
	movaps	%xmm11, -208(%rbp)
	pshufd	$27, %xmm4, %xmm4
	pshufd	$27, %xmm3, %xmm3
	movdqa	%xmm7, %xmm9
	movdqa	%xmm7, %xmm5
	pand	%xmm7, %xmm10
	pandn	%xmm6, %xmm9
	pandn	%xmm8, %xmm5
	pand	%xmm8, %xmm7
	movdqa	-144(%rbp), %xmm6
	por	%xmm5, %xmm10
	movaps	%xmm9, -288(%rbp)
	movdqa	%xmm15, %xmm9
	pcmpgtd	%xmm6, %xmm13
	movdqa	%xmm13, %xmm5
	pandn	%xmm2, %xmm5
	pand	%xmm13, %xmm2
	movaps	%xmm5, -304(%rbp)
	movdqa	%xmm13, %xmm5
	pandn	%xmm6, %xmm5
	por	%xmm5, %xmm2
	movdqa	%xmm11, %xmm5
	movdqa	-176(%rbp), %xmm11
	pcmpgtd	%xmm15, %xmm5
	movdqa	%xmm5, %xmm6
	pandn	-208(%rbp), %xmm5
	movdqa	%xmm6, %xmm12
	pand	%xmm6, %xmm9
	pandn	%xmm15, %xmm12
	por	%xmm5, %xmm9
	movdqa	-240(%rbp), %xmm15
	movaps	%xmm12, -320(%rbp)
	movdqa	%xmm0, %xmm12
	pcmpgtd	%xmm11, %xmm12
	movdqa	%xmm12, %xmm5
	pandn	%xmm0, %xmm5
	pand	%xmm12, %xmm0
	movaps	%xmm5, -336(%rbp)
	movdqa	%xmm12, %xmm5
	pandn	%xmm11, %xmm5
	por	%xmm5, %xmm0
	movdqa	%xmm14, %xmm5
	movdqa	%xmm15, %xmm14
	pcmpgtd	%xmm15, %xmm5
	movaps	%xmm0, -80(%rbp)
	movdqa	%xmm5, %xmm11
	pand	%xmm5, %xmm14
	pandn	-224(%rbp), %xmm11
	por	%xmm11, %xmm14
	movdqa	%xmm5, %xmm11
	pandn	%xmm15, %xmm11
	movaps	%xmm14, -96(%rbp)
	movdqa	-160(%rbp), %xmm15
	movaps	%xmm11, -240(%rbp)
	movdqa	%xmm4, %xmm11
	pcmpgtd	%xmm15, %xmm11
	movdqa	%xmm11, %xmm14
	pandn	%xmm4, %xmm14
	pand	%xmm11, %xmm4
	movaps	%xmm14, -352(%rbp)
	movdqa	%xmm11, %xmm14
	pandn	%xmm15, %xmm14
	movdqa	-64(%rbp), %xmm15
	por	%xmm14, %xmm4
	movaps	%xmm4, -112(%rbp)
	movdqa	%xmm15, %xmm4
	movdqa	%xmm1, %xmm15
	pcmpgtd	%xmm1, %xmm4
	movdqa	%xmm4, %xmm14
	pandn	-64(%rbp), %xmm14
	pand	%xmm4, %xmm15
	por	%xmm14, %xmm15
	movdqa	%xmm4, %xmm14
	pandn	%xmm1, %xmm14
	movdqa	%xmm3, %xmm1
	movaps	%xmm14, -368(%rbp)
	movdqa	-192(%rbp), %xmm14
	por	-288(%rbp), %xmm7
	movdqa	-160(%rbp), %xmm8
	pand	-208(%rbp), %xmm6
	pcmpgtd	%xmm14, %xmm1
	pshufd	$27, %xmm7, %xmm7
	pand	-176(%rbp), %xmm12
	pand	%xmm11, %xmm8
	por	-320(%rbp), %xmm6
	movdqa	-192(%rbp), %xmm11
	por	-336(%rbp), %xmm12
	pand	-224(%rbp), %xmm5
	movdqa	%xmm1, %xmm0
	pand	%xmm1, %xmm11
	pshufd	$27, %xmm6, %xmm6
	pandn	%xmm3, %xmm0
	pand	%xmm1, %xmm3
	pshufd	$27, %xmm12, %xmm12
	movaps	%xmm0, -384(%rbp)
	movdqa	%xmm1, %xmm0
	pand	-64(%rbp), %xmm4
	por	-352(%rbp), %xmm8
	pandn	%xmm14, %xmm0
	por	-240(%rbp), %xmm5
	por	-384(%rbp), %xmm11
	por	%xmm0, %xmm3
	pshufd	$27, %xmm8, %xmm8
	por	-368(%rbp), %xmm4
	movaps	%xmm3, -128(%rbp)
	pshufd	$27, %xmm5, %xmm5
	pshufd	$27, %xmm11, %xmm11
	movdqa	-144(%rbp), %xmm3
	pshufd	$27, %xmm4, %xmm4
	pand	%xmm13, %xmm3
	por	-304(%rbp), %xmm3
	movdqa	%xmm10, %xmm13
	pshufd	$27, %xmm3, %xmm3
	movdqa	%xmm3, %xmm1
	pcmpgtd	%xmm10, %xmm1
	movdqa	%xmm1, %xmm0
	pand	%xmm1, %xmm13
	pandn	%xmm3, %xmm0
	pand	%xmm1, %xmm3
	por	%xmm0, %xmm13
	movdqa	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm2, %xmm1
	pandn	%xmm10, %xmm0
	movdqa	%xmm2, %xmm10
	por	%xmm0, %xmm3
	movdqa	%xmm1, %xmm0
	pand	%xmm1, %xmm10
	pandn	%xmm7, %xmm0
	por	%xmm0, %xmm10
	movdqa	%xmm1, %xmm0
	pandn	%xmm2, %xmm0
	movdqa	%xmm1, %xmm2
	movdqa	%xmm12, %xmm1
	pcmpgtd	%xmm9, %xmm1
	pand	%xmm7, %xmm2
	por	%xmm0, %xmm2
	movdqa	%xmm1, %xmm7
	movdqa	%xmm1, %xmm0
	pandn	%xmm12, %xmm7
	pandn	%xmm9, %xmm0
	movdqa	%xmm7, %xmm14
	movdqa	%xmm9, %xmm7
	movdqa	%xmm1, %xmm9
	pand	%xmm12, %xmm9
	movdqa	-80(%rbp), %xmm12
	pand	%xmm1, %xmm7
	movdqa	%xmm6, %xmm1
	por	%xmm0, %xmm9
	por	%xmm14, %xmm7
	pcmpgtd	%xmm12, %xmm1
	movdqa	%xmm1, %xmm0
	pandn	%xmm6, %xmm0
	movdqa	%xmm0, %xmm14
	movdqa	%xmm12, %xmm0
	pand	%xmm1, %xmm0
	movdqa	%xmm0, %xmm12
	movdqa	%xmm1, %xmm0
	pandn	-80(%rbp), %xmm0
	por	%xmm14, %xmm12
	movdqa	%xmm0, %xmm14
	movdqa	%xmm1, %xmm0
	movdqa	%xmm8, %xmm1
	pand	%xmm6, %xmm0
	por	%xmm14, %xmm0
	movdqa	-96(%rbp), %xmm14
	movaps	%xmm0, -64(%rbp)
	pcmpgtd	%xmm14, %xmm1
	movdqa	%xmm1, %xmm6
	pand	%xmm1, %xmm14
	movdqa	%xmm1, %xmm0
	pandn	%xmm8, %xmm6
	pand	%xmm8, %xmm1
	movdqa	-112(%rbp), %xmm8
	pandn	-96(%rbp), %xmm0
	por	%xmm6, %xmm14
	movdqa	%xmm5, %xmm6
	pcmpgtd	%xmm8, %xmm6
	por	%xmm0, %xmm1
	movaps	%xmm1, -80(%rbp)
	movdqa	%xmm6, %xmm1
	pandn	%xmm5, %xmm6
	pand	%xmm1, %xmm8
	movdqa	%xmm1, %xmm0
	pand	%xmm5, %xmm1
	por	%xmm6, %xmm8
	movdqa	%xmm11, %xmm6
	pandn	-112(%rbp), %xmm0
	pcmpgtd	%xmm15, %xmm6
	movdqa	%xmm1, %xmm5
	por	%xmm0, %xmm5
	movaps	%xmm5, -96(%rbp)
	movdqa	%xmm6, %xmm1
	movdqa	%xmm6, %xmm5
	movdqa	%xmm15, %xmm6
	pandn	%xmm11, %xmm5
	pand	%xmm1, %xmm6
	por	%xmm5, %xmm6
	movdqa	%xmm1, %xmm5
	pand	%xmm11, %xmm1
	pandn	%xmm15, %xmm5
	movdqa	-128(%rbp), %xmm15
	movdqa	%xmm1, %xmm11
	movdqa	%xmm4, %xmm1
	movaps	%xmm6, -112(%rbp)
	por	%xmm5, %xmm11
	pcmpgtd	%xmm15, %xmm1
	movdqa	%xmm15, %xmm6
	pand	%xmm1, %xmm6
	movdqa	%xmm1, %xmm5
	pandn	%xmm4, %xmm5
	movdqa	%xmm6, %xmm0
	por	%xmm5, %xmm0
	movdqa	%xmm1, %xmm5
	pand	%xmm4, %xmm1
	pshufd	$27, %xmm13, %xmm4
	movdqa	%xmm0, %xmm15
	pandn	-128(%rbp), %xmm5
	movdqa	%xmm4, %xmm0
	pcmpgtd	%xmm13, %xmm0
	por	%xmm5, %xmm1
	movaps	%xmm1, -128(%rbp)
	movdqa	%xmm0, %xmm6
	movdqa	%xmm0, %xmm1
	movdqa	%xmm0, %xmm5
	pandn	%xmm4, %xmm6
	pand	%xmm4, %xmm1
	pshufd	$27, %xmm3, %xmm4
	pandn	%xmm13, %xmm5
	pand	%xmm0, %xmm13
	movdqa	%xmm4, %xmm0
	pcmpgtd	%xmm3, %xmm0
	por	%xmm5, %xmm1
	por	%xmm6, %xmm13
	shufpd	$2, %xmm1, %xmm13
	movdqa	%xmm0, %xmm5
	movdqa	%xmm0, %xmm1
	movdqa	%xmm0, %xmm6
	pandn	%xmm3, %xmm5
	pand	%xmm4, %xmm1
	pandn	%xmm4, %xmm6
	por	%xmm5, %xmm1
	pand	%xmm0, %xmm3
	por	%xmm6, %xmm3
	movapd	%xmm1, %xmm6
	movsd	%xmm3, %xmm6
	pshufd	$27, %xmm10, %xmm3
	movdqa	%xmm3, %xmm0
	pcmpgtd	%xmm10, %xmm0
	movdqa	%xmm0, %xmm1
	movdqa	%xmm0, %xmm5
	pandn	%xmm10, %xmm0
	pandn	%xmm3, %xmm5
	pand	%xmm1, %xmm10
	pand	%xmm3, %xmm1
	pshufd	$27, %xmm2, %xmm3
	por	%xmm0, %xmm1
	por	%xmm5, %xmm10
	movdqa	%xmm3, %xmm0
	shufpd	$2, %xmm1, %xmm10
	pcmpgtd	%xmm2, %xmm0
	movdqa	%xmm0, %xmm1
	movdqa	%xmm0, %xmm5
	pandn	%xmm2, %xmm0
	pand	%xmm1, %xmm2
	pand	%xmm3, %xmm1
	pandn	%xmm3, %xmm5
	por	%xmm0, %xmm1
	por	%xmm5, %xmm2
	movdqa	-64(%rbp), %xmm0
	movapd	%xmm1, %xmm3
	movsd	%xmm2, %xmm3
	pshufd	$27, %xmm7, %xmm2
	movapd	%xmm3, %xmm5
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm7, %xmm3
	movdqa	%xmm3, %xmm1
	pandn	%xmm2, %xmm3
	movdqa	%xmm3, %xmm4
	movdqa	%xmm1, %xmm3
	pandn	%xmm7, %xmm3
	pand	%xmm1, %xmm7
	pand	%xmm2, %xmm1
	pshufd	$27, %xmm9, %xmm2
	por	%xmm3, %xmm1
	por	%xmm4, %xmm7
	movdqa	%xmm2, %xmm3
	shufpd	$2, %xmm1, %xmm7
	pcmpgtd	%xmm9, %xmm3
	movdqa	%xmm3, %xmm1
	pandn	%xmm2, %xmm3
	movdqa	%xmm3, %xmm4
	movdqa	%xmm1, %xmm3
	pandn	%xmm9, %xmm3
	pand	%xmm1, %xmm9
	pand	%xmm2, %xmm1
	pshufd	$27, %xmm12, %xmm2
	por	%xmm3, %xmm1
	por	%xmm4, %xmm9
	movdqa	%xmm2, %xmm3
	shufpd	$2, %xmm1, %xmm9
	pcmpgtd	%xmm12, %xmm3
	movdqa	%xmm3, %xmm1
	pandn	%xmm2, %xmm3
	movdqa	%xmm3, %xmm4
	movdqa	%xmm1, %xmm3
	pandn	%xmm12, %xmm3
	pand	%xmm1, %xmm12
	pand	%xmm2, %xmm1
	pshufd	$27, %xmm0, %xmm2
	por	%xmm3, %xmm1
	por	%xmm4, %xmm12
	movdqa	%xmm2, %xmm3
	shufpd	$2, %xmm1, %xmm12
	pcmpgtd	%xmm0, %xmm3
	movdqa	%xmm3, %xmm1
	pandn	%xmm2, %xmm3
	movdqa	%xmm3, %xmm4
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm3
	pand	%xmm1, %xmm0
	pand	%xmm2, %xmm1
	por	%xmm3, %xmm1
	por	%xmm4, %xmm0
	movapd	%xmm1, %xmm3
	pshufd	$27, %xmm14, %xmm1
	movsd	%xmm0, %xmm3
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm14, %xmm0
	movaps	%xmm3, -176(%rbp)
	movdqa	-80(%rbp), %xmm4
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm2
	pandn	%xmm1, %xmm3
	pandn	%xmm14, %xmm2
	pand	%xmm0, %xmm14
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm14
	pshufd	$27, %xmm4, %xmm1
	por	%xmm2, %xmm0
	shufpd	$2, %xmm0, %xmm14
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm4, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm3
	pandn	%xmm4, %xmm2
	pand	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	pandn	%xmm1, %xmm3
	por	%xmm2, %xmm0
	pshufd	$27, %xmm8, %xmm1
	movapd	%xmm0, %xmm2
	movdqa	%xmm1, %xmm0
	por	%xmm3, %xmm4
	pcmpgtd	%xmm8, %xmm0
	movsd	%xmm4, %xmm2
	movdqa	-96(%rbp), %xmm4
	movaps	%xmm2, -192(%rbp)
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm2
	pandn	%xmm1, %xmm3
	pandn	%xmm8, %xmm2
	pand	%xmm0, %xmm8
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm8
	pshufd	$27, %xmm4, %xmm1
	por	%xmm2, %xmm0
	shufpd	$2, %xmm0, %xmm8
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm4, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm2
	pandn	%xmm1, %xmm3
	pandn	%xmm4, %xmm2
	pand	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm4
	por	%xmm2, %xmm0
	movsd	%xmm4, %xmm0
	movdqa	-112(%rbp), %xmm4
	movaps	%xmm0, -208(%rbp)
	pshufd	$27, %xmm4, %xmm1
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm4, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm3
	pandn	%xmm4, %xmm2
	pand	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	pandn	%xmm1, %xmm3
	por	%xmm2, %xmm0
	pshufd	$27, %xmm11, %xmm1
	por	%xmm3, %xmm4
	movapd	%xmm0, %xmm3
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm11, %xmm0
	movsd	%xmm4, %xmm3
	movdqa	-128(%rbp), %xmm4
	movaps	%xmm3, -224(%rbp)
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm3
	pandn	%xmm1, %xmm3
	pandn	%xmm11, %xmm2
	pand	%xmm0, %xmm11
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm11
	pshufd	$27, %xmm15, %xmm1
	por	%xmm2, %xmm0
	shufpd	$2, %xmm0, %xmm11
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm15, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm2
	pandn	%xmm1, %xmm3
	pandn	%xmm15, %xmm2
	pand	%xmm0, %xmm15
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm15
	pshufd	$27, %xmm4, %xmm1
	por	%xmm2, %xmm0
	shufpd	$2, %xmm0, %xmm15
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm4, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm2
	pandn	%xmm1, %xmm3
	pandn	%xmm4, %xmm2
	pand	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm4
	pshufd	$177, %xmm13, %xmm1
	por	%xmm2, %xmm0
	shufpd	$2, %xmm0, %xmm4
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm13, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm3
	pandn	%xmm1, %xmm2
	pandn	%xmm13, %xmm3
	pand	%xmm0, %xmm1
	pand	%xmm13, %xmm0
	por	%xmm3, %xmm1
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm6, %xmm1
	movaps	%xmm0, -112(%rbp)
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm6, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm13
	pandn	%xmm1, %xmm2
	pandn	%xmm6, %xmm13
	pand	%xmm0, %xmm1
	pand	%xmm6, %xmm0
	por	%xmm13, %xmm1
	movapd	-224(%rbp), %xmm6
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm10, %xmm1
	movaps	%xmm0, -128(%rbp)
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm10, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm13
	pandn	%xmm1, %xmm2
	pandn	%xmm10, %xmm13
	pand	%xmm0, %xmm1
	pand	%xmm10, %xmm0
	por	%xmm13, %xmm1
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm5, %xmm1
	movaps	%xmm0, -144(%rbp)
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm5, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm10
	pandn	%xmm1, %xmm2
	pandn	%xmm5, %xmm10
	pand	%xmm0, %xmm1
	pand	%xmm5, %xmm0
	por	%xmm10, %xmm1
	movapd	-208(%rbp), %xmm5
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm7, %xmm1
	movaps	%xmm0, -160(%rbp)
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm7, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm10
	pandn	%xmm1, %xmm2
	pandn	%xmm7, %xmm10
	pand	%xmm0, %xmm1
	pand	%xmm7, %xmm0
	por	%xmm10, %xmm1
	movapd	-176(%rbp), %xmm7
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm9, %xmm1
	movaps	%xmm0, -64(%rbp)
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm9, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm10
	pandn	%xmm1, %xmm2
	pandn	%xmm9, %xmm10
	pand	%xmm0, %xmm1
	pand	%xmm9, %xmm0
	por	%xmm10, %xmm1
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm12, %xmm1
	movaps	%xmm0, -80(%rbp)
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm12, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm10
	pandn	%xmm1, %xmm2
	pandn	%xmm12, %xmm10
	pand	%xmm0, %xmm1
	pand	%xmm12, %xmm0
	por	%xmm10, %xmm1
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm7, %xmm1
	movaps	%xmm0, -96(%rbp)
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm7, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm3
	pandn	%xmm1, %xmm2
	pandn	%xmm7, %xmm3
	pand	%xmm0, %xmm1
	pand	%xmm7, %xmm0
	por	%xmm3, %xmm1
	movapd	-192(%rbp), %xmm7
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	movdqa	%xmm0, %xmm12
	pshufd	$177, %xmm14, %xmm0
	movdqa	%xmm0, %xmm3
	pcmpgtd	%xmm14, %xmm3
	movdqa	%xmm3, %xmm1
	movdqa	%xmm3, %xmm2
	pandn	%xmm0, %xmm1
	pandn	%xmm14, %xmm2
	pand	%xmm3, %xmm0
	pand	%xmm14, %xmm3
	por	%xmm2, %xmm0
	por	%xmm1, %xmm3
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm3, %xmm3
	punpckldq	%xmm0, %xmm3
	pshufd	$177, %xmm7, %xmm0
	movdqa	%xmm0, %xmm9
	pcmpgtd	%xmm7, %xmm9
	movdqa	%xmm9, %xmm1
	movdqa	%xmm9, %xmm2
	pandn	%xmm0, %xmm1
	pandn	%xmm7, %xmm2
	pand	%xmm9, %xmm0
	pand	%xmm7, %xmm9
	por	%xmm2, %xmm0
	por	%xmm1, %xmm9
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm9, %xmm9
	punpckldq	%xmm0, %xmm9
	pshufd	$177, %xmm8, %xmm0
	movdqa	%xmm0, %xmm7
	pcmpgtd	%xmm8, %xmm7
	movdqa	%xmm7, %xmm1
	movdqa	%xmm7, %xmm2
	pandn	%xmm0, %xmm1
	pandn	%xmm8, %xmm2
	pand	%xmm7, %xmm0
	pand	%xmm8, %xmm7
	por	%xmm2, %xmm0
	por	%xmm1, %xmm7
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm7, %xmm7
	punpckldq	%xmm0, %xmm7
	pshufd	$177, %xmm5, %xmm0
	movdqa	%xmm0, %xmm10
	pcmpgtd	%xmm5, %xmm10
	movdqa	%xmm10, %xmm1
	movdqa	%xmm10, %xmm2
	pandn	%xmm0, %xmm1
	pandn	%xmm5, %xmm2
	pand	%xmm10, %xmm0
	pand	%xmm5, %xmm10
	por	%xmm2, %xmm0
	por	%xmm1, %xmm10
	pshufd	$221, %xmm0, %xmm0
	pshufd	$177, %xmm6, %xmm1
	pshufd	$136, %xmm10, %xmm10
	punpckldq	%xmm0, %xmm10
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm6, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm5
	pandn	%xmm1, %xmm2
	pandn	%xmm6, %xmm5
	pand	%xmm0, %xmm1
	pand	%xmm6, %xmm0
	por	%xmm5, %xmm1
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm11, %xmm1
	movdqa	%xmm1, %xmm13
	pcmpgtd	%xmm11, %xmm13
	movdqa	%xmm13, %xmm2
	movdqa	%xmm13, %xmm5
	pandn	%xmm1, %xmm2
	pandn	%xmm11, %xmm5
	pand	%xmm13, %xmm1
	pand	%xmm11, %xmm13
	por	%xmm5, %xmm1
	por	%xmm2, %xmm13
	pshufd	$221, %xmm1, %xmm1
	pshufd	$177, %xmm15, %xmm2
	pshufd	$136, %xmm13, %xmm13
	punpckldq	%xmm1, %xmm13
	movdqa	%xmm2, %xmm1
	pcmpgtd	%xmm15, %xmm1
	movdqa	%xmm1, %xmm5
	movdqa	%xmm1, %xmm6
	pandn	%xmm2, %xmm5
	pandn	%xmm15, %xmm6
	pand	%xmm1, %xmm2
	pand	%xmm15, %xmm1
	por	%xmm6, %xmm2
	por	%xmm5, %xmm1
	pshufd	$221, %xmm2, %xmm2
	pshufd	$177, %xmm4, %xmm5
	pshufd	$136, %xmm1, %xmm1
	punpckldq	%xmm2, %xmm1
	movdqa	%xmm5, %xmm2
	pcmpgtd	%xmm4, %xmm2
	movdqa	%xmm2, %xmm6
	movdqa	%xmm2, %xmm8
	pandn	%xmm5, %xmm6
	pandn	%xmm4, %xmm8
	pand	%xmm2, %xmm5
	pand	%xmm4, %xmm2
	por	%xmm8, %xmm5
	por	%xmm6, %xmm2
	pshufd	$221, %xmm5, %xmm5
	pshufd	$136, %xmm2, %xmm2
	punpckldq	%xmm5, %xmm2
	movdqa	%xmm2, %xmm15
.L109:
	movdqa	-112(%rbp), %xmm4
	movq	-264(%rbp), %rdx
	movups	%xmm4, (%rdx)
	movdqa	-128(%rbp), %xmm4
	movups	%xmm4, (%r15)
	movdqa	-144(%rbp), %xmm4
	movups	%xmm4, (%r14)
	movdqa	-160(%rbp), %xmm4
	movups	%xmm4, 0(%r13)
	movdqa	-64(%rbp), %xmm4
	movups	%xmm4, (%r12)
	movdqa	-80(%rbp), %xmm4
	movups	%xmm4, (%rbx)
	movdqa	-96(%rbp), %xmm4
	movq	-248(%rbp), %rbx
	movups	%xmm4, (%r11)
	movups	%xmm12, (%r10)
	movups	%xmm3, (%r9)
	movups	%xmm9, (%r8)
	movups	%xmm7, (%rdi)
	movups	%xmm10, (%rsi)
	movups	%xmm0, (%rcx)
	movq	-256(%rbp), %rcx
	movups	%xmm13, (%rbx)
	movups	%xmm1, (%rcx)
	movups	%xmm15, (%rax)
	addq	$240, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L112:
	.cfi_restore_state
	movdqa	-192(%rbp), %xmm15
	movdqa	-208(%rbp), %xmm1
	movdqa	%xmm2, %xmm9
	jmp	.L109
	.p2align 4,,10
	.p2align 3
.L113:
	movdqa	%xmm7, %xmm9
	movdqa	%xmm8, %xmm3
	movdqa	%xmm10, %xmm7
	movdqa	%xmm2, %xmm15
	movdqa	%xmm5, %xmm10
	jmp	.L109
	.cfi_endproc
.LFE18783:
	.size	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0, .-_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	.section	.text._ZN3hwy11N_AVX3_ZEN46detail22MaybePartitionTwoValueINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy11N_AVX3_ZEN46detail22MaybePartitionTwoValueINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, @function
_ZN3hwy11N_AVX3_ZEN46detail22MaybePartitionTwoValueINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0:
.LFB18784:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %r10
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	.cfi_offset 3, -24
	cmpq	$15, %rsi
	jbe	.L130
	vmovdqa32	%zmm0, %zmm2
	vmovdqa32	%zmm1, %zmm3
	movl	$16, %r8d
	xorl	%esi, %esi
	jmp	.L121
	.p2align 4,,10
	.p2align 3
.L116:
	vmovdqu64	%zmm0, (%rax)
	kmovw	%k0, %eax
	popcntq	%rax, %rax
	addq	%rax, %rsi
	leaq	16(%r8), %rax
	cmpq	%r10, %rax
	ja	.L141
	movq	%rax, %r8
.L121:
	vmovdqu32	-64(%rdi,%r8,4), %zmm4
	leaq	-16(%r8), %r9
	leaq	(%rdi,%rsi,4), %rax
	vpcmpd	$0, %zmm2, %zmm4, %k0
	vpcmpd	$0, %zmm3, %zmm4, %k1
	kmovw	%k0, %r11d
	kmovw	%k1, %ebx
	korw	%k1, %k0, %k1
	kortestw	%k1, %k1
	jc	.L116
	kmovw	%r11d, %k6
	kmovw	%ebx, %k5
	kxnorw	%k5, %k6, %k7
	kmovw	%k7, %eax
	tzcntl	%eax, %eax
	addq	%r9, %rax
	vpbroadcastd	(%rdi,%rax,4), %zmm0
	leaq	16(%rsi), %rax
	vmovdqa32	%zmm0, (%rdx)
	cmpq	%r9, %rax
	ja	.L117
	.p2align 4,,10
	.p2align 3
.L118:
	vmovdqu64	%zmm1, -64(%rdi,%rax,4)
	movq	%rax, %rsi
	addq	$16, %rax
	cmpq	%rax, %r9
	jnb	.L118
.L117:
	subq	%rsi, %r9
	leaq	(%rdi,%rsi,4), %rdx
	movl	$65535, %eax
	cmpq	$255, %r9
	jbe	.L142
.L119:
	kmovw	%eax, %k4
	xorl	%eax, %eax
	vmovdqu32	%zmm1, (%rdx){%k4}
.L114:
	movq	-8(%rbp), %rbx
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L142:
	.cfi_restore_state
	movq	$-1, %rax
	bzhi	%r9, %rax, %rax
	movzwl	%ax, %eax
	jmp	.L119
	.p2align 4,,10
	.p2align 3
.L141:
	movq	%r10, %r11
	leaq	(%rdi,%r8,4), %rbx
	leaq	(%rdi,%rsi,4), %r9
	movl	$65535, %eax
	subq	%r8, %r11
	kmovd	%eax, %k1
	cmpq	$255, %r11
	jbe	.L115
.L122:
	vmovdqu32	(%rbx), %zmm2{%k1}{z}
	knotw	%k1, %k3
	vmovdqu32	%zmm2, (%rcx){%k1}
	vmovdqa32	(%rcx), %zmm2
	vpcmpd	$0, %zmm0, %zmm2, %k0
	vpcmpd	$0, %zmm1, %zmm2, %k2
	kandw	%k1, %k0, %k0
	korw	%k2, %k0, %k2
	korw	%k3, %k2, %k2
	kortestw	%k2, %k2
	jnc	.L143
	kmovw	%k0, %edx
	popcntq	%rdx, %rdx
	addq	%rsi, %rdx
	vmovdqu32	%zmm0, (%r9){%k1}
	leaq	16(%rdx), %rax
	cmpq	%r10, %rax
	ja	.L127
	.p2align 4,,10
	.p2align 3
.L128:
	vmovdqu64	%zmm1, -64(%rdi,%rax,4)
	movq	%rax, %rdx
	addq	$16, %rax
	cmpq	%rax, %r10
	jnb	.L128
.L127:
	subq	%rdx, %r10
	leaq	(%rdi,%rdx,4), %rcx
	movl	$65535, %eax
	cmpq	$255, %r10
	ja	.L129
	movq	$-1, %rax
	bzhi	%r10, %rax, %rax
	movzwl	%ax, %eax
.L129:
	kmovw	%eax, %k5
	movl	$1, %eax
	vmovdqu32	%zmm1, (%rcx){%k5}
	movq	-8(%rbp), %rbx
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
.L130:
	.cfi_restore_state
	movq	%rsi, %r11
	movq	%rdi, %r9
	movq	%rdi, %rbx
	xorl	%esi, %esi
	xorl	%r8d, %r8d
.L115:
	movq	$-1, %rax
	bzhi	%r11, %rax, %rax
	movzwl	%ax, %eax
	kmovd	%eax, %k1
	jmp	.L122
.L143:
	knotw	%k2, %k3
	kmovw	%k3, %eax
	tzcntl	%eax, %eax
	addq	%r8, %rax
	vpbroadcastd	(%rdi,%rax,4), %zmm0
	leaq	16(%rsi), %rax
	vmovdqa32	%zmm0, (%rdx)
	cmpq	%r8, %rax
	ja	.L124
	.p2align 4,,10
	.p2align 3
.L125:
	vmovdqu64	%zmm1, -64(%rdi,%rax,4)
	movq	%rax, %rsi
	leaq	16(%rax), %rax
	cmpq	%rax, %r8
	jnb	.L125
	leaq	(%rdi,%rsi,4), %r9
.L124:
	subq	%rsi, %r8
	movl	$65535, %eax
	cmpq	$255, %r8
	ja	.L126
	movq	$-1, %rax
	bzhi	%r8, %rax, %rax
	movzwl	%ax, %eax
.L126:
	kmovw	%eax, %k6
	xorl	%eax, %eax
	vmovdqu32	%zmm1, (%r9){%k6}
	jmp	.L114
	.cfi_endproc
.LFE18784:
	.size	_ZN3hwy11N_AVX3_ZEN46detail22MaybePartitionTwoValueINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, .-_ZN3hwy11N_AVX3_ZEN46detail22MaybePartitionTwoValueINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	.section	.text._ZN3hwy11N_AVX3_ZEN46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy11N_AVX3_ZEN46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0, @function
_ZN3hwy11N_AVX3_ZEN46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0:
.LFB18785:
	.cfi_startproc
	movq	%rsi, %r8
	movq	%rdx, %rcx
	cmpq	%rdx, %rsi
	jbe	.L154
	leaq	(%rdx,%rdx), %rdx
	leaq	1(%rcx), %r10
	leaq	1(%rdx), %rsi
	addq	$2, %rdx
	cmpq	%rsi, %r8
	jbe	.L154
	movl	(%rdi,%rcx,4), %r11d
	vpbroadcastd	%r11d, %xmm1
	jmp	.L147
	.p2align 4,,10
	.p2align 3
.L157:
	movq	%rsi, %rax
	cmpq	%rdx, %r8
	ja	.L155
.L149:
	cmpq	%rcx, %rax
	je	.L154
	leaq	(%rdi,%rax,4), %rdx
	movl	(%rdx), %ecx
	movl	%ecx, (%r9)
	movl	%r11d, (%rdx)
	cmpq	%rax, %r8
	jbe	.L156
	leaq	(%rax,%rax), %rdx
	leaq	1(%rax), %r10
	leaq	1(%rdx), %rsi
	addq	$2, %rdx
	cmpq	%r8, %rsi
	jnb	.L154
	movq	%rax, %rcx
.L147:
	vpbroadcastd	(%rdi,%rsi,4), %xmm0
	leaq	(%rdi,%rcx,4), %r9
	vpcmpd	$6, %xmm1, %xmm0, %k0
	kmovb	%k0, %eax
	testb	$1, %al
	jne	.L157
	cmpq	%rdx, %r8
	jbe	.L154
	vpbroadcastd	(%rdi,%r10,8), %xmm0
	vpcmpd	$6, %xmm1, %xmm0, %k1
	kmovb	%k1, %eax
	testb	$1, %al
	je	.L154
	movq	%rdx, %rax
	jmp	.L149
	.p2align 4,,10
	.p2align 3
.L154:
	ret
	.p2align 4,,10
	.p2align 3
.L155:
	vpbroadcastd	(%rdi,%r10,8), %xmm2
	vpcmpd	$6, %xmm0, %xmm2, %k2
	kmovb	%k2, %esi
	andl	$1, %esi
	cmovne	%rdx, %rax
	jmp	.L149
	.p2align 4,,10
	.p2align 3
.L156:
	ret
	.cfi_endproc
.LFE18785:
	.size	_ZN3hwy11N_AVX3_ZEN46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0, .-_ZN3hwy11N_AVX3_ZEN46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	.section	.text._ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0, @function
_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0:
.LFB18786:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	leaq	0(,%rsi,4), %rax
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	.cfi_offset 15, -24
	leaq	(%rdi,%rax), %r15
	pushq	%r14
	.cfi_offset 14, -32
	leaq	(%r15,%rax), %r14
	pushq	%r13
	.cfi_offset 13, -40
	leaq	(%r14,%rax), %r13
	pushq	%r12
	.cfi_offset 12, -48
	leaq	0(%r13,%rax), %r12
	pushq	%rbx
	.cfi_offset 3, -56
	leaq	(%r12,%rax), %rbx
	leaq	(%rbx,%rax), %r11
	leaq	(%r11,%rax), %r10
	leaq	(%r10,%rax), %r9
	andq	$-64, %rsp
	leaq	(%r9,%rax), %r8
	subq	$8, %rsp
	leaq	(%r8,%rax), %rdx
	leaq	(%rdx,%rax), %rcx
	vmovq	%rdx, %xmm26
	leaq	(%rcx,%rax), %rdx
	vmovq	%rcx, %xmm25
	vmovdqu32	(%rdi), %zmm7
	vpminsd	(%r15), %zmm7, %zmm15
	vmovq	%rdx, %xmm27
	addq	%rax, %rdx
	vpmaxsd	(%r15), %zmm7, %zmm0
	vmovdqu32	(%r14), %zmm7
	leaq	(%rdx,%rax), %rcx
	vpminsd	0(%r13), %zmm7, %zmm16
	addq	%rcx, %rax
	vmovq	%rcx, %xmm24
	vmovq	%xmm26, %rcx
	movq	%rax, (%rsp)
	vmovq	%xmm25, %rax
	vpmaxsd	0(%r13), %zmm7, %zmm10
	vpminsd	%zmm16, %zmm15, %zmm12
	vmovdqu32	(%r12), %zmm7
	vpminsd	(%rbx), %zmm7, %zmm11
	vpmaxsd	%zmm16, %zmm15, %zmm15
	vpmaxsd	(%rbx), %zmm7, %zmm2
	vmovdqu32	(%r11), %zmm7
	vpminsd	%zmm10, %zmm0, %zmm16
	vpminsd	(%r10), %zmm7, %zmm8
	vpmaxsd	(%r10), %zmm7, %zmm6
	vmovdqu32	(%r9), %zmm7
	vpminsd	(%r8), %zmm7, %zmm1
	vpmaxsd	%zmm10, %zmm0, %zmm0
	vpmaxsd	(%r8), %zmm7, %zmm4
	vmovdqu32	(%rcx), %zmm7
	vpminsd	%zmm8, %zmm11, %zmm10
	vpminsd	(%rax), %zmm7, %zmm9
	vpmaxsd	(%rax), %zmm7, %zmm13
	vmovq	%xmm27, %rax
	vmovdqu32	(%rax), %zmm7
	movq	(%rsp), %rax
	vpmaxsd	%zmm8, %zmm11, %zmm11
	vpminsd	%zmm6, %zmm2, %zmm8
	vpminsd	(%rdx), %zmm7, %zmm3
	vpmaxsd	(%rdx), %zmm7, %zmm5
	vmovdqu64	(%rax), %zmm7
	vmovq	%xmm24, %rax
	vpmaxsd	%zmm6, %zmm2, %zmm2
	vpminsd	%zmm9, %zmm1, %zmm6
	vpmaxsd	%zmm9, %zmm1, %zmm1
	vpminsd	%zmm13, %zmm4, %zmm9
	vmovdqa64	%zmm7, -120(%rsp)
	vmovdqa32	-120(%rsp), %zmm7
	vpminsd	(%rax), %zmm7, %zmm14
	vpmaxsd	%zmm13, %zmm4, %zmm4
	vpmaxsd	(%rax), %zmm7, %zmm7
	vpminsd	%zmm14, %zmm3, %zmm13
	vpmaxsd	%zmm14, %zmm3, %zmm3
	vpminsd	%zmm7, %zmm5, %zmm14
	vpmaxsd	%zmm7, %zmm5, %zmm5
	vpminsd	%zmm10, %zmm12, %zmm7
	vpmaxsd	%zmm10, %zmm12, %zmm12
	vpminsd	%zmm8, %zmm16, %zmm10
	vpmaxsd	%zmm8, %zmm16, %zmm16
	vpminsd	%zmm11, %zmm15, %zmm8
	vpmaxsd	%zmm11, %zmm15, %zmm15
	vpminsd	%zmm2, %zmm0, %zmm11
	vpmaxsd	%zmm2, %zmm0, %zmm0
	vpminsd	%zmm13, %zmm6, %zmm2
	vpmaxsd	%zmm13, %zmm6, %zmm6
	vpminsd	%zmm14, %zmm9, %zmm13
	vpmaxsd	%zmm14, %zmm9, %zmm9
	vpminsd	%zmm3, %zmm1, %zmm14
	vpmaxsd	%zmm3, %zmm1, %zmm1
	vpminsd	%zmm5, %zmm4, %zmm3
	vpmaxsd	%zmm5, %zmm4, %zmm4
	vpminsd	%zmm2, %zmm7, %zmm5
	vpmaxsd	%zmm2, %zmm7, %zmm7
	vpminsd	%zmm13, %zmm10, %zmm2
	vpmaxsd	%zmm13, %zmm10, %zmm10
	vpminsd	%zmm14, %zmm8, %zmm13
	vpmaxsd	%zmm14, %zmm8, %zmm8
	vpminsd	%zmm3, %zmm11, %zmm14
	vpmaxsd	%zmm3, %zmm11, %zmm11
	vpminsd	%zmm6, %zmm12, %zmm3
	vpmaxsd	%zmm6, %zmm12, %zmm12
	vpminsd	%zmm9, %zmm16, %zmm6
	vpmaxsd	%zmm9, %zmm16, %zmm16
	vpminsd	%zmm1, %zmm15, %zmm9
	vpmaxsd	%zmm1, %zmm15, %zmm15
	vpminsd	%zmm4, %zmm0, %zmm1
	vpmaxsd	%zmm4, %zmm0, %zmm0
	vpminsd	%zmm8, %zmm6, %zmm4
	vpmaxsd	%zmm8, %zmm6, %zmm6
	vpminsd	%zmm10, %zmm9, %zmm8
	vpmaxsd	%zmm10, %zmm9, %zmm9
	vpminsd	%zmm12, %zmm14, %zmm10
	vpmaxsd	%zmm12, %zmm14, %zmm14
	vpminsd	%zmm11, %zmm1, %zmm12
	vpmaxsd	%zmm11, %zmm1, %zmm1
	vpminsd	%zmm15, %zmm16, %zmm11
	vpmaxsd	%zmm15, %zmm16, %zmm16
	vpminsd	%zmm7, %zmm3, %zmm15
	vpmaxsd	%zmm7, %zmm3, %zmm3
	vpminsd	%zmm13, %zmm2, %zmm7
	vpmaxsd	%zmm13, %zmm2, %zmm2
	vpminsd	%zmm15, %zmm7, %zmm13
	vpmaxsd	%zmm15, %zmm7, %zmm7
	vpminsd	%zmm11, %zmm12, %zmm15
	vpmaxsd	%zmm11, %zmm12, %zmm12
	vpminsd	%zmm3, %zmm2, %zmm11
	vpmaxsd	%zmm3, %zmm2, %zmm2
	vpminsd	%zmm16, %zmm1, %zmm3
	vpminsd	%zmm7, %zmm11, %zmm18
	vpmaxsd	%zmm7, %zmm11, %zmm11
	vpminsd	%zmm8, %zmm4, %zmm7
	vpmaxsd	%zmm8, %zmm4, %zmm4
	vpminsd	%zmm6, %zmm9, %zmm8
	vpmaxsd	%zmm6, %zmm9, %zmm9
	vpminsd	%zmm12, %zmm3, %zmm6
	vpmaxsd	%zmm12, %zmm3, %zmm3
	vpminsd	%zmm2, %zmm10, %zmm12
	vpmaxsd	%zmm2, %zmm10, %zmm10
	vpminsd	%zmm14, %zmm15, %zmm2
	vpmaxsd	%zmm14, %zmm15, %zmm15
	vpminsd	%zmm7, %zmm12, %zmm14
	vpmaxsd	%zmm7, %zmm12, %zmm12
	vpminsd	%zmm10, %zmm4, %zmm7
	vpmaxsd	%zmm10, %zmm4, %zmm4
	vpminsd	%zmm8, %zmm2, %zmm10
	vpmaxsd	%zmm8, %zmm2, %zmm2
	vpminsd	%zmm15, %zmm9, %zmm8
	vpmaxsd	%zmm16, %zmm1, %zmm1
	vpmaxsd	%zmm15, %zmm9, %zmm9
	vpminsd	%zmm7, %zmm12, %zmm16
	vpmaxsd	%zmm7, %zmm12, %zmm12
	vpminsd	%zmm4, %zmm10, %zmm7
	vpmaxsd	%zmm4, %zmm10, %zmm10
	vpminsd	%zmm8, %zmm2, %zmm4
	vpminsd	%zmm7, %zmm12, %zmm15
	vpminsd	%zmm11, %zmm14, %zmm17
	vpmaxsd	%zmm8, %zmm2, %zmm2
	vpmaxsd	%zmm7, %zmm12, %zmm12
	vpminsd	%zmm9, %zmm6, %zmm8
	vpminsd	%zmm4, %zmm10, %zmm7
	vpmaxsd	%zmm11, %zmm14, %zmm14
	vpmaxsd	%zmm9, %zmm6, %zmm6
	vpmaxsd	%zmm4, %zmm10, %zmm10
	cmpq	$1, %rsi
	jbe	.L160
	vpshufd	$177, %zmm2, %zmm2
	vpshufd	$177, %zmm0, %zmm0
	vpshufd	$177, %zmm8, %zmm8
	movl	$21845, %eax
	vpminsd	%zmm0, %zmm5, %zmm22
	vpshufd	$177, %zmm3, %zmm3
	vpmaxsd	%zmm0, %zmm5, %zmm9
	kmovw	%eax, %k1
	vpminsd	%zmm2, %zmm16, %zmm5
	vpshufd	$177, %zmm10, %zmm10
	vpminsd	%zmm3, %zmm18, %zmm20
	vpshufd	$177, %zmm7, %zmm7
	vpshufd	$177, %zmm1, %zmm1
	vpmaxsd	%zmm3, %zmm18, %zmm0
	vpminsd	%zmm8, %zmm14, %zmm11
	vpminsd	%zmm10, %zmm15, %zmm4
	vpshufd	$177, %zmm6, %zmm6
	vpshufd	$177, %zmm5, %zmm5
	vpminsd	%zmm1, %zmm13, %zmm21
	vpminsd	%zmm6, %zmm17, %zmm19
	vpmaxsd	%zmm2, %zmm16, %zmm16
	vpminsd	%zmm7, %zmm12, %zmm3
	vpshufd	$177, %zmm9, %zmm9
	vpminsd	%zmm5, %zmm20, %zmm2
	vpmaxsd	%zmm1, %zmm13, %zmm13
	vpshufd	$177, %zmm11, %zmm11
	vpmaxsd	%zmm7, %zmm12, %zmm1
	vpmaxsd	%zmm6, %zmm17, %zmm6
	vpshufd	$177, %zmm0, %zmm0
	vpshufd	$177, %zmm4, %zmm4
	vpminsd	%zmm9, %zmm1, %zmm18
	vpmaxsd	%zmm8, %zmm14, %zmm14
	vpminsd	%zmm4, %zmm21, %zmm12
	vpmaxsd	%zmm10, %zmm15, %zmm10
	vpshufd	$177, %zmm3, %zmm7
	vpshufd	$177, %zmm2, %zmm2
	vpminsd	%zmm11, %zmm19, %zmm3
	vpshufd	$177, %zmm6, %zmm6
	vpshufd	$177, %zmm13, %zmm13
	vpmaxsd	%zmm9, %zmm1, %zmm1
	vpmaxsd	%zmm4, %zmm21, %zmm4
	vpminsd	%zmm0, %zmm16, %zmm9
	vpminsd	%zmm7, %zmm22, %zmm17
	vpshufd	$177, %zmm4, %zmm4
	vpminsd	%zmm13, %zmm10, %zmm15
	vpmaxsd	%zmm5, %zmm20, %zmm5
	vpshufd	$177, %zmm3, %zmm3
	vpmaxsd	%zmm0, %zmm16, %zmm0
	vpminsd	%zmm6, %zmm14, %zmm8
	vpshufd	$177, %zmm9, %zmm16
	vpmaxsd	%zmm6, %zmm14, %zmm6
	vpminsd	%zmm2, %zmm12, %zmm20
	vpshufd	$177, %zmm1, %zmm1
	vpmaxsd	%zmm7, %zmm22, %zmm7
	vpmaxsd	%zmm13, %zmm10, %zmm13
	vpshufd	$177, %zmm8, %zmm8
	vpminsd	%zmm3, %zmm17, %zmm14
	vpmaxsd	%zmm11, %zmm19, %zmm11
	vpshufd	$177, %zmm13, %zmm13
	vpmaxsd	%zmm2, %zmm12, %zmm12
	vpminsd	%zmm1, %zmm6, %zmm19
	vpshufd	$177, %zmm20, %zmm2
	vpminsd	%zmm4, %zmm5, %zmm9
	vpshufd	$177, %zmm7, %zmm7
	vpmaxsd	%zmm4, %zmm5, %zmm5
	vpmaxsd	%zmm1, %zmm6, %zmm1
	vpminsd	%zmm16, %zmm15, %zmm4
	vpshufd	$177, %zmm9, %zmm9
	vpminsd	%zmm7, %zmm11, %zmm10
	vpshufd	$177, %zmm4, %zmm4
	vpshufd	$177, %zmm1, %zmm1
	vpmaxsd	%zmm3, %zmm17, %zmm3
	vpmaxsd	%zmm7, %zmm11, %zmm7
	vpminsd	%zmm8, %zmm18, %zmm17
	vpmaxsd	%zmm8, %zmm18, %zmm8
	vpshufd	$177, %zmm7, %zmm7
	vpminsd	%zmm13, %zmm0, %zmm18
	vpmaxsd	%zmm13, %zmm0, %zmm0
	vpshufd	$177, %zmm3, %zmm3
	vpminsd	%zmm2, %zmm14, %zmm13
	vpminsd	%zmm4, %zmm17, %zmm23
	vpshufd	$177, %zmm18, %zmm18
	vpminsd	%zmm1, %zmm0, %zmm20
	vpmaxsd	%zmm16, %zmm15, %zmm16
	vpshufd	$177, %zmm8, %zmm8
	vpminsd	%zmm9, %zmm10, %zmm15
	vpmaxsd	%zmm9, %zmm10, %zmm10
	vpmaxsd	%zmm4, %zmm17, %zmm9
	vpmaxsd	%zmm1, %zmm0, %zmm4
	vpshufd	$177, %zmm13, %zmm0
	vpmaxsd	%zmm2, %zmm14, %zmm11
	vpminsd	%zmm3, %zmm12, %zmm14
	vpmaxsd	%zmm7, %zmm5, %zmm2
	vpmaxsd	%zmm3, %zmm12, %zmm3
	vpminsd	%zmm7, %zmm5, %zmm12
	vpmaxsd	%zmm0, %zmm13, %zmm5
	vpminsd	%zmm0, %zmm13, %zmm5{%k1}
	vpshufd	$177, %zmm11, %zmm0
	vpminsd	%zmm18, %zmm19, %zmm21
	vpmaxsd	%zmm0, %zmm11, %zmm13
	vpmaxsd	%zmm18, %zmm19, %zmm19
	vpminsd	%zmm0, %zmm11, %zmm13{%k1}
	vpshufd	$177, %zmm14, %zmm0
	vpminsd	%zmm8, %zmm16, %zmm22
	vpmaxsd	%zmm0, %zmm14, %zmm18
	vpmaxsd	%zmm8, %zmm16, %zmm6
	vpshufd	$177, %zmm22, %zmm1
	vpminsd	%zmm0, %zmm14, %zmm18{%k1}
	vpshufd	$177, %zmm3, %zmm0
	vpmaxsd	%zmm0, %zmm3, %zmm17
	vpminsd	%zmm0, %zmm3, %zmm17{%k1}
	vpshufd	$177, %zmm15, %zmm0
	vpshufd	$177, %zmm4, %zmm3
	vpmaxsd	%zmm0, %zmm15, %zmm14
	vpminsd	%zmm0, %zmm15, %zmm14{%k1}
	vpshufd	$177, %zmm10, %zmm0
	vpmaxsd	%zmm0, %zmm10, %zmm16
	vpminsd	%zmm0, %zmm10, %zmm16{%k1}
	vpshufd	$177, %zmm12, %zmm0
	vpmaxsd	%zmm0, %zmm12, %zmm15
	vpminsd	%zmm0, %zmm12, %zmm15{%k1}
	vpshufd	$177, %zmm2, %zmm0
	vpmaxsd	%zmm0, %zmm2, %zmm12
	vpminsd	%zmm0, %zmm2, %zmm12{%k1}
	vpshufd	$177, %zmm23, %zmm0
	vpmaxsd	%zmm1, %zmm22, %zmm2
	vpmaxsd	%zmm0, %zmm23, %zmm7
	vpminsd	%zmm1, %zmm22, %zmm2{%k1}
	vpminsd	%zmm0, %zmm23, %zmm7{%k1}
	vpshufd	$177, %zmm9, %zmm0
	vpmaxsd	%zmm0, %zmm9, %zmm10
	vpminsd	%zmm0, %zmm9, %zmm10{%k1}
	vpshufd	$177, %zmm6, %zmm0
	vpmaxsd	%zmm0, %zmm6, %zmm8
	vpminsd	%zmm0, %zmm6, %zmm8{%k1}
	vpshufd	$177, %zmm21, %zmm0
	vpmaxsd	%zmm0, %zmm21, %zmm6
	vpminsd	%zmm0, %zmm21, %zmm6{%k1}
	vpshufd	$177, %zmm19, %zmm0
	vpmaxsd	%zmm0, %zmm19, %zmm9
	vpminsd	%zmm0, %zmm19, %zmm9{%k1}
	vpshufd	$177, %zmm20, %zmm0
	vpmaxsd	%zmm0, %zmm20, %zmm1
	vpminsd	%zmm0, %zmm20, %zmm1{%k1}
	vpmaxsd	%zmm3, %zmm4, %zmm0
	vpminsd	%zmm3, %zmm4, %zmm0{%k1}
	vmovdqa64	%zmm9, %zmm3
	cmpq	$3, %rsi
	jbe	.L160
	vpshufd	$27, %zmm2, %zmm2
	vpshufd	$27, %zmm8, %zmm8
	vpshufd	$27, %zmm9, %zmm9
	movl	$85, %eax
	vpminsd	%zmm9, %zmm18, %zmm20
	vpshufd	$27, %zmm7, %zmm7
	vpshufd	$27, %zmm10, %zmm10
	kmovb	%eax, %k2
	vpshufd	$27, %zmm6, %zmm6
	vpshufd	$27, %zmm1, %zmm1
	vpshufd	$27, %zmm0, %zmm0
	vpmaxsd	%zmm9, %zmm18, %zmm23
	vpmaxsd	%zmm8, %zmm14, %zmm18
	vpminsd	%zmm8, %zmm14, %zmm9
	vpminsd	%zmm2, %zmm16, %zmm8
	vpminsd	%zmm1, %zmm13, %zmm21
	vpminsd	%zmm6, %zmm17, %zmm19
	vpshufd	$27, %zmm8, %zmm8
	vpminsd	%zmm0, %zmm5, %zmm22
	vpmaxsd	%zmm0, %zmm5, %zmm4
	vpshufd	$27, %zmm9, %zmm9
	vpmaxsd	%zmm1, %zmm13, %zmm13
	vpmaxsd	%zmm6, %zmm17, %zmm11
	vpshufd	$27, %zmm4, %zmm4
	vpminsd	%zmm10, %zmm15, %zmm5
	vpminsd	%zmm7, %zmm12, %zmm6
	vpshufd	$27, %zmm11, %zmm11
	vpmaxsd	%zmm2, %zmm16, %zmm0
	vpmaxsd	%zmm10, %zmm15, %zmm3
	vpshufd	$27, %zmm13, %zmm2
	vpmaxsd	%zmm7, %zmm12, %zmm1
	vpshufd	$27, %zmm5, %zmm5
	vpshufd	$27, %zmm23, %zmm12
	vpminsd	%zmm8, %zmm20, %zmm7
	vpshufd	$27, %zmm6, %zmm6
	vpminsd	%zmm4, %zmm1, %zmm14
	vpmaxsd	%zmm8, %zmm20, %zmm16
	vpminsd	%zmm2, %zmm3, %zmm17
	vpshufd	$27, %zmm7, %zmm7
	vpminsd	%zmm12, %zmm0, %zmm10
	vpminsd	%zmm6, %zmm22, %zmm13
	vpminsd	%zmm9, %zmm19, %zmm8
	vpmaxsd	%zmm4, %zmm1, %zmm1
	vpmaxsd	%zmm2, %zmm3, %zmm3
	vpminsd	%zmm5, %zmm21, %zmm4
	vpshufd	$27, %zmm8, %zmm8
	vpmaxsd	%zmm5, %zmm21, %zmm5
	vpmaxsd	%zmm6, %zmm22, %zmm6
	vpshufd	$27, %zmm3, %zmm3
	vpminsd	%zmm7, %zmm4, %zmm20
	vpmaxsd	%zmm12, %zmm0, %zmm0
	vpshufd	$27, %zmm5, %zmm5
	vpmaxsd	%zmm9, %zmm19, %zmm9
	vpminsd	%zmm11, %zmm18, %zmm2
	vpshufd	$27, %zmm6, %zmm6
	vpmaxsd	%zmm11, %zmm18, %zmm11
	vpshufd	$27, %zmm1, %zmm1
	vpshufd	$27, %zmm10, %zmm18
	vpminsd	%zmm8, %zmm13, %zmm12
	vpminsd	%zmm6, %zmm9, %zmm15
	vpshufd	$27, %zmm2, %zmm2
	vpminsd	%zmm5, %zmm16, %zmm10
	vpmaxsd	%zmm8, %zmm13, %zmm13
	vpmaxsd	%zmm7, %zmm4, %zmm4
	vpminsd	%zmm18, %zmm17, %zmm19
	vpminsd	%zmm1, %zmm11, %zmm7
	vpmaxsd	%zmm18, %zmm17, %zmm17
	vpshufd	$27, %zmm19, %zmm19
	vpminsd	%zmm3, %zmm0, %zmm18
	vpmaxsd	%zmm6, %zmm9, %zmm9
	vpmaxsd	%zmm1, %zmm11, %zmm1
	vpmaxsd	%zmm5, %zmm16, %zmm6
	vpshufd	$27, %zmm20, %zmm5
	vpminsd	%zmm2, %zmm14, %zmm8
	vpshufd	$27, %zmm13, %zmm11
	vpmaxsd	%zmm3, %zmm0, %zmm0
	vpminsd	%zmm5, %zmm12, %zmm13
	vpshufd	$27, %zmm10, %zmm3
	vpmaxsd	%zmm5, %zmm12, %zmm5
	vpshufd	$27, %zmm9, %zmm9
	vpshufd	$27, %zmm18, %zmm18
	vpshufd	$27, %zmm1, %zmm1
	vpmaxsd	%zmm2, %zmm14, %zmm2
	vpminsd	%zmm19, %zmm8, %zmm16
	vpminsd	%zmm3, %zmm15, %zmm12
	vpminsd	%zmm9, %zmm6, %zmm10
	vpshufd	$27, %zmm2, %zmm2
	vpmaxsd	%zmm3, %zmm15, %zmm15
	vpminsd	%zmm18, %zmm7, %zmm21
	vpmaxsd	%zmm9, %zmm6, %zmm3
	vpmaxsd	%zmm19, %zmm8, %zmm8
	vpmaxsd	%zmm18, %zmm7, %zmm9
	vpminsd	%zmm1, %zmm0, %zmm19
	vpshufd	$27, %zmm5, %zmm7
	vpmaxsd	%zmm1, %zmm0, %zmm0
	vpshufd	$27, %zmm13, %zmm1
	vpminsd	%zmm11, %zmm4, %zmm14
	vpmaxsd	%zmm2, %zmm17, %zmm6
	vpmaxsd	%zmm11, %zmm4, %zmm4
	vpminsd	%zmm2, %zmm17, %zmm11
	vpminsd	%zmm1, %zmm13, %zmm2
	vpmaxsd	%zmm1, %zmm13, %zmm13
	vpminsd	%zmm7, %zmm5, %zmm1
	vpmaxsd	%zmm7, %zmm5, %zmm5
	vmovdqa64	%zmm2, %zmm13{%k2}
	vpblendmq	%zmm1, %zmm5, %zmm7{%k2}
	vpshufd	$27, %zmm14, %zmm1
	vpminsd	%zmm1, %zmm14, %zmm2
	vpmaxsd	%zmm1, %zmm14, %zmm14
	vmovdqa64	%zmm2, %zmm14{%k2}
	vpshufd	$27, %zmm4, %zmm2
	vpminsd	%zmm2, %zmm4, %zmm1
	vpmaxsd	%zmm2, %zmm4, %zmm2
	vmovdqa64	%zmm1, %zmm2{%k2}
	vpshufd	$27, %zmm12, %zmm1
	vpminsd	%zmm1, %zmm12, %zmm4
	vpmaxsd	%zmm1, %zmm12, %zmm12
	vmovdqa64	%zmm4, %zmm12{%k2}
	vpshufd	$27, %zmm15, %zmm4
	vpminsd	%zmm4, %zmm15, %zmm1
	vpmaxsd	%zmm4, %zmm15, %zmm4
	vmovdqa64	%zmm1, %zmm4{%k2}
	vpshufd	$27, %zmm10, %zmm1
	vpminsd	%zmm1, %zmm10, %zmm5
	vpmaxsd	%zmm1, %zmm10, %zmm10
	vpshufd	$27, %zmm3, %zmm1
	vmovdqa64	%zmm5, %zmm10{%k2}
	vpminsd	%zmm1, %zmm3, %zmm5
	vpmaxsd	%zmm1, %zmm3, %zmm1
	vpshufd	$27, %zmm16, %zmm3
	vmovdqa64	%zmm5, %zmm1{%k2}
	vpminsd	%zmm3, %zmm16, %zmm5
	vpmaxsd	%zmm3, %zmm16, %zmm3
	vmovdqa64	%zmm5, %zmm3{%k2}
	vpshufd	$27, %zmm8, %zmm5
	vpminsd	%zmm5, %zmm8, %zmm15
	vpmaxsd	%zmm5, %zmm8, %zmm8
	vpshufd	$27, %zmm11, %zmm5
	vmovdqa64	%zmm15, %zmm8{%k2}
	vpminsd	%zmm5, %zmm11, %zmm15
	vpmaxsd	%zmm5, %zmm11, %zmm11
	vpshufd	$27, %zmm6, %zmm5
	vmovdqa64	%zmm15, %zmm11{%k2}
	vpminsd	%zmm5, %zmm6, %zmm15
	vpmaxsd	%zmm5, %zmm6, %zmm6
	vpshufd	$27, %zmm21, %zmm5
	vmovdqa64	%zmm15, %zmm6{%k2}
	vpminsd	%zmm5, %zmm21, %zmm15
	vpmaxsd	%zmm5, %zmm21, %zmm21
	vpshufd	$27, %zmm9, %zmm5
	vmovdqa64	%zmm15, %zmm21{%k2}
	vpminsd	%zmm5, %zmm9, %zmm15
	vpmaxsd	%zmm5, %zmm9, %zmm9
	vpshufd	$27, %zmm19, %zmm5
	vmovdqa64	%zmm15, %zmm9{%k2}
	vpminsd	%zmm5, %zmm19, %zmm15
	vpmaxsd	%zmm5, %zmm19, %zmm19
	vpshufd	$27, %zmm0, %zmm5
	vmovdqa64	%zmm15, %zmm19{%k2}
	vpminsd	%zmm5, %zmm0, %zmm20
	vpmaxsd	%zmm5, %zmm0, %zmm0
	vpblendmq	%zmm20, %zmm0, %zmm20{%k2}
	vpshufd	$177, %zmm13, %zmm0
	vpmaxsd	%zmm13, %zmm0, %zmm5
	vpminsd	%zmm13, %zmm0, %zmm5{%k1}
	vpshufd	$177, %zmm7, %zmm0
	vpmaxsd	%zmm7, %zmm0, %zmm13
	vpminsd	%zmm7, %zmm0, %zmm13{%k1}
	vpshufd	$177, %zmm14, %zmm0
	vpmaxsd	%zmm14, %zmm0, %zmm18
	vpminsd	%zmm14, %zmm0, %zmm18{%k1}
	vpshufd	$177, %zmm2, %zmm0
	vpmaxsd	%zmm2, %zmm0, %zmm17
	vpminsd	%zmm2, %zmm0, %zmm17{%k1}
	vpshufd	$177, %zmm12, %zmm0
	vpmaxsd	%zmm12, %zmm0, %zmm14
	vpminsd	%zmm12, %zmm0, %zmm14{%k1}
	vpshufd	$177, %zmm4, %zmm0
	vpmaxsd	%zmm4, %zmm0, %zmm16
	vpminsd	%zmm4, %zmm0, %zmm16{%k1}
	vpshufd	$177, %zmm10, %zmm0
	vpshufd	$177, %zmm20, %zmm4
	vpmaxsd	%zmm10, %zmm0, %zmm15
	vpminsd	%zmm10, %zmm0, %zmm15{%k1}
	vpshufd	$177, %zmm1, %zmm0
	vpmaxsd	%zmm1, %zmm0, %zmm12
	vpminsd	%zmm1, %zmm0, %zmm12{%k1}
	vpshufd	$177, %zmm3, %zmm0
	vpshufd	$177, %zmm11, %zmm1
	vpmaxsd	%zmm3, %zmm0, %zmm7
	vpmaxsd	%zmm11, %zmm1, %zmm2
	vpminsd	%zmm3, %zmm0, %zmm7{%k1}
	vpshufd	$177, %zmm8, %zmm0
	vpminsd	%zmm11, %zmm1, %zmm2{%k1}
	vpmaxsd	%zmm8, %zmm0, %zmm10
	vpshufd	$177, %zmm9, %zmm1
	vpminsd	%zmm8, %zmm0, %zmm10{%k1}
	vpshufd	$177, %zmm6, %zmm0
	vpmaxsd	%zmm9, %zmm1, %zmm3
	vpmaxsd	%zmm6, %zmm0, %zmm8
	vpminsd	%zmm9, %zmm1, %zmm3{%k1}
	vpminsd	%zmm6, %zmm0, %zmm8{%k1}
	vpshufd	$177, %zmm21, %zmm0
	vpmaxsd	%zmm21, %zmm0, %zmm6
	vpminsd	%zmm21, %zmm0, %zmm6{%k1}
	vpshufd	$177, %zmm19, %zmm0
	vpmaxsd	%zmm19, %zmm0, %zmm1
	vpminsd	%zmm19, %zmm0, %zmm1{%k1}
	vpmaxsd	%zmm20, %zmm4, %zmm0
	vpminsd	%zmm20, %zmm4, %zmm0{%k1}
	cmpq	$7, %rsi
	jbe	.L160
	vmovdqa32	.LC1(%rip), %zmm9
	movl	$51, %eax
	kmovb	%eax, %k3
	vpermd	%zmm2, %zmm9, %zmm2
	vpermd	%zmm1, %zmm9, %zmm1
	vpermd	%zmm7, %zmm9, %zmm7
	vpminsd	%zmm1, %zmm13, %zmm21
	vpermd	%zmm10, %zmm9, %zmm10
	vpermd	%zmm8, %zmm9, %zmm8
	vpermd	%zmm6, %zmm9, %zmm6
	vpermd	%zmm3, %zmm9, %zmm3
	vpermd	%zmm0, %zmm9, %zmm0
	vpminsd	%zmm2, %zmm16, %zmm11
	vpmaxsd	%zmm1, %zmm13, %zmm1
	vpminsd	%zmm3, %zmm18, %zmm20
	vpminsd	%zmm6, %zmm17, %zmm19
	vpermd	%zmm11, %zmm9, %zmm11
	vpminsd	%zmm0, %zmm5, %zmm22
	vpminsd	%zmm7, %zmm12, %zmm4
	vpermd	%zmm1, %zmm9, %zmm1
	vpmaxsd	%zmm6, %zmm17, %zmm6
	vpmaxsd	%zmm0, %zmm5, %zmm0
	vpminsd	%zmm8, %zmm14, %zmm17
	vpminsd	%zmm10, %zmm15, %zmm5
	vpermd	%zmm0, %zmm9, %zmm0
	vpmaxsd	%zmm10, %zmm15, %zmm10
	vpmaxsd	%zmm3, %zmm18, %zmm3
	vpermd	%zmm17, %zmm9, %zmm17
	vpminsd	%zmm1, %zmm10, %zmm13
	vpmaxsd	%zmm8, %zmm14, %zmm8
	vpermd	%zmm3, %zmm9, %zmm3
	vpmaxsd	%zmm2, %zmm16, %zmm2
	vpmaxsd	%zmm7, %zmm12, %zmm7
	vpermd	%zmm5, %zmm9, %zmm5
	vpermd	%zmm6, %zmm9, %zmm12
	vpermd	%zmm4, %zmm9, %zmm6
	vpmaxsd	%zmm1, %zmm10, %zmm4
	vpminsd	%zmm11, %zmm20, %zmm10
	vpminsd	%zmm5, %zmm21, %zmm18
	vpermd	%zmm4, %zmm9, %zmm4
	vpminsd	%zmm0, %zmm7, %zmm16
	vpmaxsd	%zmm11, %zmm20, %zmm1
	vpminsd	%zmm6, %zmm22, %zmm14
	vpminsd	%zmm3, %zmm2, %zmm15
	vpminsd	%zmm17, %zmm19, %zmm11
	vpmaxsd	%zmm0, %zmm7, %zmm7
	vpermd	%zmm15, %zmm9, %zmm15
	vpmaxsd	%zmm3, %zmm2, %zmm0
	vpmaxsd	%zmm5, %zmm21, %zmm5
	vpermd	%zmm11, %zmm9, %zmm11
	vpmaxsd	%zmm17, %zmm19, %zmm3
	vpmaxsd	%zmm12, %zmm8, %zmm2
	vpermd	%zmm5, %zmm9, %zmm5
	vpminsd	%zmm12, %zmm8, %zmm17
	vpmaxsd	%zmm6, %zmm22, %zmm6
	vpermd	%zmm10, %zmm9, %zmm8
	vpermd	%zmm6, %zmm9, %zmm6
	vpermd	%zmm17, %zmm9, %zmm17
	vpermd	%zmm7, %zmm9, %zmm7
	vpminsd	%zmm8, %zmm18, %zmm19
	vpminsd	%zmm11, %zmm14, %zmm12
	vpmaxsd	%zmm8, %zmm18, %zmm10
	vpmaxsd	%zmm11, %zmm14, %zmm14
	vpminsd	%zmm6, %zmm3, %zmm8
	vpmaxsd	%zmm15, %zmm13, %zmm11
	vpminsd	%zmm5, %zmm1, %zmm18
	vpmaxsd	%zmm6, %zmm3, %zmm3
	vpmaxsd	%zmm5, %zmm1, %zmm1
	vpminsd	%zmm17, %zmm16, %zmm6
	vpermd	%zmm19, %zmm9, %zmm5
	vpmaxsd	%zmm17, %zmm16, %zmm16
	vpminsd	%zmm15, %zmm13, %zmm17
	vpermd	%zmm18, %zmm9, %zmm18
	vpminsd	%zmm7, %zmm2, %zmm13
	vpmaxsd	%zmm7, %zmm2, %zmm2
	vpermd	%zmm17, %zmm9, %zmm17
	vpermd	%zmm2, %zmm9, %zmm2
	vpminsd	%zmm4, %zmm0, %zmm15
	vpmaxsd	%zmm4, %zmm0, %zmm0
	vpermd	%zmm14, %zmm9, %zmm4
	vpminsd	%zmm5, %zmm12, %zmm14
	vpminsd	%zmm17, %zmm6, %zmm20
	vpermd	%zmm3, %zmm9, %zmm3
	vpermd	%zmm15, %zmm9, %zmm15
	vpmaxsd	%zmm5, %zmm12, %zmm5
	vpmaxsd	%zmm17, %zmm6, %zmm6
	vpminsd	%zmm2, %zmm0, %zmm17
	vpermd	%zmm16, %zmm9, %zmm16
	vpmaxsd	%zmm2, %zmm0, %zmm0
	vpermd	%zmm14, %zmm9, %zmm2
	vpminsd	%zmm4, %zmm10, %zmm12
	vpminsd	%zmm18, %zmm8, %zmm7
	vpmaxsd	%zmm4, %zmm10, %zmm4
	vpmaxsd	%zmm18, %zmm8, %zmm8
	vpminsd	%zmm3, %zmm1, %zmm10
	vpminsd	%zmm15, %zmm13, %zmm18
	vpmaxsd	%zmm3, %zmm1, %zmm1
	vpmaxsd	%zmm15, %zmm13, %zmm3
	vpminsd	%zmm2, %zmm14, %zmm13
	vpmaxsd	%zmm2, %zmm14, %zmm14
	vpermd	%zmm5, %zmm9, %zmm2
	vpminsd	%zmm16, %zmm11, %zmm19
	vmovdqa64	%zmm13, %zmm14{%k3}
	vpminsd	%zmm2, %zmm5, %zmm13
	vpmaxsd	%zmm2, %zmm5, %zmm5
	vpermd	%zmm12, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm5{%k3}
	vpmaxsd	%zmm16, %zmm11, %zmm11
	vpminsd	%zmm2, %zmm12, %zmm13
	vpmaxsd	%zmm2, %zmm12, %zmm12
	vpermd	%zmm4, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm12{%k3}
	vpminsd	%zmm2, %zmm4, %zmm13
	vpmaxsd	%zmm2, %zmm4, %zmm4
	vpermd	%zmm7, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm4{%k3}
	vpshufd	$78, %zmm5, %zmm16
	vpminsd	%zmm2, %zmm7, %zmm13
	vpmaxsd	%zmm2, %zmm7, %zmm7
	vpermd	%zmm8, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm7{%k3}
	vpminsd	%zmm2, %zmm8, %zmm13
	vpmaxsd	%zmm2, %zmm8, %zmm8
	vpermd	%zmm10, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm8{%k3}
	vpshufd	$78, %zmm12, %zmm15
	vpminsd	%zmm2, %zmm10, %zmm13
	vpmaxsd	%zmm2, %zmm10, %zmm10
	vpermd	%zmm1, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm10{%k3}
	vpminsd	%zmm2, %zmm1, %zmm13
	vpmaxsd	%zmm2, %zmm1, %zmm1
	vpermd	%zmm20, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm1{%k3}
	vpminsd	%zmm2, %zmm20, %zmm13
	vpmaxsd	%zmm2, %zmm20, %zmm20
	vpermd	%zmm6, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm20{%k3}
	vpminsd	%zmm2, %zmm6, %zmm13
	vpmaxsd	%zmm2, %zmm6, %zmm6
	vpermd	%zmm19, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm6{%k3}
	vpminsd	%zmm2, %zmm19, %zmm13
	vpmaxsd	%zmm2, %zmm19, %zmm19
	vpermd	%zmm11, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm19{%k3}
	vpminsd	%zmm2, %zmm11, %zmm13
	vpmaxsd	%zmm2, %zmm11, %zmm2
	vpermd	%zmm18, %zmm9, %zmm11
	vmovdqa64	%zmm13, %zmm2{%k3}
	vpminsd	%zmm11, %zmm18, %zmm13
	vpmaxsd	%zmm11, %zmm18, %zmm18
	vpermd	%zmm3, %zmm9, %zmm11
	vmovdqa64	%zmm13, %zmm18{%k3}
	vpminsd	%zmm11, %zmm3, %zmm13
	vpmaxsd	%zmm11, %zmm3, %zmm3
	vpermd	%zmm17, %zmm9, %zmm11
	vmovdqa64	%zmm13, %zmm3{%k3}
	vpermd	%zmm0, %zmm9, %zmm9
	vpminsd	%zmm11, %zmm17, %zmm13
	vpmaxsd	%zmm11, %zmm17, %zmm17
	vpshufd	$78, %zmm2, %zmm21
	vmovdqa64	%zmm13, %zmm17{%k3}
	vpshufd	$78, %zmm14, %zmm13
	vpminsd	%zmm9, %zmm0, %zmm11
	vpmaxsd	%zmm9, %zmm0, %zmm0
	vpminsd	%zmm14, %zmm13, %zmm9
	vpmaxsd	%zmm14, %zmm13, %zmm13
	vpshufd	$78, %zmm4, %zmm14
	vmovdqa64	%zmm11, %zmm0{%k3}
	vmovdqa64	%zmm9, %zmm13{%k2}
	vpminsd	%zmm5, %zmm16, %zmm9
	vpmaxsd	%zmm5, %zmm16, %zmm16
	vpminsd	%zmm12, %zmm15, %zmm5
	vpmaxsd	%zmm12, %zmm15, %zmm15
	vpshufd	$78, %zmm7, %zmm12
	vmovdqa64	%zmm5, %zmm15{%k2}
	vpshufd	$78, %zmm8, %zmm11
	vpminsd	%zmm4, %zmm14, %zmm5
	vpmaxsd	%zmm4, %zmm14, %zmm14
	vpminsd	%zmm7, %zmm12, %zmm4
	vmovdqa64	%zmm9, %zmm16{%k2}
	vpmaxsd	%zmm7, %zmm12, %zmm12
	vpshufd	$78, %zmm10, %zmm7
	vmovdqa64	%zmm5, %zmm14{%k2}
	vmovdqa64	%zmm4, %zmm12{%k2}
	vpminsd	%zmm8, %zmm11, %zmm4
	vpmaxsd	%zmm8, %zmm11, %zmm11
	vmovdqa64	%zmm4, %zmm11{%k2}
	vpminsd	%zmm10, %zmm7, %zmm4
	vpmaxsd	%zmm10, %zmm7, %zmm7
	vmovdqa64	%zmm4, %zmm7{%k2}
	vpshufd	$78, %zmm20, %zmm10
	vpshufd	$78, %zmm1, %zmm4
	vpminsd	%zmm1, %zmm4, %zmm5
	vpshufd	$78, %zmm6, %zmm8
	vpmaxsd	%zmm1, %zmm4, %zmm1
	vpminsd	%zmm20, %zmm10, %zmm4
	vpmaxsd	%zmm20, %zmm10, %zmm10
	vpshufd	$78, %zmm18, %zmm20
	vmovdqa64	%zmm4, %zmm10{%k2}
	vpminsd	%zmm6, %zmm8, %zmm4
	vpmaxsd	%zmm6, %zmm8, %zmm8
	vpshufd	$78, %zmm19, %zmm6
	vmovdqa64	%zmm4, %zmm8{%k2}
	vpshufd	$78, %zmm0, %zmm9
	vpminsd	%zmm19, %zmm6, %zmm4
	vpmaxsd	%zmm19, %zmm6, %zmm6
	vpshufd	$78, %zmm3, %zmm19
	vmovdqa64	%zmm4, %zmm6{%k2}
	vpminsd	%zmm2, %zmm21, %zmm4
	vpmaxsd	%zmm2, %zmm21, %zmm21
	vpminsd	%zmm18, %zmm20, %zmm2
	vpmaxsd	%zmm18, %zmm20, %zmm20
	vmovdqa64	%zmm4, %zmm21{%k2}
	vmovdqa64	%zmm2, %zmm20{%k2}
	vpshufd	$78, %zmm17, %zmm4
	vpminsd	%zmm3, %zmm19, %zmm2
	vpmaxsd	%zmm3, %zmm19, %zmm19
	vmovdqa64	%zmm5, %zmm1{%k2}
	vmovdqa64	%zmm2, %zmm19{%k2}
	vpminsd	%zmm17, %zmm4, %zmm2
	vpmaxsd	%zmm17, %zmm4, %zmm4
	vmovdqa64	%zmm2, %zmm4{%k2}
	vpminsd	%zmm0, %zmm9, %zmm2
	vpmaxsd	%zmm0, %zmm9, %zmm9
	vpshufd	$177, %zmm13, %zmm0
	vmovdqa64	%zmm2, %zmm9{%k2}
	vpmaxsd	%zmm13, %zmm0, %zmm5
	vpminsd	%zmm13, %zmm0, %zmm5{%k1}
	vpshufd	$177, %zmm16, %zmm0
	vpmaxsd	%zmm16, %zmm0, %zmm13
	vpminsd	%zmm16, %zmm0, %zmm13{%k1}
	vpshufd	$177, %zmm15, %zmm0
	vpmaxsd	%zmm15, %zmm0, %zmm18
	vpminsd	%zmm15, %zmm0, %zmm18{%k1}
	vpshufd	$177, %zmm14, %zmm0
	vpmaxsd	%zmm14, %zmm0, %zmm17
	vpminsd	%zmm14, %zmm0, %zmm17{%k1}
	vpshufd	$177, %zmm12, %zmm0
	vpmaxsd	%zmm12, %zmm0, %zmm14
	vpminsd	%zmm12, %zmm0, %zmm14{%k1}
	vpshufd	$177, %zmm11, %zmm0
	vpmaxsd	%zmm11, %zmm0, %zmm16
	vpminsd	%zmm11, %zmm0, %zmm16{%k1}
	vpshufd	$177, %zmm7, %zmm0
	vpshufd	$177, %zmm9, %zmm11
	vpmaxsd	%zmm7, %zmm0, %zmm15
	vpminsd	%zmm7, %zmm0, %zmm15{%k1}
	vpshufd	$177, %zmm1, %zmm0
	vpmaxsd	%zmm1, %zmm0, %zmm12
	vpminsd	%zmm1, %zmm0, %zmm12{%k1}
	vpshufd	$177, %zmm10, %zmm0
	vpshufd	$177, %zmm19, %zmm1
	vpmaxsd	%zmm10, %zmm0, %zmm7
	vpmaxsd	%zmm19, %zmm1, %zmm3
	vpminsd	%zmm10, %zmm0, %zmm7{%k1}
	vpshufd	$177, %zmm8, %zmm0
	vpminsd	%zmm19, %zmm1, %zmm3{%k1}
	vpmaxsd	%zmm8, %zmm0, %zmm10
	vpminsd	%zmm8, %zmm0, %zmm10{%k1}
	vpshufd	$177, %zmm6, %zmm0
	vpmaxsd	%zmm6, %zmm0, %zmm2
	vpminsd	%zmm6, %zmm0, %zmm2{%k1}
	vpshufd	$177, %zmm21, %zmm0
	vpmaxsd	%zmm21, %zmm0, %zmm8
	vpminsd	%zmm21, %zmm0, %zmm8{%k1}
	vpshufd	$177, %zmm20, %zmm0
	vpmaxsd	%zmm20, %zmm0, %zmm6
	vpminsd	%zmm20, %zmm0, %zmm6{%k1}
	vpshufd	$177, %zmm4, %zmm0
	vpmaxsd	%zmm4, %zmm0, %zmm1
	vpminsd	%zmm4, %zmm0, %zmm1{%k1}
	vpmaxsd	%zmm9, %zmm11, %zmm0
	vmovdqa32	%zmm0, %zmm4
	vpminsd	%zmm9, %zmm11, %zmm4{%k1}
	vmovdqa64	%zmm4, %zmm0
	cmpq	$15, %rsi
	jbe	.L160
	vmovdqa32	.LC2(%rip), %zmm0
	movl	$65535, %eax
	kmovd	%eax, %k1
	movl	$51, %eax
	vpermd	%zmm6, %zmm0, %zmm11
	vpermd	%zmm10, %zmm0, %zmm10
	vpermd	%zmm4, %zmm0, %zmm6
	vpermd	%zmm2, %zmm0, %zmm2
	vpermd	%zmm1, %zmm0, %zmm1
	vpminsd	%zmm6, %zmm5, %zmm20
	vpminsd	%zmm1, %zmm13, %zmm19
	vpermd	%zmm8, %zmm0, %zmm8
	vpermd	%zmm7, %zmm0, %zmm7
	vpermd	%zmm3, %zmm0, %zmm3
	vpmaxsd	%zmm6, %zmm5, %zmm6
	vpmaxsd	%zmm1, %zmm13, %zmm1
	vpminsd	%zmm10, %zmm15, %zmm5
	vpminsd	%zmm2, %zmm16, %zmm13
	vpermd	%zmm6, %zmm0, %zmm6
	vpminsd	%zmm3, %zmm18, %zmm9
	vpermd	%zmm5, %zmm0, %zmm5
	vpminsd	%zmm8, %zmm14, %zmm4
	vpmaxsd	%zmm3, %zmm18, %zmm3
	vpminsd	%zmm11, %zmm17, %zmm18
	vpermd	%zmm1, %zmm0, %zmm1
	vpmaxsd	%zmm11, %zmm17, %zmm11
	vpmaxsd	%zmm8, %zmm14, %zmm17
	vpermd	%zmm3, %zmm0, %zmm3
	vpminsd	%zmm7, %zmm12, %zmm8
	vpmaxsd	%zmm7, %zmm12, %zmm7
	vpermd	%zmm13, %zmm0, %zmm12
	vpmaxsd	%zmm10, %zmm15, %zmm10
	vpminsd	%zmm6, %zmm7, %zmm13
	vpermd	%zmm11, %zmm0, %zmm15
	vpminsd	%zmm5, %zmm19, %zmm14
	vpmaxsd	%zmm2, %zmm16, %zmm2
	vpermd	%zmm4, %zmm0, %zmm4
	vpermd	%zmm8, %zmm0, %zmm8
	vpmaxsd	%zmm6, %zmm7, %zmm6
	vpmaxsd	%zmm5, %zmm19, %zmm7
	vpminsd	%zmm12, %zmm9, %zmm19
	vpminsd	%zmm8, %zmm20, %zmm16
	vpermd	%zmm7, %zmm0, %zmm7
	vpminsd	%zmm1, %zmm10, %zmm11
	vpermd	%zmm19, %zmm0, %zmm19
	vpmaxsd	%zmm1, %zmm10, %zmm5
	vpmaxsd	%zmm8, %zmm20, %zmm8
	vpmaxsd	%zmm12, %zmm9, %zmm1
	vpermd	%zmm5, %zmm0, %zmm5
	vpminsd	%zmm3, %zmm2, %zmm12
	vpminsd	%zmm4, %zmm18, %zmm9
	vpermd	%zmm8, %zmm0, %zmm8
	vpmaxsd	%zmm4, %zmm18, %zmm4
	vpminsd	%zmm15, %zmm17, %zmm18
	vpermd	%zmm9, %zmm0, %zmm9
	vpermd	%zmm12, %zmm0, %zmm12
	vpermd	%zmm18, %zmm0, %zmm18
	vpermd	%zmm6, %zmm0, %zmm6
	vpmaxsd	%zmm3, %zmm2, %zmm2
	vpmaxsd	%zmm15, %zmm17, %zmm3
	vpminsd	%zmm19, %zmm14, %zmm17
	vpminsd	%zmm9, %zmm16, %zmm15
	vpmaxsd	%zmm9, %zmm16, %zmm10
	vpmaxsd	%zmm19, %zmm14, %zmm14
	vpminsd	%zmm8, %zmm4, %zmm9
	vpminsd	%zmm7, %zmm1, %zmm16
	vpminsd	%zmm12, %zmm11, %zmm19
	vpmaxsd	%zmm7, %zmm1, %zmm1
	vpermd	%zmm16, %zmm0, %zmm16
	vpminsd	%zmm18, %zmm13, %zmm7
	vpmaxsd	%zmm12, %zmm11, %zmm11
	vpermd	%zmm19, %zmm0, %zmm19
	vpminsd	%zmm5, %zmm2, %zmm12
	vpmaxsd	%zmm8, %zmm4, %zmm4
	vpmaxsd	%zmm18, %zmm13, %zmm13
	vpminsd	%zmm6, %zmm3, %zmm8
	vpermd	%zmm4, %zmm0, %zmm4
	vpmaxsd	%zmm6, %zmm3, %zmm3
	vpermd	%zmm17, %zmm0, %zmm6
	vpermd	%zmm13, %zmm0, %zmm13
	vpermd	%zmm12, %zmm0, %zmm12
	vpermd	%zmm3, %zmm0, %zmm3
	vpmaxsd	%zmm5, %zmm2, %zmm2
	vpermd	%zmm10, %zmm0, %zmm5
	vpminsd	%zmm6, %zmm15, %zmm10
	vpminsd	%zmm16, %zmm9, %zmm17
	vpminsd	%zmm5, %zmm14, %zmm18
	vpmaxsd	%zmm6, %zmm15, %zmm6
	vpmaxsd	%zmm5, %zmm14, %zmm5
	vpmaxsd	%zmm16, %zmm9, %zmm9
	vpminsd	%zmm13, %zmm11, %zmm14
	vpminsd	%zmm4, %zmm1, %zmm16
	vpmaxsd	%zmm13, %zmm11, %zmm11
	vpmaxsd	%zmm4, %zmm1, %zmm1
	vpminsd	%zmm12, %zmm8, %zmm13
	vpmaxsd	%zmm12, %zmm8, %zmm4
	vpminsd	%zmm3, %zmm2, %zmm8
	vpmaxsd	%zmm3, %zmm2, %zmm2
	vpermd	%zmm10, %zmm0, %zmm3
	vpminsd	%zmm3, %zmm10, %zmm12
	vpmaxsd	%zmm3, %zmm10, %zmm10
	vpermd	%zmm6, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm10{%k1}
	vpminsd	%zmm3, %zmm6, %zmm12
	vpmaxsd	%zmm3, %zmm6, %zmm6
	vpermd	%zmm18, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm6{%k1}
	vpminsd	%zmm19, %zmm7, %zmm15
	vpminsd	%zmm3, %zmm18, %zmm12
	vpmaxsd	%zmm3, %zmm18, %zmm18
	vpermd	%zmm5, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm18{%k1}
	vpminsd	%zmm3, %zmm5, %zmm12
	vpmaxsd	%zmm3, %zmm5, %zmm5
	vpermd	%zmm17, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm5{%k1}
	vpmaxsd	%zmm19, %zmm7, %zmm7
	vpminsd	%zmm3, %zmm17, %zmm12
	vpmaxsd	%zmm3, %zmm17, %zmm17
	vpermd	%zmm9, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm17{%k1}
	vpminsd	%zmm3, %zmm9, %zmm12
	vpmaxsd	%zmm3, %zmm9, %zmm9
	vpermd	%zmm16, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm9{%k1}
	vshufi32x4	$177, %zmm5, %zmm5, %zmm22
	vpminsd	%zmm3, %zmm16, %zmm12
	vpmaxsd	%zmm3, %zmm16, %zmm16
	vpermd	%zmm1, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm16{%k1}
	vpminsd	%zmm3, %zmm1, %zmm12
	vpmaxsd	%zmm3, %zmm1, %zmm1
	vpermd	%zmm15, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm1{%k1}
	vshufi32x4	$177, %zmm17, %zmm17, %zmm21
	vpminsd	%zmm3, %zmm15, %zmm12
	vpmaxsd	%zmm3, %zmm15, %zmm15
	vpermd	%zmm7, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm15{%k1}
	vpminsd	%zmm3, %zmm7, %zmm12
	vpmaxsd	%zmm3, %zmm7, %zmm7
	vpermd	%zmm14, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm7{%k1}
	vshufi32x4	$177, %zmm9, %zmm9, %zmm20
	vpminsd	%zmm3, %zmm14, %zmm12
	vpmaxsd	%zmm3, %zmm14, %zmm14
	vpermd	%zmm11, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm14{%k1}
	vpminsd	%zmm3, %zmm11, %zmm12
	vpmaxsd	%zmm3, %zmm11, %zmm3
	vpermd	%zmm13, %zmm0, %zmm11
	vmovdqu16	%zmm12, %zmm3{%k1}
	vshufi32x4	$177, %zmm16, %zmm16, %zmm19
	vpminsd	%zmm11, %zmm13, %zmm12
	vpmaxsd	%zmm11, %zmm13, %zmm13
	vpermd	%zmm4, %zmm0, %zmm11
	vmovdqu16	%zmm12, %zmm13{%k1}
	vpminsd	%zmm11, %zmm4, %zmm12
	vpmaxsd	%zmm11, %zmm4, %zmm4
	vpermd	%zmm8, %zmm0, %zmm11
	vmovdqu16	%zmm12, %zmm4{%k1}
	vpermd	%zmm2, %zmm0, %zmm0
	vpminsd	%zmm11, %zmm8, %zmm12
	vpmaxsd	%zmm11, %zmm8, %zmm8
	vmovdqu16	%zmm12, %zmm8{%k1}
	vpminsd	%zmm0, %zmm2, %zmm11
	vshufi32x4	$177, %zmm10, %zmm10, %zmm12
	vpmaxsd	%zmm0, %zmm2, %zmm0
	vpminsd	%zmm10, %zmm12, %zmm2
	vmovdqu16	%zmm11, %zmm0{%k1}
	vpmaxsd	%zmm10, %zmm12, %zmm12
	kmovb	%eax, %k1
	vshufi32x4	$177, %zmm6, %zmm6, %zmm11
	vmovdqa64	%zmm2, %zmm12{%k1}
	vpminsd	%zmm6, %zmm11, %zmm2
	vpmaxsd	%zmm6, %zmm11, %zmm11
	movl	$85, %eax
	vshufi32x4	$177, %zmm18, %zmm18, %zmm10
	vmovdqa64	%zmm2, %zmm11{%k1}
	vshufi32x4	$177, %zmm14, %zmm14, %zmm6
	vpminsd	%zmm18, %zmm10, %zmm2
	vpmaxsd	%zmm18, %zmm10, %zmm10
	vshufi32x4	$177, %zmm15, %zmm15, %zmm18
	vmovdqa64	%zmm2, %zmm10{%k1}
	vpminsd	%zmm5, %zmm22, %zmm2
	vpmaxsd	%zmm5, %zmm22, %zmm22
	vmovdqa64	%zmm2, %zmm22{%k1}
	vpminsd	%zmm17, %zmm21, %zmm2
	vpmaxsd	%zmm17, %zmm21, %zmm21
	vmovdqa64	%zmm2, %zmm21{%k1}
	vpminsd	%zmm9, %zmm20, %zmm2
	vpmaxsd	%zmm9, %zmm20, %zmm20
	vmovdqa64	%zmm2, %zmm20{%k1}
	vpminsd	%zmm16, %zmm19, %zmm2
	vpmaxsd	%zmm16, %zmm19, %zmm19
	vmovdqa64	%zmm2, %zmm19{%k1}
	vshufi32x4	$177, %zmm1, %zmm1, %zmm2
	vshufi32x4	$177, %zmm7, %zmm7, %zmm17
	vpminsd	%zmm1, %zmm2, %zmm5
	vpmaxsd	%zmm1, %zmm2, %zmm2
	vshufi32x4	$177, %zmm3, %zmm3, %zmm16
	vpminsd	%zmm15, %zmm18, %zmm1
	vpmaxsd	%zmm15, %zmm18, %zmm18
	vshufi32x4	$177, %zmm13, %zmm13, %zmm15
	vmovdqa64	%zmm1, %zmm18{%k1}
	vpminsd	%zmm7, %zmm17, %zmm1
	vpmaxsd	%zmm7, %zmm17, %zmm17
	vmovdqa64	%zmm1, %zmm17{%k1}
	vpminsd	%zmm14, %zmm6, %zmm1
	vpmaxsd	%zmm14, %zmm6, %zmm6
	vmovdqa64	%zmm1, %zmm6{%k1}
	vpminsd	%zmm3, %zmm16, %zmm1
	vpmaxsd	%zmm3, %zmm16, %zmm16
	vmovdqa64	%zmm1, %zmm16{%k1}
	vshufi32x4	$177, %zmm4, %zmm4, %zmm14
	vpminsd	%zmm13, %zmm15, %zmm1
	vpmaxsd	%zmm13, %zmm15, %zmm15
	vshufi32x4	$177, %zmm8, %zmm8, %zmm9
	vmovdqa64	%zmm5, %zmm2{%k1}
	vmovdqa64	%zmm1, %zmm15{%k1}
	vpminsd	%zmm4, %zmm14, %zmm1
	vpmaxsd	%zmm4, %zmm14, %zmm14
	vmovdqa64	%zmm1, %zmm14{%k1}
	vshufi32x4	$177, %zmm0, %zmm0, %zmm5
	vpminsd	%zmm8, %zmm9, %zmm1
	vpshufd	$78, %zmm12, %zmm13
	vpmaxsd	%zmm8, %zmm9, %zmm9
	vpshufd	$78, %zmm21, %zmm7
	vmovdqa64	%zmm1, %zmm9{%k1}
	vpminsd	%zmm0, %zmm5, %zmm1
	vpmaxsd	%zmm0, %zmm5, %zmm5
	vpminsd	%zmm12, %zmm13, %zmm0
	vpmaxsd	%zmm12, %zmm13, %zmm13
	vpshufd	$78, %zmm11, %zmm12
	vmovdqa64	%zmm1, %zmm5{%k1}
	kmovb	%eax, %k1
	vmovdqa64	%zmm0, %zmm13{%k1}
	vpminsd	%zmm11, %zmm12, %zmm0
	vpmaxsd	%zmm11, %zmm12, %zmm12
	vpshufd	$78, %zmm10, %zmm11
	vpshufd	$78, %zmm20, %zmm4
	movl	$21845, %eax
	vmovdqa64	%zmm0, %zmm12{%k1}
	vpminsd	%zmm10, %zmm11, %zmm0
	vpmaxsd	%zmm10, %zmm11, %zmm11
	vpshufd	$78, %zmm22, %zmm10
	vmovdqa64	%zmm0, %zmm11{%k1}
	vpshufd	$78, %zmm19, %zmm3
	vpminsd	%zmm22, %zmm10, %zmm0
	vpmaxsd	%zmm22, %zmm10, %zmm10
	vpshufd	$78, %zmm2, %zmm1
	vmovdqa64	%zmm0, %zmm10{%k1}
	vpminsd	%zmm21, %zmm7, %zmm0
	vpmaxsd	%zmm21, %zmm7, %zmm7
	vmovdqa64	%zmm0, %zmm7{%k1}
	vpminsd	%zmm20, %zmm4, %zmm0
	vpmaxsd	%zmm20, %zmm4, %zmm4
	vmovdqa64	%zmm0, %zmm4{%k1}
	vpminsd	%zmm19, %zmm3, %zmm0
	vpmaxsd	%zmm19, %zmm3, %zmm3
	vmovdqa64	%zmm0, %zmm3{%k1}
	vpminsd	%zmm2, %zmm1, %zmm0
	vpmaxsd	%zmm2, %zmm1, %zmm1
	vpshufd	$78, %zmm18, %zmm2
	vmovdqa64	%zmm0, %zmm1{%k1}
	vpshufd	$78, %zmm15, %zmm21
	vpminsd	%zmm18, %zmm2, %zmm0
	vpmaxsd	%zmm18, %zmm2, %zmm2
	vpshufd	$78, %zmm14, %zmm20
	vmovdqa64	%zmm0, %zmm2{%k1}
	vpshufd	$78, %zmm17, %zmm0
	vpshufd	$78, %zmm9, %zmm19
	vpminsd	%zmm17, %zmm0, %zmm8
	vpmaxsd	%zmm17, %zmm0, %zmm0
	vmovdqa64	%zmm8, %zmm0{%k1}
	vpshufd	$78, %zmm6, %zmm8
	vpminsd	%zmm6, %zmm8, %zmm17
	vpmaxsd	%zmm6, %zmm8, %zmm8
	vpshufd	$78, %zmm16, %zmm6
	vmovdqa64	%zmm17, %zmm8{%k1}
	vpminsd	%zmm16, %zmm6, %zmm17
	vpmaxsd	%zmm16, %zmm6, %zmm6
	vpminsd	%zmm15, %zmm21, %zmm16
	vpmaxsd	%zmm15, %zmm21, %zmm21
	vmovdqa64	%zmm17, %zmm6{%k1}
	vpminsd	%zmm14, %zmm20, %zmm15
	vpmaxsd	%zmm14, %zmm20, %zmm20
	vmovdqa64	%zmm16, %zmm21{%k1}
	vpminsd	%zmm9, %zmm19, %zmm14
	vpmaxsd	%zmm9, %zmm19, %zmm19
	vpshufd	$78, %zmm5, %zmm9
	vmovdqa64	%zmm14, %zmm19{%k1}
	vpminsd	%zmm5, %zmm9, %zmm14
	vpmaxsd	%zmm5, %zmm9, %zmm9
	vmovdqa64	%zmm14, %zmm9{%k1}
	vpshufd	$177, %zmm13, %zmm14
	vmovdqa64	%zmm15, %zmm20{%k1}
	kmovw	%eax, %k1
	vpmaxsd	%zmm13, %zmm14, %zmm5
	vpminsd	%zmm13, %zmm14, %zmm5{%k1}
	vpshufd	$177, %zmm12, %zmm14
	vpmaxsd	%zmm12, %zmm14, %zmm13
	vpminsd	%zmm12, %zmm14, %zmm13{%k1}
	vpshufd	$177, %zmm11, %zmm12
	vpmaxsd	%zmm11, %zmm12, %zmm18
	vpminsd	%zmm11, %zmm12, %zmm18{%k1}
	vpshufd	$177, %zmm10, %zmm11
	vpmaxsd	%zmm10, %zmm11, %zmm17
	vpminsd	%zmm10, %zmm11, %zmm17{%k1}
	vpshufd	$177, %zmm7, %zmm10
	vpmaxsd	%zmm7, %zmm10, %zmm14
	vpminsd	%zmm7, %zmm10, %zmm14{%k1}
	vpshufd	$177, %zmm4, %zmm7
	vpmaxsd	%zmm4, %zmm7, %zmm16
	vpminsd	%zmm4, %zmm7, %zmm16{%k1}
	vpshufd	$177, %zmm3, %zmm4
	vpmaxsd	%zmm3, %zmm4, %zmm15
	vpminsd	%zmm3, %zmm4, %zmm15{%k1}
	vpshufd	$177, %zmm1, %zmm3
	vpshufd	$177, %zmm9, %zmm4
	vpmaxsd	%zmm1, %zmm3, %zmm12
	vpminsd	%zmm1, %zmm3, %zmm12{%k1}
	vpshufd	$177, %zmm0, %zmm1
	vpshufd	$177, %zmm2, %zmm3
	vpmaxsd	%zmm0, %zmm1, %zmm10
	vpmaxsd	%zmm2, %zmm3, %zmm7
	vpminsd	%zmm0, %zmm1, %zmm10{%k1}
	vpshufd	$177, %zmm8, %zmm1
	vpshufd	$177, %zmm6, %zmm0
	vpminsd	%zmm2, %zmm3, %zmm7{%k1}
	vpmaxsd	%zmm8, %zmm1, %zmm2
	vpminsd	%zmm8, %zmm1, %zmm2{%k1}
	vpmaxsd	%zmm6, %zmm0, %zmm8
	vpshufd	$177, %zmm20, %zmm1
	vpminsd	%zmm6, %zmm0, %zmm8{%k1}
	vpshufd	$177, %zmm21, %zmm0
	vpmaxsd	%zmm20, %zmm1, %zmm3
	vpmaxsd	%zmm21, %zmm0, %zmm6
	vpminsd	%zmm20, %zmm1, %zmm3{%k1}
	vpminsd	%zmm21, %zmm0, %zmm6{%k1}
	vpshufd	$177, %zmm19, %zmm0
	vpmaxsd	%zmm19, %zmm0, %zmm1
	vpminsd	%zmm19, %zmm0, %zmm1{%k1}
	vpmaxsd	%zmm9, %zmm4, %zmm0
	vpminsd	%zmm9, %zmm4, %zmm0{%k1}
.L160:
	vmovq	%xmm26, %rax
	vmovdqu64	%zmm5, (%rdi)
	vmovdqu64	%zmm13, (%r15)
	vmovdqu64	%zmm18, (%r14)
	vmovdqu64	%zmm17, 0(%r13)
	vmovdqu64	%zmm14, (%r12)
	vmovdqu64	%zmm16, (%rbx)
	vmovdqu64	%zmm15, (%r11)
	vmovdqu64	%zmm12, (%r10)
	vmovdqu64	%zmm7, (%r9)
	vmovdqu64	%zmm10, (%r8)
	vmovdqu64	%zmm2, (%rax)
	vmovq	%xmm25, %rax
	vmovdqu64	%zmm8, (%rax)
	vmovq	%xmm27, %rax
	vmovdqu64	%zmm6, (%rax)
	vmovq	%xmm24, %rax
	vmovdqu64	%zmm3, (%rdx)
	vmovdqu64	%zmm1, (%rax)
	movq	(%rsp), %rax
	vmovdqu64	%zmm0, (%rax)
	vzeroupper
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE18786:
	.size	_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0, .-_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	.section	.text._ZN3hwy6N_AVX36detail22MaybePartitionTwoValueINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX36detail22MaybePartitionTwoValueINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, @function
_ZN3hwy6N_AVX36detail22MaybePartitionTwoValueINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0:
.LFB18787:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %r10
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	.cfi_offset 3, -24
	cmpq	$15, %rsi
	jbe	.L181
	vmovdqa32	%zmm0, %zmm2
	vmovdqa32	%zmm1, %zmm3
	movl	$16, %r8d
	xorl	%esi, %esi
	jmp	.L172
	.p2align 4,,10
	.p2align 3
.L167:
	vmovdqu64	%zmm0, (%rax)
	kmovw	%k0, %eax
	popcntq	%rax, %rax
	addq	%rax, %rsi
	leaq	16(%r8), %rax
	cmpq	%r10, %rax
	ja	.L192
	movq	%rax, %r8
.L172:
	vmovdqu32	-64(%rdi,%r8,4), %zmm4
	leaq	-16(%r8), %r9
	leaq	(%rdi,%rsi,4), %rax
	vpcmpd	$0, %zmm2, %zmm4, %k0
	vpcmpd	$0, %zmm3, %zmm4, %k1
	kmovw	%k0, %r11d
	kmovw	%k1, %ebx
	korw	%k1, %k0, %k1
	kortestw	%k1, %k1
	jc	.L167
	kmovw	%r11d, %k6
	kmovw	%ebx, %k5
	kxnorw	%k5, %k6, %k7
	kmovw	%k7, %eax
	tzcntl	%eax, %eax
	addq	%r9, %rax
	vpbroadcastd	(%rdi,%rax,4), %zmm0
	leaq	16(%rsi), %rax
	vmovdqa32	%zmm0, (%rdx)
	cmpq	%r9, %rax
	ja	.L168
	.p2align 4,,10
	.p2align 3
.L169:
	vmovdqu64	%zmm1, -64(%rdi,%rax,4)
	movq	%rax, %rsi
	addq	$16, %rax
	cmpq	%rax, %r9
	jnb	.L169
.L168:
	subq	%rsi, %r9
	leaq	(%rdi,%rsi,4), %rdx
	movl	$65535, %eax
	cmpq	$255, %r9
	jbe	.L193
.L170:
	kmovw	%eax, %k4
	xorl	%eax, %eax
	vmovdqu32	%zmm1, (%rdx){%k4}
.L165:
	movq	-8(%rbp), %rbx
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L193:
	.cfi_restore_state
	movq	$-1, %rax
	bzhi	%r9, %rax, %rax
	movzwl	%ax, %eax
	jmp	.L170
	.p2align 4,,10
	.p2align 3
.L192:
	movq	%r10, %r11
	leaq	(%rdi,%r8,4), %rbx
	leaq	(%rdi,%rsi,4), %r9
	movl	$65535, %eax
	subq	%r8, %r11
	kmovd	%eax, %k1
	cmpq	$255, %r11
	jbe	.L166
.L173:
	vmovdqu32	(%rbx), %zmm2{%k1}{z}
	knotw	%k1, %k3
	vmovdqu32	%zmm2, (%rcx){%k1}
	vmovdqa32	(%rcx), %zmm2
	vpcmpd	$0, %zmm0, %zmm2, %k0
	vpcmpd	$0, %zmm1, %zmm2, %k2
	kandw	%k1, %k0, %k0
	korw	%k2, %k0, %k2
	korw	%k3, %k2, %k2
	kortestw	%k2, %k2
	jnc	.L194
	kmovw	%k0, %edx
	popcntq	%rdx, %rdx
	addq	%rsi, %rdx
	vmovdqu32	%zmm0, (%r9){%k1}
	leaq	16(%rdx), %rax
	cmpq	%r10, %rax
	ja	.L178
	.p2align 4,,10
	.p2align 3
.L179:
	vmovdqu64	%zmm1, -64(%rdi,%rax,4)
	movq	%rax, %rdx
	addq	$16, %rax
	cmpq	%rax, %r10
	jnb	.L179
.L178:
	subq	%rdx, %r10
	leaq	(%rdi,%rdx,4), %rcx
	movl	$65535, %eax
	cmpq	$255, %r10
	ja	.L180
	movq	$-1, %rax
	bzhi	%r10, %rax, %rax
	movzwl	%ax, %eax
.L180:
	kmovw	%eax, %k5
	movl	$1, %eax
	vmovdqu32	%zmm1, (%rcx){%k5}
	movq	-8(%rbp), %rbx
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
.L181:
	.cfi_restore_state
	movq	%rsi, %r11
	movq	%rdi, %r9
	movq	%rdi, %rbx
	xorl	%esi, %esi
	xorl	%r8d, %r8d
.L166:
	movq	$-1, %rax
	bzhi	%r11, %rax, %rax
	movzwl	%ax, %eax
	kmovd	%eax, %k1
	jmp	.L173
.L194:
	knotw	%k2, %k3
	kmovw	%k3, %eax
	tzcntl	%eax, %eax
	addq	%r8, %rax
	vpbroadcastd	(%rdi,%rax,4), %zmm0
	leaq	16(%rsi), %rax
	vmovdqa32	%zmm0, (%rdx)
	cmpq	%r8, %rax
	ja	.L175
	.p2align 4,,10
	.p2align 3
.L176:
	vmovdqu64	%zmm1, -64(%rdi,%rax,4)
	movq	%rax, %rsi
	leaq	16(%rax), %rax
	cmpq	%rax, %r8
	jnb	.L176
	leaq	(%rdi,%rsi,4), %r9
.L175:
	subq	%rsi, %r8
	movl	$65535, %eax
	cmpq	$255, %r8
	ja	.L177
	movq	$-1, %rax
	bzhi	%r8, %rax, %rax
	movzwl	%ax, %eax
.L177:
	kmovw	%eax, %k6
	xorl	%eax, %eax
	vmovdqu32	%zmm1, (%r9){%k6}
	jmp	.L165
	.cfi_endproc
.LFE18787:
	.size	_ZN3hwy6N_AVX36detail22MaybePartitionTwoValueINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, .-_ZN3hwy6N_AVX36detail22MaybePartitionTwoValueINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	.section	.text._ZN3hwy6N_AVX36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0, @function
_ZN3hwy6N_AVX36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0:
.LFB18788:
	.cfi_startproc
	movq	%rsi, %r8
	movq	%rdx, %rcx
	cmpq	%rdx, %rsi
	jbe	.L205
	leaq	(%rdx,%rdx), %rdx
	leaq	1(%rcx), %r10
	leaq	1(%rdx), %rsi
	addq	$2, %rdx
	cmpq	%rsi, %r8
	jbe	.L205
	movl	(%rdi,%rcx,4), %r11d
	vpbroadcastd	%r11d, %xmm1
	jmp	.L198
	.p2align 4,,10
	.p2align 3
.L208:
	movq	%rsi, %rax
	cmpq	%rdx, %r8
	ja	.L206
.L200:
	cmpq	%rcx, %rax
	je	.L205
	leaq	(%rdi,%rax,4), %rdx
	movl	(%rdx), %ecx
	movl	%ecx, (%r9)
	movl	%r11d, (%rdx)
	cmpq	%rax, %r8
	jbe	.L207
	leaq	(%rax,%rax), %rdx
	leaq	1(%rax), %r10
	leaq	1(%rdx), %rsi
	addq	$2, %rdx
	cmpq	%r8, %rsi
	jnb	.L205
	movq	%rax, %rcx
.L198:
	vpbroadcastd	(%rdi,%rsi,4), %xmm0
	leaq	(%rdi,%rcx,4), %r9
	vpcmpd	$6, %xmm1, %xmm0, %k0
	kmovb	%k0, %eax
	testb	$1, %al
	jne	.L208
	cmpq	%rdx, %r8
	jbe	.L205
	vpbroadcastd	(%rdi,%r10,8), %xmm0
	vpcmpd	$6, %xmm1, %xmm0, %k1
	kmovb	%k1, %eax
	testb	$1, %al
	je	.L205
	movq	%rdx, %rax
	jmp	.L200
	.p2align 4,,10
	.p2align 3
.L205:
	ret
	.p2align 4,,10
	.p2align 3
.L206:
	vpbroadcastd	(%rdi,%r10,8), %xmm2
	vpcmpd	$6, %xmm0, %xmm2, %k2
	kmovb	%k2, %esi
	andl	$1, %esi
	cmovne	%rdx, %rax
	jmp	.L200
	.p2align 4,,10
	.p2align 3
.L207:
	ret
	.cfi_endproc
.LFE18788:
	.size	_ZN3hwy6N_AVX36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0, .-_ZN3hwy6N_AVX36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	.section	.text._ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0, @function
_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0:
.LFB18789:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	leaq	0(,%rsi,4), %rax
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	.cfi_offset 15, -24
	leaq	(%rdi,%rax), %r15
	pushq	%r14
	.cfi_offset 14, -32
	leaq	(%r15,%rax), %r14
	pushq	%r13
	.cfi_offset 13, -40
	leaq	(%r14,%rax), %r13
	pushq	%r12
	.cfi_offset 12, -48
	leaq	0(%r13,%rax), %r12
	pushq	%rbx
	.cfi_offset 3, -56
	leaq	(%r12,%rax), %rbx
	leaq	(%rbx,%rax), %r11
	leaq	(%r11,%rax), %r10
	leaq	(%r10,%rax), %r9
	andq	$-64, %rsp
	leaq	(%r9,%rax), %r8
	subq	$8, %rsp
	leaq	(%r8,%rax), %rdx
	leaq	(%rdx,%rax), %rcx
	vmovq	%rdx, %xmm26
	leaq	(%rcx,%rax), %rdx
	vmovq	%rcx, %xmm25
	vmovdqu32	(%rdi), %zmm7
	vpminsd	(%r15), %zmm7, %zmm15
	vmovq	%rdx, %xmm27
	addq	%rax, %rdx
	vpmaxsd	(%r15), %zmm7, %zmm0
	vmovdqu32	(%r14), %zmm7
	leaq	(%rdx,%rax), %rcx
	vpminsd	0(%r13), %zmm7, %zmm16
	addq	%rcx, %rax
	vmovq	%rcx, %xmm24
	vmovq	%xmm26, %rcx
	movq	%rax, (%rsp)
	vmovq	%xmm25, %rax
	vpmaxsd	0(%r13), %zmm7, %zmm10
	vpminsd	%zmm16, %zmm15, %zmm12
	vmovdqu32	(%r12), %zmm7
	vpminsd	(%rbx), %zmm7, %zmm11
	vpmaxsd	%zmm16, %zmm15, %zmm15
	vpmaxsd	(%rbx), %zmm7, %zmm2
	vmovdqu32	(%r11), %zmm7
	vpminsd	%zmm10, %zmm0, %zmm16
	vpminsd	(%r10), %zmm7, %zmm8
	vpmaxsd	(%r10), %zmm7, %zmm6
	vmovdqu32	(%r9), %zmm7
	vpminsd	(%r8), %zmm7, %zmm1
	vpmaxsd	%zmm10, %zmm0, %zmm0
	vpmaxsd	(%r8), %zmm7, %zmm4
	vmovdqu32	(%rcx), %zmm7
	vpminsd	%zmm8, %zmm11, %zmm10
	vpminsd	(%rax), %zmm7, %zmm9
	vpmaxsd	(%rax), %zmm7, %zmm13
	vmovq	%xmm27, %rax
	vmovdqu32	(%rax), %zmm7
	movq	(%rsp), %rax
	vpmaxsd	%zmm8, %zmm11, %zmm11
	vpminsd	%zmm6, %zmm2, %zmm8
	vpminsd	(%rdx), %zmm7, %zmm3
	vpmaxsd	(%rdx), %zmm7, %zmm5
	vmovdqu64	(%rax), %zmm7
	vmovq	%xmm24, %rax
	vpmaxsd	%zmm6, %zmm2, %zmm2
	vpminsd	%zmm9, %zmm1, %zmm6
	vpmaxsd	%zmm9, %zmm1, %zmm1
	vpminsd	%zmm13, %zmm4, %zmm9
	vmovdqa64	%zmm7, -120(%rsp)
	vmovdqa32	-120(%rsp), %zmm7
	vpminsd	(%rax), %zmm7, %zmm14
	vpmaxsd	%zmm13, %zmm4, %zmm4
	vpmaxsd	(%rax), %zmm7, %zmm7
	vpminsd	%zmm14, %zmm3, %zmm13
	vpmaxsd	%zmm14, %zmm3, %zmm3
	vpminsd	%zmm7, %zmm5, %zmm14
	vpmaxsd	%zmm7, %zmm5, %zmm5
	vpminsd	%zmm10, %zmm12, %zmm7
	vpmaxsd	%zmm10, %zmm12, %zmm12
	vpminsd	%zmm8, %zmm16, %zmm10
	vpmaxsd	%zmm8, %zmm16, %zmm16
	vpminsd	%zmm11, %zmm15, %zmm8
	vpmaxsd	%zmm11, %zmm15, %zmm15
	vpminsd	%zmm2, %zmm0, %zmm11
	vpmaxsd	%zmm2, %zmm0, %zmm0
	vpminsd	%zmm13, %zmm6, %zmm2
	vpmaxsd	%zmm13, %zmm6, %zmm6
	vpminsd	%zmm14, %zmm9, %zmm13
	vpmaxsd	%zmm14, %zmm9, %zmm9
	vpminsd	%zmm3, %zmm1, %zmm14
	vpmaxsd	%zmm3, %zmm1, %zmm1
	vpminsd	%zmm5, %zmm4, %zmm3
	vpmaxsd	%zmm5, %zmm4, %zmm4
	vpminsd	%zmm2, %zmm7, %zmm5
	vpmaxsd	%zmm2, %zmm7, %zmm7
	vpminsd	%zmm13, %zmm10, %zmm2
	vpmaxsd	%zmm13, %zmm10, %zmm10
	vpminsd	%zmm14, %zmm8, %zmm13
	vpmaxsd	%zmm14, %zmm8, %zmm8
	vpminsd	%zmm3, %zmm11, %zmm14
	vpmaxsd	%zmm3, %zmm11, %zmm11
	vpminsd	%zmm6, %zmm12, %zmm3
	vpmaxsd	%zmm6, %zmm12, %zmm12
	vpminsd	%zmm9, %zmm16, %zmm6
	vpmaxsd	%zmm9, %zmm16, %zmm16
	vpminsd	%zmm1, %zmm15, %zmm9
	vpmaxsd	%zmm1, %zmm15, %zmm15
	vpminsd	%zmm4, %zmm0, %zmm1
	vpmaxsd	%zmm4, %zmm0, %zmm0
	vpminsd	%zmm8, %zmm6, %zmm4
	vpmaxsd	%zmm8, %zmm6, %zmm6
	vpminsd	%zmm10, %zmm9, %zmm8
	vpmaxsd	%zmm10, %zmm9, %zmm9
	vpminsd	%zmm12, %zmm14, %zmm10
	vpmaxsd	%zmm12, %zmm14, %zmm14
	vpminsd	%zmm11, %zmm1, %zmm12
	vpmaxsd	%zmm11, %zmm1, %zmm1
	vpminsd	%zmm15, %zmm16, %zmm11
	vpmaxsd	%zmm15, %zmm16, %zmm16
	vpminsd	%zmm7, %zmm3, %zmm15
	vpmaxsd	%zmm7, %zmm3, %zmm3
	vpminsd	%zmm13, %zmm2, %zmm7
	vpmaxsd	%zmm13, %zmm2, %zmm2
	vpminsd	%zmm15, %zmm7, %zmm13
	vpmaxsd	%zmm15, %zmm7, %zmm7
	vpminsd	%zmm11, %zmm12, %zmm15
	vpmaxsd	%zmm11, %zmm12, %zmm12
	vpminsd	%zmm3, %zmm2, %zmm11
	vpmaxsd	%zmm3, %zmm2, %zmm2
	vpminsd	%zmm16, %zmm1, %zmm3
	vpminsd	%zmm7, %zmm11, %zmm18
	vpmaxsd	%zmm7, %zmm11, %zmm11
	vpminsd	%zmm8, %zmm4, %zmm7
	vpmaxsd	%zmm8, %zmm4, %zmm4
	vpminsd	%zmm6, %zmm9, %zmm8
	vpmaxsd	%zmm6, %zmm9, %zmm9
	vpminsd	%zmm12, %zmm3, %zmm6
	vpmaxsd	%zmm12, %zmm3, %zmm3
	vpminsd	%zmm2, %zmm10, %zmm12
	vpmaxsd	%zmm2, %zmm10, %zmm10
	vpminsd	%zmm14, %zmm15, %zmm2
	vpmaxsd	%zmm14, %zmm15, %zmm15
	vpminsd	%zmm7, %zmm12, %zmm14
	vpmaxsd	%zmm7, %zmm12, %zmm12
	vpminsd	%zmm10, %zmm4, %zmm7
	vpmaxsd	%zmm10, %zmm4, %zmm4
	vpminsd	%zmm8, %zmm2, %zmm10
	vpmaxsd	%zmm8, %zmm2, %zmm2
	vpminsd	%zmm15, %zmm9, %zmm8
	vpmaxsd	%zmm16, %zmm1, %zmm1
	vpmaxsd	%zmm15, %zmm9, %zmm9
	vpminsd	%zmm7, %zmm12, %zmm16
	vpmaxsd	%zmm7, %zmm12, %zmm12
	vpminsd	%zmm4, %zmm10, %zmm7
	vpmaxsd	%zmm4, %zmm10, %zmm10
	vpminsd	%zmm8, %zmm2, %zmm4
	vpminsd	%zmm7, %zmm12, %zmm15
	vpminsd	%zmm11, %zmm14, %zmm17
	vpmaxsd	%zmm8, %zmm2, %zmm2
	vpmaxsd	%zmm7, %zmm12, %zmm12
	vpminsd	%zmm9, %zmm6, %zmm8
	vpminsd	%zmm4, %zmm10, %zmm7
	vpmaxsd	%zmm11, %zmm14, %zmm14
	vpmaxsd	%zmm9, %zmm6, %zmm6
	vpmaxsd	%zmm4, %zmm10, %zmm10
	cmpq	$1, %rsi
	jbe	.L211
	vpshufd	$177, %zmm2, %zmm2
	vpshufd	$177, %zmm0, %zmm0
	vpshufd	$177, %zmm8, %zmm8
	movl	$21845, %eax
	vpminsd	%zmm0, %zmm5, %zmm22
	vpshufd	$177, %zmm3, %zmm3
	vpmaxsd	%zmm0, %zmm5, %zmm9
	kmovw	%eax, %k1
	vpminsd	%zmm2, %zmm16, %zmm5
	vpshufd	$177, %zmm10, %zmm10
	vpminsd	%zmm3, %zmm18, %zmm20
	vpshufd	$177, %zmm7, %zmm7
	vpshufd	$177, %zmm1, %zmm1
	vpmaxsd	%zmm3, %zmm18, %zmm0
	vpminsd	%zmm8, %zmm14, %zmm11
	vpminsd	%zmm10, %zmm15, %zmm4
	vpshufd	$177, %zmm6, %zmm6
	vpshufd	$177, %zmm5, %zmm5
	vpminsd	%zmm1, %zmm13, %zmm21
	vpminsd	%zmm6, %zmm17, %zmm19
	vpmaxsd	%zmm2, %zmm16, %zmm16
	vpminsd	%zmm7, %zmm12, %zmm3
	vpshufd	$177, %zmm9, %zmm9
	vpminsd	%zmm5, %zmm20, %zmm2
	vpmaxsd	%zmm1, %zmm13, %zmm13
	vpshufd	$177, %zmm11, %zmm11
	vpmaxsd	%zmm7, %zmm12, %zmm1
	vpmaxsd	%zmm6, %zmm17, %zmm6
	vpshufd	$177, %zmm0, %zmm0
	vpshufd	$177, %zmm4, %zmm4
	vpminsd	%zmm9, %zmm1, %zmm18
	vpmaxsd	%zmm8, %zmm14, %zmm14
	vpminsd	%zmm4, %zmm21, %zmm12
	vpmaxsd	%zmm10, %zmm15, %zmm10
	vpshufd	$177, %zmm3, %zmm7
	vpshufd	$177, %zmm2, %zmm2
	vpminsd	%zmm11, %zmm19, %zmm3
	vpshufd	$177, %zmm6, %zmm6
	vpshufd	$177, %zmm13, %zmm13
	vpmaxsd	%zmm9, %zmm1, %zmm1
	vpmaxsd	%zmm4, %zmm21, %zmm4
	vpminsd	%zmm0, %zmm16, %zmm9
	vpminsd	%zmm7, %zmm22, %zmm17
	vpshufd	$177, %zmm4, %zmm4
	vpminsd	%zmm13, %zmm10, %zmm15
	vpmaxsd	%zmm5, %zmm20, %zmm5
	vpshufd	$177, %zmm3, %zmm3
	vpmaxsd	%zmm0, %zmm16, %zmm0
	vpminsd	%zmm6, %zmm14, %zmm8
	vpshufd	$177, %zmm9, %zmm16
	vpmaxsd	%zmm6, %zmm14, %zmm6
	vpminsd	%zmm2, %zmm12, %zmm20
	vpshufd	$177, %zmm1, %zmm1
	vpmaxsd	%zmm7, %zmm22, %zmm7
	vpmaxsd	%zmm13, %zmm10, %zmm13
	vpshufd	$177, %zmm8, %zmm8
	vpminsd	%zmm3, %zmm17, %zmm14
	vpmaxsd	%zmm11, %zmm19, %zmm11
	vpshufd	$177, %zmm13, %zmm13
	vpmaxsd	%zmm2, %zmm12, %zmm12
	vpminsd	%zmm1, %zmm6, %zmm19
	vpshufd	$177, %zmm20, %zmm2
	vpminsd	%zmm4, %zmm5, %zmm9
	vpshufd	$177, %zmm7, %zmm7
	vpmaxsd	%zmm4, %zmm5, %zmm5
	vpmaxsd	%zmm1, %zmm6, %zmm1
	vpminsd	%zmm16, %zmm15, %zmm4
	vpshufd	$177, %zmm9, %zmm9
	vpminsd	%zmm7, %zmm11, %zmm10
	vpshufd	$177, %zmm4, %zmm4
	vpshufd	$177, %zmm1, %zmm1
	vpmaxsd	%zmm3, %zmm17, %zmm3
	vpmaxsd	%zmm7, %zmm11, %zmm7
	vpminsd	%zmm8, %zmm18, %zmm17
	vpmaxsd	%zmm8, %zmm18, %zmm8
	vpshufd	$177, %zmm7, %zmm7
	vpminsd	%zmm13, %zmm0, %zmm18
	vpmaxsd	%zmm13, %zmm0, %zmm0
	vpshufd	$177, %zmm3, %zmm3
	vpminsd	%zmm2, %zmm14, %zmm13
	vpminsd	%zmm4, %zmm17, %zmm23
	vpshufd	$177, %zmm18, %zmm18
	vpminsd	%zmm1, %zmm0, %zmm20
	vpmaxsd	%zmm16, %zmm15, %zmm16
	vpshufd	$177, %zmm8, %zmm8
	vpminsd	%zmm9, %zmm10, %zmm15
	vpmaxsd	%zmm9, %zmm10, %zmm10
	vpmaxsd	%zmm4, %zmm17, %zmm9
	vpmaxsd	%zmm1, %zmm0, %zmm4
	vpshufd	$177, %zmm13, %zmm0
	vpmaxsd	%zmm2, %zmm14, %zmm11
	vpminsd	%zmm3, %zmm12, %zmm14
	vpmaxsd	%zmm7, %zmm5, %zmm2
	vpmaxsd	%zmm3, %zmm12, %zmm3
	vpminsd	%zmm7, %zmm5, %zmm12
	vpmaxsd	%zmm0, %zmm13, %zmm5
	vpminsd	%zmm0, %zmm13, %zmm5{%k1}
	vpshufd	$177, %zmm11, %zmm0
	vpminsd	%zmm18, %zmm19, %zmm21
	vpmaxsd	%zmm0, %zmm11, %zmm13
	vpmaxsd	%zmm18, %zmm19, %zmm19
	vpminsd	%zmm0, %zmm11, %zmm13{%k1}
	vpshufd	$177, %zmm14, %zmm0
	vpminsd	%zmm8, %zmm16, %zmm22
	vpmaxsd	%zmm0, %zmm14, %zmm18
	vpmaxsd	%zmm8, %zmm16, %zmm6
	vpshufd	$177, %zmm22, %zmm1
	vpminsd	%zmm0, %zmm14, %zmm18{%k1}
	vpshufd	$177, %zmm3, %zmm0
	vpmaxsd	%zmm0, %zmm3, %zmm17
	vpminsd	%zmm0, %zmm3, %zmm17{%k1}
	vpshufd	$177, %zmm15, %zmm0
	vpshufd	$177, %zmm4, %zmm3
	vpmaxsd	%zmm0, %zmm15, %zmm14
	vpminsd	%zmm0, %zmm15, %zmm14{%k1}
	vpshufd	$177, %zmm10, %zmm0
	vpmaxsd	%zmm0, %zmm10, %zmm16
	vpminsd	%zmm0, %zmm10, %zmm16{%k1}
	vpshufd	$177, %zmm12, %zmm0
	vpmaxsd	%zmm0, %zmm12, %zmm15
	vpminsd	%zmm0, %zmm12, %zmm15{%k1}
	vpshufd	$177, %zmm2, %zmm0
	vpmaxsd	%zmm0, %zmm2, %zmm12
	vpminsd	%zmm0, %zmm2, %zmm12{%k1}
	vpshufd	$177, %zmm23, %zmm0
	vpmaxsd	%zmm1, %zmm22, %zmm2
	vpmaxsd	%zmm0, %zmm23, %zmm7
	vpminsd	%zmm1, %zmm22, %zmm2{%k1}
	vpminsd	%zmm0, %zmm23, %zmm7{%k1}
	vpshufd	$177, %zmm9, %zmm0
	vpmaxsd	%zmm0, %zmm9, %zmm10
	vpminsd	%zmm0, %zmm9, %zmm10{%k1}
	vpshufd	$177, %zmm6, %zmm0
	vpmaxsd	%zmm0, %zmm6, %zmm8
	vpminsd	%zmm0, %zmm6, %zmm8{%k1}
	vpshufd	$177, %zmm21, %zmm0
	vpmaxsd	%zmm0, %zmm21, %zmm6
	vpminsd	%zmm0, %zmm21, %zmm6{%k1}
	vpshufd	$177, %zmm19, %zmm0
	vpmaxsd	%zmm0, %zmm19, %zmm9
	vpminsd	%zmm0, %zmm19, %zmm9{%k1}
	vpshufd	$177, %zmm20, %zmm0
	vpmaxsd	%zmm0, %zmm20, %zmm1
	vpminsd	%zmm0, %zmm20, %zmm1{%k1}
	vpmaxsd	%zmm3, %zmm4, %zmm0
	vpminsd	%zmm3, %zmm4, %zmm0{%k1}
	vmovdqa64	%zmm9, %zmm3
	cmpq	$3, %rsi
	jbe	.L211
	vpshufd	$27, %zmm2, %zmm2
	vpshufd	$27, %zmm8, %zmm8
	vpshufd	$27, %zmm9, %zmm9
	movl	$85, %eax
	vpminsd	%zmm9, %zmm18, %zmm20
	vpshufd	$27, %zmm7, %zmm7
	vpshufd	$27, %zmm10, %zmm10
	kmovb	%eax, %k2
	vpshufd	$27, %zmm6, %zmm6
	vpshufd	$27, %zmm1, %zmm1
	vpshufd	$27, %zmm0, %zmm0
	vpmaxsd	%zmm9, %zmm18, %zmm23
	vpmaxsd	%zmm8, %zmm14, %zmm18
	vpminsd	%zmm8, %zmm14, %zmm9
	vpminsd	%zmm2, %zmm16, %zmm8
	vpminsd	%zmm1, %zmm13, %zmm21
	vpminsd	%zmm6, %zmm17, %zmm19
	vpshufd	$27, %zmm8, %zmm8
	vpminsd	%zmm0, %zmm5, %zmm22
	vpmaxsd	%zmm0, %zmm5, %zmm4
	vpshufd	$27, %zmm9, %zmm9
	vpmaxsd	%zmm1, %zmm13, %zmm13
	vpmaxsd	%zmm6, %zmm17, %zmm11
	vpshufd	$27, %zmm4, %zmm4
	vpminsd	%zmm10, %zmm15, %zmm5
	vpminsd	%zmm7, %zmm12, %zmm6
	vpshufd	$27, %zmm11, %zmm11
	vpmaxsd	%zmm2, %zmm16, %zmm0
	vpmaxsd	%zmm10, %zmm15, %zmm3
	vpshufd	$27, %zmm13, %zmm2
	vpmaxsd	%zmm7, %zmm12, %zmm1
	vpshufd	$27, %zmm5, %zmm5
	vpshufd	$27, %zmm23, %zmm12
	vpminsd	%zmm8, %zmm20, %zmm7
	vpshufd	$27, %zmm6, %zmm6
	vpminsd	%zmm4, %zmm1, %zmm14
	vpmaxsd	%zmm8, %zmm20, %zmm16
	vpminsd	%zmm2, %zmm3, %zmm17
	vpshufd	$27, %zmm7, %zmm7
	vpminsd	%zmm12, %zmm0, %zmm10
	vpminsd	%zmm6, %zmm22, %zmm13
	vpminsd	%zmm9, %zmm19, %zmm8
	vpmaxsd	%zmm4, %zmm1, %zmm1
	vpmaxsd	%zmm2, %zmm3, %zmm3
	vpminsd	%zmm5, %zmm21, %zmm4
	vpshufd	$27, %zmm8, %zmm8
	vpmaxsd	%zmm5, %zmm21, %zmm5
	vpmaxsd	%zmm6, %zmm22, %zmm6
	vpshufd	$27, %zmm3, %zmm3
	vpminsd	%zmm7, %zmm4, %zmm20
	vpmaxsd	%zmm12, %zmm0, %zmm0
	vpshufd	$27, %zmm5, %zmm5
	vpmaxsd	%zmm9, %zmm19, %zmm9
	vpminsd	%zmm11, %zmm18, %zmm2
	vpshufd	$27, %zmm6, %zmm6
	vpmaxsd	%zmm11, %zmm18, %zmm11
	vpshufd	$27, %zmm1, %zmm1
	vpshufd	$27, %zmm10, %zmm18
	vpminsd	%zmm8, %zmm13, %zmm12
	vpminsd	%zmm6, %zmm9, %zmm15
	vpshufd	$27, %zmm2, %zmm2
	vpminsd	%zmm5, %zmm16, %zmm10
	vpmaxsd	%zmm8, %zmm13, %zmm13
	vpmaxsd	%zmm7, %zmm4, %zmm4
	vpminsd	%zmm18, %zmm17, %zmm19
	vpminsd	%zmm1, %zmm11, %zmm7
	vpmaxsd	%zmm18, %zmm17, %zmm17
	vpshufd	$27, %zmm19, %zmm19
	vpminsd	%zmm3, %zmm0, %zmm18
	vpmaxsd	%zmm6, %zmm9, %zmm9
	vpmaxsd	%zmm1, %zmm11, %zmm1
	vpmaxsd	%zmm5, %zmm16, %zmm6
	vpshufd	$27, %zmm20, %zmm5
	vpminsd	%zmm2, %zmm14, %zmm8
	vpshufd	$27, %zmm13, %zmm11
	vpmaxsd	%zmm3, %zmm0, %zmm0
	vpminsd	%zmm5, %zmm12, %zmm13
	vpshufd	$27, %zmm10, %zmm3
	vpmaxsd	%zmm5, %zmm12, %zmm5
	vpshufd	$27, %zmm9, %zmm9
	vpshufd	$27, %zmm18, %zmm18
	vpshufd	$27, %zmm1, %zmm1
	vpmaxsd	%zmm2, %zmm14, %zmm2
	vpminsd	%zmm19, %zmm8, %zmm16
	vpminsd	%zmm3, %zmm15, %zmm12
	vpminsd	%zmm9, %zmm6, %zmm10
	vpshufd	$27, %zmm2, %zmm2
	vpmaxsd	%zmm3, %zmm15, %zmm15
	vpminsd	%zmm18, %zmm7, %zmm21
	vpmaxsd	%zmm9, %zmm6, %zmm3
	vpmaxsd	%zmm19, %zmm8, %zmm8
	vpmaxsd	%zmm18, %zmm7, %zmm9
	vpminsd	%zmm1, %zmm0, %zmm19
	vpshufd	$27, %zmm5, %zmm7
	vpmaxsd	%zmm1, %zmm0, %zmm0
	vpshufd	$27, %zmm13, %zmm1
	vpminsd	%zmm11, %zmm4, %zmm14
	vpmaxsd	%zmm2, %zmm17, %zmm6
	vpmaxsd	%zmm11, %zmm4, %zmm4
	vpminsd	%zmm2, %zmm17, %zmm11
	vpminsd	%zmm1, %zmm13, %zmm2
	vpmaxsd	%zmm1, %zmm13, %zmm13
	vpminsd	%zmm7, %zmm5, %zmm1
	vpmaxsd	%zmm7, %zmm5, %zmm5
	vmovdqa64	%zmm2, %zmm13{%k2}
	vpblendmq	%zmm1, %zmm5, %zmm7{%k2}
	vpshufd	$27, %zmm14, %zmm1
	vpminsd	%zmm1, %zmm14, %zmm2
	vpmaxsd	%zmm1, %zmm14, %zmm14
	vmovdqa64	%zmm2, %zmm14{%k2}
	vpshufd	$27, %zmm4, %zmm2
	vpminsd	%zmm2, %zmm4, %zmm1
	vpmaxsd	%zmm2, %zmm4, %zmm2
	vmovdqa64	%zmm1, %zmm2{%k2}
	vpshufd	$27, %zmm12, %zmm1
	vpminsd	%zmm1, %zmm12, %zmm4
	vpmaxsd	%zmm1, %zmm12, %zmm12
	vmovdqa64	%zmm4, %zmm12{%k2}
	vpshufd	$27, %zmm15, %zmm4
	vpminsd	%zmm4, %zmm15, %zmm1
	vpmaxsd	%zmm4, %zmm15, %zmm4
	vmovdqa64	%zmm1, %zmm4{%k2}
	vpshufd	$27, %zmm10, %zmm1
	vpminsd	%zmm1, %zmm10, %zmm5
	vpmaxsd	%zmm1, %zmm10, %zmm10
	vpshufd	$27, %zmm3, %zmm1
	vmovdqa64	%zmm5, %zmm10{%k2}
	vpminsd	%zmm1, %zmm3, %zmm5
	vpmaxsd	%zmm1, %zmm3, %zmm1
	vpshufd	$27, %zmm16, %zmm3
	vmovdqa64	%zmm5, %zmm1{%k2}
	vpminsd	%zmm3, %zmm16, %zmm5
	vpmaxsd	%zmm3, %zmm16, %zmm3
	vmovdqa64	%zmm5, %zmm3{%k2}
	vpshufd	$27, %zmm8, %zmm5
	vpminsd	%zmm5, %zmm8, %zmm15
	vpmaxsd	%zmm5, %zmm8, %zmm8
	vpshufd	$27, %zmm11, %zmm5
	vmovdqa64	%zmm15, %zmm8{%k2}
	vpminsd	%zmm5, %zmm11, %zmm15
	vpmaxsd	%zmm5, %zmm11, %zmm11
	vpshufd	$27, %zmm6, %zmm5
	vmovdqa64	%zmm15, %zmm11{%k2}
	vpminsd	%zmm5, %zmm6, %zmm15
	vpmaxsd	%zmm5, %zmm6, %zmm6
	vpshufd	$27, %zmm21, %zmm5
	vmovdqa64	%zmm15, %zmm6{%k2}
	vpminsd	%zmm5, %zmm21, %zmm15
	vpmaxsd	%zmm5, %zmm21, %zmm21
	vpshufd	$27, %zmm9, %zmm5
	vmovdqa64	%zmm15, %zmm21{%k2}
	vpminsd	%zmm5, %zmm9, %zmm15
	vpmaxsd	%zmm5, %zmm9, %zmm9
	vpshufd	$27, %zmm19, %zmm5
	vmovdqa64	%zmm15, %zmm9{%k2}
	vpminsd	%zmm5, %zmm19, %zmm15
	vpmaxsd	%zmm5, %zmm19, %zmm19
	vpshufd	$27, %zmm0, %zmm5
	vmovdqa64	%zmm15, %zmm19{%k2}
	vpminsd	%zmm5, %zmm0, %zmm20
	vpmaxsd	%zmm5, %zmm0, %zmm0
	vpblendmq	%zmm20, %zmm0, %zmm20{%k2}
	vpshufd	$177, %zmm13, %zmm0
	vpmaxsd	%zmm13, %zmm0, %zmm5
	vpminsd	%zmm13, %zmm0, %zmm5{%k1}
	vpshufd	$177, %zmm7, %zmm0
	vpmaxsd	%zmm7, %zmm0, %zmm13
	vpminsd	%zmm7, %zmm0, %zmm13{%k1}
	vpshufd	$177, %zmm14, %zmm0
	vpmaxsd	%zmm14, %zmm0, %zmm18
	vpminsd	%zmm14, %zmm0, %zmm18{%k1}
	vpshufd	$177, %zmm2, %zmm0
	vpmaxsd	%zmm2, %zmm0, %zmm17
	vpminsd	%zmm2, %zmm0, %zmm17{%k1}
	vpshufd	$177, %zmm12, %zmm0
	vpmaxsd	%zmm12, %zmm0, %zmm14
	vpminsd	%zmm12, %zmm0, %zmm14{%k1}
	vpshufd	$177, %zmm4, %zmm0
	vpmaxsd	%zmm4, %zmm0, %zmm16
	vpminsd	%zmm4, %zmm0, %zmm16{%k1}
	vpshufd	$177, %zmm10, %zmm0
	vpshufd	$177, %zmm20, %zmm4
	vpmaxsd	%zmm10, %zmm0, %zmm15
	vpminsd	%zmm10, %zmm0, %zmm15{%k1}
	vpshufd	$177, %zmm1, %zmm0
	vpmaxsd	%zmm1, %zmm0, %zmm12
	vpminsd	%zmm1, %zmm0, %zmm12{%k1}
	vpshufd	$177, %zmm3, %zmm0
	vpshufd	$177, %zmm11, %zmm1
	vpmaxsd	%zmm3, %zmm0, %zmm7
	vpmaxsd	%zmm11, %zmm1, %zmm2
	vpminsd	%zmm3, %zmm0, %zmm7{%k1}
	vpshufd	$177, %zmm8, %zmm0
	vpminsd	%zmm11, %zmm1, %zmm2{%k1}
	vpmaxsd	%zmm8, %zmm0, %zmm10
	vpshufd	$177, %zmm9, %zmm1
	vpminsd	%zmm8, %zmm0, %zmm10{%k1}
	vpshufd	$177, %zmm6, %zmm0
	vpmaxsd	%zmm9, %zmm1, %zmm3
	vpmaxsd	%zmm6, %zmm0, %zmm8
	vpminsd	%zmm9, %zmm1, %zmm3{%k1}
	vpminsd	%zmm6, %zmm0, %zmm8{%k1}
	vpshufd	$177, %zmm21, %zmm0
	vpmaxsd	%zmm21, %zmm0, %zmm6
	vpminsd	%zmm21, %zmm0, %zmm6{%k1}
	vpshufd	$177, %zmm19, %zmm0
	vpmaxsd	%zmm19, %zmm0, %zmm1
	vpminsd	%zmm19, %zmm0, %zmm1{%k1}
	vpmaxsd	%zmm20, %zmm4, %zmm0
	vpminsd	%zmm20, %zmm4, %zmm0{%k1}
	cmpq	$7, %rsi
	jbe	.L211
	vmovdqa32	.LC1(%rip), %zmm9
	movl	$51, %eax
	kmovb	%eax, %k3
	vpermd	%zmm2, %zmm9, %zmm2
	vpermd	%zmm1, %zmm9, %zmm1
	vpermd	%zmm7, %zmm9, %zmm7
	vpminsd	%zmm1, %zmm13, %zmm21
	vpermd	%zmm10, %zmm9, %zmm10
	vpermd	%zmm8, %zmm9, %zmm8
	vpermd	%zmm6, %zmm9, %zmm6
	vpermd	%zmm3, %zmm9, %zmm3
	vpermd	%zmm0, %zmm9, %zmm0
	vpminsd	%zmm2, %zmm16, %zmm11
	vpmaxsd	%zmm1, %zmm13, %zmm1
	vpminsd	%zmm3, %zmm18, %zmm20
	vpminsd	%zmm6, %zmm17, %zmm19
	vpermd	%zmm11, %zmm9, %zmm11
	vpminsd	%zmm0, %zmm5, %zmm22
	vpminsd	%zmm7, %zmm12, %zmm4
	vpermd	%zmm1, %zmm9, %zmm1
	vpmaxsd	%zmm6, %zmm17, %zmm6
	vpmaxsd	%zmm0, %zmm5, %zmm0
	vpminsd	%zmm8, %zmm14, %zmm17
	vpminsd	%zmm10, %zmm15, %zmm5
	vpermd	%zmm0, %zmm9, %zmm0
	vpmaxsd	%zmm10, %zmm15, %zmm10
	vpmaxsd	%zmm3, %zmm18, %zmm3
	vpermd	%zmm17, %zmm9, %zmm17
	vpminsd	%zmm1, %zmm10, %zmm13
	vpmaxsd	%zmm8, %zmm14, %zmm8
	vpermd	%zmm3, %zmm9, %zmm3
	vpmaxsd	%zmm2, %zmm16, %zmm2
	vpmaxsd	%zmm7, %zmm12, %zmm7
	vpermd	%zmm5, %zmm9, %zmm5
	vpermd	%zmm6, %zmm9, %zmm12
	vpermd	%zmm4, %zmm9, %zmm6
	vpmaxsd	%zmm1, %zmm10, %zmm4
	vpminsd	%zmm11, %zmm20, %zmm10
	vpminsd	%zmm5, %zmm21, %zmm18
	vpermd	%zmm4, %zmm9, %zmm4
	vpminsd	%zmm0, %zmm7, %zmm16
	vpmaxsd	%zmm11, %zmm20, %zmm1
	vpminsd	%zmm6, %zmm22, %zmm14
	vpminsd	%zmm3, %zmm2, %zmm15
	vpminsd	%zmm17, %zmm19, %zmm11
	vpmaxsd	%zmm0, %zmm7, %zmm7
	vpermd	%zmm15, %zmm9, %zmm15
	vpmaxsd	%zmm3, %zmm2, %zmm0
	vpmaxsd	%zmm5, %zmm21, %zmm5
	vpermd	%zmm11, %zmm9, %zmm11
	vpmaxsd	%zmm17, %zmm19, %zmm3
	vpmaxsd	%zmm12, %zmm8, %zmm2
	vpermd	%zmm5, %zmm9, %zmm5
	vpminsd	%zmm12, %zmm8, %zmm17
	vpmaxsd	%zmm6, %zmm22, %zmm6
	vpermd	%zmm10, %zmm9, %zmm8
	vpermd	%zmm6, %zmm9, %zmm6
	vpermd	%zmm17, %zmm9, %zmm17
	vpermd	%zmm7, %zmm9, %zmm7
	vpminsd	%zmm8, %zmm18, %zmm19
	vpminsd	%zmm11, %zmm14, %zmm12
	vpmaxsd	%zmm8, %zmm18, %zmm10
	vpmaxsd	%zmm11, %zmm14, %zmm14
	vpminsd	%zmm6, %zmm3, %zmm8
	vpmaxsd	%zmm15, %zmm13, %zmm11
	vpminsd	%zmm5, %zmm1, %zmm18
	vpmaxsd	%zmm6, %zmm3, %zmm3
	vpmaxsd	%zmm5, %zmm1, %zmm1
	vpminsd	%zmm17, %zmm16, %zmm6
	vpermd	%zmm19, %zmm9, %zmm5
	vpmaxsd	%zmm17, %zmm16, %zmm16
	vpminsd	%zmm15, %zmm13, %zmm17
	vpermd	%zmm18, %zmm9, %zmm18
	vpminsd	%zmm7, %zmm2, %zmm13
	vpmaxsd	%zmm7, %zmm2, %zmm2
	vpermd	%zmm17, %zmm9, %zmm17
	vpermd	%zmm2, %zmm9, %zmm2
	vpminsd	%zmm4, %zmm0, %zmm15
	vpmaxsd	%zmm4, %zmm0, %zmm0
	vpermd	%zmm14, %zmm9, %zmm4
	vpminsd	%zmm5, %zmm12, %zmm14
	vpminsd	%zmm17, %zmm6, %zmm20
	vpermd	%zmm3, %zmm9, %zmm3
	vpermd	%zmm15, %zmm9, %zmm15
	vpmaxsd	%zmm5, %zmm12, %zmm5
	vpmaxsd	%zmm17, %zmm6, %zmm6
	vpminsd	%zmm2, %zmm0, %zmm17
	vpermd	%zmm16, %zmm9, %zmm16
	vpmaxsd	%zmm2, %zmm0, %zmm0
	vpermd	%zmm14, %zmm9, %zmm2
	vpminsd	%zmm4, %zmm10, %zmm12
	vpminsd	%zmm18, %zmm8, %zmm7
	vpmaxsd	%zmm4, %zmm10, %zmm4
	vpmaxsd	%zmm18, %zmm8, %zmm8
	vpminsd	%zmm3, %zmm1, %zmm10
	vpminsd	%zmm15, %zmm13, %zmm18
	vpmaxsd	%zmm3, %zmm1, %zmm1
	vpmaxsd	%zmm15, %zmm13, %zmm3
	vpminsd	%zmm2, %zmm14, %zmm13
	vpmaxsd	%zmm2, %zmm14, %zmm14
	vpermd	%zmm5, %zmm9, %zmm2
	vpminsd	%zmm16, %zmm11, %zmm19
	vmovdqa64	%zmm13, %zmm14{%k3}
	vpminsd	%zmm2, %zmm5, %zmm13
	vpmaxsd	%zmm2, %zmm5, %zmm5
	vpermd	%zmm12, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm5{%k3}
	vpmaxsd	%zmm16, %zmm11, %zmm11
	vpminsd	%zmm2, %zmm12, %zmm13
	vpmaxsd	%zmm2, %zmm12, %zmm12
	vpermd	%zmm4, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm12{%k3}
	vpminsd	%zmm2, %zmm4, %zmm13
	vpmaxsd	%zmm2, %zmm4, %zmm4
	vpermd	%zmm7, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm4{%k3}
	vpshufd	$78, %zmm5, %zmm16
	vpminsd	%zmm2, %zmm7, %zmm13
	vpmaxsd	%zmm2, %zmm7, %zmm7
	vpermd	%zmm8, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm7{%k3}
	vpminsd	%zmm2, %zmm8, %zmm13
	vpmaxsd	%zmm2, %zmm8, %zmm8
	vpermd	%zmm10, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm8{%k3}
	vpshufd	$78, %zmm12, %zmm15
	vpminsd	%zmm2, %zmm10, %zmm13
	vpmaxsd	%zmm2, %zmm10, %zmm10
	vpermd	%zmm1, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm10{%k3}
	vpminsd	%zmm2, %zmm1, %zmm13
	vpmaxsd	%zmm2, %zmm1, %zmm1
	vpermd	%zmm20, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm1{%k3}
	vpminsd	%zmm2, %zmm20, %zmm13
	vpmaxsd	%zmm2, %zmm20, %zmm20
	vpermd	%zmm6, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm20{%k3}
	vpminsd	%zmm2, %zmm6, %zmm13
	vpmaxsd	%zmm2, %zmm6, %zmm6
	vpermd	%zmm19, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm6{%k3}
	vpminsd	%zmm2, %zmm19, %zmm13
	vpmaxsd	%zmm2, %zmm19, %zmm19
	vpermd	%zmm11, %zmm9, %zmm2
	vmovdqa64	%zmm13, %zmm19{%k3}
	vpminsd	%zmm2, %zmm11, %zmm13
	vpmaxsd	%zmm2, %zmm11, %zmm2
	vpermd	%zmm18, %zmm9, %zmm11
	vmovdqa64	%zmm13, %zmm2{%k3}
	vpminsd	%zmm11, %zmm18, %zmm13
	vpmaxsd	%zmm11, %zmm18, %zmm18
	vpermd	%zmm3, %zmm9, %zmm11
	vmovdqa64	%zmm13, %zmm18{%k3}
	vpminsd	%zmm11, %zmm3, %zmm13
	vpmaxsd	%zmm11, %zmm3, %zmm3
	vpermd	%zmm17, %zmm9, %zmm11
	vmovdqa64	%zmm13, %zmm3{%k3}
	vpermd	%zmm0, %zmm9, %zmm9
	vpminsd	%zmm11, %zmm17, %zmm13
	vpmaxsd	%zmm11, %zmm17, %zmm17
	vpshufd	$78, %zmm2, %zmm21
	vmovdqa64	%zmm13, %zmm17{%k3}
	vpshufd	$78, %zmm14, %zmm13
	vpminsd	%zmm9, %zmm0, %zmm11
	vpmaxsd	%zmm9, %zmm0, %zmm0
	vpminsd	%zmm14, %zmm13, %zmm9
	vpmaxsd	%zmm14, %zmm13, %zmm13
	vpshufd	$78, %zmm4, %zmm14
	vmovdqa64	%zmm11, %zmm0{%k3}
	vmovdqa64	%zmm9, %zmm13{%k2}
	vpminsd	%zmm5, %zmm16, %zmm9
	vpmaxsd	%zmm5, %zmm16, %zmm16
	vpminsd	%zmm12, %zmm15, %zmm5
	vpmaxsd	%zmm12, %zmm15, %zmm15
	vpshufd	$78, %zmm7, %zmm12
	vmovdqa64	%zmm5, %zmm15{%k2}
	vpshufd	$78, %zmm8, %zmm11
	vpminsd	%zmm4, %zmm14, %zmm5
	vpmaxsd	%zmm4, %zmm14, %zmm14
	vpminsd	%zmm7, %zmm12, %zmm4
	vmovdqa64	%zmm9, %zmm16{%k2}
	vpmaxsd	%zmm7, %zmm12, %zmm12
	vpshufd	$78, %zmm10, %zmm7
	vmovdqa64	%zmm5, %zmm14{%k2}
	vmovdqa64	%zmm4, %zmm12{%k2}
	vpminsd	%zmm8, %zmm11, %zmm4
	vpmaxsd	%zmm8, %zmm11, %zmm11
	vmovdqa64	%zmm4, %zmm11{%k2}
	vpminsd	%zmm10, %zmm7, %zmm4
	vpmaxsd	%zmm10, %zmm7, %zmm7
	vmovdqa64	%zmm4, %zmm7{%k2}
	vpshufd	$78, %zmm20, %zmm10
	vpshufd	$78, %zmm1, %zmm4
	vpminsd	%zmm1, %zmm4, %zmm5
	vpshufd	$78, %zmm6, %zmm8
	vpmaxsd	%zmm1, %zmm4, %zmm1
	vpminsd	%zmm20, %zmm10, %zmm4
	vpmaxsd	%zmm20, %zmm10, %zmm10
	vpshufd	$78, %zmm18, %zmm20
	vmovdqa64	%zmm4, %zmm10{%k2}
	vpminsd	%zmm6, %zmm8, %zmm4
	vpmaxsd	%zmm6, %zmm8, %zmm8
	vpshufd	$78, %zmm19, %zmm6
	vmovdqa64	%zmm4, %zmm8{%k2}
	vpshufd	$78, %zmm0, %zmm9
	vpminsd	%zmm19, %zmm6, %zmm4
	vpmaxsd	%zmm19, %zmm6, %zmm6
	vpshufd	$78, %zmm3, %zmm19
	vmovdqa64	%zmm4, %zmm6{%k2}
	vpminsd	%zmm2, %zmm21, %zmm4
	vpmaxsd	%zmm2, %zmm21, %zmm21
	vpminsd	%zmm18, %zmm20, %zmm2
	vpmaxsd	%zmm18, %zmm20, %zmm20
	vmovdqa64	%zmm4, %zmm21{%k2}
	vmovdqa64	%zmm2, %zmm20{%k2}
	vpshufd	$78, %zmm17, %zmm4
	vpminsd	%zmm3, %zmm19, %zmm2
	vpmaxsd	%zmm3, %zmm19, %zmm19
	vmovdqa64	%zmm5, %zmm1{%k2}
	vmovdqa64	%zmm2, %zmm19{%k2}
	vpminsd	%zmm17, %zmm4, %zmm2
	vpmaxsd	%zmm17, %zmm4, %zmm4
	vmovdqa64	%zmm2, %zmm4{%k2}
	vpminsd	%zmm0, %zmm9, %zmm2
	vpmaxsd	%zmm0, %zmm9, %zmm9
	vpshufd	$177, %zmm13, %zmm0
	vmovdqa64	%zmm2, %zmm9{%k2}
	vpmaxsd	%zmm13, %zmm0, %zmm5
	vpminsd	%zmm13, %zmm0, %zmm5{%k1}
	vpshufd	$177, %zmm16, %zmm0
	vpmaxsd	%zmm16, %zmm0, %zmm13
	vpminsd	%zmm16, %zmm0, %zmm13{%k1}
	vpshufd	$177, %zmm15, %zmm0
	vpmaxsd	%zmm15, %zmm0, %zmm18
	vpminsd	%zmm15, %zmm0, %zmm18{%k1}
	vpshufd	$177, %zmm14, %zmm0
	vpmaxsd	%zmm14, %zmm0, %zmm17
	vpminsd	%zmm14, %zmm0, %zmm17{%k1}
	vpshufd	$177, %zmm12, %zmm0
	vpmaxsd	%zmm12, %zmm0, %zmm14
	vpminsd	%zmm12, %zmm0, %zmm14{%k1}
	vpshufd	$177, %zmm11, %zmm0
	vpmaxsd	%zmm11, %zmm0, %zmm16
	vpminsd	%zmm11, %zmm0, %zmm16{%k1}
	vpshufd	$177, %zmm7, %zmm0
	vpshufd	$177, %zmm9, %zmm11
	vpmaxsd	%zmm7, %zmm0, %zmm15
	vpminsd	%zmm7, %zmm0, %zmm15{%k1}
	vpshufd	$177, %zmm1, %zmm0
	vpmaxsd	%zmm1, %zmm0, %zmm12
	vpminsd	%zmm1, %zmm0, %zmm12{%k1}
	vpshufd	$177, %zmm10, %zmm0
	vpshufd	$177, %zmm19, %zmm1
	vpmaxsd	%zmm10, %zmm0, %zmm7
	vpmaxsd	%zmm19, %zmm1, %zmm3
	vpminsd	%zmm10, %zmm0, %zmm7{%k1}
	vpshufd	$177, %zmm8, %zmm0
	vpminsd	%zmm19, %zmm1, %zmm3{%k1}
	vpmaxsd	%zmm8, %zmm0, %zmm10
	vpminsd	%zmm8, %zmm0, %zmm10{%k1}
	vpshufd	$177, %zmm6, %zmm0
	vpmaxsd	%zmm6, %zmm0, %zmm2
	vpminsd	%zmm6, %zmm0, %zmm2{%k1}
	vpshufd	$177, %zmm21, %zmm0
	vpmaxsd	%zmm21, %zmm0, %zmm8
	vpminsd	%zmm21, %zmm0, %zmm8{%k1}
	vpshufd	$177, %zmm20, %zmm0
	vpmaxsd	%zmm20, %zmm0, %zmm6
	vpminsd	%zmm20, %zmm0, %zmm6{%k1}
	vpshufd	$177, %zmm4, %zmm0
	vpmaxsd	%zmm4, %zmm0, %zmm1
	vpminsd	%zmm4, %zmm0, %zmm1{%k1}
	vpmaxsd	%zmm9, %zmm11, %zmm0
	vmovdqa32	%zmm0, %zmm4
	vpminsd	%zmm9, %zmm11, %zmm4{%k1}
	vmovdqa64	%zmm4, %zmm0
	cmpq	$15, %rsi
	jbe	.L211
	vmovdqa32	.LC2(%rip), %zmm0
	movl	$65535, %eax
	kmovd	%eax, %k1
	movl	$51, %eax
	vpermd	%zmm6, %zmm0, %zmm11
	vpermd	%zmm10, %zmm0, %zmm10
	vpermd	%zmm4, %zmm0, %zmm6
	vpermd	%zmm2, %zmm0, %zmm2
	vpermd	%zmm1, %zmm0, %zmm1
	vpminsd	%zmm6, %zmm5, %zmm20
	vpminsd	%zmm1, %zmm13, %zmm19
	vpermd	%zmm8, %zmm0, %zmm8
	vpermd	%zmm7, %zmm0, %zmm7
	vpermd	%zmm3, %zmm0, %zmm3
	vpmaxsd	%zmm6, %zmm5, %zmm6
	vpmaxsd	%zmm1, %zmm13, %zmm1
	vpminsd	%zmm10, %zmm15, %zmm5
	vpminsd	%zmm2, %zmm16, %zmm13
	vpermd	%zmm6, %zmm0, %zmm6
	vpminsd	%zmm3, %zmm18, %zmm9
	vpermd	%zmm5, %zmm0, %zmm5
	vpminsd	%zmm8, %zmm14, %zmm4
	vpmaxsd	%zmm3, %zmm18, %zmm3
	vpminsd	%zmm11, %zmm17, %zmm18
	vpermd	%zmm1, %zmm0, %zmm1
	vpmaxsd	%zmm11, %zmm17, %zmm11
	vpmaxsd	%zmm8, %zmm14, %zmm17
	vpermd	%zmm3, %zmm0, %zmm3
	vpminsd	%zmm7, %zmm12, %zmm8
	vpmaxsd	%zmm7, %zmm12, %zmm7
	vpermd	%zmm13, %zmm0, %zmm12
	vpmaxsd	%zmm10, %zmm15, %zmm10
	vpminsd	%zmm6, %zmm7, %zmm13
	vpermd	%zmm11, %zmm0, %zmm15
	vpminsd	%zmm5, %zmm19, %zmm14
	vpmaxsd	%zmm2, %zmm16, %zmm2
	vpermd	%zmm4, %zmm0, %zmm4
	vpermd	%zmm8, %zmm0, %zmm8
	vpmaxsd	%zmm6, %zmm7, %zmm6
	vpmaxsd	%zmm5, %zmm19, %zmm7
	vpminsd	%zmm12, %zmm9, %zmm19
	vpminsd	%zmm8, %zmm20, %zmm16
	vpermd	%zmm7, %zmm0, %zmm7
	vpminsd	%zmm1, %zmm10, %zmm11
	vpermd	%zmm19, %zmm0, %zmm19
	vpmaxsd	%zmm1, %zmm10, %zmm5
	vpmaxsd	%zmm8, %zmm20, %zmm8
	vpmaxsd	%zmm12, %zmm9, %zmm1
	vpermd	%zmm5, %zmm0, %zmm5
	vpminsd	%zmm3, %zmm2, %zmm12
	vpminsd	%zmm4, %zmm18, %zmm9
	vpermd	%zmm8, %zmm0, %zmm8
	vpmaxsd	%zmm4, %zmm18, %zmm4
	vpminsd	%zmm15, %zmm17, %zmm18
	vpermd	%zmm9, %zmm0, %zmm9
	vpermd	%zmm12, %zmm0, %zmm12
	vpermd	%zmm18, %zmm0, %zmm18
	vpermd	%zmm6, %zmm0, %zmm6
	vpmaxsd	%zmm3, %zmm2, %zmm2
	vpmaxsd	%zmm15, %zmm17, %zmm3
	vpminsd	%zmm19, %zmm14, %zmm17
	vpminsd	%zmm9, %zmm16, %zmm15
	vpmaxsd	%zmm9, %zmm16, %zmm10
	vpmaxsd	%zmm19, %zmm14, %zmm14
	vpminsd	%zmm8, %zmm4, %zmm9
	vpminsd	%zmm7, %zmm1, %zmm16
	vpminsd	%zmm12, %zmm11, %zmm19
	vpmaxsd	%zmm7, %zmm1, %zmm1
	vpermd	%zmm16, %zmm0, %zmm16
	vpminsd	%zmm18, %zmm13, %zmm7
	vpmaxsd	%zmm12, %zmm11, %zmm11
	vpermd	%zmm19, %zmm0, %zmm19
	vpminsd	%zmm5, %zmm2, %zmm12
	vpmaxsd	%zmm8, %zmm4, %zmm4
	vpmaxsd	%zmm18, %zmm13, %zmm13
	vpminsd	%zmm6, %zmm3, %zmm8
	vpermd	%zmm4, %zmm0, %zmm4
	vpmaxsd	%zmm6, %zmm3, %zmm3
	vpermd	%zmm17, %zmm0, %zmm6
	vpermd	%zmm13, %zmm0, %zmm13
	vpermd	%zmm12, %zmm0, %zmm12
	vpermd	%zmm3, %zmm0, %zmm3
	vpmaxsd	%zmm5, %zmm2, %zmm2
	vpermd	%zmm10, %zmm0, %zmm5
	vpminsd	%zmm6, %zmm15, %zmm10
	vpminsd	%zmm16, %zmm9, %zmm17
	vpminsd	%zmm5, %zmm14, %zmm18
	vpmaxsd	%zmm6, %zmm15, %zmm6
	vpmaxsd	%zmm5, %zmm14, %zmm5
	vpmaxsd	%zmm16, %zmm9, %zmm9
	vpminsd	%zmm13, %zmm11, %zmm14
	vpminsd	%zmm4, %zmm1, %zmm16
	vpmaxsd	%zmm13, %zmm11, %zmm11
	vpmaxsd	%zmm4, %zmm1, %zmm1
	vpminsd	%zmm12, %zmm8, %zmm13
	vpmaxsd	%zmm12, %zmm8, %zmm4
	vpminsd	%zmm3, %zmm2, %zmm8
	vpmaxsd	%zmm3, %zmm2, %zmm2
	vpermd	%zmm10, %zmm0, %zmm3
	vpminsd	%zmm3, %zmm10, %zmm12
	vpmaxsd	%zmm3, %zmm10, %zmm10
	vpermd	%zmm6, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm10{%k1}
	vpminsd	%zmm3, %zmm6, %zmm12
	vpmaxsd	%zmm3, %zmm6, %zmm6
	vpermd	%zmm18, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm6{%k1}
	vpminsd	%zmm19, %zmm7, %zmm15
	vpminsd	%zmm3, %zmm18, %zmm12
	vpmaxsd	%zmm3, %zmm18, %zmm18
	vpermd	%zmm5, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm18{%k1}
	vpminsd	%zmm3, %zmm5, %zmm12
	vpmaxsd	%zmm3, %zmm5, %zmm5
	vpermd	%zmm17, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm5{%k1}
	vpmaxsd	%zmm19, %zmm7, %zmm7
	vpminsd	%zmm3, %zmm17, %zmm12
	vpmaxsd	%zmm3, %zmm17, %zmm17
	vpermd	%zmm9, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm17{%k1}
	vpminsd	%zmm3, %zmm9, %zmm12
	vpmaxsd	%zmm3, %zmm9, %zmm9
	vpermd	%zmm16, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm9{%k1}
	vshufi32x4	$177, %zmm5, %zmm5, %zmm22
	vpminsd	%zmm3, %zmm16, %zmm12
	vpmaxsd	%zmm3, %zmm16, %zmm16
	vpermd	%zmm1, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm16{%k1}
	vpminsd	%zmm3, %zmm1, %zmm12
	vpmaxsd	%zmm3, %zmm1, %zmm1
	vpermd	%zmm15, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm1{%k1}
	vshufi32x4	$177, %zmm17, %zmm17, %zmm21
	vpminsd	%zmm3, %zmm15, %zmm12
	vpmaxsd	%zmm3, %zmm15, %zmm15
	vpermd	%zmm7, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm15{%k1}
	vpminsd	%zmm3, %zmm7, %zmm12
	vpmaxsd	%zmm3, %zmm7, %zmm7
	vpermd	%zmm14, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm7{%k1}
	vshufi32x4	$177, %zmm9, %zmm9, %zmm20
	vpminsd	%zmm3, %zmm14, %zmm12
	vpmaxsd	%zmm3, %zmm14, %zmm14
	vpermd	%zmm11, %zmm0, %zmm3
	vmovdqu16	%zmm12, %zmm14{%k1}
	vpminsd	%zmm3, %zmm11, %zmm12
	vpmaxsd	%zmm3, %zmm11, %zmm3
	vpermd	%zmm13, %zmm0, %zmm11
	vmovdqu16	%zmm12, %zmm3{%k1}
	vshufi32x4	$177, %zmm16, %zmm16, %zmm19
	vpminsd	%zmm11, %zmm13, %zmm12
	vpmaxsd	%zmm11, %zmm13, %zmm13
	vpermd	%zmm4, %zmm0, %zmm11
	vmovdqu16	%zmm12, %zmm13{%k1}
	vpminsd	%zmm11, %zmm4, %zmm12
	vpmaxsd	%zmm11, %zmm4, %zmm4
	vpermd	%zmm8, %zmm0, %zmm11
	vmovdqu16	%zmm12, %zmm4{%k1}
	vpermd	%zmm2, %zmm0, %zmm0
	vpminsd	%zmm11, %zmm8, %zmm12
	vpmaxsd	%zmm11, %zmm8, %zmm8
	vmovdqu16	%zmm12, %zmm8{%k1}
	vpminsd	%zmm0, %zmm2, %zmm11
	vshufi32x4	$177, %zmm10, %zmm10, %zmm12
	vpmaxsd	%zmm0, %zmm2, %zmm0
	vpminsd	%zmm10, %zmm12, %zmm2
	vmovdqu16	%zmm11, %zmm0{%k1}
	vpmaxsd	%zmm10, %zmm12, %zmm12
	kmovb	%eax, %k1
	vshufi32x4	$177, %zmm6, %zmm6, %zmm11
	vmovdqa64	%zmm2, %zmm12{%k1}
	vpminsd	%zmm6, %zmm11, %zmm2
	vpmaxsd	%zmm6, %zmm11, %zmm11
	movl	$85, %eax
	vshufi32x4	$177, %zmm18, %zmm18, %zmm10
	vmovdqa64	%zmm2, %zmm11{%k1}
	vshufi32x4	$177, %zmm14, %zmm14, %zmm6
	vpminsd	%zmm18, %zmm10, %zmm2
	vpmaxsd	%zmm18, %zmm10, %zmm10
	vshufi32x4	$177, %zmm15, %zmm15, %zmm18
	vmovdqa64	%zmm2, %zmm10{%k1}
	vpminsd	%zmm5, %zmm22, %zmm2
	vpmaxsd	%zmm5, %zmm22, %zmm22
	vmovdqa64	%zmm2, %zmm22{%k1}
	vpminsd	%zmm17, %zmm21, %zmm2
	vpmaxsd	%zmm17, %zmm21, %zmm21
	vmovdqa64	%zmm2, %zmm21{%k1}
	vpminsd	%zmm9, %zmm20, %zmm2
	vpmaxsd	%zmm9, %zmm20, %zmm20
	vmovdqa64	%zmm2, %zmm20{%k1}
	vpminsd	%zmm16, %zmm19, %zmm2
	vpmaxsd	%zmm16, %zmm19, %zmm19
	vmovdqa64	%zmm2, %zmm19{%k1}
	vshufi32x4	$177, %zmm1, %zmm1, %zmm2
	vshufi32x4	$177, %zmm7, %zmm7, %zmm17
	vpminsd	%zmm1, %zmm2, %zmm5
	vpmaxsd	%zmm1, %zmm2, %zmm2
	vshufi32x4	$177, %zmm3, %zmm3, %zmm16
	vpminsd	%zmm15, %zmm18, %zmm1
	vpmaxsd	%zmm15, %zmm18, %zmm18
	vshufi32x4	$177, %zmm13, %zmm13, %zmm15
	vmovdqa64	%zmm1, %zmm18{%k1}
	vpminsd	%zmm7, %zmm17, %zmm1
	vpmaxsd	%zmm7, %zmm17, %zmm17
	vmovdqa64	%zmm1, %zmm17{%k1}
	vpminsd	%zmm14, %zmm6, %zmm1
	vpmaxsd	%zmm14, %zmm6, %zmm6
	vmovdqa64	%zmm1, %zmm6{%k1}
	vpminsd	%zmm3, %zmm16, %zmm1
	vpmaxsd	%zmm3, %zmm16, %zmm16
	vmovdqa64	%zmm1, %zmm16{%k1}
	vshufi32x4	$177, %zmm4, %zmm4, %zmm14
	vpminsd	%zmm13, %zmm15, %zmm1
	vpmaxsd	%zmm13, %zmm15, %zmm15
	vshufi32x4	$177, %zmm8, %zmm8, %zmm9
	vmovdqa64	%zmm5, %zmm2{%k1}
	vmovdqa64	%zmm1, %zmm15{%k1}
	vpminsd	%zmm4, %zmm14, %zmm1
	vpmaxsd	%zmm4, %zmm14, %zmm14
	vmovdqa64	%zmm1, %zmm14{%k1}
	vshufi32x4	$177, %zmm0, %zmm0, %zmm5
	vpminsd	%zmm8, %zmm9, %zmm1
	vpshufd	$78, %zmm12, %zmm13
	vpmaxsd	%zmm8, %zmm9, %zmm9
	vpshufd	$78, %zmm21, %zmm7
	vmovdqa64	%zmm1, %zmm9{%k1}
	vpminsd	%zmm0, %zmm5, %zmm1
	vpmaxsd	%zmm0, %zmm5, %zmm5
	vpminsd	%zmm12, %zmm13, %zmm0
	vpmaxsd	%zmm12, %zmm13, %zmm13
	vpshufd	$78, %zmm11, %zmm12
	vmovdqa64	%zmm1, %zmm5{%k1}
	kmovb	%eax, %k1
	vmovdqa64	%zmm0, %zmm13{%k1}
	vpminsd	%zmm11, %zmm12, %zmm0
	vpmaxsd	%zmm11, %zmm12, %zmm12
	vpshufd	$78, %zmm10, %zmm11
	vpshufd	$78, %zmm20, %zmm4
	movl	$21845, %eax
	vmovdqa64	%zmm0, %zmm12{%k1}
	vpminsd	%zmm10, %zmm11, %zmm0
	vpmaxsd	%zmm10, %zmm11, %zmm11
	vpshufd	$78, %zmm22, %zmm10
	vmovdqa64	%zmm0, %zmm11{%k1}
	vpshufd	$78, %zmm19, %zmm3
	vpminsd	%zmm22, %zmm10, %zmm0
	vpmaxsd	%zmm22, %zmm10, %zmm10
	vpshufd	$78, %zmm2, %zmm1
	vmovdqa64	%zmm0, %zmm10{%k1}
	vpminsd	%zmm21, %zmm7, %zmm0
	vpmaxsd	%zmm21, %zmm7, %zmm7
	vmovdqa64	%zmm0, %zmm7{%k1}
	vpminsd	%zmm20, %zmm4, %zmm0
	vpmaxsd	%zmm20, %zmm4, %zmm4
	vmovdqa64	%zmm0, %zmm4{%k1}
	vpminsd	%zmm19, %zmm3, %zmm0
	vpmaxsd	%zmm19, %zmm3, %zmm3
	vmovdqa64	%zmm0, %zmm3{%k1}
	vpminsd	%zmm2, %zmm1, %zmm0
	vpmaxsd	%zmm2, %zmm1, %zmm1
	vpshufd	$78, %zmm18, %zmm2
	vmovdqa64	%zmm0, %zmm1{%k1}
	vpshufd	$78, %zmm15, %zmm21
	vpminsd	%zmm18, %zmm2, %zmm0
	vpmaxsd	%zmm18, %zmm2, %zmm2
	vpshufd	$78, %zmm14, %zmm20
	vmovdqa64	%zmm0, %zmm2{%k1}
	vpshufd	$78, %zmm17, %zmm0
	vpshufd	$78, %zmm9, %zmm19
	vpminsd	%zmm17, %zmm0, %zmm8
	vpmaxsd	%zmm17, %zmm0, %zmm0
	vmovdqa64	%zmm8, %zmm0{%k1}
	vpshufd	$78, %zmm6, %zmm8
	vpminsd	%zmm6, %zmm8, %zmm17
	vpmaxsd	%zmm6, %zmm8, %zmm8
	vpshufd	$78, %zmm16, %zmm6
	vmovdqa64	%zmm17, %zmm8{%k1}
	vpminsd	%zmm16, %zmm6, %zmm17
	vpmaxsd	%zmm16, %zmm6, %zmm6
	vpminsd	%zmm15, %zmm21, %zmm16
	vpmaxsd	%zmm15, %zmm21, %zmm21
	vmovdqa64	%zmm17, %zmm6{%k1}
	vpminsd	%zmm14, %zmm20, %zmm15
	vpmaxsd	%zmm14, %zmm20, %zmm20
	vmovdqa64	%zmm16, %zmm21{%k1}
	vpminsd	%zmm9, %zmm19, %zmm14
	vpmaxsd	%zmm9, %zmm19, %zmm19
	vpshufd	$78, %zmm5, %zmm9
	vmovdqa64	%zmm14, %zmm19{%k1}
	vpminsd	%zmm5, %zmm9, %zmm14
	vpmaxsd	%zmm5, %zmm9, %zmm9
	vmovdqa64	%zmm14, %zmm9{%k1}
	vpshufd	$177, %zmm13, %zmm14
	vmovdqa64	%zmm15, %zmm20{%k1}
	kmovw	%eax, %k1
	vpmaxsd	%zmm13, %zmm14, %zmm5
	vpminsd	%zmm13, %zmm14, %zmm5{%k1}
	vpshufd	$177, %zmm12, %zmm14
	vpmaxsd	%zmm12, %zmm14, %zmm13
	vpminsd	%zmm12, %zmm14, %zmm13{%k1}
	vpshufd	$177, %zmm11, %zmm12
	vpmaxsd	%zmm11, %zmm12, %zmm18
	vpminsd	%zmm11, %zmm12, %zmm18{%k1}
	vpshufd	$177, %zmm10, %zmm11
	vpmaxsd	%zmm10, %zmm11, %zmm17
	vpminsd	%zmm10, %zmm11, %zmm17{%k1}
	vpshufd	$177, %zmm7, %zmm10
	vpmaxsd	%zmm7, %zmm10, %zmm14
	vpminsd	%zmm7, %zmm10, %zmm14{%k1}
	vpshufd	$177, %zmm4, %zmm7
	vpmaxsd	%zmm4, %zmm7, %zmm16
	vpminsd	%zmm4, %zmm7, %zmm16{%k1}
	vpshufd	$177, %zmm3, %zmm4
	vpmaxsd	%zmm3, %zmm4, %zmm15
	vpminsd	%zmm3, %zmm4, %zmm15{%k1}
	vpshufd	$177, %zmm1, %zmm3
	vpshufd	$177, %zmm9, %zmm4
	vpmaxsd	%zmm1, %zmm3, %zmm12
	vpminsd	%zmm1, %zmm3, %zmm12{%k1}
	vpshufd	$177, %zmm0, %zmm1
	vpshufd	$177, %zmm2, %zmm3
	vpmaxsd	%zmm0, %zmm1, %zmm10
	vpmaxsd	%zmm2, %zmm3, %zmm7
	vpminsd	%zmm0, %zmm1, %zmm10{%k1}
	vpshufd	$177, %zmm8, %zmm1
	vpshufd	$177, %zmm6, %zmm0
	vpminsd	%zmm2, %zmm3, %zmm7{%k1}
	vpmaxsd	%zmm8, %zmm1, %zmm2
	vpminsd	%zmm8, %zmm1, %zmm2{%k1}
	vpmaxsd	%zmm6, %zmm0, %zmm8
	vpshufd	$177, %zmm20, %zmm1
	vpminsd	%zmm6, %zmm0, %zmm8{%k1}
	vpshufd	$177, %zmm21, %zmm0
	vpmaxsd	%zmm20, %zmm1, %zmm3
	vpmaxsd	%zmm21, %zmm0, %zmm6
	vpminsd	%zmm20, %zmm1, %zmm3{%k1}
	vpminsd	%zmm21, %zmm0, %zmm6{%k1}
	vpshufd	$177, %zmm19, %zmm0
	vpmaxsd	%zmm19, %zmm0, %zmm1
	vpminsd	%zmm19, %zmm0, %zmm1{%k1}
	vpmaxsd	%zmm9, %zmm4, %zmm0
	vpminsd	%zmm9, %zmm4, %zmm0{%k1}
.L211:
	vmovq	%xmm26, %rax
	vmovdqu64	%zmm5, (%rdi)
	vmovdqu64	%zmm13, (%r15)
	vmovdqu64	%zmm18, (%r14)
	vmovdqu64	%zmm17, 0(%r13)
	vmovdqu64	%zmm14, (%r12)
	vmovdqu64	%zmm16, (%rbx)
	vmovdqu64	%zmm15, (%r11)
	vmovdqu64	%zmm12, (%r10)
	vmovdqu64	%zmm7, (%r9)
	vmovdqu64	%zmm10, (%r8)
	vmovdqu64	%zmm2, (%rax)
	vmovq	%xmm25, %rax
	vmovdqu64	%zmm8, (%rax)
	vmovq	%xmm27, %rax
	vmovdqu64	%zmm6, (%rax)
	vmovq	%xmm24, %rax
	vmovdqu64	%zmm3, (%rdx)
	vmovdqu64	%zmm1, (%rax)
	movq	(%rsp), %rax
	vmovdqu64	%zmm0, (%rax)
	vzeroupper
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE18789:
	.size	_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0, .-_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	.section	.text._ZN3hwy6N_AVX26detail22MaybePartitionTwoValueINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX26detail22MaybePartitionTwoValueINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, @function
_ZN3hwy6N_AVX26detail22MaybePartitionTwoValueINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0:
.LFB18790:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	vmovdqa	%ymm0, %ymm3
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	.cfi_offset 15, -24
	movq	%rdx, %r15
	pushq	%r14
	.cfi_offset 14, -32
	movq	%rsi, %r14
	pushq	%r13
	pushq	%r12
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	movq	%rdi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$96, %rsp
	.cfi_offset 3, -56
	cmpq	$7, %rsi
	jbe	.L231
	vmovdqa	%ymm0, %ymm5
	vmovdqa	%ymm1, %ymm6
	movl	$8, %r13d
	xorl	%ebx, %ebx
	jmp	.L222
	.p2align 4,,10
	.p2align 3
.L218:
	vmovmskps	%ymm2, %eax
	vmovdqu	%ymm3, (%rsi)
	popcntq	%rax, %rax
	addq	%rax, %rbx
	leaq	8(%r13), %rax
	cmpq	%r14, %rax
	ja	.L239
	movq	%rax, %r13
.L222:
	vpcmpeqd	-32(%r12,%r13,4), %ymm6, %ymm0
	vpcmpeqd	-32(%r12,%r13,4), %ymm5, %ymm2
	leaq	-8(%r13), %rdx
	leaq	(%r12,%rbx,4), %rsi
	vmovdqa	%ymm0, %ymm4
	vpor	%ymm2, %ymm0, %ymm0
	vmovmskps	%ymm0, %eax
	cmpl	$255, %eax
	je	.L218
	vpcmpeqd	%ymm0, %ymm0, %ymm0
	vpxor	%ymm0, %ymm4, %ymm4
	vpandn	%ymm4, %ymm2, %ymm2
	vmovmskps	%ymm2, %eax
	tzcntl	%eax, %eax
	addq	%rdx, %rax
	vpbroadcastd	(%r12,%rax,4), %ymm0
	leaq	8(%rbx), %rax
	vmovdqa	%ymm0, (%r15)
	cmpq	%rdx, %rax
	ja	.L219
	.p2align 4,,10
	.p2align 3
.L220:
	vmovdqu	%ymm1, -32(%r12,%rax,4)
	movq	%rax, %rbx
	addq	$8, %rax
	cmpq	%rax, %rdx
	jnb	.L220
.L219:
	subq	%rbx, %rdx
	xorl	%eax, %eax
	vmovd	%edx, %xmm0
	vpbroadcastd	%xmm0, %ymm0
	vpcmpgtd	.LC3(%rip), %ymm0, %ymm0
	vpmaskmovd	%ymm1, %ymm0, (%r12,%rbx,4)
.L216:
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L239:
	.cfi_restore_state
	movq	%r14, %r8
	leaq	0(,%r13,4), %rsi
	leaq	(%r12,%rbx,4), %r9
	subq	%r13, %r8
.L217:
	testq	%r8, %r8
	je	.L226
	leaq	0(,%r8,4), %rdx
	addq	%r12, %rsi
	movq	%rcx, %rdi
	movq	%r9, 80(%rsp)
	movq	%r8, 88(%rsp)
	vmovdqa	%ymm1, (%rsp)
	vmovdqa	%ymm3, 32(%rsp)
	vzeroupper
	call	memcpy@PLT
	movq	88(%rsp), %r8
	movq	80(%rsp), %r9
	vmovdqa	32(%rsp), %ymm3
	vmovdqa	(%rsp), %ymm1
	movq	%rax, %rcx
.L226:
	vmovdqa	(%rcx), %ymm4
	vmovdqa	.LC3(%rip), %ymm5
	vmovd	%r8d, %xmm2
	vpbroadcastd	%xmm2, %ymm2
	vpcmpeqd	%ymm3, %ymm4, %ymm0
	vpcmpgtd	%ymm5, %ymm2, %ymm2
	vpcmpeqd	%ymm1, %ymm4, %ymm4
	vpand	%ymm0, %ymm2, %ymm7
	vpor	%ymm4, %ymm0, %ymm0
	vpcmpeqd	%ymm4, %ymm4, %ymm4
	vpxor	%ymm2, %ymm4, %ymm6
	vpor	%ymm6, %ymm0, %ymm0
	vmovmskps	%ymm0, %eax
	cmpl	$255, %eax
	jne	.L240
	vmovmskps	%ymm7, %edx
	vpmaskmovd	%ymm3, %ymm2, (%r9)
	popcntq	%rdx, %rdx
	addq	%rbx, %rdx
	leaq	8(%rdx), %rax
	cmpq	%r14, %rax
	ja	.L229
	.p2align 4,,10
	.p2align 3
.L230:
	vmovdqu	%ymm1, -32(%r12,%rax,4)
	movq	%rax, %rdx
	addq	$8, %rax
	cmpq	%rax, %r14
	jnb	.L230
.L229:
	subq	%rdx, %r14
	movl	$1, %eax
	vmovd	%r14d, %xmm0
	vpbroadcastd	%xmm0, %ymm0
	vpcmpgtd	%ymm5, %ymm0, %ymm0
	vpmaskmovd	%ymm1, %ymm0, (%r12,%rdx,4)
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
.L231:
	.cfi_restore_state
	movq	%rsi, %r8
	movq	%rdi, %r9
	xorl	%esi, %esi
	xorl	%ebx, %ebx
	xorl	%r13d, %r13d
	jmp	.L217
.L240:
	vpxor	%ymm4, %ymm0, %ymm0
	vmovmskps	%ymm0, %eax
	tzcntl	%eax, %eax
	addq	%r13, %rax
	vpbroadcastd	(%r12,%rax,4), %ymm0
	leaq	8(%rbx), %rax
	vmovdqa	%ymm0, (%r15)
	cmpq	%rax, %r13
	jb	.L227
	.p2align 4,,10
	.p2align 3
.L228:
	vmovdqu	%ymm1, -32(%r12,%rax,4)
	movq	%rax, %rbx
	leaq	8(%rax), %rax
	cmpq	%rax, %r13
	jnb	.L228
	leaq	(%r12,%rbx,4), %r9
.L227:
	subq	%rbx, %r13
	xorl	%eax, %eax
	vmovd	%r13d, %xmm0
	vpbroadcastd	%xmm0, %ymm0
	vpcmpgtd	%ymm5, %ymm0, %ymm0
	vpmaskmovd	%ymm1, %ymm0, (%r9)
	jmp	.L216
	.cfi_endproc
.LFE18790:
	.size	_ZN3hwy6N_AVX26detail22MaybePartitionTwoValueINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, .-_ZN3hwy6N_AVX26detail22MaybePartitionTwoValueINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	.section	.text._ZN3hwy6N_AVX26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0, @function
_ZN3hwy6N_AVX26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0:
.LFB18791:
	.cfi_startproc
	cmpq	%rdx, %rsi
	jbe	.L254
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r10
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rax, %rsi
	jbe	.L254
	movl	(%rdi,%rdx,4), %r11d
	vmovd	%r11d, %xmm4
	vpshufd	$0, %xmm4, %xmm0
	jmp	.L244
	.p2align 4,,10
	.p2align 3
.L245:
	cmpq	%rcx, %rsi
	jbe	.L254
	movq	%rdx, %rax
.L250:
	vbroadcastss	(%rdi,%r10,8), %xmm1
	vpcmpgtd	%xmm3, %xmm1, %xmm1
	vmovmskps	%xmm1, %r8d
	andl	$1, %r8d
	jne	.L247
.L246:
	cmpq	%rdx, %rax
	je	.L254
	leaq	(%rdi,%rax,4), %rdx
	movl	(%rdx), %ecx
	movl	%ecx, (%r9)
	movl	%r11d, (%rdx)
	cmpq	%rax, %rsi
	jbe	.L255
	movq	%rax, %rdx
.L248:
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r10
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rsi, %rax
	jnb	.L254
.L244:
	vbroadcastss	(%rdi,%rax,4), %xmm1
	leaq	(%rdi,%rdx,4), %r9
	vmovdqa	%xmm0, %xmm3
	vpcmpgtd	%xmm0, %xmm1, %xmm2
	vmovmskps	%xmm2, %r8d
	andl	$1, %r8d
	je	.L245
	cmpq	%rcx, %rsi
	jbe	.L246
	vmovdqa	%xmm1, %xmm3
	jmp	.L250
	.p2align 4,,10
	.p2align 3
.L247:
	cmpq	%rcx, %rdx
	je	.L256
	leaq	(%rdi,%rcx,4), %rax
	movl	(%rax), %edx
	movl	%edx, (%r9)
	movq	%rcx, %rdx
	movl	%r11d, (%rax)
	jmp	.L248
	.p2align 4,,10
	.p2align 3
.L254:
	ret
	.p2align 4,,10
	.p2align 3
.L255:
	ret
.L256:
	ret
	.cfi_endproc
.LFE18791:
	.size	_ZN3hwy6N_AVX26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0, .-_ZN3hwy6N_AVX26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	.section	.text._ZN3hwy6N_SSE46detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE46detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, @function
_ZN3hwy6N_SSE46detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0:
.LFB18792:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movdqa	%xmm0, %xmm3
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	.cfi_offset 15, -24
	movq	%rdx, %r15
	pushq	%r14
	.cfi_offset 14, -32
	movq	%rsi, %r14
	pushq	%r13
	pushq	%r12
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	movq	%rdi, %r12
	pushq	%rbx
	subq	$56, %rsp
	.cfi_offset 3, -56
	cmpq	$3, %rsi
	jbe	.L288
	movdqa	%xmm0, %xmm6
	movdqa	%xmm1, %xmm5
	movl	$4, %r13d
	xorl	%ebx, %ebx
	jmp	.L267
	.p2align 4,,10
	.p2align 3
.L259:
	movmskps	%xmm2, %eax
	movups	%xmm3, (%r12,%rbx,4)
	popcntq	%rax, %rax
	addq	%rax, %rbx
	leaq	4(%r13), %rax
	cmpq	%r14, %rax
	ja	.L344
	movq	%rax, %r13
.L267:
	movdqu	-16(%r12,%r13,4), %xmm2
	movdqu	-16(%r12,%r13,4), %xmm0
	leaq	-4(%r13), %rdx
	pcmpeqd	%xmm5, %xmm0
	pcmpeqd	%xmm6, %xmm2
	movdqa	%xmm0, %xmm4
	por	%xmm2, %xmm0
	movmskps	%xmm0, %eax
	cmpl	$15, %eax
	je	.L259
	pcmpeqd	%xmm0, %xmm0
	pxor	%xmm0, %xmm4
	pandn	%xmm4, %xmm2
	movmskps	%xmm2, %eax
	rep bsfl	%eax, %eax
	cltq
	addq	%rdx, %rax
	movd	(%r12,%rax,4), %xmm7
	leaq	4(%rbx), %rax
	pshufd	$0, %xmm7, %xmm0
	movaps	%xmm0, (%r15)
	cmpq	%rdx, %rax
	ja	.L260
	.p2align 4,,10
	.p2align 3
.L261:
	movups	%xmm1, -16(%r12,%rax,4)
	movq	%rax, %rbx
	addq	$4, %rax
	cmpq	%rdx, %rax
	jbe	.L261
.L260:
	subq	%rbx, %rdx
	leaq	0(,%rbx,4), %rcx
	movd	%edx, %xmm7
	pshufd	$0, %xmm7, %xmm0
	pcmpgtd	.LC0(%rip), %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L262
	movd	%xmm1, (%r12,%rbx,4)
.L262:
	pextrd	$1, %xmm0, %eax
	testl	%eax, %eax
	je	.L263
	pextrd	$1, %xmm1, 4(%r12,%rcx)
.L263:
	pextrd	$2, %xmm0, %eax
	testl	%eax, %eax
	je	.L264
	pextrd	$2, %xmm1, 8(%r12,%rcx)
.L264:
	pextrd	$3, %xmm0, %eax
	testl	%eax, %eax
	jne	.L345
.L277:
	addq	$56, %rsp
	xorl	%eax, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L345:
	.cfi_restore_state
	pextrd	$3, %xmm1, 12(%r12,%rcx)
	jmp	.L277
	.p2align 4,,10
	.p2align 3
.L344:
	movq	%r14, %r8
	leaq	0(,%r13,4), %rsi
	leaq	0(,%rbx,4), %r9
	subq	%r13, %r8
.L258:
	testq	%r8, %r8
	je	.L271
	leaq	0(,%r8,4), %rdx
	movq	%rcx, %rdi
	addq	%r12, %rsi
	movq	%r9, -64(%rbp)
	movq	%r8, -56(%rbp)
	movaps	%xmm1, -96(%rbp)
	movaps	%xmm3, -80(%rbp)
	call	memcpy@PLT
	movq	-56(%rbp), %r8
	movq	-64(%rbp), %r9
	movdqa	-80(%rbp), %xmm3
	movdqa	-96(%rbp), %xmm1
	movq	%rax, %rcx
.L271:
	movdqa	(%rcx), %xmm4
	movd	%r8d, %xmm7
	movdqa	.LC0(%rip), %xmm5
	pshufd	$0, %xmm7, %xmm0
	pcmpgtd	%xmm5, %xmm0
	movdqa	%xmm4, %xmm2
	pcmpeqd	%xmm3, %xmm2
	pcmpeqd	%xmm1, %xmm4
	movdqa	%xmm0, %xmm7
	pand	%xmm2, %xmm7
	por	%xmm4, %xmm2
	pcmpeqd	%xmm4, %xmm4
	movdqa	%xmm4, %xmm6
	pxor	%xmm0, %xmm6
	por	%xmm6, %xmm2
	movmskps	%xmm2, %eax
	cmpl	$15, %eax
	jne	.L346
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L278
	movd	%xmm3, (%r12,%r9)
.L278:
	pextrd	$1, %xmm0, %eax
	testl	%eax, %eax
	je	.L279
	pextrd	$1, %xmm3, 4(%r12,%r9)
.L279:
	pextrd	$2, %xmm0, %eax
	testl	%eax, %eax
	je	.L280
	pextrd	$2, %xmm3, 8(%r12,%r9)
.L280:
	pextrd	$3, %xmm0, %eax
	testl	%eax, %eax
	jne	.L347
.L281:
	movmskps	%xmm7, %edx
	popcntq	%rdx, %rdx
	addq	%rbx, %rdx
	leaq	4(%rdx), %rax
	cmpq	%rax, %r14
	jb	.L282
	.p2align 4,,10
	.p2align 3
.L283:
	movups	%xmm1, -16(%r12,%rax,4)
	movq	%rax, %rdx
	addq	$4, %rax
	cmpq	%rax, %r14
	jnb	.L283
.L282:
	subq	%rdx, %r14
	leaq	0(,%rdx,4), %rcx
	movd	%r14d, %xmm7
	pshufd	$0, %xmm7, %xmm0
	pcmpgtd	%xmm5, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L284
	movd	%xmm1, (%r12,%rdx,4)
.L284:
	pextrd	$1, %xmm0, %eax
	testl	%eax, %eax
	je	.L285
	pextrd	$1, %xmm1, 4(%r12,%rcx)
.L285:
	pextrd	$2, %xmm0, %eax
	testl	%eax, %eax
	je	.L286
	pextrd	$2, %xmm1, 8(%r12,%rcx)
.L286:
	pextrd	$3, %xmm0, %eax
	testl	%eax, %eax
	je	.L287
	pextrd	$3, %xmm1, 12(%r12,%rcx)
.L287:
	addq	$56, %rsp
	movl	$1, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L347:
	.cfi_restore_state
	pextrd	$3, %xmm3, 12(%r12,%r9)
	jmp	.L281
.L288:
	movq	%rsi, %r8
	xorl	%r9d, %r9d
	xorl	%esi, %esi
	xorl	%ebx, %ebx
	xorl	%r13d, %r13d
	jmp	.L258
.L346:
	pxor	%xmm4, %xmm2
	movmskps	%xmm2, %eax
	rep bsfl	%eax, %eax
	cltq
	addq	%r13, %rax
	movd	(%r12,%rax,4), %xmm7
	leaq	4(%rbx), %rax
	pshufd	$0, %xmm7, %xmm0
	movaps	%xmm0, (%r15)
	cmpq	%rax, %r13
	jb	.L272
	.p2align 4,,10
	.p2align 3
.L273:
	movups	%xmm1, -16(%r12,%rax,4)
	movq	%rax, %rbx
	leaq	4(%rax), %rax
	cmpq	%r13, %rax
	jbe	.L273
	leaq	0(,%rbx,4), %r9
.L272:
	subq	%rbx, %r13
	movd	%r13d, %xmm7
	pshufd	$0, %xmm7, %xmm0
	pcmpgtd	%xmm5, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L274
	movd	%xmm1, (%r12,%r9)
.L274:
	pextrd	$1, %xmm0, %eax
	testl	%eax, %eax
	je	.L275
	pextrd	$1, %xmm1, 4(%r12,%r9)
.L275:
	pextrd	$2, %xmm0, %eax
	testl	%eax, %eax
	je	.L276
	pextrd	$2, %xmm1, 8(%r12,%r9)
.L276:
	pextrd	$3, %xmm0, %eax
	testl	%eax, %eax
	je	.L277
	pextrd	$3, %xmm1, 12(%r12,%r9)
	jmp	.L277
	.cfi_endproc
.LFE18792:
	.size	_ZN3hwy6N_SSE46detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, .-_ZN3hwy6N_SSE46detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	.section	.text._ZN3hwy6N_SSE46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0, @function
_ZN3hwy6N_SSE46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0:
.LFB18793:
	.cfi_startproc
	cmpq	%rdx, %rsi
	jbe	.L348
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r10
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rax, %rsi
	jbe	.L348
	movl	(%rdi,%rdx,4), %r11d
	movd	%r11d, %xmm6
	pshufd	$0, %xmm6, %xmm0
	jmp	.L351
	.p2align 4,,10
	.p2align 3
.L352:
	cmpq	%rcx, %rsi
	jbe	.L348
	movq	%rdx, %rax
.L357:
	movd	(%rdi,%r10,8), %xmm5
	pshufd	$0, %xmm5, %xmm1
	pcmpgtd	%xmm3, %xmm1
	movmskps	%xmm1, %r8d
	andl	$1, %r8d
	jne	.L354
.L353:
	cmpq	%rdx, %rax
	je	.L348
	leaq	(%rdi,%rax,4), %rdx
	movl	(%rdx), %ecx
	movl	%ecx, (%r9)
	movl	%r11d, (%rdx)
	cmpq	%rax, %rsi
	jbe	.L361
	movq	%rax, %rdx
.L355:
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r10
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rsi, %rax
	jnb	.L348
.L351:
	movd	(%rdi,%rax,4), %xmm4
	leaq	(%rdi,%rdx,4), %r9
	movdqa	%xmm0, %xmm3
	pshufd	$0, %xmm4, %xmm1
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm0, %xmm2
	movmskps	%xmm2, %r8d
	andl	$1, %r8d
	je	.L352
	cmpq	%rcx, %rsi
	jbe	.L353
	movdqa	%xmm1, %xmm3
	jmp	.L357
	.p2align 4,,10
	.p2align 3
.L354:
	cmpq	%rcx, %rdx
	je	.L362
	leaq	(%rdi,%rcx,4), %rax
	movl	(%rax), %edx
	movl	%edx, (%r9)
	movq	%rcx, %rdx
	movl	%r11d, (%rax)
	jmp	.L355
	.p2align 4,,10
	.p2align 3
.L348:
	ret
	.p2align 4,,10
	.p2align 3
.L361:
	ret
.L362:
	ret
	.cfi_endproc
.LFE18793:
	.size	_ZN3hwy6N_SSE46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0, .-_ZN3hwy6N_SSE46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	.section	.text._ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0, @function
_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0:
.LFB18794:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %rax
	salq	$2, %rax
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	.cfi_offset 15, -24
	leaq	(%rdi,%rax), %r15
	pushq	%r14
	.cfi_offset 14, -32
	leaq	(%r15,%rax), %r14
	pushq	%r13
	.cfi_offset 13, -40
	leaq	(%r14,%rax), %r13
	pushq	%r12
	.cfi_offset 12, -48
	leaq	0(%r13,%rax), %r12
	pushq	%rbx
	.cfi_offset 3, -56
	leaq	(%r12,%rax), %rbx
	leaq	(%rbx,%rax), %r11
	leaq	(%r11,%rax), %r10
	leaq	(%r10,%rax), %r9
	subq	$64, %rsp
	leaq	(%r9,%rax), %r8
	movq	%rdi, -56(%rbp)
	leaq	(%r8,%rax), %rdi
	movq	%rsi, -128(%rbp)
	leaq	(%rdi,%rax), %rsi
	movdqu	(%r15), %xmm4
	movdqu	(%r14), %xmm14
	leaq	(%rsi,%rax), %rcx
	movdqu	(%r14), %xmm8
	movdqu	(%r11), %xmm3
	movdqu	(%r12), %xmm13
	movdqu	(%r12), %xmm1
	movq	%rcx, -64(%rbp)
	addq	%rax, %rcx
	leaq	(%rcx,%rax), %rdx
	movdqu	(%r11), %xmm7
	movdqu	(%r9), %xmm12
	movq	%rcx, -136(%rbp)
	addq	%rdx, %rax
	movq	%rdx, -112(%rbp)
	movdqu	(%r9), %xmm6
	movq	%rdx, -144(%rbp)
	movq	-56(%rbp), %rdx
	movdqu	(%rdi), %xmm11
	movdqu	(%rdx), %xmm15
	movdqu	(%rdx), %xmm9
	pminsd	%xmm4, %xmm15
	pmaxsd	%xmm4, %xmm9
	movdqu	0(%r13), %xmm4
	pminsd	%xmm4, %xmm14
	pmaxsd	%xmm4, %xmm8
	movdqu	(%rbx), %xmm4
	pminsd	%xmm4, %xmm13
	pmaxsd	%xmm4, %xmm1
	movdqu	(%r10), %xmm4
	pmaxsd	%xmm4, %xmm7
	pminsd	%xmm4, %xmm3
	movdqu	(%r8), %xmm4
	pmaxsd	%xmm4, %xmm6
	pminsd	%xmm4, %xmm12
	movdqu	(%rsi), %xmm4
	movdqu	(%rdi), %xmm5
	movdqu	(%rax), %xmm10
	cmpq	$1, -128(%rbp)
	pminsd	%xmm4, %xmm11
	pmaxsd	%xmm4, %xmm5
	movdqu	(%rcx), %xmm4
	movq	-64(%rbp), %rcx
	movaps	%xmm4, -80(%rbp)
	movdqu	(%rcx), %xmm4
	movq	-112(%rbp), %rcx
	movaps	%xmm4, -96(%rbp)
	movdqa	-96(%rbp), %xmm4
	pminsd	-80(%rbp), %xmm4
	movdqu	(%rcx), %xmm2
	movdqa	-96(%rbp), %xmm0
	movaps	%xmm4, -160(%rbp)
	movdqu	(%rcx), %xmm4
	pmaxsd	-80(%rbp), %xmm0
	pminsd	%xmm10, %xmm2
	pmaxsd	%xmm10, %xmm4
	movdqa	%xmm15, %xmm10
	pmaxsd	%xmm14, %xmm15
	pminsd	%xmm14, %xmm10
	movdqa	%xmm9, %xmm14
	pmaxsd	%xmm8, %xmm9
	movaps	%xmm9, -80(%rbp)
	movdqa	%xmm1, %xmm9
	pminsd	%xmm8, %xmm14
	pmaxsd	%xmm7, %xmm1
	pminsd	%xmm7, %xmm9
	movdqa	%xmm13, %xmm8
	movdqa	%xmm12, %xmm7
	pminsd	%xmm3, %xmm8
	pminsd	%xmm11, %xmm7
	pmaxsd	%xmm13, %xmm3
	pmaxsd	%xmm11, %xmm12
	movdqa	%xmm6, %xmm13
	movdqa	%xmm6, %xmm11
	movdqa	-160(%rbp), %xmm6
	pmaxsd	%xmm5, %xmm13
	pminsd	%xmm5, %xmm11
	movdqa	%xmm6, %xmm5
	pminsd	%xmm2, %xmm5
	pmaxsd	%xmm6, %xmm2
	movdqa	%xmm0, %xmm6
	pminsd	%xmm4, %xmm6
	pmaxsd	%xmm4, %xmm0
	movdqa	%xmm10, %xmm4
	pminsd	%xmm8, %xmm4
	pmaxsd	%xmm8, %xmm10
	movdqa	%xmm14, %xmm8
	pminsd	%xmm9, %xmm8
	pmaxsd	%xmm9, %xmm14
	movdqa	%xmm15, %xmm9
	pmaxsd	%xmm3, %xmm15
	pminsd	%xmm3, %xmm9
	movdqa	-80(%rbp), %xmm3
	movaps	%xmm15, -96(%rbp)
	movdqa	-80(%rbp), %xmm15
	pminsd	%xmm1, %xmm3
	pmaxsd	%xmm1, %xmm15
	movdqa	%xmm7, %xmm1
	pminsd	%xmm5, %xmm1
	pmaxsd	%xmm7, %xmm5
	movdqa	%xmm11, %xmm7
	pminsd	%xmm6, %xmm7
	pmaxsd	%xmm11, %xmm6
	movdqa	%xmm12, %xmm11
	pminsd	%xmm2, %xmm11
	pmaxsd	%xmm2, %xmm12
	movdqa	%xmm13, %xmm2
	pminsd	%xmm0, %xmm2
	pmaxsd	%xmm13, %xmm0
	movdqa	%xmm4, %xmm13
	pminsd	%xmm1, %xmm13
	pmaxsd	%xmm4, %xmm1
	movaps	%xmm13, -80(%rbp)
	movdqa	%xmm8, %xmm13
	pminsd	%xmm7, %xmm13
	pmaxsd	%xmm8, %xmm7
	movdqa	%xmm9, %xmm8
	pminsd	%xmm11, %xmm8
	pmaxsd	%xmm9, %xmm11
	movdqa	%xmm3, %xmm9
	pminsd	%xmm2, %xmm9
	pmaxsd	%xmm3, %xmm2
	movdqa	%xmm10, %xmm3
	pminsd	%xmm5, %xmm3
	pmaxsd	%xmm10, %xmm5
	movdqa	%xmm14, %xmm10
	pminsd	%xmm6, %xmm10
	pmaxsd	%xmm14, %xmm6
	movdqa	-96(%rbp), %xmm14
	movdqa	%xmm14, %xmm4
	pminsd	%xmm12, %xmm4
	pmaxsd	%xmm14, %xmm12
	movdqa	%xmm15, %xmm14
	pminsd	%xmm0, %xmm14
	pmaxsd	%xmm15, %xmm0
	movdqa	%xmm9, %xmm15
	movaps	%xmm0, -96(%rbp)
	movdqa	%xmm10, %xmm0
	pmaxsd	%xmm11, %xmm10
	pminsd	%xmm5, %xmm15
	pminsd	%xmm11, %xmm0
	movdqa	%xmm4, %xmm11
	pmaxsd	%xmm7, %xmm4
	pminsd	%xmm7, %xmm11
	movdqa	%xmm14, %xmm7
	pmaxsd	%xmm2, %xmm14
	pminsd	%xmm2, %xmm7
	movdqa	%xmm3, %xmm2
	pmaxsd	%xmm1, %xmm3
	pminsd	%xmm1, %xmm2
	movdqa	%xmm13, %xmm1
	pmaxsd	%xmm9, %xmm5
	pminsd	%xmm8, %xmm1
	movdqa	%xmm6, %xmm9
	pmaxsd	%xmm13, %xmm8
	pminsd	%xmm12, %xmm9
	pmaxsd	%xmm6, %xmm12
	movdqa	%xmm1, %xmm6
	pminsd	%xmm2, %xmm6
	pmaxsd	%xmm2, %xmm1
	movdqa	%xmm7, %xmm2
	movaps	%xmm6, -112(%rbp)
	movdqa	%xmm8, %xmm6
	pminsd	%xmm9, %xmm2
	pmaxsd	%xmm3, %xmm8
	pminsd	%xmm3, %xmm6
	pmaxsd	%xmm7, %xmm9
	movdqa	%xmm2, %xmm13
	movdqa	%xmm6, %xmm3
	movdqa	%xmm14, %xmm7
	pminsd	%xmm5, %xmm13
	pminsd	%xmm1, %xmm3
	pminsd	%xmm12, %xmm7
	pmaxsd	%xmm12, %xmm14
	movdqa	%xmm0, %xmm12
	pmaxsd	%xmm1, %xmm6
	movaps	%xmm3, -176(%rbp)
	movdqa	%xmm4, %xmm3
	pmaxsd	%xmm11, %xmm0
	movdqa	%xmm15, %xmm1
	movaps	%xmm14, -160(%rbp)
	pminsd	%xmm10, %xmm3
	pminsd	%xmm11, %xmm12
	movdqa	%xmm15, %xmm11
	movdqa	%xmm13, %xmm15
	pmaxsd	%xmm10, %xmm4
	pmaxsd	%xmm2, %xmm5
	pminsd	%xmm8, %xmm11
	pmaxsd	%xmm8, %xmm1
	movdqa	%xmm0, %xmm8
	pminsd	%xmm3, %xmm15
	pminsd	%xmm1, %xmm8
	movdqa	%xmm7, %xmm10
	pmaxsd	%xmm0, %xmm1
	pmaxsd	%xmm3, %xmm13
	movdqa	%xmm4, %xmm0
	movdqa	%xmm15, %xmm3
	pminsd	%xmm9, %xmm10
	pminsd	%xmm5, %xmm0
	pminsd	%xmm1, %xmm3
	pmaxsd	%xmm1, %xmm15
	movdqa	%xmm11, %xmm2
	movdqa	%xmm13, %xmm1
	pmaxsd	%xmm5, %xmm4
	pmaxsd	%xmm12, %xmm11
	pminsd	%xmm0, %xmm1
	movdqa	%xmm11, %xmm5
	movdqa	%xmm15, %xmm14
	pminsd	%xmm12, %xmm2
	pmaxsd	%xmm8, %xmm11
	pmaxsd	%xmm0, %xmm13
	movdqa	%xmm10, %xmm0
	pmaxsd	%xmm9, %xmm7
	pminsd	%xmm4, %xmm0
	movdqa	%xmm2, %xmm9
	pmaxsd	%xmm10, %xmm4
	movdqa	%xmm11, %xmm10
	pminsd	%xmm6, %xmm9
	pmaxsd	%xmm6, %xmm2
	pminsd	%xmm8, %xmm5
	pminsd	%xmm3, %xmm10
	pmaxsd	%xmm3, %xmm11
	pminsd	%xmm1, %xmm14
	pmaxsd	%xmm1, %xmm15
	jbe	.L368
	movdqa	-80(%rbp), %xmm3
	pshufd	$177, -96(%rbp), %xmm6
	pshufd	$177, %xmm7, %xmm7
	pshufd	$177, -160(%rbp), %xmm8
	pshufd	$177, %xmm4, %xmm4
	pshufd	$177, %xmm0, %xmm0
	pshufd	$177, %xmm13, %xmm13
	cmpq	$3, -128(%rbp)
	movdqa	%xmm3, %xmm12
	pshufd	$177, %xmm15, %xmm15
	pshufd	$177, %xmm14, %xmm14
	pminsd	%xmm6, %xmm12
	pmaxsd	%xmm3, %xmm6
	movdqa	-112(%rbp), %xmm3
	movaps	%xmm6, -80(%rbp)
	movdqa	-176(%rbp), %xmm6
	movdqa	%xmm3, %xmm1
	pminsd	%xmm8, %xmm1
	pmaxsd	%xmm3, %xmm8
	movdqa	%xmm6, %xmm3
	pminsd	%xmm7, %xmm3
	pmaxsd	%xmm6, %xmm7
	movdqa	%xmm9, %xmm6
	pminsd	%xmm4, %xmm6
	pmaxsd	%xmm4, %xmm9
	movdqa	%xmm2, %xmm4
	pminsd	%xmm0, %xmm4
	pmaxsd	%xmm0, %xmm2
	movdqa	%xmm5, %xmm0
	movaps	%xmm6, -96(%rbp)
	pmaxsd	%xmm13, %xmm5
	pshufd	$177, %xmm7, %xmm7
	pshufd	$177, %xmm4, %xmm4
	pshufd	$177, -80(%rbp), %xmm6
	pminsd	%xmm13, %xmm0
	movdqa	%xmm10, %xmm13
	pmaxsd	%xmm15, %xmm10
	pminsd	%xmm15, %xmm13
	movdqa	%xmm11, %xmm15
	pmaxsd	%xmm14, %xmm11
	pminsd	%xmm14, %xmm15
	movdqa	%xmm12, %xmm14
	pshufd	$177, %xmm13, %xmm13
	pshufd	$177, %xmm15, %xmm15
	pshufd	$177, %xmm8, %xmm8
	pshufd	$177, %xmm0, %xmm0
	pminsd	%xmm15, %xmm14
	pmaxsd	%xmm15, %xmm12
	movdqa	%xmm11, %xmm15
	pminsd	%xmm6, %xmm15
	pmaxsd	%xmm6, %xmm11
	movdqa	%xmm1, %xmm6
	pminsd	%xmm13, %xmm6
	pmaxsd	%xmm13, %xmm1
	movdqa	%xmm10, %xmm13
	movaps	%xmm15, -80(%rbp)
	pminsd	%xmm8, %xmm13
	pmaxsd	%xmm8, %xmm10
	movdqa	%xmm3, %xmm8
	pminsd	%xmm0, %xmm8
	pmaxsd	%xmm0, %xmm3
	movdqa	%xmm5, %xmm0
	pminsd	%xmm7, %xmm0
	pmaxsd	%xmm7, %xmm5
	movdqa	-96(%rbp), %xmm7
	movdqa	%xmm13, %xmm15
	pshufd	$177, %xmm9, %xmm9
	pshufd	$177, %xmm8, %xmm8
	pshufd	$177, %xmm12, %xmm12
	movdqa	%xmm7, %xmm13
	pshufd	$177, %xmm1, %xmm1
	pshufd	$177, %xmm0, %xmm0
	pminsd	%xmm4, %xmm13
	pmaxsd	%xmm7, %xmm4
	movdqa	%xmm2, %xmm7
	pshufd	$177, %xmm13, %xmm13
	pmaxsd	%xmm9, %xmm2
	pminsd	%xmm9, %xmm7
	movdqa	%xmm14, %xmm9
	pmaxsd	%xmm13, %xmm14
	pshufd	$177, %xmm7, %xmm7
	pminsd	%xmm13, %xmm9
	movdqa	%xmm6, %xmm13
	pmaxsd	%xmm8, %xmm6
	pminsd	%xmm8, %xmm13
	movdqa	%xmm4, %xmm8
	pmaxsd	%xmm12, %xmm4
	pminsd	%xmm12, %xmm8
	movaps	%xmm13, -96(%rbp)
	movdqa	-80(%rbp), %xmm12
	movdqa	%xmm3, %xmm13
	pminsd	%xmm1, %xmm13
	pmaxsd	%xmm1, %xmm3
	pshufd	$177, %xmm11, %xmm11
	movdqa	%xmm12, %xmm1
	pshufd	$177, %xmm10, %xmm10
	pshufd	$177, %xmm4, %xmm4
	pminsd	%xmm7, %xmm1
	pmaxsd	%xmm12, %xmm7
	movdqa	%xmm15, %xmm12
	pminsd	%xmm0, %xmm12
	pmaxsd	%xmm15, %xmm0
	movdqa	%xmm2, %xmm15
	pminsd	%xmm11, %xmm15
	pmaxsd	%xmm11, %xmm2
	movdqa	%xmm5, %xmm11
	pminsd	%xmm10, %xmm11
	movaps	%xmm15, -80(%rbp)
	pmaxsd	%xmm10, %xmm5
	movdqa	%xmm9, %xmm10
	pshufd	$177, %xmm14, %xmm14
	pshufd	$177, %xmm7, %xmm7
	pshufd	$177, %xmm2, %xmm2
	pshufd	$177, -96(%rbp), %xmm15
	pminsd	%xmm15, %xmm10
	pmaxsd	%xmm15, %xmm9
	movdqa	%xmm6, %xmm15
	pminsd	%xmm14, %xmm15
	pmaxsd	%xmm14, %xmm6
	pshufd	$177, %xmm13, %xmm13
	movdqa	%xmm8, %xmm14
	pmaxsd	%xmm13, %xmm8
	pshufd	$177, %xmm12, %xmm12
	pminsd	%xmm13, %xmm14
	movdqa	%xmm3, %xmm13
	pmaxsd	%xmm4, %xmm3
	pminsd	%xmm4, %xmm13
	movdqa	%xmm1, %xmm4
	pmaxsd	%xmm12, %xmm1
	pminsd	%xmm12, %xmm4
	movdqa	%xmm0, %xmm12
	pmaxsd	%xmm7, %xmm0
	movaps	%xmm0, -96(%rbp)
	movdqa	-80(%rbp), %xmm0
	pminsd	%xmm7, %xmm12
	pshufd	$177, %xmm11, %xmm11
	movdqa	%xmm0, %xmm7
	pminsd	%xmm11, %xmm7
	pmaxsd	%xmm0, %xmm11
	movdqa	%xmm5, %xmm0
	pmaxsd	%xmm2, %xmm5
	pminsd	%xmm2, %xmm0
	pshufd	$177, %xmm10, %xmm2
	movaps	%xmm5, -112(%rbp)
	movdqa	%xmm10, %xmm5
	pmaxsd	%xmm2, %xmm10
	pminsd	%xmm2, %xmm5
	pshufd	$177, %xmm9, %xmm2
	movaps	%xmm0, -80(%rbp)
	blendps	$5, %xmm5, %xmm10
	movdqa	%xmm9, %xmm5
	pmaxsd	%xmm2, %xmm9
	pminsd	%xmm2, %xmm5
	movdqa	%xmm15, %xmm2
	blendps	$5, %xmm5, %xmm9
	pshufd	$177, %xmm15, %xmm5
	pminsd	%xmm5, %xmm2
	pmaxsd	%xmm5, %xmm15
	pshufd	$177, %xmm6, %xmm5
	blendps	$5, %xmm2, %xmm15
	movdqa	%xmm6, %xmm2
	pmaxsd	%xmm5, %xmm6
	pminsd	%xmm5, %xmm2
	pshufd	$177, %xmm14, %xmm5
	blendps	$5, %xmm2, %xmm6
	movdqa	%xmm14, %xmm2
	pmaxsd	%xmm5, %xmm14
	pminsd	%xmm5, %xmm2
	movaps	%xmm6, -160(%rbp)
	movaps	%xmm14, %xmm6
	pshufd	$177, %xmm8, %xmm5
	blendps	$5, %xmm2, %xmm6
	movdqa	%xmm8, %xmm2
	pmaxsd	%xmm5, %xmm8
	pminsd	%xmm5, %xmm2
	movaps	%xmm6, -176(%rbp)
	movaps	%xmm8, %xmm6
	pshufd	$177, %xmm13, %xmm5
	blendps	$5, %xmm2, %xmm6
	movdqa	%xmm13, %xmm2
	pmaxsd	%xmm5, %xmm13
	pminsd	%xmm5, %xmm2
	movaps	%xmm6, -192(%rbp)
	movaps	%xmm13, %xmm6
	pshufd	$177, %xmm3, %xmm5
	blendps	$5, %xmm2, %xmm6
	movdqa	%xmm3, %xmm2
	pmaxsd	%xmm5, %xmm3
	pminsd	%xmm5, %xmm2
	movaps	%xmm3, %xmm13
	movaps	%xmm6, -208(%rbp)
	pshufd	$177, %xmm4, %xmm6
	pshufd	$177, %xmm1, %xmm3
	blendps	$5, %xmm2, %xmm13
	movdqa	%xmm4, %xmm2
	movdqa	-96(%rbp), %xmm5
	pminsd	%xmm6, %xmm2
	pmaxsd	%xmm4, %xmm6
	pshufd	$177, %xmm7, %xmm8
	blendps	$5, %xmm2, %xmm6
	movdqa	%xmm1, %xmm2
	pmaxsd	%xmm3, %xmm1
	pminsd	%xmm3, %xmm2
	pshufd	$177, %xmm12, %xmm3
	pshufd	$177, %xmm5, %xmm14
	blendps	$5, %xmm2, %xmm1
	movdqa	%xmm12, %xmm2
	movdqa	%xmm7, %xmm4
	pminsd	%xmm3, %xmm2
	pmaxsd	%xmm3, %xmm12
	movdqa	%xmm11, %xmm3
	blendps	$5, %xmm2, %xmm12
	movdqa	%xmm5, %xmm2
	pmaxsd	%xmm14, %xmm5
	pminsd	%xmm14, %xmm2
	movaps	%xmm5, %xmm14
	pminsd	%xmm8, %xmm4
	blendps	$5, %xmm2, %xmm14
	pshufd	$177, %xmm11, %xmm2
	pmaxsd	%xmm7, %xmm8
	movdqa	-80(%rbp), %xmm7
	pmaxsd	%xmm2, %xmm11
	movdqa	%xmm4, %xmm0
	pminsd	%xmm2, %xmm3
	movaps	%xmm8, %xmm4
	movaps	%xmm11, %xmm2
	blendps	$5, %xmm0, %xmm4
	blendps	$5, %xmm3, %xmm2
	movdqa	%xmm7, %xmm0
	pshufd	$177, %xmm7, %xmm3
	pminsd	%xmm3, %xmm0
	pmaxsd	%xmm7, %xmm3
	movdqa	-112(%rbp), %xmm7
	blendps	$5, %xmm0, %xmm3
	pshufd	$177, %xmm7, %xmm5
	movdqa	%xmm7, %xmm0
	pminsd	%xmm5, %xmm0
	pmaxsd	%xmm7, %xmm5
	blendps	$5, %xmm0, %xmm5
	movaps	%xmm5, -224(%rbp)
	jbe	.L369
	pshufd	$27, %xmm2, %xmm7
	pshufd	$27, %xmm3, %xmm2
	movaps	-160(%rbp), %xmm3
	pshufd	$27, %xmm12, %xmm0
	pshufd	$27, %xmm4, %xmm8
	pshufd	$27, %xmm1, %xmm11
	pshufd	$27, %xmm6, %xmm6
	pshufd	$27, -224(%rbp), %xmm5
	movdqa	%xmm3, %xmm4
	movdqa	%xmm5, %xmm12
	pmaxsd	%xmm10, %xmm5
	pminsd	%xmm10, %xmm12
	pminsd	%xmm8, %xmm4
	movdqa	%xmm2, %xmm10
	pmaxsd	%xmm3, %xmm8
	movaps	-176(%rbp), %xmm3
	pshufd	$27, %xmm14, %xmm1
	pminsd	%xmm9, %xmm10
	pmaxsd	%xmm2, %xmm9
	movdqa	%xmm7, %xmm2
	movaps	%xmm4, -80(%rbp)
	movaps	-192(%rbp), %xmm14
	pminsd	%xmm15, %xmm2
	pmaxsd	%xmm15, %xmm7
	movdqa	%xmm3, %xmm4
	movaps	-208(%rbp), %xmm15
	pminsd	%xmm1, %xmm4
	pmaxsd	%xmm3, %xmm1
	movdqa	%xmm14, %xmm3
	pminsd	%xmm0, %xmm3
	pmaxsd	%xmm14, %xmm0
	movdqa	%xmm15, %xmm14
	pminsd	%xmm11, %xmm14
	pmaxsd	%xmm15, %xmm11
	movdqa	%xmm6, %xmm15
	pminsd	%xmm13, %xmm15
	pshufd	$27, %xmm5, %xmm5
	pmaxsd	%xmm13, %xmm6
	pshufd	$27, %xmm15, %xmm15
	movdqa	%xmm12, %xmm13
	pshufd	$27, %xmm3, %xmm3
	pminsd	%xmm15, %xmm13
	pshufd	$27, %xmm7, %xmm7
	pshufd	$27, %xmm4, %xmm4
	pmaxsd	%xmm15, %xmm12
	movdqa	%xmm6, %xmm15
	pshufd	$27, %xmm14, %xmm14
	pminsd	%xmm5, %xmm15
	pmaxsd	%xmm5, %xmm6
	movdqa	%xmm10, %xmm5
	pshufd	$27, %xmm9, %xmm9
	pminsd	%xmm14, %xmm5
	movaps	%xmm15, -96(%rbp)
	pmaxsd	%xmm14, %xmm10
	movdqa	%xmm11, %xmm14
	pmaxsd	%xmm9, %xmm11
	pshufd	$27, %xmm8, %xmm8
	pminsd	%xmm9, %xmm14
	movdqa	%xmm2, %xmm9
	pmaxsd	%xmm3, %xmm2
	pminsd	%xmm3, %xmm9
	movdqa	%xmm0, %xmm3
	pmaxsd	%xmm7, %xmm0
	pminsd	%xmm7, %xmm3
	movdqa	-80(%rbp), %xmm7
	movdqa	%xmm14, %xmm15
	pshufd	$27, %xmm9, %xmm9
	pshufd	$27, %xmm12, %xmm12
	pshufd	$27, %xmm3, %xmm3
	pshufd	$27, %xmm6, %xmm6
	movdqa	%xmm7, %xmm14
	pshufd	$27, %xmm11, %xmm11
	pminsd	%xmm4, %xmm14
	pmaxsd	%xmm7, %xmm4
	movdqa	%xmm1, %xmm7
	pminsd	%xmm8, %xmm7
	pmaxsd	%xmm8, %xmm1
	pshufd	$27, %xmm14, %xmm14
	pshufd	$27, %xmm10, %xmm8
	movdqa	%xmm13, %xmm10
	pshufd	$27, %xmm7, %xmm7
	pminsd	%xmm14, %xmm10
	pmaxsd	%xmm13, %xmm14
	movdqa	%xmm5, %xmm13
	pminsd	%xmm9, %xmm13
	pmaxsd	%xmm9, %xmm5
	movdqa	%xmm4, %xmm9
	pminsd	%xmm12, %xmm9
	pmaxsd	%xmm12, %xmm4
	movdqa	-96(%rbp), %xmm12
	movaps	%xmm13, -80(%rbp)
	movdqa	%xmm2, %xmm13
	pmaxsd	%xmm8, %xmm2
	pshufd	$27, %xmm14, %xmm14
	pminsd	%xmm8, %xmm13
	movdqa	%xmm12, %xmm8
	pshufd	$27, %xmm4, %xmm4
	pminsd	%xmm7, %xmm8
	pmaxsd	%xmm12, %xmm7
	movdqa	%xmm15, %xmm12
	pminsd	%xmm3, %xmm12
	pmaxsd	%xmm15, %xmm3
	movdqa	%xmm1, %xmm15
	pminsd	%xmm6, %xmm15
	pmaxsd	%xmm6, %xmm1
	movdqa	%xmm0, %xmm6
	pminsd	%xmm11, %xmm6
	movaps	%xmm15, -96(%rbp)
	pmaxsd	%xmm11, %xmm0
	movdqa	%xmm10, %xmm11
	pshufd	$27, %xmm13, %xmm13
	pshufd	$27, %xmm12, %xmm12
	pshufd	$27, %xmm7, %xmm7
	pshufd	$27, -80(%rbp), %xmm15
	pminsd	%xmm15, %xmm11
	pmaxsd	%xmm15, %xmm10
	movdqa	%xmm5, %xmm15
	pminsd	%xmm14, %xmm15
	pmaxsd	%xmm14, %xmm5
	movdqa	%xmm9, %xmm14
	pminsd	%xmm13, %xmm14
	pmaxsd	%xmm13, %xmm9
	movdqa	%xmm2, %xmm13
	pmaxsd	%xmm4, %xmm2
	pminsd	%xmm4, %xmm13
	movdqa	%xmm8, %xmm4
	movaps	%xmm2, -80(%rbp)
	movdqa	-96(%rbp), %xmm2
	pminsd	%xmm12, %xmm4
	pmaxsd	%xmm12, %xmm8
	movdqa	%xmm3, %xmm12
	pshufd	$27, %xmm1, %xmm1
	pmaxsd	%xmm7, %xmm3
	pminsd	%xmm7, %xmm12
	pshufd	$27, %xmm6, %xmm6
	movdqa	%xmm2, %xmm7
	pminsd	%xmm6, %xmm7
	pmaxsd	%xmm2, %xmm6
	movdqa	%xmm0, %xmm2
	pmaxsd	%xmm1, %xmm0
	pminsd	%xmm1, %xmm2
	movdqa	%xmm11, %xmm1
	movaps	%xmm0, -112(%rbp)
	pshufd	$27, %xmm11, %xmm0
	pminsd	%xmm0, %xmm1
	pmaxsd	%xmm0, %xmm11
	movaps	%xmm2, -96(%rbp)
	pshufd	$27, %xmm10, %xmm0
	movsd	%xmm1, %xmm11
	movdqa	%xmm10, %xmm1
	pmaxsd	%xmm0, %xmm10
	movdqa	-80(%rbp), %xmm2
	pminsd	%xmm0, %xmm1
	pshufd	$27, %xmm15, %xmm0
	movsd	%xmm1, %xmm10
	movdqa	%xmm15, %xmm1
	pmaxsd	%xmm0, %xmm15
	pminsd	%xmm0, %xmm1
	pshufd	$27, %xmm5, %xmm0
	movsd	%xmm1, %xmm15
	movdqa	%xmm5, %xmm1
	pmaxsd	%xmm0, %xmm5
	pminsd	%xmm0, %xmm1
	pshufd	$27, %xmm14, %xmm0
	movsd	%xmm1, %xmm5
	movdqa	%xmm14, %xmm1
	pmaxsd	%xmm0, %xmm14
	pminsd	%xmm0, %xmm1
	pshufd	$27, %xmm9, %xmm0
	movsd	%xmm1, %xmm14
	movdqa	%xmm9, %xmm1
	pmaxsd	%xmm0, %xmm9
	pminsd	%xmm0, %xmm1
	pshufd	$27, %xmm13, %xmm0
	movsd	%xmm1, %xmm9
	movdqa	%xmm13, %xmm1
	pmaxsd	%xmm0, %xmm13
	pminsd	%xmm0, %xmm1
	pshufd	$27, %xmm2, %xmm0
	movsd	%xmm1, %xmm13
	movdqa	%xmm2, %xmm1
	pmaxsd	%xmm0, %xmm2
	pminsd	%xmm0, %xmm1
	pshufd	$27, %xmm4, %xmm0
	movsd	%xmm1, %xmm2
	movdqa	%xmm4, %xmm1
	pmaxsd	%xmm0, %xmm4
	pminsd	%xmm0, %xmm1
	pshufd	$27, %xmm8, %xmm0
	movsd	%xmm1, %xmm4
	movdqa	%xmm8, %xmm1
	pmaxsd	%xmm0, %xmm8
	pminsd	%xmm0, %xmm1
	pshufd	$27, %xmm12, %xmm0
	movsd	%xmm1, %xmm8
	movdqa	%xmm12, %xmm1
	pmaxsd	%xmm0, %xmm12
	pminsd	%xmm0, %xmm1
	pshufd	$27, %xmm3, %xmm0
	movsd	%xmm1, %xmm12
	movdqa	%xmm3, %xmm1
	pmaxsd	%xmm0, %xmm3
	pminsd	%xmm0, %xmm1
	pshufd	$27, %xmm7, %xmm0
	movsd	%xmm1, %xmm3
	movdqa	%xmm7, %xmm1
	pmaxsd	%xmm0, %xmm7
	pminsd	%xmm0, %xmm1
	pshufd	$27, %xmm6, %xmm0
	movsd	%xmm1, %xmm7
	movdqa	%xmm6, %xmm1
	pmaxsd	%xmm0, %xmm6
	pminsd	%xmm0, %xmm1
	movapd	%xmm6, %xmm0
	movdqa	-96(%rbp), %xmm6
	movsd	%xmm1, %xmm0
	movaps	%xmm0, -176(%rbp)
	movdqa	%xmm6, %xmm1
	pshufd	$27, %xmm6, %xmm0
	pminsd	%xmm0, %xmm1
	pmaxsd	%xmm6, %xmm0
	movapd	%xmm0, %xmm6
	movsd	%xmm1, %xmm6
	movaps	%xmm6, -192(%rbp)
	movdqa	-112(%rbp), %xmm6
	pshufd	$27, %xmm6, %xmm0
	movdqa	%xmm6, %xmm1
	pminsd	%xmm0, %xmm1
	pmaxsd	%xmm6, %xmm0
	movapd	%xmm0, %xmm6
	pshufd	$177, %xmm11, %xmm0
	movsd	%xmm1, %xmm6
	movdqa	%xmm0, %xmm1
	pmaxsd	%xmm11, %xmm0
	pminsd	%xmm11, %xmm1
	movaps	%xmm0, %xmm11
	pshufd	$177, %xmm10, %xmm0
	movaps	%xmm6, -208(%rbp)
	blendps	$5, %xmm1, %xmm11
	movdqa	%xmm0, %xmm1
	pshufd	$177, %xmm4, %xmm6
	pmaxsd	%xmm10, %xmm0
	pminsd	%xmm10, %xmm1
	movaps	%xmm11, -80(%rbp)
	movaps	%xmm0, %xmm10
	blendps	$5, %xmm1, %xmm10
	pshufd	$177, %xmm15, %xmm1
	movdqa	%xmm1, %xmm0
	pmaxsd	%xmm15, %xmm1
	movaps	%xmm10, -96(%rbp)
	pshufd	$177, %xmm13, %xmm10
	pminsd	%xmm15, %xmm0
	pshufd	$177, %xmm8, %xmm15
	blendps	$5, %xmm0, %xmm1
	movaps	%xmm1, -112(%rbp)
	pshufd	$177, %xmm5, %xmm1
	movdqa	%xmm1, %xmm0
	pmaxsd	%xmm5, %xmm1
	pminsd	%xmm5, %xmm0
	movaps	%xmm1, %xmm5
	pshufd	$177, %xmm14, %xmm1
	blendps	$5, %xmm0, %xmm5
	movaps	%xmm5, -128(%rbp)
	movdqa	%xmm1, %xmm5
	pmaxsd	%xmm14, %xmm1
	pminsd	%xmm14, %xmm5
	movdqa	%xmm5, %xmm0
	movaps	%xmm1, %xmm5
	pshufd	$177, %xmm9, %xmm1
	blendps	$5, %xmm0, %xmm5
	movaps	%xmm5, -160(%rbp)
	movdqa	%xmm1, %xmm5
	pmaxsd	%xmm9, %xmm1
	pminsd	%xmm9, %xmm5
	movdqa	%xmm5, %xmm0
	movaps	%xmm1, %xmm5
	blendps	$5, %xmm0, %xmm5
	movdqa	%xmm10, %xmm0
	pmaxsd	%xmm13, %xmm10
	pminsd	%xmm13, %xmm0
	pshufd	$177, %xmm2, %xmm13
	blendps	$5, %xmm0, %xmm10
	movdqa	%xmm13, %xmm0
	pmaxsd	%xmm2, %xmm13
	pminsd	%xmm2, %xmm0
	movaps	%xmm13, %xmm1
	pshufd	$177, %xmm12, %xmm13
	movapd	-192(%rbp), %xmm2
	blendps	$5, %xmm0, %xmm1
	movdqa	%xmm6, %xmm0
	pmaxsd	%xmm4, %xmm6
	pminsd	%xmm4, %xmm0
	movdqa	%xmm15, %xmm4
	pmaxsd	%xmm8, %xmm15
	pminsd	%xmm8, %xmm4
	blendps	$5, %xmm0, %xmm6
	movaps	%xmm1, %xmm9
	pshufd	$177, %xmm3, %xmm0
	blendps	$5, %xmm4, %xmm15
	movdqa	%xmm13, %xmm1
	movdqa	%xmm0, %xmm4
	pmaxsd	%xmm3, %xmm0
	pminsd	%xmm12, %xmm1
	pminsd	%xmm3, %xmm4
	pmaxsd	%xmm12, %xmm13
	movapd	-176(%rbp), %xmm3
	blendps	$5, %xmm4, %xmm0
	pshufd	$177, %xmm7, %xmm4
	blendps	$5, %xmm1, %xmm13
	movdqa	%xmm4, %xmm1
	pmaxsd	%xmm7, %xmm4
	pminsd	%xmm7, %xmm1
	pshufd	$177, %xmm3, %xmm7
	blendps	$5, %xmm1, %xmm4
	movdqa	%xmm3, %xmm1
	pminsd	%xmm7, %xmm1
	pmaxsd	%xmm3, %xmm7
	pshufd	$177, %xmm2, %xmm3
	blendps	$5, %xmm1, %xmm7
	movdqa	%xmm2, %xmm1
	pminsd	%xmm3, %xmm1
	pmaxsd	%xmm2, %xmm3
	movapd	-208(%rbp), %xmm2
	blendps	$5, %xmm1, %xmm3
	pshufd	$177, %xmm2, %xmm8
	movdqa	%xmm2, %xmm1
	pminsd	%xmm8, %xmm1
	pmaxsd	%xmm2, %xmm8
	blendps	$5, %xmm1, %xmm8
.L365:
	movdqa	-80(%rbp), %xmm2
	movups	%xmm2, (%rdx)
	movdqa	-96(%rbp), %xmm2
	movq	-64(%rbp), %rdx
	movups	%xmm2, (%r15)
	movdqa	-112(%rbp), %xmm2
	movups	%xmm2, (%r14)
	movdqa	-128(%rbp), %xmm2
	movups	%xmm2, 0(%r13)
	movdqa	-160(%rbp), %xmm2
	movups	%xmm2, (%r12)
	movups	%xmm5, (%rbx)
	movq	-136(%rbp), %rbx
	movups	%xmm10, (%r11)
	movups	%xmm9, (%r10)
	movups	%xmm6, (%r9)
	movups	%xmm15, (%r8)
	movups	%xmm13, (%rdi)
	movups	%xmm0, (%rsi)
	movups	%xmm4, (%rdx)
	movups	%xmm7, (%rbx)
	movq	-144(%rbp), %rbx
	movups	%xmm3, (%rbx)
	movups	%xmm8, (%rax)
	addq	$64, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L368:
	.cfi_restore_state
	movdqa	-112(%rbp), %xmm3
	movdqa	-96(%rbp), %xmm8
	movaps	%xmm9, -128(%rbp)
	movdqa	%xmm11, %xmm9
	movdqa	-176(%rbp), %xmm6
	movaps	%xmm3, -96(%rbp)
	movdqa	-160(%rbp), %xmm3
	movaps	%xmm6, -112(%rbp)
	movdqa	%xmm14, %xmm6
	movaps	%xmm2, -160(%rbp)
	jmp	.L365
	.p2align 4,,10
	.p2align 3
.L369:
	movaps	%xmm10, -80(%rbp)
	movdqa	%xmm14, %xmm0
	movdqa	%xmm2, %xmm7
	movdqa	-160(%rbp), %xmm5
	movdqa	-208(%rbp), %xmm10
	movaps	%xmm9, -96(%rbp)
	movdqa	-224(%rbp), %xmm8
	movdqa	%xmm13, %xmm9
	movaps	%xmm5, -128(%rbp)
	movdqa	-176(%rbp), %xmm5
	movdqa	%xmm12, %xmm13
	movaps	%xmm15, -112(%rbp)
	movdqa	%xmm1, %xmm15
	movaps	%xmm5, -160(%rbp)
	movdqa	-192(%rbp), %xmm5
	jmp	.L365
	.cfi_endproc
.LFE18794:
	.size	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0, .-_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	.section	.text._ZN3hwy7N_SSSE36detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy7N_SSSE36detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, @function
_ZN3hwy7N_SSSE36detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0:
.LFB18795:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	movq	%rcx, %r14
	pushq	%r13
	.cfi_offset 13, -40
	movq	%rsi, %r13
	pushq	%r12
	.cfi_offset 12, -48
	movq	%rdi, %r12
	pushq	%rbx
	subq	$88, %rsp
	.cfi_offset 3, -56
	movq	%rdx, -120(%rbp)
	movaps	%xmm0, -80(%rbp)
	movaps	%xmm1, -64(%rbp)
	movaps	%xmm0, -112(%rbp)
	movaps	%xmm1, -96(%rbp)
	cmpq	$3, %rsi
	jbe	.L401
	movl	$4, %r15d
	xorl	%ebx, %ebx
	jmp	.L380
	.p2align 4,,10
	.p2align 3
.L372:
	movdqa	-80(%rbp), %xmm5
	movmskps	%xmm1, %edi
	movups	%xmm5, (%r12,%rbx,4)
	call	__popcountdi2@PLT
	cltq
	addq	%rax, %rbx
	leaq	4(%r15), %rax
	cmpq	%r13, %rax
	ja	.L457
	movq	%rax, %r15
.L380:
	movdqu	-16(%r12,%r15,4), %xmm1
	movdqu	-16(%r12,%r15,4), %xmm0
	leaq	-4(%r15), %rdx
	pcmpeqd	-96(%rbp), %xmm0
	pcmpeqd	-112(%rbp), %xmm1
	movdqa	%xmm0, %xmm2
	por	%xmm1, %xmm0
	movmskps	%xmm0, %eax
	cmpl	$15, %eax
	je	.L372
	pcmpeqd	%xmm0, %xmm0
	pxor	%xmm0, %xmm2
	pandn	%xmm2, %xmm1
	movmskps	%xmm1, %eax
	rep bsfl	%eax, %eax
	cltq
	addq	%rdx, %rax
	movd	(%r12,%rax,4), %xmm3
	movq	-120(%rbp), %rax
	pshufd	$0, %xmm3, %xmm0
	movaps	%xmm0, (%rax)
	leaq	4(%rbx), %rax
	cmpq	%rdx, %rax
	ja	.L373
	.p2align 4,,10
	.p2align 3
.L374:
	movdqa	-64(%rbp), %xmm4
	movq	%rax, %rbx
	movups	%xmm4, -16(%r12,%rax,4)
	addq	$4, %rax
	cmpq	%rdx, %rax
	jbe	.L374
.L373:
	subq	%rbx, %rdx
	leaq	0(,%rbx,4), %rcx
	movd	%edx, %xmm3
	pshufd	$0, %xmm3, %xmm0
	pcmpgtd	.LC0(%rip), %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L375
	movdqa	-64(%rbp), %xmm3
	movd	%xmm3, (%r12,%rbx,4)
.L375:
	pshufd	$85, %xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L376
	pshufd	$85, -64(%rbp), %xmm1
	movd	%xmm1, 4(%r12,%rcx)
.L376:
	movdqa	%xmm0, %xmm1
	punpckhdq	%xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L377
	movdqa	-64(%rbp), %xmm3
	movdqa	%xmm3, %xmm1
	punpckhdq	%xmm3, %xmm1
	movd	%xmm1, 8(%r12,%rcx)
.L377:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L458
.L390:
	addq	$88, %rsp
	xorl	%eax, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L458:
	.cfi_restore_state
	pshufd	$255, -64(%rbp), %xmm0
	movd	%xmm0, 12(%r12,%rcx)
	jmp	.L390
	.p2align 4,,10
	.p2align 3
.L457:
	movq	%r13, %r8
	leaq	0(,%r15,4), %rsi
	leaq	0(,%rbx,4), %r9
	subq	%r15, %r8
.L371:
	testq	%r8, %r8
	je	.L384
	leaq	0(,%r8,4), %rdx
	addq	%r12, %rsi
	movq	%r14, %rdi
	movq	%r9, -112(%rbp)
	movq	%r8, -96(%rbp)
	call	memcpy@PLT
	movq	-96(%rbp), %r8
	movq	-112(%rbp), %r9
.L384:
	movd	%r8d, %xmm3
	movdqa	(%r14), %xmm2
	movdqa	-80(%rbp), %xmm1
	pshufd	$0, %xmm3, %xmm0
	movdqa	.LC0(%rip), %xmm3
	pcmpeqd	%xmm2, %xmm1
	pcmpeqd	-64(%rbp), %xmm2
	pcmpgtd	%xmm3, %xmm0
	movdqa	%xmm0, %xmm5
	pand	%xmm1, %xmm5
	por	%xmm2, %xmm1
	pcmpeqd	%xmm2, %xmm2
	movdqa	%xmm2, %xmm4
	pxor	%xmm0, %xmm4
	por	%xmm4, %xmm1
	movmskps	%xmm1, %eax
	cmpl	$15, %eax
	jne	.L459
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L391
	movdqa	-80(%rbp), %xmm4
	movd	%xmm4, (%r12,%r9)
.L391:
	pshufd	$85, %xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L392
	pshufd	$85, -80(%rbp), %xmm1
	movd	%xmm1, 4(%r12,%r9)
.L392:
	movdqa	%xmm0, %xmm1
	punpckhdq	%xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L393
	movdqa	-80(%rbp), %xmm7
	movdqa	%xmm7, %xmm1
	punpckhdq	%xmm7, %xmm1
	movd	%xmm1, 8(%r12,%r9)
.L393:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L460
.L394:
	movmskps	%xmm5, %edi
	call	__popcountdi2@PLT
	movdqa	.LC0(%rip), %xmm3
	movslq	%eax, %rdx
	addq	%rbx, %rdx
	leaq	4(%rdx), %rax
	cmpq	%rax, %r13
	jb	.L395
	.p2align 4,,10
	.p2align 3
.L396:
	movdqa	-64(%rbp), %xmm2
	movq	%rax, %rdx
	movups	%xmm2, -16(%r12,%rax,4)
	addq	$4, %rax
	cmpq	%rax, %r13
	jnb	.L396
.L395:
	subq	%rdx, %r13
	leaq	0(,%rdx,4), %rcx
	movd	%r13d, %xmm4
	pshufd	$0, %xmm4, %xmm0
	pcmpgtd	%xmm3, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L397
	movdqa	-64(%rbp), %xmm3
	movd	%xmm3, (%r12,%rdx,4)
.L397:
	pshufd	$85, %xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L398
	pshufd	$85, -64(%rbp), %xmm1
	movd	%xmm1, 4(%r12,%rcx)
.L398:
	movdqa	%xmm0, %xmm1
	punpckhdq	%xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L399
	movdqa	-64(%rbp), %xmm3
	movdqa	%xmm3, %xmm1
	punpckhdq	%xmm3, %xmm1
	movd	%xmm1, 8(%r12,%rcx)
.L399:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L400
	pshufd	$255, -64(%rbp), %xmm0
	movd	%xmm0, 12(%r12,%rcx)
.L400:
	addq	$88, %rsp
	movl	$1, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L460:
	.cfi_restore_state
	pshufd	$255, -80(%rbp), %xmm0
	movd	%xmm0, 12(%r12,%r9)
	jmp	.L394
.L401:
	movq	%rsi, %r8
	xorl	%r9d, %r9d
	xorl	%esi, %esi
	xorl	%ebx, %ebx
	xorl	%r15d, %r15d
	jmp	.L371
.L459:
	pxor	%xmm2, %xmm1
	movmskps	%xmm1, %eax
	rep bsfl	%eax, %eax
	cltq
	addq	%r15, %rax
	movd	(%r12,%rax,4), %xmm4
	movq	-120(%rbp), %rax
	pshufd	$0, %xmm4, %xmm0
	movaps	%xmm0, (%rax)
	leaq	4(%rbx), %rax
	cmpq	%rax, %r15
	jb	.L385
	.p2align 4,,10
	.p2align 3
.L386:
	movdqa	-64(%rbp), %xmm6
	movq	%rax, %rbx
	movups	%xmm6, -16(%r12,%rax,4)
	leaq	4(%rax), %rax
	cmpq	%r15, %rax
	jbe	.L386
	leaq	0(,%rbx,4), %r9
.L385:
	movq	%r15, %rcx
	subq	%rbx, %rcx
	movd	%ecx, %xmm4
	pshufd	$0, %xmm4, %xmm0
	pcmpgtd	%xmm3, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L387
	movdqa	-64(%rbp), %xmm3
	movd	%xmm3, (%r12,%r9)
.L387:
	pshufd	$85, %xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L388
	pshufd	$85, -64(%rbp), %xmm1
	movd	%xmm1, 4(%r12,%r9)
.L388:
	movdqa	%xmm0, %xmm1
	punpckhdq	%xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L389
	movdqa	-64(%rbp), %xmm3
	movdqa	%xmm3, %xmm1
	punpckhdq	%xmm3, %xmm1
	movd	%xmm1, 8(%r12,%r9)
.L389:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L390
	pshufd	$255, -64(%rbp), %xmm0
	movd	%xmm0, 12(%r12,%r9)
	jmp	.L390
	.cfi_endproc
.LFE18795:
	.size	_ZN3hwy7N_SSSE36detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, .-_ZN3hwy7N_SSSE36detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	.section	.text._ZN3hwy7N_SSSE36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy7N_SSSE36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0, @function
_ZN3hwy7N_SSSE36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0:
.LFB18796:
	.cfi_startproc
	cmpq	%rdx, %rsi
	jbe	.L461
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r10
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rax, %rsi
	jbe	.L461
	movl	(%rdi,%rdx,4), %r11d
	movd	%r11d, %xmm6
	pshufd	$0, %xmm6, %xmm0
	jmp	.L464
	.p2align 4,,10
	.p2align 3
.L465:
	cmpq	%rcx, %rsi
	jbe	.L461
	movq	%rdx, %rax
.L470:
	movd	(%rdi,%r10,8), %xmm5
	pshufd	$0, %xmm5, %xmm1
	pcmpgtd	%xmm3, %xmm1
	movmskps	%xmm1, %r8d
	andl	$1, %r8d
	jne	.L467
.L466:
	cmpq	%rdx, %rax
	je	.L461
	leaq	(%rdi,%rax,4), %rdx
	movl	(%rdx), %ecx
	movl	%ecx, (%r9)
	movl	%r11d, (%rdx)
	cmpq	%rax, %rsi
	jbe	.L474
	movq	%rax, %rdx
.L468:
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r10
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rsi, %rax
	jnb	.L461
.L464:
	movd	(%rdi,%rax,4), %xmm4
	leaq	(%rdi,%rdx,4), %r9
	movdqa	%xmm0, %xmm3
	pshufd	$0, %xmm4, %xmm1
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm0, %xmm2
	movmskps	%xmm2, %r8d
	andl	$1, %r8d
	je	.L465
	cmpq	%rcx, %rsi
	jbe	.L466
	movdqa	%xmm1, %xmm3
	jmp	.L470
	.p2align 4,,10
	.p2align 3
.L467:
	cmpq	%rcx, %rdx
	je	.L475
	leaq	(%rdi,%rcx,4), %rax
	movl	(%rax), %edx
	movl	%edx, (%r9)
	movq	%rcx, %rdx
	movl	%r11d, (%rax)
	jmp	.L468
	.p2align 4,,10
	.p2align 3
.L461:
	ret
	.p2align 4,,10
	.p2align 3
.L474:
	ret
.L475:
	ret
	.cfi_endproc
.LFE18796:
	.size	_ZN3hwy7N_SSSE36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0, .-_ZN3hwy7N_SSSE36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	.section	.text._ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0, @function
_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0:
.LFB18797:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %rax
	salq	$2, %rax
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	.cfi_offset 15, -24
	leaq	(%rdi,%rax), %r15
	pushq	%r14
	.cfi_offset 14, -32
	leaq	(%r15,%rax), %r14
	pushq	%r13
	.cfi_offset 13, -40
	leaq	(%r14,%rax), %r13
	pushq	%r12
	.cfi_offset 12, -48
	leaq	0(%r13,%rax), %r12
	pushq	%rbx
	.cfi_offset 3, -56
	leaq	(%r12,%rax), %rbx
	leaq	(%rbx,%rax), %r11
	leaq	(%r11,%rax), %r10
	subq	$240, %rsp
	leaq	(%r10,%rax), %r9
	leaq	(%r9,%rax), %r8
	movq	%rdi, -264(%rbp)
	movq	%rsi, -240(%rbp)
	movdqu	(%r15), %xmm6
	movdqu	(%rdi), %xmm12
	leaq	(%r8,%rax), %rdi
	leaq	(%rdi,%rax), %rsi
	movdqu	0(%r13), %xmm5
	movdqu	(%r14), %xmm14
	movdqa	%xmm6, %xmm8
	leaq	(%rsi,%rax), %rcx
	movdqu	(%rbx), %xmm3
	movdqu	(%r12), %xmm11
	pcmpgtd	%xmm12, %xmm8
	leaq	(%rcx,%rax), %rdx
	movdqu	(%r10), %xmm2
	movdqu	(%r11), %xmm10
	movdqu	(%rdx), %xmm0
	movdqu	(%rsi), %xmm4
	movq	%rdx, -248(%rbp)
	addq	%rax, %rdx
	movdqu	(%rdx), %xmm15
	movdqu	(%r8), %xmm1
	addq	%rdx, %rax
	movq	%rdx, -256(%rbp)
	movdqa	%xmm8, %xmm13
	movdqu	(%r9), %xmm7
	movdqu	(%rdi), %xmm9
	pandn	%xmm6, %xmm13
	movaps	%xmm15, -112(%rbp)
	pand	%xmm8, %xmm6
	movdqa	%xmm13, %xmm15
	movdqa	%xmm12, %xmm13
	pand	%xmm8, %xmm13
	por	%xmm15, %xmm13
	movdqa	%xmm8, %xmm15
	pandn	%xmm12, %xmm15
	movdqa	%xmm5, %xmm12
	pcmpgtd	%xmm14, %xmm12
	por	%xmm15, %xmm6
	movdqa	%xmm12, %xmm8
	pandn	%xmm5, %xmm8
	pand	%xmm12, %xmm5
	movdqa	%xmm8, %xmm15
	movdqa	%xmm14, %xmm8
	pand	%xmm12, %xmm8
	por	%xmm15, %xmm8
	movdqa	%xmm12, %xmm15
	pandn	%xmm14, %xmm15
	movdqa	%xmm3, %xmm14
	pcmpgtd	%xmm11, %xmm14
	por	%xmm15, %xmm5
	movdqa	%xmm14, %xmm12
	pandn	%xmm3, %xmm12
	pand	%xmm14, %xmm3
	movdqa	%xmm12, %xmm15
	movdqa	%xmm11, %xmm12
	pand	%xmm14, %xmm12
	por	%xmm15, %xmm12
	movdqa	%xmm14, %xmm15
	pandn	%xmm11, %xmm15
	movdqa	%xmm2, %xmm11
	pcmpgtd	%xmm10, %xmm11
	por	%xmm15, %xmm3
	movaps	%xmm3, -64(%rbp)
	movdqa	%xmm10, %xmm3
	movdqa	%xmm11, %xmm14
	pand	%xmm11, %xmm3
	pandn	%xmm2, %xmm14
	pand	%xmm11, %xmm2
	por	%xmm14, %xmm3
	movdqa	%xmm11, %xmm14
	pandn	%xmm10, %xmm14
	movdqa	%xmm1, %xmm10
	pcmpgtd	%xmm7, %xmm10
	por	%xmm14, %xmm2
	movdqa	%xmm10, %xmm11
	pandn	%xmm1, %xmm11
	pand	%xmm10, %xmm1
	movdqa	%xmm11, %xmm14
	movdqa	%xmm7, %xmm11
	pand	%xmm10, %xmm11
	por	%xmm14, %xmm11
	movdqa	%xmm10, %xmm14
	pandn	%xmm7, %xmm14
	movdqa	%xmm4, %xmm7
	pcmpgtd	%xmm9, %xmm7
	por	%xmm14, %xmm1
	movaps	%xmm1, -80(%rbp)
	movdqa	%xmm7, %xmm10
	movdqa	%xmm7, %xmm1
	movdqa	%xmm9, %xmm7
	pandn	%xmm4, %xmm10
	pand	%xmm1, %xmm7
	pand	%xmm1, %xmm4
	por	%xmm10, %xmm7
	movdqa	%xmm1, %xmm10
	movdqa	%xmm0, %xmm1
	pandn	%xmm9, %xmm10
	por	%xmm10, %xmm4
	movdqu	(%rcx), %xmm10
	pcmpgtd	%xmm10, %xmm1
	movdqu	(%rcx), %xmm10
	movdqu	(%rcx), %xmm15
	movdqa	%xmm1, %xmm9
	pand	%xmm1, %xmm10
	pandn	%xmm0, %xmm9
	pand	%xmm1, %xmm0
	por	%xmm9, %xmm10
	movdqa	%xmm1, %xmm9
	movdqu	(%rax), %xmm1
	pandn	%xmm15, %xmm9
	movdqa	-112(%rbp), %xmm15
	por	%xmm9, %xmm0
	pcmpgtd	%xmm15, %xmm1
	movaps	%xmm0, -96(%rbp)
	movdqu	(%rax), %xmm0
	movdqa	%xmm1, %xmm9
	pandn	%xmm0, %xmm9
	movdqa	%xmm15, %xmm0
	pand	%xmm1, %xmm0
	por	%xmm9, %xmm0
	movdqa	%xmm1, %xmm9
	pandn	%xmm15, %xmm9
	movdqu	(%rax), %xmm15
	pand	%xmm15, %xmm1
	movdqa	%xmm13, %xmm15
	por	%xmm9, %xmm1
	movdqa	%xmm8, %xmm9
	pcmpgtd	%xmm13, %xmm9
	movdqa	%xmm9, %xmm14
	pandn	%xmm8, %xmm9
	pand	%xmm14, %xmm15
	pand	%xmm14, %xmm8
	por	%xmm15, %xmm9
	movdqa	%xmm14, %xmm15
	movdqa	%xmm6, %xmm14
	pandn	%xmm13, %xmm15
	movdqa	%xmm5, %xmm13
	pcmpgtd	%xmm6, %xmm13
	por	%xmm8, %xmm15
	movdqa	%xmm13, %xmm8
	pand	%xmm13, %xmm14
	pandn	%xmm5, %xmm8
	pand	%xmm13, %xmm5
	por	%xmm14, %xmm8
	movdqa	%xmm13, %xmm14
	pandn	%xmm6, %xmm14
	movdqa	%xmm3, %xmm6
	pcmpgtd	%xmm12, %xmm6
	por	%xmm14, %xmm5
	movdqa	-64(%rbp), %xmm14
	movaps	%xmm5, -112(%rbp)
	movdqa	%xmm12, %xmm5
	movdqa	%xmm6, %xmm13
	pand	%xmm6, %xmm5
	pandn	%xmm3, %xmm13
	pand	%xmm6, %xmm3
	por	%xmm13, %xmm5
	movdqa	%xmm6, %xmm13
	movdqa	%xmm14, %xmm6
	pandn	%xmm12, %xmm13
	movdqa	%xmm2, %xmm12
	pcmpgtd	%xmm14, %xmm12
	por	%xmm13, %xmm3
	movdqa	%xmm12, %xmm13
	pand	%xmm12, %xmm6
	pandn	%xmm2, %xmm13
	pand	%xmm12, %xmm2
	por	%xmm13, %xmm6
	movdqa	%xmm12, %xmm13
	pandn	%xmm14, %xmm13
	movdqa	%xmm11, %xmm14
	por	%xmm13, %xmm2
	movdqa	%xmm7, %xmm13
	pcmpgtd	%xmm11, %xmm13
	movdqa	%xmm13, %xmm12
	pand	%xmm13, %xmm14
	pandn	%xmm7, %xmm12
	pand	%xmm13, %xmm7
	por	%xmm14, %xmm12
	movdqa	%xmm13, %xmm14
	pandn	%xmm11, %xmm14
	movdqa	%xmm4, %xmm11
	por	%xmm14, %xmm7
	movdqa	-80(%rbp), %xmm14
	movaps	%xmm7, -128(%rbp)
	pcmpgtd	%xmm14, %xmm11
	movdqa	%xmm14, %xmm13
	movdqa	%xmm11, %xmm7
	pand	%xmm11, %xmm13
	pandn	%xmm4, %xmm7
	pand	%xmm11, %xmm4
	por	%xmm13, %xmm7
	movdqa	%xmm11, %xmm13
	movdqa	%xmm0, %xmm11
	pcmpgtd	%xmm10, %xmm11
	pandn	%xmm14, %xmm13
	movdqa	-96(%rbp), %xmm14
	por	%xmm13, %xmm4
	movaps	%xmm4, -80(%rbp)
	movdqa	%xmm10, %xmm4
	movdqa	%xmm11, %xmm13
	pand	%xmm11, %xmm4
	pandn	%xmm0, %xmm13
	pand	%xmm11, %xmm0
	por	%xmm13, %xmm4
	movdqa	%xmm11, %xmm13
	movdqa	%xmm1, %xmm11
	pcmpgtd	%xmm14, %xmm11
	pandn	%xmm10, %xmm13
	movdqa	%xmm14, %xmm10
	por	%xmm13, %xmm0
	movdqa	%xmm11, %xmm13
	pand	%xmm11, %xmm10
	pandn	%xmm1, %xmm13
	pand	%xmm11, %xmm1
	por	%xmm13, %xmm10
	movdqa	%xmm11, %xmm13
	pandn	%xmm14, %xmm13
	movdqa	%xmm9, %xmm14
	por	%xmm13, %xmm1
	movdqa	%xmm5, %xmm13
	pcmpgtd	%xmm9, %xmm13
	movdqa	%xmm13, %xmm11
	pand	%xmm13, %xmm14
	pandn	%xmm5, %xmm11
	pand	%xmm13, %xmm5
	por	%xmm14, %xmm11
	movdqa	%xmm13, %xmm14
	movdqa	%xmm8, %xmm13
	pandn	%xmm9, %xmm14
	movdqa	%xmm6, %xmm9
	pcmpgtd	%xmm8, %xmm9
	por	%xmm5, %xmm14
	movdqa	%xmm9, %xmm5
	pand	%xmm9, %xmm13
	pandn	%xmm6, %xmm5
	pand	%xmm9, %xmm6
	por	%xmm13, %xmm5
	movdqa	%xmm9, %xmm13
	movdqa	%xmm15, %xmm9
	pandn	%xmm8, %xmm13
	movdqa	%xmm3, %xmm8
	pcmpgtd	%xmm15, %xmm8
	por	%xmm13, %xmm6
	movaps	%xmm6, -96(%rbp)
	movdqa	%xmm8, %xmm6
	pand	%xmm8, %xmm9
	pandn	%xmm3, %xmm6
	pand	%xmm8, %xmm3
	por	%xmm9, %xmm6
	movdqa	%xmm8, %xmm9
	movdqa	%xmm2, %xmm8
	pandn	%xmm15, %xmm9
	movdqa	-112(%rbp), %xmm15
	por	%xmm9, %xmm3
	pcmpgtd	%xmm15, %xmm8
	movaps	%xmm3, -144(%rbp)
	movdqa	%xmm15, %xmm9
	movdqa	%xmm8, %xmm3
	pand	%xmm8, %xmm9
	pandn	%xmm2, %xmm3
	pand	%xmm8, %xmm2
	por	%xmm9, %xmm3
	movdqa	%xmm8, %xmm9
	movdqa	%xmm12, %xmm8
	pandn	%xmm15, %xmm9
	movdqa	-128(%rbp), %xmm15
	por	%xmm9, %xmm2
	movaps	%xmm2, -64(%rbp)
	movdqa	%xmm4, %xmm2
	pcmpgtd	%xmm12, %xmm2
	movdqa	%xmm2, %xmm9
	pand	%xmm2, %xmm8
	pandn	%xmm4, %xmm9
	pand	%xmm2, %xmm4
	por	%xmm9, %xmm8
	movdqa	%xmm2, %xmm9
	movdqa	%xmm10, %xmm2
	pcmpgtd	%xmm7, %xmm2
	pandn	%xmm12, %xmm9
	por	%xmm9, %xmm4
	movdqa	%xmm7, %xmm9
	movdqa	%xmm2, %xmm12
	pand	%xmm2, %xmm9
	pandn	%xmm10, %xmm12
	pand	%xmm2, %xmm10
	por	%xmm12, %xmm9
	movdqa	%xmm2, %xmm12
	movdqa	%xmm15, %xmm2
	pandn	%xmm7, %xmm12
	movdqa	%xmm0, %xmm7
	pcmpgtd	%xmm15, %xmm7
	por	%xmm12, %xmm10
	movdqa	%xmm7, %xmm12
	pand	%xmm7, %xmm2
	pandn	%xmm0, %xmm12
	pand	%xmm7, %xmm0
	por	%xmm12, %xmm2
	movdqa	%xmm7, %xmm12
	pandn	%xmm15, %xmm12
	movdqa	-80(%rbp), %xmm15
	por	%xmm12, %xmm0
	movdqa	%xmm1, %xmm12
	pcmpgtd	%xmm15, %xmm12
	movdqa	%xmm12, %xmm7
	pandn	%xmm1, %xmm7
	pand	%xmm12, %xmm1
	movdqa	%xmm7, %xmm13
	movdqa	%xmm15, %xmm7
	pand	%xmm12, %xmm7
	por	%xmm13, %xmm7
	movdqa	%xmm12, %xmm13
	movdqa	%xmm8, %xmm12
	pcmpgtd	%xmm11, %xmm12
	pandn	%xmm15, %xmm13
	por	%xmm13, %xmm1
	movdqa	%xmm12, %xmm15
	pandn	%xmm8, %xmm15
	pand	%xmm12, %xmm8
	movdqa	%xmm15, %xmm13
	movdqa	%xmm11, %xmm15
	pand	%xmm12, %xmm15
	por	%xmm13, %xmm15
	movaps	%xmm15, -112(%rbp)
	movdqa	%xmm12, %xmm15
	movdqa	%xmm5, %xmm12
	pandn	%xmm11, %xmm15
	movdqa	%xmm15, %xmm13
	movdqa	%xmm8, %xmm15
	movdqa	%xmm9, %xmm8
	pcmpgtd	%xmm5, %xmm8
	por	%xmm13, %xmm15
	movdqa	-96(%rbp), %xmm13
	movdqa	%xmm8, %xmm11
	pand	%xmm8, %xmm12
	pandn	%xmm9, %xmm11
	pand	%xmm8, %xmm9
	por	%xmm11, %xmm12
	movdqa	%xmm8, %xmm11
	movdqa	%xmm2, %xmm8
	pcmpgtd	%xmm6, %xmm8
	pandn	%xmm5, %xmm11
	movdqa	%xmm6, %xmm5
	movaps	%xmm12, -128(%rbp)
	por	%xmm11, %xmm9
	movdqa	%xmm8, %xmm11
	pand	%xmm8, %xmm5
	pandn	%xmm2, %xmm11
	pand	%xmm8, %xmm2
	por	%xmm11, %xmm5
	movdqa	%xmm8, %xmm11
	movdqa	%xmm7, %xmm8
	pcmpgtd	%xmm3, %xmm8
	pandn	%xmm6, %xmm11
	movdqa	%xmm3, %xmm6
	movaps	%xmm5, -80(%rbp)
	por	%xmm11, %xmm2
	movdqa	%xmm8, %xmm11
	pand	%xmm8, %xmm6
	pandn	%xmm7, %xmm11
	pand	%xmm8, %xmm7
	por	%xmm11, %xmm6
	movdqa	%xmm8, %xmm11
	movdqa	%xmm4, %xmm8
	pcmpgtd	%xmm14, %xmm8
	pandn	%xmm3, %xmm11
	movdqa	%xmm14, %xmm3
	por	%xmm11, %xmm7
	movdqa	%xmm8, %xmm11
	pand	%xmm8, %xmm3
	pandn	%xmm4, %xmm11
	pand	%xmm8, %xmm4
	por	%xmm11, %xmm3
	movdqa	%xmm8, %xmm11
	movdqa	%xmm13, %xmm8
	pandn	%xmm14, %xmm11
	movdqa	-144(%rbp), %xmm14
	por	%xmm11, %xmm4
	movdqa	%xmm10, %xmm11
	pcmpgtd	%xmm13, %xmm11
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm8
	pandn	%xmm10, %xmm12
	pand	%xmm11, %xmm10
	por	%xmm12, %xmm8
	movdqa	%xmm11, %xmm12
	movdqa	%xmm0, %xmm11
	pcmpgtd	%xmm14, %xmm11
	pandn	%xmm13, %xmm12
	por	%xmm12, %xmm10
	movdqa	%xmm14, %xmm12
	movdqa	%xmm11, %xmm13
	pand	%xmm11, %xmm12
	pandn	%xmm0, %xmm13
	pand	%xmm11, %xmm0
	por	%xmm13, %xmm12
	movdqa	%xmm11, %xmm13
	pandn	%xmm14, %xmm13
	movdqa	-64(%rbp), %xmm14
	por	%xmm13, %xmm0
	movdqa	%xmm1, %xmm13
	pcmpgtd	%xmm14, %xmm13
	movdqa	%xmm13, %xmm11
	movdqa	%xmm13, %xmm5
	pandn	-64(%rbp), %xmm5
	pandn	%xmm1, %xmm11
	pand	%xmm13, %xmm1
	pand	%xmm13, %xmm14
	por	%xmm5, %xmm1
	por	%xmm14, %xmm11
	movdqa	%xmm8, %xmm13
	movaps	%xmm1, -192(%rbp)
	movdqa	%xmm2, %xmm1
	pcmpgtd	%xmm8, %xmm1
	movdqa	%xmm1, %xmm14
	pand	%xmm1, %xmm13
	pandn	%xmm2, %xmm14
	pand	%xmm1, %xmm2
	por	%xmm14, %xmm13
	movdqa	%xmm1, %xmm14
	movdqa	%xmm12, %xmm1
	pandn	%xmm8, %xmm14
	movdqa	%xmm9, %xmm8
	pcmpgtd	%xmm12, %xmm8
	por	%xmm14, %xmm2
	movdqa	%xmm8, %xmm14
	pand	%xmm8, %xmm1
	pandn	%xmm9, %xmm14
	pand	%xmm8, %xmm9
	por	%xmm14, %xmm1
	movdqa	%xmm8, %xmm14
	movdqa	%xmm4, %xmm8
	pcmpgtd	%xmm6, %xmm8
	pandn	%xmm12, %xmm14
	por	%xmm14, %xmm9
	movdqa	%xmm6, %xmm14
	movdqa	%xmm8, %xmm5
	pand	%xmm8, %xmm14
	pandn	%xmm4, %xmm5
	pand	%xmm8, %xmm4
	por	%xmm5, %xmm14
	movdqa	%xmm8, %xmm5
	movdqa	%xmm11, %xmm8
	pandn	%xmm6, %xmm5
	movdqa	%xmm7, %xmm6
	pcmpgtd	%xmm11, %xmm6
	por	%xmm5, %xmm4
	movdqa	%xmm6, %xmm5
	pand	%xmm6, %xmm8
	pandn	%xmm7, %xmm5
	pand	%xmm6, %xmm7
	por	%xmm5, %xmm8
	movdqa	%xmm6, %xmm5
	movdqa	%xmm10, %xmm6
	pandn	%xmm11, %xmm5
	movdqa	%xmm0, %xmm11
	pcmpgtd	%xmm10, %xmm11
	por	%xmm5, %xmm7
	movdqa	%xmm11, %xmm5
	pand	%xmm11, %xmm6
	pandn	%xmm0, %xmm5
	pand	%xmm11, %xmm0
	por	%xmm5, %xmm6
	movdqa	%xmm11, %xmm5
	movdqa	%xmm15, %xmm11
	pcmpgtd	%xmm3, %xmm11
	pandn	%xmm10, %xmm5
	movdqa	%xmm3, %xmm10
	por	%xmm5, %xmm0
	movaps	%xmm0, -64(%rbp)
	movdqa	-128(%rbp), %xmm0
	movdqa	%xmm11, %xmm5
	pand	%xmm11, %xmm10
	pandn	%xmm15, %xmm5
	pand	%xmm11, %xmm15
	por	%xmm5, %xmm10
	movdqa	%xmm11, %xmm5
	pandn	%xmm3, %xmm5
	movdqa	%xmm0, %xmm3
	por	%xmm5, %xmm15
	movdqa	-80(%rbp), %xmm5
	movdqa	%xmm5, %xmm11
	pcmpgtd	%xmm0, %xmm11
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm3
	pandn	%xmm5, %xmm12
	movdqa	%xmm11, %xmm5
	pandn	%xmm0, %xmm5
	por	%xmm12, %xmm3
	movdqa	%xmm5, %xmm12
	movdqa	-80(%rbp), %xmm5
	movdqa	%xmm3, %xmm0
	pand	%xmm11, %xmm5
	movdqa	%xmm10, %xmm11
	pcmpgtd	%xmm3, %xmm11
	por	%xmm12, %xmm5
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm0
	pandn	%xmm10, %xmm12
	pand	%xmm11, %xmm10
	por	%xmm0, %xmm12
	movdqa	-64(%rbp), %xmm0
	movaps	%xmm12, -128(%rbp)
	movdqa	%xmm11, %xmm12
	movdqa	%xmm6, %xmm11
	pcmpgtd	%xmm8, %xmm11
	pandn	%xmm3, %xmm12
	movdqa	%xmm8, %xmm3
	por	%xmm12, %xmm10
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm3
	pandn	%xmm6, %xmm12
	pand	%xmm11, %xmm6
	por	%xmm12, %xmm3
	movdqa	%xmm11, %xmm12
	movdqa	%xmm15, %xmm11
	pcmpgtd	%xmm5, %xmm11
	pandn	%xmm8, %xmm12
	movdqa	%xmm5, %xmm8
	por	%xmm12, %xmm6
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm8
	pandn	%xmm15, %xmm12
	pand	%xmm11, %xmm15
	por	%xmm12, %xmm8
	movdqa	%xmm11, %xmm12
	movdqa	%xmm7, %xmm11
	pandn	%xmm5, %xmm12
	movdqa	%xmm0, %xmm5
	pcmpgtd	%xmm7, %xmm5
	por	%xmm12, %xmm15
	movdqa	%xmm5, %xmm12
	pand	%xmm5, %xmm11
	pandn	%xmm0, %xmm12
	pand	%xmm5, %xmm0
	por	%xmm12, %xmm11
	movdqa	%xmm5, %xmm12
	pandn	%xmm7, %xmm12
	movdqa	%xmm8, %xmm7
	por	%xmm12, %xmm0
	movdqa	%xmm1, %xmm12
	movaps	%xmm0, -208(%rbp)
	movdqa	%xmm10, %xmm0
	pcmpgtd	%xmm13, %xmm12
	cmpq	$1, -240(%rbp)
	pcmpgtd	%xmm8, %xmm0
	movdqa	%xmm0, %xmm5
	pand	%xmm0, %xmm7
	pandn	%xmm10, %xmm5
	pand	%xmm0, %xmm10
	por	%xmm5, %xmm7
	movdqa	%xmm0, %xmm5
	movdqa	%xmm12, %xmm0
	pandn	%xmm8, %xmm5
	pandn	%xmm1, %xmm0
	pand	%xmm12, %xmm1
	movaps	%xmm7, -144(%rbp)
	por	%xmm5, %xmm10
	movdqa	%xmm13, %xmm5
	movdqa	%xmm9, %xmm7
	pand	%xmm12, %xmm5
	movdqa	%xmm11, %xmm8
	por	%xmm0, %xmm5
	movdqa	%xmm12, %xmm0
	pandn	%xmm13, %xmm0
	movdqa	%xmm6, %xmm13
	por	%xmm0, %xmm1
	pcmpgtd	%xmm11, %xmm13
	movdqa	%xmm2, %xmm0
	pcmpgtd	%xmm9, %xmm0
	movdqa	%xmm1, %xmm12
	pand	%xmm13, %xmm8
	movdqa	%xmm0, %xmm1
	pand	%xmm0, %xmm7
	pandn	%xmm2, %xmm1
	pand	%xmm0, %xmm2
	por	%xmm1, %xmm7
	movdqa	%xmm0, %xmm1
	movdqa	%xmm13, %xmm0
	pandn	%xmm9, %xmm1
	pandn	%xmm6, %xmm0
	movdqa	%xmm14, %xmm9
	por	%xmm1, %xmm2
	movdqa	%xmm15, %xmm1
	por	%xmm0, %xmm8
	pcmpgtd	%xmm14, %xmm1
	movdqa	%xmm13, %xmm0
	pand	%xmm6, %xmm13
	pandn	%xmm11, %xmm0
	movdqa	%xmm3, %xmm6
	movdqa	%xmm7, %xmm11
	por	%xmm0, %xmm13
	movdqa	%xmm1, %xmm0
	pand	%xmm1, %xmm9
	pandn	%xmm15, %xmm0
	pand	%xmm1, %xmm15
	por	%xmm0, %xmm9
	movdqa	%xmm1, %xmm0
	pandn	%xmm14, %xmm0
	movdqa	%xmm9, %xmm14
	por	%xmm0, %xmm15
	movdqa	%xmm4, %xmm0
	pcmpgtd	%xmm3, %xmm0
	movdqa	%xmm0, %xmm1
	pand	%xmm0, %xmm6
	pandn	%xmm4, %xmm1
	pand	%xmm0, %xmm4
	por	%xmm1, %xmm6
	movdqa	%xmm0, %xmm1
	movdqa	%xmm5, %xmm0
	pcmpgtd	%xmm9, %xmm0
	pcmpgtd	%xmm6, %xmm11
	pandn	%xmm3, %xmm1
	por	%xmm1, %xmm4
	movdqa	%xmm12, %xmm3
	movdqa	%xmm0, %xmm1
	pand	%xmm0, %xmm14
	pandn	%xmm5, %xmm1
	pand	%xmm0, %xmm5
	por	%xmm1, %xmm14
	movdqa	%xmm0, %xmm1
	pandn	%xmm9, %xmm1
	movdqa	%xmm6, %xmm9
	por	%xmm1, %xmm5
	movdqa	%xmm15, %xmm1
	pand	%xmm11, %xmm9
	pcmpgtd	%xmm12, %xmm1
	movdqa	%xmm1, %xmm0
	pand	%xmm1, %xmm3
	pandn	%xmm15, %xmm0
	por	%xmm0, %xmm3
	movdqa	%xmm1, %xmm0
	pand	%xmm15, %xmm1
	pandn	%xmm12, %xmm0
	por	%xmm0, %xmm1
	movdqa	%xmm11, %xmm0
	pandn	%xmm7, %xmm0
	pand	%xmm11, %xmm7
	por	%xmm0, %xmm9
	movdqa	%xmm11, %xmm0
	pandn	%xmm6, %xmm0
	por	%xmm0, %xmm7
	movdqa	%xmm4, %xmm0
	pcmpgtd	%xmm2, %xmm0
	movdqa	%xmm7, %xmm11
	movdqa	%xmm2, %xmm7
	movdqa	%xmm0, %xmm6
	movdqa	%xmm0, %xmm12
	pand	%xmm0, %xmm7
	pandn	%xmm2, %xmm6
	movdqa	%xmm10, %xmm2
	pand	%xmm4, %xmm0
	pcmpgtd	%xmm14, %xmm2
	por	%xmm6, %xmm0
	pandn	%xmm4, %xmm12
	movdqa	%xmm14, %xmm6
	por	%xmm12, %xmm7
	movdqa	%xmm2, %xmm4
	pand	%xmm2, %xmm6
	pandn	%xmm10, %xmm4
	pand	%xmm2, %xmm10
	por	%xmm4, %xmm6
	movdqa	%xmm2, %xmm4
	movdqa	%xmm3, %xmm2
	pcmpgtd	%xmm5, %xmm2
	pandn	%xmm14, %xmm4
	movaps	%xmm6, -160(%rbp)
	movdqa	%xmm5, %xmm6
	por	%xmm4, %xmm10
	movaps	%xmm10, -64(%rbp)
	movdqa	%xmm8, %xmm10
	movdqa	%xmm2, %xmm4
	pand	%xmm2, %xmm6
	pandn	%xmm3, %xmm4
	pand	%xmm2, %xmm3
	por	%xmm4, %xmm6
	movdqa	%xmm2, %xmm4
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm9, %xmm2
	pandn	%xmm5, %xmm4
	movaps	%xmm6, -80(%rbp)
	movdqa	%xmm9, %xmm5
	por	%xmm4, %xmm3
	movdqa	%xmm7, %xmm6
	pcmpgtd	%xmm11, %xmm6
	movdqa	%xmm2, %xmm4
	pand	%xmm2, %xmm5
	pandn	%xmm1, %xmm4
	pand	%xmm2, %xmm1
	por	%xmm4, %xmm5
	movdqa	%xmm2, %xmm4
	movdqa	%xmm11, %xmm2
	pandn	%xmm9, %xmm4
	pand	%xmm6, %xmm2
	por	%xmm4, %xmm1
	movdqa	%xmm6, %xmm4
	pandn	%xmm7, %xmm4
	pand	%xmm6, %xmm7
	por	%xmm4, %xmm2
	movdqa	%xmm6, %xmm4
	pandn	%xmm11, %xmm4
	movdqa	%xmm2, %xmm9
	por	%xmm4, %xmm7
	pcmpgtd	%xmm1, %xmm9
	movdqa	%xmm0, %xmm4
	pcmpgtd	%xmm8, %xmm4
	movdqa	%xmm4, %xmm6
	pand	%xmm4, %xmm10
	pandn	%xmm0, %xmm6
	pand	%xmm4, %xmm0
	por	%xmm6, %xmm10
	movdqa	%xmm4, %xmm6
	movdqa	%xmm5, %xmm4
	pcmpgtd	%xmm3, %xmm4
	pandn	%xmm8, %xmm6
	movdqa	%xmm3, %xmm8
	por	%xmm6, %xmm0
	movdqa	%xmm4, %xmm6
	pand	%xmm4, %xmm8
	pandn	%xmm5, %xmm6
	pand	%xmm4, %xmm5
	por	%xmm6, %xmm8
	movdqa	%xmm4, %xmm6
	movdqa	%xmm9, %xmm4
	pandn	%xmm3, %xmm6
	movdqa	%xmm1, %xmm3
	pandn	%xmm2, %xmm4
	movaps	%xmm8, -96(%rbp)
	pand	%xmm9, %xmm3
	por	%xmm6, %xmm5
	pand	%xmm9, %xmm2
	por	%xmm4, %xmm3
	movdqa	%xmm9, %xmm4
	movdqa	%xmm5, %xmm12
	pandn	%xmm1, %xmm4
	por	%xmm4, %xmm2
	jbe	.L481
	movdqa	-112(%rbp), %xmm5
	pshufd	$177, %xmm13, %xmm14
	pshufd	$177, -192(%rbp), %xmm13
	movdqa	%xmm13, %xmm8
	pshufd	$177, %xmm3, %xmm6
	pshufd	$177, %xmm0, %xmm0
	pshufd	$177, %xmm7, %xmm7
	pshufd	$177, -208(%rbp), %xmm15
	pcmpgtd	%xmm5, %xmm8
	movaps	%xmm6, -176(%rbp)
	movdqa	%xmm5, %xmm4
	movdqa	%xmm15, %xmm9
	movdqa	-160(%rbp), %xmm3
	pshufd	$177, %xmm10, %xmm10
	pshufd	$177, %xmm2, %xmm2
	movdqa	%xmm8, %xmm6
	movdqa	%xmm8, %xmm1
	pand	%xmm8, %xmm4
	pandn	%xmm5, %xmm6
	movdqa	-128(%rbp), %xmm5
	pandn	%xmm13, %xmm1
	pand	%xmm13, %xmm8
	por	%xmm1, %xmm4
	movaps	%xmm6, -208(%rbp)
	movdqa	-144(%rbp), %xmm6
	pcmpgtd	%xmm5, %xmm9
	movdqa	%xmm5, %xmm11
	movaps	%xmm4, -192(%rbp)
	movdqa	%xmm6, %xmm4
	movdqa	%xmm9, %xmm1
	pand	%xmm9, %xmm11
	pandn	%xmm15, %xmm1
	por	%xmm1, %xmm11
	movdqa	%xmm9, %xmm1
	pand	%xmm15, %xmm9
	pandn	%xmm5, %xmm1
	movdqa	%xmm14, %xmm5
	pcmpgtd	%xmm6, %xmm5
	movaps	%xmm1, -224(%rbp)
	movdqa	%xmm5, %xmm1
	pand	%xmm5, %xmm4
	pandn	%xmm14, %xmm1
	por	%xmm1, %xmm4
	movdqa	%xmm5, %xmm1
	pand	%xmm14, %xmm5
	pandn	%xmm6, %xmm1
	movaps	%xmm4, -112(%rbp)
	movdqa	%xmm3, %xmm6
	movaps	%xmm1, -288(%rbp)
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm3, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm6
	pandn	%xmm0, %xmm4
	por	%xmm4, %xmm6
	movdqa	%xmm1, %xmm4
	pand	%xmm0, %xmm1
	pandn	%xmm3, %xmm4
	movaps	%xmm6, -128(%rbp)
	movdqa	-64(%rbp), %xmm6
	movaps	%xmm4, -304(%rbp)
	movdqa	%xmm10, %xmm4
	por	-304(%rbp), %xmm1
	pcmpgtd	%xmm6, %xmm4
	pshufd	$177, %xmm1, %xmm1
	movdqa	%xmm4, %xmm3
	pandn	%xmm10, %xmm3
	pand	%xmm4, %xmm10
	movaps	%xmm3, -320(%rbp)
	movdqa	%xmm4, %xmm3
	pand	-64(%rbp), %xmm4
	por	-320(%rbp), %xmm4
	pandn	%xmm6, %xmm3
	por	%xmm3, %xmm10
	movdqa	%xmm7, %xmm3
	pshufd	$177, %xmm4, %xmm4
	movaps	%xmm10, -144(%rbp)
	movdqa	-80(%rbp), %xmm10
	pcmpgtd	%xmm10, %xmm3
	movdqa	%xmm3, %xmm6
	pandn	%xmm7, %xmm3
	movaps	%xmm3, -336(%rbp)
	movdqa	%xmm6, %xmm3
	pand	%xmm6, %xmm7
	pand	-80(%rbp), %xmm6
	pandn	%xmm10, %xmm3
	por	%xmm3, %xmm7
	movdqa	%xmm2, %xmm3
	movaps	%xmm7, -160(%rbp)
	movdqa	-96(%rbp), %xmm7
	pcmpgtd	%xmm7, %xmm3
	movdqa	%xmm3, %xmm10
	pandn	%xmm2, %xmm3
	movaps	%xmm3, -352(%rbp)
	movdqa	%xmm10, %xmm3
	pand	%xmm10, %xmm2
	pandn	%xmm7, %xmm3
	movdqa	-176(%rbp), %xmm7
	por	%xmm3, %xmm2
	pcmpgtd	%xmm12, %xmm7
	movdqa	%xmm7, %xmm3
	pandn	-176(%rbp), %xmm3
	movaps	%xmm3, -368(%rbp)
	movdqa	%xmm7, %xmm3
	pandn	%xmm12, %xmm3
	movaps	%xmm3, -384(%rbp)
	movdqa	-176(%rbp), %xmm3
	pand	%xmm7, %xmm3
	pand	%xmm12, %xmm7
	por	-384(%rbp), %xmm3
	por	-336(%rbp), %xmm6
	por	-368(%rbp), %xmm7
	movdqa	-192(%rbp), %xmm13
	pand	-96(%rbp), %xmm10
	pshufd	$177, %xmm7, %xmm7
	pshufd	$177, %xmm6, %xmm6
	por	-352(%rbp), %xmm10
	movdqa	%xmm7, %xmm0
	por	-288(%rbp), %xmm5
	por	-224(%rbp), %xmm9
	pcmpgtd	%xmm13, %xmm0
	pshufd	$177, %xmm10, %xmm14
	por	-208(%rbp), %xmm8
	pshufd	$177, %xmm9, %xmm15
	movdqa	%xmm13, %xmm9
	movaps	%xmm14, -64(%rbp)
	pshufd	$177, %xmm5, %xmm5
	pshufd	$177, %xmm8, %xmm12
	movdqa	%xmm3, %xmm8
	movdqa	%xmm0, %xmm10
	pandn	%xmm7, %xmm0
	pand	%xmm10, %xmm9
	por	%xmm0, %xmm9
	movdqa	%xmm10, %xmm0
	pand	%xmm7, %xmm10
	pandn	%xmm13, %xmm0
	movdqa	%xmm12, %xmm13
	pcmpgtd	%xmm3, %xmm13
	movaps	%xmm0, -80(%rbp)
	movdqa	%xmm13, %xmm0
	pand	%xmm13, %xmm8
	pandn	%xmm12, %xmm0
	por	%xmm0, %xmm8
	movdqa	%xmm13, %xmm0
	pand	%xmm12, %xmm13
	pandn	%xmm3, %xmm0
	movaps	%xmm8, -224(%rbp)
	movdqa	%xmm11, %xmm8
	movaps	%xmm0, -304(%rbp)
	movdqa	%xmm14, %xmm0
	pcmpgtd	%xmm11, %xmm0
	movdqa	%xmm0, %xmm3
	pandn	-64(%rbp), %xmm3
	pand	%xmm0, %xmm8
	por	%xmm3, %xmm8
	movdqa	%xmm0, %xmm3
	pand	-64(%rbp), %xmm0
	pandn	%xmm11, %xmm3
	movdqa	%xmm2, %xmm11
	movaps	%xmm8, -96(%rbp)
	movaps	%xmm3, -320(%rbp)
	movdqa	%xmm15, %xmm3
	por	-320(%rbp), %xmm0
	pcmpgtd	%xmm2, %xmm3
	pshufd	$177, %xmm0, %xmm0
	movdqa	%xmm3, %xmm14
	pand	%xmm3, %xmm11
	pandn	%xmm15, %xmm14
	por	%xmm14, %xmm11
	movdqa	%xmm3, %xmm14
	pand	%xmm15, %xmm3
	movaps	%xmm11, -176(%rbp)
	movdqa	-112(%rbp), %xmm11
	pandn	%xmm2, %xmm14
	movdqa	%xmm6, %xmm2
	movaps	%xmm14, -336(%rbp)
	pcmpgtd	%xmm11, %xmm2
	movdqa	%xmm2, %xmm14
	movdqa	%xmm2, %xmm8
	pandn	%xmm6, %xmm14
	pand	%xmm2, %xmm6
	pandn	%xmm11, %xmm8
	movaps	%xmm14, -352(%rbp)
	movdqa	%xmm6, %xmm14
	movdqa	%xmm5, %xmm6
	pand	-112(%rbp), %xmm2
	por	%xmm8, %xmm14
	por	-352(%rbp), %xmm2
	movdqa	-160(%rbp), %xmm8
	movaps	%xmm14, -192(%rbp)
	pcmpgtd	%xmm8, %xmm6
	pshufd	$177, %xmm2, %xmm2
	movdqa	%xmm6, %xmm14
	pandn	%xmm5, %xmm6
	movaps	%xmm6, -368(%rbp)
	movdqa	%xmm14, %xmm6
	pand	%xmm14, %xmm5
	pandn	%xmm8, %xmm6
	por	%xmm6, %xmm5
	movdqa	-128(%rbp), %xmm6
	movaps	%xmm5, -208(%rbp)
	movdqa	%xmm4, %xmm5
	pcmpgtd	%xmm6, %xmm5
	movdqa	%xmm5, %xmm8
	pandn	%xmm4, %xmm5
	movdqa	%xmm5, %xmm11
	movdqa	%xmm8, %xmm5
	pand	%xmm8, %xmm4
	pandn	%xmm6, %xmm5
	pand	-128(%rbp), %xmm8
	por	%xmm5, %xmm4
	movdqa	%xmm1, %xmm5
	pcmpgtd	-144(%rbp), %xmm5
	movaps	%xmm4, -288(%rbp)
	por	%xmm11, %xmm8
	pshufd	$177, %xmm8, %xmm8
	movdqa	%xmm5, %xmm6
	movdqa	%xmm5, %xmm4
	pandn	-144(%rbp), %xmm4
	pandn	%xmm1, %xmm6
	por	-80(%rbp), %xmm10
	pand	%xmm5, %xmm1
	movdqa	-288(%rbp), %xmm15
	pand	-144(%rbp), %xmm5
	por	%xmm4, %xmm1
	por	-304(%rbp), %xmm13
	pshufd	$177, %xmm10, %xmm11
	movdqa	%xmm15, %xmm4
	por	-336(%rbp), %xmm3
	por	%xmm6, %xmm5
	pshufd	$177, %xmm13, %xmm10
	movdqa	-96(%rbp), %xmm6
	movaps	%xmm11, -128(%rbp)
	pshufd	$177, %xmm5, %xmm7
	movaps	%xmm10, -80(%rbp)
	movdqa	%xmm9, %xmm10
	movdqa	-192(%rbp), %xmm5
	movaps	%xmm7, -64(%rbp)
	movdqa	%xmm8, %xmm7
	pshufd	$177, %xmm3, %xmm3
	pand	-160(%rbp), %xmm14
	por	-368(%rbp), %xmm14
	pcmpgtd	%xmm9, %xmm7
	pshufd	$177, %xmm14, %xmm14
	movdqa	%xmm7, %xmm13
	pand	%xmm7, %xmm10
	pandn	%xmm8, %xmm13
	por	%xmm13, %xmm10
	movdqa	%xmm7, %xmm13
	pand	%xmm8, %xmm7
	pandn	%xmm9, %xmm13
	movdqa	%xmm2, %xmm9
	movdqa	%xmm10, %xmm8
	pcmpgtd	%xmm6, %xmm9
	movaps	%xmm13, -304(%rbp)
	movdqa	%xmm9, %xmm13
	pandn	%xmm2, %xmm9
	movdqa	%xmm13, %xmm12
	pand	%xmm13, %xmm2
	movaps	%xmm9, -320(%rbp)
	pandn	%xmm6, %xmm12
	por	%xmm12, %xmm2
	movdqa	%xmm11, %xmm12
	pcmpgtd	%xmm15, %xmm12
	movdqa	%xmm12, %xmm6
	pandn	-128(%rbp), %xmm12
	pand	%xmm6, %xmm4
	movdqa	%xmm4, %xmm9
	por	%xmm12, %xmm9
	movdqa	%xmm6, %xmm12
	pandn	%xmm15, %xmm12
	movdqa	-224(%rbp), %xmm15
	movaps	%xmm12, -288(%rbp)
	movdqa	%xmm0, %xmm12
	pcmpgtd	%xmm5, %xmm12
	movdqa	%xmm12, %xmm4
	pandn	%xmm0, %xmm4
	pand	%xmm12, %xmm0
	movaps	%xmm4, -336(%rbp)
	movdqa	%xmm12, %xmm4
	pandn	%xmm5, %xmm4
	por	%xmm4, %xmm0
	movdqa	-64(%rbp), %xmm4
	movaps	%xmm0, -144(%rbp)
	movdqa	%xmm4, %xmm11
	pcmpgtd	%xmm15, %xmm11
	movdqa	%xmm11, %xmm5
	pandn	-64(%rbp), %xmm11
	movdqa	%xmm11, %xmm4
	movdqa	%xmm15, %xmm11
	pand	%xmm5, %xmm11
	por	%xmm4, %xmm11
	movdqa	%xmm5, %xmm4
	pandn	%xmm15, %xmm4
	movaps	%xmm11, -160(%rbp)
	movdqa	-176(%rbp), %xmm15
	movdqa	%xmm14, %xmm11
	movaps	%xmm4, -352(%rbp)
	pcmpgtd	%xmm15, %xmm11
	movdqa	%xmm11, %xmm4
	pandn	%xmm14, %xmm4
	pand	%xmm11, %xmm14
	movaps	%xmm4, -368(%rbp)
	movdqa	%xmm11, %xmm4
	pandn	%xmm15, %xmm4
	movdqa	-80(%rbp), %xmm15
	por	%xmm4, %xmm14
	movdqa	%xmm15, %xmm4
	movaps	%xmm14, -112(%rbp)
	movdqa	%xmm1, %xmm15
	pcmpgtd	%xmm1, %xmm4
	movdqa	%xmm4, %xmm14
	pandn	-80(%rbp), %xmm14
	pand	%xmm4, %xmm15
	por	%xmm14, %xmm15
	movdqa	%xmm4, %xmm14
	pandn	%xmm1, %xmm14
	movdqa	-208(%rbp), %xmm1
	movaps	%xmm14, -384(%rbp)
	movdqa	%xmm3, %xmm14
	pcmpgtd	%xmm1, %xmm14
	movdqa	%xmm14, %xmm0
	pandn	%xmm3, %xmm0
	pand	%xmm14, %xmm3
	movaps	%xmm0, -400(%rbp)
	movdqa	%xmm14, %xmm0
	pandn	%xmm1, %xmm0
	por	%xmm0, %xmm3
	movaps	%xmm3, -224(%rbp)
	movdqa	-96(%rbp), %xmm3
	movdqa	-208(%rbp), %xmm1
	pand	-80(%rbp), %xmm4
	pand	-64(%rbp), %xmm5
	pand	%xmm13, %xmm3
	por	-384(%rbp), %xmm4
	por	-320(%rbp), %xmm3
	movdqa	-400(%rbp), %xmm13
	pand	%xmm14, %xmm1
	por	-304(%rbp), %xmm7
	pshufd	$177, %xmm3, %xmm3
	pand	-128(%rbp), %xmm6
	pand	-192(%rbp), %xmm12
	por	%xmm1, %xmm13
	pshufd	$177, %xmm4, %xmm1
	pshufd	$177, %xmm7, %xmm7
	movdqa	-144(%rbp), %xmm4
	movaps	%xmm1, -64(%rbp)
	movdqa	%xmm3, %xmm1
	por	-336(%rbp), %xmm12
	por	-288(%rbp), %xmm6
	pcmpgtd	%xmm10, %xmm1
	por	-352(%rbp), %xmm5
	pand	-176(%rbp), %xmm11
	pshufd	$177, %xmm12, %xmm12
	pshufd	$177, %xmm6, %xmm6
	por	-368(%rbp), %xmm11
	pshufd	$177, %xmm5, %xmm5
	pshufd	$177, %xmm13, %xmm13
	movdqa	%xmm1, %xmm0
	pand	%xmm1, %xmm8
	pshufd	$177, %xmm11, %xmm11
	pandn	%xmm3, %xmm0
	por	%xmm0, %xmm8
	movdqa	%xmm1, %xmm0
	pand	%xmm3, %xmm1
	pandn	%xmm10, %xmm0
	movdqa	%xmm7, %xmm10
	pcmpgtd	%xmm2, %xmm10
	por	%xmm0, %xmm1
	movdqa	%xmm2, %xmm0
	movdqa	%xmm10, %xmm3
	pand	%xmm10, %xmm0
	pandn	%xmm7, %xmm3
	por	%xmm0, %xmm3
	movdqa	%xmm10, %xmm0
	pand	%xmm7, %xmm10
	pandn	%xmm2, %xmm0
	movdqa	%xmm12, %xmm2
	pcmpgtd	%xmm9, %xmm2
	movdqa	%xmm0, %xmm14
	movdqa	%xmm4, %xmm0
	por	%xmm10, %xmm14
	movdqa	%xmm9, %xmm10
	movdqa	%xmm2, %xmm7
	pand	%xmm2, %xmm10
	pandn	%xmm12, %xmm7
	por	%xmm7, %xmm10
	movdqa	%xmm2, %xmm7
	pand	%xmm12, %xmm2
	pandn	%xmm9, %xmm7
	por	%xmm7, %xmm2
	movdqa	%xmm2, %xmm12
	movdqa	%xmm6, %xmm2
	pcmpgtd	%xmm4, %xmm2
	pand	%xmm2, %xmm0
	movdqa	%xmm2, %xmm7
	pandn	%xmm6, %xmm7
	movdqa	%xmm0, %xmm9
	movdqa	%xmm11, %xmm0
	por	%xmm7, %xmm9
	movdqa	%xmm2, %xmm7
	pand	%xmm6, %xmm2
	pandn	%xmm4, %xmm7
	movdqa	-160(%rbp), %xmm4
	por	%xmm2, %xmm7
	pcmpgtd	%xmm4, %xmm0
	movdqa	%xmm4, %xmm6
	movdqa	%xmm0, %xmm2
	pand	%xmm0, %xmm6
	pandn	%xmm11, %xmm2
	por	%xmm2, %xmm6
	movdqa	%xmm0, %xmm2
	pand	%xmm11, %xmm0
	pandn	%xmm4, %xmm2
	movaps	%xmm6, -288(%rbp)
	movdqa	-112(%rbp), %xmm6
	por	%xmm2, %xmm0
	movdqa	%xmm0, %xmm11
	movdqa	%xmm5, %xmm0
	pcmpgtd	%xmm6, %xmm0
	movdqa	%xmm0, %xmm2
	pand	%xmm0, %xmm6
	movdqa	%xmm0, %xmm4
	pandn	%xmm5, %xmm2
	pand	%xmm5, %xmm0
	movdqa	%xmm13, %xmm5
	pcmpgtd	%xmm15, %xmm5
	pandn	-112(%rbp), %xmm4
	por	%xmm2, %xmm6
	por	%xmm4, %xmm0
	movaps	%xmm0, -304(%rbp)
	movdqa	%xmm5, %xmm2
	movdqa	%xmm5, %xmm0
	movdqa	%xmm15, %xmm5
	pandn	%xmm13, %xmm2
	pand	%xmm0, %xmm5
	por	%xmm2, %xmm5
	movdqa	%xmm0, %xmm2
	pand	%xmm13, %xmm0
	movaps	%xmm5, -320(%rbp)
	movdqa	-64(%rbp), %xmm5
	pandn	%xmm15, %xmm2
	movdqa	-224(%rbp), %xmm15
	movdqa	%xmm0, %xmm13
	movdqa	%xmm5, %xmm4
	por	%xmm2, %xmm13
	pcmpgtd	%xmm15, %xmm4
	movdqa	%xmm4, %xmm2
	movdqa	%xmm4, %xmm0
	movdqa	%xmm5, %xmm4
	pandn	%xmm5, %xmm2
	movdqa	%xmm15, %xmm5
	pand	%xmm0, %xmm5
	por	%xmm2, %xmm5
	movdqa	%xmm0, %xmm2
	pand	%xmm4, %xmm0
	pandn	%xmm15, %xmm2
	movdqa	%xmm0, %xmm15
	pshufd	$177, %xmm8, %xmm0
	movaps	%xmm5, -336(%rbp)
	por	%xmm2, %xmm15
	movdqa	%xmm0, %xmm2
	pcmpgtd	%xmm8, %xmm2
	movdqa	%xmm2, %xmm4
	movdqa	%xmm2, %xmm5
	pandn	%xmm0, %xmm4
	pandn	%xmm8, %xmm5
	pand	%xmm2, %xmm0
	pand	%xmm2, %xmm8
	por	%xmm5, %xmm0
	por	%xmm4, %xmm8
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm8, %xmm8
	punpckldq	%xmm0, %xmm8
	pshufd	$177, %xmm1, %xmm0
	movdqa	%xmm0, %xmm2
	movaps	%xmm8, -352(%rbp)
	pcmpgtd	%xmm1, %xmm2
	movaps	%xmm8, -112(%rbp)
	movdqa	%xmm2, %xmm4
	movdqa	%xmm2, %xmm8
	pandn	%xmm0, %xmm4
	pandn	%xmm1, %xmm8
	pand	%xmm2, %xmm0
	pand	%xmm2, %xmm1
	por	%xmm8, %xmm0
	por	%xmm4, %xmm1
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm1, %xmm1
	punpckldq	%xmm0, %xmm1
	pshufd	$177, %xmm3, %xmm0
	movaps	%xmm1, -368(%rbp)
	movaps	%xmm1, -128(%rbp)
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm3, %xmm1
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm4
	pandn	%xmm0, %xmm2
	pandn	%xmm3, %xmm4
	pand	%xmm1, %xmm0
	pand	%xmm1, %xmm3
	por	%xmm4, %xmm0
	por	%xmm2, %xmm3
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm3, %xmm3
	punpckldq	%xmm0, %xmm3
	pshufd	$177, %xmm14, %xmm0
	movdqa	%xmm0, %xmm1
	movaps	%xmm3, -384(%rbp)
	pcmpgtd	%xmm14, %xmm1
	movaps	%xmm3, -144(%rbp)
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm14, %xmm3
	pand	%xmm1, %xmm0
	pand	%xmm1, %xmm14
	por	%xmm3, %xmm0
	por	%xmm2, %xmm14
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm14, %xmm14
	punpckldq	%xmm0, %xmm14
	pshufd	$177, %xmm10, %xmm0
	movdqa	%xmm0, %xmm1
	movaps	%xmm14, -160(%rbp)
	cmpq	$3, -240(%rbp)
	pcmpgtd	%xmm10, %xmm1
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm10, %xmm3
	pand	%xmm1, %xmm0
	pand	%xmm1, %xmm10
	por	%xmm3, %xmm0
	por	%xmm2, %xmm10
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm10, %xmm10
	punpckldq	%xmm0, %xmm10
	pshufd	$177, %xmm12, %xmm0
	movdqa	%xmm0, %xmm1
	movaps	%xmm10, -176(%rbp)
	pcmpgtd	%xmm12, %xmm1
	movaps	%xmm10, -64(%rbp)
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm12, %xmm3
	pand	%xmm1, %xmm0
	pand	%xmm1, %xmm12
	por	%xmm3, %xmm0
	por	%xmm2, %xmm12
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm12, %xmm12
	punpckldq	%xmm0, %xmm12
	pshufd	$177, %xmm9, %xmm0
	movdqa	%xmm0, %xmm1
	movaps	%xmm12, -192(%rbp)
	pcmpgtd	%xmm9, %xmm1
	movaps	%xmm12, -80(%rbp)
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm9, %xmm3
	pand	%xmm1, %xmm0
	pand	%xmm1, %xmm9
	por	%xmm3, %xmm0
	por	%xmm2, %xmm9
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm9, %xmm9
	punpckldq	%xmm0, %xmm9
	pshufd	$177, %xmm7, %xmm0
	movdqa	%xmm0, %xmm1
	movaps	%xmm9, -208(%rbp)
	pcmpgtd	%xmm7, %xmm1
	movaps	%xmm9, -96(%rbp)
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm7, %xmm3
	pand	%xmm1, %xmm7
	por	%xmm2, %xmm7
	pand	%xmm1, %xmm0
	pshufd	$136, %xmm7, %xmm7
	por	%xmm3, %xmm0
	movdqa	%xmm7, %xmm3
	movdqa	-288(%rbp), %xmm7
	pshufd	$221, %xmm0, %xmm0
	punpckldq	%xmm0, %xmm3
	pshufd	$177, %xmm7, %xmm0
	movaps	%xmm3, -224(%rbp)
	movdqa	%xmm3, %xmm12
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm7, %xmm1
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm7, %xmm3
	pand	%xmm1, %xmm0
	pand	%xmm7, %xmm1
	por	%xmm3, %xmm0
	por	%xmm2, %xmm1
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm1, %xmm1
	punpckldq	%xmm0, %xmm1
	pshufd	$177, %xmm11, %xmm0
	movdqa	%xmm1, %xmm8
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm11, %xmm1
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm11, %xmm3
	pand	%xmm1, %xmm11
	pand	%xmm1, %xmm0
	por	%xmm2, %xmm11
	por	%xmm3, %xmm0
	pshufd	$136, %xmm11, %xmm11
	pshufd	$221, %xmm0, %xmm0
	movdqa	%xmm11, %xmm7
	punpckldq	%xmm0, %xmm7
	pshufd	$177, %xmm6, %xmm0
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm6, %xmm1
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm6, %xmm3
	pand	%xmm1, %xmm6
	por	%xmm2, %xmm6
	pand	%xmm1, %xmm0
	pshufd	$136, %xmm6, %xmm6
	por	%xmm3, %xmm0
	movdqa	%xmm6, %xmm10
	movdqa	-304(%rbp), %xmm6
	pshufd	$221, %xmm0, %xmm0
	punpckldq	%xmm0, %xmm10
	pshufd	$177, %xmm6, %xmm0
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm6, %xmm1
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm2
	pandn	%xmm6, %xmm3
	pand	%xmm1, %xmm0
	pand	%xmm6, %xmm1
	por	%xmm3, %xmm0
	movdqa	-320(%rbp), %xmm6
	por	%xmm2, %xmm1
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm1, %xmm1
	punpckldq	%xmm0, %xmm1
	movdqa	%xmm1, %xmm5
	pshufd	$177, %xmm6, %xmm1
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm6, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm3
	pandn	%xmm1, %xmm2
	pandn	%xmm6, %xmm3
	pand	%xmm0, %xmm1
	pand	%xmm6, %xmm0
	por	%xmm3, %xmm1
	movdqa	-336(%rbp), %xmm6
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm13, %xmm1
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm13, %xmm2
	movdqa	%xmm2, %xmm3
	movdqa	%xmm2, %xmm4
	pandn	%xmm1, %xmm3
	pandn	%xmm13, %xmm4
	pand	%xmm2, %xmm1
	pand	%xmm2, %xmm13
	por	%xmm4, %xmm1
	pshufd	$177, %xmm6, %xmm2
	por	%xmm3, %xmm13
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm13, %xmm13
	punpckldq	%xmm1, %xmm13
	movdqa	%xmm2, %xmm1
	pcmpgtd	%xmm6, %xmm1
	movdqa	%xmm1, %xmm3
	movdqa	%xmm1, %xmm4
	pandn	%xmm2, %xmm3
	pandn	%xmm6, %xmm4
	pand	%xmm1, %xmm2
	pand	%xmm6, %xmm1
	por	%xmm4, %xmm2
	por	%xmm3, %xmm1
	pshufd	$221, %xmm2, %xmm2
	pshufd	$177, %xmm15, %xmm3
	pshufd	$136, %xmm1, %xmm1
	punpckldq	%xmm2, %xmm1
	movdqa	%xmm3, %xmm2
	pcmpgtd	%xmm15, %xmm2
	movdqa	%xmm2, %xmm4
	movdqa	%xmm2, %xmm6
	pandn	%xmm3, %xmm4
	pandn	%xmm15, %xmm6
	pand	%xmm2, %xmm3
	pand	%xmm2, %xmm15
	por	%xmm6, %xmm3
	por	%xmm4, %xmm15
	pshufd	$221, %xmm3, %xmm3
	pshufd	$136, %xmm15, %xmm2
	punpckldq	%xmm3, %xmm2
	jbe	.L482
	pshufd	$27, %xmm2, %xmm2
	pshufd	$27, %xmm13, %xmm3
	pshufd	$27, %xmm1, %xmm4
	movdqa	-352(%rbp), %xmm15
	movdqa	%xmm2, %xmm9
	pshufd	$27, %xmm10, %xmm6
	movdqa	%xmm4, %xmm10
	movaps	%xmm4, -144(%rbp)
	pcmpgtd	%xmm15, %xmm9
	movdqa	%xmm15, %xmm13
	movdqa	%xmm3, %xmm4
	movaps	%xmm3, -64(%rbp)
	pshufd	$27, %xmm0, %xmm0
	pshufd	$27, %xmm5, %xmm5
	pshufd	$27, %xmm7, %xmm7
	pshufd	$27, %xmm8, %xmm8
	movdqa	%xmm9, %xmm1
	pand	%xmm9, %xmm13
	pandn	%xmm2, %xmm1
	por	%xmm1, %xmm13
	movdqa	%xmm9, %xmm1
	pand	%xmm2, %xmm9
	pandn	%xmm15, %xmm1
	movdqa	-368(%rbp), %xmm15
	movaps	%xmm1, -240(%rbp)
	pcmpgtd	%xmm15, %xmm10
	movdqa	%xmm15, %xmm12
	movdqa	%xmm10, %xmm11
	movdqa	%xmm10, %xmm1
	pand	%xmm10, %xmm12
	pandn	%xmm15, %xmm11
	pandn	-144(%rbp), %xmm1
	movaps	%xmm11, -288(%rbp)
	movdqa	-384(%rbp), %xmm11
	por	%xmm1, %xmm12
	pcmpgtd	%xmm11, %xmm4
	movdqa	%xmm11, %xmm15
	movdqa	%xmm4, %xmm1
	pandn	-64(%rbp), %xmm1
	pand	%xmm4, %xmm15
	por	%xmm1, %xmm15
	movdqa	%xmm4, %xmm1
	pandn	%xmm11, %xmm1
	movaps	%xmm15, -80(%rbp)
	movaps	%xmm1, -304(%rbp)
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm14, %xmm1
	movdqa	%xmm1, %xmm15
	movdqa	%xmm1, %xmm11
	pandn	%xmm0, %xmm15
	pand	%xmm14, %xmm11
	por	%xmm15, %xmm11
	movdqa	%xmm1, %xmm15
	pand	%xmm0, %xmm1
	pandn	%xmm14, %xmm15
	movdqa	%xmm5, %xmm14
	movaps	%xmm11, -96(%rbp)
	movaps	%xmm15, -320(%rbp)
	por	-320(%rbp), %xmm1
	movdqa	-176(%rbp), %xmm15
	pcmpgtd	%xmm15, %xmm14
	pshufd	$27, %xmm1, %xmm1
	movdqa	-192(%rbp), %xmm15
	movdqa	%xmm14, %xmm3
	pandn	%xmm5, %xmm14
	movdqa	%xmm3, %xmm11
	pand	%xmm3, %xmm5
	pandn	-176(%rbp), %xmm11
	movaps	%xmm14, -336(%rbp)
	pand	-176(%rbp), %xmm3
	por	-336(%rbp), %xmm3
	por	%xmm11, %xmm5
	movaps	%xmm5, -112(%rbp)
	movdqa	%xmm6, %xmm5
	pcmpgtd	%xmm15, %xmm5
	movdqa	-224(%rbp), %xmm15
	movdqa	%xmm5, %xmm11
	pandn	%xmm6, %xmm11
	pand	%xmm5, %xmm6
	movaps	%xmm11, -352(%rbp)
	movdqa	%xmm5, %xmm11
	pandn	-192(%rbp), %xmm11
	pand	-192(%rbp), %xmm5
	por	-352(%rbp), %xmm5
	por	%xmm11, %xmm6
	movdqa	%xmm7, %xmm11
	movaps	%xmm6, -128(%rbp)
	movdqa	-208(%rbp), %xmm6
	pshufd	$27, %xmm5, %xmm5
	pcmpgtd	%xmm6, %xmm11
	movdqa	%xmm8, %xmm6
	pcmpgtd	%xmm15, %xmm6
	pshufd	$27, %xmm3, %xmm15
	movdqa	%xmm11, %xmm14
	pandn	%xmm7, %xmm14
	pand	%xmm11, %xmm7
	movaps	%xmm14, -368(%rbp)
	movdqa	%xmm11, %xmm14
	pandn	-208(%rbp), %xmm14
	por	%xmm14, %xmm7
	movdqa	%xmm6, %xmm14
	pandn	%xmm8, %xmm14
	movaps	%xmm7, -160(%rbp)
	pand	%xmm6, %xmm8
	movdqa	%xmm6, %xmm7
	pandn	-224(%rbp), %xmm7
	pand	-64(%rbp), %xmm4
	pand	-224(%rbp), %xmm6
	pand	-144(%rbp), %xmm10
	por	-288(%rbp), %xmm10
	por	%xmm7, %xmm8
	por	-240(%rbp), %xmm9
	por	%xmm14, %xmm6
	por	-304(%rbp), %xmm4
	pand	-208(%rbp), %xmm11
	pshufd	$27, %xmm6, %xmm6
	pshufd	$27, %xmm10, %xmm3
	pshufd	$27, %xmm9, %xmm2
	movdqa	-160(%rbp), %xmm14
	movdqa	%xmm6, %xmm10
	movdqa	%xmm13, %xmm9
	movaps	%xmm2, -208(%rbp)
	por	-368(%rbp), %xmm11
	pcmpgtd	%xmm13, %xmm10
	movaps	%xmm3, -64(%rbp)
	pshufd	$27, %xmm4, %xmm4
	pshufd	$27, %xmm11, %xmm11
	movdqa	%xmm10, %xmm7
	movdqa	%xmm10, %xmm0
	pand	%xmm10, %xmm9
	pandn	%xmm13, %xmm7
	movdqa	%xmm2, %xmm13
	pandn	%xmm6, %xmm0
	pcmpgtd	%xmm8, %xmm13
	por	%xmm0, %xmm9
	movdqa	%xmm8, %xmm2
	movaps	%xmm7, -288(%rbp)
	movdqa	%xmm14, %xmm7
	movaps	%xmm9, -224(%rbp)
	pand	%xmm6, %xmm10
	movdqa	%xmm13, %xmm0
	pandn	-208(%rbp), %xmm0
	pand	%xmm13, %xmm2
	por	%xmm0, %xmm2
	movdqa	%xmm13, %xmm0
	pandn	%xmm8, %xmm0
	movaps	%xmm2, -240(%rbp)
	movaps	%xmm0, -304(%rbp)
	movdqa	%xmm11, %xmm0
	pcmpgtd	%xmm12, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm8
	pandn	%xmm11, %xmm3
	pandn	%xmm12, %xmm8
	movdqa	%xmm3, %xmm2
	movdqa	%xmm12, %xmm3
	movdqa	-64(%rbp), %xmm12
	movaps	%xmm8, -320(%rbp)
	pand	%xmm0, %xmm3
	movdqa	-128(%rbp), %xmm8
	pand	%xmm11, %xmm0
	por	%xmm2, %xmm3
	movaps	%xmm3, -144(%rbp)
	movdqa	%xmm12, %xmm3
	pcmpgtd	%xmm14, %xmm3
	pand	%xmm3, %xmm7
	movdqa	%xmm3, %xmm2
	pandn	-64(%rbp), %xmm2
	movdqa	%xmm7, %xmm12
	movdqa	%xmm3, %xmm7
	pandn	%xmm14, %xmm7
	movdqa	-80(%rbp), %xmm14
	por	%xmm2, %xmm12
	movdqa	%xmm5, %xmm2
	movaps	%xmm12, -160(%rbp)
	pcmpgtd	%xmm14, %xmm2
	movaps	%xmm7, -336(%rbp)
	movdqa	%xmm2, %xmm12
	movdqa	%xmm2, %xmm7
	pandn	%xmm5, %xmm12
	pandn	%xmm14, %xmm7
	pand	%xmm2, %xmm5
	por	%xmm7, %xmm5
	movdqa	%xmm4, %xmm7
	pand	-80(%rbp), %xmm2
	pcmpgtd	%xmm8, %xmm7
	movaps	%xmm5, -176(%rbp)
	por	%xmm12, %xmm2
	pshufd	$27, %xmm2, %xmm2
	movdqa	%xmm7, %xmm5
	pandn	%xmm4, %xmm5
	pand	%xmm7, %xmm4
	movaps	%xmm5, -352(%rbp)
	movdqa	%xmm7, %xmm5
	movdqa	%xmm4, %xmm14
	movdqa	%xmm15, %xmm4
	pandn	%xmm8, %xmm5
	por	%xmm5, %xmm14
	movdqa	-96(%rbp), %xmm5
	movaps	%xmm14, -192(%rbp)
	pcmpgtd	%xmm5, %xmm4
	movdqa	%xmm4, %xmm8
	pandn	%xmm15, %xmm4
	movdqa	%xmm8, %xmm14
	pand	%xmm8, %xmm15
	pand	-96(%rbp), %xmm8
	pandn	%xmm5, %xmm14
	movdqa	%xmm1, %xmm5
	por	%xmm14, %xmm15
	movdqa	-112(%rbp), %xmm14
	por	%xmm4, %xmm8
	pshufd	$27, %xmm8, %xmm8
	pcmpgtd	%xmm14, %xmm5
	movdqa	%xmm5, %xmm9
	pandn	%xmm1, %xmm9
	pand	%xmm5, %xmm1
	movaps	%xmm9, -368(%rbp)
	por	-320(%rbp), %xmm0
	movdqa	%xmm5, %xmm9
	movdqa	-128(%rbp), %xmm4
	pand	-112(%rbp), %xmm5
	pandn	%xmm14, %xmm9
	pand	-64(%rbp), %xmm3
	pand	%xmm7, %xmm4
	por	-368(%rbp), %xmm5
	pand	-208(%rbp), %xmm13
	por	-304(%rbp), %xmm13
	por	%xmm9, %xmm1
	movdqa	-224(%rbp), %xmm6
	pshufd	$27, %xmm0, %xmm0
	pshufd	$27, %xmm5, %xmm7
	por	-336(%rbp), %xmm3
	por	-288(%rbp), %xmm10
	movdqa	%xmm7, %xmm14
	pshufd	$27, %xmm13, %xmm7
	movdqa	%xmm2, %xmm13
	movaps	%xmm7, -64(%rbp)
	movdqa	%xmm8, %xmm7
	pshufd	$27, %xmm10, %xmm10
	por	-352(%rbp), %xmm4
	pcmpgtd	%xmm6, %xmm7
	movdqa	%xmm10, %xmm11
	movdqa	%xmm6, %xmm10
	movaps	%xmm14, -224(%rbp)
	movaps	%xmm11, -208(%rbp)
	pshufd	$27, %xmm4, %xmm4
	pshufd	$27, %xmm3, %xmm3
	movdqa	%xmm7, %xmm9
	movdqa	%xmm7, %xmm5
	pand	%xmm7, %xmm10
	pandn	%xmm6, %xmm9
	pandn	%xmm8, %xmm5
	pand	%xmm8, %xmm7
	movdqa	-144(%rbp), %xmm6
	por	%xmm5, %xmm10
	movaps	%xmm9, -288(%rbp)
	movdqa	%xmm15, %xmm9
	pcmpgtd	%xmm6, %xmm13
	movdqa	%xmm13, %xmm5
	pandn	%xmm2, %xmm5
	pand	%xmm13, %xmm2
	movaps	%xmm5, -304(%rbp)
	movdqa	%xmm13, %xmm5
	pandn	%xmm6, %xmm5
	por	%xmm5, %xmm2
	movdqa	%xmm11, %xmm5
	movdqa	-176(%rbp), %xmm11
	pcmpgtd	%xmm15, %xmm5
	movdqa	%xmm5, %xmm6
	pandn	-208(%rbp), %xmm5
	movdqa	%xmm6, %xmm12
	pand	%xmm6, %xmm9
	pandn	%xmm15, %xmm12
	por	%xmm5, %xmm9
	movdqa	-240(%rbp), %xmm15
	movaps	%xmm12, -320(%rbp)
	movdqa	%xmm0, %xmm12
	pcmpgtd	%xmm11, %xmm12
	movdqa	%xmm12, %xmm5
	pandn	%xmm0, %xmm5
	pand	%xmm12, %xmm0
	movaps	%xmm5, -336(%rbp)
	movdqa	%xmm12, %xmm5
	pandn	%xmm11, %xmm5
	por	%xmm5, %xmm0
	movdqa	%xmm14, %xmm5
	movdqa	%xmm15, %xmm14
	pcmpgtd	%xmm15, %xmm5
	movaps	%xmm0, -80(%rbp)
	movdqa	%xmm5, %xmm11
	pand	%xmm5, %xmm14
	pandn	-224(%rbp), %xmm11
	por	%xmm11, %xmm14
	movdqa	%xmm5, %xmm11
	pandn	%xmm15, %xmm11
	movaps	%xmm14, -96(%rbp)
	movdqa	-160(%rbp), %xmm15
	movaps	%xmm11, -240(%rbp)
	movdqa	%xmm4, %xmm11
	pcmpgtd	%xmm15, %xmm11
	movdqa	%xmm11, %xmm14
	pandn	%xmm4, %xmm14
	pand	%xmm11, %xmm4
	movaps	%xmm14, -352(%rbp)
	movdqa	%xmm11, %xmm14
	pandn	%xmm15, %xmm14
	movdqa	-64(%rbp), %xmm15
	por	%xmm14, %xmm4
	movaps	%xmm4, -112(%rbp)
	movdqa	%xmm15, %xmm4
	movdqa	%xmm1, %xmm15
	pcmpgtd	%xmm1, %xmm4
	movdqa	%xmm4, %xmm14
	pandn	-64(%rbp), %xmm14
	pand	%xmm4, %xmm15
	por	%xmm14, %xmm15
	movdqa	%xmm4, %xmm14
	pandn	%xmm1, %xmm14
	movdqa	%xmm3, %xmm1
	movaps	%xmm14, -368(%rbp)
	movdqa	-192(%rbp), %xmm14
	por	-288(%rbp), %xmm7
	movdqa	-160(%rbp), %xmm8
	pand	-208(%rbp), %xmm6
	pcmpgtd	%xmm14, %xmm1
	pshufd	$27, %xmm7, %xmm7
	pand	-176(%rbp), %xmm12
	pand	%xmm11, %xmm8
	por	-320(%rbp), %xmm6
	movdqa	-192(%rbp), %xmm11
	por	-336(%rbp), %xmm12
	pand	-224(%rbp), %xmm5
	movdqa	%xmm1, %xmm0
	pand	%xmm1, %xmm11
	pshufd	$27, %xmm6, %xmm6
	pandn	%xmm3, %xmm0
	pand	%xmm1, %xmm3
	pshufd	$27, %xmm12, %xmm12
	movaps	%xmm0, -384(%rbp)
	movdqa	%xmm1, %xmm0
	pand	-64(%rbp), %xmm4
	por	-352(%rbp), %xmm8
	pandn	%xmm14, %xmm0
	por	-240(%rbp), %xmm5
	por	-384(%rbp), %xmm11
	por	%xmm0, %xmm3
	pshufd	$27, %xmm8, %xmm8
	por	-368(%rbp), %xmm4
	movaps	%xmm3, -128(%rbp)
	pshufd	$27, %xmm5, %xmm5
	pshufd	$27, %xmm11, %xmm11
	movdqa	-144(%rbp), %xmm3
	pshufd	$27, %xmm4, %xmm4
	pand	%xmm13, %xmm3
	por	-304(%rbp), %xmm3
	movdqa	%xmm10, %xmm13
	pshufd	$27, %xmm3, %xmm3
	movdqa	%xmm3, %xmm1
	pcmpgtd	%xmm10, %xmm1
	movdqa	%xmm1, %xmm0
	pand	%xmm1, %xmm13
	pandn	%xmm3, %xmm0
	pand	%xmm1, %xmm3
	por	%xmm0, %xmm13
	movdqa	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm2, %xmm1
	pandn	%xmm10, %xmm0
	movdqa	%xmm2, %xmm10
	por	%xmm0, %xmm3
	movdqa	%xmm1, %xmm0
	pand	%xmm1, %xmm10
	pandn	%xmm7, %xmm0
	por	%xmm0, %xmm10
	movdqa	%xmm1, %xmm0
	pandn	%xmm2, %xmm0
	movdqa	%xmm1, %xmm2
	movdqa	%xmm12, %xmm1
	pcmpgtd	%xmm9, %xmm1
	pand	%xmm7, %xmm2
	por	%xmm0, %xmm2
	movdqa	%xmm1, %xmm7
	movdqa	%xmm1, %xmm0
	pandn	%xmm12, %xmm7
	pandn	%xmm9, %xmm0
	movdqa	%xmm7, %xmm14
	movdqa	%xmm9, %xmm7
	movdqa	%xmm1, %xmm9
	pand	%xmm12, %xmm9
	movdqa	-80(%rbp), %xmm12
	pand	%xmm1, %xmm7
	movdqa	%xmm6, %xmm1
	por	%xmm0, %xmm9
	por	%xmm14, %xmm7
	pcmpgtd	%xmm12, %xmm1
	movdqa	%xmm1, %xmm0
	pandn	%xmm6, %xmm0
	movdqa	%xmm0, %xmm14
	movdqa	%xmm12, %xmm0
	pand	%xmm1, %xmm0
	movdqa	%xmm0, %xmm12
	movdqa	%xmm1, %xmm0
	pandn	-80(%rbp), %xmm0
	por	%xmm14, %xmm12
	movdqa	%xmm0, %xmm14
	movdqa	%xmm1, %xmm0
	movdqa	%xmm8, %xmm1
	pand	%xmm6, %xmm0
	por	%xmm14, %xmm0
	movdqa	-96(%rbp), %xmm14
	movaps	%xmm0, -64(%rbp)
	pcmpgtd	%xmm14, %xmm1
	movdqa	%xmm1, %xmm6
	pand	%xmm1, %xmm14
	movdqa	%xmm1, %xmm0
	pandn	%xmm8, %xmm6
	pand	%xmm8, %xmm1
	movdqa	-112(%rbp), %xmm8
	pandn	-96(%rbp), %xmm0
	por	%xmm6, %xmm14
	movdqa	%xmm5, %xmm6
	pcmpgtd	%xmm8, %xmm6
	por	%xmm0, %xmm1
	movaps	%xmm1, -80(%rbp)
	movdqa	%xmm6, %xmm1
	pandn	%xmm5, %xmm6
	pand	%xmm1, %xmm8
	movdqa	%xmm1, %xmm0
	pand	%xmm5, %xmm1
	por	%xmm6, %xmm8
	movdqa	%xmm11, %xmm6
	pandn	-112(%rbp), %xmm0
	pcmpgtd	%xmm15, %xmm6
	movdqa	%xmm1, %xmm5
	por	%xmm0, %xmm5
	movaps	%xmm5, -96(%rbp)
	movdqa	%xmm6, %xmm1
	movdqa	%xmm6, %xmm5
	movdqa	%xmm15, %xmm6
	pandn	%xmm11, %xmm5
	pand	%xmm1, %xmm6
	por	%xmm5, %xmm6
	movdqa	%xmm1, %xmm5
	pand	%xmm11, %xmm1
	pandn	%xmm15, %xmm5
	movdqa	-128(%rbp), %xmm15
	movdqa	%xmm1, %xmm11
	movdqa	%xmm4, %xmm1
	movaps	%xmm6, -112(%rbp)
	por	%xmm5, %xmm11
	pcmpgtd	%xmm15, %xmm1
	movdqa	%xmm15, %xmm6
	pand	%xmm1, %xmm6
	movdqa	%xmm1, %xmm5
	pandn	%xmm4, %xmm5
	movdqa	%xmm6, %xmm0
	por	%xmm5, %xmm0
	movdqa	%xmm1, %xmm5
	pand	%xmm4, %xmm1
	pshufd	$27, %xmm13, %xmm4
	movdqa	%xmm0, %xmm15
	pandn	-128(%rbp), %xmm5
	movdqa	%xmm4, %xmm0
	pcmpgtd	%xmm13, %xmm0
	por	%xmm5, %xmm1
	movaps	%xmm1, -128(%rbp)
	movdqa	%xmm0, %xmm6
	movdqa	%xmm0, %xmm1
	movdqa	%xmm0, %xmm5
	pandn	%xmm4, %xmm6
	pand	%xmm4, %xmm1
	pshufd	$27, %xmm3, %xmm4
	pandn	%xmm13, %xmm5
	pand	%xmm0, %xmm13
	movdqa	%xmm4, %xmm0
	pcmpgtd	%xmm3, %xmm0
	por	%xmm5, %xmm1
	por	%xmm6, %xmm13
	shufpd	$2, %xmm1, %xmm13
	movdqa	%xmm0, %xmm5
	movdqa	%xmm0, %xmm1
	movdqa	%xmm0, %xmm6
	pandn	%xmm3, %xmm5
	pand	%xmm4, %xmm1
	pandn	%xmm4, %xmm6
	por	%xmm5, %xmm1
	pand	%xmm0, %xmm3
	por	%xmm6, %xmm3
	movapd	%xmm1, %xmm6
	movsd	%xmm3, %xmm6
	pshufd	$27, %xmm10, %xmm3
	movdqa	%xmm3, %xmm0
	pcmpgtd	%xmm10, %xmm0
	movdqa	%xmm0, %xmm1
	movdqa	%xmm0, %xmm5
	pandn	%xmm10, %xmm0
	pandn	%xmm3, %xmm5
	pand	%xmm1, %xmm10
	pand	%xmm3, %xmm1
	pshufd	$27, %xmm2, %xmm3
	por	%xmm0, %xmm1
	por	%xmm5, %xmm10
	movdqa	%xmm3, %xmm0
	shufpd	$2, %xmm1, %xmm10
	pcmpgtd	%xmm2, %xmm0
	movdqa	%xmm0, %xmm1
	movdqa	%xmm0, %xmm5
	pandn	%xmm2, %xmm0
	pand	%xmm1, %xmm2
	pand	%xmm3, %xmm1
	pandn	%xmm3, %xmm5
	por	%xmm0, %xmm1
	por	%xmm5, %xmm2
	movdqa	-64(%rbp), %xmm0
	movapd	%xmm1, %xmm3
	movsd	%xmm2, %xmm3
	pshufd	$27, %xmm7, %xmm2
	movapd	%xmm3, %xmm5
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm7, %xmm3
	movdqa	%xmm3, %xmm1
	pandn	%xmm2, %xmm3
	movdqa	%xmm3, %xmm4
	movdqa	%xmm1, %xmm3
	pandn	%xmm7, %xmm3
	pand	%xmm1, %xmm7
	pand	%xmm2, %xmm1
	pshufd	$27, %xmm9, %xmm2
	por	%xmm3, %xmm1
	por	%xmm4, %xmm7
	movdqa	%xmm2, %xmm3
	shufpd	$2, %xmm1, %xmm7
	pcmpgtd	%xmm9, %xmm3
	movdqa	%xmm3, %xmm1
	pandn	%xmm2, %xmm3
	movdqa	%xmm3, %xmm4
	movdqa	%xmm1, %xmm3
	pandn	%xmm9, %xmm3
	pand	%xmm1, %xmm9
	pand	%xmm2, %xmm1
	pshufd	$27, %xmm12, %xmm2
	por	%xmm3, %xmm1
	por	%xmm4, %xmm9
	movdqa	%xmm2, %xmm3
	shufpd	$2, %xmm1, %xmm9
	pcmpgtd	%xmm12, %xmm3
	movdqa	%xmm3, %xmm1
	pandn	%xmm2, %xmm3
	movdqa	%xmm3, %xmm4
	movdqa	%xmm1, %xmm3
	pandn	%xmm12, %xmm3
	pand	%xmm1, %xmm12
	pand	%xmm2, %xmm1
	pshufd	$27, %xmm0, %xmm2
	por	%xmm3, %xmm1
	por	%xmm4, %xmm12
	movdqa	%xmm2, %xmm3
	shufpd	$2, %xmm1, %xmm12
	pcmpgtd	%xmm0, %xmm3
	movdqa	%xmm3, %xmm1
	pandn	%xmm2, %xmm3
	movdqa	%xmm3, %xmm4
	movdqa	%xmm1, %xmm3
	pandn	%xmm0, %xmm3
	pand	%xmm1, %xmm0
	pand	%xmm2, %xmm1
	por	%xmm3, %xmm1
	por	%xmm4, %xmm0
	movapd	%xmm1, %xmm3
	pshufd	$27, %xmm14, %xmm1
	movsd	%xmm0, %xmm3
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm14, %xmm0
	movaps	%xmm3, -176(%rbp)
	movdqa	-80(%rbp), %xmm4
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm2
	pandn	%xmm1, %xmm3
	pandn	%xmm14, %xmm2
	pand	%xmm0, %xmm14
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm14
	pshufd	$27, %xmm4, %xmm1
	por	%xmm2, %xmm0
	shufpd	$2, %xmm0, %xmm14
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm4, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm3
	pandn	%xmm4, %xmm2
	pand	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	pandn	%xmm1, %xmm3
	por	%xmm2, %xmm0
	pshufd	$27, %xmm8, %xmm1
	movapd	%xmm0, %xmm2
	movdqa	%xmm1, %xmm0
	por	%xmm3, %xmm4
	pcmpgtd	%xmm8, %xmm0
	movsd	%xmm4, %xmm2
	movdqa	-96(%rbp), %xmm4
	movaps	%xmm2, -192(%rbp)
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm2
	pandn	%xmm1, %xmm3
	pandn	%xmm8, %xmm2
	pand	%xmm0, %xmm8
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm8
	pshufd	$27, %xmm4, %xmm1
	por	%xmm2, %xmm0
	shufpd	$2, %xmm0, %xmm8
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm4, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm2
	pandn	%xmm1, %xmm3
	pandn	%xmm4, %xmm2
	pand	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm4
	por	%xmm2, %xmm0
	movsd	%xmm4, %xmm0
	movdqa	-112(%rbp), %xmm4
	movaps	%xmm0, -208(%rbp)
	pshufd	$27, %xmm4, %xmm1
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm4, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm3
	pandn	%xmm4, %xmm2
	pand	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	pandn	%xmm1, %xmm3
	por	%xmm2, %xmm0
	pshufd	$27, %xmm11, %xmm1
	por	%xmm3, %xmm4
	movapd	%xmm0, %xmm3
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm11, %xmm0
	movsd	%xmm4, %xmm3
	movdqa	-128(%rbp), %xmm4
	movaps	%xmm3, -224(%rbp)
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm3
	pandn	%xmm1, %xmm3
	pandn	%xmm11, %xmm2
	pand	%xmm0, %xmm11
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm11
	pshufd	$27, %xmm15, %xmm1
	por	%xmm2, %xmm0
	shufpd	$2, %xmm0, %xmm11
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm15, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm2
	pandn	%xmm1, %xmm3
	pandn	%xmm15, %xmm2
	pand	%xmm0, %xmm15
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm15
	pshufd	$27, %xmm4, %xmm1
	por	%xmm2, %xmm0
	shufpd	$2, %xmm0, %xmm15
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm4, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm2
	pandn	%xmm1, %xmm3
	pandn	%xmm4, %xmm2
	pand	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm4
	pshufd	$177, %xmm13, %xmm1
	por	%xmm2, %xmm0
	shufpd	$2, %xmm0, %xmm4
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm13, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm3
	pandn	%xmm1, %xmm2
	pandn	%xmm13, %xmm3
	pand	%xmm0, %xmm1
	pand	%xmm13, %xmm0
	por	%xmm3, %xmm1
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm6, %xmm1
	movaps	%xmm0, -112(%rbp)
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm6, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm13
	pandn	%xmm1, %xmm2
	pandn	%xmm6, %xmm13
	pand	%xmm0, %xmm1
	pand	%xmm6, %xmm0
	por	%xmm13, %xmm1
	movapd	-224(%rbp), %xmm6
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm10, %xmm1
	movaps	%xmm0, -128(%rbp)
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm10, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm13
	pandn	%xmm1, %xmm2
	pandn	%xmm10, %xmm13
	pand	%xmm0, %xmm1
	pand	%xmm10, %xmm0
	por	%xmm13, %xmm1
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm5, %xmm1
	movaps	%xmm0, -144(%rbp)
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm5, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm10
	pandn	%xmm1, %xmm2
	pandn	%xmm5, %xmm10
	pand	%xmm0, %xmm1
	pand	%xmm5, %xmm0
	por	%xmm10, %xmm1
	movapd	-208(%rbp), %xmm5
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm7, %xmm1
	movaps	%xmm0, -160(%rbp)
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm7, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm10
	pandn	%xmm1, %xmm2
	pandn	%xmm7, %xmm10
	pand	%xmm0, %xmm1
	pand	%xmm7, %xmm0
	por	%xmm10, %xmm1
	movapd	-176(%rbp), %xmm7
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm9, %xmm1
	movaps	%xmm0, -64(%rbp)
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm9, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm10
	pandn	%xmm1, %xmm2
	pandn	%xmm9, %xmm10
	pand	%xmm0, %xmm1
	pand	%xmm9, %xmm0
	por	%xmm10, %xmm1
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm12, %xmm1
	movaps	%xmm0, -80(%rbp)
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm12, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm10
	pandn	%xmm1, %xmm2
	pandn	%xmm12, %xmm10
	pand	%xmm0, %xmm1
	pand	%xmm12, %xmm0
	por	%xmm10, %xmm1
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm7, %xmm1
	movaps	%xmm0, -96(%rbp)
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm7, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm3
	pandn	%xmm1, %xmm2
	pandn	%xmm7, %xmm3
	pand	%xmm0, %xmm1
	pand	%xmm7, %xmm0
	por	%xmm3, %xmm1
	movapd	-192(%rbp), %xmm7
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	movdqa	%xmm0, %xmm12
	pshufd	$177, %xmm14, %xmm0
	movdqa	%xmm0, %xmm3
	pcmpgtd	%xmm14, %xmm3
	movdqa	%xmm3, %xmm1
	movdqa	%xmm3, %xmm2
	pandn	%xmm0, %xmm1
	pandn	%xmm14, %xmm2
	pand	%xmm3, %xmm0
	pand	%xmm14, %xmm3
	por	%xmm2, %xmm0
	por	%xmm1, %xmm3
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm3, %xmm3
	punpckldq	%xmm0, %xmm3
	pshufd	$177, %xmm7, %xmm0
	movdqa	%xmm0, %xmm9
	pcmpgtd	%xmm7, %xmm9
	movdqa	%xmm9, %xmm1
	movdqa	%xmm9, %xmm2
	pandn	%xmm0, %xmm1
	pandn	%xmm7, %xmm2
	pand	%xmm9, %xmm0
	pand	%xmm7, %xmm9
	por	%xmm2, %xmm0
	por	%xmm1, %xmm9
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm9, %xmm9
	punpckldq	%xmm0, %xmm9
	pshufd	$177, %xmm8, %xmm0
	movdqa	%xmm0, %xmm7
	pcmpgtd	%xmm8, %xmm7
	movdqa	%xmm7, %xmm1
	movdqa	%xmm7, %xmm2
	pandn	%xmm0, %xmm1
	pandn	%xmm8, %xmm2
	pand	%xmm7, %xmm0
	pand	%xmm8, %xmm7
	por	%xmm2, %xmm0
	por	%xmm1, %xmm7
	pshufd	$221, %xmm0, %xmm0
	pshufd	$136, %xmm7, %xmm7
	punpckldq	%xmm0, %xmm7
	pshufd	$177, %xmm5, %xmm0
	movdqa	%xmm0, %xmm10
	pcmpgtd	%xmm5, %xmm10
	movdqa	%xmm10, %xmm1
	movdqa	%xmm10, %xmm2
	pandn	%xmm0, %xmm1
	pandn	%xmm5, %xmm2
	pand	%xmm10, %xmm0
	pand	%xmm5, %xmm10
	por	%xmm2, %xmm0
	por	%xmm1, %xmm10
	pshufd	$221, %xmm0, %xmm0
	pshufd	$177, %xmm6, %xmm1
	pshufd	$136, %xmm10, %xmm10
	punpckldq	%xmm0, %xmm10
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm6, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm5
	pandn	%xmm1, %xmm2
	pandn	%xmm6, %xmm5
	pand	%xmm0, %xmm1
	pand	%xmm6, %xmm0
	por	%xmm5, %xmm1
	por	%xmm2, %xmm0
	pshufd	$221, %xmm1, %xmm1
	pshufd	$136, %xmm0, %xmm0
	punpckldq	%xmm1, %xmm0
	pshufd	$177, %xmm11, %xmm1
	movdqa	%xmm1, %xmm13
	pcmpgtd	%xmm11, %xmm13
	movdqa	%xmm13, %xmm2
	movdqa	%xmm13, %xmm5
	pandn	%xmm1, %xmm2
	pandn	%xmm11, %xmm5
	pand	%xmm13, %xmm1
	pand	%xmm11, %xmm13
	por	%xmm5, %xmm1
	por	%xmm2, %xmm13
	pshufd	$221, %xmm1, %xmm1
	pshufd	$177, %xmm15, %xmm2
	pshufd	$136, %xmm13, %xmm13
	punpckldq	%xmm1, %xmm13
	movdqa	%xmm2, %xmm1
	pcmpgtd	%xmm15, %xmm1
	movdqa	%xmm1, %xmm5
	movdqa	%xmm1, %xmm6
	pandn	%xmm2, %xmm5
	pandn	%xmm15, %xmm6
	pand	%xmm1, %xmm2
	pand	%xmm15, %xmm1
	por	%xmm6, %xmm2
	por	%xmm5, %xmm1
	pshufd	$221, %xmm2, %xmm2
	pshufd	$177, %xmm4, %xmm5
	pshufd	$136, %xmm1, %xmm1
	punpckldq	%xmm2, %xmm1
	movdqa	%xmm5, %xmm2
	pcmpgtd	%xmm4, %xmm2
	movdqa	%xmm2, %xmm6
	movdqa	%xmm2, %xmm8
	pandn	%xmm5, %xmm6
	pandn	%xmm4, %xmm8
	pand	%xmm2, %xmm5
	pand	%xmm4, %xmm2
	por	%xmm8, %xmm5
	por	%xmm6, %xmm2
	pshufd	$221, %xmm5, %xmm5
	pshufd	$136, %xmm2, %xmm2
	punpckldq	%xmm5, %xmm2
	movdqa	%xmm2, %xmm15
.L478:
	movdqa	-112(%rbp), %xmm4
	movq	-264(%rbp), %rdx
	movups	%xmm4, (%rdx)
	movdqa	-128(%rbp), %xmm4
	movups	%xmm4, (%r15)
	movdqa	-144(%rbp), %xmm4
	movups	%xmm4, (%r14)
	movdqa	-160(%rbp), %xmm4
	movups	%xmm4, 0(%r13)
	movdqa	-64(%rbp), %xmm4
	movups	%xmm4, (%r12)
	movdqa	-80(%rbp), %xmm4
	movups	%xmm4, (%rbx)
	movdqa	-96(%rbp), %xmm4
	movq	-248(%rbp), %rbx
	movups	%xmm4, (%r11)
	movups	%xmm12, (%r10)
	movups	%xmm3, (%r9)
	movups	%xmm9, (%r8)
	movups	%xmm7, (%rdi)
	movups	%xmm10, (%rsi)
	movups	%xmm0, (%rcx)
	movq	-256(%rbp), %rcx
	movups	%xmm13, (%rbx)
	movups	%xmm1, (%rcx)
	movups	%xmm15, (%rax)
	addq	$240, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L481:
	.cfi_restore_state
	movdqa	-192(%rbp), %xmm15
	movdqa	-208(%rbp), %xmm1
	movdqa	%xmm2, %xmm9
	jmp	.L478
	.p2align 4,,10
	.p2align 3
.L482:
	movdqa	%xmm7, %xmm9
	movdqa	%xmm8, %xmm3
	movdqa	%xmm10, %xmm7
	movdqa	%xmm2, %xmm15
	movdqa	%xmm5, %xmm10
	jmp	.L478
	.cfi_endproc
.LFE18797:
	.size	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0, .-_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	.section	.text._ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0, @function
_ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0:
.LFB18798:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$6072, %rsp
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	movq	%rdi, -5768(%rbp)
	movq	%rsi, -5960(%rbp)
	movq	%rdx, -5896(%rbp)
	movq	%rcx, -5880(%rbp)
	movq	%r8, -5904(%rbp)
	movq	%r9, -5944(%rbp)
	cmpq	$64, %rdx
	jbe	.L769
	movq	%rdi, %r13
	movq	%rdi, %r14
	shrq	$2, %r13
	movq	%r13, %rax
	andl	$15, %eax
	jne	.L770
	movq	%rdx, %r11
	movq	%rdi, %r12
	movq	%r8, %r15
.L496:
	movq	8(%r15), %rdx
	movq	16(%r15), %r9
	movq	%rdx, %rax
	leaq	1(%r9), %rdi
	movq	%rdx, %rsi
	xorq	(%r15), %rdi
	rolq	$24, %rax
	shrq	$11, %rsi
	leaq	(%rdx,%rdx,8), %rcx
	leaq	2(%r9), %rdx
	addq	%rdi, %rax
	xorq	%rsi, %rcx
	movq	%rax, %rsi
	xorq	%rdx, %rcx
	leaq	(%rax,%rax,8), %rdx
	movq	%rax, %r8
	rolq	$24, %rsi
	shrq	$11, %r8
	leaq	3(%r9), %rax
	addq	%rcx, %rsi
	xorq	%r8, %rdx
	movq	%rsi, %r10
	xorq	%rax, %rdx
	leaq	(%rsi,%rsi,8), %rax
	movq	%rsi, %r8
	rolq	$24, %r10
	shrq	$11, %r8
	leaq	4(%r9), %rsi
	addq	$5, %r9
	addq	%rdx, %r10
	xorq	%r8, %rax
	movq	%r9, 16(%r15)
	movq	%r10, %r8
	movq	%r10, %rbx
	xorq	%rsi, %rax
	shrq	$11, %rbx
	rolq	$24, %r8
	leaq	(%r10,%r10,8), %rsi
	addq	%rax, %r8
	xorq	%rbx, %rsi
	xorq	%r9, %rsi
	leaq	(%r8,%r8,8), %r10
	movq	%r8, %rbx
	rolq	$24, %r8
	addq	%rsi, %r8
	shrq	$11, %rbx
	movl	%edx, %r9d
	movl	%esi, %esi
	xorq	%rbx, %r10
	movq	%r8, %xmm5
	movq	%r11, %rbx
	movabsq	$68719476719, %r8
	shrq	$4, %rbx
	cmpq	%r8, %r11
	movl	$4294967295, %r8d
	movl	%edi, %r11d
	cmova	%r8, %rbx
	movq	%r10, %xmm0
	shrq	$32, %rdi
	movl	%ecx, %r10d
	shrq	$32, %rdx
	movl	%eax, %r8d
	punpcklqdq	%xmm5, %xmm0
	shrq	$32, %rcx
	imulq	%rbx, %r11
	shrq	$32, %rax
	movups	%xmm0, (%r15)
	imulq	%rbx, %rdi
	imulq	%rbx, %r10
	imulq	%rbx, %rcx
	shrq	$32, %r11
	imulq	%rbx, %r9
	shrq	$32, %rdi
	salq	$6, %r11
	imulq	%rbx, %rdx
	shrq	$32, %r10
	salq	$6, %rdi
	addq	%r12, %r11
	imulq	%rbx, %r8
	shrq	$32, %rcx
	salq	$6, %r10
	addq	%r12, %rdi
	shrq	$32, %r9
	salq	$6, %rcx
	addq	%r12, %r10
	shrq	$32, %rdx
	salq	$6, %r9
	addq	%r12, %rcx
	shrq	$32, %r8
	salq	$6, %rdx
	addq	%r12, %r9
	salq	$6, %r8
	addq	%r12, %rdx
	addq	%r12, %r8
	imulq	%rbx, %rax
	imulq	%rbx, %rsi
	shrq	$32, %rax
	shrq	$32, %rsi
	salq	$6, %rax
	salq	$6, %rsi
	addq	%r12, %rax
	leaq	(%r12,%rsi), %rbx
	movq	-5880(%rbp), %r12
	xorl	%esi, %esi
.L498:
	movdqa	(%r10,%rsi,4), %xmm0
	movdqa	(%r11,%rsi,4), %xmm3
	movdqa	%xmm0, %xmm1
	movdqa	%xmm3, %xmm2
	pcmpgtd	%xmm3, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm2
	pandn	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	por	%xmm4, %xmm2
	movdqa	%xmm1, %xmm4
	movdqa	%xmm0, %xmm1
	movdqa	(%rdi,%rsi,4), %xmm0
	pandn	%xmm3, %xmm4
	pcmpgtd	%xmm2, %xmm0
	por	%xmm4, %xmm1
	movdqa	%xmm0, %xmm3
	pand	(%rdi,%rsi,4), %xmm0
	pandn	%xmm2, %xmm3
	movdqa	%xmm1, %xmm2
	por	%xmm3, %xmm0
	pcmpgtd	%xmm0, %xmm2
	movdqa	%xmm2, %xmm3
	pand	%xmm2, %xmm0
	pandn	%xmm1, %xmm3
	por	%xmm3, %xmm0
	movdqa	(%rcx,%rsi,4), %xmm3
	movaps	%xmm0, (%r12,%rsi,4)
	movdqa	(%rdx,%rsi,4), %xmm0
	movdqa	%xmm3, %xmm2
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm3, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm2
	pandn	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	por	%xmm4, %xmm2
	movdqa	%xmm1, %xmm4
	movdqa	%xmm0, %xmm1
	movdqa	(%r9,%rsi,4), %xmm0
	pandn	%xmm3, %xmm4
	pcmpgtd	%xmm2, %xmm0
	por	%xmm4, %xmm1
	movdqa	%xmm0, %xmm3
	pand	(%r9,%rsi,4), %xmm0
	pandn	%xmm2, %xmm3
	movdqa	%xmm1, %xmm2
	por	%xmm3, %xmm0
	pcmpgtd	%xmm0, %xmm2
	movdqa	%xmm2, %xmm3
	pand	%xmm2, %xmm0
	pandn	%xmm1, %xmm3
	por	%xmm3, %xmm0
	movdqa	(%r8,%rsi,4), %xmm3
	movaps	%xmm0, 64(%r12,%rsi,4)
	movdqa	(%rbx,%rsi,4), %xmm0
	movdqa	%xmm3, %xmm2
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm3, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm2
	pandn	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	por	%xmm4, %xmm2
	movdqa	%xmm1, %xmm4
	movdqa	%xmm0, %xmm1
	movdqa	(%rax,%rsi,4), %xmm0
	pandn	%xmm3, %xmm4
	pcmpgtd	%xmm2, %xmm0
	por	%xmm4, %xmm1
	movdqa	%xmm0, %xmm3
	pand	(%rax,%rsi,4), %xmm0
	pandn	%xmm2, %xmm3
	movdqa	%xmm1, %xmm2
	por	%xmm3, %xmm0
	pcmpgtd	%xmm0, %xmm2
	movdqa	%xmm2, %xmm3
	pand	%xmm2, %xmm0
	pandn	%xmm1, %xmm3
	por	%xmm3, %xmm0
	movaps	%xmm0, 128(%r12,%rsi,4)
	addq	$4, %rsi
	cmpq	$16, %rsi
	jne	.L498
	movq	-5880(%rbp), %rbx
	movd	(%rbx), %xmm5
	movdqa	16(%rbx), %xmm1
	movdqa	(%rbx), %xmm3
	pshufd	$0, %xmm5, %xmm0
	pxor	%xmm0, %xmm3
	pxor	%xmm0, %xmm1
	movdqa	%xmm0, %xmm2
	por	%xmm3, %xmm1
	movdqa	32(%rbx), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	48(%rbx), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	64(%rbx), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	80(%rbx), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	96(%rbx), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	112(%rbx), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	128(%rbx), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	144(%rbx), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	160(%rbx), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	176(%rbx), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	pxor	%xmm3, %xmm3
	pcmpeqd	%xmm3, %xmm1
	movmskps	%xmm1, %eax
	cmpl	$15, %eax
	je	.L499
	movdqa	.LC4(%rip), %xmm0
	movl	$4, %esi
	movq	%rbx, %rdi
	leaq	192(%rbx), %r12
	movups	%xmm0, 192(%rbx)
	movups	%xmm0, 208(%rbx)
	movups	%xmm0, 224(%rbx)
	movups	%xmm0, 240(%rbx)
	movups	%xmm0, 256(%rbx)
	call	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	movd	(%rbx), %xmm5
	pcmpeqd	%xmm2, %xmm2
	pshufd	$0, %xmm5, %xmm0
	movd	188(%rbx), %xmm5
	pshufd	$0, %xmm5, %xmm1
	paddd	%xmm1, %xmm2
	pcmpeqd	%xmm0, %xmm2
	movmskps	%xmm2, %eax
	cmpl	$15, %eax
	jne	.L501
	movq	-5896(%rbp), %rsi
	movq	-5768(%rbp), %rdi
	leaq	-64(%rbp), %rdx
	movq	%r12, %rcx
	call	_ZN3hwy6N_SSE26detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L483
.L501:
	movq	-5880(%rbp), %rbx
	movl	$23, %eax
	movl	$24, %edx
	movl	96(%rbx), %ecx
	movq	%rbx, %rdi
	cmpl	%ecx, 92(%rbx)
	je	.L543
	jmp	.L548
	.p2align 4,,10
	.p2align 3
.L546:
	testq	%rax, %rax
	je	.L771
.L543:
	movq	%rax, %rdx
	subq	$1, %rax
	movl	(%rdi,%rax,4), %esi
	cmpl	%ecx, %esi
	je	.L546
	movq	-5880(%rbp), %rbx
	movq	%rbx, %rdi
	cmpl	%ecx, (%rbx,%rdx,4)
	je	.L548
	movl	%esi, %ecx
	jmp	.L545
	.p2align 4,,10
	.p2align 3
.L549:
	cmpq	$47, %rdx
	je	.L767
.L548:
	movq	%rdx, %rsi
	addq	$1, %rdx
	cmpl	%ecx, (%rdi,%rdx,4)
	je	.L549
	movl	$24, %edx
	subq	$23, %rsi
	subq	%rax, %rdx
	cmpq	%rdx, %rsi
	jb	.L545
.L767:
	movq	-5880(%rbp), %rbx
	movl	(%rbx,%rax,4), %ecx
.L545:
	movd	%ecx, %xmm6
	pshufd	$0, %xmm6, %xmm3
.L768:
	movl	$1, -5948(%rbp)
.L542:
	cmpq	$0, -5944(%rbp)
	je	.L772
	movq	-5896(%rbp), %rax
	movq	-5768(%rbp), %rsi
	movaps	%xmm3, -5728(%rbp)
	subq	$4, %rax
	movdqu	(%rsi,%rax,4), %xmm5
	movq	%rax, %rbx
	movq	%rax, -5760(%rbp)
	andl	$15, %ebx
	movq	%rbx, -5776(%rbp)
	movaps	%xmm5, -5936(%rbp)
	andl	$12, %eax
	je	.L615
	movdqu	(%rsi), %xmm1
	pcmpeqd	%xmm0, %xmm0
	movdqa	%xmm1, %xmm4
	movaps	%xmm1, -5696(%rbp)
	pcmpgtd	%xmm3, %xmm4
	pxor	%xmm4, %xmm0
	movaps	%xmm4, -5808(%rbp)
	movmskps	%xmm0, %ebx
	movq	%rbx, %rdi
	salq	$4, %rbx
	call	__popcountdi2@PLT
	leaq	_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %rcx
	movdqa	-5696(%rbp), %xmm1
	movdqa	(%rcx,%rbx), %xmm0
	movq	%rcx, -5888(%rbp)
	movslq	%eax, %r15
	movl	%eax, -5712(%rbp)
	movaps	%xmm0, -528(%rbp)
	movzbl	-520(%rbp), %edx
	movd	%xmm0, %r9d
	movaps	%xmm0, -544(%rbp)
	andl	$15, %r9d
	movq	%rdx, %rcx
	movzbl	-535(%rbp), %edx
	movaps	%xmm0, -512(%rbp)
	movzbl	-505(%rbp), %eax
	movaps	%xmm0, -560(%rbp)
	andl	$15, %ecx
	movq	%rdx, %rdi
	movzbl	-550(%rbp), %edx
	movaps	%xmm0, -496(%rbp)
	movzbl	-490(%rbp), %r14d
	andl	$15, %edi
	andl	$15, %eax
	movaps	%xmm0, -416(%rbp)
	movzbl	-415(%rbp), %r10d
	andl	$15, %edx
	movaps	%xmm0, -432(%rbp)
	andl	$15, %r14d
	movzbl	-430(%rbp), %r11d
	movaps	%xmm0, -448(%rbp)
	movzbl	-445(%rbp), %ebx
	andl	$15, %r10d
	movaps	%xmm0, -464(%rbp)
	movzbl	-460(%rbp), %r12d
	andl	$15, %r11d
	movaps	%xmm0, -480(%rbp)
	movzbl	-475(%rbp), %r13d
	andl	$15, %ebx
	movaps	%xmm0, -576(%rbp)
	andl	$15, %r12d
	movq	%rcx, -5696(%rbp)
	andl	$15, %r13d
	movq	%rdi, -5744(%rbp)
	movq	%rdx, -5784(%rbp)
	movzbl	-565(%rbp), %edx
	movaps	%xmm1, -400(%rbp)
	movaps	%xmm0, -592(%rbp)
	movzbl	-580(%rbp), %ecx
	andl	$15, %edx
	movd	-5712(%rbp), %xmm7
	movaps	%xmm0, -608(%rbp)
	movzbl	-595(%rbp), %esi
	movaps	%xmm0, -624(%rbp)
	movzbl	-610(%rbp), %edi
	andl	$15, %ecx
	movaps	%xmm0, -640(%rbp)
	andl	$15, %esi
	pshufd	$0, %xmm7, %xmm0
	movzbl	-400(%rbp,%rax), %eax
	movzbl	-400(%rbp,%r14), %r14d
	movzbl	-400(%rbp,%rbx), %ebx
	andl	$15, %edi
	salq	$8, %rax
	movzbl	-625(%rbp), %r8d
	movzbl	-400(%rbp,%r13), %r13d
	orq	%r14, %rax
	movzbl	-400(%rbp,%r12), %r12d
	movzbl	-400(%rbp,%r11), %r11d
	salq	$8, %rax
	andl	$15, %r8d
	movzbl	-400(%rbp,%r10), %r10d
	movzbl	-400(%rbp,%rdi), %edi
	orq	%r13, %rax
	movzbl	-400(%rbp,%rsi), %esi
	movzbl	-400(%rbp,%r8), %r8d
	salq	$8, %rax
	movzbl	-400(%rbp,%rcx), %ecx
	movzbl	-400(%rbp,%rdx), %edx
	orq	%r12, %rax
	salq	$8, %r8
	movdqa	.LC0(%rip), %xmm7
	movzbl	-400(%rbp,%r9), %r9d
	salq	$8, %rax
	orq	%rbx, %rax
	pcmpgtd	%xmm7, %xmm0
	movaps	%xmm7, -5920(%rbp)
	salq	$8, %rax
	orq	%r11, %rax
	salq	$8, %rax
	orq	%r10, %rax
	salq	$8, %rax
	orq	%rdi, %r8
	movq	-5744(%rbp), %rdi
	salq	$8, %r8
	movq	%rax, %rbx
	movd	%xmm0, %eax
	orq	%rsi, %r8
	orq	%r9, %rbx
	salq	$8, %r8
	orq	%rcx, %r8
	movq	-5696(%rbp), %rcx
	movq	%rbx, -5696(%rbp)
	salq	$8, %r8
	orq	%rdx, %r8
	movq	-5784(%rbp), %rdx
	salq	$8, %r8
	movzbl	-400(%rbp,%rdx), %edx
	orq	%rdx, %r8
	movzbl	-400(%rbp,%rdi), %edx
	salq	$8, %r8
	orq	%rdx, %r8
	movzbl	-400(%rbp,%rcx), %edx
	salq	$8, %r8
	orq	%rdx, %r8
	testl	%eax, %eax
	movq	%r8, -5688(%rbp)
	movdqa	-5808(%rbp), %xmm4
	je	.L553
	movq	-5768(%rbp), %rsi
	movdqa	-5696(%rbp), %xmm6
	movd	%xmm6, (%rsi)
.L553:
	pshufd	$85, %xmm0, %xmm2
	movd	%xmm2, %eax
	testl	%eax, %eax
	je	.L554
	pshufd	$85, -5696(%rbp), %xmm2
	movq	-5768(%rbp), %rax
	movd	%xmm2, 4(%rax)
.L554:
	movdqa	%xmm0, %xmm2
	punpckhdq	%xmm0, %xmm2
	movd	%xmm2, %eax
	testl	%eax, %eax
	je	.L555
	movdqa	-5696(%rbp), %xmm2
	movq	-5768(%rbp), %rax
	punpckhdq	%xmm2, %xmm2
	movd	%xmm2, 8(%rax)
.L555:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L556
	pshufd	$255, -5696(%rbp), %xmm0
	movq	-5768(%rbp), %rax
	movd	%xmm0, 12(%rax)
.L556:
	movq	-5768(%rbp), %rax
	movaps	%xmm1, -5744(%rbp)
	leaq	(%rax,%r15,4), %rbx
	movq	%rbx, -5696(%rbp)
	movmskps	%xmm4, %ebx
	movq	%rbx, %rdi
	salq	$4, %rbx
	call	__popcountdi2@PLT
	movdqa	-5744(%rbp), %xmm1
	movslq	%eax, %rsi
	movq	%rsi, -5712(%rbp)
	movq	-5888(%rbp), %rsi
	movaps	%xmm1, -384(%rbp)
	movdqa	(%rsi,%rbx), %xmm0
	movaps	%xmm0, -768(%rbp)
	movzbl	-760(%rbp), %edx
	movd	%xmm0, %r8d
	movaps	%xmm0, -784(%rbp)
	andl	$15, %r8d
	movq	%rdx, %rcx
	movzbl	-775(%rbp), %edx
	movaps	%xmm0, -800(%rbp)
	movaps	%xmm0, -752(%rbp)
	movzbl	-745(%rbp), %eax
	andl	$15, %ecx
	movq	%rdx, %rdi
	movzbl	-790(%rbp), %edx
	movaps	%xmm0, -736(%rbp)
	movzbl	-730(%rbp), %r13d
	movaps	%xmm0, -816(%rbp)
	andl	$15, %edi
	andl	$15, %eax
	movq	%rdx, %r15
	movzbl	-805(%rbp), %edx
	movq	%rcx, -5744(%rbp)
	andl	$15, %r13d
	movq	%rdi, -5784(%rbp)
	andl	$15, %r15d
	movaps	%xmm0, -656(%rbp)
	movq	%rdx, %r14
	movzbl	-655(%rbp), %r9d
	movaps	%xmm0, -672(%rbp)
	movzbl	-670(%rbp), %r10d
	andl	$15, %r14d
	movaps	%xmm0, -688(%rbp)
	movzbl	-685(%rbp), %r11d
	andl	$15, %r9d
	movaps	%xmm0, -704(%rbp)
	movzbl	-700(%rbp), %ebx
	andl	$15, %r10d
	movaps	%xmm0, -720(%rbp)
	movzbl	-715(%rbp), %r12d
	andl	$15, %r11d
	movaps	%xmm0, -832(%rbp)
	movzbl	-820(%rbp), %edx
	andl	$15, %ebx
	movaps	%xmm0, -848(%rbp)
	andl	$15, %r12d
	movzbl	-835(%rbp), %ecx
	movaps	%xmm0, -864(%rbp)
	movzbl	-850(%rbp), %esi
	andl	$15, %edx
	movaps	%xmm0, -880(%rbp)
	movzbl	-384(%rbp,%rax), %eax
	andl	$15, %ecx
	movzbl	-384(%rbp,%r13), %r13d
	movzbl	-384(%rbp,%r12), %r12d
	movzbl	-384(%rbp,%rbx), %ebx
	andl	$15, %esi
	salq	$8, %rax
	movzbl	-865(%rbp), %edi
	movzbl	-384(%rbp,%r11), %r11d
	orq	%r13, %rax
	movzbl	-384(%rbp,%rsi), %esi
	movzbl	-384(%rbp,%r10), %r10d
	salq	$8, %rax
	andl	$15, %edi
	movzbl	-384(%rbp,%r9), %r9d
	movzbl	-384(%rbp,%rcx), %ecx
	orq	%r12, %rax
	movzbl	-384(%rbp,%rdi), %edi
	movzbl	-384(%rbp,%rdx), %edx
	movzbl	-384(%rbp,%r8), %r8d
	salq	$8, %rax
	orq	%rbx, %rax
	salq	$8, %rdi
	movq	-5784(%rbp), %rbx
	salq	$8, %rax
	orq	%r11, %rax
	salq	$8, %rax
	orq	%r10, %rax
	salq	$8, %rax
	orq	%r9, %rax
	salq	$8, %rax
	orq	%rsi, %rdi
	salq	$8, %rdi
	orq	%r8, %rax
	orq	%rcx, %rdi
	movq	-5744(%rbp), %rcx
	movq	%rax, -5744(%rbp)
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-384(%rbp,%r14), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-384(%rbp,%r15), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-384(%rbp,%rbx), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-384(%rbp,%rcx), %edx
	movq	-5880(%rbp), %rcx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movq	%rdi, -5736(%rbp)
	movdqa	-5744(%rbp), %xmm6
	movups	%xmm6, (%rcx)
	testb	$8, -5760(%rbp)
	je	.L557
	movq	-5768(%rbp), %rax
	pcmpeqd	%xmm0, %xmm0
	movdqu	16(%rax), %xmm1
	movdqa	%xmm1, %xmm3
	movaps	%xmm1, -5744(%rbp)
	pcmpgtd	-5728(%rbp), %xmm3
	pxor	%xmm3, %xmm0
	movaps	%xmm3, -5824(%rbp)
	movmskps	%xmm0, %ebx
	movq	%rbx, %rdi
	salq	$4, %rbx
	call	__popcountdi2@PLT
	movq	-5888(%rbp), %rsi
	movdqa	-5744(%rbp), %xmm1
	movl	%eax, -5784(%rbp)
	movslq	%eax, %r14
	movdqa	(%rsi,%rbx), %xmm0
	movaps	%xmm1, -368(%rbp)
	movaps	%xmm0, -896(%rbp)
	movzbl	-895(%rbp), %eax
	movd	%xmm0, %r13d
	movaps	%xmm0, -1008(%rbp)
	movzbl	-1000(%rbp), %edx
	andl	$15, %r13d
	movaps	%xmm0, -912(%rbp)
	movq	%rax, %rbx
	movzbl	-910(%rbp), %eax
	movaps	%xmm0, -1024(%rbp)
	movq	%rdx, %rsi
	andl	$15, %ebx
	movzbl	-1015(%rbp), %edx
	movaps	%xmm0, -992(%rbp)
	movq	%rax, %r15
	movzbl	-985(%rbp), %eax
	andl	$15, %esi
	movaps	%xmm0, -976(%rbp)
	movq	%rdx, %rcx
	andl	$15, %r15d
	movzbl	-970(%rbp), %r12d
	andl	$15, %ecx
	andl	$15, %eax
	movaps	%xmm0, -928(%rbp)
	movzbl	-925(%rbp), %r10d
	movaps	%xmm0, -944(%rbp)
	andl	$15, %r12d
	movzbl	-940(%rbp), %r11d
	movaps	%xmm0, -960(%rbp)
	andl	$15, %r10d
	movaps	%xmm0, -1040(%rbp)
	movzbl	-1030(%rbp), %edx
	andl	$15, %r11d
	movq	%rbx, -5744(%rbp)
	movzbl	-955(%rbp), %ebx
	movq	%rsi, -5808(%rbp)
	andl	$15, %edx
	movq	%rcx, -5792(%rbp)
	andl	$15, %ebx
	movaps	%xmm0, -1056(%rbp)
	movzbl	-1045(%rbp), %ecx
	movd	-5784(%rbp), %xmm5
	movaps	%xmm0, -1072(%rbp)
	movzbl	-1060(%rbp), %esi
	movaps	%xmm0, -1088(%rbp)
	movzbl	-1075(%rbp), %edi
	andl	$15, %ecx
	movaps	%xmm0, -1104(%rbp)
	movzbl	-1090(%rbp), %r8d
	andl	$15, %esi
	movaps	%xmm0, -1120(%rbp)
	andl	$15, %edi
	pshufd	$0, %xmm5, %xmm0
	movzbl	-368(%rbp,%rax), %eax
	movzbl	-368(%rbp,%r12), %r12d
	andl	$15, %r8d
	movzbl	-368(%rbp,%rbx), %ebx
	salq	$8, %rax
	movzbl	-1105(%rbp), %r9d
	movzbl	-368(%rbp,%r11), %r11d
	orq	%r12, %rax
	movzbl	-368(%rbp,%rdi), %edi
	movzbl	-368(%rbp,%r10), %r10d
	salq	$8, %rax
	andl	$15, %r9d
	movzbl	-368(%rbp,%r8), %r8d
	movzbl	-368(%rbp,%rsi), %esi
	orq	%rbx, %rax
	movq	-5744(%rbp), %rbx
	movzbl	-368(%rbp,%r9), %r9d
	salq	$8, %rax
	movzbl	-368(%rbp,%rcx), %ecx
	movzbl	-368(%rbp,%rdx), %edx
	orq	%r11, %rax
	salq	$8, %r9
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-368(%rbp,%r15), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-368(%rbp,%rbx), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-368(%rbp,%r13), %r10d
	salq	$8, %rax
	orq	%r8, %r9
	salq	$8, %r9
	movq	%rax, %rbx
	orq	%rdi, %r9
	orq	%r10, %rbx
	salq	$8, %r9
	movq	%rbx, -5744(%rbp)
	orq	%rsi, %r9
	movq	-5808(%rbp), %rsi
	salq	$8, %r9
	orq	%rcx, %r9
	movq	-5792(%rbp), %rcx
	salq	$8, %r9
	orq	%rdx, %r9
	movzbl	-368(%rbp,%rcx), %edx
	salq	$8, %r9
	orq	%rdx, %r9
	movzbl	-368(%rbp,%rsi), %edx
	salq	$8, %r9
	orq	%rdx, %r9
	movq	%r9, -5736(%rbp)
	pcmpgtd	-5920(%rbp), %xmm0
	movdqa	-5824(%rbp), %xmm3
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L558
	movq	-5696(%rbp), %rbx
	movdqa	-5744(%rbp), %xmm7
	movd	%xmm7, (%rbx)
.L558:
	pshufd	$85, %xmm0, %xmm2
	movd	%xmm2, %eax
	testl	%eax, %eax
	je	.L559
	pshufd	$85, -5744(%rbp), %xmm2
	movq	-5696(%rbp), %rax
	movd	%xmm2, 4(%rax)
.L559:
	movdqa	%xmm0, %xmm2
	punpckhdq	%xmm0, %xmm2
	movd	%xmm2, %eax
	testl	%eax, %eax
	je	.L560
	movdqa	-5744(%rbp), %xmm2
	movq	-5696(%rbp), %rax
	punpckhdq	%xmm2, %xmm2
	movd	%xmm2, 8(%rax)
.L560:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L561
	pshufd	$255, -5744(%rbp), %xmm0
	movq	-5696(%rbp), %rax
	movd	%xmm0, 12(%rax)
.L561:
	movq	-5696(%rbp), %rax
	movmskps	%xmm3, %ebx
	movaps	%xmm1, -5744(%rbp)
	movq	%rbx, %rdi
	salq	$4, %rbx
	leaq	(%rax,%r14,4), %rax
	movq	%rax, -5696(%rbp)
	call	__popcountdi2@PLT
	movdqa	-5744(%rbp), %xmm1
	movslq	%eax, %r13
	movq	-5888(%rbp), %rax
	movaps	%xmm1, -352(%rbp)
	movdqa	(%rax,%rbx), %xmm0
	movaps	%xmm0, -1136(%rbp)
	movzbl	-1135(%rbp), %eax
	movd	%xmm0, %r14d
	movaps	%xmm0, -1248(%rbp)
	movzbl	-1240(%rbp), %edx
	andl	$15, %r14d
	andl	$15, %eax
	movaps	%xmm0, -1152(%rbp)
	movq	%rax, -5744(%rbp)
	movzbl	-1150(%rbp), %eax
	movq	%rdx, %rsi
	movaps	%xmm0, -1264(%rbp)
	movzbl	-1255(%rbp), %edx
	andl	$15, %esi
	movq	%rax, %r15
	movaps	%xmm0, -1232(%rbp)
	movzbl	-1225(%rbp), %eax
	movaps	%xmm0, -1216(%rbp)
	movq	%rdx, %rdi
	andl	$15, %r15d
	movzbl	-1210(%rbp), %r12d
	andl	$15, %edi
	andl	$15, %eax
	movaps	%xmm0, -1168(%rbp)
	movzbl	-1165(%rbp), %r10d
	movq	%rdi, -5808(%rbp)
	andl	$15, %r12d
	movaps	%xmm0, -1184(%rbp)
	movzbl	-1180(%rbp), %r11d
	andl	$15, %r10d
	movaps	%xmm0, -1200(%rbp)
	movzbl	-1195(%rbp), %ebx
	movaps	%xmm0, -1280(%rbp)
	movzbl	-1270(%rbp), %edx
	andl	$15, %r11d
	movaps	%xmm0, -1296(%rbp)
	movzbl	-1285(%rbp), %ecx
	andl	$15, %ebx
	movq	%rsi, -5784(%rbp)
	andl	$15, %edx
	movaps	%xmm0, -1312(%rbp)
	movzbl	-1300(%rbp), %esi
	andl	$15, %ecx
	movaps	%xmm0, -1328(%rbp)
	movzbl	-1315(%rbp), %edi
	movaps	%xmm0, -1344(%rbp)
	movzbl	-1330(%rbp), %r8d
	andl	$15, %esi
	movaps	%xmm0, -1360(%rbp)
	movzbl	-352(%rbp,%rax), %eax
	andl	$15, %edi
	movzbl	-352(%rbp,%r12), %r12d
	movzbl	-352(%rbp,%rbx), %ebx
	movzbl	-1345(%rbp), %r9d
	andl	$15, %r8d
	salq	$8, %rax
	movzbl	-352(%rbp,%r11), %r11d
	movzbl	-352(%rbp,%r10), %r10d
	orq	%r12, %rax
	andl	$15, %r9d
	movzbl	-352(%rbp,%r8), %r8d
	movzbl	-352(%rbp,%rdi), %edi
	salq	$8, %rax
	movzbl	-352(%rbp,%rsi), %esi
	movzbl	-352(%rbp,%r9), %r9d
	orq	%rbx, %rax
	movq	-5744(%rbp), %rbx
	movzbl	-352(%rbp,%rcx), %ecx
	salq	$8, %rax
	salq	$8, %r9
	movzbl	-352(%rbp,%rdx), %edx
	orq	%r11, %rax
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-352(%rbp,%r15), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-352(%rbp,%rbx), %r10d
	movq	-5880(%rbp), %rbx
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-352(%rbp,%r14), %r10d
	salq	$8, %rax
	orq	%r8, %r9
	salq	$8, %r9
	orq	%r10, %rax
	orq	%rdi, %r9
	movq	-5808(%rbp), %rdi
	movq	%rax, -5744(%rbp)
	salq	$8, %r9
	movq	-5712(%rbp), %rax
	orq	%rsi, %r9
	movq	-5784(%rbp), %rsi
	salq	$8, %r9
	orq	%rcx, %r9
	salq	$8, %r9
	orq	%rdx, %r9
	movzbl	-352(%rbp,%rdi), %edx
	salq	$8, %r9
	orq	%rdx, %r9
	movzbl	-352(%rbp,%rsi), %edx
	salq	$8, %r9
	orq	%rdx, %r9
	movq	%r9, -5736(%rbp)
	movdqa	-5744(%rbp), %xmm5
	movups	%xmm5, (%rbx,%rax,4)
	addq	%r13, %rax
	cmpq	$11, -5776(%rbp)
	movq	%rax, -5712(%rbp)
	jbe	.L557
	movq	-5768(%rbp), %rbx
	pcmpeqd	%xmm0, %xmm0
	movdqu	32(%rbx), %xmm1
	movdqa	%xmm1, %xmm3
	movaps	%xmm1, -5744(%rbp)
	pcmpgtd	-5728(%rbp), %xmm3
	pxor	%xmm3, %xmm0
	movaps	%xmm3, -5824(%rbp)
	movmskps	%xmm0, %ebx
	movq	%rbx, %rdi
	salq	$4, %rbx
	call	__popcountdi2@PLT
	movdqa	-5744(%rbp), %xmm1
	movslq	%eax, %r12
	movq	-5888(%rbp), %rax
	movl	%r12d, -5784(%rbp)
	movdqa	(%rax,%rbx), %xmm0
	movaps	%xmm1, -336(%rbp)
	movaps	%xmm0, -1376(%rbp)
	movzbl	-1375(%rbp), %eax
	movd	%xmm0, %r9d
	movaps	%xmm0, -1392(%rbp)
	andl	$15, %r9d
	movaps	%xmm0, -1488(%rbp)
	movq	%rax, %r15
	movzbl	-1480(%rbp), %edx
	movzbl	-1390(%rbp), %eax
	movaps	%xmm0, -1408(%rbp)
	andl	$15, %r15d
	movaps	%xmm0, -1504(%rbp)
	movq	%rax, %r14
	movq	%rdx, %rcx
	movzbl	-1405(%rbp), %eax
	movzbl	-1495(%rbp), %edx
	andl	$15, %ecx
	andl	$15, %r14d
	movaps	%xmm0, -1472(%rbp)
	movaps	%xmm0, -1520(%rbp)
	movq	%rax, %r13
	movzbl	-1465(%rbp), %eax
	movq	%rdx, %rdi
	movzbl	-1510(%rbp), %edx
	movaps	%xmm0, -1456(%rbp)
	movzbl	-1450(%rbp), %ebx
	andl	$15, %edi
	andl	$15, %eax
	movaps	%xmm0, -1424(%rbp)
	movzbl	-1420(%rbp), %r10d
	andl	$15, %edx
	andl	$15, %ebx
	andl	$15, %r13d
	movaps	%xmm0, -1440(%rbp)
	movq	%rcx, -5744(%rbp)
	movzbl	-1435(%rbp), %r11d
	andl	$15, %r10d
	movq	%rdi, -5808(%rbp)
	movq	%rdx, -5792(%rbp)
	andl	$15, %r11d
	movaps	%xmm0, -1536(%rbp)
	movzbl	-1525(%rbp), %edx
	movd	-5784(%rbp), %xmm5
	movaps	%xmm0, -1552(%rbp)
	movzbl	-1540(%rbp), %ecx
	movaps	%xmm0, -1568(%rbp)
	movzbl	-1555(%rbp), %esi
	andl	$15, %edx
	movaps	%xmm0, -1584(%rbp)
	movzbl	-1570(%rbp), %edi
	andl	$15, %ecx
	movaps	%xmm0, -1600(%rbp)
	andl	$15, %esi
	pshufd	$0, %xmm5, %xmm0
	movzbl	-336(%rbp,%rax), %eax
	movzbl	-336(%rbp,%rbx), %ebx
	movzbl	-1585(%rbp), %r8d
	andl	$15, %edi
	salq	$8, %rax
	movzbl	-336(%rbp,%rdi), %edi
	movzbl	-336(%rbp,%r11), %r11d
	orq	%rbx, %rax
	andl	$15, %r8d
	movzbl	-336(%rbp,%r10), %r10d
	movzbl	-336(%rbp,%rsi), %esi
	salq	$8, %rax
	movzbl	-336(%rbp,%rcx), %ecx
	movzbl	-336(%rbp,%r8), %r8d
	orq	%r11, %rax
	movzbl	-336(%rbp,%rdx), %edx
	movzbl	-336(%rbp,%r9), %r9d
	salq	$8, %rax
	salq	$8, %r8
	orq	%r10, %rax
	movzbl	-336(%rbp,%r13), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-336(%rbp,%r14), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-336(%rbp,%r15), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	salq	$8, %rax
	orq	%rdi, %r8
	movq	-5808(%rbp), %rdi
	salq	$8, %r8
	orq	%r9, %rax
	orq	%rsi, %r8
	salq	$8, %r8
	orq	%rcx, %r8
	movq	-5744(%rbp), %rcx
	movq	%rax, -5744(%rbp)
	salq	$8, %r8
	orq	%rdx, %r8
	movq	-5792(%rbp), %rdx
	salq	$8, %r8
	movzbl	-336(%rbp,%rdx), %edx
	orq	%rdx, %r8
	movzbl	-336(%rbp,%rdi), %edx
	salq	$8, %r8
	orq	%rdx, %r8
	movzbl	-336(%rbp,%rcx), %edx
	salq	$8, %r8
	orq	%rdx, %r8
	movq	%r8, -5736(%rbp)
	pcmpgtd	-5920(%rbp), %xmm0
	movdqa	-5824(%rbp), %xmm3
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L562
	movq	-5696(%rbp), %rax
	movdqa	-5744(%rbp), %xmm5
	movd	%xmm5, (%rax)
.L562:
	pshufd	$85, %xmm0, %xmm2
	movd	%xmm2, %eax
	testl	%eax, %eax
	je	.L563
	pshufd	$85, -5744(%rbp), %xmm2
	movq	-5696(%rbp), %rax
	movd	%xmm2, 4(%rax)
.L563:
	movdqa	%xmm0, %xmm2
	punpckhdq	%xmm0, %xmm2
	movd	%xmm2, %eax
	testl	%eax, %eax
	je	.L564
	movdqa	-5744(%rbp), %xmm2
	movq	-5696(%rbp), %rax
	punpckhdq	%xmm2, %xmm2
	movd	%xmm2, 8(%rax)
.L564:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L565
	pshufd	$255, -5744(%rbp), %xmm0
	movq	-5696(%rbp), %rax
	movd	%xmm0, 12(%rax)
.L565:
	movq	-5696(%rbp), %rax
	movmskps	%xmm3, %ebx
	movaps	%xmm1, -5744(%rbp)
	movq	%rbx, %rdi
	salq	$4, %rbx
	leaq	(%rax,%r12,4), %rax
	movq	%rax, -5696(%rbp)
	call	__popcountdi2@PLT
	movdqa	-5744(%rbp), %xmm1
	movslq	%eax, %r8
	movq	-5888(%rbp), %rax
	movaps	%xmm1, -320(%rbp)
	movdqa	(%rax,%rbx), %xmm0
	movaps	%xmm0, -1616(%rbp)
	movzbl	-1615(%rbp), %eax
	movd	%xmm0, %r12d
	movaps	%xmm0, -1728(%rbp)
	movzbl	-1720(%rbp), %edx
	andl	$15, %r12d
	movq	%rax, %r15
	movaps	%xmm0, -1632(%rbp)
	movzbl	-1630(%rbp), %eax
	movq	%rdx, %rsi
	movaps	%xmm0, -1744(%rbp)
	movzbl	-1735(%rbp), %edx
	andl	$15, %r15d
	movq	%rax, %r14
	movaps	%xmm0, -1648(%rbp)
	movzbl	-1645(%rbp), %eax
	andl	$15, %esi
	movq	%rdx, %rcx
	movaps	%xmm0, -1760(%rbp)
	movzbl	-1750(%rbp), %edx
	andl	$15, %r14d
	movq	%rax, %r13
	movaps	%xmm0, -1712(%rbp)
	andl	$15, %ecx
	movzbl	-1705(%rbp), %eax
	movaps	%xmm0, -1696(%rbp)
	movq	%rdx, %rdi
	andl	$15, %r13d
	movzbl	-1690(%rbp), %ebx
	andl	$15, %edi
	andl	$15, %eax
	movq	%rsi, -5744(%rbp)
	movq	%rdi, -5808(%rbp)
	andl	$15, %ebx
	movaps	%xmm0, -1664(%rbp)
	movzbl	-1660(%rbp), %r10d
	movaps	%xmm0, -1680(%rbp)
	movzbl	-1675(%rbp), %r11d
	movaps	%xmm0, -1776(%rbp)
	movzbl	-1765(%rbp), %edx
	andl	$15, %r10d
	movq	%rcx, -5784(%rbp)
	andl	$15, %r11d
	movaps	%xmm0, -1792(%rbp)
	movzbl	-1780(%rbp), %ecx
	andl	$15, %edx
	movaps	%xmm0, -1808(%rbp)
	movzbl	-1795(%rbp), %esi
	movaps	%xmm0, -1824(%rbp)
	movzbl	-1810(%rbp), %edi
	andl	$15, %ecx
	movaps	%xmm0, -1840(%rbp)
	movzbl	-320(%rbp,%rax), %eax
	andl	$15, %esi
	movzbl	-320(%rbp,%rbx), %ebx
	movzbl	-320(%rbp,%r11), %r11d
	movzbl	-1825(%rbp), %r9d
	andl	$15, %edi
	salq	$8, %rax
	movzbl	-320(%rbp,%rdi), %edi
	movzbl	-320(%rbp,%r10), %r10d
	orq	%rbx, %rax
	andl	$15, %r9d
	movzbl	-320(%rbp,%rsi), %esi
	movzbl	-320(%rbp,%rcx), %ecx
	salq	$8, %rax
	movzbl	-320(%rbp,%rdx), %edx
	movzbl	-320(%rbp,%r9), %r9d
	orq	%r11, %rax
	movq	-5880(%rbp), %rbx
	salq	$8, %rax
	salq	$8, %r9
	orq	%r10, %rax
	movzbl	-320(%rbp,%r13), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-320(%rbp,%r14), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-320(%rbp,%r15), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-320(%rbp,%r12), %r10d
	salq	$8, %rax
	orq	%rdi, %r9
	movq	-5808(%rbp), %rdi
	salq	$8, %r9
	orq	%r10, %rax
	orq	%rsi, %r9
	movq	-5744(%rbp), %rsi
	movq	%rax, -5744(%rbp)
	salq	$8, %r9
	movq	-5712(%rbp), %rax
	orq	%rcx, %r9
	movq	-5784(%rbp), %rcx
	salq	$8, %r9
	orq	%rdx, %r9
	movzbl	-320(%rbp,%rdi), %edx
	salq	$8, %r9
	orq	%rdx, %r9
	movzbl	-320(%rbp,%rcx), %edx
	salq	$8, %r9
	orq	%rdx, %r9
	movzbl	-320(%rbp,%rsi), %edx
	salq	$8, %r9
	orq	%rdx, %r9
	movq	%r9, -5736(%rbp)
	movdqa	-5744(%rbp), %xmm5
	movups	%xmm5, (%rbx,%rax,4)
	addq	%r8, %rax
	movq	%rax, -5712(%rbp)
.L557:
	movq	-5776(%rbp), %rbx
	leaq	-4(%rbx), %rax
	leaq	1(%rbx), %rdx
	movq	-5712(%rbp), %rbx
	andq	$-4, %rax
	addq	$4, %rax
	cmpq	$4, %rdx
	movl	$4, %edx
	cmovbe	%rdx, %rax
	salq	$2, %rbx
.L552:
	movq	-5776(%rbp), %rcx
	cmpq	%rax, %rcx
	je	.L566
	movq	-5768(%rbp), %rsi
	subq	%rax, %rcx
	movq	%rcx, %xmm1
	movdqu	(%rsi,%rax,4), %xmm3
	pshufd	$0, %xmm1, %xmm1
	pcmpgtd	-5920(%rbp), %xmm1
	movdqa	%xmm3, %xmm2
	movaps	%xmm3, -5744(%rbp)
	pcmpgtd	-5728(%rbp), %xmm2
	movaps	%xmm1, -5824(%rbp)
	movdqa	%xmm2, %xmm0
	movaps	%xmm2, -5840(%rbp)
	pandn	%xmm1, %xmm0
	movmskps	%xmm0, %r12d
	movq	%r12, %rdi
	salq	$4, %r12
	call	__popcountdi2@PLT
	movq	-5888(%rbp), %rsi
	movdqa	-5744(%rbp), %xmm3
	movl	%eax, -5776(%rbp)
	movslq	%eax, %r14
	movdqa	(%rsi,%r12), %xmm0
	movaps	%xmm3, -304(%rbp)
	movaps	%xmm0, -1968(%rbp)
	movzbl	-1960(%rbp), %edx
	movd	%xmm0, %r8d
	movaps	%xmm0, -1856(%rbp)
	movzbl	-1855(%rbp), %eax
	andl	$15, %r8d
	movaps	%xmm0, -1984(%rbp)
	movq	%rdx, %rcx
	movzbl	-1975(%rbp), %edx
	movaps	%xmm0, -1952(%rbp)
	movq	%rax, %rsi
	movzbl	-1945(%rbp), %eax
	andl	$15, %ecx
	movaps	%xmm0, -1936(%rbp)
	movq	%rdx, %rdi
	andl	$15, %esi
	movzbl	-1930(%rbp), %r13d
	andl	$15, %edi
	andl	$15, %eax
	movaps	%xmm0, -1872(%rbp)
	movzbl	-1870(%rbp), %r9d
	movaps	%xmm0, -1888(%rbp)
	andl	$15, %r13d
	movzbl	-1885(%rbp), %r10d
	movaps	%xmm0, -1904(%rbp)
	movzbl	-1900(%rbp), %r11d
	andl	$15, %r9d
	movaps	%xmm0, -1920(%rbp)
	movzbl	-1915(%rbp), %r12d
	andl	$15, %r10d
	movq	%rsi, -5744(%rbp)
	andl	$15, %r11d
	movq	%rcx, -5784(%rbp)
	andl	$15, %r12d
	movq	%rdi, -5808(%rbp)
	movaps	%xmm0, -2000(%rbp)
	movzbl	-1990(%rbp), %edx
	movaps	%xmm0, -2016(%rbp)
	movaps	%xmm0, -2032(%rbp)
	andl	$15, %edx
	movaps	%xmm0, -2048(%rbp)
	movzbl	-2035(%rbp), %ecx
	movaps	%xmm0, -2064(%rbp)
	movzbl	-2050(%rbp), %esi
	movaps	%xmm0, -2080(%rbp)
	movzbl	-304(%rbp,%rax), %eax
	movzbl	-2065(%rbp), %edi
	andl	$15, %ecx
	movzbl	-304(%rbp,%r13), %r13d
	movq	%rdx, -5792(%rbp)
	andl	$15, %esi
	salq	$8, %rax
	movzbl	-2005(%rbp), %edx
	andl	$15, %edi
	movzbl	-304(%rbp,%r12), %r12d
	orq	%r13, %rax
	movzbl	-304(%rbp,%r11), %r11d
	movzbl	-304(%rbp,%r10), %r10d
	salq	$8, %rax
	movzbl	-304(%rbp,%rdi), %edi
	movq	%rdx, %r15
	movzbl	-304(%rbp,%r9), %r9d
	orq	%r12, %rax
	movzbl	-2020(%rbp), %edx
	andl	$15, %r15d
	movzbl	-304(%rbp,%rsi), %esi
	salq	$8, %rax
	movzbl	-304(%rbp,%rcx), %ecx
	movzbl	-304(%rbp,%r8), %r8d
	orq	%r11, %rax
	movq	-5744(%rbp), %r11
	andl	$15, %edx
	salq	$8, %rax
	movzbl	-304(%rbp,%rdx), %edx
	orq	%r10, %rax
	salq	$8, %rax
	orq	%r9, %rax
	movzbl	-304(%rbp,%r11), %r9d
	salq	$8, %rax
	orq	%r9, %rax
	salq	$8, %rax
	salq	$8, %rdi
	orq	%rsi, %rdi
	movq	-5808(%rbp), %rsi
	salq	$8, %rdi
	orq	%rcx, %rdi
	movq	-5784(%rbp), %rcx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-304(%rbp,%r15), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movq	-5792(%rbp), %rdx
	salq	$8, %rdi
	movzbl	-304(%rbp,%rdx), %edx
	orq	%rdx, %rdi
	movzbl	-304(%rbp,%rsi), %edx
	movq	%rax, %rsi
	salq	$8, %rdi
	orq	%r8, %rsi
	orq	%rdx, %rdi
	movzbl	-304(%rbp,%rcx), %edx
	movd	-5776(%rbp), %xmm7
	movq	%rsi, -5744(%rbp)
	salq	$8, %rdi
	movdqa	-5824(%rbp), %xmm1
	movdqa	-5840(%rbp), %xmm2
	pshufd	$0, %xmm7, %xmm0
	orq	%rdx, %rdi
	pcmpgtd	-5920(%rbp), %xmm0
	movq	%rdi, -5736(%rbp)
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L567
	movq	-5696(%rbp), %rax
	movdqa	-5744(%rbp), %xmm7
	movd	%xmm7, (%rax)
.L567:
	pshufd	$85, %xmm0, %xmm4
	movd	%xmm4, %eax
	testl	%eax, %eax
	je	.L568
	pshufd	$85, -5744(%rbp), %xmm4
	movq	-5696(%rbp), %rax
	movd	%xmm4, 4(%rax)
.L568:
	movdqa	%xmm0, %xmm4
	punpckhdq	%xmm0, %xmm4
	movd	%xmm4, %eax
	testl	%eax, %eax
	je	.L569
	movdqa	-5744(%rbp), %xmm4
	movq	-5696(%rbp), %rax
	punpckhdq	%xmm4, %xmm4
	movd	%xmm4, 8(%rax)
.L569:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L570
	pshufd	$255, -5744(%rbp), %xmm0
	movq	-5696(%rbp), %rax
	movd	%xmm0, 12(%rax)
.L570:
	movq	-5696(%rbp), %rax
	pand	%xmm1, %xmm2
	movaps	%xmm3, -5744(%rbp)
	movmskps	%xmm2, %r12d
	leaq	(%rax,%r14,4), %rax
	movq	%r12, %rdi
	salq	$4, %r12
	movq	%rax, -5696(%rbp)
	call	__popcountdi2@PLT
	movdqa	-5744(%rbp), %xmm3
	movslq	%eax, %r8
	movq	-5888(%rbp), %rax
	movaps	%xmm3, -288(%rbp)
	movdqa	(%rax,%r12), %xmm0
	movaps	%xmm0, -2208(%rbp)
	movzbl	-2200(%rbp), %edx
	movd	%xmm0, %r9d
	movaps	%xmm0, -2224(%rbp)
	andl	$15, %r9d
	movq	%rdx, %rsi
	movzbl	-2215(%rbp), %edx
	movaps	%xmm0, -2096(%rbp)
	movzbl	-2095(%rbp), %eax
	movaps	%xmm0, -2240(%rbp)
	andl	$15, %esi
	movq	%rdx, %rcx
	movzbl	-2230(%rbp), %edx
	movq	%rax, %r15
	movaps	%xmm0, -2112(%rbp)
	movzbl	-2110(%rbp), %eax
	andl	$15, %ecx
	andl	$15, %r15d
	movaps	%xmm0, -2256(%rbp)
	movq	%rdx, %rdi
	movzbl	-2245(%rbp), %edx
	movaps	%xmm0, -2192(%rbp)
	andl	$15, %edi
	movq	%rax, %r14
	movzbl	-2185(%rbp), %eax
	movaps	%xmm0, -2176(%rbp)
	movq	%rdi, -5784(%rbp)
	movq	%rdx, %rdi
	andl	$15, %r14d
	movzbl	-2170(%rbp), %r13d
	andl	$15, %edi
	andl	$15, %eax
	movq	%rsi, -5744(%rbp)
	movq	%rcx, -5776(%rbp)
	andl	$15, %r13d
	movq	%rdi, -5808(%rbp)
	movaps	%xmm0, -2128(%rbp)
	movzbl	-2125(%rbp), %r10d
	movaps	%xmm0, -2144(%rbp)
	movzbl	-2140(%rbp), %r11d
	movaps	%xmm0, -2160(%rbp)
	movzbl	-2155(%rbp), %r12d
	andl	$15, %r10d
	movaps	%xmm0, -2272(%rbp)
	andl	$15, %r11d
	movzbl	-2260(%rbp), %edx
	movaps	%xmm0, -2288(%rbp)
	andl	$15, %r12d
	movzbl	-2275(%rbp), %ecx
	movaps	%xmm0, -2304(%rbp)
	movzbl	-2290(%rbp), %esi
	andl	$15, %edx
	movaps	%xmm0, -2320(%rbp)
	movzbl	-288(%rbp,%rax), %eax
	movzbl	-288(%rbp,%r13), %r13d
	andl	$15, %ecx
	movzbl	-288(%rbp,%r12), %r12d
	movzbl	-2305(%rbp), %edi
	andl	$15, %esi
	salq	$8, %rax
	movzbl	-288(%rbp,%rsi), %esi
	movzbl	-288(%rbp,%r11), %r11d
	orq	%r13, %rax
	andl	$15, %edi
	movzbl	-288(%rbp,%r10), %r10d
	movzbl	-288(%rbp,%rcx), %ecx
	salq	$8, %rax
	movzbl	-288(%rbp,%rdi), %edi
	movzbl	-288(%rbp,%rdx), %edx
	movzbl	-288(%rbp,%r9), %r9d
	orq	%r12, %rax
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r11, %rax
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-288(%rbp,%r14), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-288(%rbp,%r15), %r10d
	movq	-5808(%rbp), %r15
	salq	$8, %rax
	orq	%r10, %rax
	salq	$8, %rax
	orq	%rsi, %rdi
	movq	-5744(%rbp), %rsi
	salq	$8, %rdi
	orq	%r9, %rax
	orq	%rcx, %rdi
	movq	-5776(%rbp), %rcx
	movq	%rax, -5744(%rbp)
	salq	$8, %rdi
	movq	-5880(%rbp), %rax
	orq	%rdx, %rdi
	movzbl	-288(%rbp,%r15), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movq	-5784(%rbp), %rdx
	salq	$8, %rdi
	movzbl	-288(%rbp,%rdx), %edx
	orq	%rdx, %rdi
	movzbl	-288(%rbp,%rcx), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-288(%rbp,%rsi), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movq	%rdi, -5736(%rbp)
	movdqa	-5744(%rbp), %xmm6
	movups	%xmm6, (%rax,%rbx)
	addq	%r8, -5712(%rbp)
	movq	-5712(%rbp), %rax
	leaq	0(,%rax,4), %rbx
.L566:
	movq	-5768(%rbp), %rax
	movq	-5760(%rbp), %rdx
	movl	%ebx, %ecx
	subq	-5712(%rbp), %rdx
	leaq	(%rax,%rdx,4), %rax
	cmpl	$8, %ebx
	jnb	.L571
	testb	$4, %bl
	jne	.L773
	testl	%ecx, %ecx
	jne	.L774
.L572:
	movl	%ebx, %ecx
	cmpl	$8, %ebx
	jnb	.L575
	andl	$4, %ebx
	jne	.L775
	testl	%ecx, %ecx
	jne	.L776
.L576:
	movq	-5696(%rbp), %rbx
	movq	-5760(%rbp), %rdi
	movq	%rbx, %rax
	subq	-5768(%rbp), %rax
	sarq	$2, %rax
	subq	%rax, %rdi
	movq	%rax, -5976(%rbp)
	movq	%rdi, -5968(%rbp)
	movq	%rdx, %rdi
	subq	%rax, %rdi
	movq	%rdi, -5712(%rbp)
	leaq	(%rbx,%rdi,4), %rax
	je	.L616
	movdqu	(%rbx), %xmm5
	movdqu	16(%rbx), %xmm7
	leaq	-64(%rax), %rsi
	addq	$64, %rbx
	movdqu	-32(%rbx), %xmm6
	movq	%rsi, -5784(%rbp)
	movaps	%xmm5, -6000(%rbp)
	movdqu	-16(%rbx), %xmm5
	movaps	%xmm7, -6016(%rbp)
	movdqu	-64(%rax), %xmm7
	movaps	%xmm6, -6032(%rbp)
	movdqu	-48(%rax), %xmm6
	movaps	%xmm5, -6048(%rbp)
	movdqu	-32(%rax), %xmm5
	movaps	%xmm7, -6064(%rbp)
	movdqu	-16(%rax), %xmm7
	movq	%rbx, -5776(%rbp)
	movaps	%xmm6, -6080(%rbp)
	movaps	%xmm5, -6096(%rbp)
	movaps	%xmm7, -6112(%rbp)
	cmpq	%rsi, %rbx
	je	.L617
	leaq	_ZZN3hwy6N_SSE26detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %rax
	xorl	%r15d, %r15d
	movq	%rax, -5744(%rbp)
	jmp	.L583
	.p2align 4,,10
	.p2align 3
.L778:
	movq	-5784(%rbp), %rax
	movdqu	-64(%rax), %xmm4
	movdqu	-48(%rax), %xmm3
	prefetcht0	-256(%rax)
	subq	$64, %rax
	movdqu	32(%rax), %xmm2
	movdqu	48(%rax), %xmm1
	movq	%rax, -5784(%rbp)
.L582:
	movdqa	%xmm4, %xmm0
	movq	-5744(%rbp), %rbx
	movaps	%xmm1, -5872(%rbp)
	pcmpgtd	-5728(%rbp), %xmm0
	movaps	%xmm4, -272(%rbp)
	movaps	%xmm2, -5856(%rbp)
	movaps	%xmm3, -5840(%rbp)
	movmskps	%xmm0, %r14d
	movq	%r14, %rax
	salq	$4, %rax
	movdqa	(%rbx,%rax), %xmm0
	movaps	%xmm0, -2336(%rbp)
	movzbl	-2335(%rbp), %eax
	movd	%xmm0, %esi
	movaps	%xmm0, -2448(%rbp)
	movzbl	-2440(%rbp), %edx
	andl	$15, %esi
	andl	$15, %eax
	movaps	%xmm0, -2432(%rbp)
	movaps	%xmm0, -2464(%rbp)
	movq	%rdx, %rcx
	movzbl	-2455(%rbp), %edx
	movq	%rax, -5808(%rbp)
	movzbl	-2425(%rbp), %eax
	andl	$15, %ecx
	movaps	%xmm0, -2416(%rbp)
	movzbl	-2410(%rbp), %r13d
	andl	$15, %edx
	andl	$15, %eax
	movaps	%xmm0, -2352(%rbp)
	movzbl	-2350(%rbp), %r10d
	movaps	%xmm0, -2368(%rbp)
	andl	$15, %r13d
	movzbl	-2365(%rbp), %r11d
	movaps	%xmm0, -2384(%rbp)
	movzbl	-2380(%rbp), %ebx
	andl	$15, %r10d
	movaps	%xmm0, -2400(%rbp)
	movzbl	-2395(%rbp), %r12d
	andl	$15, %r11d
	movaps	%xmm0, -2480(%rbp)
	andl	$15, %ebx
	movaps	%xmm0, -2496(%rbp)
	andl	$15, %r12d
	movq	%rsi, -5760(%rbp)
	movq	%rdx, -5824(%rbp)
	movzbl	-2470(%rbp), %edx
	movq	%rcx, -5792(%rbp)
	movzbl	-2485(%rbp), %ecx
	movaps	%xmm0, -2512(%rbp)
	movzbl	-2500(%rbp), %esi
	andl	$15, %edx
	movaps	%xmm0, -2528(%rbp)
	movzbl	-2515(%rbp), %edi
	andl	$15, %ecx
	movaps	%xmm0, -2544(%rbp)
	movzbl	-2530(%rbp), %r8d
	andl	$15, %esi
	movaps	%xmm0, -2560(%rbp)
	movzbl	-272(%rbp,%rax), %eax
	andl	$15, %edi
	movzbl	-272(%rbp,%r13), %r13d
	movzbl	-272(%rbp,%r12), %r12d
	andl	$15, %r8d
	movzbl	-272(%rbp,%rbx), %ebx
	salq	$8, %rax
	movzbl	-2545(%rbp), %r9d
	movzbl	-272(%rbp,%r11), %r11d
	orq	%r13, %rax
	movzbl	-272(%rbp,%rdi), %edi
	movzbl	-272(%rbp,%r10), %r10d
	salq	$8, %rax
	andl	$15, %r9d
	movzbl	-272(%rbp,%r8), %r8d
	movzbl	-272(%rbp,%rsi), %esi
	orq	%r12, %rax
	movzbl	-272(%rbp,%rcx), %ecx
	movzbl	-272(%rbp,%r9), %r9d
	salq	$8, %rax
	movzbl	-272(%rbp,%rdx), %edx
	orq	%rbx, %rax
	salq	$8, %r9
	salq	$8, %rax
	orq	%r8, %r9
	orq	%r11, %rax
	movq	-5760(%rbp), %r11
	salq	$8, %rax
	orq	%r10, %rax
	movq	-5808(%rbp), %r10
	salq	$8, %rax
	movzbl	-272(%rbp,%r10), %r10d
	orq	%r10, %rax
	movzbl	-272(%rbp,%r11), %r10d
	salq	$8, %rax
	salq	$8, %r9
	orq	%rdi, %r9
	orq	%r10, %rax
	movq	%r14, %rdi
	salq	$8, %r9
	movq	%rax, -5760(%rbp)
	orq	%rsi, %r9
	salq	$8, %r9
	orq	%rcx, %r9
	movq	-5792(%rbp), %rcx
	salq	$8, %r9
	orq	%rdx, %r9
	movq	-5824(%rbp), %rdx
	salq	$8, %r9
	movzbl	-272(%rbp,%rdx), %edx
	orq	%rdx, %r9
	movzbl	-272(%rbp,%rcx), %edx
	salq	$8, %r9
	orq	%rdx, %r9
	movq	%r9, -5752(%rbp)
	call	__popcountdi2@PLT
	movdqa	-5840(%rbp), %xmm3
	movdqa	-5760(%rbp), %xmm6
	movq	-5696(%rbp), %rcx
	movq	-5712(%rbp), %rsi
	cltq
	movdqa	%xmm3, %xmm0
	movq	-5744(%rbp), %rbx
	movaps	%xmm3, -256(%rbp)
	pcmpgtd	-5728(%rbp), %xmm0
	movups	%xmm6, (%rcx,%r15,4)
	leaq	-4(%rsi,%r15), %rdx
	addq	$4, %r15
	subq	%rax, %r15
	movups	%xmm6, (%rcx,%rdx,4)
	movmskps	%xmm0, %eax
	movq	%rax, -5808(%rbp)
	salq	$4, %rax
	movdqa	(%rbx,%rax), %xmm0
	movd	%xmm0, %edx
	movaps	%xmm0, -2576(%rbp)
	movzbl	-2575(%rbp), %eax
	andl	$15, %edx
	movaps	%xmm0, -2688(%rbp)
	movq	%rdx, -5760(%rbp)
	movzbl	-2680(%rbp), %edx
	movq	%rax, %r11
	movaps	%xmm0, -2672(%rbp)
	movzbl	-2665(%rbp), %eax
	andl	$15, %r11d
	movaps	%xmm0, -2656(%rbp)
	movq	%rdx, %r8
	movzbl	-2650(%rbp), %r14d
	andl	$15, %r8d
	andl	$15, %eax
	movaps	%xmm0, -2592(%rbp)
	movaps	%xmm0, -2608(%rbp)
	andl	$15, %r14d
	movzbl	-2605(%rbp), %ebx
	movaps	%xmm0, -2624(%rbp)
	movzbl	-2620(%rbp), %r12d
	movaps	%xmm0, -2640(%rbp)
	movzbl	-2635(%rbp), %r13d
	andl	$15, %ebx
	movaps	%xmm0, -2704(%rbp)
	andl	$15, %r12d
	movq	%r11, -5792(%rbp)
	movzbl	-2590(%rbp), %r11d
	andl	$15, %r13d
	movq	%r8, -5824(%rbp)
	movzbl	-2695(%rbp), %edx
	movaps	%xmm0, -2720(%rbp)
	movzbl	-2710(%rbp), %ecx
	andl	$15, %r11d
	movaps	%xmm0, -2736(%rbp)
	movzbl	-2725(%rbp), %esi
	andl	$15, %edx
	movaps	%xmm0, -2752(%rbp)
	movzbl	-2740(%rbp), %edi
	andl	$15, %ecx
	movaps	%xmm0, -2768(%rbp)
	movzbl	-2755(%rbp), %r8d
	andl	$15, %esi
	movaps	%xmm0, -2784(%rbp)
	movzbl	-2770(%rbp), %r9d
	andl	$15, %edi
	movaps	%xmm0, -2800(%rbp)
	movzbl	-2785(%rbp), %r10d
	movzbl	-256(%rbp,%rax), %eax
	andl	$15, %r8d
	movzbl	-256(%rbp,%r14), %r14d
	andl	$15, %r9d
	movzbl	-256(%rbp,%r13), %r13d
	andl	$15, %r10d
	salq	$8, %rax
	movzbl	-256(%rbp,%r9), %r9d
	movzbl	-256(%rbp,%r12), %r12d
	orq	%r14, %rax
	movzbl	-256(%rbp,%rbx), %ebx
	movzbl	-256(%rbp,%r10), %r10d
	salq	$8, %rax
	movq	-5760(%rbp), %r14
	movzbl	-256(%rbp,%r8), %r8d
	orq	%r13, %rax
	salq	$8, %r10
	movzbl	-256(%rbp,%rdi), %edi
	movzbl	-256(%rbp,%r11), %r11d
	orq	%r9, %r10
	salq	$8, %rax
	movzbl	-256(%rbp,%rsi), %esi
	movzbl	-256(%rbp,%rcx), %ecx
	orq	%r12, %rax
	salq	$8, %r10
	movzbl	-256(%rbp,%rdx), %edx
	orq	%r8, %r10
	salq	$8, %rax
	movq	-5824(%rbp), %r8
	orq	%rbx, %rax
	salq	$8, %r10
	orq	%rdi, %r10
	salq	$8, %rax
	orq	%r11, %rax
	salq	$8, %r10
	movq	-5792(%rbp), %r11
	orq	%rsi, %r10
	salq	$8, %rax
	movzbl	-256(%rbp,%r11), %r11d
	salq	$8, %r10
	orq	%rcx, %r10
	orq	%r11, %rax
	salq	$8, %r10
	movzbl	-256(%rbp,%r14), %r11d
	salq	$8, %rax
	orq	%rdx, %r10
	movzbl	-256(%rbp,%r8), %edx
	movq	-5808(%rbp), %rdi
	salq	$8, %r10
	movq	%rax, %r14
	orq	%r11, %r14
	orq	%rdx, %r10
	movq	%r14, -5760(%rbp)
	movq	%r10, -5752(%rbp)
	call	__popcountdi2@PLT
	movdqa	-5856(%rbp), %xmm2
	movq	-5712(%rbp), %rsi
	movq	-5696(%rbp), %rcx
	cltq
	movdqa	-5760(%rbp), %xmm6
	leaq	-8(%rsi,%r15), %rdx
	movdqa	%xmm2, %xmm0
	movq	-5744(%rbp), %rdi
	movaps	%xmm2, -240(%rbp)
	movups	%xmm6, (%rcx,%r15,4)
	pcmpgtd	-5728(%rbp), %xmm0
	movups	%xmm6, (%rcx,%rdx,4)
	movl	$4, %edx
	subq	%rax, %rdx
	movq	%rdx, %rbx
	addq	%r15, %rbx
	movmskps	%xmm0, %r15d
	movq	%r15, %rax
	salq	$4, %rax
	movdqa	(%rdi,%rax), %xmm0
	movaps	%xmm0, -2816(%rbp)
	movd	%xmm0, %edx
	movzbl	-2815(%rbp), %eax
	andl	$15, %edx
	movaps	%xmm0, -2928(%rbp)
	andl	$15, %eax
	movq	%rdx, -5760(%rbp)
	movzbl	-2920(%rbp), %edx
	movaps	%xmm0, -2912(%rbp)
	movq	%rax, -5808(%rbp)
	movzbl	-2905(%rbp), %eax
	movq	%rdx, %r8
	movaps	%xmm0, -2896(%rbp)
	movzbl	-2890(%rbp), %r14d
	andl	$15, %r8d
	andl	$15, %eax
	movaps	%xmm0, -2832(%rbp)
	movzbl	-2830(%rbp), %r10d
	movaps	%xmm0, -2848(%rbp)
	andl	$15, %r14d
	movzbl	-2845(%rbp), %r11d
	movaps	%xmm0, -2864(%rbp)
	movzbl	-2860(%rbp), %r12d
	andl	$15, %r10d
	movaps	%xmm0, -2880(%rbp)
	movzbl	-2875(%rbp), %r13d
	andl	$15, %r11d
	movaps	%xmm0, -2944(%rbp)
	andl	$15, %r12d
	movq	%r8, -5792(%rbp)
	movzbl	-2935(%rbp), %edx
	andl	$15, %r13d
	movaps	%xmm0, -2960(%rbp)
	movaps	%xmm0, -2976(%rbp)
	movq	%rdx, %r9
	movzbl	-2965(%rbp), %ecx
	movzbl	-2950(%rbp), %edx
	movaps	%xmm0, -2992(%rbp)
	andl	$15, %r9d
	movzbl	-2980(%rbp), %esi
	movaps	%xmm0, -3008(%rbp)
	andl	$15, %edx
	andl	$15, %ecx
	movzbl	-2995(%rbp), %edi
	movaps	%xmm0, -3024(%rbp)
	movzbl	-3010(%rbp), %r8d
	andl	$15, %esi
	movaps	%xmm0, -3040(%rbp)
	movzbl	-240(%rbp,%rax), %eax
	andl	$15, %edi
	movzbl	-240(%rbp,%r14), %r14d
	movzbl	-240(%rbp,%r13), %r13d
	movq	%r9, -5824(%rbp)
	andl	$15, %r8d
	salq	$8, %rax
	movzbl	-3025(%rbp), %r9d
	movzbl	-240(%rbp,%r12), %r12d
	orq	%r14, %rax
	movq	-5760(%rbp), %r14
	movzbl	-240(%rbp,%r11), %r11d
	salq	$8, %rax
	andl	$15, %r9d
	movzbl	-240(%rbp,%r10), %r10d
	movzbl	-240(%rbp,%r8), %r8d
	orq	%r13, %rax
	movzbl	-240(%rbp,%rdi), %edi
	movzbl	-240(%rbp,%r9), %r9d
	salq	$8, %rax
	movzbl	-240(%rbp,%rsi), %esi
	movzbl	-240(%rbp,%rcx), %ecx
	orq	%r12, %rax
	salq	$8, %r9
	movzbl	-240(%rbp,%rdx), %edx
	salq	$8, %rax
	orq	%r8, %r9
	orq	%r11, %rax
	movq	-5808(%rbp), %r11
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-240(%rbp,%r11), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-240(%rbp,%r14), %r10d
	salq	$8, %rax
	salq	$8, %r9
	orq	%rdi, %r9
	orq	%r10, %rax
	movq	%r15, %rdi
	salq	$8, %r9
	orq	%rsi, %r9
	salq	$8, %r9
	orq	%rcx, %r9
	salq	$8, %r9
	orq	%rdx, %r9
	movq	-5824(%rbp), %rdx
	salq	$8, %r9
	movzbl	-240(%rbp,%rdx), %edx
	movq	-5792(%rbp), %r8
	movq	%rax, -5760(%rbp)
	orq	%rdx, %r9
	movzbl	-240(%rbp,%r8), %edx
	salq	$8, %r9
	orq	%rdx, %r9
	movq	%r9, -5752(%rbp)
	call	__popcountdi2@PLT
	movdqa	-5872(%rbp), %xmm1
	movq	-5712(%rbp), %rsi
	movq	-5696(%rbp), %rcx
	cltq
	movdqa	-5760(%rbp), %xmm6
	movdqa	%xmm1, %xmm0
	leaq	-12(%rsi,%rbx), %rdx
	movq	-5744(%rbp), %rdi
	subq	$16, %rsi
	pcmpgtd	-5728(%rbp), %xmm0
	movups	%xmm6, (%rcx,%rbx,4)
	movups	%xmm6, (%rcx,%rdx,4)
	movl	$4, %edx
	subq	%rax, %rdx
	movq	%rsi, -5712(%rbp)
	movmskps	%xmm0, %r15d
	addq	%rdx, %rbx
	movaps	%xmm1, -224(%rbp)
	movq	%r15, %rax
	salq	$4, %rax
	movdqa	(%rdi,%rax), %xmm0
	movaps	%xmm0, -3056(%rbp)
	movzbl	-3055(%rbp), %eax
	movd	%xmm0, %edi
	movaps	%xmm0, -3168(%rbp)
	movzbl	-3160(%rbp), %edx
	andl	$15, %edi
	andl	$15, %eax
	movaps	%xmm0, -3072(%rbp)
	movzbl	-3070(%rbp), %r10d
	andl	$15, %edx
	movaps	%xmm0, -3088(%rbp)
	movzbl	-3085(%rbp), %r11d
	movaps	%xmm0, -3104(%rbp)
	movzbl	-3100(%rbp), %r12d
	andl	$15, %r10d
	movaps	%xmm0, -3120(%rbp)
	movzbl	-3115(%rbp), %r13d
	andl	$15, %r11d
	movaps	%xmm0, -3136(%rbp)
	movzbl	-3130(%rbp), %r14d
	andl	$15, %r12d
	movaps	%xmm0, -3152(%rbp)
	andl	$15, %r13d
	movq	%rdi, -5760(%rbp)
	andl	$15, %r14d
	movq	%rax, -5808(%rbp)
	movzbl	-3145(%rbp), %eax
	movq	%rdx, -5792(%rbp)
	movaps	%xmm0, -3184(%rbp)
	movzbl	-3175(%rbp), %edx
	andl	$15, %eax
	movaps	%xmm0, -3200(%rbp)
	movq	%rdx, %r8
	movaps	%xmm0, -3216(%rbp)
	movzbl	-3190(%rbp), %edx
	movzbl	-3205(%rbp), %ecx
	andl	$15, %r8d
	movaps	%xmm0, -3232(%rbp)
	movzbl	-3220(%rbp), %esi
	movaps	%xmm0, -3248(%rbp)
	andl	$15, %edx
	andl	$15, %ecx
	movzbl	-3235(%rbp), %edi
	movaps	%xmm0, -3264(%rbp)
	andl	$15, %esi
	movaps	%xmm0, -3280(%rbp)
	movzbl	-3265(%rbp), %r9d
	andl	$15, %edi
	movzbl	-224(%rbp,%rax), %eax
	movq	%r8, -5824(%rbp)
	movzbl	-3250(%rbp), %r8d
	movzbl	-224(%rbp,%rdi), %edi
	movzbl	-224(%rbp,%r14), %r14d
	andl	$15, %r8d
	andl	$15, %r9d
	salq	$8, %rax
	movzbl	-224(%rbp,%r13), %r13d
	movzbl	-224(%rbp,%r9), %r9d
	orq	%r14, %rax
	movzbl	-224(%rbp,%r8), %r8d
	salq	$8, %rax
	movq	-5760(%rbp), %r14
	movzbl	-224(%rbp,%r12), %r12d
	salq	$8, %r9
	orq	%r13, %rax
	movzbl	-224(%rbp,%rsi), %esi
	movzbl	-224(%rbp,%r11), %r11d
	orq	%r8, %r9
	salq	$8, %rax
	movzbl	-224(%rbp,%rcx), %ecx
	movzbl	-224(%rbp,%r10), %r10d
	salq	$8, %r9
	orq	%r12, %rax
	movzbl	-224(%rbp,%rdx), %edx
	movq	-5824(%rbp), %r8
	orq	%rdi, %r9
	salq	$8, %rax
	movq	%r15, %rdi
	movl	$4, %r15d
	salq	$8, %r9
	orq	%r11, %rax
	movq	-5808(%rbp), %r11
	orq	%rsi, %r9
	salq	$8, %rax
	salq	$8, %r9
	orq	%r10, %rax
	movzbl	-224(%rbp,%r11), %r10d
	orq	%rcx, %r9
	salq	$8, %rax
	salq	$8, %r9
	orq	%r10, %rax
	movzbl	-224(%rbp,%r14), %r10d
	orq	%rdx, %r9
	salq	$8, %rax
	movzbl	-224(%rbp,%r8), %edx
	salq	$8, %r9
	orq	%r10, %rax
	orq	%rdx, %r9
	movq	-5792(%rbp), %rdx
	movq	%rax, -5760(%rbp)
	salq	$8, %r9
	movzbl	-224(%rbp,%rdx), %edx
	orq	%rdx, %r9
	movq	%r9, -5752(%rbp)
	call	__popcountdi2@PLT
	movq	-5712(%rbp), %rsi
	movq	-5696(%rbp), %rcx
	movdqa	-5760(%rbp), %xmm7
	cltq
	leaq	(%rbx,%rsi), %rdx
	subq	%rax, %r15
	movups	%xmm7, (%rcx,%rbx,4)
	addq	%rbx, %r15
	movq	-5784(%rbp), %rbx
	movups	%xmm7, (%rcx,%rdx,4)
	cmpq	%rbx, -5776(%rbp)
	je	.L777
.L583:
	movq	-5776(%rbp), %rax
	subq	-5696(%rbp), %rax
	sarq	$2, %rax
	subq	%r15, %rax
	cmpq	$16, %rax
	ja	.L778
	movq	-5776(%rbp), %rax
	movdqu	(%rax), %xmm4
	movdqu	16(%rax), %xmm3
	prefetcht0	256(%rax)
	addq	$64, %rax
	movdqu	-32(%rax), %xmm2
	movdqu	-16(%rax), %xmm1
	movq	%rax, -5776(%rbp)
	jmp	.L582
	.p2align 4,,10
	.p2align 3
.L770:
	movq	-5896(%rbp), %rbx
	movl	$16, %edx
	movq	%r8, %r15
	subq	%rax, %rdx
	leaq	(%rdi,%rdx,4), %r12
	leaq	-16(%rax,%rbx), %r11
	jmp	.L496
	.p2align 4,,10
	.p2align 3
.L777:
	leaq	(%rsi,%r15), %r13
	leaq	(%rcx,%r15,4), %r14
	leaq	4(%r15), %rbx
.L580:
	movdqa	-6000(%rbp), %xmm6
	movq	-5744(%rbp), %rdi
	movdqa	%xmm6, %xmm0
	movaps	%xmm6, -208(%rbp)
	pcmpgtd	-5728(%rbp), %xmm0
	movmskps	%xmm0, %r12d
	movq	%r12, %rax
	salq	$4, %rax
	movdqa	(%rdi,%rax), %xmm0
	movaps	%xmm0, -3296(%rbp)
	movzbl	-3295(%rbp), %eax
	movd	%xmm0, %r8d
	movaps	%xmm0, -3408(%rbp)
	movzbl	-3400(%rbp), %edx
	andl	$15, %r8d
	andl	$15, %eax
	movaps	%xmm0, -3312(%rbp)
	andl	$15, %edx
	movq	%rax, -5760(%rbp)
	movzbl	-3310(%rbp), %eax
	movaps	%xmm0, -3424(%rbp)
	movq	%rdx, -5808(%rbp)
	movzbl	-3415(%rbp), %edx
	movq	%rax, %rcx
	movaps	%xmm0, -3328(%rbp)
	movzbl	-3325(%rbp), %eax
	andl	$15, %ecx
	movaps	%xmm0, -3440(%rbp)
	movq	%rdx, %r15
	movzbl	-3430(%rbp), %edx
	movaps	%xmm0, -3392(%rbp)
	movq	%rax, %rsi
	andl	$15, %r15d
	movzbl	-3385(%rbp), %eax
	movaps	%xmm0, -3376(%rbp)
	movzbl	-3370(%rbp), %r11d
	andl	$15, %esi
	movq	%rcx, -5776(%rbp)
	movq	%rdx, %rcx
	andl	$15, %eax
	andl	$15, %ecx
	movaps	%xmm0, -3344(%rbp)
	andl	$15, %r11d
	movzbl	-3340(%rbp), %r9d
	movaps	%xmm0, -3360(%rbp)
	movzbl	-3355(%rbp), %r10d
	movaps	%xmm0, -3456(%rbp)
	andl	$15, %r9d
	movq	%rsi, -5784(%rbp)
	andl	$15, %r10d
	movq	%r15, -5792(%rbp)
	movq	%rcx, -5824(%rbp)
	movzbl	-3445(%rbp), %edx
	movaps	%xmm0, -3472(%rbp)
	movaps	%xmm0, -3488(%rbp)
	movq	%rdx, %rsi
	movzbl	-3475(%rbp), %ecx
	movzbl	-3460(%rbp), %edx
	movaps	%xmm0, -3504(%rbp)
	andl	$15, %esi
	movaps	%xmm0, -3520(%rbp)
	movq	%rsi, %r15
	andl	$15, %ecx
	andl	$15, %edx
	movzbl	-208(%rbp,%rax), %eax
	movzbl	-3505(%rbp), %edi
	movzbl	-208(%rbp,%r11), %r11d
	movzbl	-3490(%rbp), %esi
	salq	$8, %rax
	andl	$15, %edi
	movzbl	-208(%rbp,%r10), %r10d
	movzbl	-208(%rbp,%r9), %r9d
	orq	%r11, %rax
	movq	-5760(%rbp), %r11
	movzbl	-208(%rbp,%rdi), %edi
	andl	$15, %esi
	salq	$8, %rax
	movzbl	-208(%rbp,%rsi), %esi
	movzbl	-208(%rbp,%rcx), %ecx
	orq	%r10, %rax
	movq	-5776(%rbp), %r10
	salq	$8, %rdi
	movzbl	-208(%rbp,%rdx), %edx
	movzbl	-208(%rbp,%r8), %r8d
	salq	$8, %rax
	orq	%rsi, %rdi
	orq	%r9, %rax
	movq	-5784(%rbp), %r9
	salq	$8, %rax
	movzbl	-208(%rbp,%r9), %r9d
	orq	%r9, %rax
	movzbl	-208(%rbp,%r10), %r9d
	salq	$8, %rax
	orq	%r9, %rax
	movzbl	-208(%rbp,%r11), %r9d
	salq	$8, %rax
	orq	%r9, %rax
	salq	$8, %rax
	salq	$8, %rdi
	orq	%rcx, %rdi
	movq	-5824(%rbp), %rcx
	orq	%r8, %rax
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-208(%rbp,%r15), %edx
	movq	-5792(%rbp), %r15
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-208(%rbp,%rcx), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-208(%rbp,%r15), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movq	-5808(%rbp), %rdx
	salq	$8, %rdi
	movzbl	-208(%rbp,%rdx), %edx
	movq	%rax, -5760(%rbp)
	movq	%rdi, %rax
	movq	%r12, %rdi
	orq	%rdx, %rax
	movq	%rax, -5752(%rbp)
	call	__popcountdi2@PLT
	movdqa	-6016(%rbp), %xmm7
	movq	-5744(%rbp), %rdi
	cltq
	movdqa	-5760(%rbp), %xmm6
	movq	-5696(%rbp), %rsi
	movdqa	%xmm7, %xmm0
	subq	%rax, %rbx
	movaps	%xmm7, -192(%rbp)
	pcmpgtd	-5728(%rbp), %xmm0
	movups	%xmm6, (%r14)
	movups	%xmm6, -16(%rsi,%r13,4)
	movmskps	%xmm0, %r11d
	movq	%r11, %rax
	salq	$4, %rax
	movdqa	(%rdi,%rax), %xmm0
	movaps	%xmm0, -3648(%rbp)
	movzbl	-3640(%rbp), %edx
	movd	%xmm0, %r8d
	movaps	%xmm0, -3536(%rbp)
	movzbl	-3535(%rbp), %eax
	andl	$15, %r8d
	movaps	%xmm0, -3664(%rbp)
	movq	%rdx, %rcx
	movzbl	-3655(%rbp), %edx
	movaps	%xmm0, -3552(%rbp)
	movq	%rax, %r15
	andl	$15, %ecx
	movzbl	-3550(%rbp), %eax
	andl	$15, %edx
	movaps	%xmm0, -3568(%rbp)
	andl	$15, %r15d
	movaps	%xmm0, -3680(%rbp)
	movq	%rax, %r14
	movzbl	-3565(%rbp), %eax
	movq	%rdx, -5776(%rbp)
	movzbl	-3670(%rbp), %edx
	andl	$15, %r14d
	movaps	%xmm0, -3584(%rbp)
	movq	%rax, %r13
	movzbl	-3580(%rbp), %eax
	movq	%rcx, -5760(%rbp)
	movq	%rdx, %rcx
	andl	$15, %r13d
	andl	$15, %ecx
	movaps	%xmm0, -3600(%rbp)
	movq	%rax, %r12
	movzbl	-3595(%rbp), %r9d
	movaps	%xmm0, -3616(%rbp)
	movzbl	-3610(%rbp), %r10d
	andl	$15, %r12d
	movaps	%xmm0, -3632(%rbp)
	movzbl	-3625(%rbp), %eax
	andl	$15, %r9d
	movq	%rcx, -5784(%rbp)
	andl	$15, %r10d
	movaps	%xmm0, -3696(%rbp)
	movzbl	-3685(%rbp), %edx
	andl	$15, %eax
	movaps	%xmm0, -3712(%rbp)
	andl	$15, %edx
	movaps	%xmm0, -3728(%rbp)
	movzbl	-3715(%rbp), %ecx
	movaps	%xmm0, -3744(%rbp)
	movzbl	-3730(%rbp), %esi
	movaps	%xmm0, -3760(%rbp)
	movzbl	-3745(%rbp), %edi
	movzbl	-192(%rbp,%rax), %eax
	andl	$15, %ecx
	movq	%rdx, -5808(%rbp)
	movzbl	-3700(%rbp), %edx
	andl	$15, %esi
	movzbl	-192(%rbp,%r10), %r10d
	andl	$15, %edi
	movzbl	-192(%rbp,%r9), %r9d
	andl	$15, %edx
	salq	$8, %rax
	movzbl	-192(%rbp,%rdi), %edi
	movzbl	-192(%rbp,%rsi), %esi
	orq	%r10, %rax
	movzbl	-192(%rbp,%rcx), %ecx
	movzbl	-192(%rbp,%rdx), %edx
	salq	$8, %rax
	salq	$8, %rdi
	movzbl	-192(%rbp,%r8), %r8d
	orq	%r9, %rax
	orq	%rsi, %rdi
	movzbl	-192(%rbp,%r12), %r9d
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r9, %rax
	orq	%rcx, %rdi
	movq	-5760(%rbp), %rcx
	movzbl	-192(%rbp,%r13), %r9d
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r9, %rax
	orq	%rdx, %rdi
	movzbl	-192(%rbp,%r14), %r9d
	movq	-5808(%rbp), %r14
	salq	$8, %rax
	salq	$8, %rdi
	movzbl	-192(%rbp,%r14), %edx
	orq	%r9, %rax
	movzbl	-192(%rbp,%r15), %r9d
	movq	-5784(%rbp), %r15
	salq	$8, %rax
	orq	%rdx, %rdi
	orq	%r9, %rax
	movzbl	-192(%rbp,%r15), %edx
	salq	$8, %rdi
	salq	$8, %rax
	orq	%r8, %rax
	orq	%rdx, %rdi
	movq	-5776(%rbp), %rdx
	salq	$8, %rdi
	movzbl	-192(%rbp,%rdx), %edx
	orq	%rdx, %rdi
	movzbl	-192(%rbp,%rcx), %edx
	movq	%rax, -5760(%rbp)
	salq	$8, %rdi
	movq	%rdi, %rax
	movq	%r11, %rdi
	orq	%rdx, %rax
	movq	%rax, -5752(%rbp)
	call	__popcountdi2@PLT
	movq	-5712(%rbp), %rcx
	movdqa	-5760(%rbp), %xmm6
	movq	-5696(%rbp), %rsi
	cltq
	movq	-5744(%rbp), %rdi
	leaq	-8(%rcx,%rbx), %rdx
	movups	%xmm6, (%rsi,%rbx,4)
	subq	%rax, %rbx
	movups	%xmm6, (%rsi,%rdx,4)
	movdqa	-6032(%rbp), %xmm6
	addq	$4, %rbx
	movdqa	%xmm6, %xmm0
	movaps	%xmm6, -176(%rbp)
	pcmpgtd	-5728(%rbp), %xmm0
	movmskps	%xmm0, %r11d
	movq	%r11, %rax
	salq	$4, %rax
	movdqa	(%rdi,%rax), %xmm0
	movaps	%xmm0, -3776(%rbp)
	movzbl	-3775(%rbp), %eax
	movd	%xmm0, %r8d
	movaps	%xmm0, -3792(%rbp)
	andl	$15, %r8d
	movq	%rax, %r15
	movzbl	-3790(%rbp), %eax
	movaps	%xmm0, -3888(%rbp)
	movzbl	-3880(%rbp), %edx
	movaps	%xmm0, -3808(%rbp)
	andl	$15, %r15d
	movq	%rax, %r14
	movzbl	-3805(%rbp), %eax
	andl	$15, %edx
	movaps	%xmm0, -3824(%rbp)
	movaps	%xmm0, -3904(%rbp)
	andl	$15, %r14d
	movq	%rdx, -5760(%rbp)
	movq	%rax, %r13
	movzbl	-3895(%rbp), %edx
	movzbl	-3820(%rbp), %eax
	movaps	%xmm0, -3872(%rbp)
	andl	$15, %r13d
	movq	%rdx, %rcx
	movaps	%xmm0, -3856(%rbp)
	movzbl	-3850(%rbp), %r10d
	movq	%rax, %r12
	movzbl	-3865(%rbp), %eax
	andl	$15, %ecx
	movaps	%xmm0, -3840(%rbp)
	movaps	%xmm0, -3920(%rbp)
	movzbl	-3835(%rbp), %r9d
	andl	$15, %r10d
	andl	$15, %r12d
	movq	%rcx, -5776(%rbp)
	andl	$15, %eax
	movzbl	-3910(%rbp), %edx
	movaps	%xmm0, -3936(%rbp)
	andl	$15, %r9d
	movaps	%xmm0, -3952(%rbp)
	andl	$15, %edx
	movaps	%xmm0, -3968(%rbp)
	movzbl	-3955(%rbp), %ecx
	movaps	%xmm0, -3984(%rbp)
	movzbl	-3970(%rbp), %esi
	movaps	%xmm0, -4000(%rbp)
	movzbl	-176(%rbp,%rax), %eax
	andl	$15, %ecx
	movzbl	-176(%rbp,%r10), %r10d
	movq	%rdx, -5784(%rbp)
	movzbl	-3925(%rbp), %edx
	andl	$15, %esi
	salq	$8, %rax
	movzbl	-176(%rbp,%rsi), %esi
	movzbl	-176(%rbp,%r9), %r9d
	orq	%r10, %rax
	movq	%rdx, %rdi
	movzbl	-3940(%rbp), %edx
	movzbl	-176(%rbp,%rcx), %ecx
	movzbl	-176(%rbp,%r8), %r8d
	salq	$8, %rax
	andl	$15, %edi
	orq	%r9, %rax
	movq	%rdi, -5808(%rbp)
	andl	$15, %edx
	movzbl	-176(%rbp,%r12), %r9d
	movzbl	-3985(%rbp), %edi
	salq	$8, %rax
	movq	-5808(%rbp), %r10
	orq	%r9, %rax
	movzbl	-176(%rbp,%rdx), %edx
	movzbl	-176(%rbp,%r13), %r9d
	andl	$15, %edi
	salq	$8, %rax
	orq	%r9, %rax
	movzbl	-176(%rbp,%rdi), %edi
	movzbl	-176(%rbp,%r14), %r9d
	salq	$8, %rax
	movq	-5784(%rbp), %r14
	orq	%r9, %rax
	salq	$8, %rdi
	movzbl	-176(%rbp,%r15), %r9d
	movq	-5776(%rbp), %r15
	orq	%rsi, %rdi
	salq	$8, %rax
	orq	%r9, %rax
	salq	$8, %rdi
	salq	$8, %rax
	orq	%rcx, %rdi
	salq	$8, %rdi
	orq	%r8, %rax
	orq	%rdx, %rdi
	movzbl	-176(%rbp,%r10), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-176(%rbp,%r14), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-176(%rbp,%r15), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movq	-5760(%rbp), %rdx
	salq	$8, %rdi
	movzbl	-176(%rbp,%rdx), %edx
	movq	%rax, -5760(%rbp)
	movq	%rdi, %rax
	movq	%r11, %rdi
	orq	%rdx, %rax
	movq	%rax, -5752(%rbp)
	call	__popcountdi2@PLT
	movdqa	-6048(%rbp), %xmm7
	movq	-5712(%rbp), %rcx
	movq	-5696(%rbp), %rsi
	cltq
	movdqa	-5760(%rbp), %xmm6
	movdqa	%xmm7, %xmm0
	leaq	-12(%rcx,%rbx), %rdx
	movq	-5744(%rbp), %rdi
	movaps	%xmm7, -160(%rbp)
	pcmpgtd	-5728(%rbp), %xmm0
	movups	%xmm6, (%rsi,%rbx,4)
	movups	%xmm6, (%rsi,%rdx,4)
	movl	$4, %edx
	subq	%rax, %rdx
	movmskps	%xmm0, %r11d
	addq	%rdx, %rbx
	movq	%r11, %rax
	salq	$4, %rax
	movdqa	(%rdi,%rax), %xmm0
	movaps	%xmm0, -4016(%rbp)
	movzbl	-4015(%rbp), %eax
	movd	%xmm0, %r8d
	movaps	%xmm0, -4032(%rbp)
	andl	$15, %r8d
	movaps	%xmm0, -4128(%rbp)
	movq	%rax, %r15
	movzbl	-4120(%rbp), %edx
	movzbl	-4030(%rbp), %eax
	movaps	%xmm0, -4048(%rbp)
	andl	$15, %r15d
	andl	$15, %edx
	movq	%rax, %r14
	movzbl	-4045(%rbp), %eax
	movaps	%xmm0, -4144(%rbp)
	movq	%rdx, -5760(%rbp)
	movzbl	-4135(%rbp), %edx
	andl	$15, %r14d
	movaps	%xmm0, -4064(%rbp)
	movq	%rax, %r13
	movzbl	-4060(%rbp), %eax
	andl	$15, %edx
	movaps	%xmm0, -4080(%rbp)
	andl	$15, %r13d
	movzbl	-4075(%rbp), %r9d
	movaps	%xmm0, -4096(%rbp)
	movq	%rax, %r12
	movzbl	-4090(%rbp), %r10d
	movaps	%xmm0, -4112(%rbp)
	movzbl	-4105(%rbp), %eax
	andl	$15, %r12d
	andl	$15, %r9d
	movq	%rdx, -5776(%rbp)
	andl	$15, %r10d
	movaps	%xmm0, -4160(%rbp)
	movzbl	-4150(%rbp), %edx
	andl	$15, %eax
	movaps	%xmm0, -4176(%rbp)
	movq	%rdx, %rcx
	movzbl	-4165(%rbp), %edx
	movaps	%xmm0, -4240(%rbp)
	andl	$15, %ecx
	movaps	%xmm0, -4192(%rbp)
	movq	%rdx, %rdi
	movaps	%xmm0, -4208(%rbp)
	movzbl	-4180(%rbp), %edx
	andl	$15, %edi
	movaps	%xmm0, -4224(%rbp)
	movzbl	-4210(%rbp), %esi
	movzbl	-160(%rbp,%rax), %eax
	movq	%rdi, -5808(%rbp)
	movzbl	-4225(%rbp), %edi
	andl	$15, %edx
	movq	%rcx, -5784(%rbp)
	movzbl	-4195(%rbp), %ecx
	andl	$15, %esi
	movzbl	-160(%rbp,%r10), %r10d
	andl	$15, %edi
	movzbl	-160(%rbp,%rsi), %esi
	movzbl	-160(%rbp,%rdi), %edi
	andl	$15, %ecx
	salq	$8, %rax
	movzbl	-160(%rbp,%r9), %r9d
	orq	%r10, %rax
	movq	-5808(%rbp), %r10
	movzbl	-160(%rbp,%rcx), %ecx
	salq	$8, %rax
	salq	$8, %rdi
	movzbl	-160(%rbp,%rdx), %edx
	movzbl	-160(%rbp,%r8), %r8d
	orq	%rsi, %rdi
	orq	%r9, %rax
	movzbl	-160(%rbp,%r12), %r9d
	salq	$8, %rax
	salq	$8, %rdi
	orq	%rcx, %rdi
	orq	%r9, %rax
	movzbl	-160(%rbp,%r13), %r9d
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r9, %rax
	orq	%rdx, %rdi
	movzbl	-160(%rbp,%r14), %r9d
	movq	-5784(%rbp), %r14
	movzbl	-160(%rbp,%r10), %edx
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r9, %rax
	movzbl	-160(%rbp,%r15), %r9d
	movq	-5776(%rbp), %r15
	orq	%rdx, %rdi
	salq	$8, %rax
	movzbl	-160(%rbp,%r14), %edx
	salq	$8, %rdi
	orq	%r9, %rax
	orq	%rdx, %rdi
	salq	$8, %rax
	movzbl	-160(%rbp,%r15), %edx
	salq	$8, %rdi
	orq	%r8, %rax
	orq	%rdx, %rdi
	movq	-5760(%rbp), %rdx
	salq	$8, %rdi
	movzbl	-160(%rbp,%rdx), %edx
	movq	%rax, -5760(%rbp)
	movq	%rdi, %rax
	movq	%r11, %rdi
	orq	%rdx, %rax
	movq	%rax, -5752(%rbp)
	call	__popcountdi2@PLT
	movq	-5712(%rbp), %rcx
	movdqa	-5760(%rbp), %xmm6
	movq	-5696(%rbp), %rsi
	cltq
	movq	-5744(%rbp), %rdi
	leaq	-16(%rcx,%rbx), %rdx
	movups	%xmm6, (%rsi,%rbx,4)
	movups	%xmm6, (%rsi,%rdx,4)
	movdqa	-6064(%rbp), %xmm6
	movl	$4, %edx
	subq	%rax, %rdx
	movdqa	%xmm6, %xmm0
	addq	%rdx, %rbx
	movaps	%xmm6, -144(%rbp)
	pcmpgtd	-5728(%rbp), %xmm0
	movmskps	%xmm0, %r11d
	movq	%r11, %rax
	salq	$4, %rax
	movdqa	(%rdi,%rax), %xmm0
	movaps	%xmm0, -4256(%rbp)
	movzbl	-4255(%rbp), %eax
	movd	%xmm0, %r8d
	movaps	%xmm0, -4272(%rbp)
	andl	$15, %r8d
	movq	%rax, %r15
	movzbl	-4270(%rbp), %eax
	movaps	%xmm0, -4288(%rbp)
	movaps	%xmm0, -4368(%rbp)
	movzbl	-4360(%rbp), %edx
	andl	$15, %r15d
	movq	%rax, %r14
	movzbl	-4285(%rbp), %eax
	movaps	%xmm0, -4304(%rbp)
	andl	$15, %edx
	movaps	%xmm0, -4352(%rbp)
	andl	$15, %r14d
	movq	%rax, %r13
	movzbl	-4300(%rbp), %eax
	movaps	%xmm0, -4384(%rbp)
	movq	%rdx, -5760(%rbp)
	movzbl	-4375(%rbp), %edx
	andl	$15, %r13d
	movq	%rax, %r12
	movzbl	-4345(%rbp), %eax
	movaps	%xmm0, -4336(%rbp)
	movzbl	-4330(%rbp), %r10d
	andl	$15, %edx
	movaps	%xmm0, -4320(%rbp)
	andl	$15, %r12d
	movzbl	-4315(%rbp), %r9d
	andl	$15, %eax
	movq	%rdx, -5776(%rbp)
	andl	$15, %r10d
	movaps	%xmm0, -4400(%rbp)
	movzbl	-4390(%rbp), %edx
	andl	$15, %r9d
	movaps	%xmm0, -4416(%rbp)
	movaps	%xmm0, -4432(%rbp)
	movq	%rdx, %rcx
	movzbl	-4405(%rbp), %edx
	movaps	%xmm0, -4448(%rbp)
	andl	$15, %ecx
	movaps	%xmm0, -4464(%rbp)
	movq	%rdx, %rdi
	movzbl	-4450(%rbp), %esi
	movzbl	-4420(%rbp), %edx
	movaps	%xmm0, -4480(%rbp)
	movzbl	-144(%rbp,%rax), %eax
	andl	$15, %edi
	movzbl	-144(%rbp,%r10), %r10d
	movzbl	-144(%rbp,%r9), %r9d
	movq	%rdi, -5808(%rbp)
	andl	$15, %esi
	andl	$15, %edx
	salq	$8, %rax
	movzbl	-4465(%rbp), %edi
	movq	%rcx, -5784(%rbp)
	orq	%r10, %rax
	movzbl	-4435(%rbp), %ecx
	movzbl	-144(%rbp,%rsi), %esi
	salq	$8, %rax
	andl	$15, %edi
	movq	-5808(%rbp), %r10
	movzbl	-144(%rbp,%rdx), %edx
	orq	%r9, %rax
	andl	$15, %ecx
	movzbl	-144(%rbp,%r12), %r9d
	movzbl	-144(%rbp,%rdi), %edi
	salq	$8, %rax
	movzbl	-144(%rbp,%rcx), %ecx
	movzbl	-144(%rbp,%r8), %r8d
	orq	%r9, %rax
	salq	$8, %rdi
	movzbl	-144(%rbp,%r13), %r9d
	salq	$8, %rax
	orq	%rsi, %rdi
	orq	%r9, %rax
	salq	$8, %rdi
	movzbl	-144(%rbp,%r14), %r9d
	movq	-5784(%rbp), %r14
	salq	$8, %rax
	orq	%r9, %rax
	movzbl	-144(%rbp,%r15), %r9d
	movq	-5776(%rbp), %r15
	salq	$8, %rax
	orq	%r9, %rax
	salq	$8, %rax
	orq	%rcx, %rdi
	salq	$8, %rdi
	orq	%r8, %rax
	orq	%rdx, %rdi
	movzbl	-144(%rbp,%r10), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-144(%rbp,%r14), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-144(%rbp,%r15), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movq	-5760(%rbp), %rdx
	salq	$8, %rdi
	movzbl	-144(%rbp,%rdx), %edx
	movq	%rax, -5760(%rbp)
	movq	%rdi, %rax
	movq	%r11, %rdi
	orq	%rdx, %rax
	movq	%rax, -5752(%rbp)
	call	__popcountdi2@PLT
	movdqa	-6080(%rbp), %xmm7
	movq	-5712(%rbp), %rcx
	movq	-5696(%rbp), %rsi
	cltq
	movdqa	-5760(%rbp), %xmm6
	movdqa	%xmm7, %xmm0
	leaq	-20(%rcx,%rbx), %rdx
	movq	-5744(%rbp), %rdi
	movaps	%xmm7, -128(%rbp)
	pcmpgtd	-5728(%rbp), %xmm0
	movups	%xmm6, (%rsi,%rbx,4)
	movups	%xmm6, (%rsi,%rdx,4)
	movl	$4, %edx
	subq	%rax, %rdx
	movmskps	%xmm0, %r11d
	addq	%rdx, %rbx
	movq	%r11, %rax
	salq	$4, %rax
	movdqa	(%rdi,%rax), %xmm0
	movaps	%xmm0, -4496(%rbp)
	movzbl	-4495(%rbp), %eax
	movd	%xmm0, %r8d
	movaps	%xmm0, -4512(%rbp)
	andl	$15, %r8d
	movaps	%xmm0, -4608(%rbp)
	movq	%rax, %r15
	movzbl	-4600(%rbp), %edx
	movzbl	-4510(%rbp), %eax
	movaps	%xmm0, -4528(%rbp)
	andl	$15, %r15d
	andl	$15, %edx
	movq	%rax, %r14
	movzbl	-4525(%rbp), %eax
	movaps	%xmm0, -4624(%rbp)
	movq	%rdx, -5760(%rbp)
	movzbl	-4615(%rbp), %edx
	andl	$15, %r14d
	movaps	%xmm0, -4544(%rbp)
	movq	%rax, %r13
	movzbl	-4540(%rbp), %eax
	andl	$15, %edx
	movaps	%xmm0, -4560(%rbp)
	andl	$15, %r13d
	movzbl	-4555(%rbp), %r9d
	movaps	%xmm0, -4576(%rbp)
	movq	%rax, %r12
	movzbl	-4570(%rbp), %r10d
	movaps	%xmm0, -4592(%rbp)
	movzbl	-4585(%rbp), %eax
	andl	$15, %r12d
	andl	$15, %r9d
	movq	%rdx, -5776(%rbp)
	andl	$15, %r10d
	movaps	%xmm0, -4640(%rbp)
	movzbl	-4630(%rbp), %edx
	andl	$15, %eax
	movaps	%xmm0, -4656(%rbp)
	movq	%rdx, %rcx
	movzbl	-4645(%rbp), %edx
	movaps	%xmm0, -4720(%rbp)
	andl	$15, %ecx
	movaps	%xmm0, -4672(%rbp)
	movq	%rdx, %rdi
	movaps	%xmm0, -4688(%rbp)
	movzbl	-4660(%rbp), %edx
	andl	$15, %edi
	movaps	%xmm0, -4704(%rbp)
	movzbl	-4690(%rbp), %esi
	movzbl	-128(%rbp,%rax), %eax
	movq	%rdi, -5808(%rbp)
	movzbl	-4705(%rbp), %edi
	andl	$15, %edx
	movq	%rcx, -5784(%rbp)
	movzbl	-4675(%rbp), %ecx
	andl	$15, %esi
	movzbl	-128(%rbp,%r10), %r10d
	andl	$15, %edi
	movzbl	-128(%rbp,%rsi), %esi
	movzbl	-128(%rbp,%rdi), %edi
	andl	$15, %ecx
	salq	$8, %rax
	movzbl	-128(%rbp,%r9), %r9d
	orq	%r10, %rax
	movzbl	-128(%rbp,%rcx), %ecx
	movq	-5808(%rbp), %r10
	salq	$8, %rax
	salq	$8, %rdi
	movzbl	-128(%rbp,%rdx), %edx
	movzbl	-128(%rbp,%r8), %r8d
	orq	%rsi, %rdi
	orq	%r9, %rax
	movzbl	-128(%rbp,%r12), %r9d
	salq	$8, %rax
	salq	$8, %rdi
	orq	%rcx, %rdi
	orq	%r9, %rax
	movzbl	-128(%rbp,%r13), %r9d
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r9, %rax
	orq	%rdx, %rdi
	movzbl	-128(%rbp,%r14), %r9d
	movzbl	-128(%rbp,%r10), %edx
	movq	-5784(%rbp), %r14
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r9, %rax
	orq	%rdx, %rdi
	movzbl	-128(%rbp,%r15), %r9d
	movq	-5776(%rbp), %r15
	movzbl	-128(%rbp,%r14), %edx
	salq	$8, %rdi
	salq	$8, %rax
	orq	%r9, %rax
	orq	%rdx, %rdi
	movzbl	-128(%rbp,%r15), %edx
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r8, %rax
	orq	%rdx, %rdi
	movq	-5760(%rbp), %rdx
	salq	$8, %rdi
	movzbl	-128(%rbp,%rdx), %edx
	movq	%rax, -5760(%rbp)
	movq	%rdi, %rax
	movq	%r11, %rdi
	orq	%rdx, %rax
	movq	%rax, -5752(%rbp)
	call	__popcountdi2@PLT
	movq	-5712(%rbp), %rcx
	movdqa	-5760(%rbp), %xmm6
	movq	-5696(%rbp), %rsi
	cltq
	movq	-5744(%rbp), %rdi
	leaq	-24(%rcx,%rbx), %rdx
	movups	%xmm6, (%rsi,%rbx,4)
	movups	%xmm6, (%rsi,%rdx,4)
	movdqa	-6096(%rbp), %xmm6
	movl	$4, %edx
	subq	%rax, %rdx
	movdqa	%xmm6, %xmm0
	addq	%rdx, %rbx
	movaps	%xmm6, -112(%rbp)
	pcmpgtd	-5728(%rbp), %xmm0
	movmskps	%xmm0, %r11d
	movq	%r11, %rax
	salq	$4, %rax
	movdqa	(%rdi,%rax), %xmm0
	movaps	%xmm0, -4736(%rbp)
	movzbl	-4735(%rbp), %eax
	movd	%xmm0, %r8d
	movaps	%xmm0, -4752(%rbp)
	andl	$15, %r8d
	movq	%rax, %r15
	movzbl	-4750(%rbp), %eax
	movaps	%xmm0, -4768(%rbp)
	movaps	%xmm0, -4848(%rbp)
	movzbl	-4840(%rbp), %edx
	andl	$15, %r15d
	movq	%rax, %r14
	movzbl	-4765(%rbp), %eax
	movaps	%xmm0, -4784(%rbp)
	andl	$15, %edx
	movaps	%xmm0, -4832(%rbp)
	andl	$15, %r14d
	movq	%rax, %r13
	movzbl	-4780(%rbp), %eax
	movaps	%xmm0, -4864(%rbp)
	movq	%rdx, -5760(%rbp)
	movzbl	-4855(%rbp), %edx
	andl	$15, %r13d
	movq	%rax, %r12
	movzbl	-4825(%rbp), %eax
	movaps	%xmm0, -4816(%rbp)
	movzbl	-4810(%rbp), %r10d
	andl	$15, %edx
	movaps	%xmm0, -4800(%rbp)
	andl	$15, %r12d
	movzbl	-4795(%rbp), %r9d
	andl	$15, %eax
	movq	%rdx, -5776(%rbp)
	andl	$15, %r10d
	movaps	%xmm0, -4880(%rbp)
	movzbl	-4870(%rbp), %edx
	andl	$15, %r9d
	movaps	%xmm0, -4896(%rbp)
	movaps	%xmm0, -4912(%rbp)
	movq	%rdx, %rcx
	movzbl	-4885(%rbp), %edx
	movaps	%xmm0, -4928(%rbp)
	andl	$15, %ecx
	movaps	%xmm0, -4944(%rbp)
	movq	%rdx, %rdi
	movzbl	-4930(%rbp), %esi
	movzbl	-4900(%rbp), %edx
	movaps	%xmm0, -4960(%rbp)
	movzbl	-112(%rbp,%rax), %eax
	movzbl	-112(%rbp,%r10), %r10d
	andl	$15, %edi
	movzbl	-112(%rbp,%r9), %r9d
	movq	%rdi, -5808(%rbp)
	andl	$15, %esi
	andl	$15, %edx
	salq	$8, %rax
	movzbl	-112(%rbp,%rsi), %esi
	movzbl	-112(%rbp,%rdx), %edx
	movq	%rcx, -5784(%rbp)
	orq	%r10, %rax
	movzbl	-4945(%rbp), %edi
	movzbl	-4915(%rbp), %ecx
	salq	$8, %rax
	movq	-5808(%rbp), %r10
	movzbl	-112(%rbp,%r8), %r8d
	orq	%r9, %rax
	movzbl	-112(%rbp,%r12), %r9d
	andl	$15, %edi
	andl	$15, %ecx
	salq	$8, %rax
	movzbl	-112(%rbp,%rdi), %edi
	movzbl	-112(%rbp,%rcx), %ecx
	orq	%r9, %rax
	movzbl	-112(%rbp,%r13), %r9d
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r9, %rax
	movzbl	-112(%rbp,%r14), %r9d
	orq	%rsi, %rdi
	movq	-5784(%rbp), %r14
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r9, %rax
	movzbl	-112(%rbp,%r15), %r9d
	movq	-5776(%rbp), %r15
	salq	$8, %rax
	orq	%r9, %rax
	salq	$8, %rax
	orq	%rcx, %rdi
	salq	$8, %rdi
	orq	%r8, %rax
	orq	%rdx, %rdi
	movzbl	-112(%rbp,%r10), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-112(%rbp,%r14), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-112(%rbp,%r15), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movq	-5760(%rbp), %rdx
	salq	$8, %rdi
	movzbl	-112(%rbp,%rdx), %edx
	movq	%rax, -5760(%rbp)
	movq	%rdi, %rax
	movq	%r11, %rdi
	orq	%rdx, %rax
	movq	%rax, -5752(%rbp)
	call	__popcountdi2@PLT
	movdqa	-6112(%rbp), %xmm7
	movq	-5712(%rbp), %rcx
	movq	-5696(%rbp), %rsi
	cltq
	movdqa	-5760(%rbp), %xmm6
	movdqa	%xmm7, %xmm0
	leaq	-28(%rcx,%rbx), %rdx
	movq	-5744(%rbp), %rdi
	movaps	%xmm7, -96(%rbp)
	pcmpgtd	-5728(%rbp), %xmm0
	movups	%xmm6, (%rsi,%rbx,4)
	movups	%xmm6, (%rsi,%rdx,4)
	movl	$4, %edx
	subq	%rax, %rdx
	movmskps	%xmm0, %r11d
	addq	%rdx, %rbx
	movq	%r11, %rax
	salq	$4, %rax
	movdqa	(%rdi,%rax), %xmm0
	movaps	%xmm0, -4976(%rbp)
	movzbl	-4975(%rbp), %eax
	movd	%xmm0, %r8d
	movaps	%xmm0, -5088(%rbp)
	movzbl	-5080(%rbp), %edx
	andl	$15, %r8d
	movaps	%xmm0, -4992(%rbp)
	movq	%rax, %r15
	movzbl	-4990(%rbp), %eax
	movaps	%xmm0, -5104(%rbp)
	movq	%rdx, %rdi
	andl	$15, %r15d
	movzbl	-5095(%rbp), %edx
	andl	$15, %edi
	movaps	%xmm0, -5008(%rbp)
	movq	%rax, %r14
	movzbl	-5005(%rbp), %eax
	movq	%rdi, -5744(%rbp)
	movq	%rdx, %rdi
	andl	$15, %r14d
	andl	$15, %edi
	movaps	%xmm0, -5024(%rbp)
	movq	%rax, %r13
	movzbl	-5020(%rbp), %r9d
	movaps	%xmm0, -5040(%rbp)
	movzbl	-5035(%rbp), %r10d
	andl	$15, %r13d
	movaps	%xmm0, -5056(%rbp)
	movzbl	-5050(%rbp), %r12d
	andl	$15, %r9d
	movaps	%xmm0, -5072(%rbp)
	movzbl	-5065(%rbp), %eax
	andl	$15, %r10d
	movq	%rdi, -5760(%rbp)
	andl	$15, %r12d
	movaps	%xmm0, -5120(%rbp)
	movzbl	-5110(%rbp), %edx
	andl	$15, %eax
	movaps	%xmm0, -5136(%rbp)
	andl	$15, %edx
	movaps	%xmm0, -5200(%rbp)
	movq	%rdx, -5776(%rbp)
	movzbl	-5125(%rbp), %edx
	movaps	%xmm0, -5152(%rbp)
	movq	%rdx, %rdi
	movaps	%xmm0, -5168(%rbp)
	movzbl	-5140(%rbp), %edx
	movzbl	-5155(%rbp), %ecx
	andl	$15, %edi
	movaps	%xmm0, -5184(%rbp)
	movzbl	-96(%rbp,%rax), %eax
	movzbl	-5170(%rbp), %esi
	movq	%rdi, -5784(%rbp)
	movzbl	-5185(%rbp), %edi
	andl	$15, %edx
	andl	$15, %ecx
	movzbl	-96(%rbp,%r12), %r12d
	andl	$15, %esi
	movzbl	-96(%rbp,%rcx), %ecx
	andl	$15, %edi
	salq	$8, %rax
	movzbl	-96(%rbp,%rsi), %esi
	movzbl	-96(%rbp,%r10), %r10d
	movzbl	-96(%rbp,%rdi), %edi
	orq	%r12, %rax
	movzbl	-96(%rbp,%rdx), %edx
	salq	$8, %rax
	movzbl	-96(%rbp,%r9), %r9d
	movzbl	-96(%rbp,%r8), %r8d
	salq	$8, %rdi
	orq	%r10, %rax
	movq	-5784(%rbp), %r10
	orq	%rsi, %rdi
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r9, %rax
	movzbl	-96(%rbp,%r13), %r9d
	orq	%rcx, %rdi
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r9, %rax
	movzbl	-96(%rbp,%r14), %r9d
	movq	-5776(%rbp), %r14
	orq	%rdx, %rdi
	movzbl	-96(%rbp,%r10), %edx
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r9, %rax
	movzbl	-96(%rbp,%r15), %r9d
	movq	-5760(%rbp), %r15
	orq	%rdx, %rdi
	movzbl	-96(%rbp,%r14), %edx
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r9, %rax
	orq	%rdx, %rdi
	movzbl	-96(%rbp,%r15), %edx
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r8, %rax
	orq	%rdx, %rdi
	movq	-5744(%rbp), %rdx
	salq	$8, %rdi
	movzbl	-96(%rbp,%rdx), %edx
	movq	%rax, -5744(%rbp)
	orq	%rdx, %rdi
	movq	%rdi, -5736(%rbp)
	movq	%r11, %rdi
	call	__popcountdi2@PLT
	movq	-5712(%rbp), %rcx
	movq	-5696(%rbp), %rsi
	movdqa	-5744(%rbp), %xmm5
	cltq
	leaq	-32(%rcx,%rbx), %rdx
	movq	%rsi, %rcx
	movups	%xmm5, (%rsi,%rbx,4)
	movups	%xmm5, (%rsi,%rdx,4)
	movl	$4, %edx
	subq	%rax, %rdx
	leaq	(%rdx,%rbx), %rax
	movq	-5968(%rbp), %rdx
	movq	%rax, -5712(%rbp)
	leaq	0(,%rax,4), %rbx
	subq	%rax, %rdx
.L579:
	movq	-5968(%rbp), %rsi
	movdqa	-5936(%rbp), %xmm2
	cmpq	$4, %rdx
	pcmpeqd	%xmm0, %xmm0
	pcmpgtd	-5728(%rbp), %xmm2
	leaq	-16(,%rsi,4), %rax
	cmovnb	%rbx, %rax
	pxor	%xmm2, %xmm0
	movdqu	(%rcx,%rax), %xmm7
	movaps	%xmm2, -5808(%rbp)
	movmskps	%xmm0, %r12d
	movups	%xmm7, (%rcx,%rsi,4)
	movq	%r12, %rdi
	salq	$4, %r12
	movaps	%xmm7, -5744(%rbp)
	call	__popcountdi2@PLT
	movdqa	-5936(%rbp), %xmm7
	movslq	%eax, %rsi
	movl	%esi, -5760(%rbp)
	movq	%rsi, -5744(%rbp)
	movq	-5888(%rbp), %rsi
	movaps	%xmm7, -80(%rbp)
	movdqa	(%rsi,%r12), %xmm0
	movaps	%xmm0, -5216(%rbp)
	movzbl	-5215(%rbp), %eax
	movd	%xmm0, %r8d
	movaps	%xmm0, -5232(%rbp)
	andl	$15, %r8d
	movaps	%xmm0, -5328(%rbp)
	movq	%rax, %r14
	movzbl	-5320(%rbp), %edx
	movzbl	-5230(%rbp), %eax
	movaps	%xmm0, -5312(%rbp)
	andl	$15, %r14d
	movaps	%xmm0, -5344(%rbp)
	movq	%rax, %r13
	movq	%rdx, %rsi
	movzbl	-5305(%rbp), %eax
	movzbl	-5335(%rbp), %edx
	andl	$15, %esi
	movaps	%xmm0, -5296(%rbp)
	movzbl	-5290(%rbp), %r12d
	andl	$15, %eax
	movaps	%xmm0, -5248(%rbp)
	movzbl	-5245(%rbp), %r9d
	andl	$15, %r13d
	andl	$15, %edx
	movaps	%xmm0, -5264(%rbp)
	andl	$15, %r12d
	movzbl	-5260(%rbp), %r10d
	movaps	%xmm0, -5280(%rbp)
	movzbl	-5275(%rbp), %r11d
	andl	$15, %r9d
	movq	%rsi, -5728(%rbp)
	andl	$15, %r10d
	movaps	%xmm0, -5360(%rbp)
	andl	$15, %r11d
	movq	%rdx, -5776(%rbp)
	movzbl	-5350(%rbp), %edx
	movaps	%xmm0, -5376(%rbp)
	movaps	%xmm0, -5392(%rbp)
	movq	%rdx, %r15
	movzbl	-5365(%rbp), %edx
	movaps	%xmm0, -5408(%rbp)
	andl	$15, %r15d
	movaps	%xmm0, -5424(%rbp)
	movq	%rdx, %rcx
	movzbl	-5410(%rbp), %esi
	movzbl	-5380(%rbp), %edx
	movaps	%xmm0, -5440(%rbp)
	movzbl	-80(%rbp,%rax), %eax
	andl	$15, %ecx
	movzbl	-80(%rbp,%r12), %r12d
	movzbl	-80(%rbp,%r11), %r11d
	movzbl	-80(%rbp,%r10), %r10d
	movq	%r15, -5784(%rbp)
	andl	$15, %esi
	salq	$8, %rax
	movzbl	-80(%rbp,%r9), %r9d
	movq	%rcx, %r15
	andl	$15, %edx
	orq	%r12, %rax
	movzbl	-80(%rbp,%rsi), %esi
	movzbl	-80(%rbp,%rdx), %edx
	salq	$8, %rax
	movzbl	-5425(%rbp), %edi
	movzbl	-5395(%rbp), %ecx
	orq	%r11, %rax
	movzbl	-80(%rbp,%r8), %r8d
	salq	$8, %rax
	andl	$15, %edi
	andl	$15, %ecx
	orq	%r10, %rax
	movzbl	-80(%rbp,%rdi), %edi
	movzbl	-80(%rbp,%rcx), %ecx
	salq	$8, %rax
	orq	%r9, %rax
	movzbl	-80(%rbp,%r13), %r9d
	salq	$8, %rdi
	salq	$8, %rax
	orq	%r9, %rax
	movzbl	-80(%rbp,%r14), %r9d
	salq	$8, %rax
	orq	%r9, %rax
	salq	$8, %rax
	orq	%rsi, %rdi
	movq	-5728(%rbp), %rsi
	salq	$8, %rdi
	orq	%rcx, %rdi
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-80(%rbp,%r15), %edx
	movq	-5784(%rbp), %r15
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-80(%rbp,%r15), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movq	-5776(%rbp), %rdx
	salq	$8, %rdi
	movzbl	-80(%rbp,%rdx), %edx
	orq	%rdx, %rdi
	movzbl	-80(%rbp,%rsi), %edx
	movq	%rax, %rsi
	orq	%r8, %rsi
	salq	$8, %rdi
	movq	%rsi, -5728(%rbp)
	movd	-5760(%rbp), %xmm6
	movq	%rdi, %rsi
	orq	%rdx, %rsi
	movdqa	-5808(%rbp), %xmm2
	pshufd	$0, %xmm6, %xmm0
	movq	%rsi, -5720(%rbp)
	pcmpgtd	-5920(%rbp), %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L585
	movq	-5696(%rbp), %rax
	movdqa	-5728(%rbp), %xmm6
	movd	%xmm6, (%rax,%rbx)
.L585:
	pshufd	$85, %xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L586
	pshufd	$85, -5728(%rbp), %xmm1
	movq	-5696(%rbp), %rax
	movd	%xmm1, 4(%rax,%rbx)
.L586:
	movdqa	%xmm0, %xmm1
	punpckhdq	%xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L587
	movdqa	-5728(%rbp), %xmm1
	movq	-5696(%rbp), %rax
	punpckhdq	%xmm1, %xmm1
	movd	%xmm1, 8(%rax,%rbx)
.L587:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L588
	pshufd	$255, -5728(%rbp), %xmm0
	movq	-5696(%rbp), %rax
	movd	%xmm0, 12(%rax,%rbx)
.L588:
	movmskps	%xmm2, %r13d
	movq	-5712(%rbp), %rbx
	addq	-5744(%rbp), %rbx
	movq	%r13, %rdi
	salq	$4, %r13
	leaq	0(,%rbx,4), %r12
	call	__popcountdi2@PLT
	movq	-5888(%rbp), %rsi
	movdqa	-5936(%rbp), %xmm7
	movl	%eax, -5776(%rbp)
	movdqa	(%rsi,%r13), %xmm0
	movaps	%xmm7, -64(%rbp)
	movaps	%xmm0, -5456(%rbp)
	movzbl	-5455(%rbp), %eax
	movd	%xmm0, %r8d
	movaps	%xmm0, -5568(%rbp)
	movzbl	-5560(%rbp), %edx
	andl	$15, %r8d
	movaps	%xmm0, -5472(%rbp)
	movq	%rax, %r15
	movzbl	-5470(%rbp), %eax
	movaps	%xmm0, -5584(%rbp)
	movq	%rdx, %rcx
	andl	$15, %r15d
	movzbl	-5575(%rbp), %edx
	movaps	%xmm0, -5552(%rbp)
	movq	%rax, %r14
	andl	$15, %ecx
	movzbl	-5545(%rbp), %eax
	movaps	%xmm0, -5600(%rbp)
	movq	%rdx, %rdi
	andl	$15, %r14d
	movzbl	-5590(%rbp), %edx
	movaps	%xmm0, -5536(%rbp)
	andl	$15, %edi
	andl	$15, %eax
	movzbl	-5530(%rbp), %r13d
	andl	$15, %edx
	movaps	%xmm0, -5488(%rbp)
	movzbl	-5485(%rbp), %r9d
	movaps	%xmm0, -5504(%rbp)
	andl	$15, %r13d
	movzbl	-5500(%rbp), %r10d
	movaps	%xmm0, -5520(%rbp)
	movzbl	-5515(%rbp), %r11d
	andl	$15, %r9d
	movq	%rcx, -5712(%rbp)
	andl	$15, %r10d
	movq	%rdi, -5728(%rbp)
	andl	$15, %r11d
	movaps	%xmm0, -5616(%rbp)
	movq	%rdx, -5744(%rbp)
	movzbl	-5605(%rbp), %edx
	movaps	%xmm0, -5632(%rbp)
	movaps	%xmm0, -5648(%rbp)
	movq	%rdx, %rdi
	movzbl	-5635(%rbp), %ecx
	movzbl	-5620(%rbp), %edx
	movaps	%xmm0, -5664(%rbp)
	andl	$15, %edi
	movzbl	-5650(%rbp), %esi
	movaps	%xmm0, -5680(%rbp)
	movzbl	-64(%rbp,%rax), %eax
	movzbl	-64(%rbp,%r13), %r13d
	andl	$15, %edx
	movzbl	-64(%rbp,%r11), %r11d
	movzbl	-64(%rbp,%r10), %r10d
	andl	$15, %esi
	andl	$15, %ecx
	salq	$8, %rax
	movzbl	-64(%rbp,%r9), %r9d
	movzbl	-64(%rbp,%rsi), %esi
	movq	%rdi, -5760(%rbp)
	orq	%r13, %rax
	movzbl	-64(%rbp,%rcx), %ecx
	movzbl	-64(%rbp,%rdx), %edx
	salq	$8, %rax
	movzbl	-5665(%rbp), %edi
	movzbl	-64(%rbp,%r8), %r8d
	orq	%r11, %rax
	salq	$8, %rax
	andl	$15, %edi
	orq	%r10, %rax
	movzbl	-64(%rbp,%rdi), %edi
	salq	$8, %rax
	orq	%r9, %rax
	movzbl	-64(%rbp,%r14), %r9d
	salq	$8, %rax
	orq	%r9, %rax
	movzbl	-64(%rbp,%r15), %r9d
	movq	-5760(%rbp), %r15
	salq	$8, %rax
	orq	%r9, %rax
	salq	$8, %rax
	salq	$8, %rdi
	orq	%rsi, %rdi
	movq	-5728(%rbp), %rsi
	orq	%r8, %rax
	salq	$8, %rdi
	orq	%rcx, %rdi
	movq	-5712(%rbp), %rcx
	movq	%rax, -5712(%rbp)
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-64(%rbp,%r15), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movq	-5744(%rbp), %rdx
	salq	$8, %rdi
	movzbl	-64(%rbp,%rdx), %edx
	orq	%rdx, %rdi
	movzbl	-64(%rbp,%rsi), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-64(%rbp,%rcx), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movq	%rdi, -5704(%rbp)
	movd	-5776(%rbp), %xmm6
	pshufd	$0, %xmm6, %xmm0
	pcmpgtd	-5920(%rbp), %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L589
	movq	-5696(%rbp), %rax
	movdqa	-5712(%rbp), %xmm7
	movd	%xmm7, (%rax,%rbx,4)
.L589:
	pshufd	$85, %xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L590
	pshufd	$85, -5712(%rbp), %xmm1
	movq	-5696(%rbp), %rax
	movd	%xmm1, 4(%rax,%r12)
.L590:
	movdqa	%xmm0, %xmm1
	punpckhdq	%xmm0, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L591
	movdqa	-5712(%rbp), %xmm1
	movq	-5696(%rbp), %rax
	punpckhdq	%xmm1, %xmm1
	movd	%xmm1, 8(%rax,%r12)
.L591:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L592
	pshufd	$255, -5712(%rbp), %xmm0
	movq	-5696(%rbp), %rax
	movd	%xmm0, 12(%rax,%r12)
.L592:
	movq	-5944(%rbp), %r12
	addq	-5976(%rbp), %rbx
	subq	$1, %r12
	cmpl	$2, -5948(%rbp)
	je	.L594
	movq	-5904(%rbp), %r8
	movq	%r12, %r9
	movq	%rbx, %rdx
	movq	-5880(%rbp), %rcx
	movq	-5960(%rbp), %rsi
	movq	-5768(%rbp), %rdi
	call	_ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	cmpl	$3, -5948(%rbp)
	je	.L483
.L594:
	movq	-5896(%rbp), %rdx
	movq	-5768(%rbp), %rax
	movq	%r12, %r9
	movq	-5904(%rbp), %r8
	movq	-5880(%rbp), %rcx
	movq	-5960(%rbp), %rsi
	subq	%rbx, %rdx
	leaq	(%rax,%rbx,4), %rdi
	call	_ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L483:
	addq	$6072, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L575:
	.cfi_restore_state
	movq	-5880(%rbp), %r15
	leaq	8(%rax), %rdi
	andq	$-8, %rdi
	movq	(%r15), %rcx
	movq	%rcx, (%rax)
	movl	%ebx, %ecx
	movq	-8(%r15,%rcx), %rsi
	movq	%rsi, -8(%rax,%rcx)
	subq	%rdi, %rax
	movq	%r15, %rsi
	leal	(%rbx,%rax), %ecx
	subq	%rax, %rsi
	shrl	$3, %ecx
	rep movsq
	jmp	.L576
	.p2align 4,,10
	.p2align 3
.L571:
	movq	-5696(%rbp), %rdi
	movq	(%rax), %rcx
	movq	%rcx, (%rdi)
	movl	%ebx, %ecx
	movq	-8(%rax,%rcx), %rsi
	movq	%rsi, -8(%rdi,%rcx)
	movq	%rdi, %rcx
	leaq	8(%rdi), %rdi
	movq	%rax, %rsi
	andq	$-8, %rdi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%ebx, %ecx
	shrl	$3, %ecx
	rep movsq
	jmp	.L572
.L771:
	movq	-5880(%rbp), %rax
	movl	(%rax), %ecx
	jmp	.L545
.L776:
	movq	-5880(%rbp), %rbx
	movzbl	(%rbx), %esi
	movb	%sil, (%rax)
	testb	$2, %cl
	je	.L576
	movq	-5880(%rbp), %rbx
	movzwl	-2(%rbx,%rcx), %esi
	movw	%si, -2(%rax,%rcx)
	jmp	.L576
.L774:
	movzbl	(%rax), %esi
	movq	-5696(%rbp), %rdi
	movb	%sil, (%rdi)
	testb	$2, %cl
	je	.L572
	movzwl	-2(%rax,%rcx), %esi
	movq	-5696(%rbp), %rdi
	movw	%si, -2(%rdi,%rcx)
	jmp	.L572
.L769:
	cmpq	$1, %rdx
	jbe	.L483
	movq	%rdi, %rax
	addq	$256, %rax
	cmpq	%rax, %rsi
	jb	.L779
	movl	$4, %esi
	call	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	jmp	.L483
.L499:
	movq	-5768(%rbp), %rax
	andl	$3, %r13d
	movl	$4, %edi
	movdqa	.LC0(%rip), %xmm7
	subq	%r13, %rdi
	movdqu	(%rax), %xmm6
	movaps	%xmm7, -5920(%rbp)
	movaps	%xmm6, -5696(%rbp)
	movd	%edi, %xmm6
	movdqa	-5696(%rbp), %xmm1
	pshufd	$0, %xmm6, %xmm3
	pcmpeqd	%xmm0, %xmm1
	pcmpgtd	%xmm7, %xmm3
	pandn	%xmm3, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	jne	.L780
	movq	-5768(%rbp), %rax
	pxor	%xmm3, %xmm3
	movq	-5896(%rbp), %r8
	pxor	%xmm5, %xmm5
	movdqa	%xmm3, %xmm1
	leaq	256(%rax,%rdi,4), %rsi
	.p2align 4,,10
	.p2align 3
.L505:
	movq	%rdi, %rcx
	leaq	64(%rdi), %rdi
	cmpq	%rdi, %r8
	jb	.L781
	leaq	-256(%rsi), %rax
.L504:
	movdqa	(%rax), %xmm4
	leaq	32(%rax), %rdx
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm1
	movdqa	16(%rax), %xmm4
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm3
	movdqa	32(%rax), %xmm4
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm1
	movdqa	48(%rax), %xmm4
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm3
	movdqa	64(%rax), %xmm4
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm1
	movdqa	80(%rax), %xmm4
	leaq	96(%rdx), %rax
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm3
	movdqa	64(%rdx), %xmm4
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm1
	movdqa	80(%rdx), %xmm4
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm3
	cmpq	%rsi, %rax
	jne	.L504
	movdqa	%xmm1, %xmm4
	leaq	352(%rdx), %rsi
	por	%xmm3, %xmm4
	pcmpeqd	%xmm5, %xmm4
	movmskps	%xmm4, %eax
	cmpl	$15, %eax
	je	.L505
	movq	-5768(%rbp), %rax
	movdqa	%xmm0, %xmm1
	pcmpeqd	%xmm2, %xmm2
	movq	-5768(%rbp), %rdx
	pcmpeqd	(%rax,%rcx,4), %xmm1
	pxor	%xmm2, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	jne	.L507
	.p2align 4,,10
	.p2align 3
.L506:
	addq	$4, %rcx
	movdqa	%xmm0, %xmm1
	pcmpeqd	(%rdx,%rcx,4), %xmm1
	pxor	%xmm2, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	je	.L506
.L507:
	rep bsfl	%eax, %eax
	cltq
	addq	%rcx, %rax
.L503:
	movq	-5768(%rbp), %rcx
	leaq	(%rcx,%rax,4), %rdi
	movl	(%rdi), %r12d
	movd	%r12d, %xmm7
	pshufd	$0, %xmm7, %xmm2
	movdqa	%xmm2, %xmm1
	movaps	%xmm2, -5696(%rbp)
	pcmpgtd	%xmm0, %xmm1
	movmskps	%xmm1, %edx
	testl	%edx, %edx
	jne	.L512
	movq	-5896(%rbp), %r15
	movl	%r12d, -5760(%rbp)
	xorl	%ebx, %ebx
	movq	%rcx, %r12
	movaps	%xmm0, -5744(%rbp)
	leaq	-4(%r15), %r13
	movaps	%xmm0, -5712(%rbp)
	movaps	%xmm2, -5728(%rbp)
	jmp	.L521
	.p2align 4,,10
	.p2align 3
.L513:
	movdqa	-5744(%rbp), %xmm5
	movmskps	%xmm1, %edi
	movups	%xmm5, (%r12,%r13,4)
	call	__popcountdi2@PLT
	cltq
	addq	%rax, %rbx
	leaq	-4(%r13), %rax
	cmpq	%rax, %r15
	jbe	.L782
	movq	%rax, %r13
.L521:
	movdqu	(%r12,%r13,4), %xmm1
	movdqu	(%r12,%r13,4), %xmm4
	pcmpeqd	-5712(%rbp), %xmm1
	pcmpeqd	-5728(%rbp), %xmm4
	movdqa	%xmm1, %xmm5
	movdqa	%xmm1, %xmm6
	por	%xmm4, %xmm5
	movmskps	%xmm5, %eax
	cmpl	$15, %eax
	je	.L513
	pcmpeqd	%xmm1, %xmm1
	movq	-5768(%rbp), %rsi
	movq	-5896(%rbp), %rdx
	leaq	4(%r13), %rcx
	pxor	%xmm1, %xmm6
	movdqa	-5744(%rbp), %xmm3
	movdqa	-5712(%rbp), %xmm0
	pandn	%xmm6, %xmm4
	subq	%rbx, %rdx
	movl	-5760(%rbp), %r12d
	movdqa	-5728(%rbp), %xmm2
	movmskps	%xmm4, %eax
	rep bsfl	%eax, %eax
	cltq
	addq	%r13, %rax
	movd	(%rsi,%rax,4), %xmm7
	leaq	8(%r13), %rax
	pshufd	$0, %xmm7, %xmm1
	movdqa	%xmm1, %xmm4
	movaps	%xmm1, -64(%rbp)
	cmpq	%rax, %rdx
	jb	.L514
.L515:
	movdqa	-5696(%rbp), %xmm6
	movq	%rax, %rcx
	movups	%xmm6, -16(%rsi,%rax,4)
	addq	$4, %rax
	cmpq	%rax, %rdx
	jnb	.L515
.L514:
	subq	%rcx, %rdx
	leaq	0(,%rcx,4), %rsi
	movq	%rdx, %xmm1
	pshufd	$0, %xmm1, %xmm1
	pcmpgtd	-5920(%rbp), %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L516
	movq	-5768(%rbp), %rax
	movl	%r12d, (%rax,%rcx,4)
.L516:
	pshufd	$85, %xmm1, %xmm5
	movd	%xmm5, %eax
	testl	%eax, %eax
	je	.L517
	movq	-5768(%rbp), %rax
	pshufd	$85, %xmm2, %xmm5
	movd	%xmm5, 4(%rax,%rsi)
.L517:
	movdqa	%xmm1, %xmm5
	punpckhdq	%xmm1, %xmm5
	movd	%xmm5, %eax
	testl	%eax, %eax
	je	.L518
	movq	-5768(%rbp), %rax
	movdqa	%xmm2, %xmm5
	punpckhdq	%xmm2, %xmm5
	movd	%xmm5, 8(%rax,%rsi)
.L518:
	pshufd	$255, %xmm1, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L520
	movq	-5768(%rbp), %rax
	pshufd	$255, %xmm2, %xmm1
	movd	%xmm1, 12(%rax,%rsi)
.L520:
	movdqa	%xmm0, %xmm1
	pcmpeqd	.LC5(%rip), %xmm1
	movmskps	%xmm1, %eax
	cmpl	$15, %eax
	je	.L612
	movdqa	%xmm0, %xmm1
	pcmpeqd	.LC6(%rip), %xmm1
	movmskps	%xmm1, %eax
	cmpl	$15, %eax
	je	.L538
	movdqa	%xmm4, %xmm1
	pcmpgtd	%xmm2, %xmm1
	movdqa	%xmm1, %xmm6
	movdqa	%xmm1, %xmm5
	pandn	%xmm4, %xmm6
	pand	%xmm2, %xmm5
	por	%xmm6, %xmm5
	movdqa	%xmm0, %xmm6
	pcmpgtd	%xmm5, %xmm6
	movmskps	%xmm6, %eax
	testl	%eax, %eax
	jne	.L783
	movdqa	%xmm3, %xmm1
	movl	$64, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L533:
	movq	-5768(%rbp), %rbx
	leaq	(%rcx,%rax,4), %rdx
	addq	$1, %rax
	movdqu	(%rbx,%rdx,4), %xmm4
	movdqa	%xmm4, %xmm2
	pcmpgtd	%xmm1, %xmm2
	movdqa	%xmm2, %xmm5
	pand	%xmm2, %xmm1
	pandn	%xmm4, %xmm5
	por	%xmm5, %xmm1
	cmpq	$16, %rax
	jne	.L533
	movdqa	%xmm0, %xmm2
	pcmpgtd	%xmm1, %xmm2
	movmskps	%xmm2, %eax
	testl	%eax, %eax
	jne	.L768
	leaq	64(%rsi), %rax
	cmpq	%rax, -5896(%rbp)
	jb	.L784
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L533
.L615:
	leaq	_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %rdi
	movq	%rsi, -5696(%rbp)
	xorl	%ebx, %ebx
	movdqa	.LC0(%rip), %xmm5
	movq	$0, -5712(%rbp)
	movq	%rdi, -5888(%rbp)
	movaps	%xmm5, -5920(%rbp)
	jmp	.L552
.L616:
	movq	-5968(%rbp), %rdx
	movq	-5696(%rbp), %rcx
	xorl	%ebx, %ebx
	jmp	.L579
.L617:
	leaq	_ZZN3hwy6N_SSE26detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %rax
	movq	-5696(%rbp), %r14
	movq	%rdi, %r13
	movl	$4, %ebx
	movq	%rax, -5744(%rbp)
	jmp	.L580
.L772:
	movq	-5896(%rbp), %rsi
	movq	-5768(%rbp), %rdi
	leaq	-1(%rsi), %rbx
	movq	%rbx, %r12
	shrq	%r12
	.p2align 4,,10
	.p2align 3
.L550:
	movq	%r12, %rdx
	call	_ZN3hwy6N_SSE26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	subq	$1, %r12
	jnb	.L550
	.p2align 4,,10
	.p2align 3
.L551:
	movl	(%rdi,%rbx,4), %edx
	movl	(%rdi), %eax
	movq	%rbx, %rsi
	movl	%edx, (%rdi)
	xorl	%edx, %edx
	movl	%eax, (%rdi,%rbx,4)
	call	_ZN3hwy6N_SSE26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	subq	$1, %rbx
	jne	.L551
	jmp	.L483
.L775:
	movq	-5880(%rbp), %rbx
	movl	(%rbx), %esi
	movl	%esi, (%rax)
	movl	-4(%rbx,%rcx), %esi
	movl	%esi, -4(%rax,%rcx)
	jmp	.L576
.L773:
	movl	(%rax), %esi
	movq	-5696(%rbp), %rdi
	movl	%esi, (%rdi)
	movl	-4(%rax,%rcx), %esi
	movl	%esi, -4(%rdi,%rcx)
	jmp	.L572
.L781:
	movq	-5768(%rbp), %rsi
	movq	-5896(%rbp), %rdi
	pcmpeqd	%xmm2, %xmm2
.L509:
	movq	%rcx, %rdx
	addq	$4, %rcx
	cmpq	%rcx, %rdi
	jb	.L785
	movdqa	%xmm0, %xmm1
	pcmpeqd	-16(%rsi,%rcx,4), %xmm1
	pxor	%xmm2, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	je	.L509
.L766:
	rep bsfl	%eax, %eax
	cltq
	addq	%rdx, %rax
	jmp	.L503
.L512:
	movq	-5896(%rbp), %rsi
	movq	-5880(%rbp), %rbx
	leaq	-64(%rbp), %rdx
	movdqa	%xmm2, %xmm1
	movaps	%xmm2, -5696(%rbp)
	subq	%rax, %rsi
	movq	%rbx, %rcx
	call	_ZN3hwy6N_SSE26detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L483
	movd	(%rbx), %xmm7
	movdqa	-64(%rbp), %xmm4
	movdqa	-5696(%rbp), %xmm2
	pshufd	$0, %xmm7, %xmm0
	movdqa	%xmm0, %xmm3
	jmp	.L520
.L782:
	movq	-5768(%rbp), %rax
	movdqa	-5712(%rbp), %xmm0
	movdqa	-5728(%rbp), %xmm2
	movq	-5896(%rbp), %r15
	movdqu	(%rax), %xmm7
	movdqa	-5744(%rbp), %xmm3
	movl	-5760(%rbp), %r12d
	subq	%rbx, %r15
	movaps	%xmm7, -5712(%rbp)
	movd	%r13d, %xmm7
	movdqa	-5712(%rbp), %xmm1
	movdqa	-5712(%rbp), %xmm5
	pshufd	$0, %xmm7, %xmm4
	pcmpgtd	-5920(%rbp), %xmm4
	pcmpeqd	%xmm0, %xmm1
	pcmpeqd	%xmm2, %xmm5
	movdqa	%xmm4, %xmm6
	pand	%xmm1, %xmm6
	por	%xmm5, %xmm1
	pcmpeqd	%xmm5, %xmm5
	pxor	%xmm5, %xmm4
	por	%xmm4, %xmm1
	movmskps	%xmm1, %eax
	cmpl	$15, %eax
	jne	.L786
	movmskps	%xmm6, %edi
	movaps	%xmm2, -5728(%rbp)
	movaps	%xmm3, -5712(%rbp)
	call	__popcountdi2@PLT
	movq	-5768(%rbp), %rbx
	movdqa	-5712(%rbp), %xmm3
	movslq	%eax, %rdx
	movq	%r15, %rax
	movdqa	-5728(%rbp), %xmm2
	subq	%rdx, %rax
	movups	%xmm3, (%rbx)
	cmpq	$3, %rax
	jbe	.L601
	leaq	-4(%rax), %rdx
	movq	%rdx, %rcx
	shrq	$2, %rcx
	salq	$4, %rcx
	leaq	16(%rbx,%rcx), %rcx
.L530:
	movdqa	-5696(%rbp), %xmm6
	addq	$16, %r14
	movups	%xmm6, -16(%r14)
	cmpq	%rcx, %r14
	jne	.L530
	andq	$-4, %rdx
	addq	$4, %rdx
	leaq	0(,%rdx,4), %rcx
	subq	%rdx, %rax
.L529:
	movq	-5880(%rbp), %rsi
	movaps	%xmm2, (%rsi)
	testq	%rax, %rax
	je	.L483
	movq	-5768(%rbp), %rdi
	leaq	0(,%rax,4), %rdx
	addq	%rcx, %rdi
	call	memcpy@PLT
	jmp	.L483
.L784:
	movq	-5896(%rbp), %rcx
	movq	%rbx, %rdx
	jmp	.L540
.L541:
	movdqu	-16(%rdx,%rsi,4), %xmm5
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm5, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	jne	.L768
.L540:
	movq	%rsi, %rax
	addq	$4, %rsi
	cmpq	%rsi, %rcx
	jnb	.L541
	movq	-5896(%rbp), %rbx
	cmpq	%rax, %rbx
	je	.L612
	movq	-5768(%rbp), %rax
	movdqu	-16(%rax,%rbx,4), %xmm7
	movaps	%xmm7, -5696(%rbp)
	pcmpgtd	-5696(%rbp), %xmm0
	movmskps	%xmm0, %eax
	cmpl	$1, %eax
	movl	$1, %eax
	adcl	$0, %eax
	movl	%eax, -5948(%rbp)
	jmp	.L542
.L785:
	movq	-5896(%rbp), %rax
	pcmpeqd	%xmm2, %xmm2
	leaq	-4(%rax), %rdx
	movq	-5768(%rbp), %rax
	movdqu	(%rax,%rdx,4), %xmm7
	movaps	%xmm7, -5696(%rbp)
	movdqa	-5696(%rbp), %xmm1
	pcmpeqd	%xmm0, %xmm1
	pxor	%xmm2, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	je	.L483
	jmp	.L766
.L788:
	movq	%rbx, %rdx
	jmp	.L536
.L537:
	movdqu	-16(%rdx,%rsi,4), %xmm1
	pcmpgtd	%xmm0, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	jne	.L768
.L536:
	movq	%rsi, %rax
	addq	$4, %rsi
	cmpq	%rsi, -5896(%rbp)
	jnb	.L537
	movq	-5896(%rbp), %rbx
	cmpq	%rax, %rbx
	je	.L538
	movq	-5768(%rbp), %rax
	movdqu	-16(%rax,%rbx,4), %xmm6
	movaps	%xmm6, -5696(%rbp)
	movdqa	-5696(%rbp), %xmm1
	pcmpgtd	%xmm0, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	jne	.L768
.L538:
	movl	$3, -5948(%rbp)
	pcmpeqd	%xmm3, %xmm3
	paddd	%xmm0, %xmm3
	jmp	.L542
.L612:
	movl	$2, -5948(%rbp)
	jmp	.L542
.L780:
	rep bsfl	%eax, %eax
	cltq
	jmp	.L503
.L779:
	cmpq	$3, %rdx
	jbe	.L787
	movq	%rdx, %r14
	leaq	-4(%rdx), %rdx
	movq	%rcx, %rbx
	movq	(%rdi), %rcx
	movq	%rdx, %rax
	movq	%rdi, %r15
	shrq	$2, %rax
	movq	%rcx, (%rbx)
	addq	$1, %rax
	salq	$4, %rax
	movl	%eax, %ecx
	movq	-8(%rdi,%rcx), %rsi
	leaq	8(%rbx), %rdi
	andq	$-8, %rdi
	movq	%rsi, -8(%rbx,%rcx)
	subq	%rdi, %rbx
	movq	%r15, %rsi
	movq	%rbx, %rcx
	subq	%rbx, %rsi
	addl	%eax, %ecx
	movq	%rdx, %rax
	andq	$-4, %rax
	shrl	$3, %ecx
	rep movsq
	addq	$4, %rax
	movq	%r14, %rcx
	subq	%rax, %rcx
	je	.L492
.L489:
	movq	-5880(%rbp), %rbx
	salq	$2, %rax
	leaq	0(,%rcx,4), %rdx
	testq	%rcx, %rcx
	movl	$4, %ecx
	leaq	(%rbx,%rax), %rdi
	movq	-5768(%rbp), %rbx
	cmove	%rcx, %rdx
	leaq	(%rbx,%rax), %rsi
	call	memcpy@PLT
.L492:
	movq	-5896(%rbp), %rbx
	movl	$32, %ecx
	movl	%ebx, %eax
	subl	$1, %eax
	bsrl	%eax, %eax
	xorl	$31, %eax
	subl	%eax, %ecx
	movl	$1, %eax
	salq	%cl, %rax
	shrq	$4, %rax
	movq	%rax, %rsi
	movl	$1, %eax
	cmove	%rax, %rsi
	movq	%rsi, %rdx
	salq	$4, %rdx
	addq	$4, %rdx
	cmpq	%rdx, %rbx
	jnb	.L491
	movdqa	.LC4(%rip), %xmm0
	movq	-5880(%rbp), %rcx
	movq	%rbx, %rax
.L490:
	movups	%xmm0, (%rcx,%rax,4)
	addq	$4, %rax
	cmpq	%rdx, %rax
	jb	.L490
.L491:
	movq	-5880(%rbp), %rdi
	call	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	xorl	%eax, %eax
	cmpq	$3, -5896(%rbp)
	jbe	.L494
	movq	-5896(%rbp), %r14
	movq	-5880(%rbp), %r15
	movq	-5768(%rbp), %rbx
	leaq	-4(%r14), %rdx
	movq	(%r15), %rcx
	movq	%rdx, %rax
	leaq	8(%rbx), %rdi
	shrq	$2, %rax
	movq	%rcx, (%rbx)
	andq	$-8, %rdi
	addq	$1, %rax
	salq	$4, %rax
	movl	%eax, %ecx
	movq	-8(%r15,%rcx), %rsi
	movq	%rsi, -8(%rbx,%rcx)
	subq	%rdi, %rbx
	movq	%r15, %rsi
	movq	%rbx, %rcx
	subq	%rbx, %rsi
	addl	%eax, %ecx
	movq	%rdx, %rax
	andq	$-4, %rax
	shrl	$3, %ecx
	rep movsq
	addq	$4, %rax
	subq	%rax, %r14
	movq	%r14, -5896(%rbp)
	je	.L483
.L494:
	movq	-5896(%rbp), %rbx
	salq	$2, %rax
	movq	-5768(%rbp), %rdi
	movl	$4, %ecx
	movq	-5880(%rbp), %rsi
	addq	%rax, %rdi
	leaq	0(,%rbx,4), %rdx
	testq	%rbx, %rbx
	cmove	%rcx, %rdx
	addq	%rax, %rsi
	call	memcpy@PLT
	jmp	.L483
.L783:
	movdqa	%xmm1, %xmm5
	pand	%xmm4, %xmm1
	pandn	%xmm2, %xmm5
	por	%xmm5, %xmm1
	pcmpgtd	%xmm0, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	jne	.L768
	movdqa	%xmm3, %xmm1
	movl	$64, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L534:
	movq	-5768(%rbp), %rbx
	leaq	(%rcx,%rax,4), %rdx
	addq	$1, %rax
	movdqu	(%rbx,%rdx,4), %xmm2
	movdqa	%xmm2, %xmm4
	pcmpgtd	%xmm1, %xmm4
	movdqa	%xmm4, %xmm5
	pand	%xmm4, %xmm2
	pandn	%xmm1, %xmm5
	movdqa	%xmm2, %xmm1
	por	%xmm5, %xmm1
	cmpq	$16, %rax
	jne	.L534
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm0, %xmm2
	movmskps	%xmm2, %eax
	testl	%eax, %eax
	jne	.L768
	leaq	64(%rsi), %rax
	cmpq	%rax, -5896(%rbp)
	jb	.L788
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L534
.L601:
	xorl	%ecx, %ecx
	jmp	.L529
.L786:
	pxor	%xmm5, %xmm1
	movq	-5768(%rbp), %rbx
	movmskps	%xmm1, %eax
	rep bsfl	%eax, %eax
	cltq
	movd	(%rbx,%rax,4), %xmm6
	leaq	4(%r13), %rax
	pshufd	$0, %xmm6, %xmm1
	movdqa	%xmm1, %xmm4
	movaps	%xmm1, -64(%rbp)
	cmpq	%r15, %rax
	ja	.L523
.L524:
	movq	-5768(%rbp), %rbx
	movdqa	-5696(%rbp), %xmm7
	movq	%rax, %r13
	movups	%xmm7, -16(%rbx,%rax,4)
	addq	$4, %rax
	cmpq	%r15, %rax
	jbe	.L524
.L523:
	subq	%r13, %r15
	leaq	0(,%r13,4), %rdx
	movq	%r15, %xmm1
	pshufd	$0, %xmm1, %xmm1
	pcmpgtd	-5920(%rbp), %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L525
	movq	-5768(%rbp), %rax
	movl	%r12d, (%rax,%r13,4)
.L525:
	pshufd	$85, %xmm1, %xmm5
	movd	%xmm5, %eax
	testl	%eax, %eax
	je	.L526
	movq	-5768(%rbp), %rax
	pshufd	$85, %xmm2, %xmm5
	movd	%xmm5, 4(%rax,%rdx)
.L526:
	movdqa	%xmm1, %xmm5
	punpckhdq	%xmm1, %xmm5
	movd	%xmm5, %eax
	testl	%eax, %eax
	je	.L527
	movq	-5768(%rbp), %rax
	movdqa	%xmm2, %xmm5
	punpckhdq	%xmm2, %xmm5
	movd	%xmm5, 8(%rax,%rdx)
.L527:
	pshufd	$255, %xmm1, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L520
	movq	-5768(%rbp), %rax
	pshufd	$255, %xmm2, %xmm1
	movd	%xmm1, 12(%rax,%rdx)
	jmp	.L520
.L787:
	movq	%rdx, %rcx
	xorl	%eax, %eax
	jmp	.L489
	.cfi_endproc
.LFE18798:
	.size	_ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0, .-_ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	.section	.text._ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0, @function
_ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0:
.LFB18800:
	.cfi_startproc
	leaq	8(%rsp), %r10
	.cfi_def_cfa 10, 0
	andq	$-64, %rsp
	pushq	-8(%r10)
	pushq	%rbp
	movq	%rsp, %rbp
	.cfi_escape 0x10,0x6,0x2,0x76,0
	pushq	%r15
	.cfi_escape 0x10,0xf,0x2,0x76,0x78
	movq	%rdx, %r15
	pushq	%r14
	pushq	%r13
	.cfi_escape 0x10,0xe,0x2,0x76,0x70
	.cfi_escape 0x10,0xd,0x2,0x76,0x68
	movq	%rdi, %r13
	pushq	%r12
	.cfi_escape 0x10,0xc,0x2,0x76,0x60
	movq	%rcx, %r12
	pushq	%r10
	.cfi_escape 0xf,0x3,0x76,0x58,0x6
	pushq	%rbx
	addq	$-128, %rsp
	.cfi_escape 0x10,0x3,0x2,0x76,0x50
	movq	%rsi, -128(%rbp)
	movq	%r9, -120(%rbp)
	cmpq	$256, %rdx
	jbe	.L956
	movq	%rdi, %r11
	movq	%rdi, -152(%rbp)
	movq	%r8, %rbx
	shrq	$2, %r11
	movq	%r11, %rdi
	andl	$15, %edi
	movq	%rdi, -144(%rbp)
	jne	.L957
	movq	%rdx, -136(%rbp)
	movq	%r13, %r11
.L801:
	movq	8(%rbx), %rdx
	movq	16(%rbx), %r9
	movq	%rdx, %rsi
	leaq	1(%r9), %rdi
	leaq	(%rdx,%rdx,8), %rcx
	xorq	(%rbx), %rdi
	shrq	$11, %rsi
	rorx	$40, %rdx, %rax
	leaq	2(%r9), %rdx
	addq	%rdi, %rax
	xorq	%rsi, %rcx
	movq	%rax, %r8
	rorx	$40, %rax, %rsi
	xorq	%rdx, %rcx
	shrq	$11, %r8
	leaq	(%rax,%rax,8), %rdx
	leaq	3(%r9), %rax
	addq	%rcx, %rsi
	xorq	%r8, %rdx
	movq	%rsi, %r8
	xorq	%rax, %rdx
	leaq	(%rsi,%rsi,8), %rax
	rorx	$40, %rsi, %r10
	shrq	$11, %r8
	addq	%rdx, %r10
	leaq	4(%r9), %rsi
	addq	$5, %r9
	xorq	%r8, %rax
	rorx	$40, %r10, %r8
	movq	%r9, 16(%rbx)
	xorq	%rsi, %rax
	movq	%r10, %rsi
	shrq	$11, %rsi
	addq	%rax, %r8
	movq	%rsi, %r14
	leaq	(%r10,%r10,8), %rsi
	leaq	(%r8,%r8,8), %r10
	xorq	%r14, %rsi
	movq	%r8, %r14
	rorx	$40, %r8, %r8
	shrq	$11, %r14
	xorq	%r9, %rsi
	movabsq	$68719476719, %r9
	xorq	%r14, %r10
	addq	%rsi, %r8
	movl	%esi, %esi
	vmovq	%r10, %xmm7
	movq	-136(%rbp), %r10
	vpinsrq	$1, %r8, %xmm7, %xmm1
	movq	%r10, %r14
	vmovdqu	%xmm1, (%rbx)
	shrq	$4, %r14
	cmpq	%r9, %r10
	movl	$4294967295, %r9d
	movq	%r14, %r8
	leaq	192(%r12), %r14
	cmova	%r9, %r8
	movl	%ecx, %r9d
	shrq	$32, %rcx
	imulq	%r8, %r9
	imulq	%r8, %rcx
	imulq	%r8, %rsi
	shrq	$32, %r9
	salq	$6, %r9
	shrq	$32, %rcx
	vmovdqa32	(%r11,%r9), %zmm3
	movl	%edi, %r9d
	shrq	$32, %rdi
	imulq	%r8, %r9
	salq	$6, %rcx
	shrq	$32, %rsi
	imulq	%r8, %rdi
	salq	$6, %rsi
	vmovdqa32	(%r11,%rsi), %zmm5
	shrq	$32, %r9
	salq	$6, %r9
	shrq	$32, %rdi
	vmovdqa32	(%r11,%r9), %zmm2
	salq	$6, %rdi
	vpminsd	%zmm3, %zmm2, %zmm1
	vpmaxsd	(%r11,%rdi), %zmm1, %zmm1
	movq	%rdx, %rdi
	movl	%edx, %edx
	shrq	$32, %rdi
	imulq	%r8, %rdx
	vpmaxsd	%zmm3, %zmm2, %zmm2
	imulq	%r8, %rdi
	vpminsd	%zmm2, %zmm1, %zmm1
	vmovdqa32	(%r11,%rcx), %zmm2
	vpbroadcastd	%xmm1, %zmm0
	vmovdqa32	%zmm1, (%r12)
	shrq	$32, %rdx
	vpxord	%zmm0, %zmm1, %zmm1
	shrq	$32, %rdi
	salq	$6, %rdx
	salq	$6, %rdi
	vmovdqa32	(%r11,%rdi), %zmm4
	vpminsd	%zmm4, %zmm2, %zmm3
	vpmaxsd	(%r11,%rdx), %zmm3, %zmm3
	movl	%eax, %edx
	shrq	$32, %rax
	imulq	%r8, %rdx
	vpmaxsd	%zmm4, %zmm2, %zmm2
	imulq	%r8, %rax
	vpminsd	%zmm2, %zmm3, %zmm3
	vmovdqa32	%zmm3, 64(%r12)
	vpxord	%zmm0, %zmm3, %zmm3
	shrq	$32, %rdx
	vpord	%zmm3, %zmm1, %zmm1
	salq	$6, %rdx
	shrq	$32, %rax
	vmovdqa32	(%r11,%rdx), %zmm4
	salq	$6, %rax
	vpminsd	%zmm5, %zmm4, %zmm2
	vpmaxsd	(%r11,%rax), %zmm2, %zmm2
	vpmaxsd	%zmm5, %zmm4, %zmm4
	vpminsd	%zmm4, %zmm2, %zmm2
	vmovdqa32	%zmm2, 128(%r12)
	vpxord	%zmm0, %zmm2, %zmm2
	vpord	%zmm2, %zmm1, %zmm1
	vptestnmd	%zmm1, %zmm1, %k0
	kortestw	%k0, %k0
	jc	.L803
	vpbroadcastq	.LC10(%rip), %zmm0
	movl	$4, %esi
	movq	%r12, %rdi
	vmovdqu64	%zmm0, 192(%r12)
	vmovdqu64	%zmm0, 256(%r12)
	vzeroupper
	call	_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	vpbroadcastd	(%r12), %zmm0
	vpbroadcastd	188(%r12), %zmm1
	vpternlogd	$0xFF, %zmm2, %zmm2, %zmm2
	vpaddd	%zmm2, %zmm1, %zmm2
	vpcmpd	$0, %zmm2, %zmm0, %k0
	kortestw	%k0, %k0
	jnc	.L805
	leaq	-112(%rbp), %rdx
	movq	%r14, %rcx
	movq	%r15, %rsi
	movq	%r13, %rdi
	call	_ZN3hwy11N_AVX3_ZEN46detail22MaybePartitionTwoValueINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L951
.L805:
	movl	96(%r12), %ecx
	movl	$23, %eax
	movl	$24, %edx
	cmpl	%ecx, 92(%r12)
	je	.L845
	jmp	.L850
	.p2align 4,,10
	.p2align 3
.L848:
	testq	%rax, %rax
	je	.L958
.L845:
	movq	%rax, %rdx
	subq	$1, %rax
	movl	(%r12,%rax,4), %esi
	cmpl	%esi, %ecx
	je	.L848
	cmpl	(%r12,%rdx,4), %ecx
	je	.L850
	movl	%esi, %ecx
	jmp	.L847
	.p2align 4,,10
	.p2align 3
.L851:
	cmpq	$47, %rdx
	je	.L954
.L850:
	movq	%rdx, %rsi
	addq	$1, %rdx
	cmpl	(%r12,%rdx,4), %ecx
	je	.L851
	movl	$24, %edx
	subq	$23, %rsi
	subq	%rax, %rdx
	cmpq	%rdx, %rsi
	jb	.L847
.L954:
	movl	(%r12,%rax,4), %ecx
.L847:
	vpbroadcastd	%ecx, %zmm1
.L955:
	movl	$1, -136(%rbp)
.L843:
	cmpq	$0, -120(%rbp)
	je	.L959
	leaq	-16(%r15), %rdx
	vmovdqa32	%zmm1, %zmm0
	leaq	0(%r13,%rdx,4), %r10
	movq	%rdx, %r9
	movq	%rdx, %rcx
	vmovdqu64	(%r10), %zmm6
	andl	$63, %r9d
	andl	$48, %ecx
	je	.L854
	vmovdqu64	0(%r13), %zmm2
	movq	$-1, %rdi
	vpcmpd	$6, %zmm1, %zmm2, %k1
	knotw	%k1, %k2
	vpcompressd	%zmm2, %zmm1{%k2}{z}
	kmovw	%k2, %eax
	popcntq	%rax, %rax
	bzhi	%rax, %rdi, %rcx
	kmovw	%ecx, %k7
	vmovdqu32	%zmm1, 0(%r13){%k7}
	vpcompressd	%zmm2, %zmm1{%k1}{z}
	kmovw	%k1, %ecx
	leaq	0(%r13,%rax,4), %rax
	vmovdqu64	%zmm1, (%r12)
	popcntq	%rcx, %rcx
	testb	$32, %dl
	je	.L855
	vmovdqu64	64(%r13), %zmm1
	vpcmpd	$6, %zmm0, %zmm1, %k1
	knotw	%k1, %k2
	vpcompressd	%zmm1, %zmm2{%k2}{z}
	kmovw	%k2, %esi
	popcntq	%rsi, %rsi
	bzhi	%rsi, %rdi, %r8
	kmovw	%r8d, %k4
	vmovdqu32	%zmm2, (%rax){%k4}
	vpcompressd	%zmm1, %zmm2{%k1}{z}
	leaq	(%rax,%rsi,4), %rax
	vmovdqu64	%zmm2, (%r12,%rcx,4)
	kmovw	%k1, %esi
	popcntq	%rsi, %rsi
	addq	%rsi, %rcx
	cmpq	$47, %r9
	jbe	.L855
	vmovdqu64	128(%r13), %zmm1
	vpcmpd	$6, %zmm0, %zmm1, %k1
	knotw	%k1, %k2
	vpcompressd	%zmm1, %zmm2{%k2}{z}
	kmovw	%k2, %esi
	popcntq	%rsi, %rsi
	bzhi	%rsi, %rdi, %rdi
	kmovw	%edi, %k7
	vmovdqu32	%zmm2, (%rax){%k7}
	vpcompressd	%zmm1, %zmm2{%k1}{z}
	leaq	(%rax,%rsi,4), %rax
	vmovdqu64	%zmm2, (%r12,%rcx,4)
	kmovw	%k1, %esi
	popcntq	%rsi, %rsi
	addq	%rsi, %rcx
	leaq	1(%r9), %rsi
	cmpq	$17, %rsi
	leaq	0(,%rcx,4), %r8
	sbbq	%rsi, %rsi
	andq	$-32, %rsi
	addq	$48, %rsi
	cmpq	%rsi, %r9
	jne	.L960
.L857:
	movq	%rax, %r9
	movq	%rdx, %rsi
	subq	%r13, %r9
	subq	%rcx, %rsi
	sarq	$2, %r9
	leaq	0(%r13,%rsi,4), %r11
	subq	%r9, %rdx
	subq	%r9, %rsi
	movq	%rdx, %r14
	leaq	(%rax,%rsi,4), %r10
	movq	%rsi, %rdx
	leaq	(%rax,%r14,4), %rdi
	vmovq	%rdi, %xmm8
	.p2align 4,,10
	.p2align 3
.L859:
	movl	%r8d, %ecx
	cmpl	$8, %r8d
	jnb	.L861
	testb	$4, %r8b
	jne	.L961
	testl	%ecx, %ecx
	jne	.L962
.L862:
	movl	%r8d, %ecx
	cmpl	$8, %r8d
	jnb	.L865
	andl	$4, %r8d
	jne	.L963
	testl	%ecx, %ecx
	jne	.L964
.L866:
	testq	%rdx, %rdx
	je	.L905
.L880:
	leaq	256(%rax), %rsi
	leaq	-256(%r10), %rdi
	vmovdqu64	(%rax), %zmm15
	vmovdqu64	64(%rax), %zmm14
	vmovdqu64	128(%rax), %zmm13
	vmovdqu64	192(%rax), %zmm12
	vmovdqu64	-128(%r10), %zmm9
	vmovdqu64	-64(%r10), %zmm7
	vmovdqu64	-256(%r10), %zmm11
	vmovdqu64	-192(%r10), %zmm10
	cmpq	%rdi, %rsi
	je	.L906
	xorl	%ecx, %ecx
	movq	$-1, %r8
	jmp	.L873
	.p2align 4,,10
	.p2align 3
.L966:
	vmovdqu64	-128(%rdi), %zmm2
	vmovdqu64	-64(%rdi), %zmm1
	prefetcht0	-1024(%rdi)
	subq	$256, %rdi
	vmovdqu64	(%rdi), %zmm4
	vmovdqu64	64(%rdi), %zmm3
.L872:
	vpcmpd	$6, %zmm0, %zmm4, %k1
	knotw	%k1, %k2
	vpcompressd	%zmm4, %zmm5{%k2}{z}
	kmovw	%k2, %r10d
	popcntq	%r10, %r10
	vmovdqu64	%zmm5, (%rax,%rcx,4)
	addq	%rcx, %r10
	vpcompressd	%zmm4, %zmm5{%k1}{z}
	kmovw	%k1, %ecx
	vpcmpd	$6, %zmm0, %zmm3, %k1
	leaq	-16(%rdx,%r10), %r11
	popcntq	%rcx, %rcx
	bzhi	%rcx, %r8, %rcx
	kmovw	%ecx, %k3
	vmovdqu32	%zmm5, (%rax,%r11,4){%k3}
	knotw	%k1, %k2
	vpcompressd	%zmm3, %zmm4{%k2}{z}
	kmovw	%k2, %ecx
	popcntq	%rcx, %rcx
	vmovdqu64	%zmm4, (%rax,%r10,4)
	addq	%r10, %rcx
	vpcompressd	%zmm3, %zmm4{%k1}{z}
	kmovw	%k1, %r10d
	vpcmpd	$6, %zmm0, %zmm2, %k1
	leaq	-32(%rdx,%rcx), %r11
	popcntq	%r10, %r10
	bzhi	%r10, %r8, %r10
	kmovw	%r10d, %k5
	vmovdqu32	%zmm4, (%rax,%r11,4){%k5}
	knotw	%k1, %k2
	vpcompressd	%zmm2, %zmm3{%k2}{z}
	kmovw	%k2, %r10d
	popcntq	%r10, %r10
	vmovdqu64	%zmm3, (%rax,%rcx,4)
	addq	%rcx, %r10
	vpcompressd	%zmm2, %zmm3{%k1}{z}
	kmovw	%k1, %ecx
	vpcmpd	$6, %zmm0, %zmm1, %k1
	leaq	-48(%rdx,%r10), %r11
	popcntq	%rcx, %rcx
	subq	$64, %rdx
	bzhi	%rcx, %r8, %rcx
	kmovw	%ecx, %k6
	vmovdqu32	%zmm3, (%rax,%r11,4){%k6}
	knotw	%k1, %k2
	vpcompressd	%zmm1, %zmm2{%k2}{z}
	kmovw	%k2, %ecx
	popcntq	%rcx, %rcx
	addq	%r10, %rcx
	vmovdqu64	%zmm2, (%rax,%r10,4)
	vpcompressd	%zmm1, %zmm2{%k1}{z}
	kmovw	%k1, %r10d
	leaq	(%rdx,%rcx), %r11
	popcntq	%r10, %r10
	bzhi	%r10, %r8, %r10
	kmovw	%r10d, %k7
	vmovdqu32	%zmm2, (%rax,%r11,4){%k7}
	cmpq	%rdi, %rsi
	je	.L965
.L873:
	movq	%rsi, %r10
	subq	%rax, %r10
	sarq	$2, %r10
	subq	%rcx, %r10
	cmpq	$64, %r10
	ja	.L966
	vmovdqu64	(%rsi), %zmm4
	vmovdqu64	64(%rsi), %zmm3
	prefetcht0	1024(%rsi)
	addq	$256, %rsi
	vmovdqu64	-128(%rsi), %zmm2
	vmovdqu64	-64(%rsi), %zmm1
	jmp	.L872
	.p2align 4,,10
	.p2align 3
.L957:
	movl	$16, %eax
	subq	%rdi, %rax
	leaq	0(%r13,%rax,4), %r11
	leaq	-16(%rdi,%rdx), %rax
	movq	%rax, -136(%rbp)
	jmp	.L801
	.p2align 4,,10
	.p2align 3
.L965:
	leaq	(%rax,%rcx,4), %rsi
.L870:
	vpcmpd	$6, %zmm0, %zmm15, %k1
	knotw	%k1, %k2
	vpcompressd	%zmm15, %zmm1{%k2}{z}
	kmovw	%k2, %edi
	popcntq	%rdi, %rdi
	vmovdqu64	%zmm1, (%rsi)
	vpcompressd	%zmm15, %zmm1{%k1}{z}
	kmovw	%k1, %esi
	addq	%rcx, %rdi
	vpcmpd	$6, %zmm0, %zmm14, %k1
	leaq	-16(%rdx,%rdi), %r8
	popcntq	%rsi, %rsi
	movq	$-1, %rcx
	bzhi	%rsi, %rcx, %rsi
	kmovw	%esi, %k4
	vmovdqu32	%zmm1, (%rax,%r8,4){%k4}
	knotw	%k1, %k2
	vpcompressd	%zmm14, %zmm1{%k2}{z}
	kmovw	%k2, %esi
	popcntq	%rsi, %rsi
	vmovdqu64	%zmm1, (%rax,%rdi,4)
	addq	%rdi, %rsi
	vpcompressd	%zmm14, %zmm1{%k1}{z}
	kmovw	%k1, %edi
	vpcmpd	$6, %zmm0, %zmm13, %k1
	leaq	-32(%rdx,%rsi), %r8
	popcntq	%rdi, %rdi
	bzhi	%rdi, %rcx, %rdi
	kmovw	%edi, %k4
	vmovdqu32	%zmm1, (%rax,%r8,4){%k4}
	knotw	%k1, %k2
	vpcompressd	%zmm13, %zmm1{%k2}{z}
	kmovw	%k2, %edi
	popcntq	%rdi, %rdi
	vmovdqu64	%zmm1, (%rax,%rsi,4)
	addq	%rsi, %rdi
	vpcompressd	%zmm13, %zmm1{%k1}{z}
	kmovw	%k1, %esi
	vpcmpd	$6, %zmm0, %zmm12, %k1
	leaq	-48(%rdx,%rdi), %r8
	popcntq	%rsi, %rsi
	bzhi	%rsi, %rcx, %rsi
	kmovw	%esi, %k4
	vmovdqu32	%zmm1, (%rax,%r8,4){%k4}
	knotw	%k1, %k2
	vpcompressd	%zmm12, %zmm1{%k2}{z}
	kmovw	%k2, %esi
	popcntq	%rsi, %rsi
	vmovdqu64	%zmm1, (%rax,%rdi,4)
	addq	%rdi, %rsi
	vpcompressd	%zmm12, %zmm1{%k1}{z}
	kmovw	%k1, %edi
	vpcmpd	$6, %zmm0, %zmm11, %k1
	leaq	-64(%rdx,%rsi), %r8
	popcntq	%rdi, %rdi
	bzhi	%rdi, %rcx, %rdi
	kmovw	%edi, %k4
	vmovdqu32	%zmm1, (%rax,%r8,4){%k4}
	knotw	%k1, %k2
	vpcompressd	%zmm11, %zmm1{%k2}{z}
	kmovw	%k2, %edi
	popcntq	%rdi, %rdi
	vmovdqu64	%zmm1, (%rax,%rsi,4)
	addq	%rsi, %rdi
	vpcompressd	%zmm11, %zmm1{%k1}{z}
	kmovw	%k1, %esi
	vpcmpd	$6, %zmm0, %zmm10, %k1
	leaq	-80(%rdx,%rdi), %r8
	popcntq	%rsi, %rsi
	bzhi	%rsi, %rcx, %rsi
	kmovw	%esi, %k4
	vmovdqu32	%zmm1, (%rax,%r8,4){%k4}
	knotw	%k1, %k2
	vpcompressd	%zmm10, %zmm1{%k2}{z}
	kmovw	%k2, %esi
	popcntq	%rsi, %rsi
	vmovdqu64	%zmm1, (%rax,%rdi,4)
	addq	%rdi, %rsi
	vpcompressd	%zmm10, %zmm1{%k1}{z}
	kmovw	%k1, %edi
	vpcmpd	$6, %zmm0, %zmm9, %k1
	leaq	-96(%rdx,%rsi), %r8
	popcntq	%rdi, %rdi
	bzhi	%rdi, %rcx, %rdi
	kmovw	%edi, %k3
	vmovdqu32	%zmm1, (%rax,%r8,4){%k3}
	knotw	%k1, %k2
	vpcompressd	%zmm9, %zmm1{%k2}{z}
	kmovw	%k2, %edi
	popcntq	%rdi, %rdi
	vmovdqu64	%zmm1, (%rax,%rsi,4)
	addq	%rsi, %rdi
	vpcompressd	%zmm9, %zmm1{%k1}{z}
	kmovw	%k1, %esi
	vpcmpd	$6, %zmm0, %zmm7, %k1
	leaq	-112(%rdx,%rdi), %r8
	popcntq	%rsi, %rsi
	bzhi	%rsi, %rcx, %rsi
	kmovw	%esi, %k5
	leaq	-128(%rdx), %rsi
	vmovdqu32	%zmm1, (%rax,%r8,4){%k5}
	knotw	%k1, %k2
	vpcompressd	%zmm7, %zmm1{%k2}{z}
	kmovw	%k2, %edx
	popcntq	%rdx, %rdx
	addq	%rdi, %rdx
	vmovdqu64	%zmm1, (%rax,%rdi,4)
	vpcompressd	%zmm7, %zmm1{%k1}{z}
	kmovw	%k1, %edi
	popcntq	%rdi, %rdi
	bzhi	%rdi, %rcx, %rcx
	addq	%rdx, %rsi
	kmovw	%ecx, %k6
	movq	%r14, %rcx
	vmovdqu32	%zmm1, (%rax,%rsi,4){%k6}
	leaq	(%rax,%rdx,4), %rdi
	subq	%rdx, %rcx
.L869:
	movq	%rdi, %rsi
	cmpq	$15, %rcx
	ja	.L874
	leaq	-64(%rax,%r14,4), %rsi
.L874:
	vpcmpd	$6, %zmm0, %zmm6, %k1
	vmovdqu64	(%rsi), %zmm7
	vmovq	%xmm8, %rcx
	movq	$-1, %rsi
	vmovdqu64	%zmm7, (%rcx)
	knotw	%k1, %k2
	vpcompressd	%zmm6, %zmm0{%k2}{z}
	kmovw	%k2, %ecx
	popcntq	%rcx, %rcx
	addq	%rcx, %rdx
	bzhi	%rcx, %rsi, %r8
	kmovw	%r8d, %k4
	kmovw	%k1, %ecx
	vmovdqu32	%zmm0, (%rdi){%k4}
	vpcompressd	%zmm6, %zmm0{%k1}{z}
	popcntq	%rcx, %rcx
	leaq	(%rdx,%r9), %r14
	bzhi	%rcx, %rsi, %rsi
	kmovw	%esi, %k4
	vmovdqu32	%zmm0, (%rax,%rdx,4){%k4}
	movq	-120(%rbp), %r9
	subq	$1, %r9
	cmpl	$2, -136(%rbp)
	je	.L967
	movq	-128(%rbp), %rsi
	movq	%rbx, %r8
	movq	%r12, %rcx
	movq	%r14, %rdx
	movq	%r13, %rdi
	movq	%r9, -120(%rbp)
	vzeroupper
	call	_ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	cmpl	$3, -136(%rbp)
	movq	-120(%rbp), %r9
	je	.L951
.L876:
	movq	%r15, %rdx
	movq	-128(%rbp), %rsi
	leaq	0(%r13,%r14,4), %rdi
	movq	%rbx, %r8
	subq	%r14, %rdx
	movq	%r12, %rcx
	call	_ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L951:
	subq	$-128, %rsp
	popq	%rbx
	popq	%r10
	.cfi_remember_state
	.cfi_def_cfa 10, 0
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	leaq	-8(%r10), %rsp
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L865:
	.cfi_restore_state
	movq	(%r12), %rcx
	leaq	8(%r11), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%r11)
	movl	%r8d, %ecx
	movq	-8(%r12,%rcx), %rsi
	movq	%rsi, -8(%r11,%rcx)
	movq	%r11, %rcx
	movq	%r12, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%r8d, %ecx
	shrl	$3, %ecx
	rep movsq
	jmp	.L866
	.p2align 4,,10
	.p2align 3
.L861:
	movq	(%r11), %rcx
	leaq	8(%rax), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%rax)
	movl	%r8d, %ecx
	movq	-8(%r11,%rcx), %rsi
	movq	%rsi, -8(%rax,%rcx)
	movq	%rax, %rcx
	movq	%r11, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%r8d, %ecx
	shrl	$3, %ecx
	rep movsq
	jmp	.L862
	.p2align 4,,10
	.p2align 3
.L958:
	movl	(%r12), %ecx
	jmp	.L847
	.p2align 4,,10
	.p2align 3
.L960:
	subq	%rsi, %r9
	leaq	0(%r13,%rsi,4), %r11
	leaq	(%r12,%r8), %rsi
.L879:
	movq	$-1, %rdi
	bzhi	%r9, %rdi, %rdi
	movzwl	%di, %edi
	kmovd	%edi, %k0
.L860:
	vmovdqu64	(%r11), %zmm1
	movq	$-1, %r8
	vpcmpd	$6, %zmm0, %zmm1, %k1
	kandnw	%k0, %k1, %k2
	vpcompressd	%zmm1, %zmm2{%k2}{z}
	kmovw	%k2, %edi
	kandw	%k0, %k1, %k1
	popcntq	%rdi, %rdi
	bzhi	%rdi, %r8, %r8
	kmovw	%r8d, %k4
	vmovdqu32	%zmm2, (%rax){%k4}
	leaq	(%rax,%rdi,4), %rax
	kmovw	%k1, %r8d
	popcntq	%r8, %r8
	addq	%rcx, %r8
	movq	%rax, %r9
	movq	%rdx, %rcx
	vpcompressd	%zmm1, %zmm2{%k1}{z}
	subq	%r13, %r9
	subq	%r8, %rcx
	vmovdqu64	%zmm2, (%rsi)
	salq	$2, %r8
	sarq	$2, %r9
	leaq	0(%r13,%rcx,4), %r11
	subq	%r9, %rdx
	movq	%rdx, %r14
	movq	%rcx, %rdx
	subq	%r9, %rdx
	leaq	(%rax,%r14,4), %rdi
	leaq	(%rax,%rdx,4), %r10
	vmovq	%rdi, %xmm8
	jmp	.L859
	.p2align 4,,10
	.p2align 3
.L964:
	movzbl	(%r12), %esi
	movb	%sil, (%r11)
	testb	$2, %cl
	je	.L866
	movzwl	-2(%r12,%rcx), %esi
	movw	%si, -2(%r11,%rcx)
	jmp	.L866
	.p2align 4,,10
	.p2align 3
.L962:
	movzbl	(%r11), %esi
	movb	%sil, (%rax)
	testb	$2, %cl
	je	.L862
	movzwl	-2(%r11,%rcx), %esi
	movw	%si, -2(%rax,%rcx)
	jmp	.L862
	.p2align 4,,10
	.p2align 3
.L855:
	leaq	-16(%r9), %rsi
	leaq	1(%r9), %rdi
	andq	$-16, %rsi
	leaq	0(,%rcx,4), %r8
	addq	$16, %rsi
	cmpq	$16, %rdi
	movl	$16, %edi
	cmovbe	%rdi, %rsi
	cmpq	%r9, %rsi
	je	.L857
	subq	%rsi, %r9
	leaq	0(%r13,%rsi,4), %r11
	leaq	(%r12,%r8), %rsi
	cmpq	$255, %r9
	jbe	.L879
	movl	$65535, %edi
	kmovd	%edi, %k0
	jmp	.L860
	.p2align 4,,10
	.p2align 3
.L956:
	cmpq	$1, %rdx
	jbe	.L951
	leaq	1024(%rdi), %rax
	cmpq	%rax, %rsi
	jb	.L968
	movl	$16, %esi
	call	_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	jmp	.L951
	.p2align 4,,10
	.p2align 3
.L803:
	vmovdqu32	0(%r13), %zmm6
	movl	$16, %esi
	movq	$-1, %rax
	subq	-144(%rbp), %rsi
	bzhi	%rsi, %rax, %rax
	kmovw	%eax, %k6
	vpcmpd	$4, %zmm0, %zmm6, %k0
	kandw	%k6, %k0, %k0
	kmovw	%k0, %eax
	kortestw	%k0, %k0
	jne	.L969
	vpxor	%xmm4, %xmm4, %xmm4
	leaq	1024(%r13,%rsi,4), %rdi
	vmovdqa64	%zmm4, %zmm3
	.p2align 4,,10
	.p2align 3
.L809:
	movq	%rsi, %rcx
	addq	$256, %rsi
	cmpq	%rsi, %r15
	jb	.L813
	leaq	-1024(%rdi), %rax
.L808:
	vpxord	(%rax), %zmm0, %zmm1
	vpxord	64(%rax), %zmm0, %zmm2
	leaq	128(%rax), %rdx
	vpord	%zmm3, %zmm1, %zmm3
	vpord	%zmm4, %zmm2, %zmm4
	vpxord	128(%rax), %zmm0, %zmm1
	vpxord	192(%rax), %zmm0, %zmm2
	vpord	%zmm3, %zmm1, %zmm3
	vpxord	256(%rax), %zmm0, %zmm1
	vpord	%zmm4, %zmm2, %zmm4
	vpxord	320(%rax), %zmm0, %zmm2
	leaq	384(%rdx), %rax
	vpord	%zmm3, %zmm1, %zmm3
	vpxord	256(%rdx), %zmm0, %zmm1
	vpord	%zmm4, %zmm2, %zmm4
	vpxord	320(%rdx), %zmm0, %zmm2
	vpord	%zmm3, %zmm1, %zmm1
	vpord	%zmm4, %zmm2, %zmm2
	vmovdqa64	%zmm1, %zmm3
	vmovdqa64	%zmm2, %zmm4
	cmpq	%rax, %rdi
	jne	.L808
	vpord	%zmm2, %zmm1, %zmm1
	leaq	1408(%rdx), %rdi
	vptestnmd	%zmm1, %zmm1, %k0
	kortestw	%k0, %k0
	setc	%al
	testb	%al, %al
	jne	.L809
	vmovdqa32	0(%r13,%rcx,4), %zmm6
	vpcmpd	$4, %zmm0, %zmm6, %k0
	kortestw	%k0, %k0
	jne	.L811
	.p2align 4,,10
	.p2align 3
.L810:
	addq	$16, %rcx
	vmovdqa32	0(%r13,%rcx,4), %zmm6
	vpcmpd	$4, %zmm0, %zmm6, %k0
	kortestw	%k0, %k0
	je	.L810
.L811:
	kmovw	%k0, %eax
	tzcntl	%eax, %eax
	addq	%rcx, %rax
.L807:
	vpbroadcastd	0(%r13,%rax,4), %zmm5
	vmovdqa64	%zmm0, %zmm2
	leaq	0(%r13,%rax,4), %rdi
	vpcmpd	$6, %zmm0, %zmm5, %k0
	vmovdqa64	%zmm5, %zmm1
	kortestw	%k0, %k0
	jne	.L816
	leaq	-16(%r15), %rax
	xorl	%ecx, %ecx
	jmp	.L822
	.p2align 4,,10
	.p2align 3
.L817:
	kmovw	%k0, %edx
	popcntq	%rdx, %rdx
	addq	%rdx, %rcx
	leaq	-16(%rax), %rdx
	vmovdqu64	%zmm2, 0(%r13,%rax,4)
	cmpq	%rdx, %r15
	jbe	.L970
	movq	%rdx, %rax
.L822:
	vmovdqu32	0(%r13,%rax,4), %zmm6
	vpcmpd	$0, %zmm5, %zmm6, %k1
	vpcmpd	$0, %zmm0, %zmm6, %k0
	kmovw	%k1, %edx
	kmovw	%k0, %esi
	korw	%k0, %k1, %k1
	kortestw	%k1, %k1
	jc	.L817
	kmovw	%edx, %k0
	kmovw	%esi, %k5
	kxnorw	%k5, %k0, %k0
	kmovw	%k0, %edx
	tzcntl	%edx, %edx
	leaq	16(%rax), %rsi
	addq	%rax, %rdx
	addq	$32, %rax
	vpbroadcastd	0(%r13,%rdx,4), %zmm2
	movq	%r15, %rdx
	subq	%rcx, %rdx
	vmovdqa64	%zmm2, %zmm0
	vmovdqa64	%zmm2, -112(%rbp)
	cmpq	%rdx, %rax
	ja	.L818
	.p2align 4,,10
	.p2align 3
.L819:
	vmovdqu64	%zmm1, -64(%r13,%rax,4)
	movq	%rax, %rsi
	addq	$16, %rax
	cmpq	%rax, %rdx
	jnb	.L819
.L818:
	subq	%rsi, %rdx
	leaq	0(%r13,%rsi,4), %rcx
	movl	$65535, %eax
	cmpq	$255, %rdx
	ja	.L820
	movq	$-1, %rax
	bzhi	%rdx, %rax, %rax
	movzwl	%ax, %eax
.L820:
	kmovw	%eax, %k3
	vmovdqu32	%zmm5, (%rcx){%k3}
.L821:
	vpbroadcastd	(%r12), %zmm3
	vpcmpd	$0, .LC8(%rip), %zmm3, %k0
	vmovdqa64	%zmm3, %zmm1
	kortestw	%k0, %k0
	jc	.L901
	vpcmpd	$0, .LC9(%rip), %zmm3, %k0
	kortestw	%k0, %k0
	jc	.L839
	vpminsd	%zmm0, %zmm5, %zmm2
	vpcmpd	$6, %zmm2, %zmm3, %k0
	kortestw	%k0, %k0
	jne	.L971
	vmovdqa64	%zmm3, %zmm2
	movl	$256, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L834:
	movq	%rax, %rdx
	addq	$1, %rax
	salq	$4, %rdx
	addq	%rcx, %rdx
	vpminsd	0(%r13,%rdx,4), %zmm2, %zmm0
	vmovdqa64	%zmm0, %zmm2
	cmpq	$16, %rax
	jne	.L834
	vpcmpd	$6, %zmm0, %zmm3, %k0
	kortestw	%k0, %k0
	jne	.L955
	leaq	256(%rsi), %rax
	cmpq	%rax, %r15
	jb	.L841
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L834
	.p2align 4,,10
	.p2align 3
.L813:
	movq	%rcx, %rdx
	addq	$16, %rcx
	cmpq	%rcx, %r15
	jb	.L972
	vmovdqa32	-64(%r13,%rcx,4), %zmm6
	vpcmpd	$4, %zmm0, %zmm6, %k0
	kmovw	%k0, %eax
	kortestw	%k0, %k0
	je	.L813
.L953:
	tzcntl	%eax, %eax
	addq	%rdx, %rax
	jmp	.L807
	.p2align 4,,10
	.p2align 3
.L854:
	movq	%r12, %rsi
	movq	%r13, %r11
	movq	%r13, %rax
	movq	%rdx, %r14
	vmovq	%r10, %xmm8
	testq	%r9, %r9
	jne	.L879
	jmp	.L880
.L967:
	vzeroupper
	jmp	.L876
.L906:
	movq	%rax, %rsi
	xorl	%ecx, %ecx
	jmp	.L870
.L905:
	movq	%rax, %rdi
	movq	%r14, %rcx
	jmp	.L869
.L959:
	leaq	-1(%r15), %rbx
	movq	%rbx, %r12
	shrq	%r12
	.p2align 4,,10
	.p2align 3
.L852:
	movq	%r12, %rdx
	movq	%r15, %rsi
	movq	%r13, %rdi
	call	_ZN3hwy11N_AVX3_ZEN46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	subq	$1, %r12
	jnb	.L852
	.p2align 4,,10
	.p2align 3
.L853:
	movl	0(%r13,%rbx,4), %edx
	movl	0(%r13), %eax
	movq	%rbx, %rsi
	movq	%r13, %rdi
	movl	%edx, 0(%r13)
	xorl	%edx, %edx
	movl	%eax, 0(%r13,%rbx,4)
	call	_ZN3hwy11N_AVX3_ZEN46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	subq	$1, %rbx
	jne	.L853
	jmp	.L951
	.p2align 4,,10
	.p2align 3
.L842:
	vpcmpd	$6, -64(%r13,%rsi,4), %zmm3, %k0
	kortestw	%k0, %k0
	jne	.L955
.L841:
	movq	%rsi, %rax
	addq	$16, %rsi
	cmpq	%rsi, %r15
	jnb	.L842
	cmpq	%rax, %r15
	je	.L901
	vpcmpd	$6, -64(%r13,%r15,4), %zmm3, %k0
	xorl	%eax, %eax
	kortestw	%k0, %k0
	sete	%al
	addl	$1, %eax
	movl	%eax, -136(%rbp)
	jmp	.L843
.L961:
	movl	(%r11), %esi
	movl	%esi, (%rax)
	movl	-4(%r11,%rcx), %esi
	movl	%esi, -4(%rax,%rcx)
	jmp	.L862
.L963:
	movl	(%r12), %esi
	movl	%esi, (%r11)
	movl	-4(%r12,%rcx), %esi
	movl	%esi, -4(%r11,%rcx)
	jmp	.L866
.L816:
	movq	%r15, %rsi
	leaq	-112(%rbp), %rdx
	movq	%r12, %rcx
	subq	%rax, %rsi
	call	_ZN3hwy11N_AVX3_ZEN46detail22MaybePartitionTwoValueINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L951
	vmovdqa64	-112(%rbp), %zmm0
	jmp	.L821
.L970:
	movl	$65535, %edi
	vmovdqu64	0(%r13), %zmm2
	kmovd	%edi, %k2
	cmpq	$255, %rax
	ja	.L823
	movq	$-1, %rdx
	bzhi	%rax, %rdx, %rdx
	movzwl	%dx, %edi
	kmovd	%edi, %k2
.L823:
	vpcmpd	$0, %zmm5, %zmm2, %k0
	vpcmpd	$0, %zmm0, %zmm2, %k1
	movq	%r15, %rdx
	kandw	%k2, %k1, %k1
	knotw	%k2, %k2
	korw	%k1, %k0, %k0
	korw	%k2, %k0, %k0
	kortestw	%k0, %k0
	setc	%sil
	subq	%rcx, %rdx
	testb	%sil, %sil
	je	.L973
	xorl	%ecx, %ecx
	kmovw	%k1, %eax
	vmovdqu64	%zmm0, 0(%r13)
	popcntq	%rax, %rcx
	movq	%rdx, %rax
	subq	%rcx, %rax
	cmpq	$15, %rax
	jbe	.L828
	leaq	-16(%rax), %rcx
	movq	-152(%rbp), %rsi
	movq	%rcx, %rdx
	shrq	$4, %rdx
	salq	$6, %rdx
	leaq	64(%r13,%rdx), %rdx
	.p2align 4,,10
	.p2align 3
.L829:
	vmovdqu64	%zmm1, (%rsi)
	addq	$64, %rsi
	cmpq	%rsi, %rdx
	jne	.L829
	andq	$-16, %rcx
	movl	$65535, %ebx
	vmovdqa64	%zmm5, (%r12)
	leaq	16(%rcx), %rdx
	kmovd	%ebx, %k1
	subq	%rdx, %rax
	leaq	0(%r13,%rdx,4), %r13
	cmpq	$255, %rax
	jbe	.L881
.L830:
	vmovdqu32	(%r12), %zmm0{%k1}{z}
	vmovdqu32	%zmm0, 0(%r13){%k1}
	vzeroupper
	jmp	.L951
.L838:
	vmovdqu32	-64(%r13,%rsi,4), %zmm6
	vpcmpd	$6, %zmm3, %zmm6, %k0
	kortestw	%k0, %k0
	jne	.L955
.L837:
	movq	%rsi, %rax
	addq	$16, %rsi
	cmpq	%rsi, %r15
	jnb	.L838
	cmpq	%rax, %r15
	je	.L839
	vmovdqu32	-64(%r13,%r15,4), %zmm6
	vpcmpd	$6, %zmm3, %zmm6, %k0
	kortestw	%k0, %k0
	jne	.L955
.L839:
	movl	$3, -136(%rbp)
	vpternlogd	$0xFF, %zmm1, %zmm1, %zmm1
	vpaddd	%zmm1, %zmm3, %zmm1
	jmp	.L843
.L972:
	leaq	-16(%r15), %rdx
	vmovdqu32	0(%r13,%rdx,4), %zmm6
	vpcmpd	$4, %zmm0, %zmm6, %k0
	kmovw	%k0, %eax
	kortestw	%k0, %k0
	jne	.L953
	vzeroupper
	jmp	.L951
.L969:
	tzcntl	%eax, %eax
	jmp	.L807
.L968:
	xorl	%eax, %eax
	cmpq	$15, %rdx
	jbe	.L795
	leaq	-16(%rdx), %rdx
	movq	(%rdi), %rcx
	movq	%rdx, %rax
	shrq	$4, %rax
	movq	%rcx, (%r12)
	addq	$1, %rax
	salq	$6, %rax
	movl	%eax, %ecx
	movq	-8(%rdi,%rcx), %rsi
	leaq	8(%r12), %rdi
	andq	$-8, %rdi
	movq	%rsi, -8(%r12,%rcx)
	movq	%r12, %rcx
	movq	%r13, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%eax, %ecx
	movq	%rdx, %rax
	shrl	$3, %ecx
	andq	$-16, %rax
	rep movsq
	addq	$16, %rax
.L795:
	movq	%r15, %rdx
	leaq	0(,%rax,4), %rbx
	subq	%rax, %rdx
	movl	$65535, %eax
	leaq	(%r12,%rbx), %r14
	addq	%r13, %rbx
	kmovd	%eax, %k4
	cmpq	$255, %rdx
	ja	.L796
	movq	$-1, %rax
	bzhi	%rdx, %rax, %rax
	movzwl	%ax, %eax
	kmovd	%eax, %k4
.L796:
	leal	-1(%r15), %eax
	movl	$32, %edx
	movl	$1, %esi
	vmovdqu32	(%rbx), %zmm0{%k4}{z}
	bsrl	%eax, %eax
	xorl	$31, %eax
	vmovdqu32	%zmm0, (%r14){%k4}
	vpbroadcastq	.LC10(%rip), %zmm0
	subl	%eax, %edx
	movl	$1, %eax
	shlx	%rdx, %rsi, %rsi
	shrq	$4, %rsi
	cmove	%rax, %rsi
	movq	%r15, %rax
	leaq	1(%rsi), %rdx
	salq	$4, %rdx
	cmpq	%rdx, %r15
	jnb	.L800
.L797:
	vmovdqu64	%zmm0, (%r12,%rax,4)
	addq	$16, %rax
	cmpq	%rdx, %rax
	jb	.L797
.L800:
	movq	%r12, %rdi
	vzeroupper
	call	_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	cmpq	$15, %r15
	jbe	.L799
	leaq	-16(%r15), %rax
	movq	(%r12), %rdx
	leaq	8(%r13), %rdi
	movq	%r12, %rsi
	shrq	$4, %rax
	andq	$-8, %rdi
	addq	$1, %rax
	movq	%rdx, 0(%r13)
	salq	$6, %rax
	movl	%eax, %edx
	movq	-8(%r12,%rdx), %rcx
	movq	%rcx, -8(%r13,%rdx)
	subq	%rdi, %r13
	leal	(%rax,%r13), %ecx
	subq	%r13, %rsi
	shrl	$3, %ecx
	rep movsq
.L799:
	vmovdqu32	(%r14), %zmm0{%k4}{z}
	vmovdqu32	%zmm0, (%rbx){%k4}
	vzeroupper
	jmp	.L951
.L828:
	vmovdqa64	%zmm5, (%r12)
.L881:
	movq	$-1, %rdx
	bzhi	%rax, %rdx, %rax
	movzwl	%ax, %eax
	kmovd	%eax, %k1
	jmp	.L830
.L901:
	movl	$2, -136(%rbp)
	jmp	.L843
.L971:
	vpmaxsd	%zmm0, %zmm5, %zmm5
	vpcmpd	$6, %zmm3, %zmm5, %k0
	kortestw	%k0, %k0
	jne	.L955
	vmovdqa64	%zmm3, %zmm2
	movl	$256, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L835:
	movq	%rax, %rdx
	addq	$1, %rax
	salq	$4, %rdx
	addq	%rcx, %rdx
	vpmaxsd	0(%r13,%rdx,4), %zmm2, %zmm0
	vmovdqa64	%zmm0, %zmm2
	cmpq	$16, %rax
	jne	.L835
	vpcmpd	$6, %zmm3, %zmm0, %k0
	kortestw	%k0, %k0
	jne	.L955
	leaq	256(%rsi), %rax
	cmpq	%rax, %r15
	jb	.L837
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L835
.L973:
	knotw	%k0, %k0
	kmovw	%k0, %ecx
	tzcntl	%ecx, %ecx
	vpbroadcastd	0(%r13,%rcx,4), %zmm2
	leaq	16(%rax), %rcx
	vmovdqa64	%zmm2, %zmm0
	vmovdqa64	%zmm2, -112(%rbp)
	cmpq	%rdx, %rcx
	ja	.L825
.L826:
	vmovdqu64	%zmm1, -64(%r13,%rcx,4)
	movq	%rcx, %rax
	addq	$16, %rcx
	cmpq	%rdx, %rcx
	jbe	.L826
.L825:
	subq	%rax, %rdx
	leaq	0(%r13,%rax,4), %rsi
	movl	$-1, %ecx
	cmpq	$255, %rdx
	ja	.L827
	orq	$-1, %rcx
	bzhi	%rdx, %rcx, %rcx
.L827:
	kmovw	%ecx, %k5
	vmovdqu32	%zmm5, (%rsi){%k5}
	jmp	.L821
	.cfi_endproc
.LFE18800:
	.size	_ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0, .-_ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	.section	.text._ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0, @function
_ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0:
.LFB18802:
	.cfi_startproc
	leaq	8(%rsp), %r10
	.cfi_def_cfa 10, 0
	andq	$-64, %rsp
	pushq	-8(%r10)
	pushq	%rbp
	movq	%rsp, %rbp
	.cfi_escape 0x10,0x6,0x2,0x76,0
	pushq	%r15
	pushq	%r14
	pushq	%r13
	.cfi_escape 0x10,0xf,0x2,0x76,0x78
	.cfi_escape 0x10,0xe,0x2,0x76,0x70
	.cfi_escape 0x10,0xd,0x2,0x76,0x68
	movq	%rdi, %r13
	pushq	%r12
	.cfi_escape 0x10,0xc,0x2,0x76,0x60
	movq	%rcx, %r12
	pushq	%r10
	.cfi_escape 0xf,0x3,0x76,0x58,0x6
	pushq	%rbx
	.cfi_escape 0x10,0x3,0x2,0x76,0x50
	movq	%rdx, %rbx
	addq	$-128, %rsp
	movq	%rsi, -128(%rbp)
	movq	%r9, -120(%rbp)
	cmpq	$256, %rdx
	jbe	.L1141
	movq	%rdi, %rax
	movq	%rdi, -152(%rbp)
	movq	%r8, %r14
	shrq	$2, %rax
	movq	%rax, %rdi
	andl	$15, %edi
	movq	%rdi, -144(%rbp)
	jne	.L1142
	movq	%rdx, -136(%rbp)
	movq	%r13, %r11
.L986:
	movq	8(%r14), %rdx
	movq	16(%r14), %r9
	movq	%rdx, %rsi
	leaq	1(%r9), %rdi
	leaq	(%rdx,%rdx,8), %rcx
	xorq	(%r14), %rdi
	shrq	$11, %rsi
	rorx	$40, %rdx, %rax
	leaq	2(%r9), %rdx
	addq	%rdi, %rax
	xorq	%rsi, %rcx
	movq	%rax, %r8
	rorx	$40, %rax, %rsi
	xorq	%rdx, %rcx
	shrq	$11, %r8
	leaq	(%rax,%rax,8), %rdx
	leaq	3(%r9), %rax
	addq	%rcx, %rsi
	xorq	%r8, %rdx
	movq	%rsi, %r8
	xorq	%rax, %rdx
	leaq	(%rsi,%rsi,8), %rax
	rorx	$40, %rsi, %r10
	shrq	$11, %r8
	addq	%rdx, %r10
	leaq	4(%r9), %rsi
	addq	$5, %r9
	xorq	%r8, %rax
	movq	%r10, %r15
	rorx	$40, %r10, %r8
	movq	%r9, 16(%r14)
	xorq	%rsi, %rax
	shrq	$11, %r15
	leaq	(%r10,%r10,8), %rsi
	addq	%rax, %r8
	xorq	%r15, %rsi
	movq	%r8, %r15
	leaq	(%r8,%r8,8), %r10
	xorq	%r9, %rsi
	rorx	$40, %r8, %r8
	shrq	$11, %r15
	addq	%rsi, %r8
	movl	%esi, %esi
	movabsq	$68719476719, %r9
	xorq	%r15, %r10
	movq	-136(%rbp), %r15
	vmovq	%r10, %xmm6
	vpinsrq	$1, %r8, %xmm6, %xmm1
	movq	%r15, %r8
	shrq	$4, %r8
	cmpq	%r9, %r15
	movl	$4294967295, %r9d
	vmovdqu	%xmm1, (%r14)
	cmova	%r9, %r8
	movl	%ecx, %r9d
	shrq	$32, %rcx
	leaq	192(%r12), %r15
	imulq	%r8, %r9
	imulq	%r8, %rcx
	imulq	%r8, %rsi
	shrq	$32, %r9
	salq	$6, %r9
	shrq	$32, %rcx
	vmovdqa32	(%r11,%r9), %zmm3
	movl	%edi, %r9d
	shrq	$32, %rdi
	imulq	%r8, %r9
	salq	$6, %rcx
	shrq	$32, %rsi
	imulq	%r8, %rdi
	salq	$6, %rsi
	vmovdqa32	(%r11,%rsi), %zmm5
	shrq	$32, %r9
	salq	$6, %r9
	shrq	$32, %rdi
	vmovdqa32	(%r11,%r9), %zmm2
	salq	$6, %rdi
	vpminsd	%zmm3, %zmm2, %zmm1
	vpmaxsd	(%r11,%rdi), %zmm1, %zmm1
	movq	%rdx, %rdi
	movl	%edx, %edx
	shrq	$32, %rdi
	imulq	%r8, %rdx
	vpmaxsd	%zmm3, %zmm2, %zmm2
	imulq	%r8, %rdi
	vpminsd	%zmm2, %zmm1, %zmm1
	vmovdqa32	(%r11,%rcx), %zmm2
	vpbroadcastd	%xmm1, %zmm0
	vmovdqa32	%zmm1, (%r12)
	shrq	$32, %rdx
	vpxord	%zmm0, %zmm1, %zmm1
	shrq	$32, %rdi
	salq	$6, %rdx
	salq	$6, %rdi
	vmovdqa32	(%r11,%rdi), %zmm4
	vpminsd	%zmm4, %zmm2, %zmm3
	vpmaxsd	(%r11,%rdx), %zmm3, %zmm3
	movl	%eax, %edx
	shrq	$32, %rax
	imulq	%r8, %rdx
	vpmaxsd	%zmm4, %zmm2, %zmm2
	imulq	%r8, %rax
	vpminsd	%zmm2, %zmm3, %zmm3
	vmovdqa32	%zmm3, 64(%r12)
	vpxord	%zmm0, %zmm3, %zmm3
	shrq	$32, %rdx
	vpord	%zmm3, %zmm1, %zmm1
	salq	$6, %rdx
	shrq	$32, %rax
	vmovdqa32	(%r11,%rdx), %zmm4
	salq	$6, %rax
	vpminsd	%zmm5, %zmm4, %zmm2
	vpmaxsd	(%r11,%rax), %zmm2, %zmm2
	vpmaxsd	%zmm5, %zmm4, %zmm4
	vpminsd	%zmm4, %zmm2, %zmm2
	vmovdqa32	%zmm2, 128(%r12)
	vpxord	%zmm0, %zmm2, %zmm2
	vpord	%zmm2, %zmm1, %zmm1
	vptestnmd	%zmm1, %zmm1, %k0
	kortestw	%k0, %k0
	jc	.L988
	vpbroadcastq	.LC10(%rip), %zmm0
	movl	$4, %esi
	movq	%r12, %rdi
	vmovdqu64	%zmm0, 192(%r12)
	vmovdqu64	%zmm0, 256(%r12)
	vzeroupper
	call	_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	vpbroadcastd	(%r12), %zmm0
	vpbroadcastd	188(%r12), %zmm1
	vpternlogd	$0xFF, %zmm2, %zmm2, %zmm2
	vpaddd	%zmm2, %zmm1, %zmm2
	vpcmpd	$0, %zmm2, %zmm0, %k0
	kortestw	%k0, %k0
	jnc	.L990
	leaq	-112(%rbp), %rdx
	movq	%r15, %rcx
	movq	%rbx, %rsi
	movq	%r13, %rdi
	call	_ZN3hwy6N_AVX36detail22MaybePartitionTwoValueINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L1136
.L990:
	movl	96(%r12), %ecx
	movl	$23, %eax
	movl	$24, %edx
	cmpl	%ecx, 92(%r12)
	je	.L1030
	jmp	.L1035
	.p2align 4,,10
	.p2align 3
.L1033:
	testq	%rax, %rax
	je	.L1143
.L1030:
	movq	%rax, %rdx
	subq	$1, %rax
	movl	(%r12,%rax,4), %esi
	cmpl	%esi, %ecx
	je	.L1033
	cmpl	(%r12,%rdx,4), %ecx
	je	.L1035
	movl	%esi, %ecx
	jmp	.L1032
	.p2align 4,,10
	.p2align 3
.L1036:
	cmpq	$47, %rdx
	je	.L1139
.L1035:
	movq	%rdx, %rsi
	addq	$1, %rdx
	cmpl	(%r12,%rdx,4), %ecx
	je	.L1036
	movl	$24, %edx
	subq	$23, %rsi
	subq	%rax, %rdx
	cmpq	%rdx, %rsi
	jb	.L1032
.L1139:
	movl	(%r12,%rax,4), %ecx
.L1032:
	vpbroadcastd	%ecx, %zmm1
.L1140:
	movl	$1, -136(%rbp)
.L1028:
	cmpq	$0, -120(%rbp)
	je	.L1144
	leaq	-16(%rbx), %rdx
	vmovdqa32	%zmm1, %zmm0
	leaq	0(%r13,%rdx,4), %r10
	movq	%rdx, %r9
	movq	%rdx, %rcx
	vmovdqu64	(%r10), %zmm13
	andl	$63, %r9d
	andl	$48, %ecx
	je	.L1039
	vmovdqu64	0(%r13), %zmm2
	vpcmpd	$6, %zmm1, %zmm2, %k2
	knotw	%k2, %k1
	vpcompressd	%zmm2, 0(%r13){%k1}
	kmovw	%k1, %eax
	kmovw	%k2, %ecx
	popcntq	%rax, %rax
	popcntq	%rcx, %rcx
	leaq	0(%r13,%rax,4), %rax
	vpcompressd	%zmm2, (%r12){%k2}
	testb	$32, %dl
	je	.L1040
	vmovdqu64	64(%r13), %zmm1
	vpcmpd	$6, %zmm0, %zmm1, %k1
	knotw	%k1, %k2
	vpcompressd	%zmm1, (%rax){%k2}
	kmovw	%k2, %esi
	popcntq	%rsi, %rsi
	vpcompressd	%zmm1, (%r12,%rcx,4){%k1}
	leaq	(%rax,%rsi,4), %rax
	kmovw	%k1, %esi
	popcntq	%rsi, %rsi
	addq	%rsi, %rcx
	cmpq	$47, %r9
	jbe	.L1040
	vmovdqu64	128(%r13), %zmm1
	vpcmpd	$6, %zmm0, %zmm1, %k1
	knotw	%k1, %k2
	vpcompressd	%zmm1, (%rax){%k2}
	kmovw	%k2, %esi
	popcntq	%rsi, %rsi
	vpcompressd	%zmm1, (%r12,%rcx,4){%k1}
	leaq	(%rax,%rsi,4), %rax
	kmovw	%k1, %esi
	popcntq	%rsi, %rsi
	addq	%rsi, %rcx
	leaq	1(%r9), %rsi
	cmpq	$17, %rsi
	leaq	0(,%rcx,4), %r8
	sbbq	%rsi, %rsi
	andq	$-32, %rsi
	addq	$48, %rsi
	cmpq	%rsi, %r9
	jne	.L1145
.L1042:
	movq	%rax, %r9
	movq	%rdx, %rsi
	subq	%r13, %r9
	subq	%rcx, %rsi
	sarq	$2, %r9
	leaq	0(%r13,%rsi,4), %r11
	subq	%r9, %rdx
	subq	%r9, %rsi
	movq	%rdx, %r15
	leaq	(%rax,%rsi,4), %r10
	movq	%rsi, %rdx
	leaq	(%rax,%r15,4), %rdi
	vmovq	%rdi, %xmm14
	.p2align 4,,10
	.p2align 3
.L1044:
	movl	%r8d, %ecx
	cmpl	$8, %r8d
	jnb	.L1046
	testb	$4, %r8b
	jne	.L1146
	testl	%ecx, %ecx
	jne	.L1147
.L1047:
	movl	%r8d, %ecx
	cmpl	$8, %r8d
	jnb	.L1050
	andl	$4, %r8d
	jne	.L1148
	testl	%ecx, %ecx
	jne	.L1149
.L1051:
	testq	%rdx, %rdx
	je	.L1090
.L1065:
	leaq	256(%rax), %rsi
	leaq	-256(%r10), %rdi
	vmovdqu64	(%rax), %zmm12
	vmovdqu64	64(%rax), %zmm11
	vmovdqu64	128(%rax), %zmm10
	vmovdqu64	192(%rax), %zmm9
	vmovdqu64	-128(%r10), %zmm6
	vmovdqu64	-64(%r10), %zmm5
	vmovdqu64	-256(%r10), %zmm8
	vmovdqu64	-192(%r10), %zmm7
	cmpq	%rdi, %rsi
	je	.L1091
	xorl	%ecx, %ecx
	jmp	.L1058
	.p2align 4,,10
	.p2align 3
.L1151:
	vmovdqu64	-128(%rdi), %zmm2
	vmovdqu64	-64(%rdi), %zmm1
	prefetcht0	-1024(%rdi)
	subq	$256, %rdi
	vmovdqu64	(%rdi), %zmm4
	vmovdqu64	64(%rdi), %zmm3
.L1057:
	vpcmpd	$6, %zmm0, %zmm4, %k2
	knotw	%k2, %k1
	kmovw	%k1, %r8d
	popcntq	%r8, %r8
	addq	%rcx, %r8
	vpcompressd	%zmm4, (%rax,%rcx,4){%k1}
	leaq	-16(%rdx,%r8), %rcx
	vpcompressd	%zmm4, (%rax,%rcx,4){%k2}
	vpcmpd	$6, %zmm0, %zmm3, %k2
	knotw	%k2, %k1
	kmovw	%k1, %ecx
	popcntq	%rcx, %rcx
	addq	%r8, %rcx
	vpcompressd	%zmm3, (%rax,%r8,4){%k1}
	leaq	-32(%rdx,%rcx), %r8
	vpcompressd	%zmm3, (%rax,%r8,4){%k2}
	vpcmpd	$6, %zmm0, %zmm2, %k2
	knotw	%k2, %k1
	kmovw	%k1, %r8d
	popcntq	%r8, %r8
	addq	%rcx, %r8
	vpcompressd	%zmm2, (%rax,%rcx,4){%k1}
	leaq	-48(%rdx,%r8), %rcx
	subq	$64, %rdx
	vpcompressd	%zmm2, (%rax,%rcx,4){%k2}
	vpcmpd	$6, %zmm0, %zmm1, %k2
	knotw	%k2, %k1
	kmovw	%k1, %ecx
	popcntq	%rcx, %rcx
	addq	%r8, %rcx
	vpcompressd	%zmm1, (%rax,%r8,4){%k1}
	leaq	(%rdx,%rcx), %r8
	vpcompressd	%zmm1, (%rax,%r8,4){%k2}
	cmpq	%rdi, %rsi
	je	.L1150
.L1058:
	movq	%rsi, %r8
	subq	%rax, %r8
	sarq	$2, %r8
	subq	%rcx, %r8
	cmpq	$64, %r8
	ja	.L1151
	vmovdqu64	(%rsi), %zmm4
	vmovdqu64	64(%rsi), %zmm3
	prefetcht0	1024(%rsi)
	addq	$256, %rsi
	vmovdqu64	-128(%rsi), %zmm2
	vmovdqu64	-64(%rsi), %zmm1
	jmp	.L1057
	.p2align 4,,10
	.p2align 3
.L1142:
	movl	$16, %eax
	subq	%rdi, %rax
	leaq	0(%r13,%rax,4), %r11
	leaq	-16(%rdi,%rdx), %rax
	movq	%rax, -136(%rbp)
	jmp	.L986
	.p2align 4,,10
	.p2align 3
.L1150:
	leaq	(%rax,%rcx,4), %rsi
.L1055:
	vpcmpd	$6, %zmm0, %zmm12, %k1
	movq	%r15, %rdi
	knotw	%k1, %k2
	vpcompressd	%zmm12, (%rsi){%k2}
	kmovw	%k2, %esi
	popcntq	%rsi, %rsi
	addq	%rsi, %rcx
	leaq	-16(%rdx,%rcx), %rsi
	vpcompressd	%zmm12, (%rax,%rsi,4){%k1}
	vpcmpd	$6, %zmm0, %zmm11, %k1
	knotw	%k1, %k2
	kmovw	%k2, %esi
	popcntq	%rsi, %rsi
	addq	%rcx, %rsi
	vpcompressd	%zmm11, (%rax,%rcx,4){%k2}
	leaq	-32(%rdx,%rsi), %rcx
	vpcompressd	%zmm11, (%rax,%rcx,4){%k1}
	vpcmpd	$6, %zmm0, %zmm10, %k1
	knotw	%k1, %k2
	kmovw	%k2, %ecx
	popcntq	%rcx, %rcx
	addq	%rsi, %rcx
	vpcompressd	%zmm10, (%rax,%rsi,4){%k2}
	leaq	-48(%rdx,%rcx), %rsi
	vpcompressd	%zmm10, (%rax,%rsi,4){%k1}
	vpcmpd	$6, %zmm0, %zmm9, %k1
	knotw	%k1, %k2
	kmovw	%k2, %esi
	popcntq	%rsi, %rsi
	addq	%rcx, %rsi
	vpcompressd	%zmm9, (%rax,%rcx,4){%k2}
	leaq	-64(%rdx,%rsi), %rcx
	vpcompressd	%zmm9, (%rax,%rcx,4){%k1}
	vpcmpd	$6, %zmm0, %zmm8, %k1
	knotw	%k1, %k2
	kmovw	%k2, %ecx
	popcntq	%rcx, %rcx
	addq	%rsi, %rcx
	vpcompressd	%zmm8, (%rax,%rsi,4){%k2}
	leaq	-80(%rdx,%rcx), %rsi
	vpcompressd	%zmm8, (%rax,%rsi,4){%k1}
	vpcmpd	$6, %zmm0, %zmm7, %k1
	knotw	%k1, %k2
	kmovw	%k2, %esi
	popcntq	%rsi, %rsi
	addq	%rcx, %rsi
	vpcompressd	%zmm7, (%rax,%rcx,4){%k2}
	leaq	-96(%rdx,%rsi), %rcx
	vpcompressd	%zmm7, (%rax,%rcx,4){%k1}
	vpcmpd	$6, %zmm0, %zmm6, %k1
	knotw	%k1, %k2
	kmovw	%k2, %ecx
	popcntq	%rcx, %rcx
	addq	%rsi, %rcx
	vpcompressd	%zmm6, (%rax,%rsi,4){%k2}
	leaq	-112(%rdx,%rcx), %rsi
	vpcompressd	%zmm6, (%rax,%rsi,4){%k1}
	vpcmpd	$6, %zmm0, %zmm5, %k1
	leaq	-128(%rdx), %rsi
	knotw	%k1, %k2
	kmovw	%k2, %edx
	popcntq	%rdx, %rdx
	addq	%rcx, %rdx
	vpcompressd	%zmm5, (%rax,%rcx,4){%k2}
	leaq	(%rsi,%rdx), %rcx
	subq	%rdx, %rdi
	vpcompressd	%zmm5, (%rax,%rcx,4){%k1}
	leaq	(%rax,%rdx,4), %rcx
.L1054:
	movq	%rcx, %rsi
	cmpq	$15, %rdi
	ja	.L1059
	leaq	-64(%rax,%r15,4), %rsi
.L1059:
	vmovdqu64	(%rsi), %zmm7
	vpcmpd	$6, %zmm0, %zmm13, %k2
	vmovq	%xmm14, %rdi
	vmovdqu64	%zmm7, (%rdi)
	knotw	%k2, %k1
	vpcompressd	%zmm13, (%rcx){%k1}
	kmovw	%k1, %ecx
	popcntq	%rcx, %rcx
	addq	%rcx, %rdx
	vpcompressd	%zmm13, (%rax,%rdx,4){%k2}
	leaq	(%rdx,%r9), %r15
	movq	-120(%rbp), %r9
	subq	$1, %r9
	cmpl	$2, -136(%rbp)
	je	.L1152
	movq	-128(%rbp), %rsi
	movq	%r14, %r8
	movq	%r12, %rcx
	movq	%r15, %rdx
	movq	%r13, %rdi
	movq	%r9, -120(%rbp)
	vzeroupper
	call	_ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	cmpl	$3, -136(%rbp)
	movq	-120(%rbp), %r9
	je	.L1136
.L1061:
	subq	%r15, %rbx
	movq	-128(%rbp), %rsi
	leaq	0(%r13,%r15,4), %rdi
	movq	%r14, %r8
	movq	%rbx, %rdx
	movq	%r12, %rcx
	call	_ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L1136:
	subq	$-128, %rsp
	popq	%rbx
	popq	%r10
	.cfi_remember_state
	.cfi_def_cfa 10, 0
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	leaq	-8(%r10), %rsp
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L1050:
	.cfi_restore_state
	movq	(%r12), %rcx
	leaq	8(%r11), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%r11)
	movl	%r8d, %ecx
	movq	-8(%r12,%rcx), %rsi
	movq	%rsi, -8(%r11,%rcx)
	movq	%r11, %rcx
	movq	%r12, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%r8d, %ecx
	shrl	$3, %ecx
	rep movsq
	jmp	.L1051
	.p2align 4,,10
	.p2align 3
.L1046:
	movq	(%r11), %rcx
	leaq	8(%rax), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%rax)
	movl	%r8d, %ecx
	movq	-8(%r11,%rcx), %rsi
	movq	%rsi, -8(%rax,%rcx)
	movq	%rax, %rcx
	movq	%r11, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%r8d, %ecx
	shrl	$3, %ecx
	rep movsq
	jmp	.L1047
	.p2align 4,,10
	.p2align 3
.L1143:
	movl	(%r12), %ecx
	jmp	.L1032
	.p2align 4,,10
	.p2align 3
.L1145:
	subq	%rsi, %r9
	leaq	0(%r13,%rsi,4), %r11
	leaq	(%r12,%r8), %rsi
.L1064:
	movq	$-1, %rdi
	bzhi	%r9, %rdi, %rdi
	movzwl	%di, %edi
	kmovd	%edi, %k0
.L1045:
	vmovdqu64	(%r11), %zmm1
	vpcmpd	$6, %zmm0, %zmm1, %k1
	kandnw	%k0, %k1, %k2
	vpcompressd	%zmm1, (%rax){%k2}
	kmovw	%k2, %edi
	kandw	%k0, %k1, %k1
	popcntq	%rdi, %rdi
	leaq	(%rax,%rdi,4), %rax
	kmovw	%k1, %r8d
	popcntq	%r8, %r8
	movq	%rax, %r9
	addq	%rcx, %r8
	movq	%rdx, %rcx
	vpcompressd	%zmm1, (%rsi){%k1}
	subq	%r13, %r9
	subq	%r8, %rcx
	salq	$2, %r8
	sarq	$2, %r9
	leaq	0(%r13,%rcx,4), %r11
	subq	%r9, %rdx
	movq	%rdx, %r15
	movq	%rcx, %rdx
	subq	%r9, %rdx
	leaq	(%rax,%r15,4), %rdi
	leaq	(%rax,%rdx,4), %r10
	vmovq	%rdi, %xmm14
	jmp	.L1044
	.p2align 4,,10
	.p2align 3
.L1149:
	movzbl	(%r12), %esi
	movb	%sil, (%r11)
	testb	$2, %cl
	je	.L1051
	movzwl	-2(%r12,%rcx), %esi
	movw	%si, -2(%r11,%rcx)
	jmp	.L1051
	.p2align 4,,10
	.p2align 3
.L1147:
	movzbl	(%r11), %esi
	movb	%sil, (%rax)
	testb	$2, %cl
	je	.L1047
	movzwl	-2(%r11,%rcx), %esi
	movw	%si, -2(%rax,%rcx)
	jmp	.L1047
	.p2align 4,,10
	.p2align 3
.L1040:
	leaq	-16(%r9), %rsi
	leaq	1(%r9), %rdi
	andq	$-16, %rsi
	leaq	0(,%rcx,4), %r8
	addq	$16, %rsi
	cmpq	$16, %rdi
	movl	$16, %edi
	cmovbe	%rdi, %rsi
	cmpq	%r9, %rsi
	je	.L1042
	subq	%rsi, %r9
	leaq	0(%r13,%rsi,4), %r11
	leaq	(%r12,%r8), %rsi
	cmpq	$255, %r9
	jbe	.L1064
	movl	$65535, %edi
	kmovd	%edi, %k0
	jmp	.L1045
	.p2align 4,,10
	.p2align 3
.L1141:
	cmpq	$1, %rdx
	jbe	.L1136
	leaq	1024(%rdi), %rax
	cmpq	%rax, %rsi
	jb	.L1153
	movl	$16, %esi
	call	_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	jmp	.L1136
	.p2align 4,,10
	.p2align 3
.L988:
	vmovdqu32	0(%r13), %zmm6
	movl	$16, %edi
	movq	$-1, %rax
	subq	-144(%rbp), %rdi
	bzhi	%rdi, %rax, %rax
	kmovw	%eax, %k6
	vpcmpd	$4, %zmm0, %zmm6, %k0
	kandw	%k6, %k0, %k0
	kmovw	%k0, %eax
	kortestw	%k0, %k0
	jne	.L1154
	vpxor	%xmm4, %xmm4, %xmm4
	leaq	1024(%r13,%rdi,4), %rsi
	vmovdqa64	%zmm4, %zmm3
	.p2align 4,,10
	.p2align 3
.L994:
	movq	%rdi, %rcx
	addq	$256, %rdi
	cmpq	%rdi, %rbx
	jb	.L998
	leaq	-1024(%rsi), %rax
.L993:
	vpxord	(%rax), %zmm0, %zmm1
	vpxord	64(%rax), %zmm0, %zmm2
	leaq	128(%rax), %rdx
	vpord	%zmm3, %zmm1, %zmm3
	vpord	%zmm4, %zmm2, %zmm4
	vpxord	128(%rax), %zmm0, %zmm1
	vpxord	192(%rax), %zmm0, %zmm2
	vpord	%zmm3, %zmm1, %zmm3
	vpxord	256(%rax), %zmm0, %zmm1
	vpord	%zmm4, %zmm2, %zmm4
	vpxord	320(%rax), %zmm0, %zmm2
	leaq	384(%rdx), %rax
	vpord	%zmm3, %zmm1, %zmm3
	vpxord	256(%rdx), %zmm0, %zmm1
	vpord	%zmm4, %zmm2, %zmm4
	vpxord	320(%rdx), %zmm0, %zmm2
	vpord	%zmm3, %zmm1, %zmm1
	vpord	%zmm4, %zmm2, %zmm2
	vmovdqa64	%zmm1, %zmm3
	vmovdqa64	%zmm2, %zmm4
	cmpq	%rax, %rsi
	jne	.L993
	vpord	%zmm2, %zmm1, %zmm1
	leaq	1408(%rdx), %rsi
	vptestnmd	%zmm1, %zmm1, %k0
	kortestw	%k0, %k0
	setc	%al
	testb	%al, %al
	jne	.L994
	vmovdqa32	0(%r13,%rcx,4), %zmm6
	vpcmpd	$4, %zmm0, %zmm6, %k0
	kortestw	%k0, %k0
	jne	.L996
	.p2align 4,,10
	.p2align 3
.L995:
	addq	$16, %rcx
	vmovdqa32	0(%r13,%rcx,4), %zmm7
	vpcmpd	$4, %zmm0, %zmm7, %k0
	kortestw	%k0, %k0
	je	.L995
.L996:
	kmovw	%k0, %eax
	tzcntl	%eax, %eax
	addq	%rcx, %rax
.L992:
	vpbroadcastd	0(%r13,%rax,4), %zmm5
	vmovdqa64	%zmm0, %zmm2
	leaq	0(%r13,%rax,4), %rdi
	vpcmpd	$6, %zmm0, %zmm5, %k0
	vmovdqa64	%zmm5, %zmm1
	kortestw	%k0, %k0
	jne	.L1001
	leaq	-16(%rbx), %rax
	xorl	%ecx, %ecx
	jmp	.L1007
	.p2align 4,,10
	.p2align 3
.L1002:
	kmovw	%k0, %edx
	popcntq	%rdx, %rdx
	addq	%rdx, %rcx
	leaq	-16(%rax), %rdx
	vmovdqu64	%zmm2, 0(%r13,%rax,4)
	cmpq	%rdx, %rbx
	jbe	.L1155
	movq	%rdx, %rax
.L1007:
	vmovdqu32	0(%r13,%rax,4), %zmm6
	vpcmpd	$0, %zmm5, %zmm6, %k1
	vpcmpd	$0, %zmm0, %zmm6, %k0
	kmovw	%k1, %edx
	kmovw	%k0, %esi
	korw	%k0, %k1, %k1
	kortestw	%k1, %k1
	jc	.L1002
	kmovw	%edx, %k5
	kmovw	%esi, %k3
	kxnorw	%k3, %k5, %k7
	kmovw	%k7, %edx
	tzcntl	%edx, %edx
	leaq	16(%rax), %rsi
	addq	%rax, %rdx
	addq	$32, %rax
	vpbroadcastd	0(%r13,%rdx,4), %zmm2
	movq	%rbx, %rdx
	subq	%rcx, %rdx
	vmovdqa64	%zmm2, %zmm0
	vmovdqa64	%zmm2, -112(%rbp)
	cmpq	%rdx, %rax
	ja	.L1003
	.p2align 4,,10
	.p2align 3
.L1004:
	vmovdqu64	%zmm1, -64(%r13,%rax,4)
	movq	%rax, %rsi
	addq	$16, %rax
	cmpq	%rdx, %rax
	jbe	.L1004
.L1003:
	subq	%rsi, %rdx
	leaq	0(%r13,%rsi,4), %rcx
	movl	$65535, %eax
	cmpq	$255, %rdx
	ja	.L1005
	movq	$-1, %rax
	bzhi	%rdx, %rax, %rax
	movzwl	%ax, %eax
.L1005:
	kmovw	%eax, %k3
	vmovdqu32	%zmm5, (%rcx){%k3}
.L1006:
	vpbroadcastd	(%r12), %zmm3
	vpcmpd	$0, .LC8(%rip), %zmm3, %k0
	vmovdqa64	%zmm3, %zmm1
	kortestw	%k0, %k0
	jc	.L1086
	vpcmpd	$0, .LC9(%rip), %zmm3, %k0
	kortestw	%k0, %k0
	jc	.L1024
	vpminsd	%zmm0, %zmm5, %zmm2
	vpcmpd	$6, %zmm2, %zmm3, %k0
	kortestw	%k0, %k0
	jne	.L1156
	vmovdqa64	%zmm3, %zmm2
	movl	$256, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L1019:
	movq	%rax, %rdx
	addq	$1, %rax
	salq	$4, %rdx
	addq	%rcx, %rdx
	vpminsd	0(%r13,%rdx,4), %zmm2, %zmm0
	vmovdqa64	%zmm0, %zmm2
	cmpq	$16, %rax
	jne	.L1019
	vpcmpd	$6, %zmm0, %zmm3, %k0
	kortestw	%k0, %k0
	jne	.L1140
	leaq	256(%rsi), %rax
	cmpq	%rax, %rbx
	jb	.L1026
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L1019
	.p2align 4,,10
	.p2align 3
.L998:
	movq	%rcx, %rdx
	addq	$16, %rcx
	cmpq	%rcx, %rbx
	jb	.L1157
	vmovdqa32	-64(%r13,%rcx,4), %zmm6
	vpcmpd	$4, %zmm0, %zmm6, %k0
	kmovw	%k0, %eax
	kortestw	%k0, %k0
	je	.L998
.L1138:
	tzcntl	%eax, %eax
	addq	%rdx, %rax
	jmp	.L992
	.p2align 4,,10
	.p2align 3
.L1039:
	movq	%r12, %rsi
	movq	%r13, %r11
	movq	%r13, %rax
	movq	%rdx, %r15
	vmovq	%r10, %xmm14
	testq	%r9, %r9
	jne	.L1064
	jmp	.L1065
.L1152:
	vzeroupper
	jmp	.L1061
.L1091:
	movq	%rax, %rsi
	xorl	%ecx, %ecx
	jmp	.L1055
.L1090:
	movq	%rax, %rcx
	movq	%r15, %rdi
	jmp	.L1054
.L1144:
	leaq	-1(%rbx), %r12
	movq	%r12, %r14
	shrq	%r14
	.p2align 4,,10
	.p2align 3
.L1037:
	movq	%r14, %rdx
	movq	%rbx, %rsi
	movq	%r13, %rdi
	call	_ZN3hwy6N_AVX36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	subq	$1, %r14
	jnb	.L1037
	.p2align 4,,10
	.p2align 3
.L1038:
	movl	0(%r13,%r12,4), %edx
	movl	0(%r13), %eax
	movq	%r12, %rsi
	movq	%r13, %rdi
	movl	%edx, 0(%r13)
	xorl	%edx, %edx
	movl	%eax, 0(%r13,%r12,4)
	call	_ZN3hwy6N_AVX36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	subq	$1, %r12
	jne	.L1038
	jmp	.L1136
	.p2align 4,,10
	.p2align 3
.L1027:
	vpcmpd	$6, -64(%r13,%rsi,4), %zmm3, %k0
	kortestw	%k0, %k0
	jne	.L1140
.L1026:
	movq	%rsi, %rax
	addq	$16, %rsi
	cmpq	%rsi, %rbx
	jnb	.L1027
	cmpq	%rax, %rbx
	je	.L1086
	vpcmpd	$6, -64(%r13,%rbx,4), %zmm3, %k0
	xorl	%eax, %eax
	kortestw	%k0, %k0
	sete	%al
	addl	$1, %eax
	movl	%eax, -136(%rbp)
	jmp	.L1028
.L1146:
	movl	(%r11), %esi
	movl	%esi, (%rax)
	movl	-4(%r11,%rcx), %esi
	movl	%esi, -4(%rax,%rcx)
	jmp	.L1047
.L1148:
	movl	(%r12), %esi
	movl	%esi, (%r11)
	movl	-4(%r12,%rcx), %esi
	movl	%esi, -4(%r11,%rcx)
	jmp	.L1051
.L1001:
	movq	%rbx, %rsi
	leaq	-112(%rbp), %rdx
	movq	%r12, %rcx
	subq	%rax, %rsi
	call	_ZN3hwy6N_AVX36detail22MaybePartitionTwoValueINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L1136
	vmovdqa64	-112(%rbp), %zmm0
	jmp	.L1006
.L1155:
	movl	$65535, %edi
	vmovdqu64	0(%r13), %zmm2
	kmovd	%edi, %k2
	cmpq	$255, %rax
	ja	.L1008
	movq	$-1, %rdx
	bzhi	%rax, %rdx, %rdx
	movzwl	%dx, %edi
	kmovd	%edi, %k2
.L1008:
	vpcmpd	$0, %zmm5, %zmm2, %k0
	vpcmpd	$0, %zmm0, %zmm2, %k1
	movq	%rbx, %rdx
	kandw	%k2, %k1, %k1
	knotw	%k2, %k2
	korw	%k1, %k0, %k0
	korw	%k2, %k0, %k0
	kortestw	%k0, %k0
	setc	%sil
	subq	%rcx, %rdx
	testb	%sil, %sil
	je	.L1158
	xorl	%ecx, %ecx
	kmovw	%k1, %eax
	vmovdqu64	%zmm0, 0(%r13)
	popcntq	%rax, %rcx
	movq	%rdx, %rax
	subq	%rcx, %rax
	cmpq	$15, %rax
	jbe	.L1013
	leaq	-16(%rax), %rcx
	movq	-152(%rbp), %rsi
	movq	%rcx, %rdx
	shrq	$4, %rdx
	salq	$6, %rdx
	leaq	64(%r13,%rdx), %rdx
	.p2align 4,,10
	.p2align 3
.L1014:
	vmovdqu64	%zmm1, (%rsi)
	addq	$64, %rsi
	cmpq	%rdx, %rsi
	jne	.L1014
	andq	$-16, %rcx
	movl	$65535, %ebx
	vmovdqa64	%zmm5, (%r12)
	leaq	16(%rcx), %rdx
	kmovd	%ebx, %k1
	subq	%rdx, %rax
	leaq	0(%r13,%rdx,4), %r13
	cmpq	$255, %rax
	jbe	.L1066
.L1015:
	vmovdqu32	(%r12), %zmm0{%k1}{z}
	vmovdqu32	%zmm0, 0(%r13){%k1}
	vzeroupper
	jmp	.L1136
.L1023:
	vmovdqu32	-64(%r13,%rsi,4), %zmm7
	vpcmpd	$6, %zmm3, %zmm7, %k0
	kortestw	%k0, %k0
	jne	.L1140
.L1022:
	movq	%rsi, %rax
	addq	$16, %rsi
	cmpq	%rsi, %rbx
	jnb	.L1023
	cmpq	%rax, %rbx
	je	.L1024
	vmovdqu32	-64(%r13,%rbx,4), %zmm7
	vpcmpd	$6, %zmm3, %zmm7, %k0
	kortestw	%k0, %k0
	jne	.L1140
.L1024:
	movl	$3, -136(%rbp)
	vpternlogd	$0xFF, %zmm1, %zmm1, %zmm1
	vpaddd	%zmm1, %zmm3, %zmm1
	jmp	.L1028
.L1157:
	leaq	-16(%rbx), %rdx
	vmovdqu32	0(%r13,%rdx,4), %zmm7
	vpcmpd	$4, %zmm0, %zmm7, %k0
	kmovw	%k0, %eax
	kortestw	%k0, %k0
	jne	.L1138
	vzeroupper
	jmp	.L1136
.L1154:
	tzcntl	%eax, %eax
	jmp	.L992
.L1153:
	xorl	%eax, %eax
	cmpq	$15, %rdx
	jbe	.L980
	leaq	-16(%rdx), %rdx
	movq	(%rdi), %rcx
	movq	%rdx, %rax
	shrq	$4, %rax
	movq	%rcx, (%r12)
	addq	$1, %rax
	salq	$6, %rax
	movl	%eax, %ecx
	movq	-8(%rdi,%rcx), %rsi
	leaq	8(%r12), %rdi
	andq	$-8, %rdi
	movq	%rsi, -8(%r12,%rcx)
	movq	%r12, %rcx
	movq	%r13, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%eax, %ecx
	movq	%rdx, %rax
	shrl	$3, %ecx
	andq	$-16, %rax
	rep movsq
	addq	$16, %rax
.L980:
	movq	%rbx, %rdx
	leaq	0(,%rax,4), %r14
	subq	%rax, %rdx
	movl	$65535, %eax
	leaq	(%r12,%r14), %r15
	addq	%r13, %r14
	kmovd	%eax, %k4
	cmpq	$255, %rdx
	ja	.L981
	movq	$-1, %rax
	bzhi	%rdx, %rax, %rax
	movzwl	%ax, %eax
	kmovd	%eax, %k4
.L981:
	leal	-1(%rbx), %eax
	movl	$32, %edx
	movl	$1, %esi
	vmovdqu32	(%r14), %zmm0{%k4}{z}
	bsrl	%eax, %eax
	xorl	$31, %eax
	vmovdqu32	%zmm0, (%r15){%k4}
	vpbroadcastq	.LC10(%rip), %zmm0
	subl	%eax, %edx
	movl	$1, %eax
	shlx	%rdx, %rsi, %rsi
	shrq	$4, %rsi
	cmove	%rax, %rsi
	movq	%rbx, %rax
	leaq	1(%rsi), %rdx
	salq	$4, %rdx
	cmpq	%rdx, %rbx
	jnb	.L985
.L982:
	vmovdqu64	%zmm0, (%r12,%rax,4)
	addq	$16, %rax
	cmpq	%rdx, %rax
	jb	.L982
.L985:
	movq	%r12, %rdi
	vzeroupper
	call	_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	cmpq	$15, %rbx
	jbe	.L984
	leaq	-16(%rbx), %rax
	movq	(%r12), %rdx
	leaq	8(%r13), %rdi
	movq	%r12, %rsi
	shrq	$4, %rax
	andq	$-8, %rdi
	addq	$1, %rax
	movq	%rdx, 0(%r13)
	salq	$6, %rax
	movl	%eax, %edx
	movq	-8(%r12,%rdx), %rcx
	movq	%rcx, -8(%r13,%rdx)
	subq	%rdi, %r13
	leal	(%rax,%r13), %ecx
	subq	%r13, %rsi
	shrl	$3, %ecx
	rep movsq
.L984:
	vmovdqu32	(%r15), %zmm0{%k4}{z}
	vmovdqu32	%zmm0, (%r14){%k4}
	vzeroupper
	jmp	.L1136
.L1013:
	vmovdqa64	%zmm5, (%r12)
.L1066:
	movq	$-1, %rdx
	bzhi	%rax, %rdx, %rax
	movzwl	%ax, %eax
	kmovd	%eax, %k1
	jmp	.L1015
.L1086:
	movl	$2, -136(%rbp)
	jmp	.L1028
.L1156:
	vpmaxsd	%zmm0, %zmm5, %zmm5
	vpcmpd	$6, %zmm3, %zmm5, %k0
	kortestw	%k0, %k0
	jne	.L1140
	vmovdqa64	%zmm3, %zmm2
	movl	$256, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L1020:
	movq	%rax, %rdx
	addq	$1, %rax
	salq	$4, %rdx
	addq	%rcx, %rdx
	vpmaxsd	0(%r13,%rdx,4), %zmm2, %zmm0
	vmovdqa64	%zmm0, %zmm2
	cmpq	$16, %rax
	jne	.L1020
	vpcmpd	$6, %zmm3, %zmm0, %k0
	kortestw	%k0, %k0
	jne	.L1140
	leaq	256(%rsi), %rax
	cmpq	%rax, %rbx
	jb	.L1022
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L1020
.L1158:
	knotw	%k0, %k7
	kmovw	%k7, %ecx
	tzcntl	%ecx, %ecx
	vpbroadcastd	0(%r13,%rcx,4), %zmm2
	leaq	16(%rax), %rcx
	vmovdqa64	%zmm2, %zmm0
	vmovdqa64	%zmm2, -112(%rbp)
	cmpq	%rdx, %rcx
	ja	.L1010
.L1011:
	vmovdqu64	%zmm1, -64(%r13,%rcx,4)
	movq	%rcx, %rax
	addq	$16, %rcx
	cmpq	%rdx, %rcx
	jbe	.L1011
.L1010:
	subq	%rax, %rdx
	leaq	0(%r13,%rax,4), %rsi
	movl	$-1, %ecx
	cmpq	$255, %rdx
	ja	.L1012
	orq	$-1, %rcx
	bzhi	%rdx, %rcx, %rcx
.L1012:
	kmovw	%ecx, %k5
	vmovdqu32	%zmm5, (%rsi){%k5}
	jmp	.L1006
	.cfi_endproc
.LFE18802:
	.size	_ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0, .-_ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	.section	.text._ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0, @function
_ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0:
.LFB18804:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	movq	%rdi, %r13
	pushq	%r12
	.cfi_offset 12, -48
	movq	%rcx, %r12
	pushq	%rbx
	subq	$88, %rsp
	.cfi_offset 3, -56
	movq	%rsi, -96(%rbp)
	movq	%rdx, -72(%rbp)
	movq	%r8, -80(%rbp)
	movq	%r9, -88(%rbp)
	cmpq	$64, %rdx
	jbe	.L1449
	movq	%rdi, %rax
	movq	%rdi, -120(%rbp)
	shrq	$2, %rax
	movq	%rax, %rdx
	movq	%rax, -112(%rbp)
	andl	$15, %edx
	jne	.L1450
	movq	-72(%rbp), %r14
	movq	%rdi, %rax
	movq	%r8, %r15
.L1172:
	movq	8(%r15), %rcx
	movq	16(%r15), %r10
	movq	%rcx, %rsi
	leaq	1(%r10), %r8
	movq	%rcx, %rdi
	xorq	(%r15), %r8
	rolq	$24, %rsi
	shrq	$11, %rdi
	leaq	(%rcx,%rcx,8), %rdx
	leaq	2(%r10), %rcx
	addq	%r8, %rsi
	xorq	%rdi, %rdx
	movq	%rsi, %rdi
	xorq	%rcx, %rdx
	leaq	(%rsi,%rsi,8), %rcx
	movq	%rsi, %r9
	rolq	$24, %rdi
	shrq	$11, %r9
	leaq	3(%r10), %rsi
	addq	%rdx, %rdi
	xorq	%r9, %rcx
	movq	%rdi, %r11
	xorq	%rsi, %rcx
	leaq	(%rdi,%rdi,8), %rsi
	movq	%rdi, %r9
	rolq	$24, %r11
	shrq	$11, %r9
	leaq	4(%r10), %rdi
	addq	$5, %r10
	addq	%rcx, %r11
	xorq	%r9, %rsi
	movq	%r10, 16(%r15)
	movq	%r11, %r9
	xorq	%rdi, %rsi
	leaq	(%r11,%r11,8), %rdi
	movq	%r11, %rbx
	rolq	$24, %r9
	shrq	$11, %rbx
	addq	%rsi, %r9
	xorq	%rbx, %rdi
	movq	%r9, %rbx
	leaq	(%r9,%r9,8), %r11
	xorq	%r10, %rdi
	rolq	$24, %r9
	shrq	$11, %rbx
	addq	%rdi, %r9
	movl	%r8d, %r10d
	movl	%edi, %edi
	xorq	%rbx, %r11
	movq	%r14, %rbx
	movq	%r11, %xmm0
	shrq	$4, %rbx
	movl	%ecx, %r11d
	pinsrq	$1, %r9, %xmm0
	movabsq	$68719476719, %r9
	cmpq	%r9, %r14
	movl	$4294967295, %r9d
	movups	%xmm0, (%r15)
	movl	%edx, %r15d
	cmova	%r9, %rbx
	shrq	$32, %r8
	movl	%esi, %r9d
	shrq	$32, %rdx
	imulq	%rbx, %r15
	shrq	$32, %rcx
	shrq	$32, %rsi
	imulq	%rbx, %r10
	imulq	%rbx, %r8
	shrq	$32, %r15
	imulq	%rbx, %rdx
	imulq	%rbx, %r11
	shrq	$32, %r10
	imulq	%rbx, %rcx
	shrq	$32, %r8
	imulq	%rbx, %r9
	shrq	$32, %rdx
	imulq	%rbx, %rsi
	shrq	$32, %r11
	imulq	%rbx, %rdi
	movq	%r15, %rbx
	shrq	$32, %rcx
	salq	$6, %rbx
	shrq	$32, %r9
	movdqa	(%rax,%rbx), %xmm1
	movq	%r10, %rbx
	shrq	$32, %rsi
	salq	$6, %rbx
	shrq	$32, %rdi
	movdqa	(%rax,%rbx), %xmm0
	movq	%r8, %rbx
	salq	$4, %r10
	salq	$6, %rbx
	movdqa	%xmm0, %xmm4
	pmaxsd	%xmm1, %xmm0
	pminsd	%xmm1, %xmm4
	pmaxsd	(%rax,%rbx), %xmm4
	movq	%rcx, %rbx
	salq	$6, %rbx
	movdqa	(%rax,%rbx), %xmm1
	movq	%rdx, %rbx
	pminsd	%xmm0, %xmm4
	salq	$6, %rbx
	movaps	%xmm4, (%r12)
	movdqa	(%rax,%rbx), %xmm0
	movq	%r11, %rbx
	salq	$6, %rbx
	movdqa	%xmm0, %xmm12
	pmaxsd	%xmm1, %xmm0
	pminsd	%xmm1, %xmm12
	pmaxsd	(%rax,%rbx), %xmm12
	movq	%rdi, %rbx
	salq	$6, %rbx
	movdqa	(%rax,%rbx), %xmm1
	movq	%r9, %rbx
	pminsd	%xmm0, %xmm12
	salq	$6, %rbx
	movaps	%xmm12, 64(%r12)
	movdqa	(%rax,%rbx), %xmm0
	movq	%rsi, %rbx
	salq	$6, %rbx
	addq	$4, %r10
	salq	$4, %r15
	movdqa	%xmm0, %xmm8
	addq	$4, %r15
	pmaxsd	%xmm1, %xmm0
	salq	$4, %r8
	pminsd	%xmm1, %xmm8
	movdqa	(%rax,%r10,4), %xmm1
	salq	$4, %rdx
	salq	$4, %rcx
	pmaxsd	(%rax,%rbx), %xmm8
	addq	$4, %rdx
	addq	$4, %rcx
	salq	$4, %r11
	movdqa	(%rax,%r15,4), %xmm2
	salq	$4, %r9
	salq	$4, %rdi
	leaq	0(,%r15,4), %r14
	pminsd	%xmm0, %xmm8
	movdqa	%xmm1, %xmm0
	addq	$4, %r9
	addq	$4, %rdi
	pminsd	%xmm2, %xmm0
	pmaxsd	16(%rax,%r8,4), %xmm0
	salq	$4, %rsi
	movaps	%xmm8, 128(%r12)
	pmaxsd	%xmm2, %xmm1
	movdqa	(%rax,%rcx,4), %xmm2
	leaq	0(,%r10,4), %rbx
	pminsd	%xmm1, %xmm0
	movdqa	(%rax,%rdx,4), %xmm1
	movdqa	16(%rax,%rbx), %xmm14
	leaq	0(,%rdx,4), %r10
	pminsd	16(%rax,%r14), %xmm14
	movaps	%xmm0, 16(%r12)
	leaq	0(,%rcx,4), %r15
	movdqa	%xmm1, %xmm11
	pmaxsd	%xmm2, %xmm1
	leaq	0(,%r9,4), %rdx
	pminsd	%xmm2, %xmm11
	pmaxsd	16(%rax,%r11,4), %xmm11
	movdqa	(%rax,%rdi,4), %xmm2
	leaq	0(,%rdi,4), %rcx
	pminsd	%xmm1, %xmm11
	movdqa	(%rax,%r9,4), %xmm1
	movaps	%xmm11, 80(%r12)
	movdqa	%xmm1, %xmm7
	pmaxsd	%xmm2, %xmm1
	pminsd	%xmm2, %xmm7
	pmaxsd	16(%rax,%rsi,4), %xmm7
	pminsd	%xmm1, %xmm7
	movdqa	16(%rax,%rbx), %xmm1
	movaps	%xmm7, 144(%r12)
	pmaxsd	16(%rax,%r14), %xmm1
	movdqa	16(%rax,%r10), %xmm10
	pmaxsd	32(%rax,%r8,4), %xmm14
	pminsd	16(%rax,%r15), %xmm10
	pmaxsd	32(%rax,%r11,4), %xmm10
	movdqa	16(%rax,%rdx), %xmm6
	pminsd	%xmm1, %xmm14
	movdqa	16(%rax,%r10), %xmm1
	pmaxsd	16(%rax,%r15), %xmm1
	pminsd	16(%rax,%rcx), %xmm6
	pmaxsd	32(%rax,%rsi,4), %xmm6
	movaps	%xmm14, 32(%r12)
	pminsd	%xmm1, %xmm10
	movdqa	16(%rax,%rdx), %xmm1
	pmaxsd	16(%rax,%rcx), %xmm1
	movdqa	32(%rax,%r14), %xmm2
	movaps	%xmm10, 96(%r12)
	leaq	192(%r12), %r14
	pminsd	%xmm1, %xmm6
	movdqa	32(%rax,%rbx), %xmm1
	movaps	%xmm6, 160(%r12)
	movdqa	%xmm1, %xmm13
	pmaxsd	%xmm2, %xmm1
	pminsd	%xmm2, %xmm13
	pmaxsd	48(%rax,%r8,4), %xmm13
	movdqa	32(%rax,%r15), %xmm2
	pminsd	%xmm1, %xmm13
	movdqa	32(%rax,%r10), %xmm1
	movaps	%xmm13, 48(%r12)
	movdqa	%xmm1, %xmm9
	pmaxsd	%xmm2, %xmm1
	pminsd	%xmm2, %xmm9
	pmaxsd	48(%rax,%r11,4), %xmm9
	movdqa	32(%rax,%rcx), %xmm2
	pminsd	%xmm1, %xmm9
	movdqa	32(%rax,%rdx), %xmm1
	movaps	%xmm9, 112(%r12)
	movdqa	%xmm1, %xmm5
	pmaxsd	%xmm2, %xmm1
	pminsd	%xmm2, %xmm5
	pmaxsd	48(%rax,%rsi,4), %xmm5
	pshufd	$0, %xmm4, %xmm2
	pxor	%xmm2, %xmm4
	pxor	%xmm2, %xmm14
	pxor	%xmm2, %xmm13
	pminsd	%xmm1, %xmm5
	movdqa	%xmm0, %xmm1
	pxor	%xmm2, %xmm12
	pxor	%xmm2, %xmm1
	pxor	%xmm2, %xmm11
	pxor	%xmm2, %xmm10
	movaps	%xmm5, 176(%r12)
	por	%xmm4, %xmm1
	pxor	%xmm2, %xmm9
	pxor	%xmm2, %xmm8
	movdqa	192(%r12), %xmm4
	por	%xmm14, %xmm1
	pxor	%xmm2, %xmm7
	pxor	%xmm2, %xmm6
	por	%xmm13, %xmm1
	pxor	%xmm2, %xmm5
	pxor	%xmm2, %xmm4
	por	%xmm12, %xmm1
	pxor	%xmm0, %xmm0
	movdqa	%xmm2, %xmm3
	por	%xmm11, %xmm1
	por	%xmm10, %xmm1
	por	%xmm9, %xmm1
	por	%xmm8, %xmm1
	por	%xmm7, %xmm1
	por	%xmm6, %xmm1
	por	%xmm5, %xmm1
	por	%xmm1, %xmm4
	pblendvb	%xmm0, %xmm4, %xmm1
	pxor	%xmm0, %xmm0
	pcmpeqd	%xmm0, %xmm1
	movmskps	%xmm1, %eax
	cmpl	$15, %eax
	je	.L1174
	movdqa	.LC4(%rip), %xmm0
	movl	$4, %esi
	movq	%r12, %rdi
	movups	%xmm0, 192(%r12)
	movups	%xmm0, 208(%r12)
	movups	%xmm0, 224(%r12)
	movups	%xmm0, 240(%r12)
	movups	%xmm0, 256(%r12)
	call	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	movd	(%r12), %xmm6
	pcmpeqd	%xmm2, %xmm2
	movd	188(%r12), %xmm5
	pshufd	$0, %xmm5, %xmm1
	pshufd	$0, %xmm6, %xmm0
	paddd	%xmm1, %xmm2
	pcmpeqd	%xmm0, %xmm2
	movmskps	%xmm2, %eax
	cmpl	$15, %eax
	jne	.L1176
	movq	-72(%rbp), %rsi
	leaq	-64(%rbp), %rdx
	movq	%r14, %rcx
	movq	%r13, %rdi
	call	_ZN3hwy6N_SSE46detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L1159
.L1176:
	movl	96(%r12), %ecx
	movl	$23, %eax
	movl	$24, %edx
	cmpl	%ecx, 92(%r12)
	je	.L1218
	jmp	.L1223
	.p2align 4,,10
	.p2align 3
.L1221:
	testq	%rax, %rax
	je	.L1451
.L1218:
	movq	%rax, %rdx
	subq	$1, %rax
	movl	(%r12,%rax,4), %esi
	cmpl	%esi, %ecx
	je	.L1221
	cmpl	(%r12,%rdx,4), %ecx
	je	.L1223
	movl	%esi, %ecx
	jmp	.L1220
	.p2align 4,,10
	.p2align 3
.L1224:
	cmpq	$47, %rdx
	je	.L1442
.L1223:
	movq	%rdx, %rsi
	addq	$1, %rdx
	cmpl	(%r12,%rdx,4), %ecx
	je	.L1224
	movl	$24, %edx
	subq	$23, %rsi
	subq	%rax, %rdx
	cmpq	%rdx, %rsi
	jb	.L1220
.L1442:
	movl	(%r12,%rax,4), %ecx
.L1220:
	movd	%ecx, %xmm6
	pshufd	$0, %xmm6, %xmm1
.L1443:
	movl	$1, -112(%rbp)
.L1217:
	cmpq	$0, -88(%rbp)
	je	.L1452
	movq	-72(%rbp), %rax
	movdqa	%xmm1, %xmm4
	leaq	-4(%rax), %r10
	movq	%r10, %rdx
	movq	%r10, %rcx
	movdqu	0(%r13,%r10,4), %xmm6
	andl	$15, %edx
	andl	$12, %ecx
	je	.L1290
	movdqu	0(%r13), %xmm2
	pcmpeqd	%xmm0, %xmm0
	xorl	%edi, %edi
	movdqa	.LC0(%rip), %xmm5
	leaq	_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %r9
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm1, %xmm3
	movdqa	%xmm2, %xmm1
	pxor	%xmm3, %xmm0
	movmskps	%xmm0, %eax
	popcntq	%rax, %rdi
	movq	%rdi, %xmm0
	salq	$4, %rax
	movq	%rdi, %rcx
	pshufd	$0, %xmm0, %xmm0
	pshufb	(%r9,%rax), %xmm1
	pcmpgtd	%xmm5, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1228
	movd	%xmm1, 0(%r13)
.L1228:
	pextrd	$1, %xmm0, %eax
	testl	%eax, %eax
	je	.L1229
	pextrd	$1, %xmm1, 4(%r13)
.L1229:
	pextrd	$2, %xmm0, %eax
	testl	%eax, %eax
	je	.L1230
	pextrd	$2, %xmm1, 8(%r13)
.L1230:
	pextrd	$3, %xmm0, %eax
	testl	%eax, %eax
	je	.L1231
	pextrd	$3, %xmm1, 12(%r13)
.L1231:
	leaq	0(%r13,%rcx,4), %rax
	xorl	%esi, %esi
	movmskps	%xmm3, %ecx
	popcntq	%rcx, %rsi
	salq	$4, %rcx
	pshufb	(%r9,%rcx), %xmm2
	movups	%xmm2, (%r12)
	testb	$8, %r10b
	je	.L1232
	movdqu	16(%r13), %xmm1
	pcmpeqd	%xmm0, %xmm0
	xorl	%edi, %edi
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pcmpgtd	%xmm4, %xmm2
	pxor	%xmm2, %xmm0
	movmskps	%xmm0, %ecx
	popcntq	%rcx, %rdi
	movq	%rdi, %xmm0
	salq	$4, %rcx
	pshufd	$0, %xmm0, %xmm0
	pshufb	(%r9,%rcx), %xmm3
	pcmpgtd	%xmm5, %xmm0
	movd	%xmm0, %ecx
	testl	%ecx, %ecx
	je	.L1233
	movd	%xmm3, (%rax)
.L1233:
	pextrd	$1, %xmm0, %ecx
	testl	%ecx, %ecx
	je	.L1234
	pextrd	$1, %xmm3, 4(%rax)
.L1234:
	pextrd	$2, %xmm0, %ecx
	testl	%ecx, %ecx
	je	.L1235
	pextrd	$2, %xmm3, 8(%rax)
.L1235:
	pextrd	$3, %xmm0, %ecx
	testl	%ecx, %ecx
	je	.L1236
	pextrd	$3, %xmm3, 12(%rax)
.L1236:
	movmskps	%xmm2, %ecx
	leaq	(%rax,%rdi,4), %rax
	movq	%rcx, %rdi
	popcntq	%rcx, %rcx
	salq	$4, %rdi
	pshufb	(%r9,%rdi), %xmm1
	movups	%xmm1, (%r12,%rsi,4)
	addq	%rcx, %rsi
	cmpq	$11, %rdx
	jbe	.L1232
	movdqu	32(%r13), %xmm1
	pcmpeqd	%xmm0, %xmm0
	xorl	%edi, %edi
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm3
	pcmpgtd	%xmm4, %xmm2
	pxor	%xmm2, %xmm0
	movmskps	%xmm0, %ecx
	popcntq	%rcx, %rdi
	movq	%rdi, %xmm0
	salq	$4, %rcx
	pshufd	$0, %xmm0, %xmm0
	pshufb	(%r9,%rcx), %xmm3
	pcmpgtd	%xmm5, %xmm0
	movd	%xmm0, %ecx
	testl	%ecx, %ecx
	je	.L1237
	movd	%xmm3, (%rax)
.L1237:
	pextrd	$1, %xmm0, %ecx
	testl	%ecx, %ecx
	je	.L1238
	pextrd	$1, %xmm3, 4(%rax)
.L1238:
	pextrd	$2, %xmm0, %ecx
	testl	%ecx, %ecx
	je	.L1239
	pextrd	$2, %xmm3, 8(%rax)
.L1239:
	pextrd	$3, %xmm0, %ecx
	testl	%ecx, %ecx
	je	.L1240
	pextrd	$3, %xmm3, 12(%rax)
.L1240:
	movmskps	%xmm2, %ecx
	leaq	(%rax,%rdi,4), %rax
	movq	%rcx, %rdi
	popcntq	%rcx, %rcx
	salq	$4, %rdi
	pshufb	(%r9,%rdi), %xmm1
	movups	%xmm1, (%r12,%rsi,4)
	addq	%rcx, %rsi
.L1232:
	leaq	-4(%rdx), %rcx
	leaq	1(%rdx), %rdi
	andq	$-4, %rcx
	leaq	0(,%rsi,4), %r8
	addq	$4, %rcx
	cmpq	$4, %rdi
	movl	$4, %edi
	cmovbe	%rdi, %rcx
.L1227:
	cmpq	%rdx, %rcx
	je	.L1241
	movdqu	0(%r13,%rcx,4), %xmm2
	subq	%rcx, %rdx
	xorl	%edi, %edi
	movd	%edx, %xmm7
	movdqa	%xmm2, %xmm3
	pshufd	$0, %xmm7, %xmm0
	movdqa	%xmm2, %xmm7
	pcmpgtd	%xmm4, %xmm3
	pcmpgtd	%xmm5, %xmm0
	movdqa	%xmm3, %xmm1
	pandn	%xmm0, %xmm1
	movmskps	%xmm1, %edx
	popcntq	%rdx, %rdi
	movq	%rdi, %xmm1
	salq	$4, %rdx
	movq	%rdi, %rcx
	pshufd	$0, %xmm1, %xmm1
	pshufb	(%r9,%rdx), %xmm7
	pcmpgtd	%xmm5, %xmm1
	movd	%xmm1, %edx
	testl	%edx, %edx
	je	.L1242
	movd	%xmm7, (%rax)
.L1242:
	pextrd	$1, %xmm1, %edx
	testl	%edx, %edx
	je	.L1243
	pextrd	$1, %xmm7, 4(%rax)
.L1243:
	pextrd	$2, %xmm1, %edx
	testl	%edx, %edx
	je	.L1244
	pextrd	$2, %xmm7, 8(%rax)
.L1244:
	pextrd	$3, %xmm1, %edx
	testl	%edx, %edx
	je	.L1245
	pextrd	$3, %xmm7, 12(%rax)
.L1245:
	pand	%xmm0, %xmm3
	leaq	(%rax,%rcx,4), %rax
	movmskps	%xmm3, %edx
	movq	%rdx, %rcx
	popcntq	%rdx, %rdx
	addq	%rdx, %rsi
	salq	$4, %rcx
	pshufb	(%r9,%rcx), %xmm2
	movups	%xmm2, (%r12,%r8)
	leaq	0(,%rsi,4), %r8
.L1241:
	movq	%r10, %rdx
	movl	%r8d, %ecx
	subq	%rsi, %rdx
	leaq	0(%r13,%rdx,4), %r11
	cmpl	$8, %r8d
	jnb	.L1246
	testb	$4, %r8b
	jne	.L1453
	testl	%ecx, %ecx
	jne	.L1454
.L1247:
	movl	%r8d, %ecx
	cmpl	$8, %r8d
	jnb	.L1250
	andl	$4, %r8d
	jne	.L1455
	testl	%ecx, %ecx
	jne	.L1456
.L1251:
	movq	%rax, %rcx
	movq	%r10, %r14
	subq	%r13, %rcx
	sarq	$2, %rcx
	subq	%rcx, %r14
	subq	%rcx, %rdx
	movq	%rcx, %r15
	leaq	(%rax,%rdx,4), %rcx
	je	.L1291
	leaq	64(%rax), %rsi
	leaq	-64(%rcx), %r10
	movdqu	(%rax), %xmm14
	movdqu	16(%rax), %xmm13
	movdqu	32(%rax), %xmm12
	movdqu	48(%rax), %xmm11
	movdqu	-64(%rcx), %xmm10
	movdqu	-48(%rcx), %xmm9
	movdqu	-32(%rcx), %xmm8
	movdqu	-16(%rcx), %xmm7
	cmpq	%r10, %rsi
	je	.L1292
	xorl	%ecx, %ecx
	leaq	_ZZN3hwy6N_SSE46detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %rdi
	movl	$4, %r11d
	jmp	.L1258
	.p2align 4,,10
	.p2align 3
.L1458:
	movdqu	-64(%r10), %xmm3
	movdqu	-48(%r10), %xmm2
	prefetcht0	-256(%r10)
	subq	$64, %r10
	movdqu	32(%r10), %xmm1
	movdqu	48(%r10), %xmm0
.L1257:
	movdqa	%xmm3, %xmm15
	pcmpgtd	%xmm4, %xmm15
	movmskps	%xmm15, %r8d
	movq	%r8, %rbx
	popcntq	%r8, %r8
	salq	$4, %rbx
	pshufb	(%rdi,%rbx), %xmm3
	leaq	-4(%rcx,%rdx), %rbx
	movups	%xmm3, (%rax,%rcx,4)
	addq	$4, %rcx
	movups	%xmm3, (%rax,%rbx,4)
	movdqa	%xmm2, %xmm3
	subq	%r8, %rcx
	pcmpgtd	%xmm4, %xmm3
	movmskps	%xmm3, %r8d
	movq	%r8, %rbx
	popcntq	%r8, %r8
	salq	$4, %rbx
	pshufb	(%rdi,%rbx), %xmm2
	leaq	-8(%rcx,%rdx), %rbx
	movups	%xmm2, (%rax,%rcx,4)
	movups	%xmm2, (%rax,%rbx,4)
	movdqa	%xmm1, %xmm2
	movq	%r11, %rbx
	pcmpgtd	%xmm4, %xmm2
	subq	%r8, %rbx
	addq	%rbx, %rcx
	movmskps	%xmm2, %r8d
	movq	%r8, %rbx
	popcntq	%r8, %r8
	salq	$4, %rbx
	pshufb	(%rdi,%rbx), %xmm1
	leaq	-12(%rcx,%rdx), %rbx
	subq	$16, %rdx
	movups	%xmm1, (%rax,%rcx,4)
	movups	%xmm1, (%rax,%rbx,4)
	movdqa	%xmm0, %xmm1
	movq	%r11, %rbx
	pcmpgtd	%xmm4, %xmm1
	subq	%r8, %rbx
	leaq	(%rbx,%rcx), %r8
	movmskps	%xmm1, %ecx
	movq	%rcx, %rbx
	popcntq	%rcx, %rcx
	salq	$4, %rbx
	pshufb	(%rdi,%rbx), %xmm0
	leaq	(%r8,%rdx), %rbx
	movups	%xmm0, (%rax,%r8,4)
	movups	%xmm0, (%rax,%rbx,4)
	movq	%r11, %rbx
	subq	%rcx, %rbx
	leaq	(%rbx,%r8), %rcx
	cmpq	%r10, %rsi
	je	.L1457
.L1258:
	movq	%rsi, %r8
	subq	%rax, %r8
	sarq	$2, %r8
	subq	%rcx, %r8
	cmpq	$16, %r8
	ja	.L1458
	movdqu	(%rsi), %xmm3
	movdqu	16(%rsi), %xmm2
	prefetcht0	256(%rsi)
	addq	$64, %rsi
	movdqu	-32(%rsi), %xmm1
	movdqu	-16(%rsi), %xmm0
	jmp	.L1257
	.p2align 4,,10
	.p2align 3
.L1450:
	movl	$16, %eax
	movq	%r8, %r15
	subq	%rdx, %rax
	leaq	(%rdi,%rax,4), %rax
	movq	-72(%rbp), %rdi
	leaq	-16(%rdx,%rdi), %r14
	jmp	.L1172
	.p2align 4,,10
	.p2align 3
.L1457:
	leaq	(%rax,%rcx,4), %r10
	leaq	(%rdx,%rcx), %r8
	addq	$4, %rcx
.L1255:
	movdqa	%xmm14, %xmm0
	pcmpgtd	%xmm4, %xmm0
	movmskps	%xmm0, %esi
	movdqa	%xmm13, %xmm0
	pcmpgtd	%xmm4, %xmm0
	movq	%rsi, %r11
	popcntq	%rsi, %rsi
	subq	%rsi, %rcx
	salq	$4, %r11
	pshufb	(%rdi,%r11), %xmm14
	movmskps	%xmm0, %esi
	movdqa	%xmm12, %xmm0
	movups	%xmm14, (%r10)
	pcmpgtd	%xmm4, %xmm0
	movups	%xmm14, -16(%rax,%r8,4)
	movq	%rsi, %r8
	popcntq	%rsi, %rsi
	salq	$4, %r8
	pshufb	(%rdi,%r8), %xmm13
	leaq	-8(%rdx,%rcx), %r8
	movups	%xmm13, (%rax,%rcx,4)
	subq	%rsi, %rcx
	movups	%xmm13, (%rax,%r8,4)
	movmskps	%xmm0, %r8d
	addq	$4, %rcx
	movdqa	%xmm11, %xmm0
	movq	%r8, %rsi
	pcmpgtd	%xmm4, %xmm0
	popcntq	%r8, %r8
	salq	$4, %rsi
	pshufb	(%rdi,%rsi), %xmm12
	leaq	-12(%rdx,%rcx), %rsi
	movups	%xmm12, (%rax,%rcx,4)
	movups	%xmm12, (%rax,%rsi,4)
	movl	$4, %esi
	movq	%rsi, %r10
	subq	%r8, %r10
	movmskps	%xmm0, %r8d
	movdqa	%xmm10, %xmm0
	addq	%r10, %rcx
	pcmpgtd	%xmm4, %xmm0
	movq	%r8, %r10
	popcntq	%r8, %r8
	salq	$4, %r10
	pshufb	(%rdi,%r10), %xmm11
	leaq	-16(%rdx,%rcx), %r10
	movups	%xmm11, (%rax,%rcx,4)
	movups	%xmm11, (%rax,%r10,4)
	movq	%rsi, %r10
	subq	%r8, %r10
	leaq	(%r10,%rcx), %r8
	movmskps	%xmm0, %ecx
	movdqa	%xmm9, %xmm0
	movq	%rcx, %r10
	pcmpgtd	%xmm4, %xmm0
	popcntq	%rcx, %rcx
	salq	$4, %r10
	pshufb	(%rdi,%r10), %xmm10
	leaq	-20(%rdx,%r8), %r10
	movups	%xmm10, (%rax,%r8,4)
	movups	%xmm10, (%rax,%r10,4)
	movq	%rsi, %r10
	subq	%rcx, %r10
	leaq	(%r10,%r8), %rcx
	movmskps	%xmm0, %r8d
	movdqa	%xmm8, %xmm0
	movq	%r8, %r10
	pcmpgtd	%xmm4, %xmm0
	popcntq	%r8, %r8
	salq	$4, %r10
	pshufb	(%rdi,%r10), %xmm9
	leaq	-24(%rdx,%rcx), %r10
	movups	%xmm9, (%rax,%rcx,4)
	movups	%xmm9, (%rax,%r10,4)
	movq	%rsi, %r10
	subq	%r8, %r10
	leaq	(%r10,%rcx), %r8
	movmskps	%xmm0, %ecx
	movdqa	%xmm7, %xmm0
	movq	%rcx, %r10
	pcmpgtd	%xmm4, %xmm0
	salq	$4, %r10
	pshufb	(%rdi,%r10), %xmm8
	leaq	-28(%rdx,%r8), %r10
	movups	%xmm8, (%rax,%r8,4)
	movups	%xmm8, (%rax,%r10,4)
	xorl	%r10d, %r10d
	popcntq	%rcx, %r10
	movq	%rsi, %rcx
	subq	%r10, %rcx
	addq	%r8, %rcx
	movmskps	%xmm0, %r8d
	movq	%r8, %r10
	leaq	-32(%rdx,%rcx), %rdx
	popcntq	%r8, %r8
	subq	%r8, %rsi
	salq	$4, %r10
	pshufb	(%rdi,%r10), %xmm7
	movq	%r14, %rdi
	movups	%xmm7, (%rax,%rcx,4)
	movups	%xmm7, (%rax,%rdx,4)
	leaq	(%rsi,%rcx), %rdx
	subq	%rdx, %rdi
	leaq	0(,%rdx,4), %rcx
.L1254:
	movdqa	%xmm6, %xmm1
	pcmpeqd	%xmm0, %xmm0
	movdqa	%xmm6, %xmm2
	cmpq	$4, %rdi
	pcmpgtd	%xmm4, %xmm1
	leaq	-16(,%r14,4), %rsi
	cmovnb	%rcx, %rsi
	xorl	%edi, %edi
	pxor	%xmm1, %xmm0
	movdqu	(%rax,%rsi), %xmm7
	movmskps	%xmm0, %esi
	popcntq	%rsi, %rdi
	movq	%rdi, %xmm0
	salq	$4, %rsi
	movups	%xmm7, (%rax,%r14,4)
	pshufd	$0, %xmm0, %xmm0
	pshufb	(%r9,%rsi), %xmm2
	pcmpgtd	%xmm5, %xmm0
	movd	%xmm0, %esi
	testl	%esi, %esi
	je	.L1260
	movd	%xmm2, (%rax,%rcx)
.L1260:
	pextrd	$1, %xmm0, %esi
	testl	%esi, %esi
	je	.L1261
	pextrd	$1, %xmm2, 4(%rax,%rcx)
.L1261:
	pextrd	$2, %xmm0, %esi
	testl	%esi, %esi
	je	.L1262
	pextrd	$2, %xmm2, 8(%rax,%rcx)
.L1262:
	pextrd	$3, %xmm0, %esi
	testl	%esi, %esi
	je	.L1263
	pextrd	$3, %xmm2, 12(%rax,%rcx)
.L1263:
	addq	%rdx, %rdi
	xorl	%esi, %esi
	movmskps	%xmm1, %edx
	popcntq	%rdx, %rsi
	movq	%rsi, %xmm0
	salq	$4, %rdx
	leaq	0(,%rdi,4), %rcx
	pshufd	$0, %xmm0, %xmm0
	pshufb	(%r9,%rdx), %xmm6
	pcmpgtd	%xmm5, %xmm0
	movd	%xmm0, %edx
	testl	%edx, %edx
	je	.L1264
	movd	%xmm6, (%rax,%rdi,4)
.L1264:
	pextrd	$1, %xmm0, %edx
	testl	%edx, %edx
	je	.L1265
	pextrd	$1, %xmm6, 4(%rax,%rcx)
.L1265:
	pextrd	$2, %xmm0, %edx
	testl	%edx, %edx
	je	.L1266
	pextrd	$2, %xmm6, 8(%rax,%rcx)
.L1266:
	pextrd	$3, %xmm0, %edx
	testl	%edx, %edx
	je	.L1267
	pextrd	$3, %xmm6, 12(%rax,%rcx)
.L1267:
	movq	-88(%rbp), %r14
	addq	%rdi, %r15
	subq	$1, %r14
	cmpl	$2, -112(%rbp)
	je	.L1269
	movq	-80(%rbp), %r8
	movq	-96(%rbp), %rsi
	movq	%r14, %r9
	movq	%r12, %rcx
	movq	%r15, %rdx
	movq	%r13, %rdi
	call	_ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	cmpl	$3, -112(%rbp)
	je	.L1159
.L1269:
	movq	-72(%rbp), %rdx
	movq	-80(%rbp), %r8
	leaq	0(%r13,%r15,4), %rdi
	movq	%r14, %r9
	movq	-96(%rbp), %rsi
	movq	%r12, %rcx
	subq	%r15, %rdx
	call	_ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L1159:
	addq	$88, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L1250:
	.cfi_restore_state
	movq	(%r12), %rcx
	leaq	8(%r11), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%r11)
	movl	%r8d, %ecx
	movq	-8(%r12,%rcx), %rsi
	movq	%rsi, -8(%r11,%rcx)
	subq	%rdi, %r11
	movq	%r12, %rsi
	leal	(%r8,%r11), %ecx
	subq	%r11, %rsi
	shrl	$3, %ecx
	rep movsq
	jmp	.L1251
	.p2align 4,,10
	.p2align 3
.L1246:
	movq	(%r11), %rcx
	leaq	8(%rax), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%rax)
	movl	%r8d, %ecx
	movq	-8(%r11,%rcx), %rsi
	movq	%rsi, -8(%rax,%rcx)
	movq	%rax, %rcx
	movq	%r11, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%r8d, %ecx
	shrl	$3, %ecx
	rep movsq
	jmp	.L1247
.L1451:
	movl	(%r12), %ecx
	jmp	.L1220
.L1456:
	movzbl	(%r12), %esi
	movb	%sil, (%r11)
	testb	$2, %cl
	je	.L1251
	movzwl	-2(%r12,%rcx), %esi
	movw	%si, -2(%r11,%rcx)
	jmp	.L1251
.L1454:
	movzbl	(%r11), %esi
	movb	%sil, (%rax)
	testb	$2, %cl
	je	.L1247
	movzwl	-2(%r11,%rcx), %esi
	movw	%si, -2(%rax,%rcx)
	jmp	.L1247
.L1449:
	cmpq	$1, %rdx
	jbe	.L1159
	leaq	256(%rdi), %rax
	cmpq	%rax, %rsi
	jb	.L1459
	movl	$4, %esi
	call	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	jmp	.L1159
.L1174:
	movq	-112(%rbp), %rax
	movl	$4, %edi
	movdqu	0(%r13), %xmm0
	andl	$3, %eax
	pcmpeqd	%xmm2, %xmm0
	subq	%rax, %rdi
	movd	%edi, %xmm5
	pshufd	$0, %xmm5, %xmm1
	movdqa	.LC0(%rip), %xmm5
	pcmpgtd	%xmm5, %xmm1
	pandn	%xmm1, %xmm0
	movmskps	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1460
	pxor	%xmm1, %xmm1
	movq	-72(%rbp), %r8
	leaq	256(%r13,%rdi,4), %rsi
	pxor	%xmm6, %xmm6
	movdqa	%xmm1, %xmm0
	.p2align 4,,10
	.p2align 3
.L1180:
	movq	%rdi, %rcx
	leaq	64(%rdi), %rdi
	cmpq	%rdi, %r8
	jb	.L1461
	leaq	-256(%rsi), %rax
.L1179:
	movdqa	(%rax), %xmm4
	leaq	32(%rax), %rdx
	pxor	%xmm3, %xmm4
	por	%xmm4, %xmm0
	movdqa	16(%rax), %xmm4
	pxor	%xmm3, %xmm4
	por	%xmm4, %xmm1
	movdqa	32(%rax), %xmm4
	pxor	%xmm3, %xmm4
	por	%xmm4, %xmm0
	movdqa	48(%rax), %xmm4
	pxor	%xmm3, %xmm4
	por	%xmm4, %xmm1
	movdqa	64(%rax), %xmm4
	pxor	%xmm3, %xmm4
	por	%xmm4, %xmm0
	movdqa	80(%rax), %xmm4
	leaq	96(%rdx), %rax
	pxor	%xmm3, %xmm4
	por	%xmm4, %xmm1
	movdqa	64(%rdx), %xmm4
	pxor	%xmm3, %xmm4
	por	%xmm4, %xmm0
	movdqa	80(%rdx), %xmm4
	pxor	%xmm3, %xmm4
	por	%xmm4, %xmm1
	cmpq	%rsi, %rax
	jne	.L1179
	movdqa	%xmm0, %xmm4
	leaq	352(%rdx), %rsi
	por	%xmm1, %xmm4
	pcmpeqd	%xmm6, %xmm4
	movmskps	%xmm4, %eax
	cmpl	$15, %eax
	je	.L1180
	movdqa	%xmm2, %xmm0
	pcmpeqd	%xmm1, %xmm1
	pcmpeqd	0(%r13,%rcx,4), %xmm0
	pxor	%xmm1, %xmm0
	movmskps	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1182
	.p2align 4,,10
	.p2align 3
.L1181:
	addq	$4, %rcx
	movdqa	%xmm2, %xmm0
	pcmpeqd	0(%r13,%rcx,4), %xmm0
	pxor	%xmm1, %xmm0
	movmskps	%xmm0, %eax
	testl	%eax, %eax
	je	.L1181
.L1182:
	rep bsfl	%eax, %eax
	cltq
	addq	%rcx, %rax
.L1178:
	leaq	0(%r13,%rax,4), %rdi
	movdqa	%xmm2, %xmm1
	movl	(%rdi), %ecx
	movd	%ecx, %xmm6
	pshufd	$0, %xmm6, %xmm3
	movdqa	%xmm3, %xmm0
	movdqa	%xmm3, %xmm8
	pcmpgtd	%xmm2, %xmm0
	movmskps	%xmm0, %edx
	testl	%edx, %edx
	jne	.L1187
	movq	-72(%rbp), %rdi
	xorl	%esi, %esi
	leaq	-4(%rdi), %rax
	jmp	.L1196
	.p2align 4,,10
	.p2align 3
.L1188:
	movmskps	%xmm0, %edx
	movups	%xmm1, 0(%r13,%rax,4)
	popcntq	%rdx, %rdx
	addq	%rdx, %rsi
	leaq	-4(%rax), %rdx
	cmpq	%rdx, %rdi
	jbe	.L1462
	movq	%rdx, %rax
.L1196:
	movdqu	0(%r13,%rax,4), %xmm0
	movdqu	0(%r13,%rax,4), %xmm4
	pcmpeqd	%xmm2, %xmm0
	pcmpeqd	%xmm3, %xmm4
	movdqa	%xmm0, %xmm6
	movdqa	%xmm0, %xmm7
	por	%xmm4, %xmm6
	movmskps	%xmm6, %edx
	cmpl	$15, %edx
	je	.L1188
	pcmpeqd	%xmm0, %xmm0
	leaq	4(%rax), %rdi
	pxor	%xmm0, %xmm7
	pandn	%xmm7, %xmm4
	movmskps	%xmm4, %edx
	rep bsfl	%edx, %edx
	movslq	%edx, %rdx
	addq	%rax, %rdx
	addq	$8, %rax
	movd	0(%r13,%rdx,4), %xmm6
	movq	-72(%rbp), %rdx
	pshufd	$0, %xmm6, %xmm0
	subq	%rsi, %rdx
	movdqa	%xmm0, %xmm4
	movaps	%xmm0, -64(%rbp)
	cmpq	%rax, %rdx
	jb	.L1189
.L1190:
	movups	%xmm8, -16(%r13,%rax,4)
	movq	%rax, %rdi
	addq	$4, %rax
	cmpq	%rdx, %rax
	jbe	.L1190
.L1189:
	subq	%rdi, %rdx
	leaq	0(,%rdi,4), %rsi
	movq	%rdx, %xmm0
	pshufd	$0, %xmm0, %xmm0
	pcmpgtd	%xmm5, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1200
	movl	%ecx, 0(%r13,%rdi,4)
.L1200:
	pextrd	$1, %xmm0, %eax
	testl	%eax, %eax
	je	.L1201
	movl	%ecx, 4(%r13,%rsi)
.L1201:
	pextrd	$2, %xmm0, %eax
	testl	%eax, %eax
	je	.L1202
	movl	%ecx, 8(%r13,%rsi)
.L1202:
	pextrd	$3, %xmm0, %eax
	testl	%eax, %eax
	je	.L1195
	movl	%ecx, 12(%r13,%rsi)
.L1195:
	movdqa	%xmm2, %xmm0
	pcmpeqd	.LC5(%rip), %xmm0
	movmskps	%xmm0, %eax
	cmpl	$15, %eax
	je	.L1287
	movdqa	%xmm2, %xmm0
	pcmpeqd	.LC6(%rip), %xmm0
	movmskps	%xmm0, %eax
	cmpl	$15, %eax
	je	.L1213
	movdqa	%xmm3, %xmm5
	movdqa	%xmm2, %xmm0
	pminsd	%xmm4, %xmm5
	pcmpgtd	%xmm5, %xmm0
	movmskps	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1463
	movdqa	%xmm1, %xmm3
	movl	$64, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L1208:
	leaq	(%rcx,%rax,4), %rdx
	addq	$1, %rax
	movdqu	0(%r13,%rdx,4), %xmm0
	pminsd	%xmm3, %xmm0
	movdqa	%xmm0, %xmm3
	cmpq	$16, %rax
	jne	.L1208
	movdqa	%xmm2, %xmm4
	pcmpgtd	%xmm0, %xmm4
	movmskps	%xmm4, %eax
	testl	%eax, %eax
	jne	.L1443
	leaq	64(%rsi), %rax
	cmpq	%rax, -72(%rbp)
	jb	.L1464
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L1208
.L1290:
	movdqa	.LC0(%rip), %xmm5
	xorl	%r8d, %r8d
	xorl	%esi, %esi
	movq	%r13, %rax
	leaq	_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %r9
	jmp	.L1227
.L1291:
	xorl	%ecx, %ecx
	movq	%r14, %rdi
	jmp	.L1254
.L1292:
	movq	%rdx, %r8
	movq	%rax, %r10
	movl	$4, %ecx
	leaq	_ZZN3hwy6N_SSE46detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %rdi
	jmp	.L1255
.L1452:
	movq	-72(%rbp), %rsi
	leaq	-1(%rsi), %rbx
	movq	%rbx, %r12
	shrq	%r12
	.p2align 4,,10
	.p2align 3
.L1225:
	movq	%r12, %rdx
	movq	%r13, %rdi
	call	_ZN3hwy6N_SSE46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	subq	$1, %r12
	jnb	.L1225
	.p2align 4,,10
	.p2align 3
.L1226:
	movl	0(%r13,%rbx,4), %edx
	movl	0(%r13), %eax
	movq	%rbx, %rsi
	movq	%r13, %rdi
	movl	%edx, 0(%r13)
	xorl	%edx, %edx
	movl	%eax, 0(%r13,%rbx,4)
	call	_ZN3hwy6N_SSE46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	subq	$1, %rbx
	jne	.L1226
	jmp	.L1159
.L1455:
	movl	(%r12), %esi
	movl	%esi, (%r11)
	movl	-4(%r12,%rcx), %esi
	movl	%esi, -4(%r11,%rcx)
	jmp	.L1251
.L1453:
	movl	(%r11), %esi
	movl	%esi, (%rax)
	movl	-4(%r11,%rcx), %esi
	movl	%esi, -4(%rax,%rcx)
	jmp	.L1247
.L1461:
	movq	-72(%rbp), %rsi
	pcmpeqd	%xmm1, %xmm1
.L1184:
	movq	%rcx, %rdx
	addq	$4, %rcx
	cmpq	%rcx, %rsi
	jb	.L1465
	movdqa	%xmm2, %xmm0
	pcmpeqd	-16(%r13,%rcx,4), %xmm0
	pxor	%xmm1, %xmm0
	movmskps	%xmm0, %eax
	testl	%eax, %eax
	je	.L1184
.L1440:
	rep bsfl	%eax, %eax
	cltq
	addq	%rdx, %rax
	jmp	.L1178
.L1187:
	movq	-72(%rbp), %rsi
	leaq	-64(%rbp), %rdx
	movq	%r12, %rcx
	movdqa	%xmm3, %xmm1
	movdqa	%xmm2, %xmm0
	movaps	%xmm3, -112(%rbp)
	subq	%rax, %rsi
	call	_ZN3hwy6N_SSE46detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L1159
	movd	(%r12), %xmm5
	movdqa	-64(%rbp), %xmm4
	movdqa	-112(%rbp), %xmm3
	pshufd	$0, %xmm5, %xmm2
	movdqa	%xmm2, %xmm1
	jmp	.L1195
.L1462:
	movd	%eax, %xmm6
	movdqu	0(%r13), %xmm0
	movq	-72(%rbp), %rdx
	pshufd	$0, %xmm6, %xmm4
	movdqu	0(%r13), %xmm6
	pcmpgtd	%xmm5, %xmm4
	pcmpeqd	%xmm2, %xmm0
	subq	%rsi, %rdx
	pcmpeqd	%xmm3, %xmm6
	movdqa	%xmm4, %xmm7
	pand	%xmm0, %xmm7
	por	%xmm6, %xmm0
	pcmpeqd	%xmm6, %xmm6
	pxor	%xmm6, %xmm4
	por	%xmm4, %xmm0
	movmskps	%xmm0, %edi
	cmpl	$15, %edi
	jne	.L1466
	movmskps	%xmm7, %ecx
	movq	%rdx, %rax
	movups	%xmm1, 0(%r13)
	popcntq	%rcx, %rcx
	subq	%rcx, %rax
	cmpq	$3, %rax
	jbe	.L1276
	leaq	-4(%rax), %rdx
	movq	-120(%rbp), %rsi
	movq	%rdx, %rcx
	shrq	$2, %rcx
	salq	$4, %rcx
	leaq	16(%r13,%rcx), %rcx
.L1205:
	movups	%xmm8, (%rsi)
	addq	$16, %rsi
	cmpq	%rcx, %rsi
	jne	.L1205
	andq	$-4, %rdx
	addq	$4, %rdx
	leaq	0(,%rdx,4), %rcx
	subq	%rdx, %rax
.L1204:
	movaps	%xmm3, (%r12)
	testq	%rax, %rax
	je	.L1159
	leaq	0(%r13,%rcx), %rdi
	leaq	0(,%rax,4), %rdx
	movq	%r12, %rsi
	call	memcpy@PLT
	jmp	.L1159
.L1464:
	movq	-72(%rbp), %rdx
	jmp	.L1215
.L1216:
	movdqu	-16(%r13,%rsi,4), %xmm5
	movdqa	%xmm2, %xmm0
	pcmpgtd	%xmm5, %xmm0
	movmskps	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1443
.L1215:
	movq	%rsi, %rax
	addq	$4, %rsi
	cmpq	%rsi, %rdx
	jnb	.L1216
	movq	-72(%rbp), %rdi
	cmpq	%rax, %rdi
	je	.L1287
	movdqu	-16(%r13,%rdi,4), %xmm5
	movaps	%xmm5, -112(%rbp)
	pcmpgtd	-112(%rbp), %xmm2
	movmskps	%xmm2, %eax
	cmpl	$1, %eax
	movl	$1, %eax
	adcl	$0, %eax
	movl	%eax, -112(%rbp)
	jmp	.L1217
.L1465:
	movq	-72(%rbp), %rax
	pcmpeqd	%xmm1, %xmm1
	leaq	-4(%rax), %rdx
	movdqu	0(%r13,%rdx,4), %xmm0
	pcmpeqd	%xmm2, %xmm0
	pxor	%xmm1, %xmm0
	movmskps	%xmm0, %eax
	testl	%eax, %eax
	je	.L1159
	jmp	.L1440
	.p2align 4,,10
	.p2align 3
.L1212:
	movdqu	-16(%r13,%rsi,4), %xmm0
	pcmpgtd	%xmm2, %xmm0
	movmskps	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1443
.L1211:
	movq	%rsi, %rax
	addq	$4, %rsi
	cmpq	%rsi, -72(%rbp)
	jnb	.L1212
	movq	-72(%rbp), %rdi
	cmpq	%rax, %rdi
	je	.L1213
	movdqu	-16(%r13,%rdi,4), %xmm5
	movaps	%xmm5, -112(%rbp)
	movdqa	-112(%rbp), %xmm0
	pcmpgtd	%xmm2, %xmm0
	movmskps	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1443
.L1213:
	pcmpeqd	%xmm1, %xmm1
	movl	$3, -112(%rbp)
	paddd	%xmm2, %xmm1
	jmp	.L1217
.L1287:
	movl	$2, -112(%rbp)
	jmp	.L1217
.L1460:
	rep bsfl	%eax, %eax
	cltq
	jmp	.L1178
.L1459:
	movq	%rdx, %rcx
	xorl	%eax, %eax
	cmpq	$3, %rdx
	jbe	.L1165
	movq	%rdx, %rbx
	leaq	-4(%rdx), %rdx
	movq	(%rdi), %rcx
	movq	%rdx, %rax
	shrq	$2, %rax
	movq	%rcx, (%r12)
	addq	$1, %rax
	salq	$4, %rax
	movl	%eax, %ecx
	movq	-8(%rdi,%rcx), %rsi
	leaq	8(%r12), %rdi
	andq	$-8, %rdi
	movq	%rsi, -8(%r12,%rcx)
	movq	%r12, %rcx
	movq	%r13, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%eax, %ecx
	movq	%rdx, %rax
	andq	$-4, %rax
	shrl	$3, %ecx
	rep movsq
	addq	$4, %rax
	movq	%rbx, %rcx
	subq	%rax, %rcx
	je	.L1168
.L1165:
	salq	$2, %rax
	leaq	0(,%rcx,4), %rdx
	testq	%rcx, %rcx
	movl	$4, %ecx
	cmove	%rcx, %rdx
	leaq	(%r12,%rax), %rdi
	leaq	0(%r13,%rax), %rsi
	call	memcpy@PLT
.L1168:
	movq	-72(%rbp), %rdi
	movl	$32, %ecx
	movl	%edi, %eax
	subl	$1, %eax
	bsrl	%eax, %eax
	xorl	$31, %eax
	subl	%eax, %ecx
	movl	$1, %eax
	salq	%cl, %rax
	shrq	$4, %rax
	movq	%rax, %rsi
	movl	$1, %eax
	cmove	%rax, %rsi
	movq	%rsi, %rdx
	salq	$4, %rdx
	addq	$4, %rdx
	cmpq	%rdx, %rdi
	jnb	.L1167
	movdqa	.LC4(%rip), %xmm0
	movq	%rdi, %rax
.L1166:
	movups	%xmm0, (%r12,%rax,4)
	addq	$4, %rax
	cmpq	%rdx, %rax
	jb	.L1166
.L1167:
	movq	%r12, %rdi
	call	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	xorl	%eax, %eax
	cmpq	$3, -72(%rbp)
	jbe	.L1170
	movq	-72(%rbp), %rbx
	movq	(%r12), %rcx
	leaq	8(%r13), %rdi
	andq	$-8, %rdi
	leaq	-4(%rbx), %rdx
	movq	%rcx, 0(%r13)
	movq	%rdx, %rax
	shrq	$2, %rax
	addq	$1, %rax
	salq	$4, %rax
	movl	%eax, %ecx
	movq	-8(%r12,%rcx), %rsi
	movq	%rsi, -8(%r13,%rcx)
	movq	%r13, %rcx
	movq	%r12, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%eax, %ecx
	movq	%rdx, %rax
	andq	$-4, %rax
	shrl	$3, %ecx
	rep movsq
	addq	$4, %rax
	subq	%rax, %rbx
	movq	%rbx, -72(%rbp)
	je	.L1159
.L1170:
	movq	-72(%rbp), %rsi
	salq	$2, %rax
	movl	$4, %ecx
	leaq	0(%r13,%rax), %rdi
	testq	%rsi, %rsi
	leaq	0(,%rsi,4), %rdx
	leaq	(%r12,%rax), %rsi
	cmove	%rcx, %rdx
	call	memcpy@PLT
	jmp	.L1159
.L1463:
	pmaxsd	%xmm4, %xmm3
	movdqa	%xmm3, %xmm0
	pcmpgtd	%xmm2, %xmm0
	movmskps	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1443
	movdqa	%xmm1, %xmm3
	movl	$64, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L1209:
	leaq	(%rcx,%rax,4), %rdx
	addq	$1, %rax
	movdqu	0(%r13,%rdx,4), %xmm0
	pmaxsd	%xmm3, %xmm0
	movdqa	%xmm0, %xmm3
	cmpq	$16, %rax
	jne	.L1209
	pcmpgtd	%xmm2, %xmm0
	movmskps	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1443
	leaq	64(%rsi), %rax
	cmpq	%rax, -72(%rbp)
	jb	.L1211
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L1209
.L1466:
	pxor	%xmm6, %xmm0
	movmskps	%xmm0, %esi
	rep bsfl	%esi, %esi
	movslq	%esi, %rsi
	movd	0(%r13,%rsi,4), %xmm6
	leaq	4(%rax), %rsi
	pshufd	$0, %xmm6, %xmm0
	movdqa	%xmm0, %xmm4
	movaps	%xmm0, -64(%rbp)
	cmpq	%rdx, %rsi
	ja	.L1198
.L1199:
	movups	%xmm8, -16(%r13,%rsi,4)
	movq	%rsi, %rax
	addq	$4, %rsi
	cmpq	%rdx, %rsi
	jbe	.L1199
.L1198:
	subq	%rax, %rdx
	leaq	0(,%rax,4), %rsi
	movq	%rdx, %xmm0
	pshufd	$0, %xmm0, %xmm0
	pcmpgtd	%xmm5, %xmm0
	movd	%xmm0, %edx
	testl	%edx, %edx
	je	.L1200
	movl	%ecx, 0(%r13,%rax,4)
	jmp	.L1200
.L1276:
	xorl	%ecx, %ecx
	jmp	.L1204
	.cfi_endproc
.LFE18804:
	.size	_ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0, .-_ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	.section	.text._ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0, @function
_ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0:
.LFB18806:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	movq	%rcx, %r13
	pushq	%r12
	pushq	%rbx
	subq	$360, %rsp
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	movq	%rdi, -88(%rbp)
	movq	%rsi, -240(%rbp)
	movq	%rdx, -176(%rbp)
	movq	%r8, -184(%rbp)
	movq	%r9, -192(%rbp)
	cmpq	$64, %rdx
	jbe	.L1753
	movq	%rdi, %r15
	movq	%rdi, %r10
	shrq	$2, %r15
	movq	%r15, %rax
	andl	$15, %eax
	jne	.L1754
	movq	%rdx, %rbx
	movq	%rdi, %r14
	movq	%r8, %rax
.L1480:
	movq	8(%rax), %rdx
	movq	16(%rax), %r9
	movq	%rdx, %rcx
	leaq	1(%r9), %rdi
	movq	%rdx, %rsi
	xorq	(%rax), %rdi
	rolq	$24, %rcx
	shrq	$11, %rsi
	movq	%rcx, %rax
	leaq	(%rdx,%rdx,8), %rcx
	leaq	2(%r9), %rdx
	addq	%rdi, %rax
	xorq	%rsi, %rcx
	movq	%rax, %rsi
	xorq	%rdx, %rcx
	leaq	(%rax,%rax,8), %rdx
	movq	%rax, %r8
	rolq	$24, %rsi
	shrq	$11, %r8
	leaq	3(%r9), %rax
	addq	%rcx, %rsi
	xorq	%r8, %rdx
	movq	%rsi, %r8
	xorq	%rax, %rdx
	leaq	(%rsi,%rsi,8), %rax
	rolq	$24, %r8
	movq	%r8, %r11
	movq	%rsi, %r8
	leaq	4(%r9), %rsi
	addq	$5, %r9
	addq	%rdx, %r11
	shrq	$11, %r8
	xorq	%r8, %rax
	movq	%r11, %r12
	movq	%r11, %r8
	shrq	$11, %r12
	xorq	%rsi, %rax
	rolq	$24, %r8
	leaq	(%r11,%r11,8), %rsi
	addq	%rax, %r8
	xorq	%r12, %rsi
	xorq	%r9, %rsi
	leaq	(%r8,%r8,8), %r11
	movq	%r8, %r12
	rolq	$24, %r8
	addq	%rsi, %r8
	shrq	$11, %r12
	movl	%esi, %esi
	xorq	%r12, %r11
	movq	%r8, %xmm6
	movq	%rbx, %r12
	movabsq	$68719476719, %r8
	movq	%r11, %xmm0
	shrq	$4, %r12
	movq	-184(%rbp), %r11
	cmpq	%r8, %rbx
	movl	$4294967295, %r8d
	punpcklqdq	%xmm6, %xmm0
	movl	%edi, %ebx
	cmova	%r8, %r12
	movq	%r9, 16(%r11)
	shrq	$32, %rdi
	movl	%edx, %r9d
	movl	%eax, %r8d
	shrq	$32, %rdx
	movups	%xmm0, (%r11)
	movl	%ecx, %r11d
	shrq	$32, %rcx
	imulq	%r12, %rbx
	shrq	$32, %rax
	imulq	%r12, %rdi
	imulq	%r12, %r11
	imulq	%r12, %rcx
	shrq	$32, %rbx
	imulq	%r12, %r9
	shrq	$32, %rdi
	salq	$6, %rbx
	imulq	%r12, %rdx
	shrq	$32, %r11
	salq	$6, %rdi
	addq	%r14, %rbx
	imulq	%r12, %r8
	shrq	$32, %rcx
	salq	$6, %r11
	addq	%r14, %rdi
	shrq	$32, %r9
	salq	$6, %rcx
	addq	%r14, %r11
	shrq	$32, %rdx
	salq	$6, %r9
	addq	%r14, %rcx
	shrq	$32, %r8
	salq	$6, %rdx
	addq	%r14, %r9
	salq	$6, %r8
	addq	%r14, %rdx
	addq	%r14, %r8
	imulq	%r12, %rax
	imulq	%r12, %rsi
	shrq	$32, %rax
	shrq	$32, %rsi
	salq	$6, %rax
	salq	$6, %rsi
	addq	%r14, %rax
	leaq	(%r14,%rsi), %r12
	xorl	%esi, %esi
.L1482:
	movdqa	(%r11,%rsi,4), %xmm0
	movdqa	(%rbx,%rsi,4), %xmm3
	movdqa	%xmm0, %xmm1
	movdqa	%xmm3, %xmm2
	pcmpgtd	%xmm3, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm2
	pandn	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	por	%xmm4, %xmm2
	movdqa	%xmm1, %xmm4
	movdqa	%xmm0, %xmm1
	movdqa	(%rdi,%rsi,4), %xmm0
	pandn	%xmm3, %xmm4
	pcmpgtd	%xmm2, %xmm0
	por	%xmm4, %xmm1
	movdqa	%xmm0, %xmm3
	pand	(%rdi,%rsi,4), %xmm0
	pandn	%xmm2, %xmm3
	movdqa	%xmm1, %xmm2
	por	%xmm3, %xmm0
	pcmpgtd	%xmm0, %xmm2
	movdqa	%xmm2, %xmm3
	pand	%xmm2, %xmm0
	pandn	%xmm1, %xmm3
	por	%xmm3, %xmm0
	movdqa	(%rcx,%rsi,4), %xmm3
	movaps	%xmm0, 0(%r13,%rsi,4)
	movdqa	(%rdx,%rsi,4), %xmm0
	movdqa	%xmm3, %xmm2
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm3, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm2
	pandn	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	por	%xmm4, %xmm2
	movdqa	%xmm1, %xmm4
	movdqa	%xmm0, %xmm1
	movdqa	(%r9,%rsi,4), %xmm0
	pandn	%xmm3, %xmm4
	pcmpgtd	%xmm2, %xmm0
	por	%xmm4, %xmm1
	movdqa	%xmm0, %xmm3
	pand	(%r9,%rsi,4), %xmm0
	pandn	%xmm2, %xmm3
	movdqa	%xmm1, %xmm2
	por	%xmm3, %xmm0
	pcmpgtd	%xmm0, %xmm2
	movdqa	%xmm2, %xmm3
	pand	%xmm2, %xmm0
	pandn	%xmm1, %xmm3
	por	%xmm3, %xmm0
	movdqa	(%r8,%rsi,4), %xmm3
	movaps	%xmm0, 64(%r13,%rsi,4)
	movdqa	(%r12,%rsi,4), %xmm0
	movdqa	%xmm3, %xmm2
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm3, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm2
	pandn	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	por	%xmm4, %xmm2
	movdqa	%xmm1, %xmm4
	movdqa	%xmm0, %xmm1
	movdqa	(%rax,%rsi,4), %xmm0
	pandn	%xmm3, %xmm4
	pcmpgtd	%xmm2, %xmm0
	por	%xmm4, %xmm1
	movdqa	%xmm0, %xmm3
	pand	(%rax,%rsi,4), %xmm0
	pandn	%xmm2, %xmm3
	movdqa	%xmm1, %xmm2
	por	%xmm3, %xmm0
	pcmpgtd	%xmm0, %xmm2
	movdqa	%xmm2, %xmm3
	pand	%xmm2, %xmm0
	pandn	%xmm1, %xmm3
	por	%xmm3, %xmm0
	movaps	%xmm0, 128(%r13,%rsi,4)
	addq	$4, %rsi
	cmpq	$16, %rsi
	jne	.L1482
	movd	0(%r13), %xmm6
	movdqa	16(%r13), %xmm1
	movdqa	0(%r13), %xmm3
	pshufd	$0, %xmm6, %xmm0
	pxor	%xmm0, %xmm3
	pxor	%xmm0, %xmm1
	movdqa	%xmm0, %xmm2
	por	%xmm3, %xmm1
	movdqa	32(%r13), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	48(%r13), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	64(%r13), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	80(%r13), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	96(%r13), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	112(%r13), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	128(%r13), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	144(%r13), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	160(%r13), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	movdqa	176(%r13), %xmm3
	pxor	%xmm0, %xmm3
	por	%xmm3, %xmm1
	pxor	%xmm3, %xmm3
	pcmpeqd	%xmm3, %xmm1
	movmskps	%xmm1, %eax
	cmpl	$15, %eax
	je	.L1483
	movdqa	.LC4(%rip), %xmm0
	movl	$4, %esi
	movq	%r13, %rdi
	leaq	192(%r13), %r12
	movups	%xmm0, 192(%r13)
	movups	%xmm0, 208(%r13)
	movups	%xmm0, 224(%r13)
	movups	%xmm0, 240(%r13)
	movups	%xmm0, 256(%r13)
	call	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	movd	0(%r13), %xmm6
	pcmpeqd	%xmm2, %xmm2
	pshufd	$0, %xmm6, %xmm0
	movd	188(%r13), %xmm6
	pshufd	$0, %xmm6, %xmm1
	paddd	%xmm1, %xmm2
	pcmpeqd	%xmm0, %xmm2
	movmskps	%xmm2, %eax
	cmpl	$15, %eax
	jne	.L1485
	movq	-176(%rbp), %rsi
	movq	-88(%rbp), %rdi
	leaq	-64(%rbp), %rdx
	movq	%r12, %rcx
	call	_ZN3hwy7N_SSSE36detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L1467
.L1485:
	movl	96(%r13), %ecx
	movl	$23, %eax
	movl	$24, %edx
	cmpl	%ecx, 92(%r13)
	je	.L1527
	jmp	.L1532
	.p2align 4,,10
	.p2align 3
.L1530:
	testq	%rax, %rax
	je	.L1755
.L1527:
	movq	%rax, %rdx
	subq	$1, %rax
	movl	0(%r13,%rax,4), %esi
	cmpl	%ecx, %esi
	je	.L1530
	cmpl	%ecx, 0(%r13,%rdx,4)
	je	.L1532
	movl	%esi, %ecx
	jmp	.L1529
	.p2align 4,,10
	.p2align 3
.L1533:
	cmpq	$47, %rdx
	je	.L1751
.L1532:
	movq	%rdx, %rsi
	addq	$1, %rdx
	cmpl	%ecx, 0(%r13,%rdx,4)
	je	.L1533
	movl	$24, %edx
	subq	$23, %rsi
	subq	%rax, %rdx
	cmpq	%rdx, %rsi
	jb	.L1529
.L1751:
	movl	0(%r13,%rax,4), %ecx
.L1529:
	movd	%ecx, %xmm6
	pshufd	$0, %xmm6, %xmm3
.L1752:
	movl	$1, -228(%rbp)
.L1526:
	cmpq	$0, -192(%rbp)
	je	.L1756
	movq	-176(%rbp), %rax
	movq	-88(%rbp), %r14
	movaps	%xmm3, -80(%rbp)
	subq	$4, %rax
	movdqu	(%r14,%rax,4), %xmm6
	movq	%rax, %rbx
	movq	%rax, -112(%rbp)
	andl	$15, %ebx
	movaps	%xmm6, -224(%rbp)
	andl	$12, %eax
	je	.L1599
	movdqu	(%r14), %xmm1
	pcmpeqd	%xmm0, %xmm0
	movdqa	%xmm1, %xmm4
	movaps	%xmm1, -144(%rbp)
	pcmpgtd	%xmm3, %xmm4
	pxor	%xmm4, %xmm0
	movaps	%xmm4, -128(%rbp)
	movmskps	%xmm0, %r12d
	movq	%r12, %rdi
	salq	$4, %r12
	call	__popcountdi2@PLT
	movdqa	-144(%rbp), %xmm1
	movdqa	-128(%rbp), %xmm4
	leaq	_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %rcx
	movd	%eax, %xmm6
	movslq	%eax, %rdx
	movq	%rcx, -168(%rbp)
	pshufd	$0, %xmm6, %xmm0
	movdqa	.LC0(%rip), %xmm6
	movdqa	%xmm1, %xmm2
	pshufb	(%rcx,%r12), %xmm2
	pcmpgtd	%xmm6, %xmm0
	movaps	%xmm6, -208(%rbp)
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1537
	movd	%xmm2, (%r14)
.L1537:
	pshufd	$85, %xmm0, %xmm3
	movd	%xmm3, %eax
	testl	%eax, %eax
	je	.L1538
	movq	-88(%rbp), %rax
	pshufd	$85, %xmm2, %xmm3
	movd	%xmm3, 4(%rax)
.L1538:
	movdqa	%xmm0, %xmm3
	punpckhdq	%xmm0, %xmm3
	movd	%xmm3, %eax
	testl	%eax, %eax
	je	.L1539
	movq	-88(%rbp), %rax
	movdqa	%xmm2, %xmm3
	punpckhdq	%xmm2, %xmm3
	movd	%xmm3, 8(%rax)
.L1539:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1540
	movq	-88(%rbp), %rax
	pshufd	$255, %xmm2, %xmm2
	movd	%xmm2, 12(%rax)
.L1540:
	movmskps	%xmm4, %r15d
	movq	-88(%rbp), %rax
	movaps	%xmm1, -128(%rbp)
	movq	%r15, %rdi
	salq	$4, %r15
	leaq	(%rax,%rdx,4), %r14
	call	__popcountdi2@PLT
	movq	-168(%rbp), %rcx
	movdqa	-128(%rbp), %xmm1
	movslq	%eax, %r12
	pshufb	(%rcx,%r15), %xmm1
	movups	%xmm1, 0(%r13)
	testb	$8, -112(%rbp)
	je	.L1541
	movq	-88(%rbp), %rax
	pcmpeqd	%xmm0, %xmm0
	movdqu	16(%rax), %xmm1
	movdqa	%xmm1, %xmm4
	movaps	%xmm1, -144(%rbp)
	pcmpgtd	-80(%rbp), %xmm4
	pxor	%xmm4, %xmm0
	movaps	%xmm4, -128(%rbp)
	movmskps	%xmm0, %r15d
	movq	%r15, %rdi
	salq	$4, %r15
	call	__popcountdi2@PLT
	movdqa	-144(%rbp), %xmm1
	movdqa	-128(%rbp), %xmm4
	movd	%eax, %xmm6
	movq	-168(%rbp), %rsi
	movslq	%eax, %rcx
	pshufd	$0, %xmm6, %xmm0
	movdqa	%xmm1, %xmm2
	pcmpgtd	-208(%rbp), %xmm0
	pshufb	(%rsi,%r15), %xmm2
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1542
	movd	%xmm2, (%r14)
.L1542:
	pshufd	$85, %xmm0, %xmm3
	movd	%xmm3, %eax
	testl	%eax, %eax
	je	.L1543
	pshufd	$85, %xmm2, %xmm3
	movd	%xmm3, 4(%r14)
.L1543:
	movdqa	%xmm0, %xmm3
	punpckhdq	%xmm0, %xmm3
	movd	%xmm3, %eax
	testl	%eax, %eax
	je	.L1544
	movdqa	%xmm2, %xmm3
	punpckhdq	%xmm2, %xmm3
	movd	%xmm3, 8(%r14)
.L1544:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1545
	pshufd	$255, %xmm2, %xmm2
	movd	%xmm2, 12(%r14)
.L1545:
	movmskps	%xmm4, %r15d
	movaps	%xmm1, -128(%rbp)
	leaq	(%r14,%rcx,4), %r14
	movq	%r15, %rdi
	salq	$4, %r15
	call	__popcountdi2@PLT
	movq	-168(%rbp), %rcx
	movdqa	-128(%rbp), %xmm1
	cltq
	pshufb	(%rcx,%r15), %xmm1
	movups	%xmm1, 0(%r13,%r12,4)
	addq	%rax, %r12
	cmpq	$11, %rbx
	jbe	.L1541
	movq	-88(%rbp), %rax
	pcmpeqd	%xmm0, %xmm0
	movdqu	32(%rax), %xmm1
	movdqa	%xmm1, %xmm4
	movaps	%xmm1, -144(%rbp)
	pcmpgtd	-80(%rbp), %xmm4
	pxor	%xmm4, %xmm0
	movaps	%xmm4, -128(%rbp)
	movmskps	%xmm0, %r15d
	movq	%r15, %rdi
	salq	$4, %r15
	call	__popcountdi2@PLT
	movdqa	-144(%rbp), %xmm1
	movdqa	-128(%rbp), %xmm4
	movd	%eax, %xmm6
	movq	-168(%rbp), %rsi
	movslq	%eax, %rcx
	pshufd	$0, %xmm6, %xmm0
	movdqa	%xmm1, %xmm2
	pcmpgtd	-208(%rbp), %xmm0
	pshufb	(%rsi,%r15), %xmm2
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1546
	movd	%xmm2, (%r14)
.L1546:
	pshufd	$85, %xmm0, %xmm3
	movd	%xmm3, %eax
	testl	%eax, %eax
	je	.L1547
	pshufd	$85, %xmm2, %xmm3
	movd	%xmm3, 4(%r14)
.L1547:
	movdqa	%xmm0, %xmm3
	punpckhdq	%xmm0, %xmm3
	movd	%xmm3, %eax
	testl	%eax, %eax
	je	.L1548
	movdqa	%xmm2, %xmm3
	punpckhdq	%xmm2, %xmm3
	movd	%xmm3, 8(%r14)
.L1548:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1549
	pshufd	$255, %xmm2, %xmm2
	movd	%xmm2, 12(%r14)
.L1549:
	movmskps	%xmm4, %r15d
	movaps	%xmm1, -128(%rbp)
	leaq	(%r14,%rcx,4), %r14
	movq	%r15, %rdi
	salq	$4, %r15
	call	__popcountdi2@PLT
	movq	-168(%rbp), %rcx
	movdqa	-128(%rbp), %xmm1
	cltq
	pshufb	(%rcx,%r15), %xmm1
	movups	%xmm1, 0(%r13,%r12,4)
	addq	%rax, %r12
.L1541:
	leaq	-4(%rbx), %rax
	leaq	1(%rbx), %rcx
	andq	$-4, %rax
	leaq	0(,%r12,4), %r15
	addq	$4, %rax
	cmpq	$4, %rcx
	movl	$4, %ecx
	cmovbe	%rcx, %rax
.L1536:
	cmpq	%rax, %rbx
	je	.L1550
	subq	%rax, %rbx
	movd	%ebx, %xmm6
	movq	-88(%rbp), %rbx
	pshufd	$0, %xmm6, %xmm1
	movdqu	(%rbx,%rax,4), %xmm2
	pcmpgtd	-208(%rbp), %xmm1
	movdqa	%xmm2, %xmm3
	movaps	%xmm2, -160(%rbp)
	pcmpgtd	-80(%rbp), %xmm3
	movaps	%xmm1, -128(%rbp)
	movdqa	%xmm3, %xmm0
	movaps	%xmm3, -144(%rbp)
	pandn	%xmm1, %xmm0
	movmskps	%xmm0, %ebx
	movq	%rbx, %rdi
	salq	$4, %rbx
	call	__popcountdi2@PLT
	movdqa	-160(%rbp), %xmm2
	movq	-168(%rbp), %rsi
	movd	%eax, %xmm7
	movslq	%eax, %rcx
	movdqa	-128(%rbp), %xmm1
	movdqa	-144(%rbp), %xmm3
	pshufd	$0, %xmm7, %xmm0
	movdqa	%xmm2, %xmm4
	pcmpgtd	-208(%rbp), %xmm0
	pshufb	(%rsi,%rbx), %xmm4
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1551
	movd	%xmm4, (%r14)
.L1551:
	pshufd	$85, %xmm0, %xmm5
	movd	%xmm5, %eax
	testl	%eax, %eax
	je	.L1552
	pshufd	$85, %xmm4, %xmm5
	movd	%xmm5, 4(%r14)
.L1552:
	movdqa	%xmm0, %xmm5
	punpckhdq	%xmm0, %xmm5
	movd	%xmm5, %eax
	testl	%eax, %eax
	je	.L1553
	movdqa	%xmm4, %xmm5
	punpckhdq	%xmm4, %xmm5
	movd	%xmm5, 8(%r14)
.L1553:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1554
	pshufd	$255, %xmm4, %xmm4
	movd	%xmm4, 12(%r14)
.L1554:
	pand	%xmm1, %xmm3
	movaps	%xmm2, -128(%rbp)
	leaq	(%r14,%rcx,4), %r14
	movmskps	%xmm3, %ebx
	movq	%rbx, %rdi
	salq	$4, %rbx
	call	__popcountdi2@PLT
	movq	-168(%rbp), %rcx
	movdqa	-128(%rbp), %xmm2
	cltq
	pshufb	(%rcx,%rbx), %xmm2
	addq	%rax, %r12
	movups	%xmm2, 0(%r13,%r15)
	leaq	0(,%r12,4), %r15
.L1550:
	movq	-112(%rbp), %rbx
	movq	-88(%rbp), %rax
	movl	%r15d, %ecx
	subq	%r12, %rbx
	leaq	(%rax,%rbx,4), %rax
	cmpl	$8, %r15d
	jnb	.L1555
	testb	$4, %r15b
	jne	.L1757
	testl	%ecx, %ecx
	jne	.L1758
.L1556:
	movl	%r15d, %ecx
	cmpl	$8, %r15d
	jnb	.L1559
	andl	$4, %r15d
	jne	.L1759
	testl	%ecx, %ecx
	jne	.L1760
.L1560:
	movq	-112(%rbp), %rcx
	movq	%r14, %rax
	subq	-88(%rbp), %rax
	sarq	$2, %rax
	subq	%rax, %rcx
	subq	%rax, %rbx
	movq	%rax, -256(%rbp)
	movq	%rcx, -248(%rbp)
	leaq	(%r14,%rbx,4), %rax
	je	.L1600
	movdqu	(%r14), %xmm6
	movdqu	-16(%rax), %xmm7
	leaq	64(%r14), %rsi
	leaq	-64(%rax), %r8
	movaps	%xmm6, -272(%rbp)
	movdqu	16(%r14), %xmm6
	movaps	%xmm7, -384(%rbp)
	movaps	%xmm6, -288(%rbp)
	movdqu	32(%r14), %xmm6
	movaps	%xmm6, -304(%rbp)
	movdqu	48(%r14), %xmm6
	movaps	%xmm6, -320(%rbp)
	movdqu	-64(%rax), %xmm6
	movaps	%xmm6, -336(%rbp)
	movdqu	-48(%rax), %xmm6
	movaps	%xmm6, -352(%rbp)
	movdqu	-32(%rax), %xmm6
	movaps	%xmm6, -368(%rbp)
	cmpq	%r8, %rsi
	je	.L1601
	movq	%r13, -392(%rbp)
	xorl	%r15d, %r15d
	movq	%rsi, %r13
	leaq	_ZZN3hwy7N_SSSE36detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %r12
	jmp	.L1567
	.p2align 4,,10
	.p2align 3
.L1762:
	movdqu	-64(%r8), %xmm3
	movdqu	-48(%r8), %xmm2
	prefetcht0	-256(%r8)
	subq	$64, %r8
	movdqu	32(%r8), %xmm1
	movdqu	48(%r8), %xmm0
.L1566:
	movdqa	%xmm3, %xmm4
	movq	%r8, -96(%rbp)
	pcmpgtd	-80(%rbp), %xmm4
	movaps	%xmm0, -160(%rbp)
	movaps	%xmm1, -144(%rbp)
	movaps	%xmm2, -128(%rbp)
	movmskps	%xmm4, %edi
	movq	%rdi, %rax
	salq	$4, %rax
	pshufb	(%r12,%rax), %xmm3
	movaps	%xmm3, -112(%rbp)
	call	__popcountdi2@PLT
	movdqa	-112(%rbp), %xmm3
	movdqa	-128(%rbp), %xmm2
	leaq	-4(%rbx,%r15), %rdx
	cltq
	movups	%xmm3, (%r14,%r15,4)
	addq	$4, %r15
	movups	%xmm3, (%r14,%rdx,4)
	movdqa	%xmm2, %xmm3
	subq	%rax, %r15
	pcmpgtd	-80(%rbp), %xmm3
	movmskps	%xmm3, %edi
	movq	%rdi, %rax
	salq	$4, %rax
	pshufb	(%r12,%rax), %xmm2
	movaps	%xmm2, -112(%rbp)
	call	__popcountdi2@PLT
	movdqa	-112(%rbp), %xmm2
	leaq	-8(%rbx,%r15), %rdx
	movdqa	-144(%rbp), %xmm1
	cltq
	movups	%xmm2, (%r14,%r15,4)
	movups	%xmm2, (%r14,%rdx,4)
	movdqa	%xmm1, %xmm2
	movl	$4, %edx
	pcmpgtd	-80(%rbp), %xmm2
	subq	%rax, %rdx
	addq	%rdx, %r15
	movmskps	%xmm2, %edi
	movq	%rdi, %rax
	salq	$4, %rax
	pshufb	(%r12,%rax), %xmm1
	movaps	%xmm1, -112(%rbp)
	call	__popcountdi2@PLT
	movdqa	-112(%rbp), %xmm1
	leaq	-12(%rbx,%r15), %rdx
	movdqa	-160(%rbp), %xmm0
	cltq
	subq	$16, %rbx
	movups	%xmm1, (%r14,%r15,4)
	movups	%xmm1, (%r14,%rdx,4)
	movdqa	%xmm0, %xmm1
	movl	$4, %edx
	pcmpgtd	-80(%rbp), %xmm1
	subq	%rax, %rdx
	addq	%rdx, %r15
	movmskps	%xmm1, %edi
	movq	%rdi, %rax
	salq	$4, %rax
	pshufb	(%r12,%rax), %xmm0
	movaps	%xmm0, -112(%rbp)
	call	__popcountdi2@PLT
	movdqa	-112(%rbp), %xmm0
	movl	$4, %edx
	movq	-96(%rbp), %r8
	cltq
	leaq	(%r15,%rbx), %rcx
	subq	%rax, %rdx
	movups	%xmm0, (%r14,%r15,4)
	movups	%xmm0, (%r14,%rcx,4)
	addq	%rdx, %r15
	cmpq	%r8, %r13
	je	.L1761
.L1567:
	movq	%r13, %rax
	subq	%r14, %rax
	sarq	$2, %rax
	subq	%r15, %rax
	cmpq	$16, %rax
	ja	.L1762
	movdqu	0(%r13), %xmm3
	movdqu	16(%r13), %xmm2
	prefetcht0	256(%r13)
	addq	$64, %r13
	movdqu	-32(%r13), %xmm1
	movdqu	-16(%r13), %xmm0
	jmp	.L1566
	.p2align 4,,10
	.p2align 3
.L1754:
	movq	-176(%rbp), %rbx
	movl	$16, %edx
	subq	%rax, %rdx
	leaq	-16(%rax,%rbx), %rbx
	leaq	(%rdi,%rdx,4), %r14
	movq	%r8, %rax
	jmp	.L1480
	.p2align 4,,10
	.p2align 3
.L1761:
	leaq	(%rbx,%r15), %rax
	movq	-392(%rbp), %r13
	movq	%rax, -128(%rbp)
	leaq	(%r14,%r15,4), %rax
	addq	$4, %r15
	movq	%rax, -144(%rbp)
.L1564:
	movdqa	-272(%rbp), %xmm5
	movdqa	%xmm5, %xmm0
	pcmpgtd	-80(%rbp), %xmm0
	movmskps	%xmm0, %edi
	movdqa	%xmm5, %xmm0
	movq	%rdi, %rax
	salq	$4, %rax
	pshufb	(%r12,%rax), %xmm0
	movaps	%xmm0, -112(%rbp)
	call	__popcountdi2@PLT
	movdqa	-112(%rbp), %xmm0
	movq	-144(%rbp), %rcx
	movdqa	-288(%rbp), %xmm7
	cltq
	movups	%xmm0, (%rcx)
	movq	-128(%rbp), %rcx
	subq	%rax, %r15
	movups	%xmm0, -16(%r14,%rcx,4)
	movdqa	%xmm7, %xmm0
	pcmpgtd	-80(%rbp), %xmm0
	movmskps	%xmm0, %edi
	movdqa	%xmm7, %xmm0
	movq	%rdi, %rax
	salq	$4, %rax
	pshufb	(%r12,%rax), %xmm0
	movaps	%xmm0, -112(%rbp)
	call	__popcountdi2@PLT
	movdqa	-112(%rbp), %xmm0
	leaq	-8(%rbx,%r15), %rcx
	movdqa	-304(%rbp), %xmm5
	cltq
	movups	%xmm0, (%r14,%r15,4)
	subq	%rax, %r15
	movups	%xmm0, (%r14,%rcx,4)
	movdqa	%xmm5, %xmm0
	addq	$4, %r15
	pcmpgtd	-80(%rbp), %xmm0
	movmskps	%xmm0, %edi
	movdqa	%xmm5, %xmm0
	movq	%rdi, %rax
	salq	$4, %rax
	pshufb	(%r12,%rax), %xmm0
	movaps	%xmm0, -112(%rbp)
	call	__popcountdi2@PLT
	movdqa	-112(%rbp), %xmm0
	leaq	-12(%rbx,%r15), %rcx
	movdqa	-320(%rbp), %xmm7
	cltq
	movups	%xmm0, (%r14,%r15,4)
	movups	%xmm0, (%r14,%rcx,4)
	movdqa	%xmm7, %xmm0
	movl	$4, %ecx
	pcmpgtd	-80(%rbp), %xmm0
	subq	%rax, %rcx
	addq	%rcx, %r15
	movmskps	%xmm0, %edi
	movdqa	%xmm7, %xmm0
	movq	%rdi, %rax
	salq	$4, %rax
	pshufb	(%r12,%rax), %xmm0
	movaps	%xmm0, -112(%rbp)
	call	__popcountdi2@PLT
	movdqa	-112(%rbp), %xmm0
	leaq	-16(%rbx,%r15), %rcx
	movdqa	-336(%rbp), %xmm5
	cltq
	movups	%xmm0, (%r14,%r15,4)
	movups	%xmm0, (%r14,%rcx,4)
	movdqa	%xmm5, %xmm0
	movl	$4, %ecx
	pcmpgtd	-80(%rbp), %xmm0
	subq	%rax, %rcx
	addq	%rcx, %r15
	movmskps	%xmm0, %edi
	movdqa	%xmm5, %xmm0
	movq	%rdi, %rax
	salq	$4, %rax
	pshufb	(%r12,%rax), %xmm0
	movaps	%xmm0, -112(%rbp)
	call	__popcountdi2@PLT
	movdqa	-112(%rbp), %xmm0
	leaq	-20(%rbx,%r15), %rcx
	movdqa	-352(%rbp), %xmm5
	cltq
	movups	%xmm0, (%r14,%r15,4)
	movups	%xmm0, (%r14,%rcx,4)
	movdqa	%xmm5, %xmm0
	movl	$4, %ecx
	pcmpgtd	-80(%rbp), %xmm0
	subq	%rax, %rcx
	addq	%rcx, %r15
	movmskps	%xmm0, %edi
	movdqa	%xmm5, %xmm0
	movq	%rdi, %rax
	salq	$4, %rax
	pshufb	(%r12,%rax), %xmm0
	movaps	%xmm0, -112(%rbp)
	call	__popcountdi2@PLT
	movdqa	-112(%rbp), %xmm0
	leaq	-24(%rbx,%r15), %rcx
	movdqa	-368(%rbp), %xmm5
	cltq
	movups	%xmm0, (%r14,%r15,4)
	movups	%xmm0, (%r14,%rcx,4)
	movdqa	%xmm5, %xmm0
	movl	$4, %ecx
	pcmpgtd	-80(%rbp), %xmm0
	subq	%rax, %rcx
	addq	%rcx, %r15
	movmskps	%xmm0, %edi
	movdqa	%xmm5, %xmm0
	movq	%rdi, %rax
	salq	$4, %rax
	pshufb	(%r12,%rax), %xmm0
	movaps	%xmm0, -112(%rbp)
	call	__popcountdi2@PLT
	movdqa	-112(%rbp), %xmm0
	leaq	-28(%rbx,%r15), %rcx
	movdqa	-384(%rbp), %xmm5
	cltq
	movups	%xmm0, (%r14,%r15,4)
	movups	%xmm0, (%r14,%rcx,4)
	movdqa	%xmm5, %xmm0
	movl	$4, %ecx
	pcmpgtd	-80(%rbp), %xmm0
	subq	%rax, %rcx
	addq	%rcx, %r15
	movmskps	%xmm0, %edi
	movdqa	%xmm5, %xmm0
	movq	%rdi, %rax
	salq	$4, %rax
	pshufb	(%r12,%rax), %xmm0
	movaps	%xmm0, -112(%rbp)
	call	__popcountdi2@PLT
	movdqa	-112(%rbp), %xmm0
	leaq	-32(%rbx,%r15), %rcx
	movl	$4, %edx
	cltq
	movups	%xmm0, (%r14,%r15,4)
	subq	%rax, %rdx
	movups	%xmm0, (%r14,%rcx,4)
	movq	-248(%rbp), %rcx
	leaq	(%rdx,%r15), %rbx
	leaq	0(,%rbx,4), %r15
	subq	%rbx, %rcx
.L1563:
	movq	-248(%rbp), %rsi
	movdqa	-224(%rbp), %xmm3
	cmpq	$4, %rcx
	pcmpeqd	%xmm0, %xmm0
	pcmpgtd	-80(%rbp), %xmm3
	leaq	-16(,%rsi,4), %rax
	cmovnb	%r15, %rax
	pxor	%xmm3, %xmm0
	movdqu	(%r14,%rax), %xmm6
	movaps	%xmm3, -80(%rbp)
	movmskps	%xmm0, %r12d
	movups	%xmm6, (%r14,%rsi,4)
	movq	%r12, %rdi
	salq	$4, %r12
	movaps	%xmm6, -112(%rbp)
	call	__popcountdi2@PLT
	movq	-168(%rbp), %rsi
	movdqa	-80(%rbp), %xmm3
	movd	%eax, %xmm6
	movdqa	-224(%rbp), %xmm1
	movslq	%eax, %rcx
	pshufd	$0, %xmm6, %xmm0
	pcmpgtd	-208(%rbp), %xmm0
	pshufb	(%rsi,%r12), %xmm1
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1569
	movd	%xmm1, (%r14,%r15)
.L1569:
	pshufd	$85, %xmm0, %xmm2
	movd	%xmm2, %eax
	testl	%eax, %eax
	je	.L1570
	pshufd	$85, %xmm1, %xmm2
	movd	%xmm2, 4(%r14,%r15)
.L1570:
	movdqa	%xmm0, %xmm2
	punpckhdq	%xmm0, %xmm2
	movd	%xmm2, %eax
	testl	%eax, %eax
	je	.L1571
	movdqa	%xmm1, %xmm2
	punpckhdq	%xmm1, %xmm2
	movd	%xmm2, 8(%r14,%r15)
.L1571:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1572
	pshufd	$255, %xmm1, %xmm1
	movd	%xmm1, 12(%r14,%r15)
.L1572:
	movmskps	%xmm3, %r12d
	addq	%rcx, %rbx
	movq	%r12, %rdi
	salq	$4, %r12
	leaq	0(,%rbx,4), %r15
	call	__popcountdi2@PLT
	movq	-168(%rbp), %rcx
	movdqa	-224(%rbp), %xmm1
	movd	%eax, %xmm6
	pshufd	$0, %xmm6, %xmm0
	pshufb	(%rcx,%r12), %xmm1
	pcmpgtd	-208(%rbp), %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1573
	movd	%xmm1, (%r14,%rbx,4)
.L1573:
	pshufd	$85, %xmm0, %xmm2
	movd	%xmm2, %eax
	testl	%eax, %eax
	je	.L1574
	pshufd	$85, %xmm1, %xmm2
	movd	%xmm2, 4(%r14,%r15)
.L1574:
	movdqa	%xmm0, %xmm2
	punpckhdq	%xmm0, %xmm2
	movd	%xmm2, %eax
	testl	%eax, %eax
	je	.L1575
	movdqa	%xmm1, %xmm2
	punpckhdq	%xmm1, %xmm2
	movd	%xmm2, 8(%r14,%r15)
.L1575:
	pshufd	$255, %xmm0, %xmm0
	movd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1576
	pshufd	$255, %xmm1, %xmm1
	movd	%xmm1, 12(%r14,%r15)
.L1576:
	movq	-192(%rbp), %r12
	addq	-256(%rbp), %rbx
	subq	$1, %r12
	cmpl	$2, -228(%rbp)
	je	.L1578
	movq	-184(%rbp), %r8
	movq	%r12, %r9
	movq	%r13, %rcx
	movq	%rbx, %rdx
	movq	-240(%rbp), %rsi
	movq	-88(%rbp), %rdi
	call	_ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	cmpl	$3, -228(%rbp)
	je	.L1467
.L1578:
	movq	-176(%rbp), %rdx
	movq	-88(%rbp), %rax
	movq	%r12, %r9
	movq	%r13, %rcx
	movq	-184(%rbp), %r8
	movq	-240(%rbp), %rsi
	subq	%rbx, %rdx
	leaq	(%rax,%rbx,4), %rdi
	call	_ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L1467:
	addq	$360, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L1559:
	.cfi_restore_state
	movq	0(%r13), %rcx
	leaq	8(%rax), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%rax)
	movl	%r15d, %ecx
	movq	-8(%r13,%rcx), %rsi
	movq	%rsi, -8(%rax,%rcx)
	subq	%rdi, %rax
	movq	%r13, %rsi
	leal	(%r15,%rax), %ecx
	subq	%rax, %rsi
	shrl	$3, %ecx
	rep movsq
	jmp	.L1560
	.p2align 4,,10
	.p2align 3
.L1555:
	movq	(%rax), %rcx
	leaq	8(%r14), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%r14)
	movl	%r15d, %ecx
	movq	-8(%rax,%rcx), %rsi
	movq	%rsi, -8(%r14,%rcx)
	movq	%r14, %rcx
	movq	%rax, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%r15d, %ecx
	shrl	$3, %ecx
	rep movsq
	jmp	.L1556
.L1755:
	movl	0(%r13), %ecx
	jmp	.L1529
.L1760:
	movzbl	0(%r13), %esi
	movb	%sil, (%rax)
	testb	$2, %cl
	je	.L1560
	movzwl	-2(%r13,%rcx), %esi
	movw	%si, -2(%rax,%rcx)
	jmp	.L1560
.L1758:
	movzbl	(%rax), %esi
	movb	%sil, (%r14)
	testb	$2, %cl
	je	.L1556
	movzwl	-2(%rax,%rcx), %esi
	movw	%si, -2(%r14,%rcx)
	jmp	.L1556
.L1753:
	cmpq	$1, %rdx
	jbe	.L1467
	movq	%rdi, %rax
	addq	$256, %rax
	cmpq	%rax, %rsi
	jb	.L1763
	movl	$4, %esi
	call	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	jmp	.L1467
.L1483:
	movq	-88(%rbp), %rax
	andl	$3, %r15d
	movl	$4, %edi
	movdqa	.LC0(%rip), %xmm7
	subq	%r15, %rdi
	movdqu	(%rax), %xmm5
	movaps	%xmm7, -208(%rbp)
	movaps	%xmm5, -80(%rbp)
	movd	%edi, %xmm5
	movdqa	-80(%rbp), %xmm1
	pshufd	$0, %xmm5, %xmm3
	pcmpeqd	%xmm0, %xmm1
	pcmpgtd	%xmm7, %xmm3
	pandn	%xmm3, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	jne	.L1764
	movq	-88(%rbp), %rax
	pxor	%xmm3, %xmm3
	movq	-176(%rbp), %r8
	pxor	%xmm5, %xmm5
	movdqa	%xmm3, %xmm1
	leaq	256(%rax,%rdi,4), %rsi
	.p2align 4,,10
	.p2align 3
.L1489:
	movq	%rdi, %rcx
	leaq	64(%rdi), %rdi
	cmpq	%rdi, %r8
	jb	.L1765
	leaq	-256(%rsi), %rax
.L1488:
	movdqa	(%rax), %xmm4
	leaq	32(%rax), %rdx
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm1
	movdqa	16(%rax), %xmm4
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm3
	movdqa	32(%rax), %xmm4
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm1
	movdqa	48(%rax), %xmm4
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm3
	movdqa	64(%rax), %xmm4
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm1
	movdqa	80(%rax), %xmm4
	leaq	96(%rdx), %rax
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm3
	movdqa	64(%rdx), %xmm4
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm1
	movdqa	80(%rdx), %xmm4
	pxor	%xmm2, %xmm4
	por	%xmm4, %xmm3
	cmpq	%rsi, %rax
	jne	.L1488
	movdqa	%xmm1, %xmm4
	leaq	352(%rdx), %rsi
	por	%xmm3, %xmm4
	pcmpeqd	%xmm5, %xmm4
	movmskps	%xmm4, %eax
	cmpl	$15, %eax
	je	.L1489
	movq	-88(%rbp), %rax
	movdqa	%xmm0, %xmm1
	pcmpeqd	%xmm2, %xmm2
	movq	-88(%rbp), %rdx
	pcmpeqd	(%rax,%rcx,4), %xmm1
	pxor	%xmm2, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	jne	.L1491
	.p2align 4,,10
	.p2align 3
.L1490:
	addq	$4, %rcx
	movdqa	%xmm0, %xmm1
	pcmpeqd	(%rdx,%rcx,4), %xmm1
	pxor	%xmm2, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	je	.L1490
.L1491:
	rep bsfl	%eax, %eax
	cltq
	addq	%rcx, %rax
.L1487:
	movq	-88(%rbp), %rcx
	leaq	(%rcx,%rax,4), %rdi
	movl	(%rdi), %r12d
	movd	%r12d, %xmm7
	pshufd	$0, %xmm7, %xmm2
	movdqa	%xmm2, %xmm1
	movaps	%xmm2, -80(%rbp)
	pcmpgtd	%xmm0, %xmm1
	movmskps	%xmm1, %edx
	testl	%edx, %edx
	jne	.L1496
	movq	-176(%rbp), %r15
	movl	%r12d, -160(%rbp)
	xorl	%ebx, %ebx
	movq	%r10, -96(%rbp)
	leaq	-4(%r15), %r14
	movaps	%xmm0, -144(%rbp)
	movq	%r14, %r12
	movaps	%xmm0, -112(%rbp)
	movq	%r13, %r14
	movq	%rcx, %r13
	movaps	%xmm2, -128(%rbp)
	jmp	.L1505
	.p2align 4,,10
	.p2align 3
.L1497:
	movdqa	-144(%rbp), %xmm7
	movmskps	%xmm1, %edi
	movups	%xmm7, 0(%r13,%r12,4)
	call	__popcountdi2@PLT
	cltq
	addq	%rax, %rbx
	leaq	-4(%r12), %rax
	cmpq	%rax, %r15
	jbe	.L1766
	movq	%rax, %r12
.L1505:
	movdqu	0(%r13,%r12,4), %xmm1
	movdqu	0(%r13,%r12,4), %xmm4
	pcmpeqd	-112(%rbp), %xmm1
	pcmpeqd	-128(%rbp), %xmm4
	movdqa	%xmm1, %xmm5
	movdqa	%xmm1, %xmm6
	por	%xmm4, %xmm5
	movmskps	%xmm5, %eax
	cmpl	$15, %eax
	je	.L1497
	pcmpeqd	%xmm1, %xmm1
	movq	-88(%rbp), %rsi
	movq	%r14, %r13
	movq	%r12, %r14
	pxor	%xmm1, %xmm6
	movq	-176(%rbp), %rdx
	movdqa	-112(%rbp), %xmm0
	leaq	4(%r14), %rcx
	pandn	%xmm6, %xmm4
	movdqa	-144(%rbp), %xmm3
	movdqa	-128(%rbp), %xmm2
	movmskps	%xmm4, %eax
	subq	%rbx, %rdx
	movl	-160(%rbp), %r12d
	rep bsfl	%eax, %eax
	cltq
	addq	%r14, %rax
	movd	(%rsi,%rax,4), %xmm7
	leaq	8(%r14), %rax
	pshufd	$0, %xmm7, %xmm1
	movdqa	%xmm1, %xmm4
	movaps	%xmm1, -64(%rbp)
	cmpq	%rax, %rdx
	jb	.L1498
.L1499:
	movdqa	-80(%rbp), %xmm6
	movq	%rax, %rcx
	movups	%xmm6, -16(%rsi,%rax,4)
	addq	$4, %rax
	cmpq	%rax, %rdx
	jnb	.L1499
.L1498:
	subq	%rcx, %rdx
	leaq	0(,%rcx,4), %rsi
	movq	%rdx, %xmm1
	pshufd	$0, %xmm1, %xmm1
	pcmpgtd	-208(%rbp), %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L1500
	movq	-88(%rbp), %rax
	movl	%r12d, (%rax,%rcx,4)
.L1500:
	pshufd	$85, %xmm1, %xmm5
	movd	%xmm5, %eax
	testl	%eax, %eax
	je	.L1501
	movq	-88(%rbp), %rax
	pshufd	$85, %xmm2, %xmm5
	movd	%xmm5, 4(%rax,%rsi)
.L1501:
	movdqa	%xmm1, %xmm5
	punpckhdq	%xmm1, %xmm5
	movd	%xmm5, %eax
	testl	%eax, %eax
	je	.L1502
	movq	-88(%rbp), %rax
	movdqa	%xmm2, %xmm5
	punpckhdq	%xmm2, %xmm5
	movd	%xmm5, 8(%rax,%rsi)
.L1502:
	pshufd	$255, %xmm1, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L1504
	movq	-88(%rbp), %rax
	pshufd	$255, %xmm2, %xmm1
	movd	%xmm1, 12(%rax,%rsi)
.L1504:
	movdqa	%xmm0, %xmm1
	pcmpeqd	.LC5(%rip), %xmm1
	movmskps	%xmm1, %eax
	cmpl	$15, %eax
	je	.L1596
	movdqa	%xmm0, %xmm1
	pcmpeqd	.LC6(%rip), %xmm1
	movmskps	%xmm1, %eax
	cmpl	$15, %eax
	je	.L1522
	movdqa	%xmm4, %xmm1
	pcmpgtd	%xmm2, %xmm1
	movdqa	%xmm1, %xmm6
	movdqa	%xmm1, %xmm5
	pandn	%xmm4, %xmm6
	pand	%xmm2, %xmm5
	por	%xmm6, %xmm5
	movdqa	%xmm0, %xmm6
	pcmpgtd	%xmm5, %xmm6
	movmskps	%xmm6, %eax
	testl	%eax, %eax
	jne	.L1767
	movdqa	%xmm3, %xmm1
	movl	$64, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L1517:
	movq	-88(%rbp), %rbx
	leaq	(%rcx,%rax,4), %rdx
	addq	$1, %rax
	movdqu	(%rbx,%rdx,4), %xmm4
	movdqa	%xmm4, %xmm2
	pcmpgtd	%xmm1, %xmm2
	movdqa	%xmm2, %xmm5
	pand	%xmm2, %xmm1
	pandn	%xmm4, %xmm5
	por	%xmm5, %xmm1
	cmpq	$16, %rax
	jne	.L1517
	movdqa	%xmm0, %xmm2
	pcmpgtd	%xmm1, %xmm2
	movmskps	%xmm2, %eax
	testl	%eax, %eax
	jne	.L1752
	leaq	64(%rsi), %rax
	cmpq	%rax, -176(%rbp)
	jb	.L1768
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L1517
.L1599:
	movdqa	.LC0(%rip), %xmm7
	leaq	_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %rcx
	xorl	%r15d, %r15d
	xorl	%r12d, %r12d
	movq	%rcx, -168(%rbp)
	movaps	%xmm7, -208(%rbp)
	jmp	.L1536
.L1600:
	xorl	%r15d, %r15d
	jmp	.L1563
.L1601:
	movq	%r14, -144(%rbp)
	movl	$4, %r15d
	leaq	_ZZN3hwy7N_SSSE36detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %r12
	movq	%rbx, -128(%rbp)
	jmp	.L1564
.L1756:
	movq	-176(%rbp), %rsi
	movq	-88(%rbp), %rdi
	leaq	-1(%rsi), %rbx
	movq	%rbx, %r12
	shrq	%r12
	.p2align 4,,10
	.p2align 3
.L1534:
	movq	%r12, %rdx
	call	_ZN3hwy7N_SSSE36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	subq	$1, %r12
	jnb	.L1534
	.p2align 4,,10
	.p2align 3
.L1535:
	movl	(%rdi,%rbx,4), %edx
	movl	(%rdi), %eax
	movq	%rbx, %rsi
	movl	%edx, (%rdi)
	xorl	%edx, %edx
	movl	%eax, (%rdi,%rbx,4)
	call	_ZN3hwy7N_SSSE36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	subq	$1, %rbx
	jne	.L1535
	jmp	.L1467
.L1759:
	movl	0(%r13), %esi
	movl	%esi, (%rax)
	movl	-4(%r13,%rcx), %esi
	movl	%esi, -4(%rax,%rcx)
	jmp	.L1560
.L1757:
	movl	(%rax), %esi
	movl	%esi, (%r14)
	movl	-4(%rax,%rcx), %esi
	movl	%esi, -4(%r14,%rcx)
	jmp	.L1556
.L1765:
	movq	-88(%rbp), %rsi
	movq	-176(%rbp), %rdi
	pcmpeqd	%xmm2, %xmm2
.L1493:
	movq	%rcx, %rdx
	addq	$4, %rcx
	cmpq	%rcx, %rdi
	jb	.L1769
	movdqa	%xmm0, %xmm1
	pcmpeqd	-16(%rsi,%rcx,4), %xmm1
	pxor	%xmm2, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	je	.L1493
.L1750:
	rep bsfl	%eax, %eax
	cltq
	addq	%rdx, %rax
	jmp	.L1487
.L1496:
	leaq	-64(%rbp), %rdx
	movq	%r13, %rcx
	movdqa	%xmm2, %xmm1
	movaps	%xmm2, -80(%rbp)
	movq	-176(%rbp), %rsi
	subq	%rax, %rsi
	call	_ZN3hwy7N_SSSE36detail22MaybePartitionTwoValueINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L1467
	movd	0(%r13), %xmm7
	movdqa	-64(%rbp), %xmm4
	movdqa	-80(%rbp), %xmm2
	pshufd	$0, %xmm7, %xmm0
	movdqa	%xmm0, %xmm3
	jmp	.L1504
.L1766:
	movq	-88(%rbp), %rax
	movq	%r14, %r13
	movq	%r12, %r14
	movdqa	-112(%rbp), %xmm0
	movd	%r14d, %xmm6
	movdqa	-128(%rbp), %xmm2
	movq	-96(%rbp), %r10
	movdqu	(%rax), %xmm7
	pshufd	$0, %xmm6, %xmm4
	movq	-176(%rbp), %r15
	pcmpgtd	-208(%rbp), %xmm4
	movdqa	-144(%rbp), %xmm3
	movl	-160(%rbp), %r12d
	movaps	%xmm7, -112(%rbp)
	movdqa	-112(%rbp), %xmm1
	movdqa	-112(%rbp), %xmm5
	subq	%rbx, %r15
	pcmpeqd	%xmm0, %xmm1
	pcmpeqd	%xmm2, %xmm5
	movdqa	%xmm4, %xmm6
	pand	%xmm1, %xmm6
	por	%xmm5, %xmm1
	pcmpeqd	%xmm5, %xmm5
	pxor	%xmm5, %xmm4
	por	%xmm4, %xmm1
	movmskps	%xmm1, %eax
	cmpl	$15, %eax
	jne	.L1770
	movmskps	%xmm6, %edi
	movaps	%xmm2, -128(%rbp)
	movaps	%xmm3, -112(%rbp)
	movq	%r10, -144(%rbp)
	call	__popcountdi2@PLT
	movq	-88(%rbp), %rbx
	movdqa	-112(%rbp), %xmm3
	movslq	%eax, %rdx
	movq	%r15, %rax
	movdqa	-128(%rbp), %xmm2
	subq	%rdx, %rax
	movups	%xmm3, (%rbx)
	cmpq	$3, %rax
	jbe	.L1585
	leaq	-4(%rax), %rdx
	movq	-144(%rbp), %r10
	movq	%rdx, %rcx
	shrq	$2, %rcx
	salq	$4, %rcx
	leaq	16(%rbx,%rcx), %rcx
.L1514:
	movdqa	-80(%rbp), %xmm7
	addq	$16, %r10
	movups	%xmm7, -16(%r10)
	cmpq	%rcx, %r10
	jne	.L1514
	andq	$-4, %rdx
	addq	$4, %rdx
	leaq	0(,%rdx,4), %rcx
	subq	%rdx, %rax
.L1513:
	movaps	%xmm2, 0(%r13)
	testq	%rax, %rax
	je	.L1467
	movq	-88(%rbp), %rdi
	leaq	0(,%rax,4), %rdx
	movq	%r13, %rsi
	addq	%rcx, %rdi
	call	memcpy@PLT
	jmp	.L1467
.L1768:
	movq	-176(%rbp), %rcx
	movq	%rbx, %rdx
	jmp	.L1524
.L1525:
	movdqu	-16(%rdx,%rsi,4), %xmm6
	movdqa	%xmm0, %xmm1
	pcmpgtd	%xmm6, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	jne	.L1752
.L1524:
	movq	%rsi, %rax
	addq	$4, %rsi
	cmpq	%rsi, %rcx
	jnb	.L1525
	movq	-176(%rbp), %rbx
	cmpq	%rax, %rbx
	je	.L1596
	movq	-88(%rbp), %rax
	movdqu	-16(%rax,%rbx,4), %xmm7
	movaps	%xmm7, -80(%rbp)
	pcmpgtd	-80(%rbp), %xmm0
	movmskps	%xmm0, %eax
	cmpl	$1, %eax
	movl	$1, %eax
	adcl	$0, %eax
	movl	%eax, -228(%rbp)
	jmp	.L1526
.L1769:
	movq	-176(%rbp), %rax
	pcmpeqd	%xmm2, %xmm2
	leaq	-4(%rax), %rdx
	movq	-88(%rbp), %rax
	movdqu	(%rax,%rdx,4), %xmm6
	movaps	%xmm6, -80(%rbp)
	movdqa	-80(%rbp), %xmm1
	pcmpeqd	%xmm0, %xmm1
	pxor	%xmm2, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	je	.L1467
	jmp	.L1750
.L1771:
	movq	%rbx, %rdx
	jmp	.L1520
.L1521:
	movdqu	-16(%rdx,%rsi,4), %xmm1
	pcmpgtd	%xmm0, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	jne	.L1752
.L1520:
	movq	%rsi, %rax
	addq	$4, %rsi
	cmpq	%rsi, -176(%rbp)
	jnb	.L1521
	movq	-176(%rbp), %rbx
	cmpq	%rax, %rbx
	je	.L1522
	movq	-88(%rbp), %rax
	movdqu	-16(%rax,%rbx,4), %xmm6
	movaps	%xmm6, -80(%rbp)
	movdqa	-80(%rbp), %xmm1
	pcmpgtd	%xmm0, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	jne	.L1752
.L1522:
	movl	$3, -228(%rbp)
	pcmpeqd	%xmm3, %xmm3
	paddd	%xmm0, %xmm3
	jmp	.L1526
.L1596:
	movl	$2, -228(%rbp)
	jmp	.L1526
.L1764:
	rep bsfl	%eax, %eax
	cltq
	jmp	.L1487
.L1763:
	movq	%rdx, %rcx
	xorl	%eax, %eax
	cmpq	$3, %rdx
	jbe	.L1473
	movq	%rdx, %r10
	leaq	-4(%rdx), %rdx
	movq	(%rdi), %rcx
	movq	%rdi, %rbx
	movq	%rdx, %rax
	shrq	$2, %rax
	movq	%rcx, 0(%r13)
	addq	$1, %rax
	salq	$4, %rax
	movl	%eax, %ecx
	movq	-8(%rdi,%rcx), %rsi
	leaq	8(%r13), %rdi
	andq	$-8, %rdi
	movq	%rsi, -8(%r13,%rcx)
	movq	%r13, %rcx
	subq	%rdi, %rcx
	subq	%rcx, %rbx
	addl	%eax, %ecx
	movq	%rdx, %rax
	andq	$-4, %rax
	movq	%rbx, %rsi
	shrl	$3, %ecx
	rep movsq
	addq	$4, %rax
	movq	%r10, %rcx
	subq	%rax, %rcx
	je	.L1476
.L1473:
	salq	$2, %rax
	leaq	0(,%rcx,4), %rdx
	testq	%rcx, %rcx
	movq	-88(%rbp), %rbx
	movl	$4, %ecx
	leaq	0(%r13,%rax), %rdi
	cmove	%rcx, %rdx
	leaq	(%rbx,%rax), %rsi
	call	memcpy@PLT
.L1476:
	movq	-176(%rbp), %rbx
	movl	$32, %ecx
	movl	%ebx, %eax
	subl	$1, %eax
	bsrl	%eax, %eax
	xorl	$31, %eax
	subl	%eax, %ecx
	movl	$1, %eax
	salq	%cl, %rax
	shrq	$4, %rax
	movq	%rax, %rsi
	movl	$1, %eax
	cmove	%rax, %rsi
	movq	%rsi, %rdx
	salq	$4, %rdx
	addq	$4, %rdx
	cmpq	%rdx, %rbx
	jnb	.L1475
	movdqa	.LC4(%rip), %xmm0
	movq	%rbx, %rax
.L1474:
	movups	%xmm0, 0(%r13,%rax,4)
	addq	$4, %rax
	cmpq	%rdx, %rax
	jb	.L1474
.L1475:
	movq	%r13, %rdi
	call	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	xorl	%eax, %eax
	cmpq	$3, -176(%rbp)
	jbe	.L1478
	movq	-176(%rbp), %r10
	movq	0(%r13), %rcx
	movq	-88(%rbp), %rbx
	leaq	-4(%r10), %rdx
	movq	%rdx, %rax
	movq	%rcx, (%rbx)
	leaq	8(%rbx), %rdi
	shrq	$2, %rax
	andq	$-8, %rdi
	addq	$1, %rax
	salq	$4, %rax
	movl	%eax, %ecx
	movq	-8(%r13,%rcx), %rsi
	movq	%rsi, -8(%rbx,%rcx)
	subq	%rdi, %rbx
	movq	%r13, %rsi
	movq	%rbx, %rcx
	subq	%rbx, %rsi
	addl	%eax, %ecx
	movq	%rdx, %rax
	andq	$-4, %rax
	shrl	$3, %ecx
	rep movsq
	addq	$4, %rax
	subq	%rax, %r10
	movq	%r10, -176(%rbp)
	je	.L1467
.L1478:
	movq	-176(%rbp), %rbx
	movq	-88(%rbp), %rdi
	salq	$2, %rax
	movl	$4, %ecx
	leaq	0(%r13,%rax), %rsi
	addq	%rax, %rdi
	leaq	0(,%rbx,4), %rdx
	testq	%rbx, %rbx
	cmove	%rcx, %rdx
	call	memcpy@PLT
	jmp	.L1467
.L1767:
	movdqa	%xmm1, %xmm5
	pand	%xmm4, %xmm1
	pandn	%xmm2, %xmm5
	por	%xmm5, %xmm1
	pcmpgtd	%xmm0, %xmm1
	movmskps	%xmm1, %eax
	testl	%eax, %eax
	jne	.L1752
	movdqa	%xmm3, %xmm1
	movl	$64, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L1518:
	movq	-88(%rbp), %rbx
	leaq	(%rcx,%rax,4), %rdx
	addq	$1, %rax
	movdqu	(%rbx,%rdx,4), %xmm2
	movdqa	%xmm2, %xmm4
	pcmpgtd	%xmm1, %xmm4
	movdqa	%xmm4, %xmm5
	pand	%xmm4, %xmm2
	pandn	%xmm1, %xmm5
	movdqa	%xmm2, %xmm1
	por	%xmm5, %xmm1
	cmpq	$16, %rax
	jne	.L1518
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm0, %xmm2
	movmskps	%xmm2, %eax
	testl	%eax, %eax
	jne	.L1752
	leaq	64(%rsi), %rax
	cmpq	%rax, -176(%rbp)
	jb	.L1771
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L1518
.L1770:
	pxor	%xmm5, %xmm1
	movq	-88(%rbp), %rbx
	movmskps	%xmm1, %eax
	rep bsfl	%eax, %eax
	cltq
	movd	(%rbx,%rax,4), %xmm6
	leaq	4(%r14), %rax
	pshufd	$0, %xmm6, %xmm1
	movdqa	%xmm1, %xmm4
	movaps	%xmm1, -64(%rbp)
	cmpq	%r15, %rax
	ja	.L1507
.L1508:
	movq	-88(%rbp), %rbx
	movdqa	-80(%rbp), %xmm7
	movq	%rax, %r14
	movups	%xmm7, -16(%rbx,%rax,4)
	addq	$4, %rax
	cmpq	%r15, %rax
	jbe	.L1508
.L1507:
	subq	%r14, %r15
	leaq	0(,%r14,4), %rdx
	movq	%r15, %xmm1
	pshufd	$0, %xmm1, %xmm1
	pcmpgtd	-208(%rbp), %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L1509
	movq	-88(%rbp), %rax
	movl	%r12d, (%rax,%r14,4)
.L1509:
	pshufd	$85, %xmm1, %xmm5
	movd	%xmm5, %eax
	testl	%eax, %eax
	je	.L1510
	movq	-88(%rbp), %rax
	pshufd	$85, %xmm2, %xmm5
	movd	%xmm5, 4(%rax,%rdx)
.L1510:
	movdqa	%xmm1, %xmm5
	punpckhdq	%xmm1, %xmm5
	movd	%xmm5, %eax
	testl	%eax, %eax
	je	.L1511
	movq	-88(%rbp), %rax
	movdqa	%xmm2, %xmm5
	punpckhdq	%xmm2, %xmm5
	movd	%xmm5, 8(%rax,%rdx)
.L1511:
	pshufd	$255, %xmm1, %xmm1
	movd	%xmm1, %eax
	testl	%eax, %eax
	je	.L1504
	movq	-88(%rbp), %rax
	pshufd	$255, %xmm2, %xmm1
	movd	%xmm1, 12(%rax,%rdx)
	jmp	.L1504
.L1585:
	xorl	%ecx, %ecx
	jmp	.L1513
	.cfi_endproc
.LFE18806:
	.size	_ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0, .-_ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	.section	.text._ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0, @function
_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0:
.LFB18808:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %rax
	salq	$2, %rax
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	.cfi_offset 15, -24
	leaq	(%rdi,%rax), %r15
	pushq	%r14
	.cfi_offset 14, -32
	leaq	(%r15,%rax), %r14
	pushq	%r13
	.cfi_offset 13, -40
	leaq	(%r14,%rax), %r13
	pushq	%r12
	.cfi_offset 12, -48
	leaq	0(%r13,%rax), %r12
	pushq	%rbx
	.cfi_offset 3, -56
	leaq	(%r12,%rax), %rbx
	leaq	(%rbx,%rax), %r11
	leaq	(%r11,%rax), %r10
	leaq	(%r10,%rax), %r9
	andq	$-32, %rsp
	subq	$200, %rsp
	leaq	(%r9,%rax), %r8
	movq	%rdi, 176(%rsp)
	leaq	(%r8,%rax), %rdi
	movq	%rsi, 136(%rsp)
	leaq	(%rdi,%rax), %rsi
	leaq	(%rsi,%rax), %rcx
	leaq	(%rcx,%rax), %rdx
	movq	%rdx, 184(%rsp)
	addq	%rax, %rdx
	addq	%rdx, %rax
	movq	%rdx, 192(%rsp)
	movq	176(%rsp), %rdx
	vmovdqu	(%rdx), %ymm5
	vpmaxsd	(%r15), %ymm5, %ymm9
	vpminsd	(%r15), %ymm5, %ymm15
	vmovdqu	(%r14), %ymm5
	vpminsd	0(%r13), %ymm5, %ymm14
	vpmaxsd	0(%r13), %ymm5, %ymm8
	vmovdqu	(%r12), %ymm5
	vpminsd	(%rbx), %ymm5, %ymm13
	vpmaxsd	(%rbx), %ymm5, %ymm1
	vmovdqu	(%r11), %ymm5
	vpmaxsd	(%r10), %ymm5, %ymm7
	vpminsd	(%r10), %ymm5, %ymm3
	vmovdqu	(%r9), %ymm5
	vpmaxsd	(%r8), %ymm5, %ymm6
	vpminsd	(%r8), %ymm5, %ymm12
	vmovdqu	(%rdi), %ymm5
	vpminsd	(%rsi), %ymm5, %ymm11
	vpmaxsd	(%rsi), %ymm5, %ymm5
	movq	184(%rsp), %rdx
	cmpq	$1, 136(%rsp)
	vmovdqu	(%rdx), %ymm2
	movq	192(%rsp), %rdx
	vmovdqa	%ymm2, 104(%rsp)
	vmovdqa	104(%rsp), %ymm2
	vpminsd	(%rcx), %ymm2, %ymm4
	vpmaxsd	(%rcx), %ymm2, %ymm0
	vmovdqu	(%rdx), %ymm2
	vmovdqa	%ymm4, 72(%rsp)
	vmovdqa	%ymm2, 104(%rsp)
	vmovdqa	104(%rsp), %ymm10
	vpmaxsd	(%rax), %ymm10, %ymm4
	vpminsd	%ymm14, %ymm15, %ymm10
	vpmaxsd	%ymm14, %ymm15, %ymm15
	vpminsd	%ymm8, %ymm9, %ymm14
	vmovdqa	104(%rsp), %ymm2
	vpmaxsd	%ymm8, %ymm9, %ymm8
	vpminsd	(%rax), %ymm2, %ymm2
	vmovdqa	%ymm8, 104(%rsp)
	vpminsd	%ymm7, %ymm1, %ymm9
	vpminsd	%ymm3, %ymm13, %ymm8
	vpmaxsd	%ymm7, %ymm1, %ymm1
	vpmaxsd	%ymm3, %ymm13, %ymm3
	vpminsd	%ymm11, %ymm12, %ymm7
	vpmaxsd	%ymm5, %ymm6, %ymm13
	vpmaxsd	%ymm11, %ymm12, %ymm12
	vpminsd	%ymm5, %ymm6, %ymm11
	vmovdqa	72(%rsp), %ymm6
	vpminsd	%ymm6, %ymm2, %ymm5
	vpmaxsd	%ymm6, %ymm2, %ymm2
	vpminsd	%ymm4, %ymm0, %ymm6
	vpmaxsd	%ymm4, %ymm0, %ymm0
	vpminsd	%ymm8, %ymm10, %ymm4
	vpmaxsd	%ymm8, %ymm10, %ymm10
	vpminsd	%ymm9, %ymm14, %ymm8
	vpmaxsd	%ymm9, %ymm14, %ymm14
	vpminsd	%ymm3, %ymm15, %ymm9
	vpmaxsd	%ymm3, %ymm15, %ymm15
	vpminsd	104(%rsp), %ymm1, %ymm3
	vpmaxsd	104(%rsp), %ymm1, %ymm1
	vmovdqa	%ymm1, 104(%rsp)
	vpminsd	%ymm5, %ymm7, %ymm1
	vpmaxsd	%ymm5, %ymm7, %ymm5
	vpminsd	%ymm6, %ymm11, %ymm7
	vpmaxsd	%ymm6, %ymm11, %ymm6
	vpminsd	%ymm2, %ymm12, %ymm11
	vpmaxsd	%ymm2, %ymm12, %ymm12
	vpminsd	%ymm0, %ymm13, %ymm2
	vpmaxsd	%ymm0, %ymm13, %ymm0
	vpminsd	%ymm1, %ymm4, %ymm13
	vpmaxsd	%ymm1, %ymm4, %ymm1
	vpminsd	%ymm12, %ymm15, %ymm4
	vpmaxsd	%ymm12, %ymm15, %ymm12
	vmovdqa	104(%rsp), %ymm15
	vmovdqa	%ymm13, 72(%rsp)
	vpminsd	%ymm7, %ymm8, %ymm13
	vpmaxsd	%ymm7, %ymm8, %ymm7
	vpminsd	%ymm11, %ymm9, %ymm8
	vpmaxsd	%ymm11, %ymm9, %ymm11
	vpminsd	%ymm2, %ymm3, %ymm9
	vpmaxsd	%ymm2, %ymm3, %ymm2
	vpminsd	%ymm5, %ymm10, %ymm3
	vpmaxsd	%ymm5, %ymm10, %ymm5
	vpminsd	%ymm6, %ymm14, %ymm10
	vpmaxsd	%ymm6, %ymm14, %ymm6
	vpminsd	%ymm15, %ymm0, %ymm14
	vpmaxsd	%ymm15, %ymm0, %ymm0
	vmovdqa	%ymm0, 104(%rsp)
	vpminsd	%ymm11, %ymm10, %ymm0
	vpmaxsd	%ymm11, %ymm10, %ymm10
	vpminsd	%ymm7, %ymm4, %ymm11
	vpmaxsd	%ymm7, %ymm4, %ymm4
	vpminsd	%ymm2, %ymm14, %ymm7
	vpmaxsd	%ymm2, %ymm14, %ymm14
	vpminsd	%ymm1, %ymm3, %ymm2
	vpmaxsd	%ymm1, %ymm3, %ymm3
	vpminsd	%ymm8, %ymm13, %ymm1
	vpminsd	%ymm5, %ymm9, %ymm15
	vpmaxsd	%ymm8, %ymm13, %ymm8
	vpmaxsd	%ymm5, %ymm9, %ymm5
	vpminsd	%ymm12, %ymm6, %ymm9
	vpmaxsd	%ymm12, %ymm6, %ymm12
	vpminsd	%ymm2, %ymm1, %ymm6
	vpmaxsd	%ymm2, %ymm1, %ymm1
	vmovdqa	%ymm6, 40(%rsp)
	vpminsd	%ymm3, %ymm8, %ymm6
	vpmaxsd	%ymm3, %ymm8, %ymm8
	vpmaxsd	%ymm12, %ymm14, %ymm3
	vpminsd	%ymm9, %ymm7, %ymm2
	vmovdqa	%ymm3, 8(%rsp)
	vpminsd	%ymm1, %ymm6, %ymm3
	vpmaxsd	%ymm9, %ymm7, %ymm9
	vpminsd	%ymm5, %ymm2, %ymm13
	vpminsd	%ymm12, %ymm14, %ymm7
	vmovdqa	%ymm3, -24(%rsp)
	vpminsd	%ymm11, %ymm0, %ymm12
	vpmaxsd	%ymm11, %ymm0, %ymm14
	vpminsd	%ymm10, %ymm4, %ymm3
	vpminsd	%ymm8, %ymm15, %ymm11
	vpmaxsd	%ymm5, %ymm2, %ymm5
	vpmaxsd	%ymm1, %ymm6, %ymm6
	vpmaxsd	%ymm10, %ymm4, %ymm4
	vpmaxsd	%ymm8, %ymm15, %ymm1
	vpminsd	%ymm12, %ymm11, %ymm2
	vpminsd	%ymm5, %ymm4, %ymm8
	vpmaxsd	%ymm12, %ymm11, %ymm11
	vpminsd	%ymm3, %ymm13, %ymm15
	vpminsd	%ymm1, %ymm14, %ymm12
	vpmaxsd	%ymm3, %ymm13, %ymm13
	vpmaxsd	%ymm1, %ymm14, %ymm1
	vpminsd	%ymm9, %ymm7, %ymm10
	vpminsd	%ymm1, %ymm15, %ymm3
	vpmaxsd	%ymm5, %ymm4, %ymm4
	vpmaxsd	%ymm1, %ymm15, %ymm15
	vpminsd	%ymm8, %ymm13, %ymm0
	vpminsd	%ymm12, %ymm11, %ymm5
	vpmaxsd	%ymm12, %ymm11, %ymm11
	vpminsd	%ymm4, %ymm10, %ymm1
	vpmaxsd	%ymm9, %ymm7, %ymm7
	vpmaxsd	%ymm4, %ymm10, %ymm4
	vpminsd	%ymm6, %ymm2, %ymm9
	vpminsd	%ymm3, %ymm11, %ymm10
	vpminsd	%ymm0, %ymm15, %ymm14
	vpmaxsd	%ymm6, %ymm2, %ymm2
	vpmaxsd	%ymm8, %ymm13, %ymm13
	vpmaxsd	%ymm3, %ymm11, %ymm11
	vpmaxsd	%ymm0, %ymm15, %ymm15
	jbe	.L1778
	vmovdqa	72(%rsp), %ymm3
	vpshufd	$177, %ymm14, %ymm14
	vpshufd	$177, %ymm15, %ymm15
	vpshufd	$177, 104(%rsp), %ymm6
	vpshufd	$177, %ymm13, %ymm13
	vpshufd	$177, %ymm1, %ymm1
	vpshufd	$177, %ymm4, %ymm4
	vpshufd	$177, 8(%rsp), %ymm8
	vpminsd	%ymm3, %ymm6, %ymm12
	vpmaxsd	%ymm3, %ymm6, %ymm6
	vmovdqa	40(%rsp), %ymm3
	vpshufd	$177, %ymm7, %ymm7
	cmpq	$3, 136(%rsp)
	vmovdqa	%ymm6, 104(%rsp)
	vpminsd	%ymm3, %ymm8, %ymm0
	vpmaxsd	%ymm3, %ymm8, %ymm8
	vmovdqa	-24(%rsp), %ymm3
	vpshufd	$177, %ymm8, %ymm8
	vpminsd	%ymm3, %ymm7, %ymm6
	vpmaxsd	%ymm3, %ymm7, %ymm7
	vpminsd	%ymm4, %ymm9, %ymm3
	vpmaxsd	%ymm4, %ymm9, %ymm9
	vmovdqa	%ymm6, 72(%rsp)
	vpshufd	$177, %ymm7, %ymm7
	vpminsd	%ymm1, %ymm2, %ymm4
	vpmaxsd	%ymm1, %ymm2, %ymm2
	vmovdqa	%ymm3, 40(%rsp)
	vmovdqa	72(%rsp), %ymm3
	vpminsd	%ymm13, %ymm5, %ymm1
	vpmaxsd	%ymm13, %ymm5, %ymm5
	vpshufd	$177, %ymm4, %ymm4
	vpshufd	$177, 104(%rsp), %ymm6
	vpminsd	%ymm15, %ymm10, %ymm13
	vpmaxsd	%ymm15, %ymm10, %ymm10
	vpshufd	$177, %ymm1, %ymm1
	vpminsd	%ymm14, %ymm11, %ymm15
	vpmaxsd	%ymm14, %ymm11, %ymm11
	vpshufd	$177, %ymm13, %ymm13
	vpshufd	$177, %ymm15, %ymm15
	vpshufd	$177, %ymm9, %ymm9
	vpminsd	%ymm15, %ymm12, %ymm14
	vpmaxsd	%ymm15, %ymm12, %ymm12
	vpminsd	%ymm6, %ymm11, %ymm15
	vpmaxsd	%ymm6, %ymm11, %ymm11
	vpshufd	$177, %ymm12, %ymm12
	vmovdqa	%ymm15, 104(%rsp)
	vpminsd	%ymm8, %ymm10, %ymm15
	vpmaxsd	%ymm8, %ymm10, %ymm10
	vpshufd	$177, %ymm11, %ymm11
	vpminsd	%ymm3, %ymm1, %ymm8
	vpmaxsd	%ymm3, %ymm1, %ymm3
	vpshufd	$177, %ymm10, %ymm10
	vpminsd	%ymm7, %ymm5, %ymm1
	vpmaxsd	%ymm7, %ymm5, %ymm5
	vmovdqa	40(%rsp), %ymm7
	vpshufd	$177, %ymm8, %ymm8
	vpminsd	%ymm13, %ymm0, %ymm6
	vpmaxsd	%ymm13, %ymm0, %ymm0
	vpshufd	$177, %ymm1, %ymm1
	vpminsd	%ymm7, %ymm4, %ymm13
	vpmaxsd	%ymm7, %ymm4, %ymm4
	vpshufd	$177, %ymm0, %ymm0
	vpshufd	$177, %ymm13, %ymm13
	vpminsd	%ymm9, %ymm2, %ymm7
	vpmaxsd	%ymm9, %ymm2, %ymm2
	vpminsd	%ymm13, %ymm14, %ymm9
	vpmaxsd	%ymm13, %ymm14, %ymm14
	vpshufd	$177, %ymm7, %ymm7
	vpminsd	%ymm8, %ymm6, %ymm13
	vpmaxsd	%ymm8, %ymm6, %ymm6
	vpshufd	$177, %ymm14, %ymm14
	vpminsd	%ymm12, %ymm4, %ymm8
	vpmaxsd	%ymm12, %ymm4, %ymm4
	vmovdqa	104(%rsp), %ymm12
	vmovdqa	%ymm13, 72(%rsp)
	vpminsd	%ymm0, %ymm3, %ymm13
	vpmaxsd	%ymm0, %ymm3, %ymm3
	vpshufd	$177, %ymm4, %ymm4
	vpminsd	%ymm12, %ymm7, %ymm0
	vpmaxsd	%ymm12, %ymm7, %ymm7
	vpshufd	$177, %ymm13, %ymm13
	vpminsd	%ymm1, %ymm15, %ymm12
	vpmaxsd	%ymm1, %ymm15, %ymm1
	vpshufd	$177, %ymm7, %ymm7
	vpminsd	%ymm11, %ymm2, %ymm15
	vpshufd	$177, %ymm12, %ymm12
	vpmaxsd	%ymm11, %ymm2, %ymm2
	vmovdqa	%ymm15, 104(%rsp)
	vpminsd	%ymm10, %ymm5, %ymm11
	vpmaxsd	%ymm10, %ymm5, %ymm5
	vpshufd	$177, 72(%rsp), %ymm15
	vpminsd	%ymm15, %ymm9, %ymm10
	vpmaxsd	%ymm15, %ymm9, %ymm9
	vpshufd	$177, %ymm2, %ymm2
	vpminsd	%ymm14, %ymm6, %ymm15
	vpmaxsd	%ymm14, %ymm6, %ymm6
	vpshufd	$177, %ymm11, %ymm11
	vpminsd	%ymm13, %ymm8, %ymm14
	vpmaxsd	%ymm13, %ymm8, %ymm8
	vpminsd	%ymm4, %ymm3, %ymm13
	vpmaxsd	%ymm4, %ymm3, %ymm3
	vpminsd	%ymm12, %ymm0, %ymm4
	vpmaxsd	%ymm12, %ymm0, %ymm0
	vmovdqa	%ymm4, 72(%rsp)
	vmovdqa	104(%rsp), %ymm4
	vpminsd	%ymm7, %ymm1, %ymm12
	vpmaxsd	%ymm7, %ymm1, %ymm1
	vpminsd	%ymm4, %ymm11, %ymm7
	vpmaxsd	%ymm4, %ymm11, %ymm11
	vpminsd	%ymm2, %ymm5, %ymm4
	vpmaxsd	%ymm2, %ymm5, %ymm5
	vpshufd	$177, %ymm10, %ymm2
	vmovdqa	%ymm5, 40(%rsp)
	vpminsd	%ymm2, %ymm10, %ymm5
	vpmaxsd	%ymm2, %ymm10, %ymm10
	vpshufd	$177, %ymm9, %ymm2
	vpblendd	$85, %ymm5, %ymm10, %ymm10
	vpminsd	%ymm2, %ymm9, %ymm5
	vpmaxsd	%ymm2, %ymm9, %ymm9
	vmovdqa	%ymm4, 104(%rsp)
	vpblendd	$85, %ymm5, %ymm9, %ymm2
	vmovdqa	%ymm2, 8(%rsp)
	vpshufd	$177, %ymm15, %ymm2
	vpminsd	%ymm2, %ymm15, %ymm5
	vpmaxsd	%ymm2, %ymm15, %ymm15
	vpshufd	$177, %ymm6, %ymm2
	vpblendd	$85, %ymm5, %ymm15, %ymm15
	vpminsd	%ymm2, %ymm6, %ymm5
	vpmaxsd	%ymm2, %ymm6, %ymm6
	vpshufd	$177, %ymm14, %ymm2
	vpblendd	$85, %ymm5, %ymm6, %ymm6
	vpminsd	%ymm2, %ymm14, %ymm5
	vpmaxsd	%ymm2, %ymm14, %ymm14
	vpshufd	$177, %ymm8, %ymm2
	vmovdqa	%ymm6, -24(%rsp)
	vpblendd	$85, %ymm5, %ymm14, %ymm14
	vpminsd	%ymm2, %ymm8, %ymm5
	vpmaxsd	%ymm2, %ymm8, %ymm8
	vpshufd	$177, %ymm13, %ymm2
	vpblendd	$85, %ymm5, %ymm8, %ymm6
	vpshufd	$177, %ymm7, %ymm8
	vpminsd	%ymm2, %ymm13, %ymm5
	vpmaxsd	%ymm2, %ymm13, %ymm13
	vpshufd	$177, %ymm3, %ymm2
	vmovdqa	%ymm6, -56(%rsp)
	vpblendd	$85, %ymm5, %ymm13, %ymm6
	vpminsd	%ymm2, %ymm3, %ymm5
	vpmaxsd	%ymm2, %ymm3, %ymm3
	vpblendd	$85, %ymm5, %ymm3, %ymm13
	vmovdqa	72(%rsp), %ymm5
	vmovdqa	%ymm6, -88(%rsp)
	vpshufd	$177, %ymm5, %ymm6
	vpminsd	%ymm5, %ymm6, %ymm2
	vpmaxsd	%ymm5, %ymm6, %ymm6
	vpblendd	$85, %ymm2, %ymm6, %ymm6
	vpshufd	$177, %ymm0, %ymm2
	vpminsd	%ymm2, %ymm0, %ymm3
	vpmaxsd	%ymm2, %ymm0, %ymm0
	vpblendd	$85, %ymm3, %ymm0, %ymm2
	vpshufd	$177, %ymm12, %ymm0
	vpminsd	%ymm0, %ymm12, %ymm3
	vpmaxsd	%ymm0, %ymm12, %ymm0
	vpblendd	$85, %ymm3, %ymm0, %ymm0
	vpshufd	$177, %ymm1, %ymm3
	vpminsd	%ymm3, %ymm1, %ymm4
	vpmaxsd	%ymm3, %ymm1, %ymm1
	vpblendd	$85, %ymm4, %ymm1, %ymm1
	vmovdqa	104(%rsp), %ymm4
	vpminsd	%ymm8, %ymm7, %ymm3
	vpmaxsd	%ymm8, %ymm7, %ymm8
	vpshufd	$177, %ymm11, %ymm7
	vpblendd	$85, %ymm3, %ymm8, %ymm8
	vpshufd	$177, %ymm4, %ymm9
	vpminsd	%ymm7, %ymm11, %ymm3
	vpmaxsd	%ymm7, %ymm11, %ymm7
	vpblendd	$85, %ymm3, %ymm7, %ymm7
	vpminsd	%ymm4, %ymm9, %ymm3
	vpmaxsd	%ymm4, %ymm9, %ymm9
	vmovdqa	40(%rsp), %ymm4
	vpblendd	$85, %ymm3, %ymm9, %ymm9
	vpshufd	$177, %ymm4, %ymm5
	vpminsd	%ymm4, %ymm5, %ymm3
	vpmaxsd	%ymm4, %ymm5, %ymm5
	vpblendd	$85, %ymm3, %ymm5, %ymm5
	jbe	.L1779
	vmovdqa	8(%rsp), %ymm3
	vmovdqa	-24(%rsp), %ymm4
	vpshufd	$27, %ymm1, %ymm1
	vpshufd	$27, %ymm8, %ymm8
	vpshufd	$27, %ymm7, %ymm7
	vpshufd	$27, %ymm9, %ymm9
	vpshufd	$27, %ymm5, %ymm5
	cmpq	$7, 136(%rsp)
	vpminsd	%ymm5, %ymm10, %ymm12
	vpshufd	$27, %ymm2, %ymm11
	vpmaxsd	%ymm5, %ymm10, %ymm5
	vpminsd	%ymm7, %ymm15, %ymm2
	vpminsd	%ymm3, %ymm9, %ymm10
	vpshufd	$27, %ymm6, %ymm6
	vpmaxsd	%ymm3, %ymm9, %ymm9
	vpmaxsd	%ymm7, %ymm15, %ymm7
	vmovdqa	-88(%rsp), %ymm15
	vpshufd	$27, %ymm0, %ymm0
	vpminsd	%ymm4, %ymm8, %ymm3
	vpmaxsd	%ymm4, %ymm8, %ymm8
	vpshufd	$27, %ymm5, %ymm5
	vpminsd	%ymm1, %ymm14, %ymm4
	vpmaxsd	%ymm1, %ymm14, %ymm1
	vmovdqa	-56(%rsp), %ymm14
	vmovdqa	%ymm3, 104(%rsp)
	vpshufd	$27, %ymm7, %ymm7
	vpshufd	$27, %ymm9, %ymm9
	vpshufd	$27, %ymm4, %ymm4
	vpminsd	%ymm14, %ymm0, %ymm3
	vpmaxsd	%ymm14, %ymm0, %ymm0
	vpshufd	$27, %ymm8, %ymm8
	vpminsd	%ymm15, %ymm11, %ymm14
	vpmaxsd	%ymm15, %ymm11, %ymm11
	vpshufd	$27, %ymm3, %ymm3
	vpminsd	%ymm6, %ymm13, %ymm15
	vpmaxsd	%ymm6, %ymm13, %ymm6
	vpshufd	$27, %ymm14, %ymm14
	vpshufd	$27, %ymm15, %ymm15
	vpminsd	%ymm15, %ymm12, %ymm13
	vpmaxsd	%ymm15, %ymm12, %ymm12
	vpminsd	%ymm5, %ymm6, %ymm15
	vpmaxsd	%ymm5, %ymm6, %ymm6
	vpshufd	$27, %ymm12, %ymm12
	vmovdqa	%ymm15, 72(%rsp)
	vpminsd	%ymm9, %ymm11, %ymm15
	vpmaxsd	%ymm9, %ymm11, %ymm11
	vpshufd	$27, %ymm6, %ymm6
	vpminsd	%ymm3, %ymm2, %ymm9
	vpmaxsd	%ymm3, %ymm2, %ymm2
	vpshufd	$27, %ymm11, %ymm11
	vpminsd	%ymm7, %ymm0, %ymm3
	vpmaxsd	%ymm7, %ymm0, %ymm0
	vmovdqa	104(%rsp), %ymm7
	vpshufd	$27, %ymm9, %ymm9
	vpminsd	%ymm14, %ymm10, %ymm5
	vpmaxsd	%ymm14, %ymm10, %ymm10
	vpshufd	$27, %ymm3, %ymm3
	vpminsd	%ymm7, %ymm4, %ymm14
	vpmaxsd	%ymm7, %ymm4, %ymm4
	vpshufd	$27, %ymm14, %ymm14
	vpminsd	%ymm8, %ymm1, %ymm7
	vpmaxsd	%ymm8, %ymm1, %ymm1
	vpshufd	$27, %ymm10, %ymm8
	vpminsd	%ymm14, %ymm13, %ymm10
	vpmaxsd	%ymm14, %ymm13, %ymm14
	vpminsd	%ymm9, %ymm5, %ymm13
	vpmaxsd	%ymm9, %ymm5, %ymm5
	vpshufd	$27, %ymm7, %ymm7
	vpminsd	%ymm12, %ymm4, %ymm9
	vpmaxsd	%ymm12, %ymm4, %ymm4
	vmovdqa	72(%rsp), %ymm12
	vmovdqa	%ymm13, 104(%rsp)
	vpminsd	%ymm8, %ymm2, %ymm13
	vpshufd	$27, %ymm14, %ymm14
	vpmaxsd	%ymm8, %ymm2, %ymm2
	vpshufd	$27, %ymm13, %ymm13
	vpminsd	%ymm12, %ymm7, %ymm8
	vpshufd	$27, %ymm4, %ymm4
	vpmaxsd	%ymm12, %ymm7, %ymm7
	vpminsd	%ymm3, %ymm15, %ymm12
	vpmaxsd	%ymm3, %ymm15, %ymm3
	vpminsd	%ymm6, %ymm1, %ymm15
	vpshufd	$27, %ymm12, %ymm12
	vmovdqa	%ymm15, 72(%rsp)
	vpmaxsd	%ymm6, %ymm1, %ymm1
	vpminsd	%ymm11, %ymm0, %ymm6
	vpshufd	$27, 104(%rsp), %ymm15
	vpmaxsd	%ymm11, %ymm0, %ymm0
	vpminsd	%ymm15, %ymm10, %ymm11
	vpshufd	$27, %ymm7, %ymm7
	vpmaxsd	%ymm15, %ymm10, %ymm10
	vpminsd	%ymm14, %ymm5, %ymm15
	vpshufd	$27, %ymm1, %ymm1
	vpmaxsd	%ymm14, %ymm5, %ymm5
	vpminsd	%ymm13, %ymm9, %ymm14
	vpshufd	$27, %ymm6, %ymm6
	vpmaxsd	%ymm13, %ymm9, %ymm9
	vpminsd	%ymm4, %ymm2, %ymm13
	vpmaxsd	%ymm4, %ymm2, %ymm2
	vpminsd	%ymm12, %ymm8, %ymm4
	vmovdqa	%ymm2, 40(%rsp)
	vmovdqa	72(%rsp), %ymm2
	vpmaxsd	%ymm12, %ymm8, %ymm8
	vpminsd	%ymm7, %ymm3, %ymm12
	vpmaxsd	%ymm7, %ymm3, %ymm3
	vpminsd	%ymm2, %ymm6, %ymm7
	vpmaxsd	%ymm2, %ymm6, %ymm6
	vpminsd	%ymm1, %ymm0, %ymm2
	vpmaxsd	%ymm1, %ymm0, %ymm0
	vmovdqa	%ymm0, 72(%rsp)
	vpshufd	$27, %ymm11, %ymm0
	vpminsd	%ymm0, %ymm11, %ymm1
	vpmaxsd	%ymm0, %ymm11, %ymm11
	vpshufd	$27, %ymm10, %ymm0
	vmovdqa	%ymm2, 104(%rsp)
	vpblendd	$51, %ymm1, %ymm11, %ymm11
	vpminsd	%ymm0, %ymm10, %ymm1
	vpmaxsd	%ymm0, %ymm10, %ymm10
	vmovdqa	40(%rsp), %ymm2
	vpshufd	$27, %ymm15, %ymm0
	vpblendd	$51, %ymm1, %ymm10, %ymm10
	vpminsd	%ymm0, %ymm15, %ymm1
	vpmaxsd	%ymm0, %ymm15, %ymm15
	vpshufd	$27, %ymm5, %ymm0
	vpblendd	$51, %ymm1, %ymm15, %ymm15
	vpminsd	%ymm0, %ymm5, %ymm1
	vpmaxsd	%ymm0, %ymm5, %ymm5
	vpshufd	$27, %ymm14, %ymm0
	vpblendd	$51, %ymm1, %ymm5, %ymm5
	vpminsd	%ymm0, %ymm14, %ymm1
	vpmaxsd	%ymm0, %ymm14, %ymm14
	vpshufd	$27, %ymm9, %ymm0
	vpblendd	$51, %ymm1, %ymm14, %ymm14
	vpminsd	%ymm0, %ymm9, %ymm1
	vpmaxsd	%ymm0, %ymm9, %ymm9
	vpshufd	$27, %ymm13, %ymm0
	vpblendd	$51, %ymm1, %ymm9, %ymm9
	vpminsd	%ymm0, %ymm13, %ymm1
	vpmaxsd	%ymm0, %ymm13, %ymm13
	vpshufd	$27, %ymm2, %ymm0
	vpblendd	$51, %ymm1, %ymm13, %ymm13
	vpminsd	%ymm2, %ymm0, %ymm1
	vpmaxsd	%ymm2, %ymm0, %ymm2
	vpshufd	$27, %ymm4, %ymm0
	vpblendd	$51, %ymm1, %ymm2, %ymm2
	vpminsd	%ymm0, %ymm4, %ymm1
	vpmaxsd	%ymm0, %ymm4, %ymm4
	vpshufd	$27, %ymm8, %ymm0
	vpblendd	$51, %ymm1, %ymm4, %ymm4
	vpminsd	%ymm0, %ymm8, %ymm1
	vpmaxsd	%ymm0, %ymm8, %ymm8
	vpshufd	$27, %ymm12, %ymm0
	vpblendd	$51, %ymm1, %ymm8, %ymm8
	vpminsd	%ymm0, %ymm12, %ymm1
	vpmaxsd	%ymm0, %ymm12, %ymm12
	vpshufd	$27, %ymm3, %ymm0
	vpblendd	$51, %ymm1, %ymm12, %ymm12
	vpminsd	%ymm0, %ymm3, %ymm1
	vpmaxsd	%ymm0, %ymm3, %ymm3
	vpshufd	$27, %ymm7, %ymm0
	vpblendd	$51, %ymm1, %ymm3, %ymm3
	vpminsd	%ymm0, %ymm7, %ymm1
	vpmaxsd	%ymm0, %ymm7, %ymm7
	vpshufd	$27, %ymm6, %ymm0
	vpblendd	$51, %ymm1, %ymm7, %ymm7
	vpminsd	%ymm0, %ymm6, %ymm1
	vpmaxsd	%ymm0, %ymm6, %ymm6
	vpblendd	$51, %ymm1, %ymm6, %ymm6
	vmovdqa	104(%rsp), %ymm1
	vpshufd	$27, %ymm1, %ymm0
	vpminsd	%ymm1, %ymm0, %ymm1
	vpmaxsd	104(%rsp), %ymm0, %ymm0
	vpblendd	$51, %ymm1, %ymm0, %ymm1
	vmovdqa	%ymm1, 104(%rsp)
	vmovdqa	72(%rsp), %ymm1
	vpshufd	$27, %ymm1, %ymm0
	vpminsd	%ymm1, %ymm0, %ymm1
	vpmaxsd	72(%rsp), %ymm0, %ymm0
	vpblendd	$51, %ymm1, %ymm0, %ymm0
	vmovdqa	%ymm0, 72(%rsp)
	vpshufd	$177, %ymm11, %ymm0
	vpminsd	%ymm0, %ymm11, %ymm1
	vpmaxsd	%ymm0, %ymm11, %ymm11
	vpshufd	$177, %ymm10, %ymm0
	vpblendd	$85, %ymm1, %ymm11, %ymm11
	vpminsd	%ymm0, %ymm10, %ymm1
	vpmaxsd	%ymm0, %ymm10, %ymm10
	vpshufd	$177, %ymm15, %ymm0
	vpblendd	$85, %ymm1, %ymm10, %ymm10
	vmovdqa	%ymm11, 40(%rsp)
	vpminsd	%ymm0, %ymm15, %ymm1
	vpmaxsd	%ymm0, %ymm15, %ymm15
	vpshufd	$177, %ymm5, %ymm0
	vpblendd	$85, %ymm1, %ymm15, %ymm11
	vpminsd	%ymm0, %ymm5, %ymm1
	vpmaxsd	%ymm0, %ymm5, %ymm5
	vpshufd	$177, %ymm14, %ymm0
	vpblendd	$85, %ymm1, %ymm5, %ymm5
	vmovdqa	%ymm11, 8(%rsp)
	vpshufd	$177, %ymm8, %ymm15
	vpminsd	%ymm0, %ymm14, %ymm1
	vpmaxsd	%ymm0, %ymm14, %ymm14
	vpshufd	$177, %ymm9, %ymm0
	vpblendd	$85, %ymm1, %ymm14, %ymm11
	vpminsd	%ymm0, %ymm9, %ymm1
	vpmaxsd	%ymm0, %ymm9, %ymm9
	vpshufd	$177, %ymm13, %ymm0
	vmovdqa	%ymm11, -24(%rsp)
	vpshufd	$177, %ymm4, %ymm14
	vpblendd	$85, %ymm1, %ymm9, %ymm11
	vpminsd	%ymm0, %ymm13, %ymm1
	vpmaxsd	%ymm0, %ymm13, %ymm13
	vpshufd	$177, %ymm2, %ymm0
	vmovdqa	%ymm11, -56(%rsp)
	vpblendd	$85, %ymm1, %ymm13, %ymm11
	vpminsd	%ymm0, %ymm2, %ymm1
	vpmaxsd	%ymm0, %ymm2, %ymm2
	vpshufd	$177, %ymm12, %ymm13
	vmovdqa	%ymm11, -88(%rsp)
	vpminsd	%ymm14, %ymm4, %ymm0
	vpblendd	$85, %ymm1, %ymm2, %ymm11
	vpminsd	%ymm15, %ymm8, %ymm1
	vpmaxsd	%ymm15, %ymm8, %ymm15
	vpblendd	$85, %ymm1, %ymm15, %ymm15
	vpminsd	%ymm13, %ymm12, %ymm1
	vpmaxsd	%ymm13, %ymm12, %ymm13
	vpmaxsd	%ymm14, %ymm4, %ymm14
	vpblendd	$85, %ymm1, %ymm13, %ymm13
	vpshufd	$177, %ymm3, %ymm1
	vpshufd	$177, %ymm7, %ymm4
	vpblendd	$85, %ymm0, %ymm14, %ymm14
	vpminsd	%ymm1, %ymm3, %ymm0
	vpmaxsd	%ymm1, %ymm3, %ymm1
	vmovdqa	72(%rsp), %ymm3
	vpblendd	$85, %ymm0, %ymm1, %ymm1
	vpminsd	%ymm4, %ymm7, %ymm0
	vpmaxsd	%ymm4, %ymm7, %ymm4
	vpshufd	$177, %ymm6, %ymm7
	vpblendd	$85, %ymm0, %ymm4, %ymm4
	vpshufd	$177, %ymm3, %ymm12
	vpminsd	%ymm7, %ymm6, %ymm0
	vpmaxsd	%ymm7, %ymm6, %ymm7
	vmovdqa	104(%rsp), %ymm6
	vpblendd	$85, %ymm0, %ymm7, %ymm7
	vpshufd	$177, %ymm6, %ymm9
	vpminsd	%ymm6, %ymm9, %ymm0
	vpmaxsd	%ymm6, %ymm9, %ymm9
	vpblendd	$85, %ymm0, %ymm9, %ymm9
	vpminsd	%ymm3, %ymm12, %ymm0
	vpmaxsd	%ymm3, %ymm12, %ymm12
	vpblendd	$85, %ymm0, %ymm12, %ymm12
	jbe	.L1780
	vmovdqa	.LC13(%rip), %ymm2
	vmovdqa	40(%rsp), %ymm3
	vpermd	%ymm12, %ymm2, %ymm12
	vpermd	%ymm15, %ymm2, %ymm0
	vpermd	%ymm13, %ymm2, %ymm6
	vpmaxsd	%ymm3, %ymm12, %ymm15
	vpminsd	%ymm3, %ymm12, %ymm13
	vpermd	%ymm14, %ymm2, %ymm14
	vmovdqa	8(%rsp), %ymm3
	vmovdqa	%ymm14, 136(%rsp)
	vpermd	%ymm7, %ymm2, %ymm7
	vpermd	%ymm1, %ymm2, %ymm14
	vpermd	%ymm4, %ymm2, %ymm1
	vmovdqa	-24(%rsp), %ymm4
	vpminsd	%ymm3, %ymm7, %ymm8
	vpmaxsd	%ymm3, %ymm7, %ymm7
	vmovdqa	%ymm6, 104(%rsp)
	vpminsd	%ymm1, %ymm5, %ymm3
	vpermd	%ymm9, %ymm2, %ymm6
	vpmaxsd	%ymm1, %ymm5, %ymm1
	vmovdqa	104(%rsp), %ymm5
	vmovdqa	%ymm3, 72(%rsp)
	vpminsd	%ymm4, %ymm14, %ymm3
	vpermd	%ymm7, %ymm2, %ymm7
	vpermd	%ymm1, %ymm2, %ymm1
	vpmaxsd	%ymm4, %ymm14, %ymm4
	vpminsd	%ymm6, %ymm10, %ymm12
	vpermd	%ymm3, %ymm2, %ymm3
	vmovdqa	136(%rsp), %ymm14
	vmovdqa	%ymm4, 40(%rsp)
	vpmaxsd	%ymm6, %ymm10, %ymm6
	vmovdqa	-88(%rsp), %ymm4
	vmovdqa	-56(%rsp), %ymm10
	vpermd	%ymm6, %ymm2, %ymm6
	vpminsd	%ymm10, %ymm5, %ymm9
	vpmaxsd	%ymm5, %ymm10, %ymm5
	vpminsd	%ymm4, %ymm0, %ymm10
	vpmaxsd	%ymm4, %ymm0, %ymm0
	vpermd	%ymm9, %ymm2, %ymm9
	vpminsd	%ymm14, %ymm11, %ymm4
	vpmaxsd	%ymm14, %ymm11, %ymm11
	vpermd	%ymm15, %ymm2, %ymm14
	vpermd	%ymm4, %ymm2, %ymm4
	vpermd	%ymm10, %ymm2, %ymm10
	vpminsd	%ymm4, %ymm13, %ymm15
	vpmaxsd	%ymm4, %ymm13, %ymm13
	vpminsd	%ymm14, %ymm11, %ymm4
	vpmaxsd	%ymm14, %ymm11, %ymm11
	vpermd	%ymm13, %ymm2, %ymm13
	vmovdqa	%ymm4, 136(%rsp)
	vpmaxsd	%ymm7, %ymm5, %ymm14
	vpminsd	%ymm10, %ymm12, %ymm4
	vpermd	%ymm11, %ymm2, %ymm11
	vpmaxsd	%ymm10, %ymm12, %ymm12
	vpminsd	%ymm6, %ymm0, %ymm10
	vpmaxsd	%ymm6, %ymm0, %ymm0
	vpminsd	%ymm9, %ymm8, %ymm6
	vmovdqa	%ymm10, 104(%rsp)
	vpermd	%ymm12, %ymm2, %ymm12
	vpmaxsd	%ymm9, %ymm8, %ymm8
	vpminsd	%ymm7, %ymm5, %ymm9
	vmovdqa	72(%rsp), %ymm7
	vpermd	%ymm6, %ymm2, %ymm6
	vpermd	%ymm9, %ymm2, %ymm9
	vpermd	%ymm0, %ymm2, %ymm0
	vpminsd	%ymm7, %ymm3, %ymm10
	vpmaxsd	%ymm7, %ymm3, %ymm3
	vmovdqa	40(%rsp), %ymm7
	vpermd	%ymm10, %ymm2, %ymm10
	vpminsd	%ymm7, %ymm1, %ymm5
	vpmaxsd	%ymm7, %ymm1, %ymm1
	vpminsd	%ymm10, %ymm15, %ymm7
	vpmaxsd	%ymm10, %ymm15, %ymm15
	vpermd	%ymm5, %ymm2, %ymm5
	vpminsd	%ymm6, %ymm4, %ymm10
	vpmaxsd	%ymm6, %ymm4, %ymm4
	vpermd	%ymm15, %ymm2, %ymm15
	vpminsd	%ymm13, %ymm3, %ymm6
	vpmaxsd	%ymm13, %ymm3, %ymm3
	vpermd	%ymm10, %ymm2, %ymm10
	vpminsd	%ymm12, %ymm8, %ymm13
	vpmaxsd	%ymm12, %ymm8, %ymm8
	vpermd	%ymm3, %ymm2, %ymm3
	vmovdqa	%ymm13, 72(%rsp)
	vmovdqa	136(%rsp), %ymm13
	vpminsd	%ymm13, %ymm5, %ymm12
	vpmaxsd	%ymm13, %ymm5, %ymm5
	vmovdqa	%ymm12, 40(%rsp)
	vmovdqa	104(%rsp), %ymm12
	vpermd	%ymm5, %ymm2, %ymm5
	vpminsd	%ymm12, %ymm9, %ymm13
	vpmaxsd	%ymm12, %ymm9, %ymm9
	vpminsd	%ymm11, %ymm1, %ymm12
	vpermd	%ymm13, %ymm2, %ymm13
	vpmaxsd	%ymm11, %ymm1, %ymm1
	vmovdqa	%ymm12, 104(%rsp)
	vpminsd	%ymm0, %ymm14, %ymm12
	vpmaxsd	%ymm0, %ymm14, %ymm0
	vpermd	72(%rsp), %ymm2, %ymm14
	vpminsd	%ymm10, %ymm7, %ymm11
	vpmaxsd	%ymm10, %ymm7, %ymm7
	vpermd	%ymm12, %ymm2, %ymm12
	vmovdqa	%ymm0, 136(%rsp)
	vmovdqa	40(%rsp), %ymm0
	vpminsd	%ymm15, %ymm4, %ymm10
	vpmaxsd	%ymm15, %ymm4, %ymm4
	vpermd	%ymm1, %ymm2, %ymm1
	vpminsd	%ymm14, %ymm6, %ymm15
	vpmaxsd	%ymm14, %ymm6, %ymm6
	vpminsd	%ymm3, %ymm8, %ymm14
	vpmaxsd	%ymm3, %ymm8, %ymm3
	vpminsd	%ymm0, %ymm13, %ymm8
	vpmaxsd	%ymm0, %ymm13, %ymm13
	vpminsd	%ymm5, %ymm9, %ymm0
	vpmaxsd	%ymm5, %ymm9, %ymm5
	vmovdqa	%ymm0, 72(%rsp)
	vmovdqa	104(%rsp), %ymm0
	vpminsd	%ymm0, %ymm12, %ymm9
	vpmaxsd	%ymm0, %ymm12, %ymm12
	vpminsd	136(%rsp), %ymm1, %ymm0
	vpmaxsd	136(%rsp), %ymm1, %ymm1
	vmovdqa	%ymm0, 104(%rsp)
	vpermd	%ymm11, %ymm2, %ymm0
	vmovdqa	%ymm1, 40(%rsp)
	vpminsd	%ymm0, %ymm11, %ymm1
	vpmaxsd	%ymm0, %ymm11, %ymm11
	vpermd	%ymm7, %ymm2, %ymm0
	vpblendd	$15, %ymm1, %ymm11, %ymm11
	vpminsd	%ymm0, %ymm7, %ymm1
	vpmaxsd	%ymm0, %ymm7, %ymm7
	vpermd	%ymm10, %ymm2, %ymm0
	vpblendd	$15, %ymm1, %ymm7, %ymm7
	vpminsd	%ymm0, %ymm10, %ymm1
	vpmaxsd	%ymm0, %ymm10, %ymm10
	vpermd	%ymm4, %ymm2, %ymm0
	vpblendd	$15, %ymm1, %ymm10, %ymm10
	vpminsd	%ymm0, %ymm4, %ymm1
	vpmaxsd	%ymm0, %ymm4, %ymm4
	vpermd	%ymm15, %ymm2, %ymm0
	vpblendd	$15, %ymm1, %ymm4, %ymm4
	vmovdqa	%ymm10, 8(%rsp)
	vpminsd	%ymm0, %ymm15, %ymm1
	vpmaxsd	%ymm0, %ymm15, %ymm15
	vpermd	%ymm6, %ymm2, %ymm0
	vpblendd	$15, %ymm1, %ymm15, %ymm15
	vpminsd	%ymm0, %ymm6, %ymm1
	vpmaxsd	%ymm0, %ymm6, %ymm6
	vpermd	%ymm14, %ymm2, %ymm0
	vpblendd	$15, %ymm1, %ymm6, %ymm10
	vpminsd	%ymm0, %ymm14, %ymm1
	vpmaxsd	%ymm0, %ymm14, %ymm14
	vpermd	%ymm3, %ymm2, %ymm0
	vpblendd	$15, %ymm1, %ymm14, %ymm14
	vpminsd	%ymm0, %ymm3, %ymm1
	vpmaxsd	%ymm0, %ymm3, %ymm3
	vpermd	%ymm8, %ymm2, %ymm0
	vmovdqa	%ymm14, -24(%rsp)
	vpblendd	$15, %ymm1, %ymm3, %ymm14
	vpminsd	%ymm0, %ymm8, %ymm1
	vpmaxsd	%ymm0, %ymm8, %ymm8
	vpermd	%ymm13, %ymm2, %ymm0
	vpblendd	$15, %ymm1, %ymm8, %ymm8
	vpminsd	%ymm0, %ymm13, %ymm1
	vpmaxsd	%ymm0, %ymm13, %ymm13
	vmovdqa	%ymm8, -56(%rsp)
	vmovdqa	72(%rsp), %ymm3
	vpblendd	$15, %ymm1, %ymm13, %ymm13
	vmovdqa	40(%rsp), %ymm6
	vpermd	%ymm3, %ymm2, %ymm0
	vpminsd	%ymm3, %ymm0, %ymm1
	vpmaxsd	%ymm3, %ymm0, %ymm0
	vpblendd	$15, %ymm1, %ymm0, %ymm3
	vpermd	%ymm5, %ymm2, %ymm0
	vpminsd	%ymm0, %ymm5, %ymm1
	vpmaxsd	%ymm0, %ymm5, %ymm5
	vpermd	%ymm9, %ymm2, %ymm0
	vmovdqa	%ymm3, 72(%rsp)
	vpblendd	$15, %ymm1, %ymm5, %ymm8
	vmovdqa	104(%rsp), %ymm5
	vpminsd	%ymm0, %ymm9, %ymm1
	vpmaxsd	%ymm0, %ymm9, %ymm9
	vpermd	%ymm12, %ymm2, %ymm0
	vpblendd	$15, %ymm1, %ymm9, %ymm9
	vpminsd	%ymm0, %ymm12, %ymm1
	vpmaxsd	%ymm0, %ymm12, %ymm12
	vpermd	%ymm5, %ymm2, %ymm0
	vmovdqa	%ymm9, -88(%rsp)
	vpblendd	$15, %ymm1, %ymm12, %ymm12
	vpermd	%ymm6, %ymm2, %ymm2
	vpminsd	%ymm5, %ymm0, %ymm1
	vpmaxsd	%ymm5, %ymm0, %ymm0
	vpshufd	$78, %ymm10, %ymm9
	vmovdqa	%ymm12, 136(%rsp)
	vpblendd	$15, %ymm1, %ymm0, %ymm5
	vpminsd	%ymm6, %ymm2, %ymm0
	vpmaxsd	%ymm6, %ymm2, %ymm2
	vpblendd	$15, %ymm0, %ymm2, %ymm3
	vpshufd	$78, %ymm11, %ymm0
	vpshufd	$78, %ymm7, %ymm6
	vmovdqa	%ymm5, 104(%rsp)
	vpminsd	%ymm0, %ymm11, %ymm1
	vpmaxsd	%ymm0, %ymm11, %ymm0
	vpshufd	$78, %ymm15, %ymm5
	vpblendd	$51, %ymm1, %ymm0, %ymm0
	vpminsd	%ymm6, %ymm7, %ymm1
	vpmaxsd	%ymm6, %ymm7, %ymm6
	vmovdqa	8(%rsp), %ymm7
	vpblendd	$51, %ymm1, %ymm6, %ymm6
	vpshufd	$78, %ymm14, %ymm11
	vpshufd	$78, %ymm7, %ymm12
	vpminsd	%ymm7, %ymm12, %ymm1
	vpmaxsd	%ymm7, %ymm12, %ymm12
	vpshufd	$78, %ymm4, %ymm7
	vpblendd	$51, %ymm1, %ymm12, %ymm12
	vpminsd	%ymm7, %ymm4, %ymm1
	vpmaxsd	%ymm7, %ymm4, %ymm7
	vmovdqa	-24(%rsp), %ymm4
	vpblendd	$51, %ymm1, %ymm7, %ymm7
	vpminsd	%ymm5, %ymm15, %ymm1
	vpmaxsd	%ymm5, %ymm15, %ymm5
	vpblendd	$51, %ymm1, %ymm5, %ymm5
	vpminsd	%ymm9, %ymm10, %ymm1
	vpmaxsd	%ymm9, %ymm10, %ymm9
	vpshufd	$78, %ymm4, %ymm10
	vpblendd	$51, %ymm1, %ymm9, %ymm9
	vpshufd	$78, %ymm13, %ymm15
	vpminsd	%ymm4, %ymm10, %ymm1
	vpmaxsd	%ymm4, %ymm10, %ymm10
	vmovdqa	-56(%rsp), %ymm4
	vpblendd	$51, %ymm1, %ymm10, %ymm10
	vpminsd	%ymm11, %ymm14, %ymm1
	vpmaxsd	%ymm11, %ymm14, %ymm11
	vpshufd	$78, %ymm4, %ymm14
	vpblendd	$51, %ymm1, %ymm11, %ymm11
	vpminsd	%ymm4, %ymm14, %ymm1
	vpmaxsd	%ymm4, %ymm14, %ymm14
	vmovdqa	72(%rsp), %ymm4
	vpblendd	$51, %ymm1, %ymm14, %ymm14
	vpminsd	%ymm15, %ymm13, %ymm1
	vpmaxsd	%ymm15, %ymm13, %ymm15
	vpshufd	$78, %ymm4, %ymm13
	vpblendd	$51, %ymm1, %ymm15, %ymm15
	vpminsd	%ymm4, %ymm13, %ymm1
	vpmaxsd	%ymm4, %ymm13, %ymm13
	vmovdqa	-88(%rsp), %ymm4
	vpblendd	$51, %ymm1, %ymm13, %ymm13
	vpshufd	$78, %ymm8, %ymm1
	vpminsd	%ymm1, %ymm8, %ymm2
	vpmaxsd	%ymm1, %ymm8, %ymm1
	vpshufd	$78, %ymm4, %ymm8
	vpblendd	$51, %ymm2, %ymm1, %ymm1
	vpminsd	%ymm4, %ymm8, %ymm2
	vpmaxsd	%ymm4, %ymm8, %ymm8
	vmovdqa	136(%rsp), %ymm4
	vpblendd	$51, %ymm2, %ymm8, %ymm8
	vpshufd	$78, %ymm4, %ymm2
	vpminsd	%ymm4, %ymm2, %ymm4
	vpmaxsd	136(%rsp), %ymm2, %ymm2
	vpblendd	$51, %ymm4, %ymm2, %ymm2
	vmovdqa	104(%rsp), %ymm4
	vmovdqa	%ymm2, -56(%rsp)
	vpshufd	$78, %ymm4, %ymm2
	vpminsd	%ymm4, %ymm2, %ymm4
	vpmaxsd	104(%rsp), %ymm2, %ymm2
	vpblendd	$51, %ymm4, %ymm2, %ymm4
	vpshufd	$78, %ymm3, %ymm2
	vmovdqa	%ymm4, -88(%rsp)
	vpminsd	%ymm2, %ymm3, %ymm4
	vpmaxsd	%ymm2, %ymm3, %ymm3
	vpblendd	$51, %ymm4, %ymm3, %ymm2
	vpshufd	$177, %ymm8, %ymm4
	vmovdqa	%ymm2, -120(%rsp)
	vpshufd	$177, %ymm0, %ymm2
	vpminsd	%ymm2, %ymm0, %ymm3
	vpmaxsd	%ymm2, %ymm0, %ymm0
	vpblendd	$85, %ymm3, %ymm0, %ymm0
	vmovdqa	%ymm0, 136(%rsp)
	vpshufd	$177, %ymm6, %ymm0
	vpminsd	%ymm0, %ymm6, %ymm2
	vpmaxsd	%ymm0, %ymm6, %ymm6
	vpblendd	$85, %ymm2, %ymm6, %ymm0
	vpshufd	$177, %ymm14, %ymm6
	vmovdqa	%ymm0, 104(%rsp)
	vpshufd	$177, %ymm12, %ymm0
	vpminsd	%ymm0, %ymm12, %ymm2
	vpmaxsd	%ymm0, %ymm12, %ymm12
	vpblendd	$85, %ymm2, %ymm12, %ymm0
	vmovdqa	%ymm0, 72(%rsp)
	vpshufd	$177, %ymm7, %ymm0
	vpminsd	%ymm0, %ymm7, %ymm2
	vpmaxsd	%ymm0, %ymm7, %ymm7
	vpshufd	$177, %ymm5, %ymm0
	vpblendd	$85, %ymm2, %ymm7, %ymm7
	vpminsd	%ymm0, %ymm5, %ymm2
	vpmaxsd	%ymm0, %ymm5, %ymm5
	vpshufd	$177, %ymm9, %ymm0
	vmovdqa	%ymm7, 40(%rsp)
	vpblendd	$85, %ymm2, %ymm5, %ymm7
	vmovdqa	-56(%rsp), %ymm5
	vpminsd	%ymm0, %ymm9, %ymm2
	vpmaxsd	%ymm0, %ymm9, %ymm9
	vpshufd	$177, %ymm10, %ymm0
	vmovdqa	%ymm7, 8(%rsp)
	vpblendd	$85, %ymm2, %ymm9, %ymm7
	vpminsd	%ymm0, %ymm10, %ymm2
	vpmaxsd	%ymm0, %ymm10, %ymm10
	vpshufd	$177, %ymm11, %ymm0
	vpblendd	$85, %ymm2, %ymm10, %ymm10
	vmovdqa	%ymm7, -24(%rsp)
	vpshufd	$177, %ymm5, %ymm7
	vpminsd	%ymm0, %ymm11, %ymm2
	vpmaxsd	%ymm0, %ymm11, %ymm11
	vpblendd	$85, %ymm2, %ymm11, %ymm11
	vpminsd	%ymm6, %ymm14, %ymm0
	vpshufd	$177, %ymm15, %ymm2
	vpmaxsd	%ymm6, %ymm14, %ymm14
	vpminsd	%ymm2, %ymm15, %ymm3
	vmovdqa	-120(%rsp), %ymm6
	vpmaxsd	%ymm2, %ymm15, %ymm15
	vpblendd	$85, %ymm0, %ymm14, %ymm14
	vpshufd	$177, %ymm13, %ymm0
	vpminsd	%ymm0, %ymm13, %ymm2
	vpblendd	$85, %ymm3, %ymm15, %ymm15
	vpmaxsd	%ymm0, %ymm13, %ymm13
	vmovdqa	-88(%rsp), %ymm3
	vpshufd	$177, %ymm1, %ymm0
	vpblendd	$85, %ymm2, %ymm13, %ymm13
	vpminsd	%ymm0, %ymm1, %ymm2
	vpmaxsd	%ymm0, %ymm1, %ymm1
	vpshufd	$177, %ymm3, %ymm9
	vpminsd	%ymm4, %ymm8, %ymm0
	vpmaxsd	%ymm4, %ymm8, %ymm4
	vpblendd	$85, %ymm2, %ymm1, %ymm1
	vpblendd	$85, %ymm0, %ymm4, %ymm4
	vpminsd	%ymm5, %ymm7, %ymm0
	vpmaxsd	%ymm5, %ymm7, %ymm7
	vpblendd	$85, %ymm0, %ymm7, %ymm7
	vpshufd	$177, %ymm6, %ymm5
	vpminsd	%ymm3, %ymm9, %ymm0
	vpmaxsd	%ymm3, %ymm9, %ymm9
	vpblendd	$85, %ymm0, %ymm9, %ymm3
	vpminsd	%ymm6, %ymm5, %ymm0
	vpmaxsd	%ymm6, %ymm5, %ymm5
	vpblendd	$85, %ymm0, %ymm5, %ymm12
.L1774:
	vmovdqa	136(%rsp), %ymm6
	vmovdqa	104(%rsp), %ymm5
	movq	176(%rsp), %rdx
	vmovdqu	%ymm6, (%rdx)
	vmovdqa	72(%rsp), %ymm6
	vmovdqu	%ymm5, (%r15)
	vmovdqa	40(%rsp), %ymm5
	vmovdqu	%ymm6, (%r14)
	vmovdqa	8(%rsp), %ymm6
	vmovdqu	%ymm5, 0(%r13)
	vmovdqa	-24(%rsp), %ymm5
	vmovdqu	%ymm6, (%r12)
	vmovdqu	%ymm5, (%rbx)
	movq	192(%rsp), %rbx
	vmovdqu	%ymm10, (%r11)
	vmovdqu	%ymm11, (%r10)
	vmovdqu	%ymm14, (%r9)
	vmovdqu	%ymm15, (%r8)
	vmovdqu	%ymm13, (%rdi)
	vmovdqu	%ymm1, (%rsi)
	vmovdqu	%ymm4, (%rcx)
	movq	184(%rsp), %rcx
	vmovdqu	%ymm7, (%rcx)
	vmovdqu	%ymm3, (%rbx)
	vmovdqu	%ymm12, (%rax)
	vzeroupper
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L1778:
	.cfi_restore_state
	vmovdqa	72(%rsp), %ymm6
	vmovdqa	104(%rsp), %ymm12
	vmovdqa	8(%rsp), %ymm3
	vmovdqa	%ymm2, 8(%rsp)
	vmovdqa	%ymm6, 136(%rsp)
	vmovdqa	40(%rsp), %ymm6
	vmovdqa	%ymm9, 40(%rsp)
	vmovdqa	%ymm6, 104(%rsp)
	vmovdqa	-24(%rsp), %ymm6
	vmovdqa	%ymm5, -24(%rsp)
	vmovdqa	%ymm6, 72(%rsp)
	jmp	.L1774
	.p2align 4,,10
	.p2align 3
.L1779:
	vmovdqa	8(%rsp), %ymm3
	vmovdqa	%ymm13, %ymm11
	vmovdqa	%ymm8, %ymm4
	vmovdqa	%ymm0, %ymm13
	vmovdqa	%ymm10, 136(%rsp)
	vmovdqa	%ymm5, %ymm12
	vmovdqa	-88(%rsp), %ymm10
	vmovdqa	%ymm3, 104(%rsp)
	vmovdqa	-24(%rsp), %ymm3
	vmovdqa	%ymm15, 72(%rsp)
	vmovdqa	%ymm2, %ymm15
	vmovdqa	%ymm3, 40(%rsp)
	vmovdqa	-56(%rsp), %ymm3
	vmovdqa	%ymm14, 8(%rsp)
	vmovdqa	%ymm6, %ymm14
	vmovdqa	%ymm3, -24(%rsp)
	vmovdqa	%ymm9, %ymm3
	jmp	.L1774
	.p2align 4,,10
	.p2align 3
.L1780:
	vmovdqa	40(%rsp), %ymm6
	vmovdqa	%ymm5, 40(%rsp)
	vmovdqa	-24(%rsp), %ymm5
	vmovdqa	%ymm9, %ymm3
	vmovdqa	%ymm10, 104(%rsp)
	vmovdqa	-88(%rsp), %ymm10
	vmovdqa	%ymm6, 136(%rsp)
	vmovdqa	8(%rsp), %ymm6
	vmovdqa	%ymm5, 8(%rsp)
	vmovdqa	-56(%rsp), %ymm5
	vmovdqa	%ymm6, 72(%rsp)
	vmovdqa	%ymm5, -24(%rsp)
	jmp	.L1774
	.cfi_endproc
.LFE18808:
	.size	_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0, .-_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	.section	.text._ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0, @function
_ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0:
.LFB18809:
	.cfi_startproc
	leaq	8(%rsp), %r10
	.cfi_def_cfa 10, 0
	andq	$-32, %rsp
	pushq	-8(%r10)
	pushq	%rbp
	movq	%rsp, %rbp
	.cfi_escape 0x10,0x6,0x2,0x76,0
	pushq	%r15
	pushq	%r14
	pushq	%r13
	.cfi_escape 0x10,0xf,0x2,0x76,0x78
	.cfi_escape 0x10,0xe,0x2,0x76,0x70
	.cfi_escape 0x10,0xd,0x2,0x76,0x68
	movq	%rdi, %r13
	pushq	%r12
	.cfi_escape 0x10,0xc,0x2,0x76,0x60
	movq	%rcx, %r12
	pushq	%r10
	.cfi_escape 0xf,0x3,0x76,0x58,0x6
	pushq	%rbx
	addq	$-128, %rsp
	.cfi_escape 0x10,0x3,0x2,0x76,0x50
	movq	%rsi, -104(%rbp)
	movq	%rdx, -88(%rbp)
	movq	%r9, -96(%rbp)
	cmpq	$128, %rdx
	jbe	.L1942
	movq	%rdi, %rax
	movq	%rdi, -112(%rbp)
	movq	%r8, %rbx
	shrq	$2, %rax
	movq	%rax, %rdx
	movq	%rax, -144(%rbp)
	andl	$15, %edx
	jne	.L1943
	movq	-88(%rbp), %r14
	movq	%rdi, %rax
.L1794:
	movq	8(%rbx), %rsi
	movq	16(%rbx), %r11
	movq	%rsi, %rdi
	leaq	1(%r11), %r9
	leaq	(%rsi,%rsi,8), %rcx
	xorq	(%rbx), %r9
	shrq	$11, %rdi
	rorx	$40, %rsi, %rdx
	leaq	2(%r11), %rsi
	addq	%r9, %rdx
	xorq	%rdi, %rcx
	movq	%rdx, %r8
	rorx	$40, %rdx, %rdi
	xorq	%rsi, %rcx
	shrq	$11, %r8
	leaq	(%rdx,%rdx,8), %rsi
	leaq	3(%r11), %rdx
	addq	%rcx, %rdi
	xorq	%r8, %rsi
	movq	%rdi, %r8
	xorq	%rdx, %rsi
	leaq	(%rdi,%rdi,8), %rdx
	rorx	$40, %rdi, %r15
	shrq	$11, %r8
	leaq	4(%r11), %rdi
	addq	%rsi, %r15
	addq	$5, %r11
	xorq	%r8, %rdx
	rorx	$40, %r15, %r10
	leaq	(%r15,%r15,8), %r8
	movq	%r11, 16(%rbx)
	xorq	%rdi, %rdx
	movq	%r15, %rdi
	addq	%rdx, %r10
	shrq	$11, %rdi
	movq	%r10, %r15
	xorq	%rdi, %r8
	leaq	(%r10,%r10,8), %rdi
	rorx	$40, %r10, %r10
	shrq	$11, %r15
	xorq	%r11, %r8
	movl	%esi, %r11d
	xorq	%r15, %rdi
	addq	%r8, %r10
	movl	%ecx, %r15d
	movl	%r8d, %r8d
	vmovq	%rdi, %xmm5
	movq	%r14, %rdi
	vpinsrq	$1, %r10, %xmm5, %xmm0
	shrq	$4, %rdi
	movabsq	$68719476719, %r10
	cmpq	%r10, %r14
	movl	$4294967295, %r10d
	movl	%r9d, %r14d
	vmovdqu	%xmm0, (%rbx)
	cmova	%r10, %rdi
	shrq	$32, %r9
	movl	%edx, %r10d
	shrq	$32, %rcx
	imulq	%rdi, %r15
	shrq	$32, %rsi
	shrq	$32, %rdx
	imulq	%rdi, %r8
	imulq	%rdi, %r14
	shrq	$32, %r15
	imulq	%rdi, %r9
	imulq	%rdi, %rcx
	imulq	%rdi, %r11
	shrq	$32, %r14
	imulq	%rdi, %rsi
	shrq	$32, %r9
	imulq	%rdi, %r10
	shrq	$32, %rcx
	imulq	%rdi, %rdx
	movq	%r8, %rdi
	movq	%r15, %r8
	shrq	$32, %r11
	salq	$6, %r8
	shrq	$32, %rsi
	vmovdqa	(%rax,%r8), %ymm1
	shrq	$32, %rdi
	movq	%r14, %r8
	shrq	$32, %r10
	salq	$6, %r8
	shrq	$32, %rdx
	vmovdqa	(%rax,%r8), %ymm0
	movq	%r9, %r8
	salq	$4, %r15
	salq	$6, %r8
	vmovdqa	32(%rax,%r15,4), %ymm3
	vpminsd	%ymm1, %ymm0, %ymm2
	vpmaxsd	(%rax,%r8), %ymm2, %ymm2
	movq	%rsi, %r8
	salq	$6, %r8
	vpmaxsd	%ymm1, %ymm0, %ymm0
	vmovdqa	(%rax,%r8), %ymm1
	movq	%rcx, %r8
	vpminsd	%ymm0, %ymm2, %ymm2
	salq	$6, %r8
	vmovdqa	%ymm2, (%r12)
	vmovdqa	(%rax,%r8), %ymm0
	movq	%r11, %r8
	salq	$6, %r8
	vpminsd	%ymm1, %ymm0, %ymm7
	vpmaxsd	(%rax,%r8), %ymm7, %ymm7
	movq	%rdi, %r8
	salq	$6, %r8
	vpmaxsd	%ymm1, %ymm0, %ymm0
	vmovdqa	(%rax,%r8), %ymm1
	movq	%r10, %r8
	vpminsd	%ymm0, %ymm7, %ymm7
	salq	$6, %r8
	vmovdqa	%ymm7, 64(%r12)
	vmovdqa	(%rax,%r8), %ymm0
	movq	%rdx, %r8
	salq	$6, %r8
	salq	$4, %r14
	vpminsd	%ymm1, %ymm0, %ymm6
	vpmaxsd	(%rax,%r8), %ymm6, %ymm6
	salq	$4, %r9
	salq	$4, %rsi
	vpmaxsd	%ymm1, %ymm0, %ymm0
	salq	$4, %rcx
	vmovdqa	32(%rax,%rsi,4), %ymm4
	salq	$4, %r11
	vpminsd	%ymm0, %ymm6, %ymm6
	vmovdqa	32(%rax,%r14,4), %ymm0
	salq	$4, %rdi
	salq	$4, %r10
	salq	$4, %rdx
	leaq	192(%r12), %r14
	vmovdqa	%ymm6, 128(%r12)
	vpminsd	%ymm3, %ymm0, %ymm1
	vpmaxsd	32(%rax,%r9,4), %ymm1, %ymm1
	vpmaxsd	%ymm3, %ymm0, %ymm0
	vpminsd	%ymm0, %ymm1, %ymm1
	vmovdqa	32(%rax,%rcx,4), %ymm0
	vmovdqa	%ymm1, 32(%r12)
	vpminsd	%ymm4, %ymm0, %ymm3
	vpmaxsd	32(%rax,%r11,4), %ymm3, %ymm3
	vpmaxsd	%ymm4, %ymm0, %ymm0
	vmovdqa	32(%rax,%rdi,4), %ymm4
	vpminsd	%ymm0, %ymm3, %ymm3
	vmovdqa	32(%rax,%r10,4), %ymm0
	vmovdqa	%ymm3, 96(%r12)
	vpminsd	%ymm4, %ymm0, %ymm5
	vpmaxsd	32(%rax,%rdx,4), %ymm5, %ymm5
	vpmaxsd	%ymm4, %ymm0, %ymm0
	vpminsd	%ymm0, %ymm5, %ymm5
	vpbroadcastd	%xmm2, %ymm0
	vpxor	%ymm2, %ymm0, %ymm2
	vpxor	%ymm1, %ymm0, %ymm1
	vpxor	%ymm7, %ymm0, %ymm7
	vmovdqa	%ymm5, 160(%r12)
	vpor	%ymm2, %ymm1, %ymm1
	vpxor	%ymm3, %ymm0, %ymm3
	vpxor	%ymm6, %ymm0, %ymm6
	vpor	%ymm7, %ymm1, %ymm1
	vpxor	%ymm5, %ymm0, %ymm5
	vmovdqa	%ymm0, %ymm4
	vpxor	192(%r12), %ymm0, %ymm2
	vpor	%ymm3, %ymm1, %ymm1
	vpxor	%xmm3, %xmm3, %xmm3
	vpor	%ymm6, %ymm1, %ymm1
	vpor	%ymm5, %ymm1, %ymm1
	vpor	%ymm1, %ymm2, %ymm2
	vpblendvb	%ymm3, %ymm2, %ymm1, %ymm1
	vpxor	%xmm2, %xmm2, %xmm2
	vpcmpeqd	%ymm2, %ymm1, %ymm1
	vmovmskps	%ymm1, %eax
	cmpl	$255, %eax
	je	.L1796
	vmovdqa	.LC14(%rip), %ymm0
	movl	$4, %esi
	movq	%r12, %rdi
	vmovdqu	%ymm0, 192(%r12)
	vmovdqu	%ymm0, 224(%r12)
	vmovdqu	%ymm0, 256(%r12)
	vzeroupper
	call	_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	vpbroadcastd	(%r12), %ymm0
	vpcmpeqd	%ymm2, %ymm2, %ymm2
	vpbroadcastd	188(%r12), %ymm1
	vpaddd	%ymm2, %ymm1, %ymm2
	vpcmpeqd	%ymm0, %ymm2, %ymm2
	vmovmskps	%ymm2, %eax
	cmpl	$255, %eax
	jne	.L1798
	movq	-88(%rbp), %rsi
	leaq	-80(%rbp), %rdx
	movq	%r14, %rcx
	movq	%r13, %rdi
	call	_ZN3hwy6N_AVX26detail22MaybePartitionTwoValueINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L1937
.L1798:
	movl	96(%r12), %ecx
	movl	$23, %eax
	movl	$24, %edx
	cmpl	92(%r12), %ecx
	je	.L1832
	jmp	.L1837
	.p2align 4,,10
	.p2align 3
.L1835:
	testq	%rax, %rax
	je	.L1944
.L1832:
	movq	%rax, %rdx
	subq	$1, %rax
	movl	(%r12,%rax,4), %esi
	cmpl	%esi, %ecx
	je	.L1835
	cmpl	%ecx, (%r12,%rdx,4)
	je	.L1837
	movl	%esi, %ecx
	jmp	.L1834
	.p2align 4,,10
	.p2align 3
.L1838:
	cmpq	$47, %rdx
	je	.L1940
.L1837:
	movq	%rdx, %rsi
	addq	$1, %rdx
	cmpl	(%r12,%rdx,4), %ecx
	je	.L1838
	movl	$24, %edx
	subq	$23, %rsi
	subq	%rax, %rdx
	cmpq	%rdx, %rsi
	jb	.L1834
.L1940:
	movl	(%r12,%rax,4), %ecx
.L1834:
	vmovd	%ecx, %xmm0
	vpbroadcastd	%xmm0, %ymm0
.L1941:
	movl	$1, -112(%rbp)
.L1831:
	cmpq	$0, -96(%rbp)
	je	.L1945
	movq	-88(%rbp), %rax
	vmovdqa	%ymm0, %ymm1
	leaq	-8(%rax), %r10
	movq	%r10, %rdx
	movq	%r10, %rsi
	vmovdqu	0(%r13,%r10,4), %ymm5
	andl	$31, %edx
	andl	$24, %esi
	je	.L1880
	vmovdqu	0(%r13), %ymm3
	vpcmpeqd	%ymm4, %ymm4, %ymm4
	leaq	_ZZN3hwy6N_AVX26detail18IndicesFromBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array(%rip), %r9
	xorl	%ecx, %ecx
	vpcmpgtd	%ymm0, %ymm3, %ymm6
	vpxor	%ymm4, %ymm6, %ymm0
	vmovmskps	%ymm6, %esi
	vmovmskps	%ymm0, %eax
	vmovdqa	.LC17(%rip), %ymm0
	popcntq	%rsi, %rcx
	vpbroadcastd	(%r9,%rax,4), %ymm2
	popcntq	%rax, %rax
	leaq	0(%r13,%rax,4), %rax
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpslld	$28, %ymm2, %ymm7
	vpermd	%ymm3, %ymm2, %ymm2
	vpmaskmovd	%ymm2, %ymm7, 0(%r13)
	vpbroadcastd	(%r9,%rsi,4), %ymm2
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpermd	%ymm3, %ymm2, %ymm3
	vmovdqu	%ymm3, (%r12)
	testb	$16, %r10b
	je	.L1842
	vmovdqu	32(%r13), %ymm3
	vpcmpgtd	%ymm1, %ymm3, %ymm6
	vpxor	%ymm4, %ymm6, %ymm2
	vmovmskps	%ymm2, %esi
	vpbroadcastd	(%r9,%rsi,4), %ymm2
	popcntq	%rsi, %rsi
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpslld	$28, %ymm2, %ymm7
	vpermd	%ymm3, %ymm2, %ymm2
	vpmaskmovd	%ymm2, %ymm7, (%rax)
	leaq	(%rax,%rsi,4), %rax
	vmovmskps	%ymm6, %esi
	vpbroadcastd	(%r9,%rsi,4), %ymm2
	popcntq	%rsi, %rsi
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpermd	%ymm3, %ymm2, %ymm3
	vmovdqu	%ymm3, (%r12,%rcx,4)
	addq	%rsi, %rcx
	cmpq	$23, %rdx
	jbe	.L1842
	vmovdqu	64(%r13), %ymm3
	vpcmpgtd	%ymm1, %ymm3, %ymm6
	vpxor	%ymm4, %ymm6, %ymm2
	vmovmskps	%ymm2, %esi
	vpbroadcastd	(%r9,%rsi,4), %ymm2
	popcntq	%rsi, %rsi
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpslld	$28, %ymm2, %ymm4
	vpermd	%ymm3, %ymm2, %ymm2
	vpmaskmovd	%ymm2, %ymm4, (%rax)
	leaq	(%rax,%rsi,4), %rax
	vmovmskps	%ymm6, %esi
	vpbroadcastd	(%r9,%rsi,4), %ymm2
	popcntq	%rsi, %rsi
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpermd	%ymm3, %ymm2, %ymm3
	vmovdqu	%ymm3, (%r12,%rcx,4)
	addq	%rsi, %rcx
.L1842:
	leaq	-8(%rdx), %rsi
	leaq	1(%rdx), %rdi
	andq	$-8, %rsi
	leaq	0(,%rcx,4), %r8
	addq	$8, %rsi
	cmpq	$8, %rdi
	movl	$8, %edi
	cmovbe	%rdi, %rsi
.L1841:
	cmpq	%rdx, %rsi
	je	.L1843
	subq	%rsi, %rdx
	vmovdqu	0(%r13,%rsi,4), %ymm4
	vmovd	%edx, %xmm2
	vpbroadcastd	%xmm2, %ymm2
	vpcmpgtd	%ymm1, %ymm4, %ymm6
	vpcmpgtd	.LC3(%rip), %ymm2, %ymm2
	vpandn	%ymm2, %ymm6, %ymm3
	vpand	%ymm2, %ymm6, %ymm6
	vmovmskps	%ymm3, %edx
	vpbroadcastd	(%r9,%rdx,4), %ymm3
	popcntq	%rdx, %rdx
	vpsrlvd	%ymm0, %ymm3, %ymm3
	vpslld	$28, %ymm3, %ymm7
	vpermd	%ymm4, %ymm3, %ymm3
	vpmaskmovd	%ymm3, %ymm7, (%rax)
	leaq	(%rax,%rdx,4), %rax
	vmovmskps	%ymm6, %edx
	vpbroadcastd	(%r9,%rdx,4), %ymm2
	popcntq	%rdx, %rdx
	addq	%rdx, %rcx
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpermd	%ymm4, %ymm2, %ymm4
	vmovdqu	%ymm4, (%r12,%r8)
	leaq	0(,%rcx,4), %r8
.L1843:
	movq	%r10, %rdx
	subq	%rcx, %rdx
	movl	%r8d, %ecx
	leaq	0(%r13,%rdx,4), %r11
	cmpl	$8, %r8d
	jnb	.L1844
	testb	$4, %r8b
	jne	.L1946
	testl	%ecx, %ecx
	jne	.L1947
.L1845:
	movl	%r8d, %ecx
	cmpl	$8, %r8d
	jnb	.L1848
	andl	$4, %r8d
	jne	.L1948
	testl	%ecx, %ecx
	jne	.L1949
.L1849:
	movq	%rax, %rcx
	subq	%r13, %rcx
	sarq	$2, %rcx
	subq	%rcx, %r10
	subq	%rcx, %rdx
	movq	%rcx, %r15
	movq	%r10, -144(%rbp)
	leaq	(%rax,%rdx,4), %rcx
	je	.L1881
	leaq	128(%rax), %rsi
	leaq	-128(%rcx), %r8
	vmovdqu	(%rax), %ymm13
	vmovdqu	32(%rax), %ymm12
	vmovdqu	64(%rax), %ymm11
	vmovdqu	96(%rax), %ymm10
	vmovdqu	-128(%rcx), %ymm9
	vmovdqu	-96(%rcx), %ymm8
	vmovdqu	-64(%rcx), %ymm7
	vmovdqu	-32(%rcx), %ymm6
	cmpq	%r8, %rsi
	je	.L1882
	xorl	%ecx, %ecx
	leaq	_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array(%rip), %rdi
	movl	$8, %r10d
	jmp	.L1856
	.p2align 4,,10
	.p2align 3
.L1951:
	vmovdqu	-128(%r8), %ymm14
	vmovdqu	-96(%r8), %ymm4
	prefetcht0	-512(%r8)
	addq	$-128, %r8
	vmovdqu	64(%r8), %ymm3
	vmovdqu	96(%r8), %ymm2
.L1855:
	vpcmpgtd	%ymm1, %ymm14, %ymm15
	leaq	-8(%rdx,%rcx), %r14
	vmovmskps	%ymm15, %r11d
	vpbroadcastd	(%rdi,%r11,4), %ymm15
	popcntq	%r11, %r11
	vpsrlvd	%ymm0, %ymm15, %ymm15
	vpermd	%ymm14, %ymm15, %ymm14
	vmovdqu	%ymm14, (%rax,%rcx,4)
	addq	$8, %rcx
	vmovdqu	%ymm14, (%rax,%r14,4)
	vpcmpgtd	%ymm1, %ymm4, %ymm14
	subq	%r11, %rcx
	leaq	-16(%rdx,%rcx), %r14
	vmovmskps	%ymm14, %r11d
	vpbroadcastd	(%rdi,%r11,4), %ymm14
	popcntq	%r11, %r11
	vpsrlvd	%ymm0, %ymm14, %ymm14
	vpermd	%ymm4, %ymm14, %ymm4
	vmovdqu	%ymm4, (%rax,%rcx,4)
	vmovdqu	%ymm4, (%rax,%r14,4)
	vpcmpgtd	%ymm1, %ymm3, %ymm4
	movq	%r10, %r14
	subq	%r11, %r14
	addq	%r14, %rcx
	vmovmskps	%ymm4, %r11d
	leaq	-24(%rdx,%rcx), %r14
	subq	$32, %rdx
	vpbroadcastd	(%rdi,%r11,4), %ymm4
	popcntq	%r11, %r11
	vpsrlvd	%ymm0, %ymm4, %ymm4
	vpermd	%ymm3, %ymm4, %ymm3
	vmovdqu	%ymm3, (%rax,%rcx,4)
	vmovdqu	%ymm3, (%rax,%r14,4)
	vpcmpgtd	%ymm1, %ymm2, %ymm3
	movq	%r10, %r14
	subq	%r11, %r14
	leaq	(%r14,%rcx), %r11
	vmovmskps	%ymm3, %ecx
	leaq	(%r11,%rdx), %r14
	vpbroadcastd	(%rdi,%rcx,4), %ymm3
	popcntq	%rcx, %rcx
	vpsrlvd	%ymm0, %ymm3, %ymm3
	vpermd	%ymm2, %ymm3, %ymm2
	vmovdqu	%ymm2, (%rax,%r11,4)
	vmovdqu	%ymm2, (%rax,%r14,4)
	movq	%r10, %r14
	subq	%rcx, %r14
	leaq	(%r14,%r11), %rcx
	cmpq	%r8, %rsi
	je	.L1950
.L1856:
	movq	%rsi, %r11
	subq	%rax, %r11
	sarq	$2, %r11
	subq	%rcx, %r11
	cmpq	$32, %r11
	ja	.L1951
	vmovdqu	(%rsi), %ymm14
	vmovdqu	32(%rsi), %ymm4
	prefetcht0	512(%rsi)
	subq	$-128, %rsi
	vmovdqu	-64(%rsi), %ymm3
	vmovdqu	-32(%rsi), %ymm2
	jmp	.L1855
	.p2align 4,,10
	.p2align 3
.L1943:
	movl	$16, %eax
	subq	%rdx, %rax
	leaq	(%rdi,%rax,4), %rax
	movq	-88(%rbp), %rdi
	leaq	-16(%rdx,%rdi), %r14
	jmp	.L1794
	.p2align 4,,10
	.p2align 3
.L1950:
	leaq	(%rdx,%rcx), %rsi
	leaq	(%rax,%rcx,4), %r8
	addq	$8, %rcx
.L1853:
	vpcmpgtd	%ymm1, %ymm13, %ymm2
	vmovmskps	%ymm2, %r10d
	vpbroadcastd	(%rdi,%r10,4), %ymm2
	popcntq	%r10, %r10
	subq	%r10, %rcx
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpermd	%ymm13, %ymm2, %ymm13
	vpcmpgtd	%ymm1, %ymm12, %ymm2
	vmovdqu	%ymm13, (%r8)
	leaq	-16(%rdx,%rcx), %r8
	vmovdqu	%ymm13, -32(%rax,%rsi,4)
	vmovmskps	%ymm2, %esi
	vpbroadcastd	(%rdi,%rsi,4), %ymm2
	popcntq	%rsi, %rsi
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpermd	%ymm12, %ymm2, %ymm12
	vpcmpgtd	%ymm1, %ymm11, %ymm2
	vmovdqu	%ymm12, (%rax,%rcx,4)
	subq	%rsi, %rcx
	vmovdqu	%ymm12, (%rax,%r8,4)
	addq	$8, %rcx
	vmovmskps	%ymm2, %r8d
	leaq	-24(%rdx,%rcx), %rsi
	vpbroadcastd	(%rdi,%r8,4), %ymm2
	popcntq	%r8, %r8
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpermd	%ymm11, %ymm2, %ymm11
	vpcmpgtd	%ymm1, %ymm10, %ymm2
	vmovdqu	%ymm11, (%rax,%rcx,4)
	vmovdqu	%ymm11, (%rax,%rsi,4)
	movl	$8, %esi
	movq	%rsi, %r10
	subq	%r8, %r10
	vmovmskps	%ymm2, %r8d
	vpbroadcastd	(%rdi,%r8,4), %ymm2
	addq	%r10, %rcx
	popcntq	%r8, %r8
	leaq	-32(%rdx,%rcx), %r10
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpermd	%ymm10, %ymm2, %ymm10
	vpcmpgtd	%ymm1, %ymm9, %ymm2
	vmovdqu	%ymm10, (%rax,%rcx,4)
	vmovdqu	%ymm10, (%rax,%r10,4)
	movq	%rsi, %r10
	subq	%r8, %r10
	leaq	(%r10,%rcx), %r8
	vmovmskps	%ymm2, %ecx
	vpbroadcastd	(%rdi,%rcx,4), %ymm2
	leaq	-40(%rdx,%r8), %r10
	popcntq	%rcx, %rcx
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpermd	%ymm9, %ymm2, %ymm9
	vpcmpgtd	%ymm1, %ymm8, %ymm2
	vmovdqu	%ymm9, (%rax,%r8,4)
	vmovdqu	%ymm9, (%rax,%r10,4)
	movq	%rsi, %r10
	subq	%rcx, %r10
	leaq	(%r10,%r8), %rcx
	vmovmskps	%ymm2, %r8d
	vpbroadcastd	(%rdi,%r8,4), %ymm2
	leaq	-48(%rdx,%rcx), %r10
	popcntq	%r8, %r8
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpermd	%ymm8, %ymm2, %ymm8
	vpcmpgtd	%ymm1, %ymm7, %ymm2
	vmovdqu	%ymm8, (%rax,%rcx,4)
	vmovdqu	%ymm8, (%rax,%r10,4)
	movq	%rsi, %r10
	subq	%r8, %r10
	leaq	(%r10,%rcx), %r8
	vmovmskps	%ymm2, %ecx
	vpbroadcastd	(%rdi,%rcx,4), %ymm2
	leaq	-56(%rdx,%r8), %r10
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpermd	%ymm7, %ymm2, %ymm7
	vpcmpgtd	%ymm1, %ymm6, %ymm2
	vmovdqu	%ymm7, (%rax,%r8,4)
	vmovdqu	%ymm7, (%rax,%r10,4)
	xorl	%r10d, %r10d
	popcntq	%rcx, %r10
	movq	%rsi, %rcx
	subq	%r10, %rcx
	addq	%r8, %rcx
	vmovmskps	%ymm2, %r8d
	vpbroadcastd	(%rdi,%r8,4), %ymm2
	leaq	-64(%rdx,%rcx), %rdx
	popcntq	%r8, %r8
	movq	-144(%rbp), %rdi
	subq	%r8, %rsi
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpermd	%ymm6, %ymm2, %ymm6
	vmovdqu	%ymm6, (%rax,%rcx,4)
	vmovdqu	%ymm6, (%rax,%rdx,4)
	leaq	(%rsi,%rcx), %rdx
	subq	%rdx, %rdi
	leaq	(%rax,%rdx,4), %rcx
.L1852:
	movq	%rcx, %rsi
	cmpq	$7, %rdi
	ja	.L1857
	movq	-144(%rbp), %rdi
	leaq	-32(%rax,%rdi,4), %rsi
.L1857:
	vpcmpgtd	%ymm1, %ymm5, %ymm1
	vpcmpeqd	%ymm2, %ymm2, %ymm2
	vmovdqu	(%rsi), %ymm7
	movq	-144(%rbp), %rdi
	vmovdqa	%ymm7, -176(%rbp)
	vpxor	%ymm2, %ymm1, %ymm2
	vmovdqu	%ymm7, (%rax,%rdi,4)
	vmovmskps	%ymm2, %esi
	vpbroadcastd	(%r9,%rsi,4), %ymm2
	popcntq	%rsi, %rsi
	addq	%rsi, %rdx
	leaq	(%r15,%rdx), %r14
	vpsrlvd	%ymm0, %ymm2, %ymm2
	vpslld	$28, %ymm2, %ymm3
	vpermd	%ymm5, %ymm2, %ymm2
	vpmaskmovd	%ymm2, %ymm3, (%rcx)
	vmovmskps	%ymm1, %ecx
	vpbroadcastd	(%r9,%rcx,4), %ymm1
	vpsrlvd	%ymm0, %ymm1, %ymm0
	vpslld	$28, %ymm0, %ymm1
	vpermd	%ymm5, %ymm0, %ymm5
	vpmaskmovd	%ymm5, %ymm1, (%rax,%rdx,4)
	movq	-96(%rbp), %r15
	subq	$1, %r15
	cmpl	$2, -112(%rbp)
	je	.L1952
	movq	-104(%rbp), %rsi
	movq	%r15, %r9
	movq	%rbx, %r8
	movq	%r12, %rcx
	movq	%r14, %rdx
	movq	%r13, %rdi
	vzeroupper
	call	_ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	cmpl	$3, -112(%rbp)
	je	.L1937
.L1859:
	movq	-88(%rbp), %rdx
	movq	-104(%rbp), %rsi
	leaq	0(%r13,%r14,4), %rdi
	movq	%r15, %r9
	movq	%rbx, %r8
	movq	%r12, %rcx
	subq	%r14, %rdx
	call	_ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L1937:
	subq	$-128, %rsp
	popq	%rbx
	popq	%r10
	.cfi_remember_state
	.cfi_def_cfa 10, 0
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	leaq	-8(%r10), %rsp
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L1848:
	.cfi_restore_state
	movq	(%r12), %rcx
	leaq	8(%r11), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%r11)
	movl	%r8d, %ecx
	movq	-8(%r12,%rcx), %rsi
	movq	%rsi, -8(%r11,%rcx)
	subq	%rdi, %r11
	movq	%r12, %rsi
	leal	(%r8,%r11), %ecx
	subq	%r11, %rsi
	shrl	$3, %ecx
	rep movsq
	jmp	.L1849
	.p2align 4,,10
	.p2align 3
.L1844:
	movq	(%r11), %rcx
	leaq	8(%rax), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%rax)
	movl	%r8d, %ecx
	movq	-8(%r11,%rcx), %rsi
	movq	%rsi, -8(%rax,%rcx)
	movq	%rax, %rcx
	movq	%r11, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%r8d, %ecx
	shrl	$3, %ecx
	rep movsq
	jmp	.L1845
	.p2align 4,,10
	.p2align 3
.L1944:
	movl	(%r12), %ecx
	jmp	.L1834
	.p2align 4,,10
	.p2align 3
.L1949:
	movzbl	(%r12), %esi
	movb	%sil, (%r11)
	testb	$2, %cl
	je	.L1849
	movzwl	-2(%r12,%rcx), %esi
	movw	%si, -2(%r11,%rcx)
	jmp	.L1849
	.p2align 4,,10
	.p2align 3
.L1947:
	movzbl	(%r11), %esi
	movb	%sil, (%rax)
	testb	$2, %cl
	je	.L1845
	movzwl	-2(%r11,%rcx), %esi
	movw	%si, -2(%rax,%rcx)
	jmp	.L1845
	.p2align 4,,10
	.p2align 3
.L1942:
	cmpq	$1, %rdx
	jbe	.L1937
	leaq	512(%rdi), %rax
	cmpq	%rax, %rsi
	jb	.L1953
	movl	$8, %esi
	call	_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	jmp	.L1937
	.p2align 4,,10
	.p2align 3
.L1796:
	movq	-144(%rbp), %rax
	movl	$8, %edi
	vmovdqa	.LC3(%rip), %ymm5
	vpcmpeqd	0(%r13), %ymm0, %ymm1
	andl	$7, %eax
	subq	%rax, %rdi
	vmovd	%edi, %xmm2
	vpbroadcastd	%xmm2, %ymm2
	vpcmpgtd	%ymm5, %ymm2, %ymm2
	vpandn	%ymm2, %ymm1, %ymm1
	vmovmskps	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1954
	vpxor	%xmm2, %xmm2, %xmm2
	movq	-88(%rbp), %r8
	leaq	512(%r13,%rdi,4), %rsi
	vpxor	%xmm6, %xmm6, %xmm6
	vmovdqa	%ymm2, %ymm1
	.p2align 4,,10
	.p2align 3
.L1802:
	movq	%rdi, %rcx
	leaq	128(%rdi), %rdi
	cmpq	%rdi, %r8
	jb	.L1955
	leaq	-512(%rsi), %rax
.L1801:
	vpxor	(%rax), %ymm4, %ymm3
	leaq	64(%rax), %rdx
	vpor	%ymm1, %ymm3, %ymm1
	vpxor	32(%rax), %ymm4, %ymm3
	vpor	%ymm2, %ymm3, %ymm2
	vpxor	64(%rax), %ymm4, %ymm3
	vpor	%ymm1, %ymm3, %ymm1
	vpxor	96(%rax), %ymm4, %ymm3
	vpor	%ymm2, %ymm3, %ymm2
	vpxor	128(%rax), %ymm4, %ymm3
	vpor	%ymm1, %ymm3, %ymm1
	vpxor	160(%rax), %ymm4, %ymm3
	leaq	192(%rdx), %rax
	vpor	%ymm2, %ymm3, %ymm2
	vpxor	128(%rdx), %ymm4, %ymm3
	vpor	%ymm1, %ymm3, %ymm1
	vpxor	160(%rdx), %ymm4, %ymm3
	vpor	%ymm2, %ymm3, %ymm2
	cmpq	%rsi, %rax
	jne	.L1801
	vpor	%ymm2, %ymm1, %ymm3
	leaq	704(%rdx), %rsi
	vpcmpeqd	%ymm6, %ymm3, %ymm3
	vmovmskps	%ymm3, %eax
	cmpl	$255, %eax
	je	.L1802
	vpcmpeqd	0(%r13,%rcx,4), %ymm0, %ymm1
	vpcmpeqd	%ymm2, %ymm2, %ymm2
	vpxor	%ymm2, %ymm1, %ymm1
	vmovmskps	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1804
	.p2align 4,,10
	.p2align 3
.L1803:
	addq	$8, %rcx
	vpcmpeqd	0(%r13,%rcx,4), %ymm0, %ymm1
	vpxor	%ymm1, %ymm2, %ymm1
	vmovmskps	%ymm1, %eax
	testl	%eax, %eax
	je	.L1803
.L1804:
	tzcntl	%eax, %eax
	addq	%rcx, %rax
.L1800:
	vpbroadcastd	0(%r13,%rax,4), %ymm2
	vmovdqa	%ymm0, %ymm8
	leaq	0(%r13,%rax,4), %rdi
	vpcmpgtd	%ymm0, %ymm2, %ymm1
	vmovdqa	%ymm2, %ymm6
	vmovmskps	%ymm1, %edx
	testl	%edx, %edx
	jne	.L1809
	movq	-88(%rbp), %rsi
	xorl	%ecx, %ecx
	leaq	-8(%rsi), %rax
	jmp	.L1814
	.p2align 4,,10
	.p2align 3
.L1810:
	vmovmskps	%ymm1, %edx
	vmovdqu	%ymm8, 0(%r13,%rax,4)
	popcntq	%rdx, %rdx
	addq	%rdx, %rcx
	leaq	-8(%rax), %rdx
	cmpq	%rdx, %rsi
	jbe	.L1956
	movq	%rdx, %rax
.L1814:
	vpcmpeqd	0(%r13,%rax,4), %ymm2, %ymm3
	vpcmpeqd	0(%r13,%rax,4), %ymm0, %ymm1
	vpor	%ymm3, %ymm1, %ymm4
	vmovmskps	%ymm4, %edx
	cmpl	$255, %edx
	je	.L1810
	vpcmpeqd	%ymm0, %ymm0, %ymm0
	leaq	8(%rax), %rsi
	vpxor	%ymm0, %ymm1, %ymm7
	vpandn	%ymm7, %ymm3, %ymm3
	vmovmskps	%ymm3, %edx
	tzcntl	%edx, %edx
	addq	%rax, %rdx
	addq	$16, %rax
	vpbroadcastd	0(%r13,%rdx,4), %ymm0
	movq	-88(%rbp), %rdx
	subq	%rcx, %rdx
	vmovdqa	%ymm0, %ymm4
	vmovdqa	%ymm0, -80(%rbp)
	cmpq	%rax, %rdx
	jb	.L1811
	.p2align 4,,10
	.p2align 3
.L1812:
	vmovdqu	%ymm6, -32(%r13,%rax,4)
	movq	%rax, %rsi
	addq	$8, %rax
	cmpq	%rdx, %rax
	jbe	.L1812
.L1811:
	subq	%rsi, %rdx
	vmovd	%edx, %xmm0
	vpbroadcastd	%xmm0, %ymm0
	vpcmpgtd	%ymm5, %ymm0, %ymm0
	vpmaskmovd	%ymm2, %ymm0, 0(%r13,%rsi,4)
.L1813:
	vpbroadcastd	(%r12), %ymm3
	vpcmpeqd	.LC15(%rip), %ymm3, %ymm1
	vmovdqa	%ymm3, %ymm0
	vmovmskps	%ymm1, %eax
	cmpl	$255, %eax
	je	.L1877
	vpcmpeqd	.LC16(%rip), %ymm3, %ymm1
	vmovmskps	%ymm1, %eax
	cmpl	$255, %eax
	je	.L1827
	vpminsd	%ymm4, %ymm2, %ymm1
	vpcmpgtd	%ymm1, %ymm3, %ymm1
	vmovmskps	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1957
	vmovdqa	%ymm3, %ymm2
	movl	$128, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L1822:
	leaq	(%rcx,%rax,8), %rdx
	addq	$1, %rax
	vpminsd	0(%r13,%rdx,4), %ymm2, %ymm1
	vmovdqa	%ymm1, %ymm2
	cmpq	$16, %rax
	jne	.L1822
	vpcmpgtd	%ymm1, %ymm3, %ymm1
	vmovmskps	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1941
	leaq	128(%rsi), %rax
	cmpq	%rax, -88(%rbp)
	jb	.L1958
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L1822
.L1880:
	vmovdqa	.LC17(%rip), %ymm0
	xorl	%r8d, %r8d
	xorl	%ecx, %ecx
	movq	%r13, %rax
	leaq	_ZZN3hwy6N_AVX26detail18IndicesFromBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array(%rip), %r9
	jmp	.L1841
.L1952:
	vzeroupper
	jmp	.L1859
.L1881:
	movq	%r10, %rdi
	movq	%rax, %rcx
	jmp	.L1852
.L1882:
	movq	%rax, %r8
	movq	%rdx, %rsi
	movl	$8, %ecx
	leaq	_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array(%rip), %rdi
	jmp	.L1853
.L1945:
	movq	-88(%rbp), %rsi
	leaq	-1(%rsi), %rbx
	movq	%rbx, %r12
	shrq	%r12
	.p2align 4,,10
	.p2align 3
.L1839:
	movq	%r12, %rdx
	movq	%r13, %rdi
	call	_ZN3hwy6N_AVX26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	subq	$1, %r12
	jnb	.L1839
	.p2align 4,,10
	.p2align 3
.L1840:
	movl	0(%r13,%rbx,4), %edx
	movl	0(%r13), %eax
	movq	%rbx, %rsi
	movq	%r13, %rdi
	movl	%edx, 0(%r13)
	xorl	%edx, %edx
	movl	%eax, 0(%r13,%rbx,4)
	call	_ZN3hwy6N_AVX26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_mm.isra.0
	subq	$1, %rbx
	jne	.L1840
	jmp	.L1937
.L1946:
	movl	(%r11), %esi
	movl	%esi, (%rax)
	movl	-4(%r11,%rcx), %esi
	movl	%esi, -4(%rax,%rcx)
	jmp	.L1845
.L1948:
	movl	(%r12), %esi
	movl	%esi, (%r11)
	movl	-4(%r12,%rcx), %esi
	movl	%esi, -4(%r11,%rcx)
	jmp	.L1849
.L1955:
	movq	-88(%rbp), %rsi
	vpcmpeqd	%ymm2, %ymm2, %ymm2
	.p2align 4,,10
	.p2align 3
.L1806:
	movq	%rcx, %rdx
	addq	$8, %rcx
	cmpq	%rcx, %rsi
	jb	.L1959
	vpcmpeqd	-32(%r13,%rcx,4), %ymm0, %ymm1
	vpxor	%ymm2, %ymm1, %ymm1
	vmovmskps	%ymm1, %eax
	testl	%eax, %eax
	je	.L1806
.L1939:
	tzcntl	%eax, %eax
	addq	%rdx, %rax
	jmp	.L1800
.L1809:
	movq	-88(%rbp), %rsi
	leaq	-80(%rbp), %rdx
	movq	%r12, %rcx
	vmovdqa	%ymm2, %ymm1
	vmovdqa	%ymm2, -144(%rbp)
	subq	%rax, %rsi
	call	_ZN3hwy6N_AVX26detail22MaybePartitionTwoValueINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L1937
	vmovdqa	-80(%rbp), %ymm4
	vmovdqa	-144(%rbp), %ymm2
	jmp	.L1813
.L1956:
	vmovd	%eax, %xmm3
	vpcmpeqd	0(%r13), %ymm0, %ymm1
	movq	-88(%rbp), %rdx
	vpbroadcastd	%xmm3, %ymm3
	vpcmpeqd	0(%r13), %ymm2, %ymm4
	vpcmpgtd	%ymm5, %ymm3, %ymm3
	subq	%rcx, %rdx
	vpand	%ymm1, %ymm3, %ymm7
	vpor	%ymm4, %ymm1, %ymm1
	vpcmpeqd	%ymm4, %ymm4, %ymm4
	vpxor	%ymm3, %ymm4, %ymm3
	vpor	%ymm3, %ymm1, %ymm1
	vmovmskps	%ymm1, %esi
	cmpl	$255, %esi
	jne	.L1960
	vmovmskps	%ymm7, %ecx
	movq	%rdx, %rax
	vmovdqu	%ymm0, 0(%r13)
	popcntq	%rcx, %rcx
	subq	%rcx, %rax
	cmpq	$7, %rax
	jbe	.L1866
	leaq	-8(%rax), %rcx
	movq	-112(%rbp), %rsi
	movq	%rcx, %rdx
	shrq	$3, %rdx
	salq	$5, %rdx
	leaq	32(%r13,%rdx), %rdx
	.p2align 4,,10
	.p2align 3
.L1819:
	vmovdqu	%ymm6, (%rsi)
	addq	$32, %rsi
	cmpq	%rdx, %rsi
	jne	.L1819
	andq	$-8, %rcx
	leaq	8(%rcx), %rdx
	leaq	0(,%rdx,4), %rcx
	subq	%rdx, %rax
.L1818:
	vmovdqa	%ymm2, (%r12)
	testq	%rax, %rax
	je	.L1936
	leaq	0(%r13,%rcx), %rdi
	leaq	0(,%rax,4), %rdx
	movq	%r12, %rsi
	vzeroupper
	call	memcpy@PLT
	jmp	.L1937
.L1958:
	movq	-88(%rbp), %rdx
	jmp	.L1829
	.p2align 4,,10
	.p2align 3
.L1830:
	vpcmpgtd	-32(%r13,%rsi,4), %ymm3, %ymm1
	vmovmskps	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1941
.L1829:
	movq	%rsi, %rax
	addq	$8, %rsi
	cmpq	%rsi, %rdx
	jnb	.L1830
	movq	-88(%rbp), %rdi
	cmpq	%rax, %rdi
	je	.L1877
	vpcmpgtd	-32(%r13,%rdi,4), %ymm3, %ymm3
	vmovmskps	%ymm3, %eax
	cmpl	$1, %eax
	movl	$1, %eax
	adcl	$0, %eax
	movl	%eax, -112(%rbp)
	jmp	.L1831
.L1959:
	movq	-88(%rbp), %rax
	vpcmpeqd	%ymm2, %ymm2, %ymm2
	vpcmpeqd	-32(%r13,%rax,4), %ymm0, %ymm1
	leaq	-8(%rax), %rdx
	vpxor	%ymm2, %ymm1, %ymm1
	vmovmskps	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1939
.L1936:
	vzeroupper
	jmp	.L1937
.L1826:
	vmovdqu	-32(%r13,%rsi,4), %ymm5
	vpcmpgtd	%ymm3, %ymm5, %ymm1
	vmovmskps	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1941
.L1825:
	movq	%rsi, %rax
	addq	$8, %rsi
	cmpq	%rsi, -88(%rbp)
	jnb	.L1826
	movq	-88(%rbp), %rdi
	cmpq	%rax, %rdi
	je	.L1827
	vmovdqu	-32(%r13,%rdi,4), %ymm5
	vmovdqa	%ymm5, -144(%rbp)
	vmovdqa	-144(%rbp), %ymm5
	vpcmpgtd	%ymm3, %ymm5, %ymm1
	vmovmskps	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1941
.L1827:
	vpcmpeqd	%ymm0, %ymm0, %ymm0
	movl	$3, -112(%rbp)
	vpaddd	%ymm0, %ymm3, %ymm0
	jmp	.L1831
.L1954:
	tzcntl	%eax, %eax
	jmp	.L1800
.L1953:
	movq	%rdx, %rcx
	xorl	%eax, %eax
	cmpq	$7, %rdx
	jbe	.L1787
	movq	%rdx, %rbx
	leaq	-8(%rdx), %rdx
	movq	(%rdi), %rcx
	movq	%rdx, %rax
	shrq	$3, %rax
	movq	%rcx, (%r12)
	addq	$1, %rax
	salq	$5, %rax
	movl	%eax, %ecx
	movq	-8(%rdi,%rcx), %rsi
	leaq	8(%r12), %rdi
	andq	$-8, %rdi
	movq	%rsi, -8(%r12,%rcx)
	movq	%r12, %rcx
	movq	%r13, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%eax, %ecx
	movq	%rdx, %rax
	andq	$-8, %rax
	shrl	$3, %ecx
	rep movsq
	addq	$8, %rax
	subq	%rax, %rbx
	movq	%rbx, %rcx
	je	.L1790
.L1787:
	salq	$2, %rax
	leaq	0(,%rcx,4), %rdx
	testq	%rcx, %rcx
	movl	$4, %ecx
	cmove	%rcx, %rdx
	leaq	(%r12,%rax), %rdi
	leaq	0(%r13,%rax), %rsi
	call	memcpy@PLT
.L1790:
	movq	-88(%rbp), %rbx
	movl	$32, %edx
	movl	$1, %esi
	movl	%ebx, %eax
	subl	$1, %eax
	bsrl	%eax, %eax
	xorl	$31, %eax
	subl	%eax, %edx
	movl	$1, %eax
	shlx	%rdx, %rsi, %rsi
	shrq	$4, %rsi
	cmove	%rax, %rsi
	movq	%rsi, %rdx
	salq	$4, %rdx
	addq	$8, %rdx
	cmpq	%rdx, %rbx
	jnb	.L1789
	vmovdqa	.LC14(%rip), %ymm0
	movq	%rbx, %rax
.L1788:
	vmovdqu	%ymm0, (%r12,%rax,4)
	addq	$8, %rax
	cmpq	%rdx, %rax
	jb	.L1788
	vzeroupper
.L1789:
	movq	%r12, %rdi
	call	_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	xorl	%eax, %eax
	cmpq	$7, -88(%rbp)
	jbe	.L1792
	movq	-88(%rbp), %rbx
	movq	(%r12), %rcx
	leaq	8(%r13), %rdi
	andq	$-8, %rdi
	leaq	-8(%rbx), %rdx
	movq	%rcx, 0(%r13)
	movq	%rdx, %rax
	shrq	$3, %rax
	addq	$1, %rax
	salq	$5, %rax
	movl	%eax, %ecx
	movq	-8(%r12,%rcx), %rsi
	movq	%rsi, -8(%r13,%rcx)
	movq	%r13, %rcx
	movq	%r12, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%eax, %ecx
	movq	%rdx, %rax
	andq	$-8, %rax
	shrl	$3, %ecx
	rep movsq
	addq	$8, %rax
	subq	%rax, %rbx
	movq	%rbx, -88(%rbp)
	je	.L1937
.L1792:
	movq	-88(%rbp), %rbx
	salq	$2, %rax
	movl	$4, %ecx
	leaq	0(%r13,%rax), %rdi
	leaq	(%r12,%rax), %rsi
	leaq	0(,%rbx,4), %rdx
	testq	%rbx, %rbx
	cmove	%rcx, %rdx
	call	memcpy@PLT
	jmp	.L1937
.L1877:
	movl	$2, -112(%rbp)
	jmp	.L1831
.L1957:
	vpmaxsd	%ymm4, %ymm2, %ymm1
	vpcmpgtd	%ymm3, %ymm1, %ymm1
	vmovmskps	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1941
	vmovdqa	%ymm3, %ymm2
	movl	$128, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L1823:
	leaq	(%rcx,%rax,8), %rdx
	addq	$1, %rax
	vpmaxsd	0(%r13,%rdx,4), %ymm2, %ymm1
	vmovdqa	%ymm1, %ymm2
	cmpq	$16, %rax
	jne	.L1823
	vpcmpgtd	%ymm3, %ymm1, %ymm1
	vmovmskps	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1941
	leaq	128(%rsi), %rax
	cmpq	%rax, -88(%rbp)
	jb	.L1825
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L1823
.L1960:
	vpxor	%ymm4, %ymm1, %ymm1
	vmovmskps	%ymm1, %ecx
	tzcntl	%ecx, %ecx
	vpbroadcastd	0(%r13,%rcx,4), %ymm0
	leaq	8(%rax), %rcx
	vmovdqa	%ymm0, %ymm4
	vmovdqa	%ymm0, -80(%rbp)
	cmpq	%rdx, %rcx
	ja	.L1816
.L1817:
	vmovdqu	%ymm6, -32(%r13,%rcx,4)
	movq	%rcx, %rax
	addq	$8, %rcx
	cmpq	%rdx, %rcx
	jbe	.L1817
.L1816:
	subq	%rax, %rdx
	vmovd	%edx, %xmm0
	vpbroadcastd	%xmm0, %ymm0
	vpcmpgtd	%ymm5, %ymm0, %ymm0
	vpmaskmovd	%ymm2, %ymm0, 0(%r13,%rax,4)
	jmp	.L1813
.L1866:
	xorl	%ecx, %ecx
	jmp	.L1818
	.cfi_endproc
.LFE18809:
	.size	_ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0, .-_ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	.section	.text._ZN3hwy7N_SSSE310SortI32AscEPim,"ax",@progbits
	.p2align 4
	.globl	_ZN3hwy7N_SSSE310SortI32AscEPim
	.hidden	_ZN3hwy7N_SSSE310SortI32AscEPim
	.type	_ZN3hwy7N_SSSE310SortI32AscEPim, @function
_ZN3hwy7N_SSSE310SortI32AscEPim:
.LFB2946:
	.cfi_startproc
	cmpq	$1, %rsi
	jbe	.L1983
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r14
	.cfi_offset 14, -24
	leaq	(%rdi,%rsi,4), %r14
	pushq	%r13
	.cfi_offset 13, -32
	movq	%rdi, %r13
	pushq	%r12
	.cfi_offset 12, -40
	movq	%rsi, %r12
	subq	$296, %rsp
	cmpq	$64, %rsi
	jbe	.L1986
	call	_ZN3hwy17GetGeneratorStateEv@PLT
	movq	%r12, %rdx
	movq	%r14, %rsi
	movq	%r13, %rdi
	movq	%rax, %r8
	leaq	-320(%rbp), %rcx
	movl	$50, %r9d
	call	_ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L1961:
	addq	$296, %rsp
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L1983:
	.cfi_restore 6
	.cfi_restore 12
	.cfi_restore 13
	.cfi_restore 14
	ret
	.p2align 4,,10
	.p2align 3
.L1986:
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	leaq	256(%rdi), %rax
	cmpq	%rax, %r14
	jb	.L1987
	movl	$4, %esi
	call	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	jmp	.L1961
.L1987:
	movq	%rsi, %rcx
	xorl	%eax, %eax
	leaq	-320(%rbp), %r14
	cmpq	$3, %rsi
	jbe	.L1967
	leaq	-4(%rsi), %rax
	leaq	-320(%rbp), %r14
	movq	%r13, %rsi
	movq	%rax, %rdx
	andq	$-4, %rax
	movq	%r14, %rdi
	shrq	$2, %rdx
	addq	$4, %rax
	leal	2(%rdx,%rdx), %ecx
	andl	$536870910, %ecx
	rep movsq
	movq	%r12, %rcx
	subq	%rax, %rcx
	je	.L1973
.L1967:
	salq	$2, %rax
	leaq	0(,%rcx,4), %rdx
	testq	%rcx, %rcx
	movl	$4, %ecx
	cmove	%rcx, %rdx
	leaq	(%r14,%rax), %rdi
	leaq	0(%r13,%rax), %rsi
	call	memcpy@PLT
.L1973:
	leal	-1(%r12), %eax
	movl	$32, %ecx
	movl	$1, %esi
	bsrl	%eax, %eax
	xorl	$31, %eax
	subl	%eax, %ecx
	movl	$1, %eax
	salq	%cl, %rsi
	shrq	$4, %rsi
	cmove	%rax, %rsi
	movq	%rsi, %rdx
	salq	$4, %rdx
	addq	$4, %rdx
	cmpq	%rdx, %r12
	jnb	.L1972
	movdqa	.LC4(%rip), %xmm0
	movq	%r12, %rax
	.p2align 4,,10
	.p2align 3
.L1971:
	movups	%xmm0, (%r14,%rax,4)
	addq	$4, %rax
	cmpq	%rdx, %rax
	jb	.L1971
.L1972:
	movq	%r14, %rdi
	call	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	xorl	%eax, %eax
	cmpq	$3, %r12
	jbe	.L1975
	leaq	-4(%r12), %rdx
	movq	-320(%rbp), %rcx
	leaq	8(%r13), %rdi
	movq	%rdx, %rax
	andq	$-8, %rdi
	andq	$-4, %rdx
	shrq	$2, %rax
	movq	%rcx, 0(%r13)
	addq	$1, %rax
	salq	$4, %rax
	movl	%eax, %ecx
	movq	-8(%r14,%rcx), %rsi
	movq	%rsi, -8(%r13,%rcx)
	movq	%r13, %rcx
	movq	%r14, %rsi
	subq	%rdi, %rcx
	addl	%ecx, %eax
	subq	%rcx, %rsi
	shrl	$3, %eax
	movl	%eax, %ecx
	leaq	4(%rdx), %rax
	rep movsq
	subq	%rax, %r12
	je	.L1961
.L1975:
	salq	$2, %rax
	leaq	0(,%r12,4), %rdx
	testq	%r12, %r12
	movl	$4, %ecx
	cmove	%rcx, %rdx
	leaq	0(%r13,%rax), %rdi
	leaq	(%r14,%rax), %rsi
	call	memcpy@PLT
	jmp	.L1961
	.cfi_endproc
.LFE2946:
	.size	_ZN3hwy7N_SSSE310SortI32AscEPim, .-_ZN3hwy7N_SSSE310SortI32AscEPim
	.section	.text._ZN3hwy6N_SSE410SortI32AscEPim,"ax",@progbits
	.p2align 4
	.globl	_ZN3hwy6N_SSE410SortI32AscEPim
	.hidden	_ZN3hwy6N_SSE410SortI32AscEPim
	.type	_ZN3hwy6N_SSE410SortI32AscEPim, @function
_ZN3hwy6N_SSE410SortI32AscEPim:
.LFB4014:
	.cfi_startproc
	cmpq	$1, %rsi
	jbe	.L2010
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r14
	.cfi_offset 14, -24
	leaq	(%rdi,%rsi,4), %r14
	pushq	%r13
	.cfi_offset 13, -32
	movq	%rdi, %r13
	pushq	%r12
	.cfi_offset 12, -40
	movq	%rsi, %r12
	subq	$296, %rsp
	cmpq	$64, %rsi
	jbe	.L2013
	call	_ZN3hwy17GetGeneratorStateEv@PLT
	movq	%r12, %rdx
	movq	%r14, %rsi
	movq	%r13, %rdi
	movq	%rax, %r8
	leaq	-320(%rbp), %rcx
	movl	$50, %r9d
	call	_ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L1988:
	addq	$296, %rsp
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L2010:
	.cfi_restore 6
	.cfi_restore 12
	.cfi_restore 13
	.cfi_restore 14
	ret
	.p2align 4,,10
	.p2align 3
.L2013:
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	leaq	256(%rdi), %rax
	cmpq	%rax, %r14
	jb	.L2014
	movl	$4, %esi
	call	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	jmp	.L1988
.L2014:
	movq	%rsi, %rcx
	xorl	%eax, %eax
	leaq	-320(%rbp), %r14
	cmpq	$3, %rsi
	jbe	.L1994
	leaq	-4(%rsi), %rax
	leaq	-320(%rbp), %r14
	movq	%r13, %rsi
	movq	%rax, %rdx
	andq	$-4, %rax
	movq	%r14, %rdi
	shrq	$2, %rdx
	addq	$4, %rax
	leal	2(%rdx,%rdx), %ecx
	andl	$536870910, %ecx
	rep movsq
	movq	%r12, %rcx
	subq	%rax, %rcx
	je	.L2000
.L1994:
	salq	$2, %rax
	leaq	0(,%rcx,4), %rdx
	testq	%rcx, %rcx
	movl	$4, %ecx
	cmove	%rcx, %rdx
	leaq	(%r14,%rax), %rdi
	leaq	0(%r13,%rax), %rsi
	call	memcpy@PLT
.L2000:
	leal	-1(%r12), %eax
	movl	$32, %ecx
	movl	$1, %esi
	bsrl	%eax, %eax
	xorl	$31, %eax
	subl	%eax, %ecx
	movl	$1, %eax
	salq	%cl, %rsi
	shrq	$4, %rsi
	cmove	%rax, %rsi
	movq	%rsi, %rdx
	salq	$4, %rdx
	addq	$4, %rdx
	cmpq	%rdx, %r12
	jnb	.L1999
	movdqa	.LC4(%rip), %xmm0
	movq	%r12, %rax
	.p2align 4,,10
	.p2align 3
.L1998:
	movups	%xmm0, (%r14,%rax,4)
	addq	$4, %rax
	cmpq	%rdx, %rax
	jb	.L1998
.L1999:
	movq	%r14, %rdi
	call	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	xorl	%eax, %eax
	cmpq	$3, %r12
	jbe	.L2002
	leaq	-4(%r12), %rdx
	movq	-320(%rbp), %rcx
	leaq	8(%r13), %rdi
	movq	%rdx, %rax
	andq	$-8, %rdi
	andq	$-4, %rdx
	shrq	$2, %rax
	movq	%rcx, 0(%r13)
	addq	$1, %rax
	salq	$4, %rax
	movl	%eax, %ecx
	movq	-8(%r14,%rcx), %rsi
	movq	%rsi, -8(%r13,%rcx)
	movq	%r13, %rcx
	movq	%r14, %rsi
	subq	%rdi, %rcx
	addl	%ecx, %eax
	subq	%rcx, %rsi
	shrl	$3, %eax
	movl	%eax, %ecx
	leaq	4(%rdx), %rax
	rep movsq
	subq	%rax, %r12
	je	.L1988
.L2002:
	salq	$2, %rax
	leaq	0(,%r12,4), %rdx
	testq	%r12, %r12
	movl	$4, %ecx
	cmove	%rcx, %rdx
	leaq	0(%r13,%rax), %rdi
	leaq	(%r14,%rax), %rsi
	call	memcpy@PLT
	jmp	.L1988
	.cfi_endproc
.LFE4014:
	.size	_ZN3hwy6N_SSE410SortI32AscEPim, .-_ZN3hwy6N_SSE410SortI32AscEPim
	.section	.text._ZN3hwy6N_AVX210SortI32AscEPim,"ax",@progbits
	.p2align 4
	.globl	_ZN3hwy6N_AVX210SortI32AscEPim
	.hidden	_ZN3hwy6N_AVX210SortI32AscEPim
	.type	_ZN3hwy6N_AVX210SortI32AscEPim, @function
_ZN3hwy6N_AVX210SortI32AscEPim:
.LFB10439:
	.cfi_startproc
	cmpq	$1, %rsi
	jbe	.L2038
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r14
	.cfi_offset 14, -24
	leaq	(%rdi,%rsi,4), %r14
	pushq	%r13
	.cfi_offset 13, -32
	movq	%rdi, %r13
	pushq	%r12
	.cfi_offset 12, -40
	movq	%rsi, %r12
	andq	$-32, %rsp
	subq	$576, %rsp
	cmpq	$128, %rsi
	jbe	.L2041
	call	_ZN3hwy17GetGeneratorStateEv@PLT
	movq	%rsp, %rcx
	movq	%r12, %rdx
	movq	%r14, %rsi
	movq	%rax, %r8
	movl	$50, %r9d
	movq	%r13, %rdi
	call	_ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIiLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L2036:
	leaq	-24(%rbp), %rsp
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L2038:
	.cfi_restore 6
	.cfi_restore 12
	.cfi_restore 13
	.cfi_restore 14
	ret
	.p2align 4,,10
	.p2align 3
.L2041:
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	leaq	512(%rdi), %rax
	cmpq	%rax, %r14
	jb	.L2042
	movl	$8, %esi
	call	_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	jmp	.L2036
.L2042:
	movq	%rsi, %rcx
	xorl	%eax, %eax
	movq	%rsp, %r14
	cmpq	$7, %rsi
	jbe	.L2021
	leaq	-8(%rsi), %rax
	movq	%rsp, %r14
	movq	%r13, %rsi
	movq	%rax, %rdx
	andq	$-8, %rax
	movq	%r14, %rdi
	shrq	$3, %rdx
	addq	$8, %rax
	leal	4(,%rdx,4), %ecx
	andl	$536870908, %ecx
	rep movsq
	movq	%r12, %rcx
	subq	%rax, %rcx
	je	.L2027
.L2021:
	salq	$2, %rax
	leaq	0(,%rcx,4), %rdx
	testq	%rcx, %rcx
	movl	$4, %ecx
	cmove	%rcx, %rdx
	leaq	(%r14,%rax), %rdi
	leaq	0(%r13,%rax), %rsi
	call	memcpy@PLT
.L2027:
	leal	-1(%r12), %eax
	movl	$32, %edx
	movl	$1, %esi
	bsrl	%eax, %eax
	xorl	$31, %eax
	subl	%eax, %edx
	movl	$1, %eax
	shlx	%rdx, %rsi, %rsi
	shrq	$4, %rsi
	cmove	%rax, %rsi
	movq	%rsi, %rdx
	salq	$4, %rdx
	addq	$8, %rdx
	cmpq	%rdx, %r12
	jnb	.L2026
	vmovdqa	.LC14(%rip), %ymm0
	movq	%r12, %rax
	.p2align 4,,10
	.p2align 3
.L2025:
	vmovdqu	%ymm0, (%r14,%rax,4)
	addq	$8, %rax
	cmpq	%rdx, %rax
	jb	.L2025
	vzeroupper
.L2026:
	movq	%r14, %rdi
	call	_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	xorl	%eax, %eax
	cmpq	$7, %r12
	jbe	.L2029
	leaq	-8(%r12), %rdx
	movq	(%rsp), %rcx
	leaq	8(%r13), %rdi
	movq	%rdx, %rax
	andq	$-8, %rdi
	andq	$-8, %rdx
	shrq	$3, %rax
	movq	%rcx, 0(%r13)
	addq	$1, %rax
	salq	$5, %rax
	movl	%eax, %ecx
	movq	-8(%r14,%rcx), %rsi
	movq	%rsi, -8(%r13,%rcx)
	movq	%r13, %rcx
	movq	%r14, %rsi
	subq	%rdi, %rcx
	addl	%ecx, %eax
	subq	%rcx, %rsi
	shrl	$3, %eax
	movl	%eax, %ecx
	leaq	8(%rdx), %rax
	rep movsq
	subq	%rax, %r12
	je	.L2036
.L2029:
	salq	$2, %rax
	leaq	0(,%r12,4), %rdx
	testq	%r12, %r12
	movl	$4, %ecx
	cmove	%rcx, %rdx
	leaq	0(%r13,%rax), %rdi
	leaq	(%r14,%rax), %rsi
	call	memcpy@PLT
	jmp	.L2036
	.cfi_endproc
.LFE10439:
	.size	_ZN3hwy6N_AVX210SortI32AscEPim, .-_ZN3hwy6N_AVX210SortI32AscEPim
	.section	.text._ZN3hwy6N_AVX310SortI32AscEPim,"ax",@progbits
	.p2align 4
	.globl	_ZN3hwy6N_AVX310SortI32AscEPim
	.hidden	_ZN3hwy6N_AVX310SortI32AscEPim
	.type	_ZN3hwy6N_AVX310SortI32AscEPim, @function
_ZN3hwy6N_AVX310SortI32AscEPim:
.LFB12848:
	.cfi_startproc
	cmpq	$1, %rsi
	jbe	.L2062
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	leaq	(%rdi,%rsi,4), %r14
	pushq	%r13
	.cfi_offset 13, -40
	movq	%rdi, %r13
	pushq	%r12
	.cfi_offset 12, -48
	movq	%rsi, %r12
	pushq	%rbx
	andq	$-64, %rsp
	subq	$1152, %rsp
	.cfi_offset 3, -56
	cmpq	$256, %rsi
	jbe	.L2065
	call	_ZN3hwy17GetGeneratorStateEv@PLT
	movq	%rsp, %rcx
	movq	%r12, %rdx
	movq	%r14, %rsi
	movq	%rax, %r8
	movl	$50, %r9d
	movq	%r13, %rdi
	call	_ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L2060:
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L2062:
	.cfi_restore 3
	.cfi_restore 6
	.cfi_restore 12
	.cfi_restore 13
	.cfi_restore 14
	.cfi_restore 15
	ret
	.p2align 4,,10
	.p2align 3
.L2065:
	.cfi_def_cfa 6, 16
	.cfi_offset 3, -56
	.cfi_offset 6, -16
	.cfi_offset 12, -48
	.cfi_offset 13, -40
	.cfi_offset 14, -32
	.cfi_offset 15, -24
	leaq	1024(%rdi), %rax
	cmpq	%rax, %r14
	jb	.L2066
	movl	$16, %esi
	call	_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	jmp	.L2060
.L2066:
	xorl	%eax, %eax
	movq	%rsp, %r14
	cmpq	$15, %rsi
	jbe	.L2048
	leaq	-16(%rsi), %rax
	movq	%rsp, %r14
	movq	%r13, %rsi
	movq	%rax, %rdx
	movq	%r14, %rdi
	andq	$-16, %rax
	shrq	$4, %rdx
	addq	$16, %rax
	leal	8(,%rdx,8), %ecx
	andl	$536870904, %ecx
	rep movsq
.L2048:
	movq	%r12, %rdx
	leaq	0(,%rax,4), %rbx
	subq	%rax, %rdx
	movl	$65535, %eax
	leaq	(%r14,%rbx), %r15
	addq	%r13, %rbx
	kmovd	%eax, %k4
	cmpq	$255, %rdx
	ja	.L2052
	movq	$-1, %rax
	bzhi	%rdx, %rax, %rax
	movzwl	%ax, %eax
	kmovd	%eax, %k4
.L2052:
	leal	-1(%r12), %eax
	movl	$32, %edx
	movl	$1, %esi
	vmovdqu32	(%rbx), %zmm0{%k4}{z}
	bsrl	%eax, %eax
	xorl	$31, %eax
	vmovdqa32	%zmm0, (%r15){%k4}
	vpbroadcastq	.LC10(%rip), %zmm0
	subl	%eax, %edx
	movl	$1, %eax
	shlx	%rdx, %rsi, %rsi
	shrq	$4, %rsi
	cmove	%rax, %rsi
	movq	%r12, %rax
	leaq	1(%rsi), %rdx
	salq	$4, %rdx
	cmpq	%rdx, %r12
	jnb	.L2056
	.p2align 4,,10
	.p2align 3
.L2053:
	vmovdqu64	%zmm0, (%r14,%rax,4)
	addq	$16, %rax
	cmpq	%rax, %rdx
	ja	.L2053
.L2056:
	movq	%r14, %rdi
	vzeroupper
	call	_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	cmpq	$15, %r12
	jbe	.L2055
	leaq	-16(%r12), %rax
	movq	(%rsp), %rdx
	leaq	8(%r13), %rdi
	movq	%r14, %rsi
	shrq	$4, %rax
	andq	$-8, %rdi
	addq	$1, %rax
	movq	%rdx, 0(%r13)
	salq	$6, %rax
	movl	%eax, %edx
	movq	-8(%r14,%rdx), %rcx
	movq	%rcx, -8(%r13,%rdx)
	subq	%rdi, %r13
	addl	%r13d, %eax
	subq	%r13, %rsi
	shrl	$3, %eax
	movl	%eax, %ecx
	rep movsq
.L2055:
	vmovdqa32	(%r15), %zmm0{%k4}{z}
	vmovdqu32	%zmm0, (%rbx){%k4}
	vzeroupper
	jmp	.L2060
	.cfi_endproc
.LFE12848:
	.size	_ZN3hwy6N_AVX310SortI32AscEPim, .-_ZN3hwy6N_AVX310SortI32AscEPim
	.section	.text._ZN3hwy11N_AVX3_ZEN410SortI32AscEPim,"ax",@progbits
	.p2align 4
	.globl	_ZN3hwy11N_AVX3_ZEN410SortI32AscEPim
	.hidden	_ZN3hwy11N_AVX3_ZEN410SortI32AscEPim
	.type	_ZN3hwy11N_AVX3_ZEN410SortI32AscEPim, @function
_ZN3hwy11N_AVX3_ZEN410SortI32AscEPim:
.LFB15264:
	.cfi_startproc
	cmpq	$1, %rsi
	jbe	.L2086
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	leaq	(%rdi,%rsi,4), %r14
	pushq	%r13
	.cfi_offset 13, -40
	movq	%rdi, %r13
	pushq	%r12
	.cfi_offset 12, -48
	movq	%rsi, %r12
	pushq	%rbx
	andq	$-64, %rsp
	subq	$1152, %rsp
	.cfi_offset 3, -56
	cmpq	$256, %rsi
	jbe	.L2089
	call	_ZN3hwy17GetGeneratorStateEv@PLT
	movq	%rsp, %rcx
	movq	%r12, %rdx
	movq	%r14, %rsi
	movq	%rax, %r8
	movl	$50, %r9d
	movq	%r13, %rdi
	call	_ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIiLm16ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L2084:
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L2086:
	.cfi_restore 3
	.cfi_restore 6
	.cfi_restore 12
	.cfi_restore 13
	.cfi_restore 14
	.cfi_restore 15
	ret
	.p2align 4,,10
	.p2align 3
.L2089:
	.cfi_def_cfa 6, 16
	.cfi_offset 3, -56
	.cfi_offset 6, -16
	.cfi_offset 12, -48
	.cfi_offset 13, -40
	.cfi_offset 14, -32
	.cfi_offset 15, -24
	leaq	1024(%rdi), %rax
	cmpq	%rax, %r14
	jb	.L2090
	movl	$16, %esi
	call	_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	jmp	.L2084
.L2090:
	xorl	%eax, %eax
	movq	%rsp, %r14
	cmpq	$15, %rsi
	jbe	.L2072
	leaq	-16(%rsi), %rax
	movq	%rsp, %r14
	movq	%r13, %rsi
	movq	%rax, %rdx
	movq	%r14, %rdi
	andq	$-16, %rax
	shrq	$4, %rdx
	addq	$16, %rax
	leal	8(,%rdx,8), %ecx
	andl	$536870904, %ecx
	rep movsq
.L2072:
	movq	%r12, %rdx
	leaq	0(,%rax,4), %rbx
	subq	%rax, %rdx
	movl	$65535, %eax
	leaq	(%r14,%rbx), %r15
	addq	%r13, %rbx
	kmovd	%eax, %k4
	cmpq	$255, %rdx
	ja	.L2076
	movq	$-1, %rax
	bzhi	%rdx, %rax, %rax
	movzwl	%ax, %eax
	kmovd	%eax, %k4
.L2076:
	leal	-1(%r12), %eax
	movl	$32, %edx
	movl	$1, %esi
	vmovdqu32	(%rbx), %zmm0{%k4}{z}
	bsrl	%eax, %eax
	xorl	$31, %eax
	vmovdqa32	%zmm0, (%r15){%k4}
	vpbroadcastq	.LC10(%rip), %zmm0
	subl	%eax, %edx
	movl	$1, %eax
	shlx	%rdx, %rsi, %rsi
	shrq	$4, %rsi
	cmove	%rax, %rsi
	movq	%r12, %rax
	leaq	1(%rsi), %rdx
	salq	$4, %rdx
	cmpq	%rdx, %r12
	jnb	.L2080
	.p2align 4,,10
	.p2align 3
.L2077:
	vmovdqu64	%zmm0, (%r14,%rax,4)
	addq	$16, %rax
	cmpq	%rax, %rdx
	ja	.L2077
.L2080:
	movq	%r14, %rdi
	vzeroupper
	call	_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	cmpq	$15, %r12
	jbe	.L2079
	leaq	-16(%r12), %rax
	movq	(%rsp), %rdx
	leaq	8(%r13), %rdi
	movq	%r14, %rsi
	shrq	$4, %rax
	andq	$-8, %rdi
	addq	$1, %rax
	movq	%rdx, 0(%r13)
	salq	$6, %rax
	movl	%eax, %edx
	movq	-8(%r14,%rdx), %rcx
	movq	%rcx, -8(%r13,%rdx)
	subq	%rdi, %r13
	addl	%r13d, %eax
	subq	%r13, %rsi
	shrl	$3, %eax
	movl	%eax, %ecx
	rep movsq
.L2079:
	vmovdqa32	(%r15), %zmm0{%k4}{z}
	vmovdqu32	%zmm0, (%rbx){%k4}
	vzeroupper
	jmp	.L2084
	.cfi_endproc
.LFE15264:
	.size	_ZN3hwy11N_AVX3_ZEN410SortI32AscEPim, .-_ZN3hwy11N_AVX3_ZEN410SortI32AscEPim
	.section	.text._ZN3hwy6N_SSE210SortI32AscEPim,"ax",@progbits
	.p2align 4
	.globl	_ZN3hwy6N_SSE210SortI32AscEPim
	.hidden	_ZN3hwy6N_SSE210SortI32AscEPim
	.type	_ZN3hwy6N_SSE210SortI32AscEPim, @function
_ZN3hwy6N_SSE210SortI32AscEPim:
.LFB16254:
	.cfi_startproc
	cmpq	$1, %rsi
	jbe	.L2113
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r14
	.cfi_offset 14, -24
	leaq	(%rdi,%rsi,4), %r14
	pushq	%r13
	.cfi_offset 13, -32
	movq	%rdi, %r13
	pushq	%r12
	.cfi_offset 12, -40
	movq	%rsi, %r12
	subq	$296, %rsp
	cmpq	$64, %rsi
	jbe	.L2116
	call	_ZN3hwy17GetGeneratorStateEv@PLT
	movq	%r12, %rdx
	movq	%r14, %rsi
	movq	%r13, %rdi
	movq	%rax, %r8
	leaq	-320(%rbp), %rcx
	movl	$50, %r9d
	call	_ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIiLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L2091:
	addq	$296, %rsp
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L2113:
	.cfi_restore 6
	.cfi_restore 12
	.cfi_restore 13
	.cfi_restore 14
	ret
	.p2align 4,,10
	.p2align 3
.L2116:
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	leaq	256(%rdi), %rax
	cmpq	%rax, %r14
	jb	.L2117
	movl	$4, %esi
	call	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	jmp	.L2091
.L2117:
	movq	%rsi, %rcx
	xorl	%eax, %eax
	leaq	-320(%rbp), %r14
	cmpq	$3, %rsi
	jbe	.L2097
	leaq	-4(%rsi), %rax
	leaq	-320(%rbp), %r14
	movq	%r13, %rsi
	movq	%rax, %rdx
	andq	$-4, %rax
	movq	%r14, %rdi
	shrq	$2, %rdx
	addq	$4, %rax
	leal	2(%rdx,%rdx), %ecx
	andl	$536870910, %ecx
	rep movsq
	movq	%r12, %rcx
	subq	%rax, %rcx
	je	.L2103
.L2097:
	salq	$2, %rax
	leaq	0(,%rcx,4), %rdx
	testq	%rcx, %rcx
	movl	$4, %ecx
	cmove	%rcx, %rdx
	leaq	(%r14,%rax), %rdi
	leaq	0(%r13,%rax), %rsi
	call	memcpy@PLT
.L2103:
	leal	-1(%r12), %eax
	movl	$32, %ecx
	movl	$1, %esi
	bsrl	%eax, %eax
	xorl	$31, %eax
	subl	%eax, %ecx
	movl	$1, %eax
	salq	%cl, %rsi
	shrq	$4, %rsi
	cmove	%rax, %rsi
	movq	%rsi, %rdx
	salq	$4, %rdx
	addq	$4, %rdx
	cmpq	%rdx, %r12
	jnb	.L2102
	movdqa	.LC4(%rip), %xmm0
	movq	%r12, %rax
	.p2align 4,,10
	.p2align 3
.L2101:
	movups	%xmm0, (%r14,%rax,4)
	addq	$4, %rax
	cmpq	%rdx, %rax
	jb	.L2101
.L2102:
	movq	%r14, %rdi
	call	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIiEEEEEEiEEvT_PT0_m.isra.0
	xorl	%eax, %eax
	cmpq	$3, %r12
	jbe	.L2105
	leaq	-4(%r12), %rdx
	movq	-320(%rbp), %rcx
	leaq	8(%r13), %rdi
	movq	%rdx, %rax
	andq	$-8, %rdi
	andq	$-4, %rdx
	shrq	$2, %rax
	movq	%rcx, 0(%r13)
	addq	$1, %rax
	salq	$4, %rax
	movl	%eax, %ecx
	movq	-8(%r14,%rcx), %rsi
	movq	%rsi, -8(%r13,%rcx)
	movq	%r13, %rcx
	movq	%r14, %rsi
	subq	%rdi, %rcx
	addl	%ecx, %eax
	subq	%rcx, %rsi
	shrl	$3, %eax
	movl	%eax, %ecx
	leaq	4(%rdx), %rax
	rep movsq
	subq	%rax, %r12
	je	.L2091
.L2105:
	salq	$2, %rax
	leaq	0(,%r12,4), %rdx
	testq	%r12, %r12
	movl	$4, %ecx
	cmove	%rcx, %rdx
	leaq	0(%r13,%rax), %rdi
	leaq	(%r14,%rax), %rsi
	call	memcpy@PLT
	jmp	.L2091
	.cfi_endproc
.LFE16254:
	.size	_ZN3hwy6N_SSE210SortI32AscEPim, .-_ZN3hwy6N_SSE210SortI32AscEPim
	.section	.text.vqsort_int32_avx2,"ax",@progbits
	.p2align 4
	.globl	vqsort_int32_avx2
	.hidden	vqsort_int32_avx2
	.type	vqsort_int32_avx2, @function
vqsort_int32_avx2:
.LFB16255:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	call	_ZN3hwy6N_AVX210SortI32AscEPim
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16255:
	.size	vqsort_int32_avx2, .-vqsort_int32_avx2
	.section	.text.vqsort_int32_sse4,"ax",@progbits
	.p2align 4
	.globl	vqsort_int32_sse4
	.hidden	vqsort_int32_sse4
	.type	vqsort_int32_sse4, @function
vqsort_int32_sse4:
.LFB16256:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	call	_ZN3hwy6N_SSE410SortI32AscEPim
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16256:
	.size	vqsort_int32_sse4, .-vqsort_int32_sse4
	.section	.text.vqsort_int32_ssse3,"ax",@progbits
	.p2align 4
	.globl	vqsort_int32_ssse3
	.hidden	vqsort_int32_ssse3
	.type	vqsort_int32_ssse3, @function
vqsort_int32_ssse3:
.LFB16257:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	call	_ZN3hwy7N_SSSE310SortI32AscEPim
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16257:
	.size	vqsort_int32_ssse3, .-vqsort_int32_ssse3
	.section	.text.vqsort_int32_sse2,"ax",@progbits
	.p2align 4
	.globl	vqsort_int32_sse2
	.hidden	vqsort_int32_sse2
	.type	vqsort_int32_sse2, @function
vqsort_int32_sse2:
.LFB16258:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	call	_ZN3hwy6N_SSE210SortI32AscEPim
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16258:
	.size	vqsort_int32_sse2, .-vqsort_int32_sse2
	.hidden	_ZZN3hwy6N_SSE26detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.weak	_ZZN3hwy6N_SSE26detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.section	.rodata._ZZN3hwy6N_SSE26detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,"aG",@progbits,_ZZN3hwy6N_SSE26detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,comdat
	.balign 16
	.type	_ZZN3hwy6N_SSE26detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, @object
	.size	_ZZN3hwy6N_SSE26detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, 256
_ZZN3hwy6N_SSE26detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices:
	.string	""
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003"
	.string	"\001\002\003\b\t\n\013\f\r\016\017\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007"
	.string	"\001\002\003\004\005\006\007\f\r\016\017\b\t\n\013\004\005\006\007\f\r\016\017"
	.string	"\001\002\003\b\t\n\013"
	.string	"\001\002\003\f\r\016\017\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\004\005\006\007\b\t\n\013"
	.string	"\001\002\003\f\r\016\017"
	.string	"\001\002\003\b\t\n\013\004\005\006\007\f\r\016\017\b\t\n\013"
	.string	"\001\002\003\004\005\006\007\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\004\005\006\007"
	.string	"\001\002\003\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.ascii	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.hidden	_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array
	.weak	_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array
	.section	.rodata._ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array,"aG",@progbits,_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array,comdat
	.balign 16
	.type	_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array, @object
	.size	_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array, 1024
_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array:
	.long	-19088744
	.long	-1880241239
	.long	-1611805784
	.long	-1728127814
	.long	-1343370344
	.long	-1459692359
	.long	-1442915144
	.long	-1450185269
	.long	-1074935144
	.long	-1191256919
	.long	-1174479704
	.long	-1181749814
	.long	-1157702504
	.long	-1164972599
	.long	-1163924024
	.long	-1164378404
	.long	-806503784
	.long	-922821719
	.long	-906044504
	.long	-913314374
	.long	-889267304
	.long	-896537159
	.long	-895488584
	.long	-895942949
	.long	-872490344
	.long	-879759959
	.long	-878711384
	.long	-879165734
	.long	-877662824
	.long	-878117159
	.long	-878051624
	.long	-878080019
	.long	-538133864
	.long	-654390359
	.long	-637613144
	.long	-644879174
	.long	-620835944
	.long	-628101959
	.long	-627053384
	.long	-627507509
	.long	-604058984
	.long	-611324759
	.long	-610276184
	.long	-610730294
	.long	-609227624
	.long	-609681719
	.long	-609616184
	.long	-609644564
	.long	-587285864
	.long	-594547799
	.long	-593499224
	.long	-593953094
	.long	-592450664
	.long	-592904519
	.long	-592838984
	.long	-592867349
	.long	-591402344
	.long	-591855959
	.long	-591790424
	.long	-591818774
	.long	-591724904
	.long	-591753239
	.long	-591749144
	.long	-591750914
	.long	-270746984
	.long	-386020439
	.long	-369243224
	.long	-376447814
	.long	-352466024
	.long	-359670599
	.long	-358622024
	.long	-359072309
	.long	-335689064
	.long	-342893399
	.long	-341844824
	.long	-342295094
	.long	-340796264
	.long	-341246519
	.long	-341180984
	.long	-341209124
	.long	-318915944
	.long	-326116439
	.long	-325067864
	.long	-325517894
	.long	-324019304
	.long	-324469319
	.long	-324403784
	.long	-324431909
	.long	-322970984
	.long	-323420759
	.long	-323355224
	.long	-323383334
	.long	-323289704
	.long	-323317799
	.long	-323313704
	.long	-323315459
	.long	-302204264
	.long	-309343319
	.long	-308294744
	.long	-308740934
	.long	-307246184
	.long	-307692359
	.long	-307626824
	.long	-307654709
	.long	-306197864
	.long	-306643799
	.long	-306578264
	.long	-306606134
	.long	-306512744
	.long	-306540599
	.long	-306536504
	.long	-306538244
	.long	-305153384
	.long	-305595479
	.long	-305529944
	.long	-305557574
	.long	-305464424
	.long	-305492039
	.long	-305487944
	.long	-305489669
	.long	-305399144
	.long	-305426519
	.long	-305422424
	.long	-305424134
	.long	-305418344
	.long	-305420039
	.long	-305419784
	.long	-305419889
	.long	-19088744
	.long	-118633559
	.long	-101856344
	.long	-108077894
	.long	-85079144
	.long	-91300679
	.long	-90252104
	.long	-90640949
	.long	-68302184
	.long	-74523479
	.long	-73474904
	.long	-73863734
	.long	-72426344
	.long	-72815159
	.long	-72749624
	.long	-72773924
	.long	-51529064
	.long	-57746519
	.long	-56697944
	.long	-57086534
	.long	-55649384
	.long	-56037959
	.long	-55972424
	.long	-55996709
	.long	-54601064
	.long	-54989399
	.long	-54923864
	.long	-54948134
	.long	-54858344
	.long	-54882599
	.long	-54878504
	.long	-54880019
	.long	-34817384
	.long	-40973399
	.long	-39924824
	.long	-40309574
	.long	-38876264
	.long	-39260999
	.long	-39195464
	.long	-39219509
	.long	-37827944
	.long	-38212439
	.long	-38146904
	.long	-38170934
	.long	-38081384
	.long	-38105399
	.long	-38101304
	.long	-38102804
	.long	-36783464
	.long	-37164119
	.long	-37098584
	.long	-37122374
	.long	-37033064
	.long	-37056839
	.long	-37052744
	.long	-37054229
	.long	-36967784
	.long	-36991319
	.long	-36987224
	.long	-36988694
	.long	-36983144
	.long	-36984599
	.long	-36984344
	.long	-36984434
	.long	-19088744
	.long	-24261719
	.long	-23213144
	.long	-23536454
	.long	-22164584
	.long	-22487879
	.long	-22422344
	.long	-22442549
	.long	-21116264
	.long	-21439319
	.long	-21373784
	.long	-21393974
	.long	-21308264
	.long	-21328439
	.long	-21324344
	.long	-21325604
	.long	-20071784
	.long	-20390999
	.long	-20325464
	.long	-20345414
	.long	-20259944
	.long	-20279879
	.long	-20275784
	.long	-20277029
	.long	-20194664
	.long	-20214359
	.long	-20210264
	.long	-20211494
	.long	-20206184
	.long	-20207399
	.long	-20207144
	.long	-20207219
	.long	-19088744
	.long	-19346519
	.long	-19280984
	.long	-19297094
	.long	-19215464
	.long	-19231559
	.long	-19227464
	.long	-19228469
	.long	-19150184
	.long	-19166039
	.long	-19161944
	.long	-19162934
	.long	-19157864
	.long	-19158839
	.long	-19158584
	.long	-19158644
	.long	-19088744
	.long	-19100759
	.long	-19096664
	.long	-19097414
	.long	-19092584
	.long	-19093319
	.long	-19093064
	.long	-19093109
	.long	-19088744
	.long	-19089239
	.long	-19088984
	.long	-19089014
	.long	-19088744
	.long	-19088759
	.long	-19088744
	.long	-19088744
	.hidden	_ZZN3hwy6N_SSE46detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.weak	_ZZN3hwy6N_SSE46detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.section	.rodata._ZZN3hwy6N_SSE46detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,"aG",@progbits,_ZZN3hwy6N_SSE46detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,comdat
	.balign 16
	.type	_ZZN3hwy6N_SSE46detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, @object
	.size	_ZZN3hwy6N_SSE46detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, 256
_ZZN3hwy6N_SSE46detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices:
	.string	""
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003"
	.string	"\001\002\003\b\t\n\013\f\r\016\017\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007"
	.string	"\001\002\003\004\005\006\007\f\r\016\017\b\t\n\013\004\005\006\007\f\r\016\017"
	.string	"\001\002\003\b\t\n\013"
	.string	"\001\002\003\f\r\016\017\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\004\005\006\007\b\t\n\013"
	.string	"\001\002\003\f\r\016\017"
	.string	"\001\002\003\b\t\n\013\004\005\006\007\f\r\016\017\b\t\n\013"
	.string	"\001\002\003\004\005\006\007\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\004\005\006\007"
	.string	"\001\002\003\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.ascii	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.hidden	_ZZN3hwy7N_SSSE36detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.weak	_ZZN3hwy7N_SSSE36detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.section	.rodata._ZZN3hwy7N_SSSE36detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,"aG",@progbits,_ZZN3hwy7N_SSSE36detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,comdat
	.balign 16
	.type	_ZZN3hwy7N_SSSE36detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, @object
	.size	_ZZN3hwy7N_SSSE36detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, 256
_ZZN3hwy7N_SSSE36detail21IndicesFromNotBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices:
	.string	""
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003"
	.string	"\001\002\003\b\t\n\013\f\r\016\017\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007"
	.string	"\001\002\003\004\005\006\007\f\r\016\017\b\t\n\013\004\005\006\007\f\r\016\017"
	.string	"\001\002\003\b\t\n\013"
	.string	"\001\002\003\f\r\016\017\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\004\005\006\007\b\t\n\013"
	.string	"\001\002\003\f\r\016\017"
	.string	"\001\002\003\b\t\n\013\004\005\006\007\f\r\016\017\b\t\n\013"
	.string	"\001\002\003\004\005\006\007\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\004\005\006\007"
	.string	"\001\002\003\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.ascii	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.hidden	_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.weak	_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.section	.rodata._ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,"aG",@progbits,_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,comdat
	.balign 16
	.type	_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, @object
	.size	_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, 256
_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices:
	.string	""
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\004\005\006\007"
	.string	"\001\002\003\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\b\t\n\013"
	.string	"\001\002\003\004\005\006\007\f\r\016\017"
	.string	"\001\002\003\b\t\n\013\004\005\006\007\f\r\016\017\004\005\006\007\b\t\n\013"
	.string	"\001\002\003\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013"
	.string	"\001\002\003\f\r\016\017\004\005\006\007\b\t\n\013\004\005\006\007\f\r\016\017"
	.string	"\001\002\003\b\t\n\013"
	.string	"\001\002\003\004\005\006\007\f\r\016\017\b\t\n\013\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007"
	.string	"\001\002\003\b\t\n\013\f\r\016\017\004\005\006\007\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003"
	.ascii	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.hidden	_ZZN3hwy6N_AVX26detail18IndicesFromBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array
	.weak	_ZZN3hwy6N_AVX26detail18IndicesFromBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array
	.section	.rodata._ZZN3hwy6N_AVX26detail18IndicesFromBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array,"aG",@progbits,_ZZN3hwy6N_AVX26detail18IndicesFromBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array,comdat
	.balign 16
	.type	_ZZN3hwy6N_AVX26detail18IndicesFromBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array, @object
	.size	_ZZN3hwy6N_AVX26detail18IndicesFromBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array, 1024
_ZZN3hwy6N_AVX26detail18IndicesFromBits256IiLPv0EEENS0_6Vec256IjEEmE12packed_array:
	.long	1985229328
	.long	1985229336
	.long	1985229321
	.long	1985229464
	.long	1985229066
	.long	1985229224
	.long	1985228969
	.long	1985231512
	.long	1985224971
	.long	1985225144
	.long	1985224889
	.long	1985227672
	.long	1985220794
	.long	1985223592
	.long	1985219497
	.long	1985264280
	.long	1985159436
	.long	1985159624
	.long	1985159369
	.long	1985162392
	.long	1985155274
	.long	1985158312
	.long	1985154217
	.long	1985202840
	.long	1985089739
	.long	1985092792
	.long	1985088697
	.long	1985137560
	.long	1985023162
	.long	1985072040
	.long	1985006505
	.long	1985788568
	.long	1984110861
	.long	1984111064
	.long	1984110809
	.long	1984114072
	.long	1984106714
	.long	1984109992
	.long	1984105897
	.long	1984158360
	.long	1984041179
	.long	1984044472
	.long	1984040377
	.long	1984093080
	.long	1983974842
	.long	1984027560
	.long	1983962025
	.long	1984805528
	.long	1982992604
	.long	1982995912
	.long	1982991817
	.long	1983044760
	.long	1982926282
	.long	1982979240
	.long	1982913705
	.long	1983761048
	.long	1981877707
	.long	1981930680
	.long	1981865145
	.long	1982712728
	.long	1980816570
	.long	1981664168
	.long	1980615593
	.long	1994177176
	.long	1967333646
	.long	1967333864
	.long	1967333609
	.long	1967337112
	.long	1967329514
	.long	1967333032
	.long	1967328937
	.long	1967385240
	.long	1967263979
	.long	1967267512
	.long	1967263417
	.long	1967319960
	.long	1967197882
	.long	1967254440
	.long	1967188905
	.long	1968093848
	.long	1966215404
	.long	1966218952
	.long	1966214857
	.long	1966271640
	.long	1966149322
	.long	1966206120
	.long	1966140585
	.long	1967049368
	.long	1965100747
	.long	1965157560
	.long	1965092025
	.long	1966001048
	.long	1964043450
	.long	1964952488
	.long	1963903913
	.long	1978448536
	.long	1949438189
	.long	1949441752
	.long	1949437657
	.long	1949494680
	.long	1949372122
	.long	1949429160
	.long	1949363625
	.long	1950276248
	.long	1948323547
	.long	1948380600
	.long	1948315065
	.long	1949227928
	.long	1947266490
	.long	1948179368
	.long	1947130793
	.long	1961736856
	.long	1931546332
	.long	1931603400
	.long	1931537865
	.long	1932450968
	.long	1930489290
	.long	1931402408
	.long	1930353833
	.long	1944963736
	.long	1913712075
	.long	1914625208
	.long	1913576633
	.long	1928186776
	.long	1896799418
	.long	1911409576
	.long	1894632361
	.long	2128394904
	.long	1698898191
	.long	1698898424
	.long	1698898169
	.long	1698901912
	.long	1698894074
	.long	1698897832
	.long	1698893737
	.long	1698953880
	.long	1698828539
	.long	1698832312
	.long	1698828217
	.long	1698888600
	.long	1698762682
	.long	1698823080
	.long	1698757545
	.long	1699723928
	.long	1697779964
	.long	1697783752
	.long	1697779657
	.long	1697840280
	.long	1697714122
	.long	1697774760
	.long	1697709225
	.long	1698679448
	.long	1696665547
	.long	1696726200
	.long	1696660665
	.long	1697631128
	.long	1695612090
	.long	1696582568
	.long	1695533993
	.long	1711061656
	.long	1681002749
	.long	1681006552
	.long	1681002457
	.long	1681063320
	.long	1680936922
	.long	1680997800
	.long	1680932265
	.long	1681906328
	.long	1679888347
	.long	1679949240
	.long	1679883705
	.long	1680858008
	.long	1678835130
	.long	1679809448
	.long	1678760873
	.long	1694349976
	.long	1663111132
	.long	1663172040
	.long	1663106505
	.long	1664081048
	.long	1662057930
	.long	1663032488
	.long	1661983913
	.long	1677576856
	.long	1645280715
	.long	1646255288
	.long	1645206713
	.long	1660799896
	.long	1628429498
	.long	1644022696
	.long	1627245481
	.long	1876736664
	.long	1412567294
	.long	1412571112
	.long	1412567017
	.long	1412628120
	.long	1412501482
	.long	1412562600
	.long	1412497065
	.long	1413474968
	.long	1411452907
	.long	1411514040
	.long	1411448505
	.long	1412426648
	.long	1410399930
	.long	1411378088
	.long	1410329513
	.long	1425980056
	.long	1394675692
	.long	1394736840
	.long	1394671305
	.long	1395649688
	.long	1393622730
	.long	1394601128
	.long	1393552553
	.long	1409206936
	.long	1376845515
	.long	1377823928
	.long	1376775353
	.long	1392429976
	.long	1359998138
	.long	1375652776
	.long	1358875561
	.long	1609349784
	.long	1126240237
	.long	1126301400
	.long	1126235865
	.long	1127214488
	.long	1125187290
	.long	1126165928
	.long	1125117353
	.long	1140775576
	.long	1108410075
	.long	1109388728
	.long	1108340153
	.long	1123998616
	.long	1091562938
	.long	1107221416
	.long	1090444201
	.long	1340979864
	.long	839974620
	.long	840953288
	.long	839904713
	.long	855563416
	.long	823127498
	.long	838786216
	.long	822009001
	.long	1072548504
	.long	554692043
	.long	570350776
	.long	553573561
	.long	804113304
	.long	285138106
	.long	535677864
	.long	267242409
	.long	-19088744
	.hidden	_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.weak	_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.section	.rodata._ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,"aG",@progbits,_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,comdat
	.balign 16
	.type	_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, @object
	.size	_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, 256
_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices:
	.string	""
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\004\005\006\007"
	.string	"\001\002\003\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\b\t\n\013"
	.string	"\001\002\003\004\005\006\007\f\r\016\017"
	.string	"\001\002\003\b\t\n\013\004\005\006\007\f\r\016\017\004\005\006\007\b\t\n\013"
	.string	"\001\002\003\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013"
	.string	"\001\002\003\f\r\016\017\004\005\006\007\b\t\n\013\004\005\006\007\f\r\016\017"
	.string	"\001\002\003\b\t\n\013"
	.string	"\001\002\003\004\005\006\007\f\r\016\017\b\t\n\013\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007"
	.string	"\001\002\003\b\t\n\013\f\r\016\017\004\005\006\007\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003"
	.ascii	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.hidden	_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.weak	_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.section	.rodata._ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,"aG",@progbits,_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,comdat
	.balign 16
	.type	_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, @object
	.size	_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, 256
_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIiLm4ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices:
	.string	""
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\004\005\006\007"
	.string	"\001\002\003\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\b\t\n\013"
	.string	"\001\002\003\004\005\006\007\f\r\016\017"
	.string	"\001\002\003\b\t\n\013\004\005\006\007\f\r\016\017\004\005\006\007\b\t\n\013"
	.string	"\001\002\003\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013"
	.string	"\001\002\003\f\r\016\017\004\005\006\007\b\t\n\013\004\005\006\007\f\r\016\017"
	.string	"\001\002\003\b\t\n\013"
	.string	"\001\002\003\004\005\006\007\f\r\016\017\b\t\n\013\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007"
	.string	"\001\002\003\b\t\n\013\f\r\016\017\004\005\006\007\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003"
	.ascii	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.set	.LC0,.LC3
	.section	.rodata
	.balign 64
.LC1:
	.long	7
	.long	6
	.long	5
	.long	4
	.long	3
	.long	2
	.long	1
	.long	0
	.long	15
	.long	14
	.long	13
	.long	12
	.long	11
	.long	10
	.long	9
	.long	8
	.balign 64
.LC2:
	.long	15
	.long	14
	.long	13
	.long	12
	.long	11
	.long	10
	.long	9
	.long	8
	.long	7
	.long	6
	.long	5
	.long	4
	.long	3
	.long	2
	.long	1
	.long	0
	.section	.rodata.cst32,"aM",@progbits,32
	.balign 32
.LC3:
	.long	0
	.long	1
	.long	2
	.long	3
	.long	4
	.long	5
	.long	6
	.long	7
	.set	.LC4,.LC9
	.set	.LC5,.LC8
	.set	.LC6,.LC9
	.section	.rodata
	.balign 64
.LC8:
	.long	-2147483648
	.long	-2147483648
	.long	-2147483648
	.long	-2147483648
	.long	-2147483648
	.long	-2147483648
	.long	-2147483648
	.long	-2147483648
	.long	-2147483648
	.long	-2147483648
	.long	-2147483648
	.long	-2147483648
	.long	-2147483648
	.long	-2147483648
	.long	-2147483648
	.long	-2147483648
	.balign 64
.LC9:
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.set	.LC10,.LC9
	.set	.LC13,.LC1
	.set	.LC14,.LC9
	.set	.LC15,.LC8
	.set	.LC16,.LC9
	.section	.rodata.cst32
	.balign 32
.LC17:
	.long	0
	.long	4
	.long	8
	.long	12
	.long	16
	.long	20
	.long	24
	.long	28
