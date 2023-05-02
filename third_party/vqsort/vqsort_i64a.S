	.text
	.globl	__popcountdi2
	.section	.text._ZN3hwy6N_SSE26detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE26detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, @function
_ZN3hwy6N_SSE26detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0:
.LFB18780:
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
	movq	%rdi, %r14
	pushq	%r13
	.cfi_offset 13, -40
	movq	%rcx, %r13
	pushq	%r12
	.cfi_offset 12, -48
	movq	%rsi, %r12
	pushq	%rbx
	subq	$88, %rsp
	.cfi_offset 3, -56
	movq	%rdx, -120(%rbp)
	movaps	%xmm0, -80(%rbp)
	movaps	%xmm1, -64(%rbp)
	movaps	%xmm0, -96(%rbp)
	movaps	%xmm1, -112(%rbp)
	cmpq	$1, %rsi
	jbe	.L24
	movl	$2, %r15d
	xorl	%ebx, %ebx
	jmp	.L9
	.p2align 4,,10
	.p2align 3
.L3:
	movdqa	-80(%rbp), %xmm5
	movmskpd	%xmm1, %edi
	movups	%xmm5, (%r14,%rbx,8)
	call	__popcountdi2@PLT
	cltq
	addq	%rax, %rbx
	leaq	2(%r15), %rax
	cmpq	%r12, %rax
	ja	.L56
	movq	%rax, %r15
.L9:
	movdqu	-16(%r14,%r15,8), %xmm0
	leaq	-2(%r15), %rdx
	leaq	0(,%rbx,8), %rax
	pcmpeqd	-96(%rbp), %xmm0
	pshufd	$177, %xmm0, %xmm1
	pand	%xmm0, %xmm1
	movdqu	-16(%r14,%r15,8), %xmm0
	pcmpeqd	-112(%rbp), %xmm0
	pshufd	$177, %xmm0, %xmm2
	pand	%xmm2, %xmm0
	movdqa	%xmm1, %xmm2
	por	%xmm0, %xmm2
	movmskpd	%xmm2, %ecx
	cmpl	$3, %ecx
	je	.L3
	pcmpeqd	%xmm2, %xmm2
	movq	-120(%rbp), %rsi
	leaq	2(%rbx), %rdi
	pxor	%xmm2, %xmm0
	pandn	%xmm0, %xmm1
	movmskpd	%xmm1, %ecx
	rep bsfl	%ecx, %ecx
	movslq	%ecx, %rcx
	addq	%rdx, %rcx
	movq	(%r14,%rcx,8), %xmm0
	punpcklqdq	%xmm0, %xmm0
	movaps	%xmm0, (%rsi)
	cmpq	%rdx, %rdi
	ja	.L4
	movq	%rdx, %rcx
	addq	%r14, %rax
	subq	%rbx, %rcx
	leaq	-2(%rcx), %rsi
	movq	%rsi, %rcx
	andq	$-2, %rcx
	addq	%rbx, %rcx
	leaq	16(%r14,%rcx,8), %rcx
	.p2align 4,,10
	.p2align 3
.L5:
	movdqa	-64(%rbp), %xmm4
	addq	$16, %rax
	movups	%xmm4, -16(%rax)
	cmpq	%rcx, %rax
	jne	.L5
	andq	$-2, %rsi
	leaq	(%rsi,%rdi), %rbx
.L4:
	subq	%rbx, %rdx
	movdqa	.LC1(%rip), %xmm2
	movdqa	.LC0(%rip), %xmm1
	leaq	0(,%rbx,8), %rcx
	movq	%rdx, %xmm0
	punpcklqdq	%xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	psubq	%xmm0, %xmm1
	pcmpeqd	%xmm2, %xmm3
	pcmpgtd	%xmm2, %xmm0
	pand	%xmm3, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L6
	movdqa	-64(%rbp), %xmm3
	movq	%xmm3, (%r14,%rbx,8)
.L6:
	movhlps	%xmm0, %xmm3
	movq	%xmm3, %rax
	testq	%rax, %rax
	jne	.L57
.L17:
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
.L57:
	.cfi_restore_state
	movdqa	-64(%rbp), %xmm3
	movhps	%xmm3, 8(%r14,%rcx)
	jmp	.L17
	.p2align 4,,10
	.p2align 3
.L56:
	movq	%r12, %rcx
	leaq	0(,%r15,8), %rsi
	leaq	0(,%rbx,8), %r9
	subq	%r15, %rcx
.L2:
	testq	%rcx, %rcx
	je	.L13
	leaq	0(,%rcx,8), %rdx
	addq	%r14, %rsi
	movq	%r13, %rdi
	movq	%r9, -112(%rbp)
	movq	%rcx, -96(%rbp)
	call	memcpy@PLT
	movq	-96(%rbp), %rcx
	movq	-112(%rbp), %r9
.L13:
	movdqa	.LC1(%rip), %xmm3
	movq	%rcx, %xmm2
	movdqa	-80(%rbp), %xmm5
	movdqa	.LC0(%rip), %xmm1
	punpcklqdq	%xmm2, %xmm2
	movdqa	%xmm3, %xmm4
	pcmpeqd	%xmm2, %xmm4
	movdqa	%xmm1, %xmm0
	psubq	%xmm2, %xmm0
	pcmpgtd	%xmm3, %xmm2
	pand	%xmm4, %xmm0
	por	%xmm2, %xmm0
	movdqa	0(%r13), %xmm2
	pshufd	$245, %xmm0, %xmm0
	pcmpeqd	%xmm2, %xmm5
	pcmpeqd	-64(%rbp), %xmm2
	pshufd	$177, %xmm5, %xmm4
	pand	%xmm0, %xmm4
	pand	%xmm5, %xmm4
	pshufd	$177, %xmm2, %xmm5
	pand	%xmm5, %xmm2
	pcmpeqd	%xmm5, %xmm5
	movdqa	%xmm5, %xmm6
	pxor	%xmm0, %xmm6
	por	%xmm6, %xmm2
	por	%xmm4, %xmm2
	movmskpd	%xmm2, %eax
	cmpl	$3, %eax
	jne	.L58
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L18
	movdqa	-80(%rbp), %xmm5
	movq	%xmm5, (%r14,%r9)
.L18:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rax
	testq	%rax, %rax
	jne	.L59
.L19:
	movmskpd	%xmm4, %edi
	call	__popcountdi2@PLT
	movdqa	.LC0(%rip), %xmm1
	movdqa	.LC1(%rip), %xmm3
	movslq	%eax, %rdx
	addq	%rbx, %rdx
	leaq	2(%rdx), %rax
	cmpq	%rax, %r12
	jb	.L20
	.p2align 4,,10
	.p2align 3
.L21:
	movdqa	-64(%rbp), %xmm7
	movq	%rax, %rdx
	movups	%xmm7, -16(%r14,%rax,8)
	addq	$2, %rax
	cmpq	%rax, %r12
	jnb	.L21
.L20:
	subq	%rdx, %r12
	movdqa	%xmm3, %xmm2
	leaq	0(,%rdx,8), %rcx
	movq	%r12, %xmm0
	punpcklqdq	%xmm0, %xmm0
	pcmpeqd	%xmm0, %xmm2
	psubq	%xmm0, %xmm1
	pcmpgtd	%xmm3, %xmm0
	pand	%xmm2, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L22
	movdqa	-64(%rbp), %xmm3
	movq	%xmm3, (%r14,%rdx,8)
.L22:
	movhlps	%xmm0, %xmm3
	movq	%xmm3, %rax
	testq	%rax, %rax
	je	.L23
	movdqa	-64(%rbp), %xmm3
	movhps	%xmm3, 8(%r14,%rcx)
.L23:
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
.L59:
	.cfi_restore_state
	movdqa	-80(%rbp), %xmm6
	movhps	%xmm6, 8(%r14,%r9)
	jmp	.L19
.L24:
	movq	%rsi, %rcx
	xorl	%r9d, %r9d
	xorl	%esi, %esi
	xorl	%ebx, %ebx
	xorl	%r15d, %r15d
	jmp	.L2
.L58:
	pxor	%xmm5, %xmm2
	leaq	2(%rbx), %rsi
	movmskpd	%xmm2, %eax
	rep bsfl	%eax, %eax
	cltq
	addq	%r15, %rax
	movq	(%r14,%rax,8), %xmm0
	movq	-120(%rbp), %rax
	punpcklqdq	%xmm0, %xmm0
	movaps	%xmm0, (%rax)
	cmpq	%r15, %rsi
	ja	.L14
	leaq	-2(%r15), %rcx
	leaq	(%r14,%rbx,8), %rax
	subq	%rbx, %rcx
	movq	%rcx, %rdx
	andq	$-2, %rdx
	addq	%rbx, %rdx
	leaq	16(%r14,%rdx,8), %rdx
	.p2align 4,,10
	.p2align 3
.L15:
	movdqa	-64(%rbp), %xmm4
	addq	$16, %rax
	movups	%xmm4, -16(%rax)
	cmpq	%rax, %rdx
	jne	.L15
	andq	$-2, %rcx
	leaq	(%rcx,%rsi), %rbx
	leaq	0(,%rbx,8), %r9
.L14:
	subq	%rbx, %r15
	movdqa	%xmm3, %xmm2
	movq	%r15, %xmm0
	punpcklqdq	%xmm0, %xmm0
	pcmpeqd	%xmm0, %xmm2
	psubq	%xmm0, %xmm1
	pcmpgtd	%xmm3, %xmm0
	pand	%xmm2, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L16
	movdqa	-64(%rbp), %xmm3
	movq	%xmm3, (%r14,%r9)
.L16:
	movhlps	%xmm0, %xmm3
	movq	%xmm3, %rax
	testq	%rax, %rax
	je	.L17
	movdqa	-64(%rbp), %xmm3
	movhps	%xmm3, 8(%r14,%r9)
	jmp	.L17
	.cfi_endproc
.LFE18780:
	.size	_ZN3hwy6N_SSE26detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, .-_ZN3hwy6N_SSE26detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	.section	.text._ZN3hwy6N_SSE26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0, @function
_ZN3hwy6N_SSE26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0:
.LFB18781:
	.cfi_startproc
	cmpq	%rdx, %rsi
	jbe	.L60
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r8
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rax, %rsi
	jbe	.L60
	movq	(%rdi,%rdx,8), %r11
	movq	%r11, %xmm3
	punpcklqdq	%xmm3, %xmm3
	movdqa	%xmm3, %xmm6
	jmp	.L63
	.p2align 4,,10
	.p2align 3
.L69:
	movq	%rdx, %rax
.L64:
	cmpq	%rcx, %rsi
	jbe	.L65
	salq	$4, %r8
	movq	(%rdi,%r8), %xmm0
	punpcklqdq	%xmm0, %xmm0
	movdqa	%xmm0, %xmm2
	psubq	%xmm0, %xmm1
	pcmpeqd	%xmm4, %xmm2
	pcmpgtd	%xmm4, %xmm0
	pand	%xmm2, %xmm1
	por	%xmm0, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movmskpd	%xmm1, %r8d
	andl	$1, %r8d
	jne	.L66
.L65:
	cmpq	%rdx, %rax
	je	.L60
	leaq	(%rdi,%rax,8), %rdx
	movq	(%rdx), %rcx
	movq	%rcx, (%r10)
	movq	%r11, (%rdx)
	cmpq	%rax, %rsi
	jbe	.L72
	movq	%rax, %rdx
.L67:
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r8
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rsi, %rax
	jnb	.L60
.L63:
	movq	(%rdi,%rax,8), %xmm2
	movdqa	%xmm3, %xmm0
	leaq	(%rdi,%rdx,8), %r10
	movdqa	%xmm6, %xmm4
	movdqa	%xmm3, %xmm1
	punpcklqdq	%xmm2, %xmm2
	movdqa	%xmm2, %xmm5
	psubq	%xmm2, %xmm0
	pcmpeqd	%xmm3, %xmm5
	pand	%xmm5, %xmm0
	movdqa	%xmm2, %xmm5
	pcmpgtd	%xmm3, %xmm5
	por	%xmm5, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movmskpd	%xmm0, %r9d
	andl	$1, %r9d
	je	.L69
	movdqa	%xmm2, %xmm1
	movdqa	%xmm2, %xmm4
	jmp	.L64
	.p2align 4,,10
	.p2align 3
.L66:
	cmpq	%rcx, %rdx
	je	.L60
	leaq	(%rdi,%rcx,8), %rax
	movq	(%rax), %rdx
	movq	%rdx, (%r10)
	movq	%rcx, %rdx
	movq	%r11, (%rax)
	jmp	.L67
	.p2align 4,,10
	.p2align 3
.L60:
	ret
	.p2align 4,,10
	.p2align 3
.L72:
	ret
	.cfi_endproc
.LFE18781:
	.size	_ZN3hwy6N_SSE26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0, .-_ZN3hwy6N_SSE26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	.section	.text._ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0, @function
_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0:
.LFB18782:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %rax
	salq	$3, %rax
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
	subq	$224, %rsp
	leaq	(%r10,%rax), %r9
	leaq	(%r9,%rax), %r8
	movq	%rdi, -232(%rbp)
	movq	%rsi, -192(%rbp)
	movdqu	(%rdi), %xmm9
	leaq	(%r8,%rax), %rdi
	movdqu	(%r15), %xmm6
	leaq	(%rdi,%rax), %rsi
	movdqu	0(%r13), %xmm5
	movdqu	(%r14), %xmm13
	leaq	(%rsi,%rax), %rcx
	movdqa	%xmm9, %xmm14
	movdqu	(%rbx), %xmm3
	movdqu	(%r12), %xmm12
	leaq	(%rcx,%rax), %rdx
	psubq	%xmm6, %xmm14
	movdqu	(%r10), %xmm2
	movdqu	(%r11), %xmm11
	movdqu	(%rdx), %xmm0
	movdqu	(%rsi), %xmm4
	movq	%rdx, -216(%rbp)
	addq	%rax, %rdx
	movdqu	(%rdx), %xmm15
	movdqu	(%r8), %xmm8
	movq	%rdx, -224(%rbp)
	addq	%rdx, %rax
	movaps	%xmm0, -96(%rbp)
	movdqa	%xmm6, %xmm0
	movdqu	(%rdi), %xmm7
	movdqu	(%rcx), %xmm1
	pcmpeqd	%xmm9, %xmm0
	movaps	%xmm15, -112(%rbp)
	movdqu	(%r9), %xmm10
	pand	%xmm14, %xmm0
	movdqa	%xmm6, %xmm14
	pcmpgtd	%xmm9, %xmm14
	por	%xmm14, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm14
	pandn	%xmm6, %xmm14
	pand	%xmm0, %xmm6
	movdqa	%xmm14, %xmm15
	movdqa	%xmm9, %xmm14
	pand	%xmm0, %xmm14
	por	%xmm15, %xmm14
	movdqa	%xmm0, %xmm15
	movdqa	%xmm5, %xmm0
	pcmpeqd	%xmm13, %xmm0
	pandn	%xmm9, %xmm15
	movdqa	%xmm13, %xmm9
	psubq	%xmm5, %xmm9
	por	%xmm15, %xmm6
	pand	%xmm9, %xmm0
	movdqa	%xmm5, %xmm9
	pcmpgtd	%xmm13, %xmm9
	por	%xmm9, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm9
	pandn	%xmm5, %xmm9
	pand	%xmm0, %xmm5
	movdqa	%xmm9, %xmm15
	movdqa	%xmm13, %xmm9
	pand	%xmm0, %xmm9
	por	%xmm15, %xmm9
	movdqa	%xmm0, %xmm15
	movdqa	%xmm3, %xmm0
	pcmpeqd	%xmm12, %xmm0
	pandn	%xmm13, %xmm15
	movdqa	%xmm12, %xmm13
	psubq	%xmm3, %xmm13
	por	%xmm15, %xmm5
	pand	%xmm13, %xmm0
	movdqa	%xmm3, %xmm13
	pcmpgtd	%xmm12, %xmm13
	por	%xmm13, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm13
	pandn	%xmm3, %xmm13
	pand	%xmm0, %xmm3
	movdqa	%xmm13, %xmm15
	movdqa	%xmm12, %xmm13
	pand	%xmm0, %xmm13
	por	%xmm15, %xmm13
	movdqa	%xmm0, %xmm15
	movdqa	%xmm2, %xmm0
	pandn	%xmm12, %xmm15
	pcmpeqd	%xmm11, %xmm0
	por	%xmm15, %xmm3
	movaps	%xmm3, -64(%rbp)
	movdqa	%xmm11, %xmm3
	psubq	%xmm2, %xmm3
	pand	%xmm3, %xmm0
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm11, %xmm3
	por	%xmm3, %xmm0
	movdqa	%xmm11, %xmm3
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm12
	pand	%xmm0, %xmm3
	pandn	%xmm2, %xmm12
	pand	%xmm0, %xmm2
	por	%xmm12, %xmm3
	movdqa	%xmm0, %xmm12
	movdqa	%xmm8, %xmm0
	pcmpeqd	%xmm10, %xmm0
	pandn	%xmm11, %xmm12
	movdqa	%xmm10, %xmm11
	psubq	%xmm8, %xmm11
	por	%xmm12, %xmm2
	movdqa	%xmm10, %xmm12
	pand	%xmm11, %xmm0
	movdqa	%xmm8, %xmm11
	pcmpgtd	%xmm10, %xmm11
	por	%xmm11, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm11
	pand	%xmm0, %xmm12
	pandn	%xmm8, %xmm11
	pand	%xmm0, %xmm8
	por	%xmm11, %xmm12
	movdqa	%xmm0, %xmm11
	movdqa	%xmm4, %xmm0
	pandn	%xmm10, %xmm11
	pcmpeqd	%xmm7, %xmm0
	por	%xmm11, %xmm8
	movaps	%xmm8, -80(%rbp)
	movdqa	%xmm7, %xmm8
	movdqa	-96(%rbp), %xmm10
	movdqa	-112(%rbp), %xmm15
	psubq	%xmm4, %xmm8
	pand	%xmm8, %xmm0
	movdqa	%xmm4, %xmm8
	pcmpgtd	%xmm7, %xmm8
	por	%xmm8, %xmm0
	movdqa	%xmm7, %xmm8
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm11
	pand	%xmm0, %xmm8
	pandn	%xmm4, %xmm11
	pand	%xmm0, %xmm4
	por	%xmm11, %xmm8
	movdqa	%xmm0, %xmm11
	movdqa	%xmm10, %xmm0
	pcmpeqd	%xmm1, %xmm0
	pandn	%xmm7, %xmm11
	movdqa	%xmm1, %xmm7
	psubq	%xmm10, %xmm7
	por	%xmm11, %xmm4
	movdqa	%xmm1, %xmm11
	pand	%xmm7, %xmm0
	movdqa	%xmm10, %xmm7
	pcmpgtd	%xmm1, %xmm7
	por	%xmm7, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm7
	pand	%xmm0, %xmm11
	pandn	%xmm10, %xmm7
	por	%xmm7, %xmm11
	movdqa	%xmm0, %xmm7
	pand	%xmm10, %xmm0
	movdqu	(%rax), %xmm10
	pandn	%xmm1, %xmm7
	movdqa	%xmm15, %xmm1
	por	%xmm7, %xmm0
	movdqu	(%rax), %xmm7
	movaps	%xmm0, -96(%rbp)
	psubq	%xmm7, %xmm1
	movdqu	(%rax), %xmm7
	movdqa	%xmm1, %xmm0
	movdqu	(%rax), %xmm1
	pcmpgtd	%xmm15, %xmm7
	pcmpeqd	%xmm15, %xmm1
	pand	%xmm0, %xmm1
	movdqa	%xmm15, %xmm0
	por	%xmm7, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm7
	pand	%xmm1, %xmm0
	pandn	%xmm10, %xmm7
	por	%xmm7, %xmm0
	movdqa	%xmm1, %xmm7
	pand	%xmm10, %xmm1
	pandn	%xmm15, %xmm7
	movdqa	%xmm14, %xmm10
	movdqa	%xmm14, %xmm15
	por	%xmm7, %xmm1
	movdqa	%xmm9, %xmm7
	psubq	%xmm9, %xmm10
	pcmpeqd	%xmm14, %xmm7
	pand	%xmm10, %xmm7
	movdqa	%xmm9, %xmm10
	pcmpgtd	%xmm14, %xmm10
	por	%xmm10, %xmm7
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm10
	pand	%xmm7, %xmm15
	pandn	%xmm9, %xmm10
	pand	%xmm7, %xmm9
	por	%xmm15, %xmm10
	movdqa	%xmm7, %xmm15
	movdqa	%xmm5, %xmm7
	pcmpeqd	%xmm6, %xmm7
	pandn	%xmm14, %xmm15
	movdqa	%xmm6, %xmm14
	por	%xmm9, %xmm15
	movdqa	%xmm6, %xmm9
	psubq	%xmm5, %xmm9
	pand	%xmm9, %xmm7
	movdqa	%xmm5, %xmm9
	pcmpgtd	%xmm6, %xmm9
	por	%xmm9, %xmm7
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm9
	pand	%xmm7, %xmm14
	pandn	%xmm5, %xmm9
	pand	%xmm7, %xmm5
	por	%xmm14, %xmm9
	movdqa	%xmm7, %xmm14
	pandn	%xmm6, %xmm14
	movdqa	%xmm3, %xmm6
	por	%xmm14, %xmm5
	pcmpeqd	%xmm13, %xmm6
	movdqa	-64(%rbp), %xmm14
	movaps	%xmm5, -112(%rbp)
	movdqa	%xmm13, %xmm5
	psubq	%xmm3, %xmm5
	pand	%xmm5, %xmm6
	movdqa	%xmm3, %xmm5
	pcmpgtd	%xmm13, %xmm5
	por	%xmm5, %xmm6
	movdqa	%xmm13, %xmm5
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm7
	pand	%xmm6, %xmm5
	pandn	%xmm3, %xmm7
	pand	%xmm6, %xmm3
	por	%xmm7, %xmm5
	movdqa	%xmm6, %xmm7
	movdqa	%xmm14, %xmm6
	pandn	%xmm13, %xmm7
	pcmpeqd	%xmm2, %xmm6
	por	%xmm7, %xmm3
	movdqa	%xmm14, %xmm7
	psubq	%xmm2, %xmm7
	pand	%xmm7, %xmm6
	movdqa	%xmm2, %xmm7
	pcmpgtd	%xmm14, %xmm7
	por	%xmm7, %xmm6
	movdqa	%xmm14, %xmm7
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm13
	pand	%xmm6, %xmm7
	pandn	%xmm2, %xmm13
	pand	%xmm6, %xmm2
	por	%xmm13, %xmm7
	movdqa	%xmm6, %xmm13
	movdqa	%xmm8, %xmm6
	pandn	%xmm14, %xmm13
	pcmpeqd	%xmm12, %xmm6
	movdqa	%xmm12, %xmm14
	por	%xmm13, %xmm2
	movdqa	%xmm12, %xmm13
	psubq	%xmm8, %xmm13
	pand	%xmm13, %xmm6
	movdqa	%xmm8, %xmm13
	pcmpgtd	%xmm12, %xmm13
	por	%xmm13, %xmm6
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm13
	pand	%xmm6, %xmm14
	pandn	%xmm8, %xmm13
	pand	%xmm6, %xmm8
	por	%xmm14, %xmm13
	movdqa	%xmm6, %xmm14
	movdqa	%xmm8, %xmm6
	pandn	%xmm12, %xmm14
	por	%xmm14, %xmm6
	movdqa	-80(%rbp), %xmm14
	movaps	%xmm6, -128(%rbp)
	movdqa	%xmm14, %xmm6
	movdqa	%xmm14, %xmm8
	movdqa	%xmm14, %xmm12
	pcmpeqd	%xmm4, %xmm6
	psubq	%xmm4, %xmm8
	pand	%xmm8, %xmm6
	movdqa	%xmm4, %xmm8
	pcmpgtd	%xmm14, %xmm8
	por	%xmm8, %xmm6
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm8
	pand	%xmm6, %xmm12
	pandn	%xmm4, %xmm8
	pand	%xmm6, %xmm4
	por	%xmm12, %xmm8
	movdqa	%xmm6, %xmm12
	movdqa	%xmm0, %xmm6
	pandn	%xmm14, %xmm12
	pcmpeqd	%xmm11, %xmm6
	movdqa	-96(%rbp), %xmm14
	por	%xmm12, %xmm4
	movaps	%xmm4, -80(%rbp)
	movdqa	%xmm11, %xmm4
	psubq	%xmm0, %xmm4
	pand	%xmm4, %xmm6
	movdqa	%xmm0, %xmm4
	pcmpgtd	%xmm11, %xmm4
	por	%xmm4, %xmm6
	movdqa	%xmm11, %xmm4
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm12
	pand	%xmm6, %xmm4
	pandn	%xmm0, %xmm12
	pand	%xmm6, %xmm0
	por	%xmm12, %xmm4
	movdqa	%xmm6, %xmm12
	movdqa	%xmm14, %xmm6
	pandn	%xmm11, %xmm12
	movdqa	%xmm14, %xmm11
	psubq	%xmm1, %xmm6
	pcmpeqd	%xmm1, %xmm11
	por	%xmm12, %xmm0
	pand	%xmm6, %xmm11
	movdqa	%xmm1, %xmm6
	pcmpgtd	%xmm14, %xmm6
	por	%xmm6, %xmm11
	movdqa	%xmm14, %xmm6
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm6
	pandn	%xmm1, %xmm12
	pand	%xmm11, %xmm1
	por	%xmm12, %xmm6
	movdqa	%xmm11, %xmm12
	movdqa	%xmm5, %xmm11
	pandn	%xmm14, %xmm12
	pcmpeqd	%xmm10, %xmm11
	movdqa	%xmm10, %xmm14
	por	%xmm12, %xmm1
	movdqa	%xmm10, %xmm12
	psubq	%xmm5, %xmm12
	pand	%xmm12, %xmm11
	movdqa	%xmm5, %xmm12
	pcmpgtd	%xmm10, %xmm12
	por	%xmm12, %xmm11
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm14
	pandn	%xmm5, %xmm12
	pand	%xmm11, %xmm5
	por	%xmm14, %xmm12
	movdqa	%xmm11, %xmm14
	movdqa	%xmm9, %xmm11
	pandn	%xmm10, %xmm14
	movdqa	%xmm9, %xmm10
	por	%xmm5, %xmm14
	movdqa	%xmm7, %xmm5
	psubq	%xmm7, %xmm10
	pcmpeqd	%xmm9, %xmm5
	pand	%xmm10, %xmm5
	movdqa	%xmm7, %xmm10
	pcmpgtd	%xmm9, %xmm10
	por	%xmm10, %xmm5
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm10
	pand	%xmm5, %xmm11
	pandn	%xmm7, %xmm10
	pand	%xmm5, %xmm7
	por	%xmm11, %xmm10
	movdqa	%xmm5, %xmm11
	movdqa	%xmm3, %xmm5
	pandn	%xmm9, %xmm11
	pcmpeqd	%xmm15, %xmm5
	movdqa	%xmm15, %xmm9
	por	%xmm11, %xmm7
	movaps	%xmm7, -96(%rbp)
	movdqa	%xmm15, %xmm7
	psubq	%xmm3, %xmm7
	pand	%xmm7, %xmm5
	movdqa	%xmm3, %xmm7
	pcmpgtd	%xmm15, %xmm7
	por	%xmm7, %xmm5
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm7
	pand	%xmm5, %xmm9
	pandn	%xmm3, %xmm7
	pand	%xmm5, %xmm3
	por	%xmm9, %xmm7
	movdqa	%xmm5, %xmm9
	pandn	%xmm15, %xmm9
	movdqa	-112(%rbp), %xmm15
	por	%xmm9, %xmm3
	movaps	%xmm3, -144(%rbp)
	movdqa	%xmm15, %xmm3
	movdqa	%xmm15, %xmm5
	movdqa	%xmm15, %xmm9
	pcmpeqd	%xmm2, %xmm3
	psubq	%xmm2, %xmm5
	pand	%xmm5, %xmm3
	movdqa	%xmm2, %xmm5
	pcmpgtd	%xmm15, %xmm5
	por	%xmm5, %xmm3
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm5
	pand	%xmm3, %xmm9
	pandn	%xmm2, %xmm5
	pand	%xmm3, %xmm2
	por	%xmm9, %xmm5
	movdqa	%xmm3, %xmm9
	movdqa	%xmm13, %xmm3
	pandn	%xmm15, %xmm9
	psubq	%xmm4, %xmm3
	movdqa	-128(%rbp), %xmm15
	por	%xmm9, %xmm2
	movaps	%xmm2, -64(%rbp)
	movdqa	%xmm4, %xmm2
	pcmpeqd	%xmm13, %xmm2
	pand	%xmm3, %xmm2
	movdqa	%xmm4, %xmm3
	pcmpgtd	%xmm13, %xmm3
	por	%xmm3, %xmm2
	movdqa	%xmm13, %xmm3
	pshufd	$245, %xmm2, %xmm2
	movdqa	%xmm2, %xmm9
	pand	%xmm2, %xmm3
	pandn	%xmm4, %xmm9
	pand	%xmm2, %xmm4
	por	%xmm9, %xmm3
	movdqa	%xmm2, %xmm9
	movdqa	%xmm6, %xmm2
	pandn	%xmm13, %xmm9
	pcmpeqd	%xmm8, %xmm2
	por	%xmm9, %xmm4
	movdqa	%xmm8, %xmm9
	psubq	%xmm6, %xmm9
	pand	%xmm9, %xmm2
	movdqa	%xmm6, %xmm9
	pcmpgtd	%xmm8, %xmm9
	por	%xmm9, %xmm2
	movdqa	%xmm8, %xmm9
	pshufd	$245, %xmm2, %xmm2
	movdqa	%xmm2, %xmm11
	pand	%xmm2, %xmm9
	pandn	%xmm6, %xmm11
	pand	%xmm2, %xmm6
	por	%xmm11, %xmm9
	movdqa	%xmm2, %xmm11
	movdqa	%xmm15, %xmm2
	pandn	%xmm8, %xmm11
	movdqa	%xmm15, %xmm8
	psubq	%xmm0, %xmm2
	pcmpeqd	%xmm0, %xmm8
	por	%xmm11, %xmm6
	pand	%xmm2, %xmm8
	movdqa	%xmm0, %xmm2
	pcmpgtd	%xmm15, %xmm2
	por	%xmm2, %xmm8
	movdqa	%xmm15, %xmm2
	pshufd	$245, %xmm8, %xmm8
	movdqa	%xmm8, %xmm11
	pand	%xmm8, %xmm2
	pandn	%xmm0, %xmm11
	pand	%xmm8, %xmm0
	por	%xmm11, %xmm2
	movdqa	%xmm8, %xmm11
	pandn	%xmm15, %xmm11
	movdqa	-80(%rbp), %xmm15
	por	%xmm11, %xmm0
	movdqa	%xmm15, %xmm11
	movdqa	%xmm15, %xmm8
	pcmpeqd	%xmm1, %xmm11
	psubq	%xmm1, %xmm8
	pand	%xmm8, %xmm11
	movdqa	%xmm1, %xmm8
	pcmpgtd	%xmm15, %xmm8
	por	%xmm8, %xmm11
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm8
	pandn	%xmm1, %xmm8
	pand	%xmm11, %xmm1
	movdqa	%xmm8, %xmm13
	movdqa	%xmm15, %xmm8
	pand	%xmm11, %xmm8
	por	%xmm13, %xmm8
	movdqa	%xmm11, %xmm13
	movdqa	%xmm3, %xmm11
	pcmpeqd	%xmm12, %xmm11
	pandn	%xmm15, %xmm13
	movdqa	%xmm12, %xmm15
	psubq	%xmm3, %xmm15
	por	%xmm13, %xmm1
	pand	%xmm15, %xmm11
	movdqa	%xmm3, %xmm15
	pcmpgtd	%xmm12, %xmm15
	por	%xmm15, %xmm11
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm15
	pandn	%xmm3, %xmm15
	pand	%xmm11, %xmm3
	movdqa	%xmm15, %xmm13
	movdqa	%xmm12, %xmm15
	pand	%xmm11, %xmm15
	por	%xmm13, %xmm15
	movaps	%xmm15, -112(%rbp)
	movdqa	%xmm11, %xmm15
	movdqa	%xmm10, %xmm11
	pandn	%xmm12, %xmm15
	psubq	%xmm9, %xmm11
	movdqa	%xmm10, %xmm12
	movdqa	%xmm15, %xmm13
	movdqa	%xmm3, %xmm15
	movdqa	%xmm9, %xmm3
	pcmpeqd	%xmm10, %xmm3
	por	%xmm13, %xmm15
	movdqa	-96(%rbp), %xmm13
	pand	%xmm11, %xmm3
	movdqa	%xmm9, %xmm11
	pcmpgtd	%xmm10, %xmm11
	por	%xmm11, %xmm3
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm11
	pand	%xmm3, %xmm12
	pandn	%xmm9, %xmm11
	pand	%xmm3, %xmm9
	por	%xmm11, %xmm12
	movdqa	%xmm3, %xmm11
	movdqa	%xmm7, %xmm3
	pandn	%xmm10, %xmm11
	movdqa	%xmm2, %xmm10
	psubq	%xmm2, %xmm3
	movaps	%xmm12, -80(%rbp)
	pcmpeqd	%xmm7, %xmm10
	por	%xmm11, %xmm9
	pand	%xmm3, %xmm10
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm7, %xmm3
	por	%xmm3, %xmm10
	movdqa	%xmm7, %xmm3
	pshufd	$245, %xmm10, %xmm10
	movdqa	%xmm10, %xmm11
	pand	%xmm10, %xmm3
	pandn	%xmm2, %xmm11
	pand	%xmm10, %xmm2
	por	%xmm11, %xmm3
	movdqa	%xmm10, %xmm11
	movdqa	%xmm8, %xmm10
	pcmpeqd	%xmm5, %xmm10
	pandn	%xmm7, %xmm11
	movdqa	%xmm5, %xmm7
	psubq	%xmm8, %xmm7
	por	%xmm11, %xmm2
	pand	%xmm7, %xmm10
	movdqa	%xmm8, %xmm7
	pcmpgtd	%xmm5, %xmm7
	por	%xmm7, %xmm10
	movdqa	%xmm5, %xmm7
	pshufd	$245, %xmm10, %xmm10
	movdqa	%xmm10, %xmm11
	pand	%xmm10, %xmm7
	pandn	%xmm8, %xmm11
	pand	%xmm10, %xmm8
	por	%xmm11, %xmm7
	movdqa	%xmm10, %xmm11
	movdqa	%xmm4, %xmm10
	pcmpeqd	%xmm14, %xmm10
	pandn	%xmm5, %xmm11
	movdqa	%xmm14, %xmm5
	psubq	%xmm4, %xmm5
	por	%xmm11, %xmm8
	pand	%xmm5, %xmm10
	movdqa	%xmm4, %xmm5
	pcmpgtd	%xmm14, %xmm5
	por	%xmm5, %xmm10
	movdqa	%xmm14, %xmm5
	pshufd	$245, %xmm10, %xmm10
	movdqa	%xmm10, %xmm11
	pand	%xmm10, %xmm5
	pandn	%xmm4, %xmm11
	pand	%xmm10, %xmm4
	por	%xmm11, %xmm5
	movdqa	%xmm10, %xmm11
	movdqa	%xmm13, %xmm10
	pandn	%xmm14, %xmm11
	psubq	%xmm6, %xmm10
	movdqa	-144(%rbp), %xmm14
	por	%xmm11, %xmm4
	movdqa	%xmm13, %xmm11
	pcmpeqd	%xmm6, %xmm11
	pand	%xmm10, %xmm11
	movdqa	%xmm6, %xmm10
	pcmpgtd	%xmm13, %xmm10
	por	%xmm10, %xmm11
	movdqa	%xmm13, %xmm10
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm10
	pandn	%xmm6, %xmm12
	pand	%xmm11, %xmm6
	por	%xmm12, %xmm10
	movdqa	%xmm11, %xmm12
	movdqa	%xmm14, %xmm11
	pandn	%xmm13, %xmm12
	pcmpeqd	%xmm0, %xmm11
	por	%xmm12, %xmm6
	movdqa	%xmm14, %xmm12
	psubq	%xmm0, %xmm12
	pand	%xmm12, %xmm11
	movdqa	%xmm0, %xmm12
	pcmpgtd	%xmm14, %xmm12
	por	%xmm12, %xmm11
	movdqa	%xmm14, %xmm12
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm13
	pand	%xmm11, %xmm12
	pandn	%xmm0, %xmm13
	pand	%xmm11, %xmm0
	por	%xmm13, %xmm12
	movdqa	%xmm11, %xmm13
	pandn	%xmm14, %xmm13
	movdqa	-64(%rbp), %xmm14
	por	%xmm13, %xmm0
	movdqa	%xmm14, %xmm11
	movdqa	%xmm14, %xmm13
	movaps	%xmm0, -96(%rbp)
	pcmpeqd	%xmm1, %xmm11
	psubq	%xmm1, %xmm13
	pand	%xmm13, %xmm11
	movdqa	%xmm1, %xmm13
	pcmpgtd	%xmm14, %xmm13
	por	%xmm13, %xmm11
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm13
	movdqa	%xmm11, %xmm0
	pandn	-64(%rbp), %xmm0
	pandn	%xmm1, %xmm13
	pand	%xmm11, %xmm1
	pand	%xmm11, %xmm14
	por	%xmm0, %xmm1
	movdqa	%xmm10, %xmm11
	por	%xmm14, %xmm13
	movaps	%xmm1, -160(%rbp)
	movdqa	%xmm2, %xmm1
	psubq	%xmm2, %xmm11
	movdqa	%xmm7, %xmm0
	pcmpeqd	%xmm10, %xmm1
	psubq	%xmm4, %xmm0
	pand	%xmm11, %xmm1
	movdqa	%xmm2, %xmm11
	pcmpgtd	%xmm10, %xmm11
	por	%xmm11, %xmm1
	movdqa	%xmm10, %xmm11
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm14
	pand	%xmm1, %xmm11
	pandn	%xmm2, %xmm14
	pand	%xmm1, %xmm2
	por	%xmm14, %xmm11
	movdqa	%xmm1, %xmm14
	movdqa	%xmm12, %xmm1
	pandn	%xmm10, %xmm14
	movdqa	%xmm9, %xmm10
	psubq	%xmm9, %xmm1
	pcmpeqd	%xmm12, %xmm10
	por	%xmm14, %xmm2
	pand	%xmm1, %xmm10
	movdqa	%xmm9, %xmm1
	pcmpgtd	%xmm12, %xmm1
	por	%xmm1, %xmm10
	movdqa	%xmm12, %xmm1
	pshufd	$245, %xmm10, %xmm10
	movdqa	%xmm10, %xmm14
	pand	%xmm10, %xmm1
	pandn	%xmm9, %xmm14
	pand	%xmm10, %xmm9
	por	%xmm14, %xmm1
	movdqa	%xmm10, %xmm14
	movdqa	%xmm4, %xmm10
	pcmpeqd	%xmm7, %xmm10
	pandn	%xmm12, %xmm14
	por	%xmm14, %xmm9
	movdqa	%xmm7, %xmm14
	pand	%xmm0, %xmm10
	movdqa	%xmm4, %xmm0
	pcmpgtd	%xmm7, %xmm0
	por	%xmm0, %xmm10
	pshufd	$245, %xmm10, %xmm10
	movdqa	%xmm10, %xmm0
	pand	%xmm10, %xmm14
	pandn	%xmm4, %xmm0
	pand	%xmm10, %xmm4
	por	%xmm0, %xmm14
	movdqa	%xmm10, %xmm0
	pandn	%xmm7, %xmm0
	movdqa	%xmm13, %xmm7
	por	%xmm0, %xmm4
	psubq	%xmm8, %xmm7
	movaps	%xmm4, -64(%rbp)
	movdqa	%xmm8, %xmm4
	pcmpeqd	%xmm13, %xmm4
	pand	%xmm7, %xmm4
	movdqa	%xmm8, %xmm7
	pcmpgtd	%xmm13, %xmm7
	por	%xmm7, %xmm4
	movdqa	%xmm13, %xmm7
	pshufd	$245, %xmm4, %xmm4
	movdqa	%xmm4, %xmm10
	pand	%xmm4, %xmm7
	pandn	%xmm8, %xmm10
	pand	%xmm4, %xmm8
	por	%xmm10, %xmm7
	movdqa	%xmm4, %xmm10
	pandn	%xmm13, %xmm10
	movdqa	-96(%rbp), %xmm13
	por	%xmm10, %xmm8
	movdqa	%xmm6, %xmm10
	movdqa	%xmm13, %xmm4
	psubq	%xmm13, %xmm10
	pcmpeqd	%xmm6, %xmm4
	pand	%xmm10, %xmm4
	movdqa	%xmm13, %xmm10
	pcmpgtd	%xmm6, %xmm10
	por	%xmm10, %xmm4
	movdqa	%xmm6, %xmm10
	pshufd	$245, %xmm4, %xmm4
	movdqa	%xmm4, %xmm0
	pand	%xmm4, %xmm10
	pandn	%xmm13, %xmm0
	por	%xmm0, %xmm10
	movdqa	%xmm4, %xmm0
	pandn	%xmm6, %xmm0
	movdqa	%xmm15, %xmm6
	movdqa	%xmm0, %xmm12
	pcmpeqd	%xmm5, %xmm6
	movdqa	%xmm13, %xmm0
	movdqa	-80(%rbp), %xmm13
	pand	%xmm4, %xmm0
	movdqa	%xmm5, %xmm4
	psubq	%xmm15, %xmm4
	por	%xmm12, %xmm0
	pand	%xmm4, %xmm6
	movdqa	%xmm15, %xmm4
	pcmpgtd	%xmm5, %xmm4
	por	%xmm4, %xmm6
	movdqa	%xmm5, %xmm4
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm12
	pand	%xmm6, %xmm4
	pandn	%xmm15, %xmm12
	pand	%xmm6, %xmm15
	por	%xmm12, %xmm4
	movdqa	%xmm6, %xmm12
	movdqa	%xmm13, %xmm6
	pandn	%xmm5, %xmm12
	movdqa	%xmm13, %xmm5
	psubq	%xmm3, %xmm6
	pcmpeqd	%xmm3, %xmm5
	por	%xmm12, %xmm15
	pand	%xmm6, %xmm5
	movdqa	%xmm3, %xmm6
	pcmpgtd	%xmm13, %xmm6
	por	%xmm6, %xmm5
	movdqa	%xmm13, %xmm6
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm12
	pand	%xmm5, %xmm6
	pandn	%xmm3, %xmm12
	pand	%xmm5, %xmm3
	por	%xmm12, %xmm6
	movdqa	%xmm5, %xmm12
	movdqa	%xmm4, %xmm5
	pandn	%xmm13, %xmm12
	pcmpeqd	%xmm6, %xmm5
	movdqa	%xmm6, %xmm13
	por	%xmm12, %xmm3
	movdqa	%xmm6, %xmm12
	psubq	%xmm4, %xmm12
	pand	%xmm12, %xmm5
	movdqa	%xmm4, %xmm12
	pcmpgtd	%xmm6, %xmm12
	por	%xmm12, %xmm5
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm12
	pand	%xmm5, %xmm13
	pandn	%xmm4, %xmm12
	pand	%xmm5, %xmm4
	por	%xmm12, %xmm13
	movdqa	%xmm5, %xmm12
	movdqa	%xmm7, %xmm5
	pandn	%xmm6, %xmm12
	movdqa	%xmm10, %xmm6
	psubq	%xmm10, %xmm5
	movaps	%xmm13, -128(%rbp)
	pcmpeqd	%xmm7, %xmm6
	por	%xmm12, %xmm4
	pand	%xmm5, %xmm6
	movdqa	%xmm10, %xmm5
	pcmpgtd	%xmm7, %xmm5
	por	%xmm5, %xmm6
	movdqa	%xmm7, %xmm5
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm12
	pand	%xmm6, %xmm5
	pandn	%xmm10, %xmm12
	pand	%xmm6, %xmm10
	por	%xmm12, %xmm5
	movdqa	%xmm6, %xmm12
	movdqa	%xmm3, %xmm6
	pandn	%xmm7, %xmm12
	movdqa	%xmm15, %xmm7
	psubq	%xmm15, %xmm6
	pcmpeqd	%xmm3, %xmm7
	por	%xmm12, %xmm10
	pand	%xmm6, %xmm7
	movdqa	%xmm15, %xmm6
	pcmpgtd	%xmm3, %xmm6
	por	%xmm6, %xmm7
	movdqa	%xmm3, %xmm6
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm12
	pand	%xmm7, %xmm6
	pandn	%xmm15, %xmm12
	pand	%xmm7, %xmm15
	por	%xmm12, %xmm6
	movdqa	%xmm7, %xmm12
	movdqa	%xmm0, %xmm7
	pcmpeqd	%xmm8, %xmm7
	pandn	%xmm3, %xmm12
	movdqa	%xmm8, %xmm3
	psubq	%xmm0, %xmm3
	por	%xmm12, %xmm15
	pand	%xmm3, %xmm7
	movdqa	%xmm0, %xmm3
	pcmpgtd	%xmm8, %xmm3
	por	%xmm3, %xmm7
	movdqa	%xmm8, %xmm3
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm12
	pand	%xmm7, %xmm3
	pandn	%xmm0, %xmm12
	pand	%xmm7, %xmm0
	por	%xmm12, %xmm3
	movdqa	%xmm7, %xmm12
	movdqa	%xmm6, %xmm7
	pandn	%xmm8, %xmm12
	psubq	%xmm4, %xmm7
	movdqa	%xmm6, %xmm8
	por	%xmm12, %xmm0
	movdqa	%xmm5, %xmm12
	movaps	%xmm0, -176(%rbp)
	movdqa	%xmm4, %xmm0
	pcmpeqd	%xmm6, %xmm0
	pand	%xmm7, %xmm0
	movdqa	%xmm4, %xmm7
	pcmpgtd	%xmm6, %xmm7
	por	%xmm7, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm7
	pand	%xmm0, %xmm8
	pandn	%xmm4, %xmm7
	pand	%xmm0, %xmm4
	por	%xmm7, %xmm8
	movdqa	%xmm0, %xmm7
	movdqa	%xmm11, %xmm0
	pandn	%xmm6, %xmm7
	movdqa	%xmm1, %xmm6
	psubq	%xmm1, %xmm0
	movaps	%xmm8, -144(%rbp)
	pcmpeqd	%xmm11, %xmm6
	por	%xmm7, %xmm4
	movdqa	%xmm9, %xmm8
	cmpq	$1, -192(%rbp)
	pand	%xmm0, %xmm6
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm11, %xmm0
	por	%xmm0, %xmm6
	movdqa	%xmm11, %xmm0
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm7
	pand	%xmm6, %xmm0
	pandn	%xmm1, %xmm7
	pand	%xmm6, %xmm1
	por	%xmm7, %xmm0
	movdqa	%xmm6, %xmm7
	pandn	%xmm11, %xmm7
	por	%xmm7, %xmm1
	movdqa	%xmm9, %xmm7
	movdqa	%xmm1, %xmm6
	movdqa	%xmm2, %xmm1
	psubq	%xmm2, %xmm7
	pcmpeqd	%xmm9, %xmm1
	pand	%xmm7, %xmm1
	movdqa	%xmm2, %xmm7
	pcmpgtd	%xmm9, %xmm7
	por	%xmm7, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm7
	pand	%xmm1, %xmm8
	pandn	%xmm2, %xmm7
	pand	%xmm1, %xmm2
	por	%xmm7, %xmm8
	movdqa	%xmm1, %xmm7
	movdqa	%xmm10, %xmm1
	pandn	%xmm9, %xmm7
	pcmpeqd	%xmm3, %xmm1
	por	%xmm7, %xmm2
	movdqa	%xmm3, %xmm7
	psubq	%xmm10, %xmm7
	pand	%xmm7, %xmm1
	movdqa	%xmm10, %xmm7
	pcmpgtd	%xmm3, %xmm7
	por	%xmm7, %xmm1
	movdqa	%xmm3, %xmm7
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm9
	pand	%xmm1, %xmm7
	pandn	%xmm10, %xmm9
	pand	%xmm1, %xmm10
	por	%xmm9, %xmm7
	movdqa	%xmm1, %xmm9
	movdqa	%xmm14, %xmm1
	pandn	%xmm3, %xmm9
	movdqa	%xmm15, %xmm3
	psubq	%xmm15, %xmm1
	pcmpeqd	%xmm14, %xmm3
	movdqa	%xmm10, %xmm13
	movdqa	%xmm14, %xmm10
	por	%xmm9, %xmm13
	movdqa	%xmm0, %xmm9
	pand	%xmm1, %xmm3
	movdqa	%xmm15, %xmm1
	pcmpgtd	%xmm14, %xmm1
	por	%xmm1, %xmm3
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm1
	pand	%xmm3, %xmm10
	pandn	%xmm15, %xmm1
	pand	%xmm3, %xmm15
	por	%xmm1, %xmm10
	movdqa	%xmm3, %xmm1
	movdqa	%xmm5, %xmm3
	pandn	%xmm14, %xmm1
	movdqa	-64(%rbp), %xmm14
	pcmpeqd	%xmm10, %xmm9
	movdqa	%xmm10, %xmm11
	por	%xmm1, %xmm15
	movdqa	%xmm14, %xmm1
	psubq	%xmm14, %xmm3
	pcmpeqd	%xmm5, %xmm1
	pand	%xmm3, %xmm1
	movdqa	%xmm14, %xmm3
	pcmpgtd	%xmm5, %xmm3
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm1, %xmm12
	pandn	%xmm14, %xmm3
	por	%xmm3, %xmm12
	movdqa	%xmm1, %xmm3
	pand	%xmm14, %xmm1
	pandn	%xmm5, %xmm3
	por	%xmm3, %xmm1
	movdqa	%xmm10, %xmm3
	psubq	%xmm0, %xmm3
	pand	%xmm3, %xmm9
	movdqa	%xmm0, %xmm3
	pcmpgtd	%xmm10, %xmm3
	por	%xmm3, %xmm9
	pshufd	$245, %xmm9, %xmm9
	movdqa	%xmm9, %xmm3
	pand	%xmm9, %xmm11
	pandn	%xmm0, %xmm3
	por	%xmm3, %xmm11
	movdqa	%xmm9, %xmm3
	pand	%xmm0, %xmm9
	pandn	%xmm10, %xmm3
	movdqa	%xmm6, %xmm0
	movdqa	%xmm12, %xmm10
	por	%xmm3, %xmm9
	movdqa	%xmm15, %xmm3
	psubq	%xmm15, %xmm0
	pcmpeqd	%xmm6, %xmm3
	pand	%xmm0, %xmm3
	movdqa	%xmm15, %xmm0
	pcmpgtd	%xmm6, %xmm0
	por	%xmm0, %xmm3
	movdqa	%xmm6, %xmm0
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm5
	pand	%xmm3, %xmm0
	pandn	%xmm15, %xmm5
	por	%xmm5, %xmm0
	movdqa	%xmm3, %xmm5
	pand	%xmm15, %xmm3
	pandn	%xmm6, %xmm5
	movdqa	%xmm12, %xmm6
	por	%xmm5, %xmm3
	movdqa	%xmm8, %xmm5
	psubq	%xmm8, %xmm6
	pcmpeqd	%xmm12, %xmm5
	pand	%xmm6, %xmm5
	movdqa	%xmm8, %xmm6
	pcmpgtd	%xmm12, %xmm6
	por	%xmm6, %xmm5
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm6
	pand	%xmm5, %xmm10
	pandn	%xmm8, %xmm6
	pand	%xmm5, %xmm8
	por	%xmm6, %xmm10
	movdqa	%xmm5, %xmm6
	movdqa	%xmm1, %xmm5
	pandn	%xmm12, %xmm6
	pcmpeqd	%xmm2, %xmm5
	por	%xmm6, %xmm8
	movdqa	%xmm2, %xmm6
	psubq	%xmm1, %xmm6
	pand	%xmm6, %xmm5
	movdqa	%xmm1, %xmm6
	pcmpgtd	%xmm2, %xmm6
	por	%xmm6, %xmm5
	movdqa	%xmm2, %xmm6
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm12
	pand	%xmm5, %xmm6
	pandn	%xmm1, %xmm12
	pand	%xmm5, %xmm1
	por	%xmm12, %xmm6
	movdqa	%xmm5, %xmm12
	movdqa	%xmm11, %xmm5
	pandn	%xmm2, %xmm12
	movdqa	%xmm4, %xmm2
	psubq	%xmm4, %xmm5
	pcmpeqd	%xmm11, %xmm2
	por	%xmm12, %xmm1
	movdqa	%xmm11, %xmm12
	pand	%xmm5, %xmm2
	movdqa	%xmm4, %xmm5
	pcmpgtd	%xmm11, %xmm5
	por	%xmm5, %xmm2
	pshufd	$245, %xmm2, %xmm2
	movdqa	%xmm2, %xmm5
	pand	%xmm2, %xmm12
	pandn	%xmm4, %xmm5
	pand	%xmm2, %xmm4
	por	%xmm12, %xmm5
	movdqa	%xmm2, %xmm12
	movdqa	%xmm0, %xmm2
	pandn	%xmm11, %xmm12
	pcmpeqd	%xmm9, %xmm2
	movdqa	%xmm9, %xmm11
	por	%xmm12, %xmm4
	movaps	%xmm4, -64(%rbp)
	movdqa	%xmm9, %xmm4
	psubq	%xmm0, %xmm4
	pand	%xmm4, %xmm2
	movdqa	%xmm0, %xmm4
	pcmpgtd	%xmm9, %xmm4
	por	%xmm4, %xmm2
	pshufd	$245, %xmm2, %xmm2
	movdqa	%xmm2, %xmm4
	pand	%xmm2, %xmm11
	pandn	%xmm0, %xmm4
	pand	%xmm2, %xmm0
	por	%xmm4, %xmm11
	movdqa	%xmm2, %xmm4
	movdqa	%xmm3, %xmm2
	pandn	%xmm9, %xmm4
	pcmpeqd	%xmm10, %xmm2
	movdqa	%xmm10, %xmm9
	movaps	%xmm11, -80(%rbp)
	por	%xmm4, %xmm0
	movdqa	%xmm10, %xmm4
	psubq	%xmm3, %xmm4
	pand	%xmm4, %xmm2
	movdqa	%xmm3, %xmm4
	pcmpgtd	%xmm10, %xmm4
	por	%xmm4, %xmm2
	pshufd	$245, %xmm2, %xmm2
	movdqa	%xmm2, %xmm4
	pand	%xmm2, %xmm9
	pandn	%xmm3, %xmm4
	pand	%xmm2, %xmm3
	por	%xmm4, %xmm9
	movdqa	%xmm2, %xmm4
	movdqa	%xmm8, %xmm2
	pandn	%xmm10, %xmm4
	psubq	%xmm6, %xmm2
	por	%xmm4, %xmm3
	movdqa	%xmm6, %xmm4
	pcmpeqd	%xmm8, %xmm4
	pand	%xmm2, %xmm4
	movdqa	%xmm6, %xmm2
	pcmpgtd	%xmm8, %xmm2
	por	%xmm2, %xmm4
	movdqa	%xmm8, %xmm2
	pshufd	$245, %xmm4, %xmm4
	movdqa	%xmm4, %xmm10
	pand	%xmm4, %xmm2
	pandn	%xmm6, %xmm10
	pand	%xmm4, %xmm6
	por	%xmm10, %xmm2
	movdqa	%xmm4, %xmm10
	movdqa	%xmm7, %xmm4
	pandn	%xmm8, %xmm10
	movdqa	%xmm1, %xmm8
	psubq	%xmm1, %xmm4
	pcmpeqd	%xmm7, %xmm8
	por	%xmm10, %xmm6
	pand	%xmm4, %xmm8
	movdqa	%xmm1, %xmm4
	pcmpgtd	%xmm7, %xmm4
	por	%xmm4, %xmm8
	movdqa	%xmm7, %xmm4
	pshufd	$245, %xmm8, %xmm8
	movdqa	%xmm8, %xmm10
	pand	%xmm8, %xmm4
	pandn	%xmm1, %xmm10
	pand	%xmm8, %xmm1
	por	%xmm10, %xmm4
	movdqa	%xmm8, %xmm10
	movdqa	%xmm0, %xmm8
	pandn	%xmm7, %xmm10
	movdqa	%xmm9, %xmm7
	psubq	%xmm9, %xmm8
	pcmpeqd	%xmm0, %xmm7
	por	%xmm10, %xmm1
	movdqa	%xmm0, %xmm10
	pand	%xmm8, %xmm7
	movdqa	%xmm9, %xmm8
	pcmpgtd	%xmm0, %xmm8
	por	%xmm8, %xmm7
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm8
	pand	%xmm7, %xmm10
	pandn	%xmm9, %xmm8
	pand	%xmm7, %xmm9
	por	%xmm8, %xmm10
	movdqa	%xmm7, %xmm8
	movdqa	%xmm2, %xmm7
	pcmpeqd	%xmm3, %xmm7
	pandn	%xmm0, %xmm8
	movdqa	%xmm3, %xmm0
	movaps	%xmm10, -96(%rbp)
	psubq	%xmm2, %xmm0
	movdqa	%xmm9, %xmm14
	por	%xmm8, %xmm14
	pand	%xmm0, %xmm7
	movdqa	%xmm2, %xmm0
	pcmpgtd	%xmm3, %xmm0
	por	%xmm0, %xmm7
	movdqa	%xmm3, %xmm0
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm8
	pand	%xmm7, %xmm0
	pandn	%xmm2, %xmm8
	pand	%xmm7, %xmm2
	por	%xmm8, %xmm0
	movdqa	%xmm7, %xmm8
	pandn	%xmm3, %xmm8
	por	%xmm8, %xmm2
	jbe	.L77
	movdqa	-112(%rbp), %xmm7
	pshufd	$78, %xmm4, %xmm4
	pshufd	$78, %xmm6, %xmm6
	pshufd	$78, -160(%rbp), %xmm10
	pshufd	$78, %xmm1, %xmm11
	pshufd	$78, %xmm13, %xmm8
	movdqa	-128(%rbp), %xmm9
	pshufd	$78, -176(%rbp), %xmm15
	movdqa	%xmm7, %xmm3
	movdqa	%xmm7, %xmm1
	movdqa	%xmm7, %xmm13
	movaps	%xmm11, -176(%rbp)
	pcmpeqd	%xmm10, %xmm3
	psubq	%xmm10, %xmm1
	movdqa	%xmm9, %xmm12
	pshufd	$78, %xmm2, %xmm2
	pshufd	$78, %xmm0, %xmm0
	pand	%xmm1, %xmm3
	movdqa	%xmm10, %xmm1
	pcmpgtd	%xmm7, %xmm1
	por	%xmm1, %xmm3
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm1
	pand	%xmm3, %xmm13
	movaps	%xmm3, -208(%rbp)
	pandn	%xmm10, %xmm1
	por	%xmm1, %xmm13
	movdqa	%xmm3, %xmm1
	movdqa	-144(%rbp), %xmm3
	pandn	%xmm7, %xmm1
	movdqa	%xmm9, %xmm7
	pcmpeqd	%xmm15, %xmm7
	movaps	%xmm1, -256(%rbp)
	movdqa	%xmm9, %xmm1
	psubq	%xmm15, %xmm1
	pand	%xmm1, %xmm7
	movdqa	%xmm15, %xmm1
	pcmpgtd	%xmm9, %xmm1
	por	%xmm1, %xmm7
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm1
	pand	%xmm7, %xmm12
	pandn	%xmm15, %xmm1
	por	%xmm1, %xmm12
	movdqa	%xmm7, %xmm1
	pand	%xmm15, %xmm7
	pandn	%xmm9, %xmm1
	movdqa	%xmm8, %xmm9
	movaps	%xmm1, -272(%rbp)
	movdqa	%xmm3, %xmm1
	psubq	%xmm8, %xmm1
	pcmpeqd	%xmm3, %xmm8
	movaps	%xmm9, -192(%rbp)
	pand	%xmm1, %xmm8
	movdqa	%xmm9, %xmm1
	movdqa	%xmm3, %xmm9
	pcmpgtd	%xmm3, %xmm1
	por	%xmm1, %xmm8
	pshufd	$245, %xmm8, %xmm8
	movdqa	%xmm8, %xmm1
	pandn	-192(%rbp), %xmm1
	pand	%xmm8, %xmm9
	por	%xmm1, %xmm9
	movdqa	%xmm5, %xmm1
	movaps	%xmm9, -128(%rbp)
	movdqa	%xmm8, %xmm9
	psubq	%xmm11, %xmm1
	pand	-192(%rbp), %xmm8
	pandn	%xmm3, %xmm9
	movaps	%xmm9, -288(%rbp)
	movdqa	%xmm1, %xmm9
	movdqa	%xmm11, %xmm1
	por	-288(%rbp), %xmm8
	pcmpeqd	%xmm5, %xmm1
	pshufd	$78, %xmm8, %xmm8
	pand	%xmm9, %xmm1
	movdqa	%xmm11, %xmm9
	movdqa	%xmm5, %xmm11
	pcmpgtd	%xmm5, %xmm9
	por	%xmm9, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm9
	pand	%xmm1, %xmm11
	pandn	-176(%rbp), %xmm9
	por	%xmm9, %xmm11
	movaps	%xmm11, -144(%rbp)
	movdqa	%xmm1, %xmm11
	pand	-176(%rbp), %xmm1
	pandn	%xmm5, %xmm11
	movaps	%xmm11, -304(%rbp)
	movdqa	-64(%rbp), %xmm11
	por	-304(%rbp), %xmm1
	movdqa	%xmm11, %xmm5
	movdqa	%xmm11, %xmm9
	pshufd	$78, %xmm1, %xmm1
	pcmpeqd	%xmm4, %xmm5
	psubq	%xmm4, %xmm9
	pand	%xmm9, %xmm5
	movdqa	%xmm4, %xmm9
	pcmpgtd	%xmm11, %xmm9
	por	%xmm9, %xmm5
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm9
	pandn	%xmm4, %xmm9
	pand	%xmm5, %xmm4
	movaps	%xmm9, -320(%rbp)
	movdqa	%xmm5, %xmm9
	pand	-64(%rbp), %xmm5
	por	-320(%rbp), %xmm5
	pandn	%xmm11, %xmm9
	movdqa	-80(%rbp), %xmm11
	por	%xmm9, %xmm4
	pshufd	$78, %xmm5, %xmm5
	movaps	%xmm4, -112(%rbp)
	movdqa	%xmm11, %xmm4
	movdqa	%xmm11, %xmm9
	pcmpeqd	%xmm6, %xmm4
	psubq	%xmm6, %xmm9
	pand	%xmm9, %xmm4
	movdqa	%xmm6, %xmm9
	pcmpgtd	%xmm11, %xmm9
	por	%xmm9, %xmm4
	pshufd	$245, %xmm4, %xmm4
	movdqa	%xmm4, %xmm3
	movdqa	%xmm4, %xmm9
	pandn	%xmm11, %xmm3
	movdqa	-96(%rbp), %xmm11
	pandn	%xmm6, %xmm9
	pand	%xmm4, %xmm6
	por	%xmm3, %xmm6
	movaps	%xmm9, -336(%rbp)
	pand	-80(%rbp), %xmm4
	por	-336(%rbp), %xmm4
	movdqa	%xmm11, %xmm3
	movaps	%xmm6, -160(%rbp)
	movdqa	%xmm11, %xmm6
	pcmpeqd	%xmm2, %xmm3
	psubq	%xmm2, %xmm6
	pshufd	$78, %xmm4, %xmm4
	movdqa	%xmm3, %xmm9
	pand	%xmm6, %xmm9
	movdqa	%xmm2, %xmm6
	pcmpgtd	%xmm11, %xmm6
	por	%xmm6, %xmm9
	pshufd	$245, %xmm9, %xmm9
	movdqa	%xmm9, %xmm3
	pandn	%xmm2, %xmm3
	pand	%xmm9, %xmm2
	movaps	%xmm3, -352(%rbp)
	movdqa	%xmm9, %xmm3
	pand	-96(%rbp), %xmm9
	por	-272(%rbp), %xmm7
	por	-352(%rbp), %xmm9
	pandn	%xmm11, %xmm3
	por	%xmm3, %xmm2
	movdqa	%xmm14, %xmm3
	pshufd	$78, %xmm7, %xmm15
	psubq	%xmm0, %xmm3
	pshufd	$78, %xmm9, %xmm9
	movdqa	%xmm3, %xmm11
	movdqa	%xmm0, %xmm3
	movaps	%xmm9, -80(%rbp)
	pcmpeqd	%xmm14, %xmm3
	movdqa	%xmm3, %xmm6
	movdqa	%xmm0, %xmm3
	pcmpgtd	%xmm14, %xmm3
	pand	%xmm11, %xmm6
	por	%xmm3, %xmm6
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm3
	pandn	%xmm0, %xmm3
	pand	%xmm6, %xmm0
	movdqa	%xmm3, %xmm11
	movdqa	%xmm6, %xmm3
	pand	%xmm14, %xmm6
	pandn	%xmm14, %xmm3
	por	%xmm11, %xmm6
	por	%xmm3, %xmm0
	pshufd	$78, %xmm6, %xmm6
	movdqa	-208(%rbp), %xmm3
	movdqa	%xmm6, %xmm7
	movdqa	%xmm0, %xmm9
	pcmpeqd	%xmm13, %xmm7
	pand	%xmm10, %xmm3
	movdqa	%xmm13, %xmm10
	por	-256(%rbp), %xmm3
	pshufd	$78, %xmm3, %xmm14
	movdqa	%xmm13, %xmm3
	movdqa	%xmm7, %xmm11
	psubq	%xmm6, %xmm3
	movdqa	%xmm14, %xmm7
	pand	%xmm3, %xmm11
	pcmpeqd	%xmm0, %xmm7
	movdqa	%xmm6, %xmm3
	pcmpgtd	%xmm13, %xmm3
	por	%xmm3, %xmm11
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm3
	pand	%xmm11, %xmm10
	pandn	%xmm6, %xmm3
	por	%xmm3, %xmm10
	movdqa	%xmm11, %xmm3
	pand	%xmm6, %xmm11
	pandn	%xmm13, %xmm3
	movdqa	-80(%rbp), %xmm13
	movaps	%xmm3, -96(%rbp)
	movdqa	%xmm0, %xmm3
	psubq	%xmm14, %xmm3
	pand	%xmm3, %xmm7
	movdqa	%xmm14, %xmm3
	pcmpgtd	%xmm0, %xmm3
	por	%xmm3, %xmm7
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm3
	pand	%xmm7, %xmm9
	movaps	%xmm7, -256(%rbp)
	pandn	%xmm14, %xmm3
	por	%xmm3, %xmm9
	movdqa	%xmm7, %xmm3
	movdqa	-112(%rbp), %xmm7
	pandn	%xmm0, %xmm3
	movdqa	%xmm13, %xmm0
	pcmpeqd	%xmm12, %xmm0
	movaps	%xmm3, -272(%rbp)
	movdqa	%xmm12, %xmm3
	psubq	%xmm13, %xmm3
	pand	%xmm3, %xmm0
	movdqa	%xmm13, %xmm3
	movdqa	%xmm12, %xmm13
	pcmpgtd	%xmm12, %xmm3
	por	%xmm3, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	pandn	-80(%rbp), %xmm3
	pand	%xmm0, %xmm13
	por	%xmm3, %xmm13
	movdqa	%xmm0, %xmm3
	pand	-80(%rbp), %xmm0
	pandn	%xmm12, %xmm3
	movdqa	%xmm2, %xmm12
	movaps	%xmm13, -176(%rbp)
	movdqa	%xmm2, %xmm13
	movaps	%xmm3, -288(%rbp)
	movdqa	%xmm15, %xmm3
	psubq	%xmm15, %xmm12
	por	-288(%rbp), %xmm0
	pcmpeqd	%xmm2, %xmm3
	pshufd	$78, %xmm0, %xmm0
	pand	%xmm12, %xmm3
	movdqa	%xmm15, %xmm12
	pcmpgtd	%xmm2, %xmm12
	por	%xmm12, %xmm3
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm12
	pand	%xmm3, %xmm13
	pandn	%xmm15, %xmm12
	por	%xmm12, %xmm13
	movdqa	%xmm3, %xmm12
	pand	%xmm15, %xmm3
	movaps	%xmm13, -192(%rbp)
	movdqa	-128(%rbp), %xmm13
	pandn	%xmm2, %xmm12
	movaps	%xmm12, -304(%rbp)
	movdqa	%xmm13, %xmm2
	psubq	%xmm4, %xmm2
	movdqa	%xmm2, %xmm12
	movdqa	%xmm13, %xmm2
	pcmpeqd	%xmm4, %xmm2
	pand	%xmm12, %xmm2
	movdqa	%xmm4, %xmm12
	pcmpgtd	%xmm13, %xmm12
	por	%xmm12, %xmm2
	pshufd	$245, %xmm2, %xmm2
	movdqa	%xmm2, %xmm12
	pandn	%xmm4, %xmm12
	pand	%xmm2, %xmm4
	movaps	%xmm12, -320(%rbp)
	movdqa	%xmm2, %xmm12
	pand	-128(%rbp), %xmm2
	por	-320(%rbp), %xmm2
	pandn	%xmm13, %xmm12
	movdqa	-160(%rbp), %xmm13
	por	%xmm12, %xmm4
	pshufd	$78, %xmm2, %xmm2
	movaps	%xmm4, -208(%rbp)
	movdqa	%xmm13, %xmm4
	movdqa	%xmm13, %xmm12
	pcmpeqd	%xmm8, %xmm4
	psubq	%xmm8, %xmm12
	pand	%xmm12, %xmm4
	movdqa	%xmm8, %xmm12
	pcmpgtd	%xmm13, %xmm12
	por	%xmm12, %xmm4
	pshufd	$245, %xmm4, %xmm4
	movdqa	%xmm4, %xmm12
	pandn	%xmm8, %xmm12
	pand	%xmm4, %xmm8
	movaps	%xmm12, -336(%rbp)
	movdqa	%xmm4, %xmm12
	pandn	%xmm13, %xmm12
	movdqa	-144(%rbp), %xmm13
	por	%xmm12, %xmm8
	movaps	%xmm8, -64(%rbp)
	movdqa	%xmm13, %xmm8
	movdqa	%xmm13, %xmm12
	pcmpeqd	%xmm5, %xmm8
	psubq	%xmm5, %xmm12
	pand	%xmm12, %xmm8
	movdqa	%xmm5, %xmm12
	pcmpgtd	%xmm13, %xmm12
	por	%xmm12, %xmm8
	pshufd	$245, %xmm8, %xmm8
	movdqa	%xmm8, %xmm12
	pandn	%xmm5, %xmm12
	pand	%xmm8, %xmm5
	movaps	%xmm12, -352(%rbp)
	movdqa	%xmm8, %xmm12
	pand	-144(%rbp), %xmm8
	por	-352(%rbp), %xmm8
	pandn	%xmm13, %xmm12
	movdqa	%xmm7, %xmm13
	por	%xmm12, %xmm5
	movdqa	%xmm7, %xmm12
	psubq	%xmm1, %xmm13
	pcmpeqd	%xmm1, %xmm12
	pshufd	$78, %xmm8, %xmm8
	pand	%xmm13, %xmm12
	movdqa	%xmm1, %xmm13
	pcmpgtd	-112(%rbp), %xmm13
	por	%xmm13, %xmm12
	pshufd	$245, %xmm12, %xmm12
	movdqa	%xmm12, %xmm13
	movdqa	%xmm12, %xmm7
	pandn	-112(%rbp), %xmm7
	pandn	%xmm1, %xmm13
	pand	%xmm12, %xmm1
	por	-96(%rbp), %xmm11
	movdqa	-176(%rbp), %xmm15
	por	%xmm7, %xmm1
	movdqa	-256(%rbp), %xmm7
	por	-304(%rbp), %xmm3
	pshufd	$78, %xmm11, %xmm6
	pand	-160(%rbp), %xmm4
	por	-336(%rbp), %xmm4
	pand	%xmm14, %xmm7
	movaps	%xmm6, -80(%rbp)
	movdqa	-112(%rbp), %xmm6
	por	-272(%rbp), %xmm7
	movdqa	%xmm10, %xmm14
	pshufd	$78, %xmm4, %xmm4
	pshufd	$78, %xmm3, %xmm3
	pshufd	$78, %xmm7, %xmm7
	pand	%xmm12, %xmm6
	movdqa	-80(%rbp), %xmm12
	movaps	%xmm7, -112(%rbp)
	movdqa	%xmm8, %xmm7
	por	%xmm13, %xmm6
	pcmpeqd	%xmm10, %xmm7
	pshufd	$78, %xmm6, %xmm11
	movdqa	%xmm10, %xmm6
	psubq	%xmm8, %xmm6
	movaps	%xmm11, -96(%rbp)
	movdqa	%xmm15, %xmm11
	pcmpeqd	%xmm2, %xmm11
	pand	%xmm6, %xmm7
	movdqa	%xmm8, %xmm6
	pcmpgtd	%xmm10, %xmm6
	por	%xmm6, %xmm7
	pshufd	$245, %xmm7, %xmm7
	pand	%xmm7, %xmm14
	movdqa	%xmm7, %xmm6
	pandn	%xmm8, %xmm6
	movdqa	%xmm14, %xmm13
	por	%xmm6, %xmm13
	movdqa	%xmm7, %xmm6
	pand	%xmm8, %xmm7
	pandn	%xmm10, %xmm6
	movdqa	%xmm5, %xmm10
	movaps	%xmm6, -272(%rbp)
	movdqa	%xmm15, %xmm6
	psubq	%xmm12, %xmm10
	psubq	%xmm2, %xmm6
	pand	%xmm6, %xmm11
	movdqa	%xmm2, %xmm6
	pcmpgtd	%xmm15, %xmm6
	por	%xmm6, %xmm11
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm14
	movdqa	%xmm11, %xmm6
	pandn	%xmm2, %xmm14
	pandn	%xmm15, %xmm6
	pand	%xmm11, %xmm2
	por	%xmm6, %xmm2
	movdqa	%xmm12, %xmm6
	movdqa	%xmm9, %xmm15
	movaps	%xmm14, -288(%rbp)
	pcmpeqd	%xmm5, %xmm6
	movaps	%xmm2, -144(%rbp)
	movdqa	-208(%rbp), %xmm14
	pand	%xmm10, %xmm6
	movdqa	%xmm12, %xmm10
	movdqa	%xmm5, %xmm12
	pcmpgtd	%xmm5, %xmm10
	por	%xmm10, %xmm6
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm10
	pandn	-80(%rbp), %xmm10
	pand	%xmm6, %xmm12
	por	%xmm10, %xmm12
	movdqa	%xmm6, %xmm10
	pandn	%xmm5, %xmm10
	movdqa	%xmm14, %xmm5
	movaps	%xmm10, -304(%rbp)
	movdqa	%xmm14, %xmm10
	psubq	%xmm0, %xmm5
	pcmpeqd	%xmm0, %xmm10
	pand	%xmm5, %xmm10
	movdqa	%xmm0, %xmm5
	pcmpgtd	%xmm14, %xmm5
	por	%xmm5, %xmm10
	pshufd	$245, %xmm10, %xmm10
	movdqa	%xmm10, %xmm5
	pandn	%xmm0, %xmm5
	pand	%xmm10, %xmm0
	movaps	%xmm5, -320(%rbp)
	movdqa	%xmm10, %xmm5
	pandn	%xmm14, %xmm5
	por	%xmm5, %xmm0
	movdqa	-96(%rbp), %xmm5
	psubq	%xmm5, %xmm15
	movdqa	%xmm15, %xmm14
	movdqa	%xmm5, %xmm15
	pcmpeqd	%xmm9, %xmm5
	pand	%xmm14, %xmm5
	movdqa	%xmm15, %xmm14
	movdqa	%xmm9, %xmm15
	pcmpgtd	%xmm9, %xmm14
	por	%xmm14, %xmm5
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm14
	pandn	-96(%rbp), %xmm14
	pand	%xmm5, %xmm15
	por	%xmm14, %xmm15
	movaps	%xmm15, -160(%rbp)
	movdqa	%xmm5, %xmm15
	pandn	%xmm9, %xmm15
	movaps	%xmm15, -336(%rbp)
	movdqa	-192(%rbp), %xmm15
	movdqa	%xmm15, %xmm9
	movdqa	%xmm15, %xmm14
	pcmpeqd	%xmm4, %xmm9
	psubq	%xmm4, %xmm14
	pand	%xmm14, %xmm9
	movdqa	%xmm4, %xmm14
	pcmpgtd	%xmm15, %xmm14
	por	%xmm14, %xmm9
	pshufd	$245, %xmm9, %xmm9
	movdqa	%xmm9, %xmm14
	movdqa	%xmm9, %xmm2
	pandn	%xmm4, %xmm14
	pandn	%xmm15, %xmm2
	pand	%xmm9, %xmm4
	movdqa	-112(%rbp), %xmm15
	por	%xmm2, %xmm4
	movaps	%xmm14, -352(%rbp)
	movdqa	%xmm1, %xmm2
	movaps	%xmm4, -128(%rbp)
	movdqa	%xmm15, %xmm4
	movdqa	%xmm15, %xmm14
	psubq	%xmm15, %xmm2
	pcmpeqd	%xmm1, %xmm4
	pcmpgtd	%xmm1, %xmm14
	pand	%xmm2, %xmm4
	por	%xmm14, %xmm4
	pshufd	$245, %xmm4, %xmm4
	movdqa	%xmm4, %xmm2
	pandn	-112(%rbp), %xmm2
	movdqa	%xmm2, %xmm14
	movdqa	%xmm1, %xmm2
	pand	%xmm4, %xmm2
	movdqa	%xmm2, %xmm15
	movdqa	%xmm4, %xmm2
	pandn	%xmm1, %xmm2
	por	%xmm14, %xmm15
	movaps	%xmm2, -368(%rbp)
	movdqa	-64(%rbp), %xmm2
	movdqa	%xmm2, %xmm14
	movdqa	%xmm2, %xmm1
	pcmpeqd	%xmm3, %xmm14
	psubq	%xmm3, %xmm1
	pand	%xmm1, %xmm14
	movdqa	%xmm3, %xmm1
	pcmpgtd	-64(%rbp), %xmm1
	por	%xmm1, %xmm14
	pshufd	$245, %xmm14, %xmm14
	movdqa	%xmm14, %xmm1
	movdqa	%xmm14, %xmm2
	pandn	%xmm3, %xmm1
	pand	%xmm14, %xmm3
	movaps	%xmm1, -384(%rbp)
	pandn	-64(%rbp), %xmm2
	pand	-176(%rbp), %xmm11
	por	-288(%rbp), %xmm11
	pand	-96(%rbp), %xmm5
	por	%xmm2, %xmm3
	pand	-80(%rbp), %xmm6
	por	-336(%rbp), %xmm5
	pshufd	$78, %xmm11, %xmm11
	movaps	%xmm3, -256(%rbp)
	movdqa	-64(%rbp), %xmm8
	por	-272(%rbp), %xmm7
	movdqa	%xmm11, %xmm2
	pshufd	$78, %xmm5, %xmm1
	movdqa	-144(%rbp), %xmm5
	pand	-208(%rbp), %xmm10
	pcmpeqd	%xmm13, %xmm2
	movaps	%xmm1, -80(%rbp)
	movdqa	%xmm13, %xmm1
	pand	%xmm14, %xmm8
	psubq	%xmm11, %xmm1
	movdqa	%xmm13, %xmm14
	pshufd	$78, %xmm7, %xmm7
	por	-320(%rbp), %xmm10
	pand	-112(%rbp), %xmm4
	movdqa	%xmm2, %xmm3
	movdqa	%xmm5, %xmm2
	por	-304(%rbp), %xmm6
	pand	%xmm1, %xmm3
	movdqa	%xmm11, %xmm1
	psubq	%xmm7, %xmm2
	pcmpgtd	%xmm13, %xmm1
	pshufd	$78, %xmm10, %xmm10
	pshufd	$78, %xmm6, %xmm6
	pand	-192(%rbp), %xmm9
	por	-368(%rbp), %xmm4
	por	-352(%rbp), %xmm9
	por	-384(%rbp), %xmm8
	por	%xmm1, %xmm3
	pshufd	$78, %xmm4, %xmm4
	pshufd	$245, %xmm3, %xmm3
	pshufd	$78, %xmm9, %xmm9
	pshufd	$78, %xmm8, %xmm8
	movdqa	%xmm3, %xmm1
	pand	%xmm3, %xmm14
	pandn	%xmm11, %xmm1
	por	%xmm1, %xmm14
	movdqa	%xmm3, %xmm1
	pand	%xmm11, %xmm3
	pandn	%xmm13, %xmm1
	movdqa	%xmm7, %xmm11
	por	%xmm1, %xmm3
	pcmpgtd	%xmm5, %xmm11
	movdqa	%xmm7, %xmm1
	pcmpeqd	%xmm5, %xmm1
	pand	%xmm2, %xmm1
	movdqa	%xmm5, %xmm2
	por	%xmm11, %xmm1
	pshufd	$245, %xmm1, %xmm1
	pand	%xmm1, %xmm2
	movdqa	%xmm1, %xmm13
	pandn	%xmm7, %xmm13
	movdqa	%xmm2, %xmm11
	movdqa	%xmm12, %xmm2
	por	%xmm13, %xmm11
	movdqa	%xmm1, %xmm13
	pand	%xmm7, %xmm1
	pandn	%xmm5, %xmm13
	psubq	%xmm10, %xmm2
	movdqa	-160(%rbp), %xmm5
	por	%xmm13, %xmm1
	movdqa	%xmm10, %xmm13
	pcmpeqd	%xmm12, %xmm13
	movdqa	%xmm13, %xmm7
	movdqa	%xmm12, %xmm13
	pand	%xmm2, %xmm7
	movdqa	%xmm10, %xmm2
	pcmpgtd	%xmm12, %xmm2
	por	%xmm2, %xmm7
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm2
	pand	%xmm7, %xmm13
	pandn	%xmm10, %xmm2
	por	%xmm13, %xmm2
	movdqa	%xmm7, %xmm13
	pand	%xmm10, %xmm7
	pandn	%xmm12, %xmm13
	movdqa	%xmm6, %xmm12
	movdqa	%xmm0, %xmm10
	pcmpeqd	%xmm0, %xmm12
	por	%xmm7, %xmm13
	psubq	%xmm6, %xmm10
	movdqa	%xmm12, %xmm7
	movdqa	%xmm0, %xmm12
	pand	%xmm10, %xmm7
	movdqa	%xmm6, %xmm10
	pcmpgtd	%xmm0, %xmm10
	por	%xmm10, %xmm7
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm10
	pand	%xmm7, %xmm12
	pandn	%xmm6, %xmm10
	por	%xmm10, %xmm12
	movdqa	%xmm7, %xmm10
	pand	%xmm6, %xmm7
	pandn	%xmm0, %xmm10
	movdqa	%xmm5, %xmm0
	movdqa	%xmm5, %xmm6
	pcmpeqd	%xmm9, %xmm0
	psubq	%xmm9, %xmm6
	por	%xmm7, %xmm10
	movdqa	%xmm5, %xmm7
	pand	%xmm6, %xmm0
	movdqa	%xmm9, %xmm6
	pcmpgtd	%xmm5, %xmm6
	por	%xmm6, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm6
	pand	%xmm0, %xmm7
	pandn	%xmm9, %xmm6
	por	%xmm6, %xmm7
	movdqa	%xmm0, %xmm6
	pand	%xmm9, %xmm0
	movaps	%xmm7, -160(%rbp)
	movdqa	-128(%rbp), %xmm7
	pandn	%xmm5, %xmm6
	movdqa	-80(%rbp), %xmm5
	movdqa	%xmm0, %xmm9
	movdqa	%xmm7, %xmm0
	por	%xmm6, %xmm9
	movdqa	%xmm7, %xmm6
	pcmpeqd	%xmm5, %xmm0
	psubq	%xmm5, %xmm6
	pand	%xmm6, %xmm0
	movdqa	%xmm5, %xmm6
	pcmpgtd	%xmm7, %xmm6
	por	%xmm6, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm6
	pandn	-80(%rbp), %xmm6
	pand	%xmm0, %xmm7
	movdqa	%xmm0, %xmm5
	pand	-80(%rbp), %xmm0
	pandn	-128(%rbp), %xmm5
	por	%xmm6, %xmm7
	movdqa	%xmm8, %xmm6
	pcmpeqd	%xmm15, %xmm6
	por	%xmm5, %xmm0
	movdqa	%xmm15, %xmm5
	psubq	%xmm8, %xmm5
	movaps	%xmm0, -176(%rbp)
	movdqa	%xmm6, %xmm0
	movdqa	%xmm15, %xmm6
	pand	%xmm5, %xmm0
	movdqa	%xmm8, %xmm5
	pcmpgtd	%xmm15, %xmm5
	por	%xmm5, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm5
	pand	%xmm0, %xmm6
	pandn	%xmm8, %xmm5
	por	%xmm5, %xmm6
	movdqa	%xmm0, %xmm5
	pand	%xmm8, %xmm0
	pandn	%xmm15, %xmm5
	movdqa	%xmm0, %xmm8
	movdqa	-256(%rbp), %xmm15
	movaps	%xmm6, -192(%rbp)
	por	%xmm5, %xmm8
	movdqa	%xmm15, %xmm0
	movdqa	%xmm15, %xmm5
	movdqa	%xmm15, %xmm6
	pcmpeqd	%xmm4, %xmm0
	psubq	%xmm4, %xmm5
	pand	%xmm5, %xmm0
	movdqa	%xmm4, %xmm5
	pcmpgtd	%xmm15, %xmm5
	por	%xmm5, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm5
	pand	%xmm0, %xmm6
	pandn	%xmm4, %xmm5
	por	%xmm5, %xmm6
	movdqa	%xmm0, %xmm5
	pand	%xmm4, %xmm0
	pandn	%xmm15, %xmm5
	pshufd	$78, %xmm14, %xmm4
	movdqa	%xmm0, %xmm15
	movaps	%xmm6, -208(%rbp)
	movdqa	%xmm14, %xmm0
	por	%xmm5, %xmm15
	movdqa	%xmm14, %xmm5
	pcmpeqd	%xmm4, %xmm0
	psubq	%xmm4, %xmm5
	pand	%xmm5, %xmm0
	movdqa	%xmm4, %xmm5
	pcmpgtd	%xmm14, %xmm5
	por	%xmm5, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm5
	pandn	%xmm4, %xmm5
	movdqa	%xmm5, %xmm6
	movdqa	%xmm0, %xmm5
	pandn	%xmm14, %xmm5
	pand	%xmm0, %xmm14
	pand	%xmm4, %xmm0
	por	%xmm5, %xmm0
	pshufd	$78, %xmm3, %xmm4
	por	%xmm6, %xmm14
	movapd	%xmm0, %xmm5
	movdqa	%xmm3, %xmm0
	movsd	%xmm14, %xmm5
	pcmpeqd	%xmm4, %xmm0
	movaps	%xmm5, -112(%rbp)
	movdqa	%xmm3, %xmm5
	psubq	%xmm4, %xmm5
	pand	%xmm5, %xmm0
	movdqa	%xmm4, %xmm5
	pcmpgtd	%xmm3, %xmm5
	por	%xmm5, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm5
	pandn	%xmm4, %xmm5
	movdqa	%xmm5, %xmm6
	movdqa	%xmm0, %xmm5
	pandn	%xmm3, %xmm5
	pand	%xmm0, %xmm3
	pand	%xmm4, %xmm0
	por	%xmm5, %xmm0
	por	%xmm6, %xmm3
	movapd	%xmm0, %xmm5
	movsd	%xmm3, %xmm5
	pshufd	$78, %xmm11, %xmm3
	movaps	%xmm5, -128(%rbp)
	movdqa	%xmm11, %xmm5
	psubq	%xmm3, %xmm5
	movdqa	%xmm5, %xmm4
	movdqa	%xmm11, %xmm5
	pcmpeqd	%xmm3, %xmm5
	movdqa	%xmm5, %xmm0
	movdqa	%xmm3, %xmm5
	pcmpgtd	%xmm11, %xmm5
	pand	%xmm4, %xmm0
	por	%xmm5, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm14
	movdqa	%xmm0, %xmm5
	pandn	%xmm11, %xmm14
	pand	%xmm0, %xmm11
	pand	%xmm3, %xmm0
	pandn	%xmm3, %xmm5
	por	%xmm14, %xmm0
	pshufd	$78, %xmm1, %xmm3
	por	%xmm5, %xmm11
	movapd	%xmm0, %xmm5
	movsd	%xmm11, %xmm5
	movaps	%xmm5, -144(%rbp)
	movdqa	%xmm1, %xmm5
	psubq	%xmm3, %xmm5
	movdqa	%xmm5, %xmm4
	movdqa	%xmm1, %xmm5
	pcmpeqd	%xmm3, %xmm5
	movdqa	%xmm5, %xmm0
	movdqa	%xmm3, %xmm5
	pcmpgtd	%xmm1, %xmm5
	pand	%xmm4, %xmm0
	por	%xmm5, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm14
	movdqa	%xmm0, %xmm5
	pandn	%xmm1, %xmm14
	pand	%xmm0, %xmm1
	pand	%xmm3, %xmm0
	pandn	%xmm3, %xmm5
	por	%xmm14, %xmm0
	movdqa	%xmm2, %xmm3
	por	%xmm5, %xmm1
	movapd	%xmm0, %xmm5
	movdqa	%xmm2, %xmm0
	movsd	%xmm1, %xmm5
	pshufd	$78, %xmm2, %xmm1
	pcmpeqd	%xmm1, %xmm0
	psubq	%xmm1, %xmm3
	pand	%xmm3, %xmm0
	movdqa	%xmm1, %xmm3
	pcmpgtd	%xmm2, %xmm3
	por	%xmm3, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	pandn	%xmm1, %xmm3
	movdqa	%xmm3, %xmm4
	movdqa	%xmm0, %xmm3
	pandn	%xmm2, %xmm3
	pand	%xmm0, %xmm2
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm0
	por	%xmm4, %xmm2
	movdqa	%xmm13, %xmm3
	movdqa	-160(%rbp), %xmm4
	movapd	%xmm0, %xmm1
	movsd	%xmm2, %xmm1
	movdqa	%xmm13, %xmm2
	movaps	%xmm1, -64(%rbp)
	pshufd	$78, %xmm13, %xmm1
	pcmpeqd	%xmm1, %xmm3
	psubq	%xmm1, %xmm2
	movdqa	%xmm3, %xmm0
	pand	%xmm2, %xmm0
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm13, %xmm2
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm2
	pandn	%xmm1, %xmm3
	pandn	%xmm13, %xmm2
	pand	%xmm0, %xmm13
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm13
	pshufd	$78, %xmm12, %xmm1
	por	%xmm2, %xmm0
	movdqa	%xmm12, %xmm3
	pcmpeqd	%xmm1, %xmm3
	movapd	%xmm0, %xmm2
	movsd	%xmm13, %xmm2
	movdqa	-176(%rbp), %xmm13
	movaps	%xmm2, -80(%rbp)
	movdqa	%xmm12, %xmm2
	psubq	%xmm1, %xmm2
	movdqa	%xmm3, %xmm0
	pand	%xmm2, %xmm0
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm12, %xmm2
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm2
	pandn	%xmm1, %xmm3
	pandn	%xmm12, %xmm2
	pand	%xmm0, %xmm12
	pand	%xmm1, %xmm0
	por	%xmm3, %xmm12
	pshufd	$78, %xmm10, %xmm1
	por	%xmm2, %xmm0
	movdqa	%xmm10, %xmm3
	pcmpeqd	%xmm1, %xmm3
	movapd	%xmm0, %xmm2
	movsd	%xmm12, %xmm2
	movaps	%xmm2, -96(%rbp)
	movdqa	%xmm10, %xmm2
	psubq	%xmm1, %xmm2
	movdqa	%xmm3, %xmm0
	pand	%xmm2, %xmm0
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm10, %xmm2
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm3
	pandn	%xmm10, %xmm2
	pand	%xmm0, %xmm10
	pand	%xmm1, %xmm0
	pandn	%xmm1, %xmm3
	por	%xmm2, %xmm0
	pshufd	$78, %xmm4, %xmm1
	por	%xmm3, %xmm10
	movapd	%xmm0, %xmm2
	movdqa	%xmm4, %xmm0
	movsd	%xmm10, %xmm2
	pcmpeqd	%xmm1, %xmm0
	movapd	%xmm2, %xmm14
	movdqa	%xmm4, %xmm2
	psubq	%xmm1, %xmm2
	pand	%xmm2, %xmm0
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm4, %xmm2
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm2
	movdqa	%xmm0, %xmm3
	pandn	%xmm4, %xmm2
	pand	%xmm0, %xmm4
	pand	%xmm1, %xmm0
	pandn	%xmm1, %xmm3
	por	%xmm2, %xmm0
	pshufd	$78, %xmm9, %xmm1
	movdqa	%xmm9, %xmm2
	por	%xmm3, %xmm4
	movdqa	%xmm9, %xmm3
	pcmpeqd	%xmm1, %xmm2
	psubq	%xmm1, %xmm3
	movsd	%xmm4, %xmm0
	pand	%xmm3, %xmm2
	movdqa	%xmm1, %xmm3
	pcmpgtd	%xmm9, %xmm3
	por	%xmm3, %xmm2
	pshufd	$245, %xmm2, %xmm2
	movdqa	%xmm2, %xmm4
	movdqa	%xmm2, %xmm3
	pandn	%xmm1, %xmm4
	pandn	%xmm9, %xmm3
	pand	%xmm2, %xmm9
	por	%xmm4, %xmm9
	pand	%xmm1, %xmm2
	movdqa	%xmm7, %xmm4
	pshufd	$78, %xmm7, %xmm1
	por	%xmm3, %xmm2
	movdqa	%xmm7, %xmm3
	pcmpeqd	%xmm1, %xmm4
	psubq	%xmm1, %xmm3
	movsd	%xmm9, %xmm2
	movdqa	%xmm4, %xmm6
	pand	%xmm3, %xmm6
	movdqa	%xmm1, %xmm3
	pcmpgtd	%xmm7, %xmm3
	por	%xmm3, %xmm6
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm4
	movdqa	%xmm6, %xmm3
	pandn	%xmm1, %xmm4
	pandn	%xmm7, %xmm3
	pand	%xmm6, %xmm7
	por	%xmm4, %xmm7
	pand	%xmm1, %xmm6
	movdqa	%xmm13, %xmm4
	pshufd	$78, %xmm13, %xmm1
	por	%xmm3, %xmm6
	movdqa	%xmm13, %xmm3
	pcmpeqd	%xmm1, %xmm4
	psubq	%xmm1, %xmm3
	movsd	%xmm7, %xmm6
	pand	%xmm3, %xmm4
	movdqa	%xmm1, %xmm3
	pcmpgtd	%xmm13, %xmm3
	por	%xmm3, %xmm4
	pshufd	$245, %xmm4, %xmm4
	movdqa	%xmm4, %xmm3
	movdqa	%xmm4, %xmm7
	pandn	%xmm13, %xmm3
	pand	%xmm4, %xmm13
	pand	%xmm1, %xmm4
	movdqa	%xmm13, %xmm9
	pandn	%xmm1, %xmm7
	por	%xmm3, %xmm4
	movdqa	-192(%rbp), %xmm13
	por	%xmm7, %xmm9
	pshufd	$78, %xmm13, %xmm3
	movdqa	%xmm13, %xmm1
	movdqa	%xmm13, %xmm7
	pcmpeqd	%xmm3, %xmm1
	psubq	%xmm3, %xmm7
	movsd	%xmm9, %xmm4
	pand	%xmm7, %xmm1
	movdqa	%xmm3, %xmm7
	pcmpgtd	%xmm13, %xmm7
	por	%xmm7, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm7
	movdqa	%xmm1, %xmm9
	pandn	%xmm13, %xmm7
	pand	%xmm1, %xmm13
	pand	%xmm3, %xmm1
	pandn	%xmm3, %xmm9
	por	%xmm7, %xmm1
	movdqa	%xmm8, %xmm3
	pshufd	$78, %xmm8, %xmm7
	movdqa	%xmm13, %xmm10
	movdqa	-208(%rbp), %xmm13
	pcmpeqd	%xmm7, %xmm3
	por	%xmm9, %xmm10
	movdqa	%xmm8, %xmm9
	psubq	%xmm7, %xmm9
	movsd	%xmm10, %xmm1
	pand	%xmm9, %xmm3
	movdqa	%xmm7, %xmm9
	pcmpgtd	%xmm8, %xmm9
	por	%xmm9, %xmm3
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm10
	movdqa	%xmm3, %xmm9
	pandn	%xmm7, %xmm10
	pandn	%xmm8, %xmm9
	pand	%xmm3, %xmm8
	pand	%xmm7, %xmm3
	por	%xmm10, %xmm8
	pshufd	$78, %xmm13, %xmm7
	por	%xmm9, %xmm3
	movdqa	%xmm13, %xmm9
	shufpd	$2, %xmm3, %xmm8
	movdqa	%xmm13, %xmm3
	psubq	%xmm7, %xmm9
	pcmpeqd	%xmm7, %xmm3
	pand	%xmm9, %xmm3
	movdqa	%xmm7, %xmm9
	pcmpgtd	%xmm13, %xmm9
	por	%xmm9, %xmm3
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm9
	movdqa	%xmm3, %xmm10
	pandn	%xmm13, %xmm9
	pand	%xmm3, %xmm13
	pand	%xmm7, %xmm3
	por	%xmm9, %xmm3
	pandn	%xmm7, %xmm10
	pshufd	$78, %xmm15, %xmm9
	movapd	%xmm3, %xmm7
	movdqa	%xmm15, %xmm3
	movdqa	%xmm13, %xmm11
	pcmpeqd	%xmm9, %xmm3
	por	%xmm10, %xmm11
	movdqa	%xmm15, %xmm10
	psubq	%xmm9, %xmm10
	movsd	%xmm11, %xmm7
	pand	%xmm10, %xmm3
	movdqa	%xmm9, %xmm10
	pcmpgtd	%xmm15, %xmm10
	por	%xmm10, %xmm3
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm11
	movdqa	%xmm3, %xmm10
	pandn	%xmm15, %xmm10
	pandn	%xmm9, %xmm11
	pand	%xmm3, %xmm15
	pand	%xmm9, %xmm3
	por	%xmm11, %xmm15
	por	%xmm10, %xmm3
	movsd	%xmm15, %xmm3
.L75:
	movdqa	-112(%rbp), %xmm15
	movq	-232(%rbp), %rdx
	movups	%xmm15, (%rdx)
	movdqa	-128(%rbp), %xmm15
	movups	%xmm15, (%r15)
	movdqa	-144(%rbp), %xmm15
	movups	%xmm15, (%r14)
	movups	%xmm5, 0(%r13)
	movdqa	-64(%rbp), %xmm5
	movups	%xmm5, (%r12)
	movdqa	-80(%rbp), %xmm5
	movups	%xmm5, (%rbx)
	movdqa	-96(%rbp), %xmm5
	movq	-224(%rbp), %rbx
	movups	%xmm5, (%r11)
	movups	%xmm14, (%r10)
	movups	%xmm0, (%r9)
	movups	%xmm2, (%r8)
	movups	%xmm6, (%rdi)
	movups	%xmm4, (%rsi)
	movups	%xmm1, (%rcx)
	movq	-216(%rbp), %rcx
	movups	%xmm8, (%rcx)
	movups	%xmm7, (%rbx)
	movups	%xmm3, (%rax)
	addq	$224, %rsp
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
.L77:
	.cfi_restore_state
	movdqa	-160(%rbp), %xmm3
	movdqa	-176(%rbp), %xmm7
	movdqa	%xmm13, %xmm8
	jmp	.L75
	.cfi_endproc
.LFE18782:
	.size	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0, .-_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	.section	.text._ZN3hwy11N_AVX3_ZEN46detail22MaybePartitionTwoValueINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy11N_AVX3_ZEN46detail22MaybePartitionTwoValueINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, @function
_ZN3hwy11N_AVX3_ZEN46detail22MaybePartitionTwoValueINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0:
.LFB18783:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %r10
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	.cfi_offset 3, -24
	cmpq	$7, %rsi
	jbe	.L94
	movl	$8, %r8d
	xorl	%esi, %esi
	jmp	.L85
	.p2align 4,,10
	.p2align 3
.L80:
	vmovdqu64	%zmm0, (%rax)
	kmovb	%k0, %eax
	popcntq	%rax, %rax
	addq	%rax, %rsi
	leaq	8(%r8), %rax
	cmpq	%r10, %rax
	ja	.L105
	movq	%rax, %r8
.L85:
	vmovdqu64	-64(%rdi,%r8,8), %zmm3
	leaq	-8(%r8), %r9
	leaq	(%rdi,%rsi,8), %rax
	vpcmpq	$0, %zmm0, %zmm3, %k0
	vpcmpq	$0, %zmm1, %zmm3, %k1
	kmovb	%k0, %r11d
	kmovb	%k1, %ebx
	korb	%k1, %k0, %k1
	kortestb	%k1, %k1
	jc	.L80
	kmovb	%r11d, %k6
	kmovb	%ebx, %k5
	kxnorb	%k5, %k6, %k7
	kmovb	%k7, %eax
	tzcntl	%eax, %eax
	addq	%r9, %rax
	vpbroadcastq	(%rdi,%rax,8), %zmm0
	leaq	8(%rsi), %rax
	vmovdqa64	%zmm0, (%rdx)
	cmpq	%r9, %rax
	ja	.L81
	.p2align 4,,10
	.p2align 3
.L82:
	vmovdqu64	%zmm1, -64(%rdi,%rax,8)
	movq	%rax, %rsi
	addq	$8, %rax
	cmpq	%rax, %r9
	jnb	.L82
.L81:
	subq	%rsi, %r9
	leaq	(%rdi,%rsi,8), %rdx
	movl	$255, %eax
	cmpq	$255, %r9
	jbe	.L106
.L83:
	kmovb	%eax, %k4
	xorl	%eax, %eax
	vmovdqu64	%zmm1, (%rdx){%k4}
.L78:
	movq	-8(%rbp), %rbx
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L106:
	.cfi_restore_state
	movq	$-1, %rax
	bzhi	%r9, %rax, %rax
	movzbl	%al, %eax
	jmp	.L83
	.p2align 4,,10
	.p2align 3
.L105:
	movq	%r10, %r11
	leaq	(%rdi,%r8,8), %rbx
	leaq	(%rdi,%rsi,8), %r9
	movl	$255, %eax
	subq	%r8, %r11
	kmovd	%eax, %k1
	cmpq	$255, %r11
	jbe	.L79
.L86:
	vmovdqu64	(%rbx), %zmm2{%k1}{z}
	knotb	%k1, %k3
	vmovdqu64	%zmm2, (%rcx){%k1}
	vmovdqa64	(%rcx), %zmm2
	vpcmpq	$0, %zmm0, %zmm2, %k0
	vpcmpq	$0, %zmm1, %zmm2, %k2
	kandb	%k1, %k0, %k0
	korb	%k2, %k0, %k2
	korb	%k3, %k2, %k2
	kortestb	%k2, %k2
	jnc	.L107
	kmovb	%k0, %edx
	popcntq	%rdx, %rdx
	addq	%rsi, %rdx
	vmovdqu64	%zmm0, (%r9){%k1}
	leaq	8(%rdx), %rax
	cmpq	%r10, %rax
	ja	.L91
	.p2align 4,,10
	.p2align 3
.L92:
	vmovdqu64	%zmm1, -64(%rdi,%rax,8)
	movq	%rax, %rdx
	addq	$8, %rax
	cmpq	%rax, %r10
	jnb	.L92
.L91:
	subq	%rdx, %r10
	leaq	(%rdi,%rdx,8), %rcx
	movl	$255, %eax
	cmpq	$255, %r10
	ja	.L93
	movq	$-1, %rax
	bzhi	%r10, %rax, %rax
	movzbl	%al, %eax
.L93:
	kmovb	%eax, %k5
	movl	$1, %eax
	vmovdqu64	%zmm1, (%rcx){%k5}
	movq	-8(%rbp), %rbx
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
.L94:
	.cfi_restore_state
	movq	%rsi, %r11
	movq	%rdi, %r9
	movq	%rdi, %rbx
	xorl	%r8d, %r8d
	xorl	%esi, %esi
.L79:
	movq	$-1, %rax
	bzhi	%r11, %rax, %rax
	movzbl	%al, %eax
	kmovd	%eax, %k1
	jmp	.L86
.L107:
	knotb	%k2, %k3
	kmovb	%k3, %eax
	tzcntl	%eax, %eax
	addq	%r8, %rax
	vpbroadcastq	(%rdi,%rax,8), %zmm0
	leaq	8(%rsi), %rax
	vmovdqa64	%zmm0, (%rdx)
	cmpq	%r8, %rax
	ja	.L88
	.p2align 4,,10
	.p2align 3
.L89:
	vmovdqu64	%zmm1, -64(%rdi,%rax,8)
	movq	%rax, %rsi
	leaq	8(%rax), %rax
	cmpq	%rax, %r8
	jnb	.L89
	leaq	(%rdi,%rsi,8), %r9
.L88:
	subq	%rsi, %r8
	movl	$255, %eax
	cmpq	$255, %r8
	ja	.L90
	movq	$-1, %rax
	bzhi	%r8, %rax, %rax
	movzbl	%al, %eax
.L90:
	kmovb	%eax, %k6
	xorl	%eax, %eax
	vmovdqu64	%zmm1, (%r9){%k6}
	jmp	.L78
	.cfi_endproc
.LFE18783:
	.size	_ZN3hwy11N_AVX3_ZEN46detail22MaybePartitionTwoValueINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, .-_ZN3hwy11N_AVX3_ZEN46detail22MaybePartitionTwoValueINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	.section	.text._ZN3hwy11N_AVX3_ZEN46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy11N_AVX3_ZEN46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0, @function
_ZN3hwy11N_AVX3_ZEN46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0:
.LFB18784:
	.cfi_startproc
	movq	%rsi, %r8
	movq	%rdx, %rcx
	cmpq	%rdx, %rsi
	jbe	.L118
	leaq	(%rdx,%rdx), %rdx
	leaq	1(%rcx), %r9
	leaq	1(%rdx), %rsi
	addq	$2, %rdx
	cmpq	%rsi, %r8
	jbe	.L118
	movq	(%rdi,%rcx,8), %r11
	vpbroadcastq	%r11, %xmm1
	jmp	.L111
	.p2align 4,,10
	.p2align 3
.L121:
	movq	%rsi, %rax
	cmpq	%rdx, %r8
	ja	.L119
.L113:
	cmpq	%rcx, %rax
	je	.L118
	leaq	(%rdi,%rax,8), %rdx
	movq	(%rdx), %rcx
	movq	%rcx, (%r10)
	movq	%r11, (%rdx)
	cmpq	%rax, %r8
	jbe	.L120
	leaq	(%rax,%rax), %rdx
	leaq	1(%rax), %r9
	leaq	1(%rdx), %rsi
	addq	$2, %rdx
	cmpq	%r8, %rsi
	jnb	.L118
	movq	%rax, %rcx
.L111:
	vpbroadcastq	(%rdi,%rsi,8), %xmm0
	leaq	(%rdi,%rcx,8), %r10
	vpcmpq	$6, %xmm1, %xmm0, %k0
	kmovb	%k0, %eax
	testb	$1, %al
	jne	.L121
	cmpq	%rdx, %r8
	jbe	.L118
	salq	$4, %r9
	vpbroadcastq	(%rdi,%r9), %xmm0
	vpcmpq	$6, %xmm1, %xmm0, %k1
	kmovb	%k1, %eax
	testb	$1, %al
	je	.L118
	movq	%rdx, %rax
	jmp	.L113
	.p2align 4,,10
	.p2align 3
.L118:
	ret
	.p2align 4,,10
	.p2align 3
.L119:
	salq	$4, %r9
	vpbroadcastq	(%rdi,%r9), %xmm2
	vpcmpq	$6, %xmm0, %xmm2, %k2
	kmovb	%k2, %esi
	andl	$1, %esi
	cmovne	%rdx, %rax
	jmp	.L113
	.p2align 4,,10
	.p2align 3
.L120:
	ret
	.cfi_endproc
.LFE18784:
	.size	_ZN3hwy11N_AVX3_ZEN46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0, .-_ZN3hwy11N_AVX3_ZEN46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	.section	.text._ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0, @function
_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0:
.LFB18785:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	.cfi_offset 15, -24
	leaq	0(,%rsi,8), %r15
	leaq	(%rdi,%r15), %rdx
	pushq	%r14
	.cfi_offset 14, -32
	leaq	(%rdx,%r15), %r14
	pushq	%r13
	vmovq	%rdx, %xmm18
	.cfi_offset 13, -40
	leaq	(%r14,%r15), %r13
	pushq	%r12
	.cfi_offset 12, -48
	leaq	0(%r13,%r15), %r12
	pushq	%rbx
	.cfi_offset 3, -56
	leaq	(%r12,%r15), %rbx
	leaq	(%rbx,%r15), %r11
	leaq	(%r11,%r15), %r10
	andq	$-64, %rsp
	leaq	(%r10,%r15), %r9
	movq	%rsi, -8(%rsp)
	leaq	(%r9,%r15), %r8
	vmovdqu64	(%rdi), %zmm4
	vmovdqu64	(%r9), %zmm5
	leaq	(%r8,%r15), %rdx
	vpminsq	(%r8), %zmm5, %zmm8
	vmovdqu64	(%r11), %zmm1
	leaq	(%rdx,%r15), %rsi
	vmovq	%rdx, %xmm23
	vpmaxsq	(%r8), %zmm5, %zmm6
	leaq	(%rsi,%r15), %rcx
	leaq	(%rcx,%r15), %rdx
	leaq	(%rdx,%r15), %rax
	addq	%rax, %r15
	movq	%r15, -16(%rsp)
	vmovq	%xmm18, %r15
	vpminsq	(%r15), %zmm4, %zmm13
	vpmaxsq	(%r15), %zmm4, %zmm7
	vmovq	%xmm23, %r15
	vmovdqu64	(%r14), %zmm4
	vmovdqu64	(%r15), %zmm5
	vpminsq	0(%r13), %zmm4, %zmm14
	vpmaxsq	0(%r13), %zmm4, %zmm3
	vpminsq	(%rsi), %zmm5, %zmm9
	vpmaxsq	(%rsi), %zmm5, %zmm15
	vmovdqu64	(%r12), %zmm4
	vmovdqu64	(%rcx), %zmm5
	vpminsq	%zmm14, %zmm13, %zmm10
	vpmaxsq	%zmm14, %zmm13, %zmm13
	vpminsq	(%rbx), %zmm4, %zmm12
	vpmaxsq	(%rbx), %zmm4, %zmm2
	vpminsq	(%rdx), %zmm5, %zmm0
	vpminsq	(%r10), %zmm1, %zmm4
	vpmaxsq	(%rdx), %zmm5, %zmm16
	vpmaxsq	(%r10), %zmm1, %zmm1
	vmovdqu64	(%rax), %zmm5
	movq	-16(%rsp), %r15
	vpminsq	%zmm3, %zmm7, %zmm14
	vpmaxsq	%zmm3, %zmm7, %zmm7
	vpminsq	%zmm4, %zmm12, %zmm3
	vpmaxsq	%zmm4, %zmm12, %zmm12
	cmpq	$1, -8(%rsp)
	vpminsq	(%r15), %zmm5, %zmm11
	vpmaxsq	(%r15), %zmm5, %zmm5
	vpminsq	%zmm1, %zmm2, %zmm4
	vpmaxsq	%zmm1, %zmm2, %zmm2
	vpminsq	%zmm9, %zmm8, %zmm1
	vpmaxsq	%zmm9, %zmm8, %zmm8
	vpminsq	%zmm15, %zmm6, %zmm9
	vpmaxsq	%zmm15, %zmm6, %zmm6
	vpminsq	%zmm11, %zmm0, %zmm15
	vpmaxsq	%zmm11, %zmm0, %zmm0
	vpminsq	%zmm5, %zmm16, %zmm11
	vpmaxsq	%zmm5, %zmm16, %zmm16
	vpminsq	%zmm3, %zmm10, %zmm5
	vpmaxsq	%zmm3, %zmm10, %zmm10
	vpminsq	%zmm4, %zmm14, %zmm3
	vpmaxsq	%zmm4, %zmm14, %zmm14
	vpminsq	%zmm12, %zmm13, %zmm4
	vpmaxsq	%zmm12, %zmm13, %zmm13
	vpminsq	%zmm2, %zmm7, %zmm12
	vpmaxsq	%zmm2, %zmm7, %zmm7
	vpminsq	%zmm15, %zmm1, %zmm2
	vpmaxsq	%zmm15, %zmm1, %zmm1
	vpminsq	%zmm11, %zmm9, %zmm15
	vpmaxsq	%zmm11, %zmm9, %zmm9
	vpminsq	%zmm0, %zmm8, %zmm11
	vpmaxsq	%zmm0, %zmm8, %zmm8
	vpminsq	%zmm16, %zmm6, %zmm0
	vpmaxsq	%zmm16, %zmm6, %zmm6
	vpminsq	%zmm2, %zmm5, %zmm17
	vpmaxsq	%zmm2, %zmm5, %zmm5
	vpminsq	%zmm15, %zmm3, %zmm2
	vpmaxsq	%zmm15, %zmm3, %zmm3
	vpminsq	%zmm11, %zmm4, %zmm15
	vpmaxsq	%zmm11, %zmm4, %zmm4
	vpminsq	%zmm0, %zmm12, %zmm11
	vpmaxsq	%zmm0, %zmm12, %zmm12
	vpminsq	%zmm1, %zmm10, %zmm0
	vpmaxsq	%zmm1, %zmm10, %zmm10
	vpminsq	%zmm9, %zmm14, %zmm1
	vpmaxsq	%zmm9, %zmm14, %zmm14
	vpminsq	%zmm8, %zmm13, %zmm9
	vpmaxsq	%zmm8, %zmm13, %zmm13
	vpminsq	%zmm6, %zmm7, %zmm8
	vpmaxsq	%zmm6, %zmm7, %zmm7
	vpminsq	%zmm4, %zmm1, %zmm6
	vpmaxsq	%zmm4, %zmm1, %zmm1
	vpminsq	%zmm3, %zmm9, %zmm4
	vpmaxsq	%zmm3, %zmm9, %zmm9
	vpminsq	%zmm10, %zmm11, %zmm3
	vpmaxsq	%zmm10, %zmm11, %zmm11
	vpminsq	%zmm12, %zmm8, %zmm10
	vpmaxsq	%zmm12, %zmm8, %zmm8
	vpminsq	%zmm13, %zmm14, %zmm12
	vpmaxsq	%zmm13, %zmm14, %zmm14
	vpminsq	%zmm5, %zmm0, %zmm13
	vpmaxsq	%zmm5, %zmm0, %zmm0
	vpminsq	%zmm15, %zmm2, %zmm5
	vpmaxsq	%zmm15, %zmm2, %zmm2
	vpminsq	%zmm13, %zmm5, %zmm16
	vpmaxsq	%zmm13, %zmm5, %zmm5
	vpminsq	%zmm12, %zmm10, %zmm13
	vpmaxsq	%zmm12, %zmm10, %zmm10
	vpminsq	%zmm0, %zmm2, %zmm12
	vpmaxsq	%zmm0, %zmm2, %zmm2
	vpminsq	%zmm14, %zmm8, %zmm0
	vpminsq	%zmm5, %zmm12, %zmm15
	vpmaxsq	%zmm5, %zmm12, %zmm12
	vpminsq	%zmm4, %zmm6, %zmm5
	vpmaxsq	%zmm4, %zmm6, %zmm6
	vpminsq	%zmm1, %zmm9, %zmm4
	vpmaxsq	%zmm1, %zmm9, %zmm9
	vpminsq	%zmm10, %zmm0, %zmm1
	vpmaxsq	%zmm10, %zmm0, %zmm0
	vpminsq	%zmm2, %zmm3, %zmm10
	vpmaxsq	%zmm2, %zmm3, %zmm3
	vpminsq	%zmm11, %zmm13, %zmm2
	vpmaxsq	%zmm11, %zmm13, %zmm13
	vpminsq	%zmm5, %zmm10, %zmm11
	vpmaxsq	%zmm5, %zmm10, %zmm10
	vpminsq	%zmm3, %zmm6, %zmm5
	vpmaxsq	%zmm3, %zmm6, %zmm6
	vpminsq	%zmm4, %zmm2, %zmm3
	vpmaxsq	%zmm4, %zmm2, %zmm2
	vpminsq	%zmm13, %zmm9, %zmm4
	vpmaxsq	%zmm13, %zmm9, %zmm9
	vpminsq	%zmm5, %zmm10, %zmm13
	vpmaxsq	%zmm5, %zmm10, %zmm10
	vpminsq	%zmm6, %zmm3, %zmm5
	vpmaxsq	%zmm6, %zmm3, %zmm3
	vpminsq	%zmm4, %zmm2, %zmm6
	vpmaxsq	%zmm14, %zmm8, %zmm8
	vpmaxsq	%zmm4, %zmm2, %zmm2
	vpminsq	%zmm12, %zmm11, %zmm14
	vpminsq	%zmm9, %zmm1, %zmm4
	vpmaxsq	%zmm12, %zmm11, %zmm11
	vpmaxsq	%zmm9, %zmm1, %zmm1
	vpminsq	%zmm5, %zmm10, %zmm12
	vpmaxsq	%zmm5, %zmm10, %zmm10
	vpminsq	%zmm6, %zmm3, %zmm5
	vpmaxsq	%zmm6, %zmm3, %zmm3
	jbe	.L123
	vpshufd	$78, %zmm0, %zmm6
	vpshufd	$78, %zmm7, %zmm7
	vpshufd	$78, %zmm5, %zmm5
	movl	$85, %r15d
	vpshufd	$78, %zmm3, %zmm3
	vpshufd	$78, %zmm2, %zmm2
	vpshufd	$78, %zmm4, %zmm4
	kmovb	%r15d, %k1
	vpshufd	$78, %zmm1, %zmm1
	vpshufd	$78, %zmm8, %zmm8
	vpminsq	%zmm7, %zmm17, %zmm0
	cmpq	$3, -8(%rsp)
	vpmaxsq	%zmm7, %zmm17, %zmm9
	vpminsq	%zmm8, %zmm16, %zmm17
	vpminsq	%zmm2, %zmm13, %zmm7
	vpmaxsq	%zmm8, %zmm16, %zmm8
	vpshufd	$78, %zmm9, %zmm9
	vpminsq	%zmm6, %zmm15, %zmm16
	vpmaxsq	%zmm6, %zmm15, %zmm6
	vpshufd	$78, %zmm8, %zmm8
	vpminsq	%zmm1, %zmm14, %zmm15
	vpmaxsq	%zmm1, %zmm14, %zmm1
	vpshufd	$78, %zmm7, %zmm7
	vpminsq	%zmm4, %zmm11, %zmm14
	vpmaxsq	%zmm4, %zmm11, %zmm4
	vpshufd	$78, %zmm1, %zmm1
	vpminsq	%zmm3, %zmm12, %zmm11
	vpmaxsq	%zmm3, %zmm12, %zmm3
	vpshufd	$78, %zmm14, %zmm14
	vpminsq	%zmm5, %zmm10, %zmm12
	vpmaxsq	%zmm5, %zmm10, %zmm5
	vpshufd	$78, %zmm11, %zmm10
	vpshufd	$78, %zmm12, %zmm12
	vpmaxsq	%zmm2, %zmm13, %zmm2
	vpmaxsq	%zmm9, %zmm5, %zmm19
	vpminsq	%zmm12, %zmm0, %zmm11
	vpmaxsq	%zmm12, %zmm0, %zmm13
	vpshufd	$78, %zmm6, %zmm6
	vpminsq	%zmm9, %zmm5, %zmm0
	vpminsq	%zmm14, %zmm15, %zmm12
	vpminsq	%zmm8, %zmm3, %zmm9
	vpminsq	%zmm10, %zmm17, %zmm5
	vpshufd	$78, %zmm12, %zmm12
	vpmaxsq	%zmm10, %zmm17, %zmm10
	vpmaxsq	%zmm8, %zmm3, %zmm17
	vpminsq	%zmm7, %zmm16, %zmm3
	vpminsq	%zmm6, %zmm2, %zmm8
	vpshufd	$78, %zmm10, %zmm10
	vpmaxsq	%zmm6, %zmm2, %zmm6
	vpmaxsq	%zmm14, %zmm15, %zmm14
	vpshufd	$78, %zmm3, %zmm2
	vpminsq	%zmm1, %zmm4, %zmm15
	vpshufd	$78, %zmm17, %zmm3
	vpmaxsq	%zmm1, %zmm4, %zmm4
	vpmaxsq	%zmm12, %zmm11, %zmm17
	vpmaxsq	%zmm7, %zmm16, %zmm7
	vpshufd	$78, %zmm8, %zmm1
	vpshufd	$78, %zmm13, %zmm16
	vpshufd	$78, %zmm15, %zmm8
	vpminsq	%zmm12, %zmm11, %zmm13
	vpshufd	$78, %zmm19, %zmm15
	vpminsq	%zmm2, %zmm5, %zmm11
	vpminsq	%zmm16, %zmm14, %zmm12
	vpminsq	%zmm8, %zmm0, %zmm19
	vpmaxsq	%zmm16, %zmm14, %zmm14
	vpshufd	$78, %zmm11, %zmm11
	vpmaxsq	%zmm8, %zmm0, %zmm16
	vpminsq	%zmm1, %zmm9, %zmm0
	vpminsq	%zmm15, %zmm4, %zmm8
	vpmaxsq	%zmm15, %zmm4, %zmm15
	vpshufd	$78, %zmm0, %zmm4
	vpmaxsq	%zmm2, %zmm5, %zmm5
	vpshufd	$78, %zmm16, %zmm0
	vpminsq	%zmm10, %zmm7, %zmm2
	vpshufd	$78, %zmm15, %zmm16
	vpmaxsq	%zmm10, %zmm7, %zmm7
	vpminsq	%zmm11, %zmm13, %zmm15
	vpshufd	$78, %zmm17, %zmm10
	vpmaxsq	%zmm1, %zmm9, %zmm1
	vpminsq	%zmm3, %zmm6, %zmm9
	vpmaxsq	%zmm3, %zmm6, %zmm6
	vpshufd	$78, %zmm14, %zmm3
	vpmaxsq	%zmm11, %zmm13, %zmm14
	vpminsq	%zmm10, %zmm5, %zmm11
	vpmaxsq	%zmm10, %zmm5, %zmm5
	vpshufd	$78, %zmm15, %zmm10
	vpmaxsq	%zmm10, %zmm15, %zmm17
	vpshufd	$78, %zmm2, %zmm2
	vpshufd	$78, %zmm9, %zmm9
	vpminsq	%zmm10, %zmm15, %zmm17{%k1}
	vpshufd	$78, %zmm14, %zmm10
	vpminsq	%zmm2, %zmm12, %zmm13
	vpmaxsq	%zmm2, %zmm12, %zmm12
	vpminsq	%zmm3, %zmm7, %zmm2
	vpmaxsq	%zmm3, %zmm7, %zmm7
	vpminsq	%zmm4, %zmm19, %zmm3
	vpmaxsq	%zmm4, %zmm19, %zmm19
	vpminsq	%zmm0, %zmm1, %zmm4
	vpmaxsq	%zmm0, %zmm1, %zmm1
	vpminsq	%zmm9, %zmm8, %zmm0
	vpmaxsq	%zmm9, %zmm8, %zmm8
	vpminsq	%zmm16, %zmm6, %zmm9
	vpmaxsq	%zmm16, %zmm6, %zmm6
	vpmaxsq	%zmm10, %zmm14, %zmm16
	vpminsq	%zmm10, %zmm14, %zmm16{%k1}
	vpshufd	$78, %zmm11, %zmm10
	vpmaxsq	%zmm10, %zmm11, %zmm15
	vpminsq	%zmm10, %zmm11, %zmm15{%k1}
	vpshufd	$78, %zmm5, %zmm10
	vpmaxsq	%zmm10, %zmm5, %zmm14
	vpminsq	%zmm10, %zmm5, %zmm14{%k1}
	vpshufd	$78, %zmm13, %zmm5
	vpmaxsq	%zmm5, %zmm13, %zmm11
	vpminsq	%zmm5, %zmm13, %zmm11{%k1}
	vpshufd	$78, %zmm12, %zmm5
	vpmaxsq	%zmm5, %zmm12, %zmm13
	vpminsq	%zmm5, %zmm12, %zmm13{%k1}
	vpshufd	$78, %zmm2, %zmm5
	vpmaxsq	%zmm5, %zmm2, %zmm12
	vpminsq	%zmm5, %zmm2, %zmm12{%k1}
	vpshufd	$78, %zmm7, %zmm2
	vpmaxsq	%zmm2, %zmm7, %zmm10
	vpminsq	%zmm2, %zmm7, %zmm10{%k1}
	vpshufd	$78, %zmm3, %zmm2
	vpshufd	$78, %zmm4, %zmm7
	vpmaxsq	%zmm2, %zmm3, %zmm5
	vpminsq	%zmm2, %zmm3, %zmm5{%k1}
	vpshufd	$78, %zmm19, %zmm2
	vpmaxsq	%zmm2, %zmm19, %zmm3
	vpminsq	%zmm2, %zmm19, %zmm3{%k1}
	vpmaxsq	%zmm7, %zmm4, %zmm2
	vpminsq	%zmm7, %zmm4, %zmm2{%k1}
	vpshufd	$78, %zmm1, %zmm7
	vpmaxsq	%zmm7, %zmm1, %zmm4
	vpminsq	%zmm7, %zmm1, %zmm4{%k1}
	vpshufd	$78, %zmm0, %zmm7
	vpmaxsq	%zmm7, %zmm0, %zmm1
	vpminsq	%zmm7, %zmm0, %zmm1{%k1}
	vpshufd	$78, %zmm8, %zmm7
	vpmaxsq	%zmm7, %zmm8, %zmm0
	vpminsq	%zmm7, %zmm8, %zmm0{%k1}
	vpshufd	$78, %zmm9, %zmm7
	vpmaxsq	%zmm7, %zmm9, %zmm8
	vpminsq	%zmm7, %zmm9, %zmm8{%k1}
	vpshufd	$78, %zmm6, %zmm9
	vpmaxsq	%zmm9, %zmm6, %zmm7
	vpminsq	%zmm9, %zmm6, %zmm7{%k1}
	jbe	.L123
	vpermq	$27, %zmm5, %zmm5
	vpermq	$27, %zmm3, %zmm3
	vpermq	$27, %zmm2, %zmm2
	movl	$51, %r15d
	vpermq	$27, %zmm4, %zmm4
	vpermq	$27, %zmm1, %zmm1
	kmovb	%r15d, %k2
	vpermq	$27, %zmm0, %zmm0
	vpermq	$27, %zmm8, %zmm8
	vpermq	$27, %zmm7, %zmm7
	vpminsq	%zmm2, %zmm13, %zmm6
	cmpq	$7, -8(%rsp)
	vpminsq	%zmm7, %zmm17, %zmm9
	vpmaxsq	%zmm7, %zmm17, %zmm7
	vpermq	$27, %zmm6, %zmm6
	vpminsq	%zmm8, %zmm16, %zmm17
	vpmaxsq	%zmm8, %zmm16, %zmm8
	vpermq	$27, %zmm7, %zmm7
	vpminsq	%zmm0, %zmm15, %zmm16
	vpmaxsq	%zmm0, %zmm15, %zmm0
	vpermq	$27, %zmm8, %zmm8
	vpminsq	%zmm1, %zmm14, %zmm15
	vpmaxsq	%zmm1, %zmm14, %zmm1
	vpermq	$27, %zmm0, %zmm0
	vpminsq	%zmm4, %zmm11, %zmm14
	vpmaxsq	%zmm4, %zmm11, %zmm4
	vpermq	$27, %zmm1, %zmm1
	vpminsq	%zmm3, %zmm12, %zmm11
	vpmaxsq	%zmm3, %zmm12, %zmm3
	vpermq	$27, %zmm14, %zmm14
	vpminsq	%zmm5, %zmm10, %zmm12
	vpmaxsq	%zmm5, %zmm10, %zmm5
	vpermq	$27, %zmm11, %zmm10
	vpermq	$27, %zmm12, %zmm12
	vpminsq	%zmm7, %zmm5, %zmm11
	vpmaxsq	%zmm2, %zmm13, %zmm2
	vpminsq	%zmm12, %zmm9, %zmm19
	vpmaxsq	%zmm12, %zmm9, %zmm13
	vpmaxsq	%zmm7, %zmm5, %zmm20
	vpminsq	%zmm8, %zmm3, %zmm9
	vpminsq	%zmm10, %zmm17, %zmm5
	vpmaxsq	%zmm10, %zmm17, %zmm10
	vpmaxsq	%zmm8, %zmm3, %zmm17
	vpminsq	%zmm6, %zmm16, %zmm3
	vpermq	$27, %zmm10, %zmm10
	vpminsq	%zmm14, %zmm15, %zmm8
	vpminsq	%zmm0, %zmm2, %zmm7
	vpmaxsq	%zmm14, %zmm15, %zmm14
	vpmaxsq	%zmm0, %zmm2, %zmm0
	vpermq	$27, %zmm3, %zmm2
	vpminsq	%zmm1, %zmm4, %zmm15
	vpmaxsq	%zmm1, %zmm4, %zmm12
	vpermq	$27, %zmm8, %zmm3
	vpmaxsq	%zmm6, %zmm16, %zmm6
	vpermq	$27, %zmm13, %zmm1
	vpermq	$27, %zmm15, %zmm16
	vpermq	$27, %zmm17, %zmm8
	vpminsq	%zmm2, %zmm5, %zmm17
	vpminsq	%zmm3, %zmm19, %zmm13
	vpminsq	%zmm16, %zmm11, %zmm4
	vpermq	$27, %zmm7, %zmm7
	vpermq	$27, %zmm20, %zmm15
	vpmaxsq	%zmm3, %zmm19, %zmm19
	vpmaxsq	%zmm16, %zmm11, %zmm20
	vpermq	$27, %zmm17, %zmm11
	vpmaxsq	%zmm2, %zmm5, %zmm3
	vpminsq	%zmm1, %zmm14, %zmm5
	vpmaxsq	%zmm1, %zmm14, %zmm14
	vpminsq	%zmm7, %zmm9, %zmm16
	vpmaxsq	%zmm10, %zmm6, %zmm1
	vpminsq	%zmm10, %zmm6, %zmm2
	vpermq	$27, %zmm19, %zmm10
	vpermq	$27, %zmm14, %zmm6
	vpmaxsq	%zmm7, %zmm9, %zmm9
	vpminsq	%zmm11, %zmm13, %zmm14
	vpminsq	%zmm15, %zmm12, %zmm7
	vpmaxsq	%zmm15, %zmm12, %zmm15
	vpermq	$27, %zmm16, %zmm17
	vpminsq	%zmm8, %zmm0, %zmm12
	vpmaxsq	%zmm11, %zmm13, %zmm13
	vpermq	$27, %zmm2, %zmm2
	vpminsq	%zmm10, %zmm3, %zmm11
	vpermq	$27, %zmm12, %zmm16
	vpmaxsq	%zmm10, %zmm3, %zmm3
	vpmaxsq	%zmm8, %zmm0, %zmm0
	vpermq	$27, %zmm14, %zmm10
	vpermq	$27, %zmm20, %zmm8
	vpminsq	%zmm17, %zmm4, %zmm19
	vpmaxsq	%zmm17, %zmm4, %zmm4
	vpermq	$27, %zmm15, %zmm15
	vpminsq	%zmm8, %zmm9, %zmm17
	vpmaxsq	%zmm8, %zmm9, %zmm9
	vpminsq	%zmm16, %zmm7, %zmm8
	vpmaxsq	%zmm16, %zmm7, %zmm7
	vpmaxsq	%zmm10, %zmm14, %zmm16
	vpminsq	%zmm2, %zmm5, %zmm12
	vpminsq	%zmm10, %zmm14, %zmm16{%k2}
	vpermq	$27, %zmm13, %zmm10
	vpmaxsq	%zmm2, %zmm5, %zmm5
	vpminsq	%zmm6, %zmm1, %zmm2
	vpmaxsq	%zmm6, %zmm1, %zmm1
	vpminsq	%zmm15, %zmm0, %zmm6
	vpmaxsq	%zmm15, %zmm0, %zmm0
	vpmaxsq	%zmm10, %zmm13, %zmm15
	vpminsq	%zmm10, %zmm13, %zmm15{%k2}
	vpermq	$27, %zmm11, %zmm10
	vpmaxsq	%zmm10, %zmm11, %zmm14
	vpminsq	%zmm10, %zmm11, %zmm14{%k2}
	vpermq	$27, %zmm3, %zmm10
	vpmaxsq	%zmm10, %zmm3, %zmm11
	vpminsq	%zmm10, %zmm3, %zmm11{%k2}
	vpermq	$27, %zmm12, %zmm3
	vpmaxsq	%zmm3, %zmm12, %zmm13
	vpminsq	%zmm3, %zmm12, %zmm13{%k2}
	vpermq	$27, %zmm5, %zmm3
	vpmaxsq	%zmm3, %zmm5, %zmm12
	vpminsq	%zmm3, %zmm5, %zmm12{%k2}
	vpermq	$27, %zmm2, %zmm3
	vpmaxsq	%zmm3, %zmm2, %zmm10
	vpminsq	%zmm3, %zmm2, %zmm10{%k2}
	vpermq	$27, %zmm1, %zmm2
	vpmaxsq	%zmm2, %zmm1, %zmm5
	vpminsq	%zmm2, %zmm1, %zmm5{%k2}
	vpermq	$27, %zmm19, %zmm1
	vpmaxsq	%zmm1, %zmm19, %zmm3
	vpminsq	%zmm1, %zmm19, %zmm3{%k2}
	vpermq	$27, %zmm4, %zmm1
	vpmaxsq	%zmm1, %zmm4, %zmm2
	vpminsq	%zmm1, %zmm4, %zmm2{%k2}
	vpermq	$27, %zmm17, %zmm1
	vpmaxsq	%zmm1, %zmm17, %zmm4
	vpminsq	%zmm1, %zmm17, %zmm4{%k2}
	vpermq	$27, %zmm9, %zmm17
	vpmaxsq	%zmm17, %zmm9, %zmm1
	vpminsq	%zmm17, %zmm9, %zmm1{%k2}
	vpermq	$27, %zmm8, %zmm17
	vpmaxsq	%zmm17, %zmm8, %zmm9
	vpminsq	%zmm17, %zmm8, %zmm9{%k2}
	vpermq	$27, %zmm7, %zmm17
	vpmaxsq	%zmm17, %zmm7, %zmm8
	vpminsq	%zmm17, %zmm7, %zmm8{%k2}
	vpermq	$27, %zmm6, %zmm17
	vpmaxsq	%zmm17, %zmm6, %zmm7
	vpminsq	%zmm17, %zmm6, %zmm7{%k2}
	vpermq	$27, %zmm0, %zmm17
	vpmaxsq	%zmm17, %zmm0, %zmm6
	vpminsq	%zmm17, %zmm0, %zmm6{%k2}
	vpshufd	$78, %zmm16, %zmm0
	vpmaxsq	%zmm0, %zmm16, %zmm17
	vpminsq	%zmm0, %zmm16, %zmm17{%k1}
	vpshufd	$78, %zmm15, %zmm0
	vpmaxsq	%zmm0, %zmm15, %zmm16
	vpminsq	%zmm0, %zmm15, %zmm16{%k1}
	vpshufd	$78, %zmm14, %zmm0
	vpmaxsq	%zmm0, %zmm14, %zmm15
	vpminsq	%zmm0, %zmm14, %zmm15{%k1}
	vpshufd	$78, %zmm11, %zmm0
	vpmaxsq	%zmm0, %zmm11, %zmm14
	vpminsq	%zmm0, %zmm11, %zmm14{%k1}
	vpshufd	$78, %zmm13, %zmm0
	vpmaxsq	%zmm0, %zmm13, %zmm11
	vpminsq	%zmm0, %zmm13, %zmm11{%k1}
	vpshufd	$78, %zmm12, %zmm0
	vpmaxsq	%zmm0, %zmm12, %zmm13
	vpminsq	%zmm0, %zmm12, %zmm13{%k1}
	vpshufd	$78, %zmm10, %zmm0
	vpmaxsq	%zmm0, %zmm10, %zmm12
	vpminsq	%zmm0, %zmm10, %zmm12{%k1}
	vpshufd	$78, %zmm5, %zmm0
	vpmaxsq	%zmm0, %zmm5, %zmm10
	vpminsq	%zmm0, %zmm5, %zmm10{%k1}
	vpshufd	$78, %zmm3, %zmm0
	vpmaxsq	%zmm0, %zmm3, %zmm5
	vpminsq	%zmm0, %zmm3, %zmm5{%k1}
	vpshufd	$78, %zmm2, %zmm0
	vpmaxsq	%zmm0, %zmm2, %zmm3
	vpminsq	%zmm0, %zmm2, %zmm3{%k1}
	vpshufd	$78, %zmm4, %zmm0
	vpmaxsq	%zmm0, %zmm4, %zmm2
	vpminsq	%zmm0, %zmm4, %zmm2{%k1}
	vpshufd	$78, %zmm1, %zmm0
	vpmaxsq	%zmm0, %zmm1, %zmm4
	vpminsq	%zmm0, %zmm1, %zmm4{%k1}
	vpshufd	$78, %zmm9, %zmm0
	vpmaxsq	%zmm0, %zmm9, %zmm1
	vpminsq	%zmm0, %zmm9, %zmm1{%k1}
	vpshufd	$78, %zmm8, %zmm9
	vpmaxsq	%zmm9, %zmm8, %zmm0
	vpminsq	%zmm9, %zmm8, %zmm0{%k1}
	vpshufd	$78, %zmm7, %zmm9
	vpmaxsq	%zmm9, %zmm7, %zmm8
	vpminsq	%zmm9, %zmm7, %zmm8{%k1}
	vpshufd	$78, %zmm6, %zmm9
	vpmaxsq	%zmm9, %zmm6, %zmm7
	vpminsq	%zmm9, %zmm6, %zmm7{%k1}
	jbe	.L123
	vmovdqa64	.LC2(%rip), %zmm6
	movl	$65535, %r15d
	kmovd	%r15d, %k3
	vpermq	%zmm7, %zmm6, %zmm7
	vpermq	%zmm5, %zmm6, %zmm5
	vpermq	%zmm3, %zmm6, %zmm3
	vpermq	%zmm2, %zmm6, %zmm2
	vpermq	%zmm4, %zmm6, %zmm4
	vpermq	%zmm1, %zmm6, %zmm1
	vpermq	%zmm0, %zmm6, %zmm0
	vpermq	%zmm8, %zmm6, %zmm8
	vpminsq	%zmm7, %zmm17, %zmm9
	vpmaxsq	%zmm7, %zmm17, %zmm19
	vpminsq	%zmm8, %zmm16, %zmm7
	vpmaxsq	%zmm8, %zmm16, %zmm8
	vpminsq	%zmm0, %zmm15, %zmm16
	vpmaxsq	%zmm0, %zmm15, %zmm0
	vpminsq	%zmm1, %zmm14, %zmm15
	vpermq	%zmm8, %zmm6, %zmm8
	vpmaxsq	%zmm1, %zmm14, %zmm1
	vpminsq	%zmm4, %zmm11, %zmm14
	vpermq	%zmm0, %zmm6, %zmm0
	vpmaxsq	%zmm4, %zmm11, %zmm4
	vpminsq	%zmm2, %zmm13, %zmm11
	vpermq	%zmm14, %zmm6, %zmm14
	vpmaxsq	%zmm2, %zmm13, %zmm2
	vpminsq	%zmm3, %zmm12, %zmm13
	vpermq	%zmm11, %zmm6, %zmm11
	vpmaxsq	%zmm3, %zmm12, %zmm3
	vpminsq	%zmm5, %zmm10, %zmm12
	vpermq	%zmm13, %zmm6, %zmm17
	vpmaxsq	%zmm5, %zmm10, %zmm5
	vpermq	%zmm19, %zmm6, %zmm13
	vpermq	%zmm12, %zmm6, %zmm12
	vpminsq	%zmm12, %zmm9, %zmm10
	vpmaxsq	%zmm12, %zmm9, %zmm19
	vpermq	%zmm1, %zmm6, %zmm1
	vpminsq	%zmm13, %zmm5, %zmm12
	vpmaxsq	%zmm13, %zmm5, %zmm20
	vpminsq	%zmm8, %zmm3, %zmm9
	vpminsq	%zmm17, %zmm7, %zmm13
	vpminsq	%zmm11, %zmm16, %zmm5
	vpmaxsq	%zmm17, %zmm7, %zmm7
	vpmaxsq	%zmm8, %zmm3, %zmm17
	vpmaxsq	%zmm11, %zmm16, %zmm3
	vpermq	%zmm19, %zmm6, %zmm11
	vpminsq	%zmm14, %zmm15, %zmm16
	vpminsq	%zmm0, %zmm2, %zmm8
	vpmaxsq	%zmm14, %zmm15, %zmm14
	vpermq	%zmm16, %zmm6, %zmm16
	vpermq	%zmm5, %zmm6, %zmm15
	vpmaxsq	%zmm0, %zmm2, %zmm0
	vpminsq	%zmm1, %zmm4, %zmm2
	vpermq	%zmm20, %zmm6, %zmm5
	vpminsq	%zmm16, %zmm10, %zmm19
	vpmaxsq	%zmm1, %zmm4, %zmm1
	vpermq	%zmm7, %zmm6, %zmm4
	vpermq	%zmm8, %zmm6, %zmm7
	vpermq	%zmm2, %zmm6, %zmm8
	vpermq	%zmm17, %zmm6, %zmm2
	vpmaxsq	%zmm16, %zmm10, %zmm17
	vpminsq	%zmm15, %zmm13, %zmm16
	vpminsq	%zmm11, %zmm14, %zmm10
	vpmaxsq	%zmm15, %zmm13, %zmm13
	vpmaxsq	%zmm8, %zmm12, %zmm20
	vpmaxsq	%zmm11, %zmm14, %zmm15
	vpermq	%zmm16, %zmm6, %zmm11
	vpminsq	%zmm4, %zmm3, %zmm14
	vpmaxsq	%zmm5, %zmm1, %zmm22
	vpmaxsq	%zmm4, %zmm3, %zmm3
	vpminsq	%zmm2, %zmm0, %zmm21
	vpermq	%zmm22, %zmm6, %zmm16
	vpminsq	%zmm8, %zmm12, %zmm4
	vpminsq	%zmm7, %zmm9, %zmm8
	vpermq	%zmm17, %zmm6, %zmm12
	vpmaxsq	%zmm7, %zmm9, %zmm9
	vpermq	%zmm21, %zmm6, %zmm17
	vpminsq	%zmm5, %zmm1, %zmm7
	vpmaxsq	%zmm2, %zmm0, %zmm0
	vpermq	%zmm14, %zmm6, %zmm5
	vpermq	%zmm15, %zmm6, %zmm2
	vpermq	%zmm8, %zmm6, %zmm1
	vpminsq	%zmm11, %zmm19, %zmm15
	vpermq	%zmm20, %zmm6, %zmm8
	vpmaxsq	%zmm11, %zmm19, %zmm14
	vpminsq	%zmm12, %zmm13, %zmm11
	vpmaxsq	%zmm12, %zmm13, %zmm13
	vpminsq	%zmm5, %zmm10, %zmm12
	vpmaxsq	%zmm5, %zmm10, %zmm10
	vpminsq	%zmm2, %zmm3, %zmm5
	vpmaxsq	%zmm2, %zmm3, %zmm3
	vpminsq	%zmm1, %zmm4, %zmm2
	vpmaxsq	%zmm1, %zmm4, %zmm4
	vpminsq	%zmm8, %zmm9, %zmm1
	vpmaxsq	%zmm8, %zmm9, %zmm9
	vpminsq	%zmm17, %zmm7, %zmm8
	vpmaxsq	%zmm17, %zmm7, %zmm7
	vpminsq	%zmm16, %zmm0, %zmm17
	vpmaxsq	%zmm16, %zmm0, %zmm0
	vpermq	%zmm15, %zmm6, %zmm16
	vpminsq	%zmm16, %zmm15, %zmm19
	vpmaxsq	%zmm16, %zmm15, %zmm15
	vpermq	%zmm14, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm15{%k3}
	vpminsq	%zmm16, %zmm14, %zmm19
	vpmaxsq	%zmm16, %zmm14, %zmm14
	vpermq	%zmm11, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm14{%k3}
	vpminsq	%zmm16, %zmm11, %zmm19
	vpmaxsq	%zmm16, %zmm11, %zmm11
	vpermq	%zmm13, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm11{%k3}
	vpminsq	%zmm16, %zmm13, %zmm19
	vpmaxsq	%zmm16, %zmm13, %zmm13
	vpermq	%zmm12, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm13{%k3}
	vpminsq	%zmm16, %zmm12, %zmm19
	vpmaxsq	%zmm16, %zmm12, %zmm12
	vpermq	%zmm10, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm12{%k3}
	vpminsq	%zmm16, %zmm10, %zmm19
	vpmaxsq	%zmm16, %zmm10, %zmm10
	vpermq	%zmm5, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm10{%k3}
	vpminsq	%zmm16, %zmm5, %zmm19
	vpmaxsq	%zmm16, %zmm5, %zmm5
	vpermq	%zmm3, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm5{%k3}
	vpminsq	%zmm16, %zmm3, %zmm19
	vpmaxsq	%zmm16, %zmm3, %zmm3
	vpermq	%zmm2, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm3{%k3}
	vpminsq	%zmm16, %zmm2, %zmm19
	vpmaxsq	%zmm16, %zmm2, %zmm2
	vpermq	%zmm4, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm2{%k3}
	vpminsq	%zmm16, %zmm4, %zmm19
	vpmaxsq	%zmm16, %zmm4, %zmm4
	vpermq	%zmm1, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm4{%k3}
	vpminsq	%zmm16, %zmm1, %zmm19
	vpmaxsq	%zmm16, %zmm1, %zmm1
	vpermq	%zmm9, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm1{%k3}
	vpminsq	%zmm16, %zmm9, %zmm19
	vpmaxsq	%zmm16, %zmm9, %zmm9
	vpermq	%zmm8, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm9{%k3}
	vpminsq	%zmm16, %zmm8, %zmm19
	vpmaxsq	%zmm16, %zmm8, %zmm8
	vpermq	%zmm7, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm8{%k3}
	vpminsq	%zmm16, %zmm7, %zmm19
	vpmaxsq	%zmm16, %zmm7, %zmm7
	vpermq	%zmm17, %zmm6, %zmm16
	vpermq	%zmm0, %zmm6, %zmm6
	vmovdqu16	%zmm19, %zmm7{%k3}
	vpminsq	%zmm16, %zmm17, %zmm19
	vpmaxsq	%zmm16, %zmm17, %zmm17
	vpminsq	%zmm6, %zmm0, %zmm16
	vpmaxsq	%zmm6, %zmm0, %zmm0
	vshufi32x4	$177, %zmm15, %zmm15, %zmm6
	vmovdqu16	%zmm16, %zmm0{%k3}
	vpmaxsq	%zmm15, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm17{%k3}
	vpminsq	%zmm15, %zmm6, %zmm16{%k2}
	vshufi32x4	$177, %zmm14, %zmm14, %zmm6
	vpmaxsq	%zmm14, %zmm6, %zmm15
	vpminsq	%zmm14, %zmm6, %zmm15{%k2}
	vshufi32x4	$177, %zmm11, %zmm11, %zmm6
	vpmaxsq	%zmm11, %zmm6, %zmm14
	vpminsq	%zmm11, %zmm6, %zmm14{%k2}
	vshufi32x4	$177, %zmm13, %zmm13, %zmm6
	vpmaxsq	%zmm13, %zmm6, %zmm11
	vpminsq	%zmm13, %zmm6, %zmm11{%k2}
	vshufi32x4	$177, %zmm12, %zmm12, %zmm6
	vpmaxsq	%zmm12, %zmm6, %zmm13
	vpminsq	%zmm12, %zmm6, %zmm13{%k2}
	vshufi32x4	$177, %zmm10, %zmm10, %zmm6
	vpmaxsq	%zmm10, %zmm6, %zmm12
	vpminsq	%zmm10, %zmm6, %zmm12{%k2}
	vshufi32x4	$177, %zmm5, %zmm5, %zmm6
	vpmaxsq	%zmm5, %zmm6, %zmm10
	vpminsq	%zmm5, %zmm6, %zmm10{%k2}
	vshufi32x4	$177, %zmm3, %zmm3, %zmm6
	vpmaxsq	%zmm3, %zmm6, %zmm5
	vpminsq	%zmm3, %zmm6, %zmm5{%k2}
	vshufi32x4	$177, %zmm2, %zmm2, %zmm6
	vpmaxsq	%zmm2, %zmm6, %zmm3
	vpminsq	%zmm2, %zmm6, %zmm3{%k2}
	vshufi32x4	$177, %zmm4, %zmm4, %zmm6
	vpmaxsq	%zmm4, %zmm6, %zmm2
	vpminsq	%zmm4, %zmm6, %zmm2{%k2}
	vshufi32x4	$177, %zmm1, %zmm1, %zmm6
	vpmaxsq	%zmm1, %zmm6, %zmm4
	vpminsq	%zmm1, %zmm6, %zmm4{%k2}
	vshufi32x4	$177, %zmm9, %zmm9, %zmm6
	vpmaxsq	%zmm9, %zmm6, %zmm1
	vpminsq	%zmm9, %zmm6, %zmm1{%k2}
	vshufi32x4	$177, %zmm8, %zmm8, %zmm6
	vpmaxsq	%zmm8, %zmm6, %zmm9
	vpminsq	%zmm8, %zmm6, %zmm9{%k2}
	vshufi32x4	$177, %zmm7, %zmm7, %zmm6
	vpmaxsq	%zmm7, %zmm6, %zmm8
	vpminsq	%zmm7, %zmm6, %zmm8{%k2}
	vshufi32x4	$177, %zmm17, %zmm17, %zmm6
	vpmaxsq	%zmm17, %zmm6, %zmm7
	vpminsq	%zmm17, %zmm6, %zmm7{%k2}
	vshufi32x4	$177, %zmm0, %zmm0, %zmm17
	vpmaxsq	%zmm0, %zmm17, %zmm6
	vpminsq	%zmm0, %zmm17, %zmm6{%k2}
	vpshufd	$78, %zmm16, %zmm0
	vpmaxsq	%zmm0, %zmm16, %zmm17
	vpminsq	%zmm0, %zmm16, %zmm17{%k1}
	vpshufd	$78, %zmm15, %zmm0
	vpmaxsq	%zmm0, %zmm15, %zmm16
	vpminsq	%zmm0, %zmm15, %zmm16{%k1}
	vpshufd	$78, %zmm14, %zmm0
	vpmaxsq	%zmm0, %zmm14, %zmm15
	vpminsq	%zmm0, %zmm14, %zmm15{%k1}
	vpshufd	$78, %zmm11, %zmm0
	vpmaxsq	%zmm0, %zmm11, %zmm14
	vpminsq	%zmm0, %zmm11, %zmm14{%k1}
	vpshufd	$78, %zmm13, %zmm0
	vpmaxsq	%zmm0, %zmm13, %zmm11
	vpminsq	%zmm0, %zmm13, %zmm11{%k1}
	vpshufd	$78, %zmm12, %zmm0
	vpmaxsq	%zmm0, %zmm12, %zmm13
	vpminsq	%zmm0, %zmm12, %zmm13{%k1}
	vpshufd	$78, %zmm10, %zmm0
	vpmaxsq	%zmm0, %zmm10, %zmm12
	vpminsq	%zmm0, %zmm10, %zmm12{%k1}
	vpshufd	$78, %zmm5, %zmm0
	vpmaxsq	%zmm0, %zmm5, %zmm10
	vpminsq	%zmm0, %zmm5, %zmm10{%k1}
	vpshufd	$78, %zmm3, %zmm0
	vpmaxsq	%zmm0, %zmm3, %zmm5
	vpminsq	%zmm0, %zmm3, %zmm5{%k1}
	vpshufd	$78, %zmm2, %zmm0
	vpmaxsq	%zmm0, %zmm2, %zmm3
	vpminsq	%zmm0, %zmm2, %zmm3{%k1}
	vpshufd	$78, %zmm4, %zmm0
	vpmaxsq	%zmm0, %zmm4, %zmm2
	vpminsq	%zmm0, %zmm4, %zmm2{%k1}
	vpshufd	$78, %zmm1, %zmm0
	vpmaxsq	%zmm0, %zmm1, %zmm4
	vpminsq	%zmm0, %zmm1, %zmm4{%k1}
	vpshufd	$78, %zmm9, %zmm0
	vpmaxsq	%zmm0, %zmm9, %zmm1
	vpminsq	%zmm0, %zmm9, %zmm1{%k1}
	vpshufd	$78, %zmm8, %zmm9
	vpmaxsq	%zmm9, %zmm8, %zmm0
	vpminsq	%zmm9, %zmm8, %zmm0{%k1}
	vpshufd	$78, %zmm7, %zmm9
	vpmaxsq	%zmm9, %zmm7, %zmm8
	vpminsq	%zmm9, %zmm7, %zmm8{%k1}
	vpshufd	$78, %zmm6, %zmm9
	vpmaxsq	%zmm9, %zmm6, %zmm7
	vpminsq	%zmm9, %zmm6, %zmm7{%k1}
.L123:
	vmovdqu64	%zmm17, (%rdi)
	vmovq	%xmm18, %rdi
	vmovdqu64	%zmm16, (%rdi)
	vmovdqu64	%zmm15, (%r14)
	vmovdqu64	%zmm14, 0(%r13)
	vmovdqu64	%zmm11, (%r12)
	vmovdqu64	%zmm13, (%rbx)
	vmovq	%xmm23, %rbx
	vmovdqu64	%zmm12, (%r11)
	vmovdqu64	%zmm10, (%r10)
	vmovdqu64	%zmm5, (%r9)
	vmovdqu64	%zmm3, (%r8)
	vmovdqu64	%zmm2, (%rbx)
	vmovdqu64	%zmm4, (%rsi)
	vmovdqu64	%zmm1, (%rcx)
	vmovdqu64	%zmm0, (%rdx)
	vmovdqu64	%zmm8, (%rax)
	movq	-16(%rsp), %rax
	vmovdqu64	%zmm7, (%rax)
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
.LFE18785:
	.size	_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0, .-_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	.section	.text._ZN3hwy6N_AVX36detail22MaybePartitionTwoValueINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX36detail22MaybePartitionTwoValueINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, @function
_ZN3hwy6N_AVX36detail22MaybePartitionTwoValueINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0:
.LFB18786:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %r10
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	.cfi_offset 3, -24
	cmpq	$7, %rsi
	jbe	.L141
	movl	$8, %r8d
	xorl	%esi, %esi
	jmp	.L132
	.p2align 4,,10
	.p2align 3
.L127:
	vmovdqu64	%zmm0, (%rax)
	kmovb	%k0, %eax
	popcntq	%rax, %rax
	addq	%rax, %rsi
	leaq	8(%r8), %rax
	cmpq	%r10, %rax
	ja	.L152
	movq	%rax, %r8
.L132:
	vmovdqu64	-64(%rdi,%r8,8), %zmm3
	leaq	-8(%r8), %r9
	leaq	(%rdi,%rsi,8), %rax
	vpcmpq	$0, %zmm0, %zmm3, %k0
	vpcmpq	$0, %zmm1, %zmm3, %k1
	kmovb	%k0, %r11d
	kmovb	%k1, %ebx
	korb	%k1, %k0, %k1
	kortestb	%k1, %k1
	jc	.L127
	kmovb	%r11d, %k6
	kmovb	%ebx, %k5
	kxnorb	%k5, %k6, %k7
	kmovb	%k7, %eax
	tzcntl	%eax, %eax
	addq	%r9, %rax
	vpbroadcastq	(%rdi,%rax,8), %zmm0
	leaq	8(%rsi), %rax
	vmovdqa64	%zmm0, (%rdx)
	cmpq	%r9, %rax
	ja	.L128
	.p2align 4,,10
	.p2align 3
.L129:
	vmovdqu64	%zmm1, -64(%rdi,%rax,8)
	movq	%rax, %rsi
	addq	$8, %rax
	cmpq	%rax, %r9
	jnb	.L129
.L128:
	subq	%rsi, %r9
	leaq	(%rdi,%rsi,8), %rdx
	movl	$255, %eax
	cmpq	$255, %r9
	jbe	.L153
.L130:
	kmovb	%eax, %k4
	xorl	%eax, %eax
	vmovdqu64	%zmm1, (%rdx){%k4}
.L125:
	movq	-8(%rbp), %rbx
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L153:
	.cfi_restore_state
	movq	$-1, %rax
	bzhi	%r9, %rax, %rax
	movzbl	%al, %eax
	jmp	.L130
	.p2align 4,,10
	.p2align 3
.L152:
	movq	%r10, %r11
	leaq	(%rdi,%r8,8), %rbx
	leaq	(%rdi,%rsi,8), %r9
	movl	$255, %eax
	subq	%r8, %r11
	kmovd	%eax, %k1
	cmpq	$255, %r11
	jbe	.L126
.L133:
	vmovdqu64	(%rbx), %zmm2{%k1}{z}
	knotb	%k1, %k3
	vmovdqu64	%zmm2, (%rcx){%k1}
	vmovdqa64	(%rcx), %zmm2
	vpcmpq	$0, %zmm0, %zmm2, %k0
	vpcmpq	$0, %zmm1, %zmm2, %k2
	kandb	%k1, %k0, %k0
	korb	%k2, %k0, %k2
	korb	%k3, %k2, %k2
	kortestb	%k2, %k2
	jnc	.L154
	kmovb	%k0, %edx
	popcntq	%rdx, %rdx
	addq	%rsi, %rdx
	vmovdqu64	%zmm0, (%r9){%k1}
	leaq	8(%rdx), %rax
	cmpq	%r10, %rax
	ja	.L138
	.p2align 4,,10
	.p2align 3
.L139:
	vmovdqu64	%zmm1, -64(%rdi,%rax,8)
	movq	%rax, %rdx
	addq	$8, %rax
	cmpq	%rax, %r10
	jnb	.L139
.L138:
	subq	%rdx, %r10
	leaq	(%rdi,%rdx,8), %rcx
	movl	$255, %eax
	cmpq	$255, %r10
	ja	.L140
	movq	$-1, %rax
	bzhi	%r10, %rax, %rax
	movzbl	%al, %eax
.L140:
	kmovb	%eax, %k5
	movl	$1, %eax
	vmovdqu64	%zmm1, (%rcx){%k5}
	movq	-8(%rbp), %rbx
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
.L141:
	.cfi_restore_state
	movq	%rsi, %r11
	movq	%rdi, %r9
	movq	%rdi, %rbx
	xorl	%r8d, %r8d
	xorl	%esi, %esi
.L126:
	movq	$-1, %rax
	bzhi	%r11, %rax, %rax
	movzbl	%al, %eax
	kmovd	%eax, %k1
	jmp	.L133
.L154:
	knotb	%k2, %k3
	kmovb	%k3, %eax
	tzcntl	%eax, %eax
	addq	%r8, %rax
	vpbroadcastq	(%rdi,%rax,8), %zmm0
	leaq	8(%rsi), %rax
	vmovdqa64	%zmm0, (%rdx)
	cmpq	%r8, %rax
	ja	.L135
	.p2align 4,,10
	.p2align 3
.L136:
	vmovdqu64	%zmm1, -64(%rdi,%rax,8)
	movq	%rax, %rsi
	leaq	8(%rax), %rax
	cmpq	%rax, %r8
	jnb	.L136
	leaq	(%rdi,%rsi,8), %r9
.L135:
	subq	%rsi, %r8
	movl	$255, %eax
	cmpq	$255, %r8
	ja	.L137
	movq	$-1, %rax
	bzhi	%r8, %rax, %rax
	movzbl	%al, %eax
.L137:
	kmovb	%eax, %k6
	xorl	%eax, %eax
	vmovdqu64	%zmm1, (%r9){%k6}
	jmp	.L125
	.cfi_endproc
.LFE18786:
	.size	_ZN3hwy6N_AVX36detail22MaybePartitionTwoValueINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, .-_ZN3hwy6N_AVX36detail22MaybePartitionTwoValueINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	.section	.text._ZN3hwy6N_AVX36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0, @function
_ZN3hwy6N_AVX36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0:
.LFB18787:
	.cfi_startproc
	movq	%rsi, %r8
	movq	%rdx, %rcx
	cmpq	%rdx, %rsi
	jbe	.L165
	leaq	(%rdx,%rdx), %rdx
	leaq	1(%rcx), %r9
	leaq	1(%rdx), %rsi
	addq	$2, %rdx
	cmpq	%rsi, %r8
	jbe	.L165
	movq	(%rdi,%rcx,8), %r11
	vpbroadcastq	%r11, %xmm1
	jmp	.L158
	.p2align 4,,10
	.p2align 3
.L168:
	movq	%rsi, %rax
	cmpq	%rdx, %r8
	ja	.L166
.L160:
	cmpq	%rcx, %rax
	je	.L165
	leaq	(%rdi,%rax,8), %rdx
	movq	(%rdx), %rcx
	movq	%rcx, (%r10)
	movq	%r11, (%rdx)
	cmpq	%rax, %r8
	jbe	.L167
	leaq	(%rax,%rax), %rdx
	leaq	1(%rax), %r9
	leaq	1(%rdx), %rsi
	addq	$2, %rdx
	cmpq	%r8, %rsi
	jnb	.L165
	movq	%rax, %rcx
.L158:
	vpbroadcastq	(%rdi,%rsi,8), %xmm0
	leaq	(%rdi,%rcx,8), %r10
	vpcmpq	$6, %xmm1, %xmm0, %k0
	kmovb	%k0, %eax
	testb	$1, %al
	jne	.L168
	cmpq	%rdx, %r8
	jbe	.L165
	salq	$4, %r9
	vpbroadcastq	(%rdi,%r9), %xmm0
	vpcmpq	$6, %xmm1, %xmm0, %k1
	kmovb	%k1, %eax
	testb	$1, %al
	je	.L165
	movq	%rdx, %rax
	jmp	.L160
	.p2align 4,,10
	.p2align 3
.L165:
	ret
	.p2align 4,,10
	.p2align 3
.L166:
	salq	$4, %r9
	vpbroadcastq	(%rdi,%r9), %xmm2
	vpcmpq	$6, %xmm0, %xmm2, %k2
	kmovb	%k2, %esi
	andl	$1, %esi
	cmovne	%rdx, %rax
	jmp	.L160
	.p2align 4,,10
	.p2align 3
.L167:
	ret
	.cfi_endproc
.LFE18787:
	.size	_ZN3hwy6N_AVX36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0, .-_ZN3hwy6N_AVX36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	.section	.text._ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0, @function
_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0:
.LFB18788:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	.cfi_offset 15, -24
	leaq	0(,%rsi,8), %r15
	leaq	(%rdi,%r15), %rdx
	pushq	%r14
	.cfi_offset 14, -32
	leaq	(%rdx,%r15), %r14
	pushq	%r13
	vmovq	%rdx, %xmm18
	.cfi_offset 13, -40
	leaq	(%r14,%r15), %r13
	pushq	%r12
	.cfi_offset 12, -48
	leaq	0(%r13,%r15), %r12
	pushq	%rbx
	.cfi_offset 3, -56
	leaq	(%r12,%r15), %rbx
	leaq	(%rbx,%r15), %r11
	leaq	(%r11,%r15), %r10
	andq	$-64, %rsp
	leaq	(%r10,%r15), %r9
	movq	%rsi, -8(%rsp)
	leaq	(%r9,%r15), %r8
	vmovdqu64	(%rdi), %zmm4
	vmovdqu64	(%r9), %zmm5
	leaq	(%r8,%r15), %rdx
	vpminsq	(%r8), %zmm5, %zmm8
	vmovdqu64	(%r11), %zmm1
	leaq	(%rdx,%r15), %rsi
	vmovq	%rdx, %xmm23
	vpmaxsq	(%r8), %zmm5, %zmm6
	leaq	(%rsi,%r15), %rcx
	leaq	(%rcx,%r15), %rdx
	leaq	(%rdx,%r15), %rax
	addq	%rax, %r15
	movq	%r15, -16(%rsp)
	vmovq	%xmm18, %r15
	vpminsq	(%r15), %zmm4, %zmm13
	vpmaxsq	(%r15), %zmm4, %zmm7
	vmovq	%xmm23, %r15
	vmovdqu64	(%r14), %zmm4
	vmovdqu64	(%r15), %zmm5
	vpminsq	0(%r13), %zmm4, %zmm14
	vpmaxsq	0(%r13), %zmm4, %zmm3
	vpminsq	(%rsi), %zmm5, %zmm9
	vpmaxsq	(%rsi), %zmm5, %zmm15
	vmovdqu64	(%r12), %zmm4
	vmovdqu64	(%rcx), %zmm5
	vpminsq	%zmm14, %zmm13, %zmm10
	vpmaxsq	%zmm14, %zmm13, %zmm13
	vpminsq	(%rbx), %zmm4, %zmm12
	vpmaxsq	(%rbx), %zmm4, %zmm2
	vpminsq	(%rdx), %zmm5, %zmm0
	vpminsq	(%r10), %zmm1, %zmm4
	vpmaxsq	(%rdx), %zmm5, %zmm16
	vpmaxsq	(%r10), %zmm1, %zmm1
	vmovdqu64	(%rax), %zmm5
	movq	-16(%rsp), %r15
	vpminsq	%zmm3, %zmm7, %zmm14
	vpmaxsq	%zmm3, %zmm7, %zmm7
	vpminsq	%zmm4, %zmm12, %zmm3
	vpmaxsq	%zmm4, %zmm12, %zmm12
	cmpq	$1, -8(%rsp)
	vpminsq	(%r15), %zmm5, %zmm11
	vpmaxsq	(%r15), %zmm5, %zmm5
	vpminsq	%zmm1, %zmm2, %zmm4
	vpmaxsq	%zmm1, %zmm2, %zmm2
	vpminsq	%zmm9, %zmm8, %zmm1
	vpmaxsq	%zmm9, %zmm8, %zmm8
	vpminsq	%zmm15, %zmm6, %zmm9
	vpmaxsq	%zmm15, %zmm6, %zmm6
	vpminsq	%zmm11, %zmm0, %zmm15
	vpmaxsq	%zmm11, %zmm0, %zmm0
	vpminsq	%zmm5, %zmm16, %zmm11
	vpmaxsq	%zmm5, %zmm16, %zmm16
	vpminsq	%zmm3, %zmm10, %zmm5
	vpmaxsq	%zmm3, %zmm10, %zmm10
	vpminsq	%zmm4, %zmm14, %zmm3
	vpmaxsq	%zmm4, %zmm14, %zmm14
	vpminsq	%zmm12, %zmm13, %zmm4
	vpmaxsq	%zmm12, %zmm13, %zmm13
	vpminsq	%zmm2, %zmm7, %zmm12
	vpmaxsq	%zmm2, %zmm7, %zmm7
	vpminsq	%zmm15, %zmm1, %zmm2
	vpmaxsq	%zmm15, %zmm1, %zmm1
	vpminsq	%zmm11, %zmm9, %zmm15
	vpmaxsq	%zmm11, %zmm9, %zmm9
	vpminsq	%zmm0, %zmm8, %zmm11
	vpmaxsq	%zmm0, %zmm8, %zmm8
	vpminsq	%zmm16, %zmm6, %zmm0
	vpmaxsq	%zmm16, %zmm6, %zmm6
	vpminsq	%zmm2, %zmm5, %zmm17
	vpmaxsq	%zmm2, %zmm5, %zmm5
	vpminsq	%zmm15, %zmm3, %zmm2
	vpmaxsq	%zmm15, %zmm3, %zmm3
	vpminsq	%zmm11, %zmm4, %zmm15
	vpmaxsq	%zmm11, %zmm4, %zmm4
	vpminsq	%zmm0, %zmm12, %zmm11
	vpmaxsq	%zmm0, %zmm12, %zmm12
	vpminsq	%zmm1, %zmm10, %zmm0
	vpmaxsq	%zmm1, %zmm10, %zmm10
	vpminsq	%zmm9, %zmm14, %zmm1
	vpmaxsq	%zmm9, %zmm14, %zmm14
	vpminsq	%zmm8, %zmm13, %zmm9
	vpmaxsq	%zmm8, %zmm13, %zmm13
	vpminsq	%zmm6, %zmm7, %zmm8
	vpmaxsq	%zmm6, %zmm7, %zmm7
	vpminsq	%zmm4, %zmm1, %zmm6
	vpmaxsq	%zmm4, %zmm1, %zmm1
	vpminsq	%zmm3, %zmm9, %zmm4
	vpmaxsq	%zmm3, %zmm9, %zmm9
	vpminsq	%zmm10, %zmm11, %zmm3
	vpmaxsq	%zmm10, %zmm11, %zmm11
	vpminsq	%zmm12, %zmm8, %zmm10
	vpmaxsq	%zmm12, %zmm8, %zmm8
	vpminsq	%zmm13, %zmm14, %zmm12
	vpmaxsq	%zmm13, %zmm14, %zmm14
	vpminsq	%zmm5, %zmm0, %zmm13
	vpmaxsq	%zmm5, %zmm0, %zmm0
	vpminsq	%zmm15, %zmm2, %zmm5
	vpmaxsq	%zmm15, %zmm2, %zmm2
	vpminsq	%zmm13, %zmm5, %zmm16
	vpmaxsq	%zmm13, %zmm5, %zmm5
	vpminsq	%zmm12, %zmm10, %zmm13
	vpmaxsq	%zmm12, %zmm10, %zmm10
	vpminsq	%zmm0, %zmm2, %zmm12
	vpmaxsq	%zmm0, %zmm2, %zmm2
	vpminsq	%zmm14, %zmm8, %zmm0
	vpminsq	%zmm5, %zmm12, %zmm15
	vpmaxsq	%zmm5, %zmm12, %zmm12
	vpminsq	%zmm4, %zmm6, %zmm5
	vpmaxsq	%zmm4, %zmm6, %zmm6
	vpminsq	%zmm1, %zmm9, %zmm4
	vpmaxsq	%zmm1, %zmm9, %zmm9
	vpminsq	%zmm10, %zmm0, %zmm1
	vpmaxsq	%zmm10, %zmm0, %zmm0
	vpminsq	%zmm2, %zmm3, %zmm10
	vpmaxsq	%zmm2, %zmm3, %zmm3
	vpminsq	%zmm11, %zmm13, %zmm2
	vpmaxsq	%zmm11, %zmm13, %zmm13
	vpminsq	%zmm5, %zmm10, %zmm11
	vpmaxsq	%zmm5, %zmm10, %zmm10
	vpminsq	%zmm3, %zmm6, %zmm5
	vpmaxsq	%zmm3, %zmm6, %zmm6
	vpminsq	%zmm4, %zmm2, %zmm3
	vpmaxsq	%zmm4, %zmm2, %zmm2
	vpminsq	%zmm13, %zmm9, %zmm4
	vpmaxsq	%zmm13, %zmm9, %zmm9
	vpminsq	%zmm5, %zmm10, %zmm13
	vpmaxsq	%zmm5, %zmm10, %zmm10
	vpminsq	%zmm6, %zmm3, %zmm5
	vpmaxsq	%zmm6, %zmm3, %zmm3
	vpminsq	%zmm4, %zmm2, %zmm6
	vpmaxsq	%zmm14, %zmm8, %zmm8
	vpmaxsq	%zmm4, %zmm2, %zmm2
	vpminsq	%zmm12, %zmm11, %zmm14
	vpminsq	%zmm9, %zmm1, %zmm4
	vpmaxsq	%zmm12, %zmm11, %zmm11
	vpmaxsq	%zmm9, %zmm1, %zmm1
	vpminsq	%zmm5, %zmm10, %zmm12
	vpmaxsq	%zmm5, %zmm10, %zmm10
	vpminsq	%zmm6, %zmm3, %zmm5
	vpmaxsq	%zmm6, %zmm3, %zmm3
	jbe	.L170
	vpshufd	$78, %zmm0, %zmm6
	vpshufd	$78, %zmm7, %zmm7
	vpshufd	$78, %zmm5, %zmm5
	movl	$85, %r15d
	vpshufd	$78, %zmm3, %zmm3
	vpshufd	$78, %zmm2, %zmm2
	vpshufd	$78, %zmm4, %zmm4
	kmovb	%r15d, %k1
	vpshufd	$78, %zmm1, %zmm1
	vpshufd	$78, %zmm8, %zmm8
	vpminsq	%zmm7, %zmm17, %zmm0
	cmpq	$3, -8(%rsp)
	vpmaxsq	%zmm7, %zmm17, %zmm9
	vpminsq	%zmm8, %zmm16, %zmm17
	vpminsq	%zmm2, %zmm13, %zmm7
	vpmaxsq	%zmm8, %zmm16, %zmm8
	vpshufd	$78, %zmm9, %zmm9
	vpminsq	%zmm6, %zmm15, %zmm16
	vpmaxsq	%zmm6, %zmm15, %zmm6
	vpshufd	$78, %zmm8, %zmm8
	vpminsq	%zmm1, %zmm14, %zmm15
	vpmaxsq	%zmm1, %zmm14, %zmm1
	vpshufd	$78, %zmm7, %zmm7
	vpminsq	%zmm4, %zmm11, %zmm14
	vpmaxsq	%zmm4, %zmm11, %zmm4
	vpshufd	$78, %zmm1, %zmm1
	vpminsq	%zmm3, %zmm12, %zmm11
	vpmaxsq	%zmm3, %zmm12, %zmm3
	vpshufd	$78, %zmm14, %zmm14
	vpminsq	%zmm5, %zmm10, %zmm12
	vpmaxsq	%zmm5, %zmm10, %zmm5
	vpshufd	$78, %zmm11, %zmm10
	vpshufd	$78, %zmm12, %zmm12
	vpmaxsq	%zmm2, %zmm13, %zmm2
	vpmaxsq	%zmm9, %zmm5, %zmm19
	vpminsq	%zmm12, %zmm0, %zmm11
	vpmaxsq	%zmm12, %zmm0, %zmm13
	vpshufd	$78, %zmm6, %zmm6
	vpminsq	%zmm9, %zmm5, %zmm0
	vpminsq	%zmm14, %zmm15, %zmm12
	vpminsq	%zmm8, %zmm3, %zmm9
	vpminsq	%zmm10, %zmm17, %zmm5
	vpshufd	$78, %zmm12, %zmm12
	vpmaxsq	%zmm10, %zmm17, %zmm10
	vpmaxsq	%zmm8, %zmm3, %zmm17
	vpminsq	%zmm7, %zmm16, %zmm3
	vpminsq	%zmm6, %zmm2, %zmm8
	vpshufd	$78, %zmm10, %zmm10
	vpmaxsq	%zmm6, %zmm2, %zmm6
	vpmaxsq	%zmm14, %zmm15, %zmm14
	vpshufd	$78, %zmm3, %zmm2
	vpminsq	%zmm1, %zmm4, %zmm15
	vpshufd	$78, %zmm17, %zmm3
	vpmaxsq	%zmm1, %zmm4, %zmm4
	vpmaxsq	%zmm12, %zmm11, %zmm17
	vpmaxsq	%zmm7, %zmm16, %zmm7
	vpshufd	$78, %zmm8, %zmm1
	vpshufd	$78, %zmm13, %zmm16
	vpshufd	$78, %zmm15, %zmm8
	vpminsq	%zmm12, %zmm11, %zmm13
	vpshufd	$78, %zmm19, %zmm15
	vpminsq	%zmm2, %zmm5, %zmm11
	vpminsq	%zmm16, %zmm14, %zmm12
	vpminsq	%zmm8, %zmm0, %zmm19
	vpmaxsq	%zmm16, %zmm14, %zmm14
	vpshufd	$78, %zmm11, %zmm11
	vpmaxsq	%zmm8, %zmm0, %zmm16
	vpminsq	%zmm1, %zmm9, %zmm0
	vpminsq	%zmm15, %zmm4, %zmm8
	vpmaxsq	%zmm15, %zmm4, %zmm15
	vpshufd	$78, %zmm0, %zmm4
	vpmaxsq	%zmm2, %zmm5, %zmm5
	vpshufd	$78, %zmm16, %zmm0
	vpminsq	%zmm10, %zmm7, %zmm2
	vpshufd	$78, %zmm15, %zmm16
	vpmaxsq	%zmm10, %zmm7, %zmm7
	vpminsq	%zmm11, %zmm13, %zmm15
	vpshufd	$78, %zmm17, %zmm10
	vpmaxsq	%zmm1, %zmm9, %zmm1
	vpminsq	%zmm3, %zmm6, %zmm9
	vpmaxsq	%zmm3, %zmm6, %zmm6
	vpshufd	$78, %zmm14, %zmm3
	vpmaxsq	%zmm11, %zmm13, %zmm14
	vpminsq	%zmm10, %zmm5, %zmm11
	vpmaxsq	%zmm10, %zmm5, %zmm5
	vpshufd	$78, %zmm15, %zmm10
	vpmaxsq	%zmm10, %zmm15, %zmm17
	vpshufd	$78, %zmm2, %zmm2
	vpshufd	$78, %zmm9, %zmm9
	vpminsq	%zmm10, %zmm15, %zmm17{%k1}
	vpshufd	$78, %zmm14, %zmm10
	vpminsq	%zmm2, %zmm12, %zmm13
	vpmaxsq	%zmm2, %zmm12, %zmm12
	vpminsq	%zmm3, %zmm7, %zmm2
	vpmaxsq	%zmm3, %zmm7, %zmm7
	vpminsq	%zmm4, %zmm19, %zmm3
	vpmaxsq	%zmm4, %zmm19, %zmm19
	vpminsq	%zmm0, %zmm1, %zmm4
	vpmaxsq	%zmm0, %zmm1, %zmm1
	vpminsq	%zmm9, %zmm8, %zmm0
	vpmaxsq	%zmm9, %zmm8, %zmm8
	vpminsq	%zmm16, %zmm6, %zmm9
	vpmaxsq	%zmm16, %zmm6, %zmm6
	vpmaxsq	%zmm10, %zmm14, %zmm16
	vpminsq	%zmm10, %zmm14, %zmm16{%k1}
	vpshufd	$78, %zmm11, %zmm10
	vpmaxsq	%zmm10, %zmm11, %zmm15
	vpminsq	%zmm10, %zmm11, %zmm15{%k1}
	vpshufd	$78, %zmm5, %zmm10
	vpmaxsq	%zmm10, %zmm5, %zmm14
	vpminsq	%zmm10, %zmm5, %zmm14{%k1}
	vpshufd	$78, %zmm13, %zmm5
	vpmaxsq	%zmm5, %zmm13, %zmm11
	vpminsq	%zmm5, %zmm13, %zmm11{%k1}
	vpshufd	$78, %zmm12, %zmm5
	vpmaxsq	%zmm5, %zmm12, %zmm13
	vpminsq	%zmm5, %zmm12, %zmm13{%k1}
	vpshufd	$78, %zmm2, %zmm5
	vpmaxsq	%zmm5, %zmm2, %zmm12
	vpminsq	%zmm5, %zmm2, %zmm12{%k1}
	vpshufd	$78, %zmm7, %zmm2
	vpmaxsq	%zmm2, %zmm7, %zmm10
	vpminsq	%zmm2, %zmm7, %zmm10{%k1}
	vpshufd	$78, %zmm3, %zmm2
	vpshufd	$78, %zmm4, %zmm7
	vpmaxsq	%zmm2, %zmm3, %zmm5
	vpminsq	%zmm2, %zmm3, %zmm5{%k1}
	vpshufd	$78, %zmm19, %zmm2
	vpmaxsq	%zmm2, %zmm19, %zmm3
	vpminsq	%zmm2, %zmm19, %zmm3{%k1}
	vpmaxsq	%zmm7, %zmm4, %zmm2
	vpminsq	%zmm7, %zmm4, %zmm2{%k1}
	vpshufd	$78, %zmm1, %zmm7
	vpmaxsq	%zmm7, %zmm1, %zmm4
	vpminsq	%zmm7, %zmm1, %zmm4{%k1}
	vpshufd	$78, %zmm0, %zmm7
	vpmaxsq	%zmm7, %zmm0, %zmm1
	vpminsq	%zmm7, %zmm0, %zmm1{%k1}
	vpshufd	$78, %zmm8, %zmm7
	vpmaxsq	%zmm7, %zmm8, %zmm0
	vpminsq	%zmm7, %zmm8, %zmm0{%k1}
	vpshufd	$78, %zmm9, %zmm7
	vpmaxsq	%zmm7, %zmm9, %zmm8
	vpminsq	%zmm7, %zmm9, %zmm8{%k1}
	vpshufd	$78, %zmm6, %zmm9
	vpmaxsq	%zmm9, %zmm6, %zmm7
	vpminsq	%zmm9, %zmm6, %zmm7{%k1}
	jbe	.L170
	vpermq	$27, %zmm5, %zmm5
	vpermq	$27, %zmm3, %zmm3
	vpermq	$27, %zmm2, %zmm2
	movl	$51, %r15d
	vpermq	$27, %zmm4, %zmm4
	vpermq	$27, %zmm1, %zmm1
	kmovb	%r15d, %k2
	vpermq	$27, %zmm0, %zmm0
	vpermq	$27, %zmm8, %zmm8
	vpermq	$27, %zmm7, %zmm7
	vpminsq	%zmm2, %zmm13, %zmm6
	cmpq	$7, -8(%rsp)
	vpminsq	%zmm7, %zmm17, %zmm9
	vpmaxsq	%zmm7, %zmm17, %zmm7
	vpermq	$27, %zmm6, %zmm6
	vpminsq	%zmm8, %zmm16, %zmm17
	vpmaxsq	%zmm8, %zmm16, %zmm8
	vpermq	$27, %zmm7, %zmm7
	vpminsq	%zmm0, %zmm15, %zmm16
	vpmaxsq	%zmm0, %zmm15, %zmm0
	vpermq	$27, %zmm8, %zmm8
	vpminsq	%zmm1, %zmm14, %zmm15
	vpmaxsq	%zmm1, %zmm14, %zmm1
	vpermq	$27, %zmm0, %zmm0
	vpminsq	%zmm4, %zmm11, %zmm14
	vpmaxsq	%zmm4, %zmm11, %zmm4
	vpermq	$27, %zmm1, %zmm1
	vpminsq	%zmm3, %zmm12, %zmm11
	vpmaxsq	%zmm3, %zmm12, %zmm3
	vpermq	$27, %zmm14, %zmm14
	vpminsq	%zmm5, %zmm10, %zmm12
	vpmaxsq	%zmm5, %zmm10, %zmm5
	vpermq	$27, %zmm11, %zmm10
	vpermq	$27, %zmm12, %zmm12
	vpminsq	%zmm7, %zmm5, %zmm11
	vpmaxsq	%zmm2, %zmm13, %zmm2
	vpminsq	%zmm12, %zmm9, %zmm19
	vpmaxsq	%zmm12, %zmm9, %zmm13
	vpmaxsq	%zmm7, %zmm5, %zmm20
	vpminsq	%zmm8, %zmm3, %zmm9
	vpminsq	%zmm10, %zmm17, %zmm5
	vpmaxsq	%zmm10, %zmm17, %zmm10
	vpmaxsq	%zmm8, %zmm3, %zmm17
	vpminsq	%zmm6, %zmm16, %zmm3
	vpermq	$27, %zmm10, %zmm10
	vpminsq	%zmm14, %zmm15, %zmm8
	vpminsq	%zmm0, %zmm2, %zmm7
	vpmaxsq	%zmm14, %zmm15, %zmm14
	vpmaxsq	%zmm0, %zmm2, %zmm0
	vpermq	$27, %zmm3, %zmm2
	vpminsq	%zmm1, %zmm4, %zmm15
	vpmaxsq	%zmm1, %zmm4, %zmm12
	vpermq	$27, %zmm8, %zmm3
	vpmaxsq	%zmm6, %zmm16, %zmm6
	vpermq	$27, %zmm13, %zmm1
	vpermq	$27, %zmm15, %zmm16
	vpermq	$27, %zmm17, %zmm8
	vpminsq	%zmm2, %zmm5, %zmm17
	vpminsq	%zmm3, %zmm19, %zmm13
	vpminsq	%zmm16, %zmm11, %zmm4
	vpermq	$27, %zmm7, %zmm7
	vpermq	$27, %zmm20, %zmm15
	vpmaxsq	%zmm3, %zmm19, %zmm19
	vpmaxsq	%zmm16, %zmm11, %zmm20
	vpermq	$27, %zmm17, %zmm11
	vpmaxsq	%zmm2, %zmm5, %zmm3
	vpminsq	%zmm1, %zmm14, %zmm5
	vpmaxsq	%zmm1, %zmm14, %zmm14
	vpminsq	%zmm7, %zmm9, %zmm16
	vpmaxsq	%zmm10, %zmm6, %zmm1
	vpminsq	%zmm10, %zmm6, %zmm2
	vpermq	$27, %zmm19, %zmm10
	vpermq	$27, %zmm14, %zmm6
	vpmaxsq	%zmm7, %zmm9, %zmm9
	vpminsq	%zmm11, %zmm13, %zmm14
	vpminsq	%zmm15, %zmm12, %zmm7
	vpmaxsq	%zmm15, %zmm12, %zmm15
	vpermq	$27, %zmm16, %zmm17
	vpminsq	%zmm8, %zmm0, %zmm12
	vpmaxsq	%zmm11, %zmm13, %zmm13
	vpermq	$27, %zmm2, %zmm2
	vpminsq	%zmm10, %zmm3, %zmm11
	vpermq	$27, %zmm12, %zmm16
	vpmaxsq	%zmm10, %zmm3, %zmm3
	vpmaxsq	%zmm8, %zmm0, %zmm0
	vpermq	$27, %zmm14, %zmm10
	vpermq	$27, %zmm20, %zmm8
	vpminsq	%zmm17, %zmm4, %zmm19
	vpmaxsq	%zmm17, %zmm4, %zmm4
	vpermq	$27, %zmm15, %zmm15
	vpminsq	%zmm8, %zmm9, %zmm17
	vpmaxsq	%zmm8, %zmm9, %zmm9
	vpminsq	%zmm16, %zmm7, %zmm8
	vpmaxsq	%zmm16, %zmm7, %zmm7
	vpmaxsq	%zmm10, %zmm14, %zmm16
	vpminsq	%zmm2, %zmm5, %zmm12
	vpminsq	%zmm10, %zmm14, %zmm16{%k2}
	vpermq	$27, %zmm13, %zmm10
	vpmaxsq	%zmm2, %zmm5, %zmm5
	vpminsq	%zmm6, %zmm1, %zmm2
	vpmaxsq	%zmm6, %zmm1, %zmm1
	vpminsq	%zmm15, %zmm0, %zmm6
	vpmaxsq	%zmm15, %zmm0, %zmm0
	vpmaxsq	%zmm10, %zmm13, %zmm15
	vpminsq	%zmm10, %zmm13, %zmm15{%k2}
	vpermq	$27, %zmm11, %zmm10
	vpmaxsq	%zmm10, %zmm11, %zmm14
	vpminsq	%zmm10, %zmm11, %zmm14{%k2}
	vpermq	$27, %zmm3, %zmm10
	vpmaxsq	%zmm10, %zmm3, %zmm11
	vpminsq	%zmm10, %zmm3, %zmm11{%k2}
	vpermq	$27, %zmm12, %zmm3
	vpmaxsq	%zmm3, %zmm12, %zmm13
	vpminsq	%zmm3, %zmm12, %zmm13{%k2}
	vpermq	$27, %zmm5, %zmm3
	vpmaxsq	%zmm3, %zmm5, %zmm12
	vpminsq	%zmm3, %zmm5, %zmm12{%k2}
	vpermq	$27, %zmm2, %zmm3
	vpmaxsq	%zmm3, %zmm2, %zmm10
	vpminsq	%zmm3, %zmm2, %zmm10{%k2}
	vpermq	$27, %zmm1, %zmm2
	vpmaxsq	%zmm2, %zmm1, %zmm5
	vpminsq	%zmm2, %zmm1, %zmm5{%k2}
	vpermq	$27, %zmm19, %zmm1
	vpmaxsq	%zmm1, %zmm19, %zmm3
	vpminsq	%zmm1, %zmm19, %zmm3{%k2}
	vpermq	$27, %zmm4, %zmm1
	vpmaxsq	%zmm1, %zmm4, %zmm2
	vpminsq	%zmm1, %zmm4, %zmm2{%k2}
	vpermq	$27, %zmm17, %zmm1
	vpmaxsq	%zmm1, %zmm17, %zmm4
	vpminsq	%zmm1, %zmm17, %zmm4{%k2}
	vpermq	$27, %zmm9, %zmm17
	vpmaxsq	%zmm17, %zmm9, %zmm1
	vpminsq	%zmm17, %zmm9, %zmm1{%k2}
	vpermq	$27, %zmm8, %zmm17
	vpmaxsq	%zmm17, %zmm8, %zmm9
	vpminsq	%zmm17, %zmm8, %zmm9{%k2}
	vpermq	$27, %zmm7, %zmm17
	vpmaxsq	%zmm17, %zmm7, %zmm8
	vpminsq	%zmm17, %zmm7, %zmm8{%k2}
	vpermq	$27, %zmm6, %zmm17
	vpmaxsq	%zmm17, %zmm6, %zmm7
	vpminsq	%zmm17, %zmm6, %zmm7{%k2}
	vpermq	$27, %zmm0, %zmm17
	vpmaxsq	%zmm17, %zmm0, %zmm6
	vpminsq	%zmm17, %zmm0, %zmm6{%k2}
	vpshufd	$78, %zmm16, %zmm0
	vpmaxsq	%zmm0, %zmm16, %zmm17
	vpminsq	%zmm0, %zmm16, %zmm17{%k1}
	vpshufd	$78, %zmm15, %zmm0
	vpmaxsq	%zmm0, %zmm15, %zmm16
	vpminsq	%zmm0, %zmm15, %zmm16{%k1}
	vpshufd	$78, %zmm14, %zmm0
	vpmaxsq	%zmm0, %zmm14, %zmm15
	vpminsq	%zmm0, %zmm14, %zmm15{%k1}
	vpshufd	$78, %zmm11, %zmm0
	vpmaxsq	%zmm0, %zmm11, %zmm14
	vpminsq	%zmm0, %zmm11, %zmm14{%k1}
	vpshufd	$78, %zmm13, %zmm0
	vpmaxsq	%zmm0, %zmm13, %zmm11
	vpminsq	%zmm0, %zmm13, %zmm11{%k1}
	vpshufd	$78, %zmm12, %zmm0
	vpmaxsq	%zmm0, %zmm12, %zmm13
	vpminsq	%zmm0, %zmm12, %zmm13{%k1}
	vpshufd	$78, %zmm10, %zmm0
	vpmaxsq	%zmm0, %zmm10, %zmm12
	vpminsq	%zmm0, %zmm10, %zmm12{%k1}
	vpshufd	$78, %zmm5, %zmm0
	vpmaxsq	%zmm0, %zmm5, %zmm10
	vpminsq	%zmm0, %zmm5, %zmm10{%k1}
	vpshufd	$78, %zmm3, %zmm0
	vpmaxsq	%zmm0, %zmm3, %zmm5
	vpminsq	%zmm0, %zmm3, %zmm5{%k1}
	vpshufd	$78, %zmm2, %zmm0
	vpmaxsq	%zmm0, %zmm2, %zmm3
	vpminsq	%zmm0, %zmm2, %zmm3{%k1}
	vpshufd	$78, %zmm4, %zmm0
	vpmaxsq	%zmm0, %zmm4, %zmm2
	vpminsq	%zmm0, %zmm4, %zmm2{%k1}
	vpshufd	$78, %zmm1, %zmm0
	vpmaxsq	%zmm0, %zmm1, %zmm4
	vpminsq	%zmm0, %zmm1, %zmm4{%k1}
	vpshufd	$78, %zmm9, %zmm0
	vpmaxsq	%zmm0, %zmm9, %zmm1
	vpminsq	%zmm0, %zmm9, %zmm1{%k1}
	vpshufd	$78, %zmm8, %zmm9
	vpmaxsq	%zmm9, %zmm8, %zmm0
	vpminsq	%zmm9, %zmm8, %zmm0{%k1}
	vpshufd	$78, %zmm7, %zmm9
	vpmaxsq	%zmm9, %zmm7, %zmm8
	vpminsq	%zmm9, %zmm7, %zmm8{%k1}
	vpshufd	$78, %zmm6, %zmm9
	vpmaxsq	%zmm9, %zmm6, %zmm7
	vpminsq	%zmm9, %zmm6, %zmm7{%k1}
	jbe	.L170
	vmovdqa64	.LC2(%rip), %zmm6
	movl	$65535, %r15d
	kmovd	%r15d, %k3
	vpermq	%zmm7, %zmm6, %zmm7
	vpermq	%zmm5, %zmm6, %zmm5
	vpermq	%zmm3, %zmm6, %zmm3
	vpermq	%zmm2, %zmm6, %zmm2
	vpermq	%zmm4, %zmm6, %zmm4
	vpermq	%zmm1, %zmm6, %zmm1
	vpermq	%zmm0, %zmm6, %zmm0
	vpermq	%zmm8, %zmm6, %zmm8
	vpminsq	%zmm7, %zmm17, %zmm9
	vpmaxsq	%zmm7, %zmm17, %zmm19
	vpminsq	%zmm8, %zmm16, %zmm7
	vpmaxsq	%zmm8, %zmm16, %zmm8
	vpminsq	%zmm0, %zmm15, %zmm16
	vpmaxsq	%zmm0, %zmm15, %zmm0
	vpminsq	%zmm1, %zmm14, %zmm15
	vpermq	%zmm8, %zmm6, %zmm8
	vpmaxsq	%zmm1, %zmm14, %zmm1
	vpminsq	%zmm4, %zmm11, %zmm14
	vpermq	%zmm0, %zmm6, %zmm0
	vpmaxsq	%zmm4, %zmm11, %zmm4
	vpminsq	%zmm2, %zmm13, %zmm11
	vpermq	%zmm14, %zmm6, %zmm14
	vpmaxsq	%zmm2, %zmm13, %zmm2
	vpminsq	%zmm3, %zmm12, %zmm13
	vpermq	%zmm11, %zmm6, %zmm11
	vpmaxsq	%zmm3, %zmm12, %zmm3
	vpminsq	%zmm5, %zmm10, %zmm12
	vpermq	%zmm13, %zmm6, %zmm17
	vpmaxsq	%zmm5, %zmm10, %zmm5
	vpermq	%zmm19, %zmm6, %zmm13
	vpermq	%zmm12, %zmm6, %zmm12
	vpminsq	%zmm12, %zmm9, %zmm10
	vpmaxsq	%zmm12, %zmm9, %zmm19
	vpermq	%zmm1, %zmm6, %zmm1
	vpminsq	%zmm13, %zmm5, %zmm12
	vpmaxsq	%zmm13, %zmm5, %zmm20
	vpminsq	%zmm8, %zmm3, %zmm9
	vpminsq	%zmm17, %zmm7, %zmm13
	vpminsq	%zmm11, %zmm16, %zmm5
	vpmaxsq	%zmm17, %zmm7, %zmm7
	vpmaxsq	%zmm8, %zmm3, %zmm17
	vpmaxsq	%zmm11, %zmm16, %zmm3
	vpermq	%zmm19, %zmm6, %zmm11
	vpminsq	%zmm14, %zmm15, %zmm16
	vpminsq	%zmm0, %zmm2, %zmm8
	vpmaxsq	%zmm14, %zmm15, %zmm14
	vpermq	%zmm16, %zmm6, %zmm16
	vpermq	%zmm5, %zmm6, %zmm15
	vpmaxsq	%zmm0, %zmm2, %zmm0
	vpminsq	%zmm1, %zmm4, %zmm2
	vpermq	%zmm20, %zmm6, %zmm5
	vpminsq	%zmm16, %zmm10, %zmm19
	vpmaxsq	%zmm1, %zmm4, %zmm1
	vpermq	%zmm7, %zmm6, %zmm4
	vpermq	%zmm8, %zmm6, %zmm7
	vpermq	%zmm2, %zmm6, %zmm8
	vpermq	%zmm17, %zmm6, %zmm2
	vpmaxsq	%zmm16, %zmm10, %zmm17
	vpminsq	%zmm15, %zmm13, %zmm16
	vpminsq	%zmm11, %zmm14, %zmm10
	vpmaxsq	%zmm15, %zmm13, %zmm13
	vpmaxsq	%zmm8, %zmm12, %zmm20
	vpmaxsq	%zmm11, %zmm14, %zmm15
	vpermq	%zmm16, %zmm6, %zmm11
	vpminsq	%zmm4, %zmm3, %zmm14
	vpmaxsq	%zmm5, %zmm1, %zmm22
	vpmaxsq	%zmm4, %zmm3, %zmm3
	vpminsq	%zmm2, %zmm0, %zmm21
	vpermq	%zmm22, %zmm6, %zmm16
	vpminsq	%zmm8, %zmm12, %zmm4
	vpminsq	%zmm7, %zmm9, %zmm8
	vpermq	%zmm17, %zmm6, %zmm12
	vpmaxsq	%zmm7, %zmm9, %zmm9
	vpermq	%zmm21, %zmm6, %zmm17
	vpminsq	%zmm5, %zmm1, %zmm7
	vpmaxsq	%zmm2, %zmm0, %zmm0
	vpermq	%zmm14, %zmm6, %zmm5
	vpermq	%zmm15, %zmm6, %zmm2
	vpermq	%zmm8, %zmm6, %zmm1
	vpminsq	%zmm11, %zmm19, %zmm15
	vpermq	%zmm20, %zmm6, %zmm8
	vpmaxsq	%zmm11, %zmm19, %zmm14
	vpminsq	%zmm12, %zmm13, %zmm11
	vpmaxsq	%zmm12, %zmm13, %zmm13
	vpminsq	%zmm5, %zmm10, %zmm12
	vpmaxsq	%zmm5, %zmm10, %zmm10
	vpminsq	%zmm2, %zmm3, %zmm5
	vpmaxsq	%zmm2, %zmm3, %zmm3
	vpminsq	%zmm1, %zmm4, %zmm2
	vpmaxsq	%zmm1, %zmm4, %zmm4
	vpminsq	%zmm8, %zmm9, %zmm1
	vpmaxsq	%zmm8, %zmm9, %zmm9
	vpminsq	%zmm17, %zmm7, %zmm8
	vpmaxsq	%zmm17, %zmm7, %zmm7
	vpminsq	%zmm16, %zmm0, %zmm17
	vpmaxsq	%zmm16, %zmm0, %zmm0
	vpermq	%zmm15, %zmm6, %zmm16
	vpminsq	%zmm16, %zmm15, %zmm19
	vpmaxsq	%zmm16, %zmm15, %zmm15
	vpermq	%zmm14, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm15{%k3}
	vpminsq	%zmm16, %zmm14, %zmm19
	vpmaxsq	%zmm16, %zmm14, %zmm14
	vpermq	%zmm11, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm14{%k3}
	vpminsq	%zmm16, %zmm11, %zmm19
	vpmaxsq	%zmm16, %zmm11, %zmm11
	vpermq	%zmm13, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm11{%k3}
	vpminsq	%zmm16, %zmm13, %zmm19
	vpmaxsq	%zmm16, %zmm13, %zmm13
	vpermq	%zmm12, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm13{%k3}
	vpminsq	%zmm16, %zmm12, %zmm19
	vpmaxsq	%zmm16, %zmm12, %zmm12
	vpermq	%zmm10, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm12{%k3}
	vpminsq	%zmm16, %zmm10, %zmm19
	vpmaxsq	%zmm16, %zmm10, %zmm10
	vpermq	%zmm5, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm10{%k3}
	vpminsq	%zmm16, %zmm5, %zmm19
	vpmaxsq	%zmm16, %zmm5, %zmm5
	vpermq	%zmm3, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm5{%k3}
	vpminsq	%zmm16, %zmm3, %zmm19
	vpmaxsq	%zmm16, %zmm3, %zmm3
	vpermq	%zmm2, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm3{%k3}
	vpminsq	%zmm16, %zmm2, %zmm19
	vpmaxsq	%zmm16, %zmm2, %zmm2
	vpermq	%zmm4, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm2{%k3}
	vpminsq	%zmm16, %zmm4, %zmm19
	vpmaxsq	%zmm16, %zmm4, %zmm4
	vpermq	%zmm1, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm4{%k3}
	vpminsq	%zmm16, %zmm1, %zmm19
	vpmaxsq	%zmm16, %zmm1, %zmm1
	vpermq	%zmm9, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm1{%k3}
	vpminsq	%zmm16, %zmm9, %zmm19
	vpmaxsq	%zmm16, %zmm9, %zmm9
	vpermq	%zmm8, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm9{%k3}
	vpminsq	%zmm16, %zmm8, %zmm19
	vpmaxsq	%zmm16, %zmm8, %zmm8
	vpermq	%zmm7, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm8{%k3}
	vpminsq	%zmm16, %zmm7, %zmm19
	vpmaxsq	%zmm16, %zmm7, %zmm7
	vpermq	%zmm17, %zmm6, %zmm16
	vpermq	%zmm0, %zmm6, %zmm6
	vmovdqu16	%zmm19, %zmm7{%k3}
	vpminsq	%zmm16, %zmm17, %zmm19
	vpmaxsq	%zmm16, %zmm17, %zmm17
	vpminsq	%zmm6, %zmm0, %zmm16
	vpmaxsq	%zmm6, %zmm0, %zmm0
	vshufi32x4	$177, %zmm15, %zmm15, %zmm6
	vmovdqu16	%zmm16, %zmm0{%k3}
	vpmaxsq	%zmm15, %zmm6, %zmm16
	vmovdqu16	%zmm19, %zmm17{%k3}
	vpminsq	%zmm15, %zmm6, %zmm16{%k2}
	vshufi32x4	$177, %zmm14, %zmm14, %zmm6
	vpmaxsq	%zmm14, %zmm6, %zmm15
	vpminsq	%zmm14, %zmm6, %zmm15{%k2}
	vshufi32x4	$177, %zmm11, %zmm11, %zmm6
	vpmaxsq	%zmm11, %zmm6, %zmm14
	vpminsq	%zmm11, %zmm6, %zmm14{%k2}
	vshufi32x4	$177, %zmm13, %zmm13, %zmm6
	vpmaxsq	%zmm13, %zmm6, %zmm11
	vpminsq	%zmm13, %zmm6, %zmm11{%k2}
	vshufi32x4	$177, %zmm12, %zmm12, %zmm6
	vpmaxsq	%zmm12, %zmm6, %zmm13
	vpminsq	%zmm12, %zmm6, %zmm13{%k2}
	vshufi32x4	$177, %zmm10, %zmm10, %zmm6
	vpmaxsq	%zmm10, %zmm6, %zmm12
	vpminsq	%zmm10, %zmm6, %zmm12{%k2}
	vshufi32x4	$177, %zmm5, %zmm5, %zmm6
	vpmaxsq	%zmm5, %zmm6, %zmm10
	vpminsq	%zmm5, %zmm6, %zmm10{%k2}
	vshufi32x4	$177, %zmm3, %zmm3, %zmm6
	vpmaxsq	%zmm3, %zmm6, %zmm5
	vpminsq	%zmm3, %zmm6, %zmm5{%k2}
	vshufi32x4	$177, %zmm2, %zmm2, %zmm6
	vpmaxsq	%zmm2, %zmm6, %zmm3
	vpminsq	%zmm2, %zmm6, %zmm3{%k2}
	vshufi32x4	$177, %zmm4, %zmm4, %zmm6
	vpmaxsq	%zmm4, %zmm6, %zmm2
	vpminsq	%zmm4, %zmm6, %zmm2{%k2}
	vshufi32x4	$177, %zmm1, %zmm1, %zmm6
	vpmaxsq	%zmm1, %zmm6, %zmm4
	vpminsq	%zmm1, %zmm6, %zmm4{%k2}
	vshufi32x4	$177, %zmm9, %zmm9, %zmm6
	vpmaxsq	%zmm9, %zmm6, %zmm1
	vpminsq	%zmm9, %zmm6, %zmm1{%k2}
	vshufi32x4	$177, %zmm8, %zmm8, %zmm6
	vpmaxsq	%zmm8, %zmm6, %zmm9
	vpminsq	%zmm8, %zmm6, %zmm9{%k2}
	vshufi32x4	$177, %zmm7, %zmm7, %zmm6
	vpmaxsq	%zmm7, %zmm6, %zmm8
	vpminsq	%zmm7, %zmm6, %zmm8{%k2}
	vshufi32x4	$177, %zmm17, %zmm17, %zmm6
	vpmaxsq	%zmm17, %zmm6, %zmm7
	vpminsq	%zmm17, %zmm6, %zmm7{%k2}
	vshufi32x4	$177, %zmm0, %zmm0, %zmm17
	vpmaxsq	%zmm0, %zmm17, %zmm6
	vpminsq	%zmm0, %zmm17, %zmm6{%k2}
	vpshufd	$78, %zmm16, %zmm0
	vpmaxsq	%zmm0, %zmm16, %zmm17
	vpminsq	%zmm0, %zmm16, %zmm17{%k1}
	vpshufd	$78, %zmm15, %zmm0
	vpmaxsq	%zmm0, %zmm15, %zmm16
	vpminsq	%zmm0, %zmm15, %zmm16{%k1}
	vpshufd	$78, %zmm14, %zmm0
	vpmaxsq	%zmm0, %zmm14, %zmm15
	vpminsq	%zmm0, %zmm14, %zmm15{%k1}
	vpshufd	$78, %zmm11, %zmm0
	vpmaxsq	%zmm0, %zmm11, %zmm14
	vpminsq	%zmm0, %zmm11, %zmm14{%k1}
	vpshufd	$78, %zmm13, %zmm0
	vpmaxsq	%zmm0, %zmm13, %zmm11
	vpminsq	%zmm0, %zmm13, %zmm11{%k1}
	vpshufd	$78, %zmm12, %zmm0
	vpmaxsq	%zmm0, %zmm12, %zmm13
	vpminsq	%zmm0, %zmm12, %zmm13{%k1}
	vpshufd	$78, %zmm10, %zmm0
	vpmaxsq	%zmm0, %zmm10, %zmm12
	vpminsq	%zmm0, %zmm10, %zmm12{%k1}
	vpshufd	$78, %zmm5, %zmm0
	vpmaxsq	%zmm0, %zmm5, %zmm10
	vpminsq	%zmm0, %zmm5, %zmm10{%k1}
	vpshufd	$78, %zmm3, %zmm0
	vpmaxsq	%zmm0, %zmm3, %zmm5
	vpminsq	%zmm0, %zmm3, %zmm5{%k1}
	vpshufd	$78, %zmm2, %zmm0
	vpmaxsq	%zmm0, %zmm2, %zmm3
	vpminsq	%zmm0, %zmm2, %zmm3{%k1}
	vpshufd	$78, %zmm4, %zmm0
	vpmaxsq	%zmm0, %zmm4, %zmm2
	vpminsq	%zmm0, %zmm4, %zmm2{%k1}
	vpshufd	$78, %zmm1, %zmm0
	vpmaxsq	%zmm0, %zmm1, %zmm4
	vpminsq	%zmm0, %zmm1, %zmm4{%k1}
	vpshufd	$78, %zmm9, %zmm0
	vpmaxsq	%zmm0, %zmm9, %zmm1
	vpminsq	%zmm0, %zmm9, %zmm1{%k1}
	vpshufd	$78, %zmm8, %zmm9
	vpmaxsq	%zmm9, %zmm8, %zmm0
	vpminsq	%zmm9, %zmm8, %zmm0{%k1}
	vpshufd	$78, %zmm7, %zmm9
	vpmaxsq	%zmm9, %zmm7, %zmm8
	vpminsq	%zmm9, %zmm7, %zmm8{%k1}
	vpshufd	$78, %zmm6, %zmm9
	vpmaxsq	%zmm9, %zmm6, %zmm7
	vpminsq	%zmm9, %zmm6, %zmm7{%k1}
.L170:
	vmovdqu64	%zmm17, (%rdi)
	vmovq	%xmm18, %rdi
	vmovdqu64	%zmm16, (%rdi)
	vmovdqu64	%zmm15, (%r14)
	vmovdqu64	%zmm14, 0(%r13)
	vmovdqu64	%zmm11, (%r12)
	vmovdqu64	%zmm13, (%rbx)
	vmovq	%xmm23, %rbx
	vmovdqu64	%zmm12, (%r11)
	vmovdqu64	%zmm10, (%r10)
	vmovdqu64	%zmm5, (%r9)
	vmovdqu64	%zmm3, (%r8)
	vmovdqu64	%zmm2, (%rbx)
	vmovdqu64	%zmm4, (%rsi)
	vmovdqu64	%zmm1, (%rcx)
	vmovdqu64	%zmm0, (%rdx)
	vmovdqu64	%zmm8, (%rax)
	movq	-16(%rsp), %rax
	vmovdqu64	%zmm7, (%rax)
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
.LFE18788:
	.size	_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0, .-_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	.section	.text._ZN3hwy6N_AVX26detail22MaybePartitionTwoValueINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX26detail22MaybePartitionTwoValueINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, @function
_ZN3hwy6N_AVX26detail22MaybePartitionTwoValueINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0:
.LFB18789:
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
	cmpq	$3, %rsi
	jbe	.L187
	movl	$4, %r13d
	xorl	%ebx, %ebx
	jmp	.L178
	.p2align 4,,10
	.p2align 3
.L174:
	vmovmskpd	%ymm2, %eax
	vmovdqu	%ymm3, (%rsi)
	popcntq	%rax, %rax
	addq	%rax, %rbx
	leaq	4(%r13), %rax
	cmpq	%r14, %rax
	ja	.L195
	movq	%rax, %r13
.L178:
	vpcmpeqq	-32(%r12,%r13,8), %ymm3, %ymm2
	vpcmpeqq	-32(%r12,%r13,8), %ymm1, %ymm0
	leaq	-4(%r13), %rdx
	leaq	(%r12,%rbx,8), %rsi
	vpor	%ymm0, %ymm2, %ymm4
	vmovmskpd	%ymm4, %eax
	cmpl	$15, %eax
	je	.L174
	vpcmpeqd	%ymm3, %ymm3, %ymm3
	vpxor	%ymm3, %ymm0, %ymm0
	vpandn	%ymm0, %ymm2, %ymm2
	vmovmskpd	%ymm2, %eax
	tzcntl	%eax, %eax
	addq	%rdx, %rax
	vpbroadcastq	(%r12,%rax,8), %ymm0
	leaq	4(%rbx), %rax
	vmovdqa	%ymm0, (%r15)
	cmpq	%rax, %rdx
	jb	.L175
	.p2align 4,,10
	.p2align 3
.L176:
	vmovdqu	%ymm1, -32(%r12,%rax,8)
	movq	%rax, %rbx
	addq	$4, %rax
	cmpq	%rdx, %rax
	jbe	.L176
.L175:
	subq	%rbx, %rdx
	xorl	%eax, %eax
	vmovq	%rdx, %xmm7
	vpbroadcastq	%xmm7, %ymm0
	vpcmpgtq	.LC3(%rip), %ymm0, %ymm0
	vpmaskmovq	%ymm1, %ymm0, (%r12,%rbx,8)
.L172:
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
.L195:
	.cfi_restore_state
	movq	%r14, %r8
	leaq	0(,%r13,8), %rsi
	leaq	(%r12,%rbx,8), %r9
	subq	%r13, %r8
.L173:
	testq	%r8, %r8
	je	.L182
	leaq	0(,%r8,8), %rdx
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
.L182:
	vmovdqa	(%rcx), %ymm0
	vmovq	%r8, %xmm6
	vmovdqa	.LC3(%rip), %ymm5
	vpbroadcastq	%xmm6, %ymm2
	vpcmpeqq	%ymm3, %ymm0, %ymm4
	vpcmpgtq	%ymm5, %ymm2, %ymm2
	vpcmpeqq	%ymm1, %ymm0, %ymm0
	vpand	%ymm2, %ymm4, %ymm7
	vpor	%ymm4, %ymm0, %ymm0
	vpcmpeqd	%ymm4, %ymm4, %ymm4
	vpxor	%ymm4, %ymm2, %ymm6
	vpor	%ymm6, %ymm0, %ymm0
	vmovmskpd	%ymm0, %eax
	cmpl	$15, %eax
	jne	.L196
	vmovmskpd	%ymm7, %edx
	vpmaskmovq	%ymm3, %ymm2, (%r9)
	popcntq	%rdx, %rdx
	addq	%rbx, %rdx
	leaq	4(%rdx), %rax
	cmpq	%rax, %r14
	jb	.L185
	.p2align 4,,10
	.p2align 3
.L186:
	vmovdqu	%ymm1, -32(%r12,%rax,8)
	movq	%rax, %rdx
	addq	$4, %rax
	cmpq	%rax, %r14
	jnb	.L186
.L185:
	subq	%rdx, %r14
	movl	$1, %eax
	vmovq	%r14, %xmm6
	vpbroadcastq	%xmm6, %ymm0
	vpcmpgtq	%ymm5, %ymm0, %ymm0
	vpmaskmovq	%ymm1, %ymm0, (%r12,%rdx,8)
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
.L187:
	.cfi_restore_state
	movq	%rsi, %r8
	movq	%rdi, %r9
	xorl	%esi, %esi
	xorl	%ebx, %ebx
	xorl	%r13d, %r13d
	jmp	.L173
.L196:
	vpxor	%ymm4, %ymm0, %ymm0
	vmovmskpd	%ymm0, %eax
	tzcntl	%eax, %eax
	addq	%r13, %rax
	vpbroadcastq	(%r12,%rax,8), %ymm0
	leaq	4(%rbx), %rax
	vmovdqa	%ymm0, (%r15)
	cmpq	%r13, %rax
	ja	.L183
	.p2align 4,,10
	.p2align 3
.L184:
	vmovdqu	%ymm1, -32(%r12,%rax,8)
	movq	%rax, %rbx
	leaq	4(%rax), %rax
	cmpq	%r13, %rax
	jbe	.L184
	leaq	(%r12,%rbx,8), %r9
.L183:
	subq	%rbx, %r13
	xorl	%eax, %eax
	vmovq	%r13, %xmm7
	vpbroadcastq	%xmm7, %ymm0
	vpcmpgtq	%ymm5, %ymm0, %ymm0
	vpmaskmovq	%ymm1, %ymm0, (%r9)
	jmp	.L172
	.cfi_endproc
.LFE18789:
	.size	_ZN3hwy6N_AVX26detail22MaybePartitionTwoValueINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, .-_ZN3hwy6N_AVX26detail22MaybePartitionTwoValueINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	.section	.text._ZN3hwy6N_AVX26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0, @function
_ZN3hwy6N_AVX26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0:
.LFB18790:
	.cfi_startproc
	cmpq	%rdx, %rsi
	jbe	.L210
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r8
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rax, %rsi
	jbe	.L210
	movq	(%rdi,%rdx,8), %r11
	vmovq	%r11, %xmm4
	vpunpcklqdq	%xmm4, %xmm4, %xmm0
	jmp	.L200
	.p2align 4,,10
	.p2align 3
.L201:
	cmpq	%rcx, %rsi
	jbe	.L210
	movq	%rdx, %rax
.L206:
	salq	$4, %r8
	vmovddup	(%rdi,%r8), %xmm1
	vpcmpgtq	%xmm3, %xmm1, %xmm1
	vmovmskpd	%xmm1, %r8d
	andl	$1, %r8d
	jne	.L203
.L202:
	cmpq	%rdx, %rax
	je	.L210
	leaq	(%rdi,%rax,8), %rdx
	movq	(%rdx), %rcx
	movq	%rcx, (%r10)
	movq	%r11, (%rdx)
	cmpq	%rax, %rsi
	jbe	.L211
	movq	%rax, %rdx
.L204:
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r8
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rsi, %rax
	jnb	.L210
.L200:
	vmovddup	(%rdi,%rax,8), %xmm1
	vpcmpgtq	%xmm0, %xmm1, %xmm2
	leaq	(%rdi,%rdx,8), %r10
	vmovdqa	%xmm0, %xmm3
	vmovmskpd	%xmm2, %r9d
	andl	$1, %r9d
	je	.L201
	cmpq	%rcx, %rsi
	jbe	.L202
	vmovdqa	%xmm1, %xmm3
	jmp	.L206
	.p2align 4,,10
	.p2align 3
.L203:
	cmpq	%rdx, %rcx
	je	.L212
	leaq	(%rdi,%rcx,8), %rax
	movq	(%rax), %rdx
	movq	%rdx, (%r10)
	movq	%rcx, %rdx
	movq	%r11, (%rax)
	jmp	.L204
	.p2align 4,,10
	.p2align 3
.L210:
	ret
	.p2align 4,,10
	.p2align 3
.L211:
	ret
.L212:
	ret
	.cfi_endproc
.LFE18790:
	.size	_ZN3hwy6N_AVX26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0, .-_ZN3hwy6N_AVX26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	.section	.text._ZN3hwy6N_SSE46detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE46detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, @function
_ZN3hwy6N_SSE46detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0:
.LFB18791:
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
	.cfi_offset 13, -40
	movq	%rdi, %r13
	pushq	%r12
	pushq	%rbx
	subq	$56, %rsp
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	cmpq	$1, %rsi
	jbe	.L236
	movl	$2, %r12d
	xorl	%ebx, %ebx
	jmp	.L221
	.p2align 4,,10
	.p2align 3
.L215:
	movmskpd	%xmm2, %eax
	movups	%xmm3, 0(%r13,%rbx,8)
	popcntq	%rax, %rax
	addq	%rax, %rbx
	leaq	2(%r12), %rax
	cmpq	%r14, %rax
	ja	.L268
	movq	%rax, %r12
.L221:
	movdqu	-16(%r13,%r12,8), %xmm2
	movdqu	-16(%r13,%r12,8), %xmm0
	leaq	-2(%r12), %rdx
	leaq	0(,%rbx,8), %rax
	pcmpeqq	%xmm3, %xmm2
	pcmpeqq	%xmm1, %xmm0
	movdqa	%xmm2, %xmm4
	por	%xmm0, %xmm4
	movmskpd	%xmm4, %esi
	cmpl	$3, %esi
	je	.L215
	pcmpeqd	%xmm3, %xmm3
	leaq	2(%rbx), %rdi
	pxor	%xmm3, %xmm0
	pandn	%xmm0, %xmm2
	movmskpd	%xmm2, %ecx
	rep bsfl	%ecx, %ecx
	movslq	%ecx, %rcx
	addq	%rdx, %rcx
	movddup	0(%r13,%rcx,8), %xmm0
	movaps	%xmm0, (%r15)
	cmpq	%rdx, %rdi
	ja	.L216
	movq	%rdx, %rcx
	addq	%r13, %rax
	subq	%rbx, %rcx
	leaq	-2(%rcx), %rsi
	movq	%rsi, %rcx
	andq	$-2, %rcx
	addq	%rbx, %rcx
	leaq	16(%r13,%rcx,8), %rcx
	.p2align 4,,10
	.p2align 3
.L217:
	movups	%xmm1, (%rax)
	addq	$16, %rax
	cmpq	%rcx, %rax
	jne	.L217
	andq	$-2, %rsi
	leaq	(%rsi,%rdi), %rbx
.L216:
	subq	%rbx, %rdx
	leaq	0(,%rbx,8), %rcx
	movq	%rdx, %xmm0
	punpcklqdq	%xmm0, %xmm0
	pcmpgtq	.LC0(%rip), %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L218
	movq	%xmm1, 0(%r13,%rbx,8)
.L218:
	pextrq	$1, %xmm0, %rax
	testq	%rax, %rax
	jne	.L269
.L229:
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
.L269:
	.cfi_restore_state
	pextrq	$1, %xmm1, 8(%r13,%rcx)
	jmp	.L229
	.p2align 4,,10
	.p2align 3
.L268:
	movq	%r14, %r8
	leaq	0(,%r12,8), %rsi
	leaq	0(,%rbx,8), %r9
	subq	%r12, %r8
.L214:
	testq	%r8, %r8
	je	.L225
	leaq	0(,%r8,8), %rdx
	movq	%rcx, %rdi
	addq	%r13, %rsi
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
.L225:
	movdqa	(%rcx), %xmm0
	movdqa	.LC0(%rip), %xmm4
	movq	%r8, %xmm2
	punpcklqdq	%xmm2, %xmm2
	movdqa	%xmm0, %xmm5
	pcmpgtq	%xmm4, %xmm2
	pcmpeqq	%xmm3, %xmm5
	pcmpeqq	%xmm1, %xmm0
	movdqa	%xmm2, %xmm6
	movdqa	%xmm5, %xmm7
	por	%xmm5, %xmm0
	pcmpeqd	%xmm5, %xmm5
	pxor	%xmm5, %xmm6
	pand	%xmm2, %xmm7
	por	%xmm6, %xmm0
	movmskpd	%xmm0, %eax
	cmpl	$3, %eax
	jne	.L270
	movq	%xmm2, %rax
	testq	%rax, %rax
	je	.L230
	movq	%xmm3, 0(%r13,%r9)
.L230:
	pextrq	$1, %xmm2, %rax
	testq	%rax, %rax
	jne	.L271
.L231:
	movmskpd	%xmm7, %edx
	popcntq	%rdx, %rdx
	addq	%rbx, %rdx
	leaq	2(%rdx), %rax
	cmpq	%rax, %r14
	jb	.L232
	.p2align 4,,10
	.p2align 3
.L233:
	movups	%xmm1, -16(%r13,%rax,8)
	movq	%rax, %rdx
	addq	$2, %rax
	cmpq	%rax, %r14
	jnb	.L233
.L232:
	subq	%rdx, %r14
	leaq	0(,%rdx,8), %rcx
	movq	%r14, %xmm0
	punpcklqdq	%xmm0, %xmm0
	pcmpgtq	%xmm4, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L234
	movq	%xmm1, 0(%r13,%rdx,8)
.L234:
	pextrq	$1, %xmm0, %rax
	testq	%rax, %rax
	je	.L235
	pextrq	$1, %xmm1, 8(%r13,%rcx)
.L235:
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
.L271:
	.cfi_restore_state
	pextrq	$1, %xmm3, 8(%r13,%r9)
	jmp	.L231
.L236:
	movq	%rsi, %r8
	xorl	%r9d, %r9d
	xorl	%esi, %esi
	xorl	%ebx, %ebx
	xorl	%r12d, %r12d
	jmp	.L214
.L270:
	pxor	%xmm5, %xmm0
	leaq	2(%rbx), %rsi
	movmskpd	%xmm0, %eax
	rep bsfl	%eax, %eax
	cltq
	addq	%r12, %rax
	movddup	0(%r13,%rax,8), %xmm0
	movaps	%xmm0, (%r15)
	cmpq	%r12, %rsi
	ja	.L226
	leaq	-2(%r12), %rcx
	leaq	0(%r13,%rbx,8), %rax
	subq	%rbx, %rcx
	movq	%rcx, %rdx
	andq	$-2, %rdx
	addq	%rbx, %rdx
	leaq	16(%r13,%rdx,8), %rdx
	.p2align 4,,10
	.p2align 3
.L227:
	movups	%xmm1, (%rax)
	addq	$16, %rax
	cmpq	%rax, %rdx
	jne	.L227
	andq	$-2, %rcx
	leaq	(%rcx,%rsi), %rbx
	leaq	0(,%rbx,8), %r9
.L226:
	subq	%rbx, %r12
	movq	%r12, %xmm0
	punpcklqdq	%xmm0, %xmm0
	pcmpgtq	%xmm4, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L228
	movq	%xmm1, 0(%r13,%r9)
.L228:
	pextrq	$1, %xmm0, %rax
	testq	%rax, %rax
	je	.L229
	pextrq	$1, %xmm1, 8(%r13,%r9)
	jmp	.L229
	.cfi_endproc
.LFE18791:
	.size	_ZN3hwy6N_SSE46detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, .-_ZN3hwy6N_SSE46detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	.section	.text._ZN3hwy6N_SSE46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0, @function
_ZN3hwy6N_SSE46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0:
.LFB18792:
	.cfi_startproc
	cmpq	%rdx, %rsi
	jbe	.L272
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r8
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rax, %rsi
	jbe	.L272
	movq	(%rdi,%rdx,8), %r11
	movq	%r11, %xmm4
	movddup	%xmm4, %xmm0
	jmp	.L275
	.p2align 4,,10
	.p2align 3
.L276:
	cmpq	%rcx, %rsi
	jbe	.L272
	movq	%rdx, %rax
.L281:
	salq	$4, %r8
	movddup	(%rdi,%r8), %xmm1
	pcmpgtq	%xmm3, %xmm1
	movmskpd	%xmm1, %r8d
	andl	$1, %r8d
	jne	.L278
.L277:
	cmpq	%rdx, %rax
	je	.L272
	leaq	(%rdi,%rax,8), %rdx
	movq	(%rdx), %rcx
	movq	%rcx, (%r10)
	movq	%r11, (%rdx)
	cmpq	%rax, %rsi
	jbe	.L285
	movq	%rax, %rdx
.L279:
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r8
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rsi, %rax
	jnb	.L272
.L275:
	movddup	(%rdi,%rax,8), %xmm1
	movdqa	%xmm1, %xmm2
	leaq	(%rdi,%rdx,8), %r10
	movdqa	%xmm0, %xmm3
	pcmpgtq	%xmm0, %xmm2
	movmskpd	%xmm2, %r9d
	andl	$1, %r9d
	je	.L276
	cmpq	%rcx, %rsi
	jbe	.L277
	movdqa	%xmm1, %xmm3
	jmp	.L281
	.p2align 4,,10
	.p2align 3
.L278:
	cmpq	%rdx, %rcx
	je	.L286
	leaq	(%rdi,%rcx,8), %rax
	movq	(%rax), %rdx
	movq	%rdx, (%r10)
	movq	%rcx, %rdx
	movq	%r11, (%rax)
	jmp	.L279
	.p2align 4,,10
	.p2align 3
.L272:
	ret
	.p2align 4,,10
	.p2align 3
.L285:
	ret
.L286:
	ret
	.cfi_endproc
.LFE18792:
	.size	_ZN3hwy6N_SSE46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0, .-_ZN3hwy6N_SSE46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	.section	.text._ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0, @function
_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0:
.LFB18793:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %rax
	salq	$3, %rax
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
	addq	$-128, %rsp
	leaq	(%r10,%rax), %r9
	leaq	(%r9,%rax), %r8
	movq	%rdi, -136(%rbp)
	movq	%rsi, -160(%rbp)
	movdqu	(%r15), %xmm14
	movdqu	(%rdi), %xmm7
	leaq	(%r8,%rax), %rdi
	movdqu	0(%r13), %xmm3
	movdqu	(%r14), %xmm8
	leaq	(%rdi,%rax), %rsi
	movdqa	%xmm14, %xmm0
	movdqa	%xmm14, %xmm12
	movdqu	(%rbx), %xmm13
	movdqu	(%r12), %xmm1
	pcmpgtq	%xmm7, %xmm0
	movdqu	(%r10), %xmm11
	movdqu	(%r11), %xmm6
	leaq	(%rsi,%rax), %rcx
	movdqu	(%r8), %xmm10
	leaq	(%rcx,%rax), %rdx
	movdqu	(%rsi), %xmm2
	movdqu	(%rdx), %xmm4
	movdqu	(%rdi), %xmm5
	movq	%rdx, -120(%rbp)
	addq	%rax, %rdx
	pblendvb	%xmm0, %xmm7, %xmm12
	addq	%rdx, %rax
	movdqu	(%rcx), %xmm15
	movq	%rdx, -128(%rbp)
	pblendvb	%xmm0, %xmm14, %xmm7
	movdqa	%xmm3, %xmm0
	movdqa	%xmm3, %xmm14
	movaps	%xmm12, -80(%rbp)
	pcmpgtq	%xmm8, %xmm0
	movdqu	(%r9), %xmm12
	movdqu	(%rax), %xmm9
	movaps	%xmm4, -64(%rbp)
	movdqu	(%rdx), %xmm4
	pblendvb	%xmm0, %xmm8, %xmm14
	pblendvb	%xmm0, %xmm3, %xmm8
	movdqa	%xmm13, %xmm0
	pcmpgtq	%xmm1, %xmm0
	movdqa	%xmm13, %xmm3
	pblendvb	%xmm0, %xmm1, %xmm3
	pblendvb	%xmm0, %xmm13, %xmm1
	movdqa	%xmm11, %xmm0
	pcmpgtq	%xmm6, %xmm0
	movdqa	%xmm11, %xmm13
	pblendvb	%xmm0, %xmm6, %xmm13
	pblendvb	%xmm0, %xmm11, %xmm6
	movdqu	(%r9), %xmm11
	movdqa	%xmm10, %xmm0
	pcmpgtq	%xmm11, %xmm0
	movdqa	%xmm10, %xmm11
	pblendvb	%xmm0, %xmm12, %xmm11
	movaps	%xmm11, -96(%rbp)
	movdqa	%xmm12, %xmm11
	movdqa	-80(%rbp), %xmm12
	pblendvb	%xmm0, %xmm10, %xmm11
	movdqa	%xmm2, %xmm0
	movdqa	%xmm2, %xmm10
	pcmpgtq	%xmm5, %xmm0
	movaps	%xmm11, -112(%rbp)
	pblendvb	%xmm0, %xmm5, %xmm10
	pblendvb	%xmm0, %xmm2, %xmm5
	movdqa	%xmm10, %xmm11
	movdqa	-64(%rbp), %xmm10
	movdqa	%xmm10, %xmm0
	movdqa	%xmm10, %xmm2
	pcmpgtq	%xmm15, %xmm0
	pblendvb	%xmm0, %xmm15, %xmm2
	pblendvb	%xmm0, %xmm10, %xmm15
	movdqa	%xmm9, %xmm0
	pcmpgtq	%xmm4, %xmm0
	movdqa	%xmm9, %xmm10
	pblendvb	%xmm0, %xmm4, %xmm10
	pblendvb	%xmm0, %xmm9, %xmm4
	movdqa	%xmm14, %xmm0
	pcmpgtq	%xmm12, %xmm0
	movdqa	%xmm14, %xmm9
	pblendvb	%xmm0, %xmm12, %xmm9
	pblendvb	%xmm0, %xmm14, %xmm12
	movdqa	%xmm8, %xmm0
	pcmpgtq	%xmm7, %xmm0
	movdqa	%xmm8, %xmm14
	pblendvb	%xmm0, %xmm7, %xmm14
	pblendvb	%xmm0, %xmm8, %xmm7
	movdqa	%xmm13, %xmm0
	pcmpgtq	%xmm3, %xmm0
	movaps	%xmm7, -176(%rbp)
	movdqa	%xmm13, %xmm7
	movdqa	%xmm6, %xmm8
	pblendvb	%xmm0, %xmm3, %xmm7
	pblendvb	%xmm0, %xmm13, %xmm3
	movdqa	%xmm6, %xmm0
	movdqa	-96(%rbp), %xmm13
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm8
	pblendvb	%xmm0, %xmm6, %xmm1
	movdqa	%xmm11, %xmm0
	pcmpgtq	%xmm13, %xmm0
	movdqa	%xmm11, %xmm6
	movaps	%xmm1, -64(%rbp)
	movdqa	%xmm5, %xmm1
	pblendvb	%xmm0, %xmm13, %xmm6
	pblendvb	%xmm0, %xmm11, %xmm13
	movdqa	%xmm5, %xmm0
	movdqa	-112(%rbp), %xmm11
	pcmpgtq	%xmm11, %xmm0
	pblendvb	%xmm0, %xmm11, %xmm1
	pblendvb	%xmm0, %xmm5, %xmm11
	movdqa	%xmm10, %xmm0
	pcmpgtq	%xmm2, %xmm0
	movdqa	%xmm10, %xmm5
	movaps	%xmm11, -80(%rbp)
	movdqa	-176(%rbp), %xmm11
	movaps	%xmm1, -96(%rbp)
	pblendvb	%xmm0, %xmm2, %xmm5
	pblendvb	%xmm0, %xmm10, %xmm2
	movdqa	%xmm4, %xmm0
	pcmpgtq	%xmm15, %xmm0
	movdqa	%xmm4, %xmm10
	pblendvb	%xmm0, %xmm15, %xmm10
	pblendvb	%xmm0, %xmm4, %xmm15
	movdqa	%xmm7, %xmm0
	pcmpgtq	%xmm9, %xmm0
	movdqa	%xmm7, %xmm4
	pblendvb	%xmm0, %xmm9, %xmm4
	pblendvb	%xmm0, %xmm7, %xmm9
	movdqa	%xmm8, %xmm0
	pcmpgtq	%xmm14, %xmm0
	movdqa	%xmm8, %xmm7
	pblendvb	%xmm0, %xmm14, %xmm7
	pblendvb	%xmm0, %xmm8, %xmm14
	movdqa	%xmm3, %xmm0
	pcmpgtq	%xmm12, %xmm0
	movdqa	%xmm3, %xmm8
	pblendvb	%xmm0, %xmm12, %xmm8
	pblendvb	%xmm0, %xmm3, %xmm12
	movdqa	-64(%rbp), %xmm3
	movdqa	%xmm3, %xmm0
	movdqa	%xmm3, %xmm1
	pcmpgtq	%xmm11, %xmm0
	pblendvb	%xmm0, %xmm11, %xmm1
	pblendvb	%xmm0, -64(%rbp), %xmm11
	movdqa	%xmm5, %xmm0
	pcmpgtq	%xmm6, %xmm0
	movdqa	%xmm1, %xmm3
	movaps	%xmm11, -112(%rbp)
	movdqa	%xmm5, %xmm1
	movdqa	-96(%rbp), %xmm11
	pblendvb	%xmm0, %xmm6, %xmm1
	pblendvb	%xmm0, %xmm5, %xmm6
	movdqa	%xmm10, %xmm0
	pcmpgtq	%xmm11, %xmm0
	movdqa	%xmm10, %xmm5
	pblendvb	%xmm0, %xmm11, %xmm5
	pblendvb	%xmm0, %xmm10, %xmm11
	movdqa	%xmm2, %xmm0
	pcmpgtq	%xmm13, %xmm0
	movdqa	%xmm2, %xmm10
	movaps	%xmm11, -64(%rbp)
	movdqa	%xmm15, %xmm11
	pblendvb	%xmm0, %xmm13, %xmm10
	pblendvb	%xmm0, %xmm2, %xmm13
	movdqa	-80(%rbp), %xmm2
	movdqa	%xmm15, %xmm0
	pcmpgtq	%xmm2, %xmm0
	pblendvb	%xmm0, %xmm2, %xmm11
	movdqa	%xmm11, %xmm2
	movdqa	-80(%rbp), %xmm11
	pblendvb	%xmm0, %xmm15, %xmm11
	movdqa	%xmm1, %xmm0
	movdqa	%xmm1, %xmm15
	pcmpgtq	%xmm4, %xmm0
	pblendvb	%xmm0, %xmm4, %xmm15
	pblendvb	%xmm0, %xmm1, %xmm4
	movdqa	%xmm5, %xmm0
	pcmpgtq	%xmm7, %xmm0
	movaps	%xmm15, -80(%rbp)
	movdqa	%xmm10, %xmm1
	movaps	%xmm15, -256(%rbp)
	movdqa	%xmm5, %xmm15
	pblendvb	%xmm0, %xmm7, %xmm15
	pblendvb	%xmm0, %xmm5, %xmm7
	movdqa	%xmm10, %xmm0
	pcmpgtq	%xmm8, %xmm0
	movdqa	%xmm2, %xmm5
	pblendvb	%xmm0, %xmm8, %xmm1
	pblendvb	%xmm0, %xmm10, %xmm8
	movdqa	%xmm2, %xmm0
	pcmpgtq	%xmm3, %xmm0
	movdqa	%xmm14, %xmm10
	pblendvb	%xmm0, %xmm3, %xmm5
	pblendvb	%xmm0, %xmm2, %xmm3
	movdqa	%xmm6, %xmm0
	pcmpgtq	%xmm9, %xmm0
	movdqa	%xmm6, %xmm2
	pblendvb	%xmm0, %xmm9, %xmm2
	pblendvb	%xmm0, %xmm6, %xmm9
	movdqa	-64(%rbp), %xmm6
	movdqa	%xmm6, %xmm0
	pcmpgtq	%xmm14, %xmm0
	pblendvb	%xmm0, -64(%rbp), %xmm10
	pblendvb	%xmm0, %xmm14, %xmm6
	movdqa	%xmm13, %xmm0
	pcmpgtq	%xmm12, %xmm0
	movdqa	%xmm13, %xmm14
	pblendvb	%xmm0, %xmm12, %xmm14
	pblendvb	%xmm0, %xmm13, %xmm12
	movdqa	%xmm11, %xmm0
	movaps	%xmm14, -64(%rbp)
	movdqa	%xmm11, %xmm14
	movdqa	-112(%rbp), %xmm11
	movdqa	%xmm14, %xmm13
	pcmpgtq	%xmm11, %xmm0
	pblendvb	%xmm0, %xmm11, %xmm13
	pblendvb	%xmm0, %xmm14, %xmm11
	movdqa	%xmm8, %xmm0
	pcmpgtq	%xmm6, %xmm0
	movaps	%xmm11, -176(%rbp)
	movdqa	%xmm8, %xmm14
	movdqa	-64(%rbp), %xmm11
	pblendvb	%xmm0, %xmm6, %xmm14
	pblendvb	%xmm0, %xmm8, %xmm6
	movdqa	%xmm7, %xmm0
	pcmpgtq	%xmm11, %xmm0
	movdqa	%xmm7, %xmm8
	pblendvb	%xmm0, %xmm11, %xmm8
	pblendvb	%xmm0, %xmm7, %xmm11
	movdqa	%xmm9, %xmm0
	pcmpgtq	%xmm5, %xmm0
	movdqa	%xmm9, %xmm7
	pblendvb	%xmm0, %xmm5, %xmm7
	pblendvb	%xmm0, %xmm9, %xmm5
	movdqa	%xmm3, %xmm0
	pcmpgtq	%xmm13, %xmm0
	movdqa	%xmm3, %xmm9
	pblendvb	%xmm0, %xmm13, %xmm9
	pblendvb	%xmm0, %xmm3, %xmm13
	movdqa	%xmm12, %xmm0
	pcmpgtq	%xmm10, %xmm0
	movdqa	%xmm12, %xmm3
	pblendvb	%xmm0, %xmm10, %xmm3
	pblendvb	%xmm0, %xmm12, %xmm10
	movdqa	%xmm4, %xmm0
	pcmpgtq	%xmm2, %xmm0
	movdqa	%xmm4, %xmm12
	pblendvb	%xmm0, %xmm2, %xmm12
	pblendvb	%xmm0, %xmm4, %xmm2
	movdqa	%xmm1, %xmm0
	pcmpgtq	%xmm15, %xmm0
	movdqa	%xmm1, %xmm4
	pblendvb	%xmm0, %xmm15, %xmm4
	pblendvb	%xmm0, %xmm1, %xmm15
	movdqa	%xmm12, %xmm0
	pcmpgtq	%xmm4, %xmm0
	movdqa	%xmm12, %xmm1
	pblendvb	%xmm0, %xmm4, %xmm1
	pblendvb	%xmm0, %xmm12, %xmm4
	movdqa	%xmm3, %xmm0
	pcmpgtq	%xmm9, %xmm0
	movdqa	%xmm3, %xmm12
	movaps	%xmm1, -272(%rbp)
	movaps	%xmm1, -64(%rbp)
	movdqa	%xmm2, %xmm1
	pblendvb	%xmm0, %xmm9, %xmm12
	pblendvb	%xmm0, %xmm3, %xmm9
	movdqa	%xmm2, %xmm0
	pcmpgtq	%xmm15, %xmm0
	movdqa	%xmm4, %xmm3
	pblendvb	%xmm0, %xmm15, %xmm1
	pblendvb	%xmm0, %xmm2, %xmm15
	movdqa	%xmm10, %xmm0
	pcmpgtq	%xmm13, %xmm0
	movdqa	%xmm10, %xmm2
	pblendvb	%xmm0, %xmm13, %xmm2
	pblendvb	%xmm0, %xmm10, %xmm13
	movdqa	%xmm4, %xmm0
	pcmpgtq	%xmm1, %xmm0
	movaps	%xmm13, -192(%rbp)
	movdqa	%xmm8, %xmm10
	pblendvb	%xmm0, %xmm1, %xmm3
	pblendvb	%xmm0, %xmm4, %xmm1
	movdqa	%xmm8, %xmm0
	pcmpgtq	%xmm14, %xmm0
	movdqa	%xmm1, %xmm13
	movdqa	%xmm14, %xmm1
	movaps	%xmm3, -288(%rbp)
	movdqa	%xmm11, %xmm4
	movaps	%xmm3, -96(%rbp)
	movdqa	%xmm15, %xmm3
	pblendvb	%xmm0, %xmm14, %xmm10
	pblendvb	%xmm0, %xmm8, %xmm1
	movdqa	%xmm6, %xmm0
	pcmpgtq	%xmm11, %xmm0
	movdqa	%xmm6, %xmm8
	pblendvb	%xmm0, %xmm6, %xmm4
	pblendvb	%xmm0, %xmm11, %xmm8
	movdqa	%xmm9, %xmm0
	pcmpgtq	%xmm2, %xmm0
	movdqa	%xmm9, %xmm6
	movdqa	%xmm8, %xmm11
	pblendvb	%xmm0, %xmm2, %xmm6
	pblendvb	%xmm0, %xmm9, %xmm2
	movdqa	%xmm15, %xmm0
	pcmpgtq	%xmm7, %xmm0
	movaps	%xmm2, -224(%rbp)
	movdqa	%xmm5, %xmm2
	movdqa	%xmm13, %xmm9
	movaps	%xmm6, -208(%rbp)
	pblendvb	%xmm0, %xmm7, %xmm3
	pblendvb	%xmm0, %xmm15, %xmm7
	movdqa	%xmm5, %xmm0
	pcmpgtq	%xmm12, %xmm0
	pblendvb	%xmm0, %xmm12, %xmm2
	pblendvb	%xmm0, %xmm5, %xmm12
	movdqa	%xmm10, %xmm0
	pcmpgtq	%xmm3, %xmm0
	movdqa	%xmm10, %xmm5
	movdqa	%xmm2, %xmm6
	movdqa	%xmm7, %xmm2
	pblendvb	%xmm0, %xmm3, %xmm5
	pblendvb	%xmm0, %xmm10, %xmm3
	movdqa	%xmm7, %xmm0
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm2
	pblendvb	%xmm0, %xmm7, %xmm1
	movdqa	%xmm8, %xmm0
	pcmpgtq	%xmm6, %xmm0
	movdqa	%xmm12, %xmm7
	movdqa	%xmm2, %xmm14
	pblendvb	%xmm0, %xmm6, %xmm11
	pblendvb	%xmm0, %xmm8, %xmm6
	movdqa	%xmm12, %xmm0
	pcmpgtq	%xmm4, %xmm0
	pblendvb	%xmm0, %xmm4, %xmm7
	pblendvb	%xmm0, %xmm12, %xmm4
	movdqa	%xmm13, %xmm0
	pcmpgtq	%xmm5, %xmm0
	pblendvb	%xmm0, %xmm5, %xmm9
	pblendvb	%xmm0, %xmm13, %xmm5
	movdqa	%xmm2, %xmm0
	pcmpgtq	%xmm3, %xmm0
	movdqa	%xmm5, %xmm10
	movdqa	%xmm5, %xmm15
	movaps	%xmm9, -112(%rbp)
	movdqa	%xmm7, %xmm5
	cmpq	$1, -160(%rbp)
	pblendvb	%xmm0, %xmm3, %xmm14
	pblendvb	%xmm0, %xmm2, %xmm3
	movdqa	%xmm1, %xmm0
	pcmpgtq	%xmm11, %xmm0
	movdqa	%xmm1, %xmm2
	pblendvb	%xmm0, %xmm11, %xmm2
	pblendvb	%xmm0, %xmm1, %xmm11
	movdqa	%xmm7, %xmm0
	pcmpgtq	%xmm6, %xmm0
	movdqa	%xmm6, %xmm1
	movdqa	%xmm2, %xmm13
	pblendvb	%xmm0, %xmm6, %xmm5
	movdqa	-208(%rbp), %xmm6
	pblendvb	%xmm0, %xmm7, %xmm1
	movdqa	%xmm4, %xmm0
	movaps	%xmm1, -240(%rbp)
	movdqa	%xmm4, %xmm1
	pcmpgtq	%xmm6, %xmm0
	movdqa	%xmm6, %xmm7
	pblendvb	%xmm0, %xmm4, %xmm7
	pblendvb	%xmm0, %xmm6, %xmm1
	movdqa	%xmm2, %xmm0
	pcmpgtq	%xmm3, %xmm0
	movdqa	%xmm5, %xmm4
	pblendvb	%xmm0, %xmm3, %xmm13
	pblendvb	%xmm0, %xmm2, %xmm3
	movdqa	%xmm5, %xmm0
	pcmpgtq	%xmm11, %xmm0
	pblendvb	%xmm0, %xmm11, %xmm4
	pblendvb	%xmm0, %xmm5, %xmm11
	jbe	.L291
	movdqa	-256(%rbp), %xmm12
	pshufd	$78, %xmm7, %xmm7
	pshufd	$78, -176(%rbp), %xmm6
	movdqa	%xmm6, %xmm0
	movdqa	%xmm6, %xmm15
	pshufd	$78, %xmm1, %xmm1
	pshufd	$78, %xmm11, %xmm11
	pshufd	$78, -192(%rbp), %xmm2
	pcmpgtq	%xmm12, %xmm0
	pshufd	$78, %xmm4, %xmm4
	pshufd	$78, -224(%rbp), %xmm5
	pshufd	$78, -240(%rbp), %xmm8
	pblendvb	%xmm0, %xmm12, %xmm15
	pblendvb	%xmm0, %xmm6, %xmm12
	movdqa	%xmm2, %xmm0
	movaps	%xmm12, -64(%rbp)
	movdqa	%xmm2, %xmm6
	movdqa	-272(%rbp), %xmm12
	pcmpgtq	%xmm12, %xmm0
	pblendvb	%xmm0, %xmm12, %xmm6
	pblendvb	%xmm0, %xmm2, %xmm12
	movdqa	%xmm5, %xmm0
	movdqa	-288(%rbp), %xmm2
	movaps	%xmm12, -80(%rbp)
	movdqa	%xmm5, %xmm12
	pcmpgtq	%xmm2, %xmm0
	pblendvb	%xmm0, %xmm2, %xmm12
	pblendvb	%xmm0, %xmm5, %xmm2
	movdqa	%xmm7, %xmm0
	pcmpgtq	%xmm9, %xmm0
	movdqa	%xmm7, %xmm5
	movaps	%xmm12, -96(%rbp)
	pblendvb	%xmm0, %xmm9, %xmm5
	pblendvb	%xmm0, %xmm7, %xmm9
	movdqa	%xmm1, %xmm0
	pcmpgtq	%xmm10, %xmm0
	movdqa	%xmm1, %xmm7
	movaps	%xmm5, -112(%rbp)
	pshufd	$78, %xmm9, %xmm5
	pshufd	$78, %xmm2, %xmm9
	pblendvb	%xmm0, %xmm10, %xmm7
	pblendvb	%xmm0, %xmm1, %xmm10
	movdqa	%xmm8, %xmm0
	pcmpgtq	%xmm14, %xmm0
	movdqa	%xmm8, %xmm1
	pshufd	$78, %xmm7, %xmm7
	movaps	%xmm10, -160(%rbp)
	pblendvb	%xmm0, %xmm14, %xmm1
	movdqa	%xmm1, %xmm10
	movdqa	%xmm14, %xmm1
	movdqa	%xmm4, %xmm14
	pblendvb	%xmm0, %xmm8, %xmm1
	movdqa	%xmm11, %xmm0
	movdqa	%xmm11, %xmm8
	pcmpgtq	%xmm13, %xmm0
	pshufd	$78, %xmm10, %xmm10
	pblendvb	%xmm0, %xmm13, %xmm8
	pblendvb	%xmm0, %xmm11, %xmm13
	movdqa	%xmm4, %xmm0
	pshufd	$78, -80(%rbp), %xmm11
	pcmpgtq	%xmm3, %xmm0
	pshufd	$78, %xmm8, %xmm8
	pblendvb	%xmm0, %xmm3, %xmm14
	pblendvb	%xmm0, %xmm4, %xmm3
	pshufd	$78, -64(%rbp), %xmm4
	pshufd	$78, %xmm14, %xmm14
	movdqa	%xmm3, %xmm2
	movdqa	%xmm14, %xmm0
	movdqa	%xmm14, %xmm12
	pcmpgtq	%xmm15, %xmm0
	pblendvb	%xmm0, %xmm15, %xmm12
	pblendvb	%xmm0, %xmm14, %xmm15
	movdqa	%xmm4, %xmm0
	pcmpgtq	%xmm3, %xmm0
	movdqa	%xmm4, %xmm14
	pblendvb	%xmm0, %xmm3, %xmm14
	pblendvb	%xmm0, %xmm4, %xmm2
	movdqa	%xmm8, %xmm0
	movdqa	-96(%rbp), %xmm3
	pcmpgtq	%xmm6, %xmm0
	movdqa	%xmm8, %xmm4
	movaps	%xmm14, -64(%rbp)
	movdqa	%xmm2, %xmm14
	movdqa	%xmm10, %xmm2
	pblendvb	%xmm0, %xmm6, %xmm4
	pblendvb	%xmm0, %xmm8, %xmm6
	movdqa	%xmm11, %xmm0
	pcmpgtq	%xmm13, %xmm0
	movdqa	%xmm11, %xmm8
	pshufd	$78, %xmm6, %xmm6
	pblendvb	%xmm0, %xmm13, %xmm8
	pblendvb	%xmm0, %xmm11, %xmm13
	movdqa	%xmm10, %xmm0
	pcmpgtq	%xmm3, %xmm0
	movaps	%xmm8, -80(%rbp)
	movdqa	%xmm9, %xmm11
	pshufd	$78, %xmm13, %xmm13
	pblendvb	%xmm0, %xmm3, %xmm2
	movdqa	%xmm2, %xmm8
	movdqa	%xmm3, %xmm2
	movdqa	-112(%rbp), %xmm3
	pblendvb	%xmm0, %xmm10, %xmm2
	movdqa	%xmm9, %xmm0
	pshufd	$78, %xmm8, %xmm8
	pcmpgtq	%xmm1, %xmm0
	pshufd	$78, %xmm15, %xmm10
	pblendvb	%xmm0, %xmm1, %xmm11
	pblendvb	%xmm0, %xmm9, %xmm1
	movdqa	%xmm7, %xmm0
	pcmpgtq	%xmm3, %xmm0
	movdqa	%xmm7, %xmm9
	movaps	%xmm11, -96(%rbp)
	pshufd	$78, -96(%rbp), %xmm15
	pblendvb	%xmm0, %xmm3, %xmm9
	pblendvb	%xmm0, %xmm7, %xmm3
	movdqa	%xmm5, %xmm0
	movdqa	%xmm3, %xmm11
	pshufd	$78, %xmm9, %xmm9
	movdqa	%xmm5, %xmm7
	movdqa	-160(%rbp), %xmm3
	pcmpgtq	%xmm3, %xmm0
	pblendvb	%xmm0, %xmm3, %xmm7
	pblendvb	%xmm0, %xmm5, %xmm3
	movdqa	%xmm9, %xmm0
	pcmpgtq	%xmm12, %xmm0
	pshufd	$78, %xmm14, %xmm5
	movdqa	%xmm9, %xmm14
	pshufd	$78, %xmm7, %xmm7
	pblendvb	%xmm0, %xmm12, %xmm14
	pblendvb	%xmm0, %xmm9, %xmm12
	movdqa	%xmm8, %xmm0
	pcmpgtq	%xmm4, %xmm0
	movdqa	%xmm8, %xmm9
	pblendvb	%xmm0, %xmm4, %xmm9
	pblendvb	%xmm0, %xmm8, %xmm4
	movdqa	%xmm10, %xmm0
	pcmpgtq	%xmm11, %xmm0
	movdqa	%xmm10, %xmm8
	movaps	%xmm9, -96(%rbp)
	movdqa	-80(%rbp), %xmm9
	pblendvb	%xmm0, %xmm11, %xmm8
	pblendvb	%xmm0, %xmm10, %xmm11
	movdqa	%xmm6, %xmm0
	pcmpgtq	%xmm2, %xmm0
	movdqa	%xmm6, %xmm10
	movaps	%xmm8, -112(%rbp)
	pshufd	$78, %xmm11, %xmm11
	pblendvb	%xmm0, %xmm2, %xmm10
	pblendvb	%xmm0, %xmm6, %xmm2
	movdqa	-64(%rbp), %xmm6
	movdqa	%xmm7, %xmm0
	movdqa	%xmm10, %xmm8
	movdqa	%xmm7, %xmm10
	pcmpgtq	%xmm6, %xmm0
	pshufd	$78, %xmm8, %xmm8
	pblendvb	%xmm0, %xmm6, %xmm10
	pblendvb	%xmm0, %xmm7, %xmm6
	movdqa	%xmm15, %xmm0
	pcmpgtq	%xmm9, %xmm0
	movdqa	%xmm15, %xmm7
	pshufd	$78, %xmm6, %xmm6
	pblendvb	%xmm0, %xmm9, %xmm7
	pblendvb	%xmm0, %xmm15, %xmm9
	movdqa	%xmm5, %xmm0
	pcmpgtq	%xmm3, %xmm0
	movdqa	%xmm5, %xmm15
	pshufd	$78, %xmm7, %xmm7
	pblendvb	%xmm0, %xmm3, %xmm15
	pblendvb	%xmm0, %xmm5, %xmm3
	movdqa	%xmm13, %xmm0
	pcmpgtq	%xmm1, %xmm0
	movdqa	%xmm13, %xmm5
	movaps	%xmm15, -64(%rbp)
	pshufd	$78, -96(%rbp), %xmm15
	pshufd	$78, %xmm3, %xmm3
	pblendvb	%xmm0, %xmm1, %xmm5
	pblendvb	%xmm0, %xmm13, %xmm1
	movdqa	%xmm15, %xmm0
	pcmpgtq	%xmm14, %xmm0
	pshufd	$78, %xmm12, %xmm13
	movdqa	%xmm15, %xmm12
	pshufd	$78, %xmm5, %xmm5
	pblendvb	%xmm0, %xmm14, %xmm12
	pblendvb	%xmm0, %xmm15, %xmm14
	movdqa	%xmm13, %xmm0
	pcmpgtq	%xmm4, %xmm0
	movaps	%xmm12, -80(%rbp)
	movdqa	%xmm13, %xmm15
	movdqa	-112(%rbp), %xmm12
	pblendvb	%xmm0, %xmm4, %xmm15
	pblendvb	%xmm0, %xmm13, %xmm4
	movdqa	%xmm8, %xmm0
	pcmpgtq	%xmm12, %xmm0
	movdqa	%xmm8, %xmm13
	pblendvb	%xmm0, %xmm12, %xmm13
	pblendvb	%xmm0, %xmm8, %xmm12
	movdqa	%xmm11, %xmm0
	pcmpgtq	%xmm2, %xmm0
	movdqa	%xmm11, %xmm8
	pblendvb	%xmm0, %xmm2, %xmm8
	pblendvb	%xmm0, %xmm11, %xmm2
	movdqa	%xmm7, %xmm0
	pcmpgtq	%xmm10, %xmm0
	movdqa	%xmm7, %xmm11
	pblendvb	%xmm0, %xmm10, %xmm11
	pblendvb	%xmm0, %xmm7, %xmm10
	movdqa	%xmm6, %xmm0
	pcmpgtq	%xmm9, %xmm0
	movdqa	%xmm6, %xmm7
	pblendvb	%xmm0, %xmm9, %xmm7
	pblendvb	%xmm0, %xmm6, %xmm9
	movdqa	%xmm5, %xmm0
	movdqa	-64(%rbp), %xmm6
	movaps	%xmm7, -160(%rbp)
	movdqa	%xmm5, %xmm7
	pcmpgtq	%xmm6, %xmm0
	pblendvb	%xmm0, %xmm6, %xmm7
	pblendvb	%xmm0, %xmm5, %xmm6
	movdqa	%xmm3, %xmm0
	pcmpgtq	%xmm1, %xmm0
	movdqa	%xmm3, %xmm5
	pblendvb	%xmm0, %xmm1, %xmm5
	pblendvb	%xmm0, %xmm3, %xmm1
	movaps	%xmm1, -176(%rbp)
	movdqa	-80(%rbp), %xmm3
	pshufd	$78, %xmm3, %xmm1
	movdqa	%xmm1, %xmm0
	pcmpgtq	%xmm3, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm3
	pshufd	$78, %xmm14, %xmm1
	movdqa	%xmm1, %xmm0
	movaps	%xmm3, -80(%rbp)
	movdqa	-160(%rbp), %xmm3
	pcmpgtq	%xmm14, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm14
	pshufd	$78, %xmm15, %xmm1
	movdqa	%xmm1, %xmm0
	movaps	%xmm14, -64(%rbp)
	movdqa	-176(%rbp), %xmm14
	pcmpgtq	%xmm15, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm15
	pshufd	$78, %xmm4, %xmm1
	movdqa	%xmm1, %xmm0
	movaps	%xmm15, -96(%rbp)
	pcmpgtq	%xmm4, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm4
	pshufd	$78, %xmm13, %xmm1
	movdqa	%xmm1, %xmm0
	movaps	%xmm4, -112(%rbp)
	pcmpgtq	%xmm13, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm13
	pshufd	$78, %xmm12, %xmm1
	movdqa	%xmm1, %xmm0
	movdqa	%xmm13, %xmm15
	pcmpgtq	%xmm12, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm12
	pshufd	$78, %xmm8, %xmm1
	movdqa	%xmm1, %xmm0
	pcmpgtq	%xmm8, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm8
	pshufd	$78, %xmm2, %xmm1
	movdqa	%xmm1, %xmm0
	pcmpgtq	%xmm2, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm2
	pshufd	$78, %xmm11, %xmm1
	movdqa	%xmm1, %xmm0
	pcmpgtq	%xmm11, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm11
	pshufd	$78, %xmm10, %xmm1
	movdqa	%xmm1, %xmm0
	movdqa	%xmm11, %xmm4
	pcmpgtq	%xmm10, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm10
	pshufd	$78, %xmm3, %xmm1
	movdqa	%xmm1, %xmm0
	movdqa	%xmm10, %xmm11
	pcmpgtq	%xmm3, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm3
	movdqa	%xmm3, %xmm10
	pshufd	$78, %xmm9, %xmm3
	movdqa	%xmm3, %xmm0
	pcmpgtq	%xmm9, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm3, %xmm9
	pshufd	$78, %xmm7, %xmm3
	movdqa	%xmm3, %xmm0
	movdqa	%xmm9, %xmm1
	pshufd	$78, %xmm5, %xmm9
	pcmpgtq	%xmm7, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm3, %xmm7
	pshufd	$78, %xmm6, %xmm3
	movdqa	%xmm3, %xmm0
	pcmpgtq	%xmm6, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm3, %xmm6
	movdqa	%xmm9, %xmm0
	pcmpgtq	%xmm5, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm9, %xmm5
	movdqa	%xmm5, %xmm3
	pshufd	$78, %xmm14, %xmm5
	movdqa	%xmm5, %xmm0
	pcmpgtq	%xmm14, %xmm0
	punpckhqdq	%xmm0, %xmm0
	pblendvb	%xmm0, %xmm5, %xmm14
	movdqa	%xmm14, %xmm0
.L289:
	movdqa	-80(%rbp), %xmm5
	movq	-136(%rbp), %rdx
	movups	%xmm5, (%rdx)
	movdqa	-64(%rbp), %xmm5
	movups	%xmm5, (%r15)
	movdqa	-96(%rbp), %xmm5
	movups	%xmm5, (%r14)
	movdqa	-112(%rbp), %xmm5
	movups	%xmm5, 0(%r13)
	movups	%xmm15, (%r12)
	movups	%xmm12, (%rbx)
	movq	-128(%rbp), %rbx
	movups	%xmm8, (%r11)
	movups	%xmm2, (%r10)
	movups	%xmm4, (%r9)
	movups	%xmm11, (%r8)
	movups	%xmm10, (%rdi)
	movups	%xmm1, (%rsi)
	movups	%xmm7, (%rcx)
	movq	-120(%rbp), %rcx
	movups	%xmm6, (%rcx)
	movups	%xmm3, (%rbx)
	movups	%xmm0, (%rax)
	subq	$-128, %rsp
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
.L291:
	.cfi_restore_state
	movdqa	%xmm3, %xmm2
	movdqa	%xmm14, %xmm12
	movdqa	-176(%rbp), %xmm0
	movdqa	-192(%rbp), %xmm3
	movdqa	-224(%rbp), %xmm6
	movdqa	%xmm13, %xmm8
	movdqa	-240(%rbp), %xmm10
	jmp	.L289
	.cfi_endproc
.LFE18793:
	.size	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0, .-_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	.section	.text._ZN3hwy7N_SSSE36detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy7N_SSSE36detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, @function
_ZN3hwy7N_SSSE36detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0:
.LFB18794:
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
	movq	%rdi, %r14
	pushq	%r13
	.cfi_offset 13, -40
	movq	%rcx, %r13
	pushq	%r12
	.cfi_offset 12, -48
	movq	%rsi, %r12
	pushq	%rbx
	subq	$88, %rsp
	.cfi_offset 3, -56
	movq	%rdx, -120(%rbp)
	movaps	%xmm0, -80(%rbp)
	movaps	%xmm1, -64(%rbp)
	movaps	%xmm0, -96(%rbp)
	movaps	%xmm1, -112(%rbp)
	cmpq	$1, %rsi
	jbe	.L315
	movl	$2, %r15d
	xorl	%ebx, %ebx
	jmp	.L300
	.p2align 4,,10
	.p2align 3
.L294:
	movdqa	-80(%rbp), %xmm5
	movmskpd	%xmm1, %edi
	movups	%xmm5, (%r14,%rbx,8)
	call	__popcountdi2@PLT
	cltq
	addq	%rax, %rbx
	leaq	2(%r15), %rax
	cmpq	%r12, %rax
	ja	.L347
	movq	%rax, %r15
.L300:
	movdqu	-16(%r14,%r15,8), %xmm0
	leaq	-2(%r15), %rdx
	leaq	0(,%rbx,8), %rax
	pcmpeqd	-96(%rbp), %xmm0
	pshufd	$177, %xmm0, %xmm1
	pand	%xmm0, %xmm1
	movdqu	-16(%r14,%r15,8), %xmm0
	pcmpeqd	-112(%rbp), %xmm0
	pshufd	$177, %xmm0, %xmm2
	pand	%xmm2, %xmm0
	movdqa	%xmm1, %xmm2
	por	%xmm0, %xmm2
	movmskpd	%xmm2, %ecx
	cmpl	$3, %ecx
	je	.L294
	pcmpeqd	%xmm2, %xmm2
	movq	-120(%rbp), %rsi
	leaq	2(%rbx), %rdi
	pxor	%xmm2, %xmm0
	pandn	%xmm0, %xmm1
	movmskpd	%xmm1, %ecx
	rep bsfl	%ecx, %ecx
	movslq	%ecx, %rcx
	addq	%rdx, %rcx
	movddup	(%r14,%rcx,8), %xmm0
	movaps	%xmm0, (%rsi)
	cmpq	%rdx, %rdi
	ja	.L295
	movq	%rdx, %rcx
	addq	%r14, %rax
	subq	%rbx, %rcx
	leaq	-2(%rcx), %rsi
	movq	%rsi, %rcx
	andq	$-2, %rcx
	addq	%rbx, %rcx
	leaq	16(%r14,%rcx,8), %rcx
	.p2align 4,,10
	.p2align 3
.L296:
	movdqa	-64(%rbp), %xmm4
	addq	$16, %rax
	movups	%xmm4, -16(%rax)
	cmpq	%rcx, %rax
	jne	.L296
	andq	$-2, %rsi
	leaq	(%rsi,%rdi), %rbx
.L295:
	subq	%rbx, %rdx
	movdqa	.LC1(%rip), %xmm2
	movdqa	.LC0(%rip), %xmm1
	leaq	0(,%rbx,8), %rcx
	movq	%rdx, %xmm0
	punpcklqdq	%xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	psubq	%xmm0, %xmm1
	pcmpeqd	%xmm2, %xmm3
	pcmpgtd	%xmm2, %xmm0
	pand	%xmm3, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L297
	movdqa	-64(%rbp), %xmm3
	movq	%xmm3, (%r14,%rbx,8)
.L297:
	movhlps	%xmm0, %xmm3
	movq	%xmm3, %rax
	testq	%rax, %rax
	jne	.L348
.L308:
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
.L348:
	.cfi_restore_state
	movdqa	-64(%rbp), %xmm3
	movhps	%xmm3, 8(%r14,%rcx)
	jmp	.L308
	.p2align 4,,10
	.p2align 3
.L347:
	movq	%r12, %rcx
	leaq	0(,%r15,8), %rsi
	leaq	0(,%rbx,8), %r9
	subq	%r15, %rcx
.L293:
	testq	%rcx, %rcx
	je	.L304
	leaq	0(,%rcx,8), %rdx
	addq	%r14, %rsi
	movq	%r13, %rdi
	movq	%r9, -112(%rbp)
	movq	%rcx, -96(%rbp)
	call	memcpy@PLT
	movq	-96(%rbp), %rcx
	movq	-112(%rbp), %r9
.L304:
	movdqa	.LC1(%rip), %xmm3
	movq	%rcx, %xmm2
	movdqa	-80(%rbp), %xmm5
	movdqa	.LC0(%rip), %xmm1
	punpcklqdq	%xmm2, %xmm2
	movdqa	%xmm3, %xmm4
	pcmpeqd	%xmm2, %xmm4
	movdqa	%xmm1, %xmm0
	psubq	%xmm2, %xmm0
	pcmpgtd	%xmm3, %xmm2
	pand	%xmm4, %xmm0
	por	%xmm2, %xmm0
	movdqa	0(%r13), %xmm2
	pshufd	$245, %xmm0, %xmm0
	pcmpeqd	%xmm2, %xmm5
	pcmpeqd	-64(%rbp), %xmm2
	pshufd	$177, %xmm5, %xmm4
	pand	%xmm0, %xmm4
	pand	%xmm5, %xmm4
	pshufd	$177, %xmm2, %xmm5
	pand	%xmm5, %xmm2
	pcmpeqd	%xmm5, %xmm5
	movdqa	%xmm5, %xmm6
	pxor	%xmm0, %xmm6
	por	%xmm6, %xmm2
	por	%xmm4, %xmm2
	movmskpd	%xmm2, %eax
	cmpl	$3, %eax
	jne	.L349
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L309
	movdqa	-80(%rbp), %xmm5
	movq	%xmm5, (%r14,%r9)
.L309:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rax
	testq	%rax, %rax
	jne	.L350
.L310:
	movmskpd	%xmm4, %edi
	call	__popcountdi2@PLT
	movdqa	.LC0(%rip), %xmm1
	movdqa	.LC1(%rip), %xmm3
	movslq	%eax, %rdx
	addq	%rbx, %rdx
	leaq	2(%rdx), %rax
	cmpq	%rax, %r12
	jb	.L311
	.p2align 4,,10
	.p2align 3
.L312:
	movdqa	-64(%rbp), %xmm7
	movq	%rax, %rdx
	movups	%xmm7, -16(%r14,%rax,8)
	addq	$2, %rax
	cmpq	%rax, %r12
	jnb	.L312
.L311:
	subq	%rdx, %r12
	movdqa	%xmm3, %xmm2
	leaq	0(,%rdx,8), %rcx
	movq	%r12, %xmm0
	punpcklqdq	%xmm0, %xmm0
	pcmpeqd	%xmm0, %xmm2
	psubq	%xmm0, %xmm1
	pcmpgtd	%xmm3, %xmm0
	pand	%xmm2, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L313
	movdqa	-64(%rbp), %xmm3
	movq	%xmm3, (%r14,%rdx,8)
.L313:
	movhlps	%xmm0, %xmm3
	movq	%xmm3, %rax
	testq	%rax, %rax
	je	.L314
	movdqa	-64(%rbp), %xmm3
	movhps	%xmm3, 8(%r14,%rcx)
.L314:
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
.L350:
	.cfi_restore_state
	movdqa	-80(%rbp), %xmm6
	movhps	%xmm6, 8(%r14,%r9)
	jmp	.L310
.L315:
	movq	%rsi, %rcx
	xorl	%r9d, %r9d
	xorl	%esi, %esi
	xorl	%ebx, %ebx
	xorl	%r15d, %r15d
	jmp	.L293
.L349:
	pxor	%xmm5, %xmm2
	leaq	2(%rbx), %rsi
	movmskpd	%xmm2, %eax
	rep bsfl	%eax, %eax
	cltq
	addq	%r15, %rax
	movddup	(%r14,%rax,8), %xmm0
	movq	-120(%rbp), %rax
	movaps	%xmm0, (%rax)
	cmpq	%r15, %rsi
	ja	.L305
	leaq	-2(%r15), %rcx
	leaq	(%r14,%rbx,8), %rax
	subq	%rbx, %rcx
	movq	%rcx, %rdx
	andq	$-2, %rdx
	addq	%rbx, %rdx
	leaq	16(%r14,%rdx,8), %rdx
	.p2align 4,,10
	.p2align 3
.L306:
	movdqa	-64(%rbp), %xmm4
	addq	$16, %rax
	movups	%xmm4, -16(%rax)
	cmpq	%rax, %rdx
	jne	.L306
	andq	$-2, %rcx
	leaq	(%rcx,%rsi), %rbx
	leaq	0(,%rbx,8), %r9
.L305:
	subq	%rbx, %r15
	movdqa	%xmm3, %xmm2
	movq	%r15, %xmm0
	punpcklqdq	%xmm0, %xmm0
	pcmpeqd	%xmm0, %xmm2
	psubq	%xmm0, %xmm1
	pcmpgtd	%xmm3, %xmm0
	pand	%xmm2, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L307
	movdqa	-64(%rbp), %xmm3
	movq	%xmm3, (%r14,%r9)
.L307:
	movhlps	%xmm0, %xmm3
	movq	%xmm3, %rax
	testq	%rax, %rax
	je	.L308
	movdqa	-64(%rbp), %xmm3
	movhps	%xmm3, 8(%r14,%r9)
	jmp	.L308
	.cfi_endproc
.LFE18794:
	.size	_ZN3hwy7N_SSSE36detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0, .-_ZN3hwy7N_SSSE36detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	.section	.text._ZN3hwy7N_SSSE36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy7N_SSSE36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0, @function
_ZN3hwy7N_SSSE36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0:
.LFB18795:
	.cfi_startproc
	cmpq	%rdx, %rsi
	jbe	.L351
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r8
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rax, %rsi
	jbe	.L351
	movq	(%rdi,%rdx,8), %r11
	movq	%r11, %xmm7
	movddup	%xmm7, %xmm3
	movdqa	%xmm3, %xmm6
	jmp	.L354
	.p2align 4,,10
	.p2align 3
.L360:
	movq	%rdx, %rax
.L355:
	cmpq	%rcx, %rsi
	jbe	.L356
	salq	$4, %r8
	movddup	(%rdi,%r8), %xmm0
	movdqa	%xmm0, %xmm2
	psubq	%xmm0, %xmm1
	pcmpeqd	%xmm4, %xmm2
	pcmpgtd	%xmm4, %xmm0
	pand	%xmm2, %xmm1
	por	%xmm0, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movmskpd	%xmm1, %r8d
	andl	$1, %r8d
	jne	.L357
.L356:
	cmpq	%rdx, %rax
	je	.L351
	leaq	(%rdi,%rax,8), %rdx
	movq	(%rdx), %rcx
	movq	%rcx, (%r10)
	movq	%r11, (%rdx)
	cmpq	%rax, %rsi
	jbe	.L363
	movq	%rax, %rdx
.L358:
	leaq	(%rdx,%rdx), %rcx
	leaq	1(%rdx), %r8
	leaq	1(%rcx), %rax
	addq	$2, %rcx
	cmpq	%rsi, %rax
	jnb	.L351
.L354:
	movddup	(%rdi,%rax,8), %xmm2
	movdqa	%xmm2, %xmm5
	movdqa	%xmm3, %xmm0
	leaq	(%rdi,%rdx,8), %r10
	pcmpeqd	%xmm3, %xmm5
	psubq	%xmm2, %xmm0
	movdqa	%xmm6, %xmm4
	movdqa	%xmm3, %xmm1
	pand	%xmm5, %xmm0
	movdqa	%xmm2, %xmm5
	pcmpgtd	%xmm3, %xmm5
	por	%xmm5, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movmskpd	%xmm0, %r9d
	andl	$1, %r9d
	je	.L360
	movdqa	%xmm2, %xmm1
	movdqa	%xmm2, %xmm4
	jmp	.L355
	.p2align 4,,10
	.p2align 3
.L357:
	cmpq	%rcx, %rdx
	je	.L351
	leaq	(%rdi,%rcx,8), %rax
	movq	(%rax), %rdx
	movq	%rdx, (%r10)
	movq	%rcx, %rdx
	movq	%r11, (%rax)
	jmp	.L358
	.p2align 4,,10
	.p2align 3
.L351:
	ret
	.p2align 4,,10
	.p2align 3
.L363:
	ret
	.cfi_endproc
.LFE18795:
	.size	_ZN3hwy7N_SSSE36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0, .-_ZN3hwy7N_SSSE36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	.section	.text._ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0, @function
_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0:
.LFB18796:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %rax
	salq	$3, %rax
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
	subq	$224, %rsp
	leaq	(%r10,%rax), %r9
	leaq	(%r9,%rax), %r8
	movq	%rdi, -232(%rbp)
	movq	%rsi, -192(%rbp)
	movdqu	(%rdi), %xmm9
	leaq	(%r8,%rax), %rdi
	movdqu	(%r15), %xmm6
	leaq	(%rdi,%rax), %rsi
	movdqu	0(%r13), %xmm5
	movdqu	(%r14), %xmm13
	leaq	(%rsi,%rax), %rcx
	movdqa	%xmm9, %xmm14
	movdqu	(%rbx), %xmm3
	movdqu	(%r12), %xmm12
	leaq	(%rcx,%rax), %rdx
	psubq	%xmm6, %xmm14
	movdqu	(%r10), %xmm2
	movdqu	(%r11), %xmm11
	movdqu	(%rdx), %xmm0
	movdqu	(%rsi), %xmm4
	movq	%rdx, -216(%rbp)
	addq	%rax, %rdx
	movdqu	(%rdx), %xmm15
	movdqu	(%r8), %xmm8
	movq	%rdx, -224(%rbp)
	addq	%rdx, %rax
	movaps	%xmm0, -96(%rbp)
	movdqa	%xmm6, %xmm0
	movdqu	(%rdi), %xmm7
	movdqu	(%rcx), %xmm1
	pcmpeqd	%xmm9, %xmm0
	movaps	%xmm15, -112(%rbp)
	movdqu	(%r9), %xmm10
	pand	%xmm14, %xmm0
	movdqa	%xmm6, %xmm14
	pcmpgtd	%xmm9, %xmm14
	por	%xmm14, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm14
	pandn	%xmm6, %xmm14
	pand	%xmm0, %xmm6
	movdqa	%xmm14, %xmm15
	movdqa	%xmm9, %xmm14
	pand	%xmm0, %xmm14
	por	%xmm15, %xmm14
	movdqa	%xmm0, %xmm15
	movdqa	%xmm5, %xmm0
	pcmpeqd	%xmm13, %xmm0
	pandn	%xmm9, %xmm15
	movdqa	%xmm13, %xmm9
	psubq	%xmm5, %xmm9
	por	%xmm15, %xmm6
	pand	%xmm9, %xmm0
	movdqa	%xmm5, %xmm9
	pcmpgtd	%xmm13, %xmm9
	por	%xmm9, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm9
	pandn	%xmm5, %xmm9
	pand	%xmm0, %xmm5
	movdqa	%xmm9, %xmm15
	movdqa	%xmm13, %xmm9
	pand	%xmm0, %xmm9
	por	%xmm15, %xmm9
	movdqa	%xmm0, %xmm15
	movdqa	%xmm3, %xmm0
	pcmpeqd	%xmm12, %xmm0
	pandn	%xmm13, %xmm15
	movdqa	%xmm12, %xmm13
	psubq	%xmm3, %xmm13
	por	%xmm15, %xmm5
	pand	%xmm13, %xmm0
	movdqa	%xmm3, %xmm13
	pcmpgtd	%xmm12, %xmm13
	por	%xmm13, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm13
	pandn	%xmm3, %xmm13
	pand	%xmm0, %xmm3
	movdqa	%xmm13, %xmm15
	movdqa	%xmm12, %xmm13
	pand	%xmm0, %xmm13
	por	%xmm15, %xmm13
	movdqa	%xmm0, %xmm15
	movdqa	%xmm2, %xmm0
	pandn	%xmm12, %xmm15
	pcmpeqd	%xmm11, %xmm0
	por	%xmm15, %xmm3
	movaps	%xmm3, -64(%rbp)
	movdqa	%xmm11, %xmm3
	psubq	%xmm2, %xmm3
	pand	%xmm3, %xmm0
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm11, %xmm3
	por	%xmm3, %xmm0
	movdqa	%xmm11, %xmm3
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm12
	pand	%xmm0, %xmm3
	pandn	%xmm2, %xmm12
	pand	%xmm0, %xmm2
	por	%xmm12, %xmm3
	movdqa	%xmm0, %xmm12
	movdqa	%xmm8, %xmm0
	pcmpeqd	%xmm10, %xmm0
	pandn	%xmm11, %xmm12
	movdqa	%xmm10, %xmm11
	psubq	%xmm8, %xmm11
	por	%xmm12, %xmm2
	movdqa	%xmm10, %xmm12
	pand	%xmm11, %xmm0
	movdqa	%xmm8, %xmm11
	pcmpgtd	%xmm10, %xmm11
	por	%xmm11, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm11
	pand	%xmm0, %xmm12
	pandn	%xmm8, %xmm11
	pand	%xmm0, %xmm8
	por	%xmm11, %xmm12
	movdqa	%xmm0, %xmm11
	movdqa	%xmm4, %xmm0
	pandn	%xmm10, %xmm11
	pcmpeqd	%xmm7, %xmm0
	por	%xmm11, %xmm8
	movaps	%xmm8, -80(%rbp)
	movdqa	%xmm7, %xmm8
	movdqa	-96(%rbp), %xmm10
	movdqa	-112(%rbp), %xmm15
	psubq	%xmm4, %xmm8
	pand	%xmm8, %xmm0
	movdqa	%xmm4, %xmm8
	pcmpgtd	%xmm7, %xmm8
	por	%xmm8, %xmm0
	movdqa	%xmm7, %xmm8
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm11
	pand	%xmm0, %xmm8
	pandn	%xmm4, %xmm11
	pand	%xmm0, %xmm4
	por	%xmm11, %xmm8
	movdqa	%xmm0, %xmm11
	movdqa	%xmm10, %xmm0
	pcmpeqd	%xmm1, %xmm0
	pandn	%xmm7, %xmm11
	movdqa	%xmm1, %xmm7
	psubq	%xmm10, %xmm7
	por	%xmm11, %xmm4
	movdqa	%xmm1, %xmm11
	pand	%xmm7, %xmm0
	movdqa	%xmm10, %xmm7
	pcmpgtd	%xmm1, %xmm7
	por	%xmm7, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm7
	pand	%xmm0, %xmm11
	pandn	%xmm10, %xmm7
	por	%xmm7, %xmm11
	movdqa	%xmm0, %xmm7
	pand	%xmm10, %xmm0
	movdqu	(%rax), %xmm10
	pandn	%xmm1, %xmm7
	movdqu	(%rax), %xmm1
	por	%xmm7, %xmm0
	movdqa	%xmm15, %xmm7
	psubq	%xmm1, %xmm7
	movdqu	(%rax), %xmm1
	movaps	%xmm0, -96(%rbp)
	movdqa	%xmm15, %xmm0
	pcmpeqd	%xmm15, %xmm1
	pand	%xmm7, %xmm1
	movdqu	(%rax), %xmm7
	pcmpgtd	%xmm15, %xmm7
	por	%xmm7, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm7
	pand	%xmm1, %xmm0
	pandn	%xmm10, %xmm7
	por	%xmm7, %xmm0
	movdqa	%xmm1, %xmm7
	pand	%xmm10, %xmm1
	pandn	%xmm15, %xmm7
	movdqa	%xmm14, %xmm10
	movdqa	%xmm14, %xmm15
	por	%xmm7, %xmm1
	movdqa	%xmm9, %xmm7
	psubq	%xmm9, %xmm10
	pcmpeqd	%xmm14, %xmm7
	pand	%xmm10, %xmm7
	movdqa	%xmm9, %xmm10
	pcmpgtd	%xmm14, %xmm10
	por	%xmm10, %xmm7
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm10
	pand	%xmm7, %xmm15
	pandn	%xmm9, %xmm10
	pand	%xmm7, %xmm9
	por	%xmm15, %xmm10
	movdqa	%xmm7, %xmm15
	movdqa	%xmm5, %xmm7
	pcmpeqd	%xmm6, %xmm7
	pandn	%xmm14, %xmm15
	movdqa	%xmm6, %xmm14
	por	%xmm9, %xmm15
	movdqa	%xmm6, %xmm9
	psubq	%xmm5, %xmm9
	pand	%xmm9, %xmm7
	movdqa	%xmm5, %xmm9
	pcmpgtd	%xmm6, %xmm9
	por	%xmm9, %xmm7
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm9
	pand	%xmm7, %xmm14
	pandn	%xmm5, %xmm9
	pand	%xmm7, %xmm5
	por	%xmm14, %xmm9
	movdqa	%xmm7, %xmm14
	pandn	%xmm6, %xmm14
	movdqa	%xmm3, %xmm6
	por	%xmm14, %xmm5
	pcmpeqd	%xmm13, %xmm6
	movdqa	-64(%rbp), %xmm14
	movaps	%xmm5, -112(%rbp)
	movdqa	%xmm13, %xmm5
	psubq	%xmm3, %xmm5
	pand	%xmm5, %xmm6
	movdqa	%xmm3, %xmm5
	pcmpgtd	%xmm13, %xmm5
	por	%xmm5, %xmm6
	movdqa	%xmm13, %xmm5
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm7
	pand	%xmm6, %xmm5
	pandn	%xmm3, %xmm7
	pand	%xmm6, %xmm3
	por	%xmm7, %xmm5
	movdqa	%xmm6, %xmm7
	movdqa	%xmm14, %xmm6
	pandn	%xmm13, %xmm7
	pcmpeqd	%xmm2, %xmm6
	por	%xmm7, %xmm3
	movdqa	%xmm14, %xmm7
	psubq	%xmm2, %xmm7
	pand	%xmm7, %xmm6
	movdqa	%xmm2, %xmm7
	pcmpgtd	%xmm14, %xmm7
	por	%xmm7, %xmm6
	movdqa	%xmm14, %xmm7
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm13
	pand	%xmm6, %xmm7
	pandn	%xmm2, %xmm13
	pand	%xmm6, %xmm2
	por	%xmm13, %xmm7
	movdqa	%xmm6, %xmm13
	movdqa	%xmm8, %xmm6
	pandn	%xmm14, %xmm13
	pcmpeqd	%xmm12, %xmm6
	movdqa	%xmm12, %xmm14
	por	%xmm13, %xmm2
	movdqa	%xmm12, %xmm13
	psubq	%xmm8, %xmm13
	pand	%xmm13, %xmm6
	movdqa	%xmm8, %xmm13
	pcmpgtd	%xmm12, %xmm13
	por	%xmm13, %xmm6
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm13
	pand	%xmm6, %xmm14
	pandn	%xmm8, %xmm13
	pand	%xmm6, %xmm8
	por	%xmm14, %xmm13
	movdqa	%xmm6, %xmm14
	movdqa	%xmm8, %xmm6
	pandn	%xmm12, %xmm14
	por	%xmm14, %xmm6
	movdqa	-80(%rbp), %xmm14
	movaps	%xmm6, -128(%rbp)
	movdqa	%xmm14, %xmm6
	movdqa	%xmm14, %xmm8
	movdqa	%xmm14, %xmm12
	pcmpeqd	%xmm4, %xmm6
	psubq	%xmm4, %xmm8
	pand	%xmm8, %xmm6
	movdqa	%xmm4, %xmm8
	pcmpgtd	%xmm14, %xmm8
	por	%xmm8, %xmm6
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm8
	pand	%xmm6, %xmm12
	pandn	%xmm4, %xmm8
	pand	%xmm6, %xmm4
	por	%xmm12, %xmm8
	movdqa	%xmm6, %xmm12
	movdqa	%xmm0, %xmm6
	pandn	%xmm14, %xmm12
	pcmpeqd	%xmm11, %xmm6
	movdqa	-96(%rbp), %xmm14
	por	%xmm12, %xmm4
	movaps	%xmm4, -80(%rbp)
	movdqa	%xmm11, %xmm4
	psubq	%xmm0, %xmm4
	pand	%xmm4, %xmm6
	movdqa	%xmm0, %xmm4
	pcmpgtd	%xmm11, %xmm4
	por	%xmm4, %xmm6
	movdqa	%xmm11, %xmm4
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm12
	pand	%xmm6, %xmm4
	pandn	%xmm0, %xmm12
	pand	%xmm6, %xmm0
	por	%xmm12, %xmm4
	movdqa	%xmm6, %xmm12
	movdqa	%xmm14, %xmm6
	pandn	%xmm11, %xmm12
	movdqa	%xmm14, %xmm11
	psubq	%xmm1, %xmm6
	pcmpeqd	%xmm1, %xmm11
	por	%xmm12, %xmm0
	pand	%xmm6, %xmm11
	movdqa	%xmm1, %xmm6
	pcmpgtd	%xmm14, %xmm6
	por	%xmm6, %xmm11
	movdqa	%xmm14, %xmm6
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm6
	pandn	%xmm1, %xmm12
	pand	%xmm11, %xmm1
	por	%xmm12, %xmm6
	movdqa	%xmm11, %xmm12
	movdqa	%xmm5, %xmm11
	pandn	%xmm14, %xmm12
	pcmpeqd	%xmm10, %xmm11
	movdqa	%xmm10, %xmm14
	por	%xmm12, %xmm1
	movdqa	%xmm10, %xmm12
	psubq	%xmm5, %xmm12
	pand	%xmm12, %xmm11
	movdqa	%xmm5, %xmm12
	pcmpgtd	%xmm10, %xmm12
	por	%xmm12, %xmm11
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm14
	pandn	%xmm5, %xmm12
	pand	%xmm11, %xmm5
	por	%xmm14, %xmm12
	movdqa	%xmm11, %xmm14
	movdqa	%xmm9, %xmm11
	pandn	%xmm10, %xmm14
	movdqa	%xmm9, %xmm10
	por	%xmm5, %xmm14
	movdqa	%xmm7, %xmm5
	psubq	%xmm7, %xmm10
	pcmpeqd	%xmm9, %xmm5
	pand	%xmm10, %xmm5
	movdqa	%xmm7, %xmm10
	pcmpgtd	%xmm9, %xmm10
	por	%xmm10, %xmm5
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm10
	pand	%xmm5, %xmm11
	pandn	%xmm7, %xmm10
	pand	%xmm5, %xmm7
	por	%xmm11, %xmm10
	movdqa	%xmm5, %xmm11
	movdqa	%xmm3, %xmm5
	pandn	%xmm9, %xmm11
	pcmpeqd	%xmm15, %xmm5
	movdqa	%xmm15, %xmm9
	por	%xmm11, %xmm7
	movaps	%xmm7, -96(%rbp)
	movdqa	%xmm15, %xmm7
	psubq	%xmm3, %xmm7
	pand	%xmm7, %xmm5
	movdqa	%xmm3, %xmm7
	pcmpgtd	%xmm15, %xmm7
	por	%xmm7, %xmm5
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm7
	pand	%xmm5, %xmm9
	pandn	%xmm3, %xmm7
	pand	%xmm5, %xmm3
	por	%xmm9, %xmm7
	movdqa	%xmm5, %xmm9
	pandn	%xmm15, %xmm9
	movdqa	-112(%rbp), %xmm15
	por	%xmm9, %xmm3
	movaps	%xmm3, -144(%rbp)
	movdqa	%xmm15, %xmm3
	movdqa	%xmm15, %xmm5
	movdqa	%xmm15, %xmm9
	pcmpeqd	%xmm2, %xmm3
	psubq	%xmm2, %xmm5
	pand	%xmm5, %xmm3
	movdqa	%xmm2, %xmm5
	pcmpgtd	%xmm15, %xmm5
	por	%xmm5, %xmm3
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm5
	pand	%xmm3, %xmm9
	pandn	%xmm2, %xmm5
	pand	%xmm3, %xmm2
	por	%xmm9, %xmm5
	movdqa	%xmm3, %xmm9
	movdqa	%xmm13, %xmm3
	pandn	%xmm15, %xmm9
	psubq	%xmm4, %xmm3
	movdqa	-128(%rbp), %xmm15
	por	%xmm9, %xmm2
	movaps	%xmm2, -64(%rbp)
	movdqa	%xmm4, %xmm2
	pcmpeqd	%xmm13, %xmm2
	pand	%xmm3, %xmm2
	movdqa	%xmm4, %xmm3
	pcmpgtd	%xmm13, %xmm3
	por	%xmm3, %xmm2
	movdqa	%xmm13, %xmm3
	pshufd	$245, %xmm2, %xmm2
	movdqa	%xmm2, %xmm9
	pand	%xmm2, %xmm3
	pandn	%xmm4, %xmm9
	pand	%xmm2, %xmm4
	por	%xmm9, %xmm3
	movdqa	%xmm2, %xmm9
	movdqa	%xmm6, %xmm2
	pandn	%xmm13, %xmm9
	pcmpeqd	%xmm8, %xmm2
	por	%xmm9, %xmm4
	movdqa	%xmm8, %xmm9
	psubq	%xmm6, %xmm9
	pand	%xmm9, %xmm2
	movdqa	%xmm6, %xmm9
	pcmpgtd	%xmm8, %xmm9
	por	%xmm9, %xmm2
	movdqa	%xmm8, %xmm9
	pshufd	$245, %xmm2, %xmm2
	movdqa	%xmm2, %xmm11
	pand	%xmm2, %xmm9
	pandn	%xmm6, %xmm11
	pand	%xmm2, %xmm6
	por	%xmm11, %xmm9
	movdqa	%xmm2, %xmm11
	movdqa	%xmm15, %xmm2
	pandn	%xmm8, %xmm11
	movdqa	%xmm15, %xmm8
	psubq	%xmm0, %xmm2
	pcmpeqd	%xmm0, %xmm8
	por	%xmm11, %xmm6
	pand	%xmm2, %xmm8
	movdqa	%xmm0, %xmm2
	pcmpgtd	%xmm15, %xmm2
	por	%xmm2, %xmm8
	movdqa	%xmm15, %xmm2
	pshufd	$245, %xmm8, %xmm8
	movdqa	%xmm8, %xmm11
	pand	%xmm8, %xmm2
	pandn	%xmm0, %xmm11
	pand	%xmm8, %xmm0
	por	%xmm11, %xmm2
	movdqa	%xmm8, %xmm11
	pandn	%xmm15, %xmm11
	movdqa	-80(%rbp), %xmm15
	por	%xmm11, %xmm0
	movdqa	%xmm15, %xmm11
	movdqa	%xmm15, %xmm8
	pcmpeqd	%xmm1, %xmm11
	psubq	%xmm1, %xmm8
	pand	%xmm8, %xmm11
	movdqa	%xmm1, %xmm8
	pcmpgtd	%xmm15, %xmm8
	por	%xmm8, %xmm11
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm8
	pandn	%xmm1, %xmm8
	pand	%xmm11, %xmm1
	movdqa	%xmm8, %xmm13
	movdqa	%xmm15, %xmm8
	pand	%xmm11, %xmm8
	por	%xmm13, %xmm8
	movdqa	%xmm11, %xmm13
	movdqa	%xmm3, %xmm11
	pcmpeqd	%xmm12, %xmm11
	pandn	%xmm15, %xmm13
	movdqa	%xmm12, %xmm15
	psubq	%xmm3, %xmm15
	por	%xmm13, %xmm1
	pand	%xmm15, %xmm11
	movdqa	%xmm3, %xmm15
	pcmpgtd	%xmm12, %xmm15
	por	%xmm15, %xmm11
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm15
	pandn	%xmm3, %xmm15
	pand	%xmm11, %xmm3
	movdqa	%xmm15, %xmm13
	movdqa	%xmm12, %xmm15
	pand	%xmm11, %xmm15
	por	%xmm13, %xmm15
	movaps	%xmm15, -112(%rbp)
	movdqa	%xmm11, %xmm15
	movdqa	%xmm10, %xmm11
	pandn	%xmm12, %xmm15
	psubq	%xmm9, %xmm11
	movdqa	%xmm10, %xmm12
	movdqa	%xmm15, %xmm13
	movdqa	%xmm3, %xmm15
	movdqa	%xmm9, %xmm3
	pcmpeqd	%xmm10, %xmm3
	por	%xmm13, %xmm15
	movdqa	-96(%rbp), %xmm13
	pand	%xmm11, %xmm3
	movdqa	%xmm9, %xmm11
	pcmpgtd	%xmm10, %xmm11
	por	%xmm11, %xmm3
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm11
	pand	%xmm3, %xmm12
	pandn	%xmm9, %xmm11
	pand	%xmm3, %xmm9
	por	%xmm11, %xmm12
	movdqa	%xmm3, %xmm11
	movdqa	%xmm7, %xmm3
	pandn	%xmm10, %xmm11
	movdqa	%xmm2, %xmm10
	psubq	%xmm2, %xmm3
	movaps	%xmm12, -80(%rbp)
	pcmpeqd	%xmm7, %xmm10
	por	%xmm11, %xmm9
	pand	%xmm3, %xmm10
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm7, %xmm3
	por	%xmm3, %xmm10
	movdqa	%xmm7, %xmm3
	pshufd	$245, %xmm10, %xmm10
	movdqa	%xmm10, %xmm11
	pand	%xmm10, %xmm3
	pandn	%xmm2, %xmm11
	pand	%xmm10, %xmm2
	por	%xmm11, %xmm3
	movdqa	%xmm10, %xmm11
	movdqa	%xmm8, %xmm10
	pcmpeqd	%xmm5, %xmm10
	pandn	%xmm7, %xmm11
	movdqa	%xmm5, %xmm7
	psubq	%xmm8, %xmm7
	por	%xmm11, %xmm2
	pand	%xmm7, %xmm10
	movdqa	%xmm8, %xmm7
	pcmpgtd	%xmm5, %xmm7
	por	%xmm7, %xmm10
	movdqa	%xmm5, %xmm7
	pshufd	$245, %xmm10, %xmm10
	movdqa	%xmm10, %xmm11
	pand	%xmm10, %xmm7
	pandn	%xmm8, %xmm11
	pand	%xmm10, %xmm8
	por	%xmm11, %xmm7
	movdqa	%xmm10, %xmm11
	movdqa	%xmm4, %xmm10
	pcmpeqd	%xmm14, %xmm10
	pandn	%xmm5, %xmm11
	movdqa	%xmm14, %xmm5
	psubq	%xmm4, %xmm5
	por	%xmm11, %xmm8
	pand	%xmm5, %xmm10
	movdqa	%xmm4, %xmm5
	pcmpgtd	%xmm14, %xmm5
	por	%xmm5, %xmm10
	movdqa	%xmm14, %xmm5
	pshufd	$245, %xmm10, %xmm10
	movdqa	%xmm10, %xmm11
	pand	%xmm10, %xmm5
	pandn	%xmm4, %xmm11
	pand	%xmm10, %xmm4
	por	%xmm11, %xmm5
	movdqa	%xmm10, %xmm11
	movdqa	%xmm13, %xmm10
	pandn	%xmm14, %xmm11
	psubq	%xmm6, %xmm10
	movdqa	-144(%rbp), %xmm14
	por	%xmm11, %xmm4
	movdqa	%xmm13, %xmm11
	pcmpeqd	%xmm6, %xmm11
	pand	%xmm10, %xmm11
	movdqa	%xmm6, %xmm10
	pcmpgtd	%xmm13, %xmm10
	por	%xmm10, %xmm11
	movdqa	%xmm13, %xmm10
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm12
	pand	%xmm11, %xmm10
	pandn	%xmm6, %xmm12
	pand	%xmm11, %xmm6
	por	%xmm12, %xmm10
	movdqa	%xmm11, %xmm12
	movdqa	%xmm14, %xmm11
	pandn	%xmm13, %xmm12
	pcmpeqd	%xmm0, %xmm11
	por	%xmm12, %xmm6
	movdqa	%xmm14, %xmm12
	psubq	%xmm0, %xmm12
	pand	%xmm12, %xmm11
	movdqa	%xmm0, %xmm12
	pcmpgtd	%xmm14, %xmm12
	por	%xmm12, %xmm11
	movdqa	%xmm14, %xmm12
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm13
	pand	%xmm11, %xmm12
	pandn	%xmm0, %xmm13
	pand	%xmm11, %xmm0
	por	%xmm13, %xmm12
	movdqa	%xmm11, %xmm13
	pandn	%xmm14, %xmm13
	movdqa	-64(%rbp), %xmm14
	por	%xmm13, %xmm0
	movdqa	%xmm14, %xmm11
	movdqa	%xmm14, %xmm13
	movaps	%xmm0, -96(%rbp)
	pcmpeqd	%xmm1, %xmm11
	psubq	%xmm1, %xmm13
	pand	%xmm13, %xmm11
	movdqa	%xmm1, %xmm13
	pcmpgtd	%xmm14, %xmm13
	por	%xmm13, %xmm11
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm13
	movdqa	%xmm11, %xmm0
	pandn	-64(%rbp), %xmm0
	pandn	%xmm1, %xmm13
	pand	%xmm11, %xmm1
	pand	%xmm11, %xmm14
	por	%xmm0, %xmm1
	movdqa	%xmm10, %xmm11
	por	%xmm14, %xmm13
	movaps	%xmm1, -144(%rbp)
	movdqa	%xmm2, %xmm1
	psubq	%xmm2, %xmm11
	movdqa	%xmm7, %xmm0
	pcmpeqd	%xmm10, %xmm1
	psubq	%xmm4, %xmm0
	pand	%xmm11, %xmm1
	movdqa	%xmm2, %xmm11
	pcmpgtd	%xmm10, %xmm11
	por	%xmm11, %xmm1
	movdqa	%xmm10, %xmm11
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm14
	pand	%xmm1, %xmm11
	pandn	%xmm2, %xmm14
	pand	%xmm1, %xmm2
	por	%xmm14, %xmm11
	movdqa	%xmm1, %xmm14
	movdqa	%xmm12, %xmm1
	pandn	%xmm10, %xmm14
	movdqa	%xmm9, %xmm10
	psubq	%xmm9, %xmm1
	pcmpeqd	%xmm12, %xmm10
	por	%xmm14, %xmm2
	pand	%xmm1, %xmm10
	movdqa	%xmm9, %xmm1
	pcmpgtd	%xmm12, %xmm1
	por	%xmm1, %xmm10
	movdqa	%xmm12, %xmm1
	pshufd	$245, %xmm10, %xmm10
	movdqa	%xmm10, %xmm14
	pand	%xmm10, %xmm1
	pandn	%xmm9, %xmm14
	pand	%xmm10, %xmm9
	por	%xmm14, %xmm1
	movdqa	%xmm10, %xmm14
	movdqa	%xmm4, %xmm10
	pcmpeqd	%xmm7, %xmm10
	pandn	%xmm12, %xmm14
	por	%xmm14, %xmm9
	movdqa	%xmm7, %xmm14
	pand	%xmm0, %xmm10
	movdqa	%xmm4, %xmm0
	pcmpgtd	%xmm7, %xmm0
	por	%xmm0, %xmm10
	pshufd	$245, %xmm10, %xmm10
	movdqa	%xmm10, %xmm0
	pand	%xmm10, %xmm14
	pandn	%xmm4, %xmm0
	pand	%xmm10, %xmm4
	por	%xmm0, %xmm14
	movdqa	%xmm10, %xmm0
	pandn	%xmm7, %xmm0
	movdqa	%xmm13, %xmm7
	por	%xmm0, %xmm4
	psubq	%xmm8, %xmm7
	movaps	%xmm4, -64(%rbp)
	movdqa	%xmm8, %xmm4
	pcmpeqd	%xmm13, %xmm4
	pand	%xmm7, %xmm4
	movdqa	%xmm8, %xmm7
	pcmpgtd	%xmm13, %xmm7
	por	%xmm7, %xmm4
	movdqa	%xmm13, %xmm7
	pshufd	$245, %xmm4, %xmm4
	movdqa	%xmm4, %xmm10
	pand	%xmm4, %xmm7
	pandn	%xmm8, %xmm10
	pand	%xmm4, %xmm8
	por	%xmm10, %xmm7
	movdqa	%xmm4, %xmm10
	pandn	%xmm13, %xmm10
	movdqa	-96(%rbp), %xmm13
	por	%xmm10, %xmm8
	movdqa	%xmm6, %xmm10
	movdqa	%xmm13, %xmm4
	psubq	%xmm13, %xmm10
	pcmpeqd	%xmm6, %xmm4
	pand	%xmm10, %xmm4
	movdqa	%xmm13, %xmm10
	pcmpgtd	%xmm6, %xmm10
	por	%xmm10, %xmm4
	movdqa	%xmm6, %xmm10
	pshufd	$245, %xmm4, %xmm4
	movdqa	%xmm4, %xmm0
	pand	%xmm4, %xmm10
	pandn	%xmm13, %xmm0
	por	%xmm0, %xmm10
	movdqa	%xmm4, %xmm0
	pandn	%xmm6, %xmm0
	movdqa	%xmm15, %xmm6
	movdqa	%xmm0, %xmm12
	pcmpeqd	%xmm5, %xmm6
	movdqa	%xmm13, %xmm0
	movdqa	-80(%rbp), %xmm13
	pand	%xmm4, %xmm0
	movdqa	%xmm5, %xmm4
	psubq	%xmm15, %xmm4
	por	%xmm12, %xmm0
	pand	%xmm4, %xmm6
	movdqa	%xmm15, %xmm4
	pcmpgtd	%xmm5, %xmm4
	por	%xmm4, %xmm6
	movdqa	%xmm5, %xmm4
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm12
	pand	%xmm6, %xmm4
	pandn	%xmm15, %xmm12
	pand	%xmm6, %xmm15
	por	%xmm12, %xmm4
	movdqa	%xmm6, %xmm12
	movdqa	%xmm13, %xmm6
	pandn	%xmm5, %xmm12
	movdqa	%xmm13, %xmm5
	psubq	%xmm3, %xmm6
	pcmpeqd	%xmm3, %xmm5
	por	%xmm12, %xmm15
	pand	%xmm6, %xmm5
	movdqa	%xmm3, %xmm6
	pcmpgtd	%xmm13, %xmm6
	por	%xmm6, %xmm5
	movdqa	%xmm13, %xmm6
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm12
	pand	%xmm5, %xmm6
	pandn	%xmm3, %xmm12
	pand	%xmm5, %xmm3
	por	%xmm12, %xmm6
	movdqa	%xmm5, %xmm12
	movdqa	%xmm4, %xmm5
	pandn	%xmm13, %xmm12
	pcmpeqd	%xmm6, %xmm5
	movdqa	%xmm6, %xmm13
	por	%xmm12, %xmm3
	movdqa	%xmm6, %xmm12
	psubq	%xmm4, %xmm12
	pand	%xmm12, %xmm5
	movdqa	%xmm4, %xmm12
	pcmpgtd	%xmm6, %xmm12
	por	%xmm12, %xmm5
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm12
	pand	%xmm5, %xmm13
	pandn	%xmm4, %xmm12
	pand	%xmm5, %xmm4
	por	%xmm12, %xmm13
	movdqa	%xmm5, %xmm12
	movdqa	%xmm10, %xmm5
	pcmpeqd	%xmm7, %xmm5
	pandn	%xmm6, %xmm12
	movdqa	%xmm7, %xmm6
	movaps	%xmm13, -128(%rbp)
	psubq	%xmm10, %xmm6
	por	%xmm12, %xmm4
	pand	%xmm6, %xmm5
	movdqa	%xmm10, %xmm6
	pcmpgtd	%xmm7, %xmm6
	por	%xmm6, %xmm5
	movdqa	%xmm7, %xmm6
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm12
	pand	%xmm5, %xmm6
	pandn	%xmm10, %xmm12
	pand	%xmm5, %xmm10
	por	%xmm12, %xmm6
	movdqa	%xmm5, %xmm12
	movdqa	%xmm15, %xmm5
	pcmpeqd	%xmm3, %xmm5
	pandn	%xmm7, %xmm12
	movdqa	%xmm3, %xmm7
	psubq	%xmm15, %xmm7
	por	%xmm12, %xmm10
	pand	%xmm7, %xmm5
	movdqa	%xmm15, %xmm7
	pcmpgtd	%xmm3, %xmm7
	por	%xmm7, %xmm5
	movdqa	%xmm3, %xmm7
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm12
	pand	%xmm5, %xmm7
	pandn	%xmm15, %xmm12
	pand	%xmm5, %xmm15
	por	%xmm12, %xmm7
	movdqa	%xmm5, %xmm12
	movdqa	%xmm15, %xmm5
	pandn	%xmm3, %xmm12
	movdqa	%xmm8, %xmm3
	por	%xmm12, %xmm5
	movdqa	%xmm0, %xmm12
	psubq	%xmm0, %xmm3
	pcmpeqd	%xmm8, %xmm12
	pand	%xmm3, %xmm12
	movdqa	%xmm0, %xmm3
	pcmpgtd	%xmm8, %xmm3
	por	%xmm3, %xmm12
	movdqa	%xmm8, %xmm3
	pshufd	$245, %xmm12, %xmm12
	movdqa	%xmm12, %xmm13
	pand	%xmm12, %xmm3
	pandn	%xmm0, %xmm13
	pand	%xmm12, %xmm0
	por	%xmm13, %xmm3
	movdqa	%xmm12, %xmm13
	movdqa	%xmm7, %xmm12
	pandn	%xmm8, %xmm13
	movdqa	%xmm7, %xmm8
	por	%xmm13, %xmm0
	psubq	%xmm4, %xmm8
	movaps	%xmm0, -160(%rbp)
	movdqa	%xmm4, %xmm0
	pcmpeqd	%xmm7, %xmm0
	pand	%xmm8, %xmm0
	movdqa	%xmm4, %xmm8
	pcmpgtd	%xmm7, %xmm8
	por	%xmm8, %xmm0
	pshufd	$245, %xmm0, %xmm0
	pand	%xmm0, %xmm12
	movdqa	%xmm0, %xmm8
	pandn	%xmm4, %xmm8
	movdqa	%xmm12, %xmm15
	pand	%xmm0, %xmm4
	por	%xmm8, %xmm15
	movdqa	%xmm0, %xmm8
	movdqa	%xmm11, %xmm0
	pandn	%xmm7, %xmm8
	movdqa	%xmm1, %xmm7
	psubq	%xmm1, %xmm0
	pcmpeqd	%xmm11, %xmm7
	por	%xmm8, %xmm4
	pand	%xmm0, %xmm7
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm11, %xmm0
	por	%xmm0, %xmm7
	movdqa	%xmm11, %xmm0
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm8
	pand	%xmm7, %xmm0
	pandn	%xmm1, %xmm8
	pand	%xmm7, %xmm1
	por	%xmm8, %xmm0
	movdqa	%xmm7, %xmm8
	movdqa	%xmm9, %xmm7
	pandn	%xmm11, %xmm8
	psubq	%xmm2, %xmm7
	por	%xmm8, %xmm1
	movdqa	%xmm1, %xmm11
	movdqa	%xmm2, %xmm1
	pcmpeqd	%xmm9, %xmm1
	pand	%xmm7, %xmm1
	movdqa	%xmm2, %xmm7
	pcmpgtd	%xmm9, %xmm7
	por	%xmm7, %xmm1
	movdqa	%xmm9, %xmm7
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm8
	pand	%xmm1, %xmm7
	pandn	%xmm2, %xmm8
	pand	%xmm1, %xmm2
	por	%xmm8, %xmm7
	movdqa	%xmm1, %xmm8
	movdqa	%xmm10, %xmm1
	pandn	%xmm9, %xmm8
	pcmpeqd	%xmm3, %xmm1
	por	%xmm8, %xmm2
	movdqa	%xmm3, %xmm8
	psubq	%xmm10, %xmm8
	pand	%xmm8, %xmm1
	movdqa	%xmm10, %xmm8
	pcmpgtd	%xmm3, %xmm8
	por	%xmm8, %xmm1
	movdqa	%xmm3, %xmm8
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm13
	pand	%xmm1, %xmm8
	pandn	%xmm10, %xmm13
	pand	%xmm1, %xmm10
	por	%xmm13, %xmm8
	movdqa	%xmm1, %xmm13
	movdqa	%xmm14, %xmm1
	pandn	%xmm3, %xmm13
	movdqa	%xmm5, %xmm3
	psubq	%xmm5, %xmm1
	pcmpeqd	%xmm14, %xmm3
	por	%xmm13, %xmm10
	movdqa	%xmm14, %xmm13
	movaps	%xmm10, -176(%rbp)
	movdqa	-64(%rbp), %xmm9
	movdqa	%xmm6, %xmm10
	cmpq	$1, -192(%rbp)
	pand	%xmm1, %xmm3
	movdqa	%xmm5, %xmm1
	pcmpgtd	%xmm14, %xmm1
	por	%xmm1, %xmm3
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm1
	pand	%xmm3, %xmm13
	pandn	%xmm5, %xmm1
	pand	%xmm3, %xmm5
	por	%xmm1, %xmm13
	movdqa	%xmm3, %xmm1
	movdqa	%xmm6, %xmm3
	pandn	%xmm14, %xmm1
	psubq	%xmm9, %xmm3
	movdqa	%xmm13, %xmm12
	por	%xmm1, %xmm5
	movdqa	%xmm9, %xmm1
	pcmpeqd	%xmm6, %xmm1
	pand	%xmm3, %xmm1
	movdqa	%xmm9, %xmm3
	pcmpgtd	%xmm6, %xmm3
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm1, %xmm10
	pandn	%xmm9, %xmm3
	por	%xmm3, %xmm10
	movdqa	%xmm1, %xmm3
	pand	%xmm9, %xmm1
	movdqa	%xmm0, %xmm9
	pandn	%xmm6, %xmm3
	pcmpeqd	%xmm13, %xmm9
	por	%xmm3, %xmm1
	movdqa	%xmm13, %xmm3
	psubq	%xmm0, %xmm3
	pand	%xmm3, %xmm9
	movdqa	%xmm0, %xmm3
	pcmpgtd	%xmm13, %xmm3
	por	%xmm3, %xmm9
	pshufd	$245, %xmm9, %xmm9
	movdqa	%xmm9, %xmm3
	pand	%xmm9, %xmm12
	pandn	%xmm0, %xmm3
	por	%xmm3, %xmm12
	movdqa	%xmm9, %xmm3
	pand	%xmm0, %xmm9
	pandn	%xmm13, %xmm3
	movdqa	%xmm11, %xmm0
	por	%xmm3, %xmm9
	movdqa	%xmm5, %xmm3
	psubq	%xmm5, %xmm0
	pcmpeqd	%xmm11, %xmm3
	pand	%xmm0, %xmm3
	movdqa	%xmm5, %xmm0
	pcmpgtd	%xmm11, %xmm0
	por	%xmm0, %xmm3
	movdqa	%xmm11, %xmm0
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm6
	pand	%xmm3, %xmm0
	pandn	%xmm5, %xmm6
	por	%xmm6, %xmm0
	movdqa	%xmm3, %xmm6
	pand	%xmm5, %xmm3
	movdqa	%xmm7, %xmm5
	pandn	%xmm11, %xmm6
	movdqa	%xmm10, %xmm11
	pcmpeqd	%xmm10, %xmm5
	por	%xmm6, %xmm3
	movdqa	%xmm10, %xmm6
	psubq	%xmm7, %xmm6
	pand	%xmm6, %xmm5
	movdqa	%xmm7, %xmm6
	pcmpgtd	%xmm10, %xmm6
	por	%xmm6, %xmm5
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm6
	pand	%xmm5, %xmm11
	pandn	%xmm7, %xmm6
	por	%xmm6, %xmm11
	movdqa	%xmm5, %xmm6
	pand	%xmm7, %xmm5
	movdqa	%xmm1, %xmm7
	pandn	%xmm10, %xmm6
	pcmpeqd	%xmm2, %xmm7
	por	%xmm6, %xmm5
	movdqa	%xmm2, %xmm6
	psubq	%xmm1, %xmm6
	pand	%xmm6, %xmm7
	movdqa	%xmm1, %xmm6
	pcmpgtd	%xmm2, %xmm6
	por	%xmm6, %xmm7
	movdqa	%xmm2, %xmm6
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm10
	pand	%xmm7, %xmm6
	pandn	%xmm1, %xmm10
	pand	%xmm7, %xmm1
	por	%xmm10, %xmm6
	movdqa	%xmm7, %xmm10
	movdqa	%xmm12, %xmm7
	pandn	%xmm2, %xmm10
	movdqa	%xmm4, %xmm2
	psubq	%xmm4, %xmm7
	pcmpeqd	%xmm12, %xmm2
	por	%xmm10, %xmm1
	movdqa	%xmm12, %xmm10
	pand	%xmm7, %xmm2
	movdqa	%xmm4, %xmm7
	pcmpgtd	%xmm12, %xmm7
	por	%xmm7, %xmm2
	pshufd	$245, %xmm2, %xmm2
	movdqa	%xmm2, %xmm7
	pand	%xmm2, %xmm10
	pandn	%xmm4, %xmm7
	pand	%xmm2, %xmm4
	por	%xmm7, %xmm10
	movdqa	%xmm2, %xmm7
	movdqa	%xmm0, %xmm2
	pandn	%xmm12, %xmm7
	pcmpeqd	%xmm9, %xmm2
	por	%xmm7, %xmm4
	movdqa	%xmm9, %xmm7
	movaps	%xmm4, -64(%rbp)
	movdqa	%xmm9, %xmm4
	psubq	%xmm0, %xmm4
	pand	%xmm4, %xmm2
	movdqa	%xmm0, %xmm4
	pcmpgtd	%xmm9, %xmm4
	por	%xmm4, %xmm2
	pshufd	$245, %xmm2, %xmm2
	movdqa	%xmm2, %xmm4
	pand	%xmm2, %xmm7
	pandn	%xmm0, %xmm4
	pand	%xmm2, %xmm0
	por	%xmm4, %xmm7
	movdqa	%xmm2, %xmm4
	movdqa	%xmm3, %xmm2
	pandn	%xmm9, %xmm4
	pcmpeqd	%xmm11, %xmm2
	movaps	%xmm7, -80(%rbp)
	por	%xmm4, %xmm0
	movdqa	%xmm11, %xmm4
	psubq	%xmm3, %xmm4
	pand	%xmm4, %xmm2
	movdqa	%xmm3, %xmm4
	pcmpgtd	%xmm11, %xmm4
	por	%xmm4, %xmm2
	movdqa	%xmm11, %xmm4
	pshufd	$245, %xmm2, %xmm2
	movdqa	%xmm2, %xmm7
	pand	%xmm2, %xmm4
	pandn	%xmm3, %xmm7
	pand	%xmm2, %xmm3
	por	%xmm7, %xmm4
	movdqa	%xmm2, %xmm7
	movdqa	%xmm5, %xmm2
	pandn	%xmm11, %xmm7
	psubq	%xmm6, %xmm2
	por	%xmm7, %xmm3
	movdqa	%xmm6, %xmm7
	pcmpeqd	%xmm5, %xmm7
	pand	%xmm2, %xmm7
	movdqa	%xmm6, %xmm2
	pcmpgtd	%xmm5, %xmm2
	por	%xmm2, %xmm7
	movdqa	%xmm5, %xmm2
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm9
	pand	%xmm7, %xmm2
	pandn	%xmm6, %xmm9
	pand	%xmm7, %xmm6
	por	%xmm9, %xmm2
	movdqa	%xmm7, %xmm9
	movdqa	%xmm8, %xmm7
	pandn	%xmm5, %xmm9
	movdqa	%xmm1, %xmm5
	psubq	%xmm1, %xmm7
	pcmpeqd	%xmm8, %xmm5
	por	%xmm9, %xmm6
	pand	%xmm7, %xmm5
	movdqa	%xmm1, %xmm7
	pcmpgtd	%xmm8, %xmm7
	por	%xmm7, %xmm5
	movdqa	%xmm8, %xmm7
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm9
	pand	%xmm5, %xmm7
	pandn	%xmm1, %xmm9
	pand	%xmm5, %xmm1
	por	%xmm9, %xmm7
	movdqa	%xmm5, %xmm9
	movdqa	%xmm4, %xmm5
	pcmpeqd	%xmm0, %xmm5
	pandn	%xmm8, %xmm9
	movdqa	%xmm0, %xmm8
	psubq	%xmm4, %xmm8
	por	%xmm9, %xmm1
	movdqa	%xmm0, %xmm9
	pand	%xmm8, %xmm5
	movdqa	%xmm4, %xmm8
	pcmpgtd	%xmm0, %xmm8
	por	%xmm8, %xmm5
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm8
	pand	%xmm5, %xmm9
	pandn	%xmm4, %xmm8
	pand	%xmm5, %xmm4
	por	%xmm8, %xmm9
	movdqa	%xmm5, %xmm8
	pandn	%xmm0, %xmm8
	movdqa	%xmm3, %xmm0
	movaps	%xmm9, -96(%rbp)
	por	%xmm8, %xmm4
	psubq	%xmm2, %xmm0
	movdqa	%xmm4, %xmm14
	movdqa	%xmm2, %xmm4
	pcmpeqd	%xmm3, %xmm4
	pand	%xmm0, %xmm4
	movdqa	%xmm2, %xmm0
	pcmpgtd	%xmm3, %xmm0
	por	%xmm0, %xmm4
	movdqa	%xmm3, %xmm0
	pshufd	$245, %xmm4, %xmm4
	movdqa	%xmm4, %xmm5
	pand	%xmm4, %xmm0
	pandn	%xmm2, %xmm5
	pand	%xmm4, %xmm2
	por	%xmm5, %xmm0
	movdqa	%xmm4, %xmm5
	pandn	%xmm3, %xmm5
	por	%xmm5, %xmm2
	jbe	.L368
	movdqa	-112(%rbp), %xmm12
	pshufd	$78, %xmm7, %xmm7
	pshufd	$78, %xmm6, %xmm6
	pshufd	$78, -144(%rbp), %xmm9
	pshufd	$78, %xmm1, %xmm11
	movdqa	-128(%rbp), %xmm8
	pshufd	$78, -160(%rbp), %xmm4
	pshufd	$78, -176(%rbp), %xmm5
	movdqa	%xmm12, %xmm3
	movdqa	%xmm12, %xmm1
	movdqa	%xmm12, %xmm13
	movaps	%xmm5, -192(%rbp)
	pcmpeqd	%xmm9, %xmm3
	psubq	%xmm9, %xmm1
	pshufd	$78, %xmm2, %xmm2
	movaps	%xmm11, -176(%rbp)
	pshufd	$78, %xmm0, %xmm0
	pand	%xmm1, %xmm3
	movdqa	%xmm9, %xmm1
	pcmpgtd	%xmm12, %xmm1
	por	%xmm1, %xmm3
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm1
	pand	%xmm3, %xmm13
	pandn	%xmm9, %xmm1
	por	%xmm1, %xmm13
	movdqa	%xmm3, %xmm1
	pand	%xmm9, %xmm3
	pandn	%xmm12, %xmm1
	movdqa	%xmm4, %xmm12
	movdqa	%xmm13, %xmm9
	movaps	%xmm1, -256(%rbp)
	movdqa	%xmm8, %xmm1
	psubq	%xmm4, %xmm1
	pcmpeqd	%xmm8, %xmm4
	movaps	%xmm12, -208(%rbp)
	pand	%xmm1, %xmm4
	movdqa	%xmm12, %xmm1
	movdqa	%xmm8, %xmm12
	pcmpgtd	%xmm8, %xmm1
	por	%xmm1, %xmm4
	pshufd	$245, %xmm4, %xmm4
	movdqa	%xmm4, %xmm1
	pandn	-208(%rbp), %xmm1
	pand	%xmm4, %xmm12
	por	%xmm1, %xmm12
	movdqa	%xmm4, %xmm1
	pandn	%xmm8, %xmm1
	movdqa	%xmm15, %xmm8
	psubq	%xmm5, %xmm8
	movaps	%xmm1, -272(%rbp)
	movdqa	%xmm8, %xmm1
	movdqa	%xmm5, %xmm8
	pcmpeqd	%xmm15, %xmm8
	pand	%xmm1, %xmm8
	movdqa	%xmm5, %xmm1
	movdqa	%xmm15, %xmm5
	pcmpgtd	%xmm15, %xmm1
	por	%xmm1, %xmm8
	pshufd	$245, %xmm8, %xmm8
	movdqa	%xmm8, %xmm1
	pandn	-192(%rbp), %xmm1
	pand	%xmm8, %xmm5
	por	%xmm1, %xmm5
	movdqa	%xmm8, %xmm1
	pand	-192(%rbp), %xmm8
	pandn	%xmm15, %xmm1
	movaps	%xmm5, -128(%rbp)
	movdqa	%xmm10, %xmm5
	movaps	%xmm1, -288(%rbp)
	movdqa	%xmm11, %xmm1
	psubq	%xmm11, %xmm5
	pcmpeqd	%xmm10, %xmm1
	pand	%xmm5, %xmm1
	movdqa	%xmm11, %xmm5
	movdqa	%xmm10, %xmm11
	pcmpgtd	%xmm10, %xmm5
	por	%xmm5, %xmm1
	pshufd	$245, %xmm1, %xmm1
	pand	%xmm1, %xmm11
	movdqa	%xmm1, %xmm5
	pandn	-176(%rbp), %xmm5
	movdqa	%xmm11, %xmm15
	movdqa	%xmm1, %xmm11
	pand	-176(%rbp), %xmm1
	por	%xmm5, %xmm15
	pandn	%xmm10, %xmm11
	movaps	%xmm15, -144(%rbp)
	movdqa	-64(%rbp), %xmm15
	movaps	%xmm11, -304(%rbp)
	por	-304(%rbp), %xmm1
	movdqa	%xmm15, %xmm5
	movdqa	%xmm15, %xmm10
	pcmpeqd	%xmm7, %xmm5
	psubq	%xmm7, %xmm10
	pshufd	$78, %xmm1, %xmm1
	pand	%xmm10, %xmm5
	movdqa	%xmm7, %xmm10
	pcmpgtd	%xmm15, %xmm10
	por	%xmm10, %xmm5
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm10
	pandn	%xmm7, %xmm10
	pand	%xmm5, %xmm7
	movaps	%xmm10, -320(%rbp)
	movdqa	%xmm5, %xmm10
	pand	-64(%rbp), %xmm5
	por	-320(%rbp), %xmm5
	pandn	%xmm15, %xmm10
	por	%xmm10, %xmm7
	movdqa	-80(%rbp), %xmm10
	pshufd	$78, %xmm5, %xmm5
	movaps	%xmm7, -112(%rbp)
	movdqa	%xmm10, %xmm11
	movdqa	%xmm10, %xmm7
	pcmpeqd	%xmm6, %xmm11
	psubq	%xmm6, %xmm7
	pand	%xmm7, %xmm11
	movdqa	%xmm6, %xmm7
	pcmpgtd	%xmm10, %xmm7
	por	%xmm7, %xmm11
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm7
	pandn	%xmm6, %xmm7
	pand	%xmm11, %xmm6
	movaps	%xmm7, -336(%rbp)
	movdqa	%xmm11, %xmm7
	pand	-80(%rbp), %xmm11
	por	-336(%rbp), %xmm11
	pandn	%xmm10, %xmm7
	movdqa	-96(%rbp), %xmm10
	por	%xmm7, %xmm6
	pshufd	$78, %xmm11, %xmm11
	movdqa	%xmm10, %xmm7
	movaps	%xmm6, -160(%rbp)
	movdqa	%xmm10, %xmm6
	pcmpeqd	%xmm2, %xmm7
	psubq	%xmm2, %xmm6
	pand	%xmm6, %xmm7
	movdqa	%xmm2, %xmm6
	pcmpgtd	%xmm10, %xmm6
	por	%xmm6, %xmm7
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm6
	pandn	%xmm2, %xmm6
	pand	%xmm7, %xmm2
	movaps	%xmm6, -352(%rbp)
	movdqa	%xmm7, %xmm6
	por	-288(%rbp), %xmm8
	pand	-96(%rbp), %xmm7
	pandn	%xmm10, %xmm6
	por	-256(%rbp), %xmm3
	por	-352(%rbp), %xmm7
	por	%xmm6, %xmm2
	movdqa	%xmm14, %xmm6
	pand	-208(%rbp), %xmm4
	psubq	%xmm0, %xmm6
	pshufd	$78, %xmm7, %xmm7
	por	-272(%rbp), %xmm4
	movdqa	%xmm6, %xmm10
	movdqa	%xmm0, %xmm6
	movaps	%xmm7, -80(%rbp)
	pshufd	$78, %xmm8, %xmm8
	pcmpeqd	%xmm14, %xmm6
	pshufd	$78, %xmm4, %xmm4
	movaps	%xmm4, -96(%rbp)
	pand	%xmm10, %xmm6
	movdqa	%xmm0, %xmm10
	pcmpgtd	%xmm14, %xmm10
	por	%xmm10, %xmm6
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm10
	movdqa	%xmm6, %xmm15
	pandn	%xmm0, %xmm10
	pand	%xmm6, %xmm0
	pand	%xmm14, %xmm6
	por	%xmm10, %xmm6
	pandn	%xmm14, %xmm15
	movdqa	-80(%rbp), %xmm14
	pshufd	$78, %xmm6, %xmm6
	por	%xmm15, %xmm0
	pshufd	$78, %xmm3, %xmm15
	movdqa	%xmm6, %xmm10
	movdqa	%xmm13, %xmm3
	movdqa	%xmm15, %xmm7
	pcmpeqd	%xmm13, %xmm10
	pcmpeqd	%xmm0, %xmm7
	psubq	%xmm6, %xmm3
	pand	%xmm3, %xmm10
	movdqa	%xmm6, %xmm3
	pcmpgtd	%xmm13, %xmm3
	por	%xmm3, %xmm10
	pshufd	$245, %xmm10, %xmm10
	movdqa	%xmm10, %xmm3
	pand	%xmm10, %xmm9
	movdqa	%xmm10, %xmm4
	pandn	%xmm6, %xmm3
	pandn	%xmm13, %xmm4
	movdqa	%xmm12, %xmm13
	por	%xmm3, %xmm9
	movdqa	%xmm0, %xmm3
	pand	%xmm6, %xmm10
	movaps	%xmm4, -256(%rbp)
	psubq	%xmm15, %xmm3
	movdqa	%xmm0, %xmm4
	pand	%xmm3, %xmm7
	movdqa	%xmm15, %xmm3
	pcmpgtd	%xmm0, %xmm3
	por	%xmm3, %xmm7
	pshufd	$245, %xmm7, %xmm7
	movdqa	%xmm7, %xmm3
	pand	%xmm7, %xmm4
	pandn	%xmm15, %xmm3
	por	%xmm3, %xmm4
	movdqa	%xmm7, %xmm3
	pand	%xmm15, %xmm7
	pandn	%xmm0, %xmm3
	movdqa	%xmm12, %xmm0
	psubq	%xmm14, %xmm0
	movaps	%xmm3, -272(%rbp)
	movdqa	%xmm0, %xmm3
	movdqa	%xmm14, %xmm0
	pcmpeqd	%xmm12, %xmm0
	pand	%xmm3, %xmm0
	movdqa	%xmm14, %xmm3
	movdqa	%xmm2, %xmm14
	pcmpgtd	%xmm12, %xmm3
	por	%xmm3, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	pandn	-80(%rbp), %xmm3
	pand	%xmm0, %xmm13
	movaps	%xmm0, -288(%rbp)
	por	%xmm3, %xmm13
	movdqa	%xmm0, %xmm3
	movdqa	-112(%rbp), %xmm0
	movaps	%xmm13, -176(%rbp)
	movdqa	-96(%rbp), %xmm13
	pandn	%xmm12, %xmm3
	movdqa	%xmm2, %xmm12
	movaps	%xmm3, -304(%rbp)
	movdqa	%xmm13, %xmm3
	psubq	%xmm13, %xmm12
	pcmpeqd	%xmm2, %xmm3
	pand	%xmm12, %xmm3
	movdqa	%xmm13, %xmm12
	movdqa	-128(%rbp), %xmm13
	pcmpgtd	%xmm2, %xmm12
	por	%xmm12, %xmm3
	pshufd	$245, %xmm3, %xmm3
	movdqa	%xmm3, %xmm12
	pandn	-96(%rbp), %xmm12
	pand	%xmm3, %xmm14
	por	%xmm12, %xmm14
	movdqa	%xmm3, %xmm12
	pandn	%xmm2, %xmm12
	movdqa	%xmm13, %xmm2
	movaps	%xmm14, -192(%rbp)
	pcmpeqd	%xmm11, %xmm2
	movaps	%xmm12, -320(%rbp)
	movdqa	%xmm13, %xmm12
	psubq	%xmm11, %xmm12
	pand	%xmm12, %xmm2
	movdqa	%xmm11, %xmm12
	pcmpgtd	%xmm13, %xmm12
	por	%xmm12, %xmm2
	pshufd	$245, %xmm2, %xmm2
	movdqa	%xmm2, %xmm14
	movdqa	%xmm2, %xmm12
	pandn	%xmm11, %xmm14
	pand	%xmm2, %xmm11
	pandn	%xmm13, %xmm12
	movdqa	%xmm11, %xmm13
	movaps	%xmm14, -336(%rbp)
	pand	-128(%rbp), %xmm2
	por	-336(%rbp), %xmm2
	por	%xmm12, %xmm13
	movdqa	-160(%rbp), %xmm12
	movaps	%xmm13, -208(%rbp)
	pshufd	$78, %xmm2, %xmm2
	movdqa	%xmm12, %xmm14
	movdqa	%xmm12, %xmm11
	pcmpeqd	%xmm8, %xmm14
	psubq	%xmm8, %xmm11
	pand	%xmm11, %xmm14
	movdqa	%xmm8, %xmm11
	pcmpgtd	%xmm12, %xmm11
	por	%xmm11, %xmm14
	pshufd	$245, %xmm14, %xmm14
	movdqa	%xmm14, %xmm11
	pandn	%xmm8, %xmm11
	pand	%xmm14, %xmm8
	movaps	%xmm11, -352(%rbp)
	movdqa	%xmm14, %xmm11
	pandn	%xmm12, %xmm11
	movdqa	-144(%rbp), %xmm12
	por	%xmm11, %xmm8
	movaps	%xmm8, -64(%rbp)
	movdqa	%xmm12, %xmm8
	movdqa	%xmm12, %xmm11
	pcmpeqd	%xmm5, %xmm8
	psubq	%xmm5, %xmm11
	pand	%xmm11, %xmm8
	movdqa	%xmm5, %xmm11
	pcmpgtd	%xmm12, %xmm11
	por	%xmm11, %xmm8
	pshufd	$245, %xmm8, %xmm8
	movdqa	%xmm8, %xmm11
	pandn	%xmm5, %xmm11
	pand	%xmm8, %xmm5
	movdqa	%xmm11, %xmm13
	movdqa	%xmm8, %xmm11
	pand	-144(%rbp), %xmm8
	pandn	%xmm12, %xmm11
	movdqa	%xmm0, %xmm12
	por	%xmm11, %xmm5
	movdqa	%xmm0, %xmm11
	psubq	%xmm1, %xmm12
	pcmpeqd	%xmm1, %xmm11
	por	%xmm13, %xmm8
	pshufd	$78, %xmm8, %xmm8
	pand	%xmm12, %xmm11
	movdqa	%xmm1, %xmm12
	pcmpgtd	-112(%rbp), %xmm12
	por	%xmm12, %xmm11
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm12
	movdqa	%xmm11, %xmm0
	pandn	-112(%rbp), %xmm0
	pandn	%xmm1, %xmm12
	pand	%xmm11, %xmm1
	por	%xmm0, %xmm1
	movdqa	-288(%rbp), %xmm0
	movdqa	-112(%rbp), %xmm6
	por	-272(%rbp), %xmm7
	pand	-96(%rbp), %xmm3
	pand	%xmm11, %xmm6
	pand	-80(%rbp), %xmm0
	por	-256(%rbp), %xmm10
	pshufd	$78, %xmm7, %xmm7
	por	%xmm12, %xmm6
	movdqa	%xmm9, %xmm12
	movdqa	-208(%rbp), %xmm15
	movaps	%xmm7, -96(%rbp)
	movdqa	%xmm8, %xmm7
	pshufd	$78, %xmm6, %xmm6
	pshufd	$78, %xmm10, %xmm10
	pcmpeqd	%xmm9, %xmm7
	movaps	%xmm6, -112(%rbp)
	movdqa	%xmm9, %xmm6
	por	-304(%rbp), %xmm0
	psubq	%xmm8, %xmm6
	movaps	%xmm10, -80(%rbp)
	por	-320(%rbp), %xmm3
	pand	-160(%rbp), %xmm14
	por	-352(%rbp), %xmm14
	pshufd	$78, %xmm0, %xmm0
	pand	%xmm6, %xmm7
	movdqa	%xmm8, %xmm6
	pshufd	$78, %xmm3, %xmm3
	pcmpgtd	%xmm9, %xmm6
	pshufd	$78, %xmm14, %xmm14
	por	%xmm6, %xmm7
	pshufd	$245, %xmm7, %xmm7
	pand	%xmm7, %xmm12
	movdqa	%xmm7, %xmm6
	movdqa	%xmm12, %xmm13
	movdqa	%xmm7, %xmm12
	pandn	%xmm8, %xmm6
	pandn	%xmm9, %xmm12
	por	%xmm6, %xmm13
	pand	%xmm8, %xmm7
	movdqa	-176(%rbp), %xmm9
	movaps	%xmm12, -272(%rbp)
	movdqa	%xmm5, %xmm12
	movdqa	%xmm9, %xmm11
	movdqa	%xmm9, %xmm6
	psubq	%xmm10, %xmm12
	pcmpeqd	%xmm2, %xmm11
	psubq	%xmm2, %xmm6
	pand	%xmm6, %xmm11
	movdqa	%xmm2, %xmm6
	pcmpgtd	%xmm9, %xmm6
	por	%xmm6, %xmm11
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm6
	pandn	%xmm2, %xmm6
	pand	%xmm11, %xmm2
	movaps	%xmm6, -288(%rbp)
	movdqa	%xmm11, %xmm6
	pandn	%xmm9, %xmm6
	por	%xmm6, %xmm2
	movdqa	%xmm10, %xmm6
	pcmpeqd	%xmm5, %xmm6
	pand	%xmm12, %xmm6
	movdqa	%xmm10, %xmm12
	pcmpgtd	%xmm5, %xmm12
	por	%xmm12, %xmm6
	pshufd	$245, %xmm6, %xmm6
	movdqa	%xmm6, %xmm10
	movdqa	%xmm6, %xmm12
	pandn	-80(%rbp), %xmm12
	pandn	%xmm5, %xmm10
	movaps	%xmm10, -304(%rbp)
	movdqa	%xmm15, %xmm10
	movdqa	%xmm12, %xmm9
	movdqa	%xmm5, %xmm12
	pcmpeqd	%xmm0, %xmm10
	movdqa	%xmm15, %xmm5
	pand	%xmm6, %xmm12
	psubq	%xmm0, %xmm5
	por	%xmm9, %xmm12
	pand	%xmm5, %xmm10
	movdqa	%xmm0, %xmm5
	pcmpgtd	%xmm15, %xmm5
	por	%xmm5, %xmm10
	pshufd	$245, %xmm10, %xmm10
	movdqa	%xmm10, %xmm5
	pandn	%xmm0, %xmm5
	pand	%xmm10, %xmm0
	movaps	%xmm5, -320(%rbp)
	movdqa	%xmm10, %xmm5
	pandn	%xmm15, %xmm5
	movdqa	%xmm4, %xmm15
	por	%xmm5, %xmm0
	movdqa	-112(%rbp), %xmm5
	movaps	%xmm0, -144(%rbp)
	psubq	%xmm5, %xmm15
	movdqa	%xmm15, %xmm9
	movdqa	%xmm5, %xmm15
	pcmpeqd	%xmm4, %xmm5
	pand	%xmm9, %xmm5
	movdqa	%xmm15, %xmm9
	movdqa	%xmm4, %xmm15
	pcmpgtd	%xmm4, %xmm9
	por	%xmm9, %xmm5
	pshufd	$245, %xmm5, %xmm5
	movdqa	%xmm5, %xmm9
	pandn	-112(%rbp), %xmm9
	pand	%xmm5, %xmm15
	por	%xmm9, %xmm15
	movdqa	%xmm5, %xmm9
	movaps	%xmm15, -160(%rbp)
	pandn	%xmm4, %xmm9
	movdqa	-192(%rbp), %xmm15
	movaps	%xmm9, -336(%rbp)
	movdqa	%xmm15, %xmm9
	movdqa	%xmm15, %xmm4
	pcmpeqd	%xmm14, %xmm9
	psubq	%xmm14, %xmm4
	pand	%xmm4, %xmm9
	movdqa	%xmm14, %xmm4
	pcmpgtd	%xmm15, %xmm4
	por	%xmm4, %xmm9
	pshufd	$245, %xmm9, %xmm9
	movdqa	%xmm9, %xmm0
	movdqa	%xmm9, %xmm4
	pandn	%xmm15, %xmm0
	movdqa	-96(%rbp), %xmm15
	pandn	%xmm14, %xmm4
	pand	%xmm9, %xmm14
	por	%xmm0, %xmm14
	movaps	%xmm4, -352(%rbp)
	movdqa	%xmm1, %xmm0
	movdqa	%xmm15, %xmm4
	movaps	%xmm14, -128(%rbp)
	movdqa	%xmm15, %xmm14
	psubq	%xmm15, %xmm0
	pcmpeqd	%xmm1, %xmm4
	pcmpgtd	%xmm1, %xmm14
	pand	%xmm0, %xmm4
	por	%xmm14, %xmm4
	pshufd	$245, %xmm4, %xmm4
	movdqa	%xmm4, %xmm0
	pandn	-96(%rbp), %xmm0
	movdqa	%xmm0, %xmm14
	movdqa	%xmm1, %xmm0
	pand	%xmm4, %xmm0
	movdqa	%xmm0, %xmm15
	movdqa	%xmm4, %xmm0
	pandn	%xmm1, %xmm0
	por	%xmm14, %xmm15
	movaps	%xmm0, -368(%rbp)
	movdqa	-64(%rbp), %xmm0
	movdqa	%xmm0, %xmm1
	movdqa	%xmm0, %xmm14
	pcmpeqd	%xmm3, %xmm1
	psubq	%xmm3, %xmm14
	pand	%xmm14, %xmm1
	movdqa	%xmm3, %xmm14
	pcmpgtd	-64(%rbp), %xmm14
	pand	-80(%rbp), %xmm6
	pand	-112(%rbp), %xmm5
	pand	-176(%rbp), %xmm11
	por	-272(%rbp), %xmm7
	por	-288(%rbp), %xmm11
	pand	-96(%rbp), %xmm4
	por	%xmm14, %xmm1
	pshufd	$78, %xmm7, %xmm7
	movdqa	-208(%rbp), %xmm8
	por	-304(%rbp), %xmm6
	pshufd	$245, %xmm1, %xmm1
	pshufd	$78, %xmm11, %xmm11
	pand	-192(%rbp), %xmm9
	movdqa	%xmm1, %xmm14
	movdqa	%xmm1, %xmm0
	pandn	-64(%rbp), %xmm0
	pandn	%xmm3, %xmm14
	pand	%xmm1, %xmm3
	pand	%xmm10, %xmm8
	por	%xmm0, %xmm3
	movdqa	%xmm13, %xmm10
	por	-320(%rbp), %xmm8
	movaps	%xmm14, -384(%rbp)
	movaps	%xmm3, -256(%rbp)
	movdqa	-64(%rbp), %xmm3
	psubq	%xmm11, %xmm10
	pshufd	$78, %xmm6, %xmm6
	pshufd	$78, %xmm8, %xmm8
	por	-336(%rbp), %xmm5
	por	-352(%rbp), %xmm9
	pand	%xmm1, %xmm3
	movdqa	%xmm11, %xmm1
	por	-384(%rbp), %xmm3
	pcmpeqd	%xmm13, %xmm1
	pshufd	$78, %xmm9, %xmm9
	pshufd	$78, %xmm5, %xmm5
	pshufd	$78, %xmm3, %xmm3
	por	-368(%rbp), %xmm4
	pand	%xmm10, %xmm1
	movdqa	%xmm11, %xmm10
	pshufd	$78, %xmm4, %xmm4
	pcmpgtd	%xmm13, %xmm10
	por	%xmm10, %xmm1
	movdqa	%xmm13, %xmm10
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm0
	pand	%xmm1, %xmm10
	pandn	%xmm11, %xmm0
	por	%xmm0, %xmm10
	movdqa	%xmm1, %xmm0
	pand	%xmm11, %xmm1
	movdqa	%xmm7, %xmm11
	pandn	%xmm13, %xmm0
	movdqa	%xmm2, %xmm13
	pcmpeqd	%xmm2, %xmm11
	psubq	%xmm7, %xmm13
	por	%xmm0, %xmm1
	movdqa	%xmm2, %xmm0
	movaps	%xmm1, -64(%rbp)
	movdqa	-144(%rbp), %xmm1
	pand	%xmm13, %xmm11
	movdqa	%xmm7, %xmm13
	pcmpgtd	%xmm2, %xmm13
	por	%xmm13, %xmm11
	pshufd	$245, %xmm11, %xmm11
	movdqa	%xmm11, %xmm13
	pand	%xmm11, %xmm0
	pandn	%xmm7, %xmm13
	por	%xmm0, %xmm13
	movdqa	%xmm11, %xmm0
	pand	%xmm7, %xmm11
	pandn	%xmm2, %xmm0
	movdqa	%xmm8, %xmm2
	movdqa	%xmm12, %xmm7
	pcmpeqd	%xmm12, %xmm2
	psubq	%xmm8, %xmm7
	movdqa	%xmm0, %xmm14
	por	%xmm11, %xmm14
	movdqa	%xmm12, %xmm11
	movdqa	%xmm1, %xmm0
	pand	%xmm7, %xmm2
	movdqa	%xmm8, %xmm7
	pcmpgtd	%xmm12, %xmm7
	por	%xmm7, %xmm2
	pshufd	$245, %xmm2, %xmm2
	movdqa	%xmm2, %xmm7
	pand	%xmm2, %xmm11
	pandn	%xmm8, %xmm7
	por	%xmm7, %xmm11
	movdqa	%xmm2, %xmm7
	pand	%xmm8, %xmm2
	pandn	%xmm12, %xmm7
	por	%xmm7, %xmm2
	movdqa	%xmm1, %xmm7
	movdqa	%xmm2, %xmm12
	movdqa	%xmm6, %xmm2
	psubq	%xmm6, %xmm7
	pcmpeqd	%xmm1, %xmm2
	pand	%xmm7, %xmm2
	movdqa	%xmm6, %xmm7
	pcmpgtd	%xmm1, %xmm7
	por	%xmm7, %xmm2
	pshufd	$245, %xmm2, %xmm2
	pand	%xmm2, %xmm0
	movdqa	%xmm2, %xmm7
	pandn	%xmm6, %xmm7
	movdqa	%xmm0, %xmm8
	por	%xmm7, %xmm8
	movdqa	%xmm2, %xmm7
	pand	%xmm6, %xmm2
	pandn	%xmm1, %xmm7
	movdqa	-160(%rbp), %xmm1
	por	%xmm2, %xmm7
	movdqa	%xmm1, %xmm0
	movdqa	%xmm1, %xmm2
	movdqa	%xmm1, %xmm6
	pcmpeqd	%xmm9, %xmm0
	psubq	%xmm9, %xmm2
	pand	%xmm2, %xmm0
	movdqa	%xmm9, %xmm2
	pcmpgtd	%xmm1, %xmm2
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm2
	pand	%xmm0, %xmm6
	pandn	%xmm9, %xmm2
	por	%xmm2, %xmm6
	movdqa	%xmm0, %xmm2
	pand	%xmm9, %xmm0
	pandn	%xmm1, %xmm2
	movaps	%xmm6, -144(%rbp)
	movdqa	-128(%rbp), %xmm6
	por	%xmm2, %xmm0
	movdqa	%xmm0, %xmm9
	movdqa	%xmm6, %xmm0
	movdqa	%xmm6, %xmm2
	pcmpeqd	%xmm5, %xmm0
	psubq	%xmm5, %xmm2
	pand	%xmm2, %xmm0
	movdqa	%xmm5, %xmm2
	pcmpgtd	%xmm6, %xmm2
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm1
	pandn	-128(%rbp), %xmm1
	movdqa	%xmm0, %xmm2
	pand	%xmm0, %xmm6
	pand	%xmm5, %xmm0
	pandn	%xmm5, %xmm2
	por	%xmm1, %xmm0
	movdqa	%xmm3, %xmm1
	por	%xmm2, %xmm6
	pcmpeqd	%xmm15, %xmm1
	movdqa	%xmm15, %xmm2
	movdqa	%xmm15, %xmm5
	movaps	%xmm0, -160(%rbp)
	psubq	%xmm3, %xmm2
	movdqa	%xmm1, %xmm0
	pand	%xmm2, %xmm0
	movdqa	%xmm3, %xmm2
	pcmpgtd	%xmm15, %xmm2
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm2
	pand	%xmm0, %xmm5
	pandn	%xmm3, %xmm2
	por	%xmm2, %xmm5
	movdqa	%xmm0, %xmm2
	pand	%xmm3, %xmm0
	pandn	%xmm15, %xmm2
	movdqa	-256(%rbp), %xmm15
	por	%xmm2, %xmm0
	movdqa	%xmm15, %xmm1
	movdqa	%xmm15, %xmm2
	movdqa	%xmm15, %xmm3
	movaps	%xmm0, -176(%rbp)
	pcmpeqd	%xmm4, %xmm1
	psubq	%xmm4, %xmm2
	movdqa	%xmm1, %xmm0
	pand	%xmm2, %xmm0
	movdqa	%xmm4, %xmm2
	pcmpgtd	%xmm15, %xmm2
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm2
	pand	%xmm0, %xmm3
	pandn	%xmm4, %xmm2
	por	%xmm2, %xmm3
	movdqa	%xmm0, %xmm2
	pand	%xmm4, %xmm0
	pandn	%xmm15, %xmm2
	movdqa	%xmm0, %xmm4
	movaps	%xmm3, -192(%rbp)
	pshufd	$78, %xmm10, %xmm3
	por	%xmm2, %xmm4
	movdqa	%xmm10, %xmm2
	movdqa	%xmm3, %xmm1
	movdqa	-64(%rbp), %xmm15
	psubq	%xmm3, %xmm2
	pcmpgtd	%xmm10, %xmm1
	movdqa	%xmm2, %xmm0
	movdqa	%xmm10, %xmm2
	pcmpeqd	%xmm3, %xmm2
	pand	%xmm0, %xmm2
	por	%xmm1, %xmm2
	pshufd	$245, %xmm2, %xmm0
	punpckhqdq	%xmm0, %xmm0
	movdqa	%xmm0, %xmm2
	pand	%xmm3, %xmm0
	pshufd	$78, %xmm15, %xmm3
	pandn	%xmm10, %xmm2
	movdqa	%xmm0, %xmm1
	por	%xmm2, %xmm1
	movdqa	%xmm15, %xmm2
	pcmpeqd	%xmm3, %xmm2
	movaps	%xmm1, -112(%rbp)
	movdqa	%xmm15, %xmm1
	psubq	%xmm3, %xmm1
	pand	%xmm1, %xmm2
	movdqa	%xmm3, %xmm1
	pcmpgtd	%xmm15, %xmm1
	por	%xmm1, %xmm2
	pshufd	$245, %xmm2, %xmm0
	punpckhqdq	%xmm0, %xmm0
	movdqa	%xmm0, %xmm2
	pand	%xmm3, %xmm0
	pshufd	$78, %xmm9, %xmm3
	pandn	%xmm15, %xmm2
	movdqa	%xmm0, %xmm1
	por	%xmm2, %xmm1
	pshufd	$78, %xmm13, %xmm2
	movaps	%xmm1, -128(%rbp)
	movdqa	%xmm13, %xmm1
	movdqa	%xmm2, %xmm15
	psubq	%xmm2, %xmm1
	pcmpgtd	%xmm13, %xmm15
	movdqa	%xmm1, %xmm0
	movdqa	%xmm13, %xmm1
	pcmpeqd	%xmm2, %xmm1
	pand	%xmm0, %xmm1
	por	%xmm15, %xmm1
	pshufd	$245, %xmm1, %xmm0
	punpckhqdq	%xmm0, %xmm0
	movdqa	%xmm0, %xmm1
	pand	%xmm2, %xmm0
	pshufd	$78, %xmm14, %xmm2
	pandn	%xmm13, %xmm1
	movdqa	%xmm0, %xmm15
	movdqa	%xmm2, %xmm10
	por	%xmm1, %xmm15
	pcmpgtd	%xmm14, %xmm10
	movdqa	%xmm14, %xmm1
	psubq	%xmm2, %xmm1
	movdqa	%xmm1, %xmm0
	movdqa	%xmm14, %xmm1
	pcmpeqd	%xmm2, %xmm1
	pand	%xmm0, %xmm1
	por	%xmm10, %xmm1
	pshufd	$245, %xmm1, %xmm0
	punpckhqdq	%xmm0, %xmm0
	movdqa	%xmm0, %xmm1
	pand	%xmm2, %xmm0
	pshufd	$78, %xmm11, %xmm2
	pandn	%xmm14, %xmm1
	movdqa	%xmm0, %xmm10
	movdqa	%xmm2, %xmm14
	por	%xmm1, %xmm10
	pcmpgtd	%xmm11, %xmm14
	movdqa	%xmm11, %xmm1
	psubq	%xmm2, %xmm1
	movdqa	%xmm1, %xmm0
	movdqa	%xmm11, %xmm1
	pcmpeqd	%xmm2, %xmm1
	pand	%xmm0, %xmm1
	por	%xmm14, %xmm1
	pshufd	$245, %xmm1, %xmm0
	punpckhqdq	%xmm0, %xmm0
	movdqa	%xmm0, %xmm1
	pand	%xmm2, %xmm0
	pandn	%xmm11, %xmm1
	movdqa	%xmm0, %xmm2
	pshufd	$78, %xmm4, %xmm11
	por	%xmm1, %xmm2
	movdqa	%xmm12, %xmm1
	movaps	%xmm2, -64(%rbp)
	pshufd	$78, %xmm12, %xmm2
	movdqa	-160(%rbp), %xmm13
	psubq	%xmm2, %xmm1
	movdqa	%xmm2, %xmm14
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm12, %xmm14
	movdqa	%xmm12, %xmm1
	pcmpeqd	%xmm2, %xmm1
	pand	%xmm0, %xmm1
	por	%xmm14, %xmm1
	pshufd	$245, %xmm1, %xmm0
	punpckhqdq	%xmm0, %xmm0
	movdqa	%xmm0, %xmm1
	pand	%xmm2, %xmm0
	pandn	%xmm12, %xmm1
	movdqa	%xmm0, %xmm2
	por	%xmm1, %xmm2
	movdqa	%xmm8, %xmm1
	movaps	%xmm2, -80(%rbp)
	pshufd	$78, %xmm8, %xmm2
	psubq	%xmm2, %xmm1
	movdqa	%xmm2, %xmm14
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm8, %xmm14
	movdqa	%xmm8, %xmm1
	pcmpeqd	%xmm2, %xmm1
	pand	%xmm0, %xmm1
	por	%xmm14, %xmm1
	pshufd	$245, %xmm1, %xmm0
	punpckhqdq	%xmm0, %xmm0
	movdqa	%xmm0, %xmm1
	pand	%xmm2, %xmm0
	pandn	%xmm8, %xmm1
	movdqa	%xmm0, %xmm2
	por	%xmm1, %xmm2
	movdqa	%xmm7, %xmm1
	movaps	%xmm2, -96(%rbp)
	pshufd	$78, %xmm7, %xmm2
	psubq	%xmm2, %xmm1
	movdqa	%xmm2, %xmm14
	movdqa	%xmm1, %xmm0
	pcmpgtd	%xmm7, %xmm14
	movdqa	%xmm7, %xmm1
	pcmpeqd	%xmm2, %xmm1
	pand	%xmm0, %xmm1
	por	%xmm14, %xmm1
	pshufd	$245, %xmm1, %xmm0
	punpckhqdq	%xmm0, %xmm0
	movdqa	%xmm0, %xmm1
	pand	%xmm2, %xmm0
	pandn	%xmm7, %xmm1
	movdqa	-144(%rbp), %xmm7
	movdqa	%xmm0, %xmm14
	por	%xmm1, %xmm14
	pshufd	$78, %xmm7, %xmm2
	movdqa	%xmm7, %xmm1
	movdqa	%xmm7, %xmm0
	pcmpeqd	%xmm2, %xmm1
	movdqa	%xmm2, %xmm8
	psubq	%xmm2, %xmm0
	pcmpgtd	%xmm7, %xmm8
	pand	%xmm0, %xmm1
	por	%xmm8, %xmm1
	pshufd	$78, %xmm5, %xmm8
	pshufd	$245, %xmm1, %xmm0
	punpckhqdq	%xmm0, %xmm0
	movdqa	%xmm0, %xmm1
	pand	%xmm2, %xmm0
	movdqa	%xmm9, %xmm2
	pandn	%xmm7, %xmm1
	psubq	%xmm3, %xmm2
	pshufd	$78, %xmm6, %xmm7
	por	%xmm1, %xmm0
	movdqa	%xmm9, %xmm1
	pcmpeqd	%xmm3, %xmm1
	pand	%xmm2, %xmm1
	movdqa	%xmm3, %xmm2
	pcmpgtd	%xmm9, %xmm2
	por	%xmm2, %xmm1
	pshufd	$245, %xmm1, %xmm2
	punpckhqdq	%xmm2, %xmm2
	movdqa	%xmm2, %xmm1
	pand	%xmm3, %xmm2
	movdqa	%xmm6, %xmm3
	pandn	%xmm9, %xmm1
	pcmpeqd	%xmm7, %xmm3
	por	%xmm1, %xmm2
	movdqa	%xmm6, %xmm1
	psubq	%xmm7, %xmm1
	pand	%xmm1, %xmm3
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm3
	pshufd	$245, %xmm3, %xmm1
	punpckhqdq	%xmm1, %xmm1
	movdqa	%xmm1, %xmm3
	pandn	%xmm6, %xmm3
	movdqa	%xmm1, %xmm6
	movdqa	%xmm13, %xmm1
	pand	%xmm7, %xmm6
	movdqa	%xmm13, %xmm7
	por	%xmm3, %xmm6
	pshufd	$78, %xmm13, %xmm3
	pcmpeqd	%xmm3, %xmm1
	psubq	%xmm3, %xmm7
	pand	%xmm7, %xmm1
	movdqa	%xmm3, %xmm7
	pcmpgtd	%xmm13, %xmm7
	por	%xmm7, %xmm1
	pshufd	$245, %xmm1, %xmm7
	punpckhqdq	%xmm7, %xmm7
	movdqa	%xmm7, %xmm1
	pand	%xmm3, %xmm7
	movdqa	%xmm5, %xmm3
	pandn	%xmm13, %xmm1
	pcmpeqd	%xmm8, %xmm3
	movdqa	-176(%rbp), %xmm13
	por	%xmm1, %xmm7
	movdqa	%xmm5, %xmm1
	psubq	%xmm8, %xmm1
	pand	%xmm1, %xmm3
	movdqa	%xmm8, %xmm1
	pcmpgtd	%xmm5, %xmm1
	por	%xmm1, %xmm3
	pshufd	$245, %xmm3, %xmm1
	punpckhqdq	%xmm1, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm8, %xmm1
	movdqa	%xmm13, %xmm8
	pandn	%xmm5, %xmm3
	pshufd	$78, %xmm13, %xmm5
	por	%xmm3, %xmm1
	movdqa	%xmm13, %xmm3
	movdqa	%xmm5, %xmm9
	pcmpeqd	%xmm5, %xmm3
	pcmpgtd	%xmm13, %xmm9
	psubq	%xmm5, %xmm8
	pand	%xmm8, %xmm3
	por	%xmm9, %xmm3
	pshufd	$245, %xmm3, %xmm9
	punpckhqdq	%xmm9, %xmm9
	movdqa	%xmm9, %xmm3
	pand	%xmm5, %xmm9
	pandn	%xmm13, %xmm3
	movdqa	-192(%rbp), %xmm13
	por	%xmm3, %xmm9
	pshufd	$78, %xmm13, %xmm5
	movdqa	%xmm13, %xmm3
	movdqa	%xmm13, %xmm8
	pcmpeqd	%xmm5, %xmm3
	psubq	%xmm5, %xmm8
	pand	%xmm8, %xmm3
	movdqa	%xmm5, %xmm8
	pcmpgtd	%xmm13, %xmm8
	por	%xmm8, %xmm3
	pshufd	$245, %xmm3, %xmm8
	punpckhqdq	%xmm8, %xmm8
	movdqa	%xmm8, %xmm3
	pand	%xmm5, %xmm8
	movdqa	%xmm4, %xmm5
	pandn	%xmm13, %xmm3
	pcmpeqd	%xmm11, %xmm5
	por	%xmm3, %xmm8
	movdqa	%xmm4, %xmm3
	psubq	%xmm11, %xmm3
	pand	%xmm3, %xmm5
	movdqa	%xmm11, %xmm3
	pcmpgtd	%xmm4, %xmm3
	por	%xmm3, %xmm5
	pshufd	$245, %xmm5, %xmm3
	punpckhqdq	%xmm3, %xmm3
	movdqa	%xmm3, %xmm5
	pand	%xmm11, %xmm3
	pandn	%xmm4, %xmm5
	por	%xmm5, %xmm3
.L366:
	movdqa	-112(%rbp), %xmm4
	movq	-232(%rbp), %rdx
	movups	%xmm4, (%rdx)
	movdqa	-128(%rbp), %xmm4
	movups	%xmm4, (%r15)
	movdqa	-64(%rbp), %xmm4
	movups	%xmm15, (%r14)
	movups	%xmm10, 0(%r13)
	movups	%xmm4, (%r12)
	movdqa	-80(%rbp), %xmm4
	movups	%xmm4, (%rbx)
	movdqa	-96(%rbp), %xmm4
	movq	-224(%rbp), %rbx
	movups	%xmm4, (%r11)
	movups	%xmm14, (%r10)
	movups	%xmm0, (%r9)
	movups	%xmm2, (%r8)
	movups	%xmm6, (%rdi)
	movups	%xmm7, (%rsi)
	movups	%xmm1, (%rcx)
	movq	-216(%rbp), %rcx
	movups	%xmm9, (%rcx)
	movups	%xmm8, (%rbx)
	movups	%xmm3, (%rax)
	addq	$224, %rsp
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
	movdqa	-144(%rbp), %xmm3
	movdqa	-160(%rbp), %xmm8
	movdqa	-176(%rbp), %xmm9
	jmp	.L366
	.cfi_endproc
.LFE18796:
	.size	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0, .-_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	.section	.text._ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0, @function
_ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0:
.LFB18797:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	.cfi_offset 15, -24
	movq	%rcx, %r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$2952, %rsp
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	movq	%rdi, -2648(%rbp)
	movq	%rsi, -2808(%rbp)
	movq	%rdx, -2760(%rbp)
	movq	%r8, -2736(%rbp)
	movq	%r9, -2768(%rbp)
	cmpq	$32, %rdx
	jbe	.L594
	movq	%rdi, %r12
	movq	%rdi, %r14
	shrq	$3, %r12
	movq	%r12, %rax
	andl	$7, %eax
	jne	.L595
	movq	%rdx, %r11
	movq	%rdi, %r13
	movq	%r8, %rax
.L381:
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
	movq	%rsi, %r10
	movq	%rsi, %r8
	xorq	%rax, %rdx
	shrq	$11, %r8
	rolq	$24, %r10
	leaq	(%rsi,%rsi,8), %rax
	leaq	4(%r9), %rsi
	addq	%rdx, %r10
	xorq	%r8, %rax
	addq	$5, %r9
	xorq	%rsi, %rax
	movq	%r10, %r8
	movq	%r10, %rsi
	shrq	$11, %rsi
	rolq	$24, %r8
	addq	%rax, %r8
	movq	%rsi, %rbx
	leaq	(%r10,%r10,8), %rsi
	xorq	%rbx, %rsi
	movq	%r8, %rbx
	leaq	(%r8,%r8,8), %r10
	rolq	$24, %r8
	xorq	%r9, %rsi
	shrq	$11, %rbx
	xorq	%rbx, %r10
	addq	%rsi, %r8
	movq	-2736(%rbp), %rbx
	movl	%esi, %esi
	movq	%r10, %xmm0
	movq	%r8, %xmm6
	movl	%ecx, %r10d
	movabsq	$34359738359, %r8
	punpcklqdq	%xmm6, %xmm0
	movq	%r9, 16(%rbx)
	movl	%edx, %r9d
	movups	%xmm0, (%rbx)
	movq	%r11, %rbx
	shrq	$3, %rbx
	cmpq	%r8, %r11
	movl	$4294967295, %r8d
	movl	%edi, %r11d
	cmova	%r8, %rbx
	shrq	$32, %rdi
	movl	%eax, %r8d
	shrq	$32, %rcx
	shrq	$32, %rdx
	imulq	%rbx, %r11
	shrq	$32, %rax
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
	addq	%r13, %r11
	imulq	%rbx, %r8
	shrq	$32, %rcx
	salq	$6, %r10
	addq	%r13, %rdi
	shrq	$32, %r9
	salq	$6, %rcx
	addq	%r13, %r10
	shrq	$32, %rdx
	salq	$6, %r9
	addq	%r13, %rcx
	shrq	$32, %r8
	salq	$6, %rdx
	addq	%r13, %r9
	salq	$6, %r8
	addq	%r13, %rdx
	addq	%r13, %r8
	imulq	%rbx, %rax
	imulq	%rbx, %rsi
	xorl	%ebx, %ebx
	shrq	$32, %rax
	shrq	$32, %rsi
	salq	$6, %rax
	salq	$6, %rsi
	addq	%r13, %rax
	addq	%r13, %rsi
.L383:
	movdqa	(%r10,%rbx,8), %xmm2
	movdqa	(%r11,%rbx,8), %xmm4
	movdqa	(%rdi,%rbx,8), %xmm0
	movdqa	%xmm2, %xmm3
	movdqa	%xmm4, %xmm1
	pcmpeqd	%xmm4, %xmm3
	psubq	%xmm2, %xmm1
	pand	%xmm3, %xmm1
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm4, %xmm3
	por	%xmm3, %xmm1
	movdqa	%xmm4, %xmm3
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm5
	pand	%xmm1, %xmm3
	pandn	%xmm2, %xmm5
	pand	%xmm1, %xmm2
	por	%xmm5, %xmm3
	movdqa	%xmm1, %xmm5
	pandn	%xmm4, %xmm5
	movdqa	%xmm0, %xmm4
	movdqa	%xmm3, %xmm1
	pcmpeqd	%xmm3, %xmm4
	psubq	%xmm0, %xmm1
	por	%xmm5, %xmm2
	pand	%xmm4, %xmm1
	movdqa	%xmm0, %xmm4
	pcmpgtd	%xmm3, %xmm4
	por	%xmm4, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm0
	pandn	%xmm3, %xmm4
	movdqa	%xmm2, %xmm3
	por	%xmm4, %xmm0
	movdqa	(%rcx,%rbx,8), %xmm4
	pcmpeqd	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	psubq	%xmm2, %xmm1
	pand	%xmm3, %xmm1
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm0, %xmm3
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm1, %xmm0
	movdqa	%xmm4, %xmm1
	pandn	%xmm2, %xmm3
	movdqa	(%rdx,%rbx,8), %xmm2
	por	%xmm3, %xmm0
	movdqa	%xmm2, %xmm3
	psubq	%xmm2, %xmm1
	movaps	%xmm0, (%r15,%rbx,8)
	movdqa	(%r9,%rbx,8), %xmm0
	pcmpeqd	%xmm4, %xmm3
	pand	%xmm3, %xmm1
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm4, %xmm3
	por	%xmm3, %xmm1
	movdqa	%xmm4, %xmm3
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm5
	pand	%xmm1, %xmm3
	pandn	%xmm2, %xmm5
	pand	%xmm1, %xmm2
	por	%xmm5, %xmm3
	movdqa	%xmm1, %xmm5
	pandn	%xmm4, %xmm5
	movdqa	%xmm0, %xmm4
	movdqa	%xmm3, %xmm1
	pcmpeqd	%xmm3, %xmm4
	psubq	%xmm0, %xmm1
	por	%xmm5, %xmm2
	pand	%xmm4, %xmm1
	movdqa	%xmm0, %xmm4
	pcmpgtd	%xmm3, %xmm4
	por	%xmm4, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm0
	pandn	%xmm3, %xmm4
	movdqa	%xmm2, %xmm3
	por	%xmm4, %xmm0
	movdqa	(%r8,%rbx,8), %xmm4
	pcmpeqd	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	psubq	%xmm2, %xmm1
	pand	%xmm3, %xmm1
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm0, %xmm3
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm1, %xmm0
	movdqa	%xmm4, %xmm1
	pandn	%xmm2, %xmm3
	movdqa	(%rsi,%rbx,8), %xmm2
	por	%xmm3, %xmm0
	movdqa	%xmm2, %xmm3
	psubq	%xmm2, %xmm1
	movaps	%xmm0, 64(%r15,%rbx,8)
	movdqa	(%rax,%rbx,8), %xmm0
	pcmpeqd	%xmm4, %xmm3
	pand	%xmm3, %xmm1
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm4, %xmm3
	por	%xmm3, %xmm1
	movdqa	%xmm4, %xmm3
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm5
	pand	%xmm1, %xmm3
	pandn	%xmm2, %xmm5
	pand	%xmm1, %xmm2
	por	%xmm5, %xmm3
	movdqa	%xmm1, %xmm5
	pandn	%xmm4, %xmm5
	movdqa	%xmm0, %xmm4
	movdqa	%xmm3, %xmm1
	pcmpeqd	%xmm3, %xmm4
	psubq	%xmm0, %xmm1
	por	%xmm5, %xmm2
	pand	%xmm4, %xmm1
	movdqa	%xmm0, %xmm4
	pcmpgtd	%xmm3, %xmm4
	por	%xmm4, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm0
	pandn	%xmm3, %xmm4
	movdqa	%xmm2, %xmm3
	por	%xmm4, %xmm0
	pcmpeqd	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	psubq	%xmm2, %xmm1
	pand	%xmm3, %xmm1
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm0, %xmm3
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm1, %xmm0
	pandn	%xmm2, %xmm3
	por	%xmm3, %xmm0
	movaps	%xmm0, 128(%r15,%rbx,8)
	addq	$2, %rbx
	cmpq	$8, %rbx
	jne	.L383
	movq	(%r15), %xmm2
	movdqa	16(%r15), %xmm0
	leaq	192(%r15), %r13
	movdqa	(%r15), %xmm1
	punpcklqdq	%xmm2, %xmm2
	pxor	%xmm2, %xmm1
	pxor	%xmm2, %xmm0
	por	%xmm1, %xmm0
	movdqa	32(%r15), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	48(%r15), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	64(%r15), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	80(%r15), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	96(%r15), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	112(%r15), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	128(%r15), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	144(%r15), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	160(%r15), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	176(%r15), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	pxor	%xmm1, %xmm1
	pcmpeqd	%xmm1, %xmm0
	pshufd	$177, %xmm0, %xmm1
	pand	%xmm1, %xmm0
	movmskpd	%xmm0, %eax
	cmpl	$3, %eax
	je	.L384
	movdqa	.LC4(%rip), %xmm0
	movl	$2, %esi
	movq	%r15, %rdi
	movups	%xmm0, 192(%r15)
	movups	%xmm0, 208(%r15)
	movups	%xmm0, 224(%r15)
	movups	%xmm0, 240(%r15)
	movups	%xmm0, 256(%r15)
	call	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	movq	(%r15), %xmm2
	pcmpeqd	%xmm0, %xmm0
	movq	184(%r15), %xmm1
	punpcklqdq	%xmm1, %xmm1
	punpcklqdq	%xmm2, %xmm2
	paddq	%xmm1, %xmm0
	pcmpeqd	%xmm2, %xmm0
	pshufd	$177, %xmm0, %xmm3
	pand	%xmm3, %xmm0
	movmskpd	%xmm0, %eax
	cmpl	$3, %eax
	jne	.L386
	movq	-2760(%rbp), %rsi
	leaq	-64(%rbp), %rdx
	movq	%r13, %rcx
	movdqa	%xmm2, %xmm0
	movq	-2648(%rbp), %rdi
	call	_ZN3hwy6N_SSE26detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L369
.L386:
	movq	96(%r15), %rax
	cmpq	%rax, 88(%r15)
	jne	.L482
	cmpq	80(%r15), %rax
	jne	.L425
	cmpq	72(%r15), %rax
	jne	.L483
	cmpq	64(%r15), %rax
	jne	.L484
	cmpq	56(%r15), %rax
	jne	.L485
	cmpq	48(%r15), %rax
	jne	.L486
	cmpq	40(%r15), %rax
	jne	.L487
	cmpq	32(%r15), %rax
	jne	.L488
	cmpq	24(%r15), %rax
	jne	.L489
	cmpq	16(%r15), %rax
	jne	.L490
	cmpq	8(%r15), %rax
	jne	.L491
	xorl	%ebx, %ebx
	movl	$1, %edx
	cmpq	%rax, (%r15)
	jne	.L428
.L427:
	movq	%rax, %xmm2
	punpcklqdq	%xmm2, %xmm2
.L593:
	movl	$1, -2812(%rbp)
.L423:
	cmpq	$0, -2768(%rbp)
	je	.L596
	movq	-2760(%rbp), %rax
	movq	-2648(%rbp), %rsi
	movaps	%xmm2, -2624(%rbp)
	subq	$2, %rax
	movdqu	(%rsi,%rax,8), %xmm6
	movq	%rax, %rcx
	movq	%rax, -2672(%rbp)
	andl	$7, %ecx
	movq	%rcx, -2688(%rbp)
	movaps	%xmm6, -2800(%rbp)
	andl	$6, %eax
	je	.L492
	movdqu	(%rsi), %xmm4
	movdqa	%xmm2, %xmm1
	movaps	%xmm2, -2848(%rbp)
	movdqa	%xmm4, %xmm0
	psubq	%xmm4, %xmm1
	movaps	%xmm4, -2640(%rbp)
	pcmpeqd	%xmm2, %xmm0
	pand	%xmm0, %xmm1
	movdqa	%xmm4, %xmm0
	pcmpgtd	%xmm2, %xmm0
	por	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm0
	pshufd	$245, %xmm1, %xmm1
	pxor	%xmm1, %xmm0
	movaps	%xmm1, -2832(%rbp)
	movmskpd	%xmm0, %ebx
	movq	%rbx, %rdi
	salq	$4, %rbx
	call	__popcountdi2@PLT
	leaq	_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %rcx
	movdqa	-2640(%rbp), %xmm4
	movdqa	(%rcx,%rbx), %xmm0
	movq	%rcx, -2728(%rbp)
	cltq
	movq	%rax, -2704(%rbp)
	movaps	%xmm0, -336(%rbp)
	movzbl	-328(%rbp), %edx
	movd	%xmm0, %r9d
	movaps	%xmm0, -352(%rbp)
	andl	$15, %r9d
	movq	%rdx, %rcx
	movzbl	-343(%rbp), %edx
	movaps	%xmm0, -320(%rbp)
	movzbl	-313(%rbp), %eax
	movaps	%xmm0, -368(%rbp)
	andl	$15, %ecx
	movq	%rdx, %rdi
	movzbl	-358(%rbp), %edx
	movaps	%xmm0, -304(%rbp)
	movzbl	-298(%rbp), %r14d
	andl	$15, %edi
	andl	$15, %eax
	movaps	%xmm0, -224(%rbp)
	movzbl	-223(%rbp), %r10d
	andl	$15, %edx
	movaps	%xmm0, -240(%rbp)
	andl	$15, %r14d
	movzbl	-238(%rbp), %r11d
	movaps	%xmm0, -256(%rbp)
	movzbl	-253(%rbp), %ebx
	andl	$15, %r10d
	movaps	%xmm0, -272(%rbp)
	movzbl	-268(%rbp), %r12d
	andl	$15, %r11d
	movaps	%xmm0, -288(%rbp)
	movzbl	-283(%rbp), %r13d
	andl	$15, %ebx
	movaps	%xmm0, -384(%rbp)
	andl	$15, %r12d
	movq	%rcx, -2640(%rbp)
	andl	$15, %r13d
	movq	%rdi, -2720(%rbp)
	movq	%rdx, -2656(%rbp)
	movzbl	-373(%rbp), %edx
	movaps	%xmm4, -208(%rbp)
	movaps	%xmm0, -400(%rbp)
	movzbl	-388(%rbp), %ecx
	andl	$15, %edx
	movdqa	.LC1(%rip), %xmm6
	movq	-2704(%rbp), %xmm3
	movaps	%xmm0, -416(%rbp)
	movzbl	-403(%rbp), %esi
	movaps	%xmm0, -432(%rbp)
	andl	$15, %ecx
	movdqa	%xmm6, %xmm5
	movzbl	-418(%rbp), %edi
	movaps	%xmm0, -448(%rbp)
	movzbl	-208(%rbp,%rax), %eax
	andl	$15, %esi
	punpcklqdq	%xmm3, %xmm3
	movzbl	-208(%rbp,%r14), %r14d
	andl	$15, %edi
	movzbl	-208(%rbp,%r13), %r13d
	movaps	%xmm6, -2752(%rbp)
	salq	$8, %rax
	movzbl	-208(%rbp,%rbx), %ebx
	movzbl	-208(%rbp,%r12), %r12d
	pcmpeqd	%xmm3, %xmm5
	orq	%r14, %rax
	movzbl	-433(%rbp), %r8d
	movzbl	-208(%rbp,%r11), %r11d
	salq	$8, %rax
	movzbl	-208(%rbp,%rdi), %edi
	movzbl	-208(%rbp,%r10), %r10d
	orq	%r13, %rax
	andl	$15, %r8d
	movzbl	-208(%rbp,%rsi), %esi
	movzbl	-208(%rbp,%rcx), %ecx
	salq	$8, %rax
	movzbl	-208(%rbp,%r8), %r8d
	movzbl	-208(%rbp,%rdx), %edx
	movzbl	-208(%rbp,%r9), %r9d
	orq	%r12, %rax
	salq	$8, %rax
	salq	$8, %r8
	orq	%rbx, %rax
	salq	$8, %rax
	orq	%r11, %rax
	salq	$8, %rax
	orq	%r10, %rax
	salq	$8, %rax
	orq	%rdi, %r8
	movq	-2720(%rbp), %rdi
	salq	$8, %r8
	orq	%r9, %rax
	orq	%rsi, %r8
	salq	$8, %r8
	orq	%rcx, %r8
	movq	-2640(%rbp), %rcx
	movq	%rax, -2640(%rbp)
	salq	$8, %r8
	orq	%rdx, %r8
	movq	-2656(%rbp), %rdx
	salq	$8, %r8
	movzbl	-208(%rbp,%rdx), %edx
	orq	%rdx, %r8
	movzbl	-208(%rbp,%rdi), %edx
	salq	$8, %r8
	orq	%rdx, %r8
	movzbl	-208(%rbp,%rcx), %edx
	salq	$8, %r8
	orq	%rdx, %r8
	movq	%r8, -2632(%rbp)
	movdqa	.LC0(%rip), %xmm7
	movdqa	-2832(%rbp), %xmm1
	movdqa	-2848(%rbp), %xmm2
	movdqa	%xmm7, %xmm0
	movaps	%xmm7, -2784(%rbp)
	psubq	%xmm3, %xmm0
	pcmpgtd	%xmm6, %xmm3
	pand	%xmm5, %xmm0
	por	%xmm3, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L433
	movq	-2648(%rbp), %rsi
	movdqa	-2640(%rbp), %xmm6
	movq	%xmm6, (%rsi)
.L433:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rax
	testq	%rax, %rax
	je	.L434
	movq	-2648(%rbp), %rax
	movdqa	-2640(%rbp), %xmm6
	movhps	%xmm6, 8(%rax)
.L434:
	movq	-2648(%rbp), %rax
	movq	-2704(%rbp), %rbx
	movmskpd	%xmm1, %r12d
	movaps	%xmm4, -2720(%rbp)
	movq	%r12, %rdi
	movaps	%xmm2, -2848(%rbp)
	salq	$4, %r12
	leaq	(%rax,%rbx,8), %rbx
	call	__popcountdi2@PLT
	movdqa	-2720(%rbp), %xmm4
	movslq	%eax, %rcx
	movq	%rcx, -2640(%rbp)
	movq	-2728(%rbp), %rcx
	movaps	%xmm4, -192(%rbp)
	movdqa	(%rcx,%r12), %xmm0
	movaps	%xmm0, -576(%rbp)
	movzbl	-568(%rbp), %edx
	movd	%xmm0, %r9d
	movaps	%xmm0, -464(%rbp)
	movzbl	-463(%rbp), %eax
	andl	$15, %r9d
	movq	%rdx, %rcx
	movaps	%xmm0, -592(%rbp)
	movzbl	-583(%rbp), %edx
	andl	$15, %eax
	movaps	%xmm0, -608(%rbp)
	andl	$15, %ecx
	movq	%rdx, %rsi
	movzbl	-598(%rbp), %edx
	movq	%rax, -2704(%rbp)
	movaps	%xmm0, -560(%rbp)
	movzbl	-553(%rbp), %eax
	andl	$15, %esi
	movaps	%xmm0, -544(%rbp)
	movq	%rdx, %rdi
	movzbl	-538(%rbp), %r14d
	andl	$15, %edi
	andl	$15, %eax
	movq	%rcx, -2720(%rbp)
	movq	%rsi, -2656(%rbp)
	andl	$15, %r14d
	movq	%rdi, -2832(%rbp)
	movaps	%xmm0, -480(%rbp)
	movzbl	-478(%rbp), %r10d
	movaps	%xmm0, -496(%rbp)
	movzbl	-493(%rbp), %r11d
	movaps	%xmm0, -512(%rbp)
	movzbl	-508(%rbp), %r12d
	andl	$15, %r10d
	movaps	%xmm0, -528(%rbp)
	movzbl	-523(%rbp), %r13d
	andl	$15, %r11d
	movaps	%xmm0, -624(%rbp)
	andl	$15, %r12d
	movzbl	-613(%rbp), %edx
	movaps	%xmm0, -640(%rbp)
	andl	$15, %r13d
	movzbl	-628(%rbp), %ecx
	movaps	%xmm0, -656(%rbp)
	movzbl	-643(%rbp), %esi
	andl	$15, %edx
	movaps	%xmm0, -672(%rbp)
	movzbl	-658(%rbp), %edi
	andl	$15, %ecx
	movaps	%xmm0, -688(%rbp)
	movzbl	-192(%rbp,%rax), %eax
	movzbl	-192(%rbp,%r14), %r14d
	andl	$15, %esi
	movzbl	-192(%rbp,%r13), %r13d
	movzbl	-673(%rbp), %r8d
	andl	$15, %edi
	salq	$8, %rax
	movzbl	-192(%rbp,%r12), %r12d
	movzbl	-192(%rbp,%r11), %r11d
	orq	%r14, %rax
	andl	$15, %r8d
	movzbl	-192(%rbp,%r10), %r10d
	movzbl	-192(%rbp,%rdi), %edi
	salq	$8, %rax
	movzbl	-192(%rbp,%rsi), %esi
	movzbl	-192(%rbp,%r8), %r8d
	orq	%r13, %rax
	movzbl	-192(%rbp,%rcx), %ecx
	movzbl	-192(%rbp,%rdx), %edx
	salq	$8, %rax
	salq	$8, %r8
	movzbl	-192(%rbp,%r9), %r9d
	orq	%r12, %rax
	salq	$8, %rax
	orq	%r11, %rax
	movq	-2704(%rbp), %r11
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-192(%rbp,%r11), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	salq	$8, %rax
	orq	%rdi, %r8
	movq	-2832(%rbp), %rdi
	salq	$8, %r8
	orq	%r9, %rax
	orq	%rsi, %r8
	movq	-2656(%rbp), %rsi
	movq	%rax, -2704(%rbp)
	salq	$8, %r8
	orq	%rcx, %r8
	movq	-2720(%rbp), %rcx
	salq	$8, %r8
	orq	%rdx, %r8
	movzbl	-192(%rbp,%rdi), %edx
	salq	$8, %r8
	orq	%rdx, %r8
	movzbl	-192(%rbp,%rsi), %edx
	salq	$8, %r8
	orq	%rdx, %r8
	movzbl	-192(%rbp,%rcx), %edx
	salq	$8, %r8
	orq	%rdx, %r8
	movq	%r8, -2696(%rbp)
	movdqa	-2704(%rbp), %xmm6
	testb	$4, -2672(%rbp)
	movdqa	-2848(%rbp), %xmm2
	movups	%xmm6, (%r15)
	je	.L435
	movq	-2648(%rbp), %rax
	movdqa	-2624(%rbp), %xmm6
	movdqa	%xmm2, %xmm1
	movaps	%xmm2, -2880(%rbp)
	movdqu	16(%rax), %xmm3
	movdqa	%xmm6, %xmm0
	pcmpeqd	%xmm3, %xmm0
	psubq	%xmm3, %xmm1
	movaps	%xmm3, -2704(%rbp)
	pand	%xmm0, %xmm1
	movdqa	%xmm3, %xmm0
	pcmpgtd	%xmm6, %xmm0
	por	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm0
	pshufd	$245, %xmm1, %xmm1
	pxor	%xmm1, %xmm0
	movaps	%xmm1, -2864(%rbp)
	movmskpd	%xmm0, %r12d
	movq	%r12, %rdi
	salq	$4, %r12
	call	__popcountdi2@PLT
	movq	-2728(%rbp), %rcx
	movdqa	-2704(%rbp), %xmm3
	cltq
	movdqa	(%rcx,%r12), %xmm0
	movq	%rax, -2720(%rbp)
	movaps	%xmm3, -176(%rbp)
	movaps	%xmm0, -816(%rbp)
	movzbl	-808(%rbp), %edx
	movd	%xmm0, %r9d
	movaps	%xmm0, -704(%rbp)
	movzbl	-703(%rbp), %eax
	andl	$15, %r9d
	movaps	%xmm0, -832(%rbp)
	movq	%rdx, %rdi
	movzbl	-823(%rbp), %edx
	movaps	%xmm0, -800(%rbp)
	movq	%rax, %rcx
	andl	$15, %edi
	movzbl	-793(%rbp), %eax
	movaps	%xmm0, -848(%rbp)
	movq	%rdx, %rsi
	movzbl	-838(%rbp), %edx
	andl	$15, %ecx
	movaps	%xmm0, -784(%rbp)
	andl	$15, %esi
	andl	$15, %eax
	movzbl	-778(%rbp), %r14d
	andl	$15, %edx
	movaps	%xmm0, -720(%rbp)
	movzbl	-718(%rbp), %r10d
	movaps	%xmm0, -736(%rbp)
	andl	$15, %r14d
	movzbl	-733(%rbp), %r11d
	movaps	%xmm0, -752(%rbp)
	movzbl	-748(%rbp), %r12d
	andl	$15, %r10d
	movaps	%xmm0, -768(%rbp)
	movzbl	-763(%rbp), %r13d
	andl	$15, %r11d
	movq	%rcx, -2704(%rbp)
	andl	$15, %r12d
	movq	%rdi, -2656(%rbp)
	andl	$15, %r13d
	movq	%rsi, -2832(%rbp)
	movq	%rdx, -2848(%rbp)
	movaps	%xmm0, -864(%rbp)
	movzbl	-853(%rbp), %edx
	movaps	%xmm0, -880(%rbp)
	movzbl	-868(%rbp), %ecx
	movaps	%xmm0, -896(%rbp)
	movzbl	-883(%rbp), %esi
	andl	$15, %edx
	movaps	%xmm0, -912(%rbp)
	movzbl	-898(%rbp), %edi
	andl	$15, %ecx
	movaps	%xmm0, -928(%rbp)
	movzbl	-176(%rbp,%rax), %eax
	movzbl	-176(%rbp,%r14), %r14d
	andl	$15, %esi
	movzbl	-176(%rbp,%r13), %r13d
	movzbl	-913(%rbp), %r8d
	andl	$15, %edi
	salq	$8, %rax
	movzbl	-176(%rbp,%r12), %r12d
	movzbl	-176(%rbp,%r11), %r11d
	orq	%r14, %rax
	andl	$15, %r8d
	movzbl	-176(%rbp,%r10), %r10d
	movzbl	-176(%rbp,%rdi), %edi
	salq	$8, %rax
	movzbl	-176(%rbp,%rsi), %esi
	movzbl	-176(%rbp,%r8), %r8d
	orq	%r13, %rax
	movzbl	-176(%rbp,%rcx), %ecx
	movzbl	-176(%rbp,%rdx), %edx
	salq	$8, %rax
	salq	$8, %r8
	movzbl	-176(%rbp,%r9), %r9d
	orq	%r12, %rax
	salq	$8, %rax
	orq	%r11, %rax
	movq	-2704(%rbp), %r11
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-176(%rbp,%r11), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	salq	$8, %rax
	orq	%rdi, %r8
	movq	-2656(%rbp), %rdi
	salq	$8, %r8
	orq	%r9, %rax
	orq	%rsi, %r8
	movq	-2832(%rbp), %rsi
	movq	%rax, -2704(%rbp)
	salq	$8, %r8
	orq	%rcx, %r8
	salq	$8, %r8
	orq	%rdx, %r8
	movq	-2848(%rbp), %rdx
	salq	$8, %r8
	movzbl	-176(%rbp,%rdx), %edx
	orq	%rdx, %r8
	movzbl	-176(%rbp,%rsi), %edx
	salq	$8, %r8
	orq	%rdx, %r8
	movzbl	-176(%rbp,%rdi), %edx
	salq	$8, %r8
	orq	%rdx, %r8
	movq	%r8, -2696(%rbp)
	movdqa	-2752(%rbp), %xmm6
	movdqa	-2784(%rbp), %xmm0
	movdqa	-2864(%rbp), %xmm1
	movq	-2720(%rbp), %xmm4
	movdqa	%xmm6, %xmm5
	movdqa	-2880(%rbp), %xmm2
	punpcklqdq	%xmm4, %xmm4
	pcmpeqd	%xmm4, %xmm5
	psubq	%xmm4, %xmm0
	pcmpgtd	%xmm6, %xmm4
	pand	%xmm5, %xmm0
	por	%xmm4, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L436
	movdqa	-2704(%rbp), %xmm6
	movq	%xmm6, (%rbx)
.L436:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rax
	testq	%rax, %rax
	je	.L437
	movdqa	-2704(%rbp), %xmm6
	movhps	%xmm6, 8(%rbx)
.L437:
	movq	-2720(%rbp), %rax
	movmskpd	%xmm1, %r12d
	movaps	%xmm3, -2704(%rbp)
	movq	%r12, %rdi
	movaps	%xmm2, -2864(%rbp)
	salq	$4, %r12
	leaq	(%rbx,%rax,8), %rbx
	call	__popcountdi2@PLT
	movdqa	-2704(%rbp), %xmm3
	movslq	%eax, %r8
	movq	-2728(%rbp), %rax
	movaps	%xmm3, -160(%rbp)
	movdqa	(%rax,%r12), %xmm0
	movaps	%xmm0, -1056(%rbp)
	movzbl	-1048(%rbp), %edx
	movd	%xmm0, %r9d
	movaps	%xmm0, -1072(%rbp)
	andl	$15, %r9d
	movq	%rdx, %rcx
	movzbl	-1063(%rbp), %edx
	movaps	%xmm0, -1088(%rbp)
	movaps	%xmm0, -944(%rbp)
	movzbl	-943(%rbp), %eax
	andl	$15, %ecx
	movq	%rdx, %rdi
	movzbl	-1078(%rbp), %edx
	movaps	%xmm0, -1104(%rbp)
	andl	$15, %eax
	andl	$15, %edi
	movaps	%xmm0, -1040(%rbp)
	andl	$15, %edx
	movq	%rax, -2704(%rbp)
	movzbl	-1033(%rbp), %eax
	movq	%rdx, -2832(%rbp)
	movzbl	-1093(%rbp), %edx
	movq	%rdi, -2656(%rbp)
	andl	$15, %eax
	movaps	%xmm0, -1024(%rbp)
	movq	%rdx, %rdi
	movzbl	-1018(%rbp), %r14d
	andl	$15, %edi
	movq	%rcx, -2720(%rbp)
	movaps	%xmm0, -960(%rbp)
	andl	$15, %r14d
	movzbl	-958(%rbp), %r10d
	movaps	%xmm0, -976(%rbp)
	movzbl	-973(%rbp), %r11d
	movaps	%xmm0, -992(%rbp)
	movzbl	-988(%rbp), %r12d
	andl	$15, %r10d
	movaps	%xmm0, -1008(%rbp)
	movzbl	-1003(%rbp), %r13d
	andl	$15, %r11d
	movq	%rdi, -2848(%rbp)
	andl	$15, %r12d
	movaps	%xmm0, -1120(%rbp)
	andl	$15, %r13d
	movzbl	-1108(%rbp), %edx
	movaps	%xmm0, -1136(%rbp)
	movzbl	-1123(%rbp), %ecx
	movaps	%xmm0, -1152(%rbp)
	movzbl	-1138(%rbp), %esi
	andl	$15, %edx
	movaps	%xmm0, -1168(%rbp)
	movzbl	-160(%rbp,%rax), %eax
	movzbl	-160(%rbp,%r14), %r14d
	andl	$15, %ecx
	movzbl	-160(%rbp,%r13), %r13d
	movzbl	-1153(%rbp), %edi
	andl	$15, %esi
	salq	$8, %rax
	movzbl	-160(%rbp,%r12), %r12d
	movzbl	-160(%rbp,%r11), %r11d
	orq	%r14, %rax
	andl	$15, %edi
	movzbl	-160(%rbp,%r10), %r10d
	movzbl	-160(%rbp,%rsi), %esi
	salq	$8, %rax
	movzbl	-160(%rbp,%rdi), %edi
	movzbl	-160(%rbp,%rcx), %ecx
	orq	%r13, %rax
	movzbl	-160(%rbp,%rdx), %edx
	movzbl	-160(%rbp,%r9), %r9d
	salq	$8, %rax
	salq	$8, %rdi
	orq	%r12, %rax
	salq	$8, %rax
	orq	%r11, %rax
	movq	-2704(%rbp), %r11
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-160(%rbp,%r11), %r10d
	movq	-2848(%rbp), %r11
	salq	$8, %rax
	orq	%r10, %rax
	salq	$8, %rax
	orq	%rsi, %rdi
	movq	-2656(%rbp), %rsi
	salq	$8, %rdi
	orq	%r9, %rax
	orq	%rcx, %rdi
	movq	-2720(%rbp), %rcx
	movq	%rax, -2704(%rbp)
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-160(%rbp,%r11), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movq	-2832(%rbp), %rdx
	salq	$8, %rdi
	movzbl	-160(%rbp,%rdx), %edx
	orq	%rdx, %rdi
	movzbl	-160(%rbp,%rsi), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movzbl	-160(%rbp,%rcx), %edx
	salq	$8, %rdi
	orq	%rdx, %rdi
	movq	%rdi, -2696(%rbp)
	movq	-2640(%rbp), %rax
	movdqa	-2704(%rbp), %xmm6
	movdqa	-2864(%rbp), %xmm2
	movups	%xmm6, (%r15,%rax,8)
	addq	%r8, %rax
	cmpq	$5, -2688(%rbp)
	movq	%rax, -2640(%rbp)
	jbe	.L435
	movq	-2648(%rbp), %rcx
	movdqa	-2624(%rbp), %xmm6
	movdqa	%xmm2, %xmm1
	movaps	%xmm2, -2880(%rbp)
	movdqu	32(%rcx), %xmm3
	movdqa	%xmm6, %xmm0
	pcmpeqd	%xmm3, %xmm0
	psubq	%xmm3, %xmm1
	movaps	%xmm3, -2704(%rbp)
	pand	%xmm0, %xmm1
	movdqa	%xmm3, %xmm0
	pcmpgtd	%xmm6, %xmm0
	por	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm0
	pshufd	$245, %xmm1, %xmm1
	pxor	%xmm1, %xmm0
	movaps	%xmm1, -2864(%rbp)
	movmskpd	%xmm0, %r12d
	movq	%r12, %rdi
	salq	$4, %r12
	call	__popcountdi2@PLT
	movdqa	-2704(%rbp), %xmm3
	movslq	%eax, %rcx
	movq	-2728(%rbp), %rax
	movq	%rcx, -2720(%rbp)
	movdqa	(%rax,%r12), %xmm0
	movaps	%xmm3, -144(%rbp)
	movaps	%xmm0, -1184(%rbp)
	movzbl	-1183(%rbp), %eax
	movd	%xmm0, %r10d
	movaps	%xmm0, -1200(%rbp)
	andl	$15, %r10d
	andl	$15, %eax
	movaps	%xmm0, -1296(%rbp)
	movzbl	-1288(%rbp), %edx
	movq	%rax, -2704(%rbp)
	movzbl	-1198(%rbp), %eax
	movaps	%xmm0, -1280(%rbp)
	movq	%rdx, %rdi
	movaps	%xmm0, -1312(%rbp)
	movq	%rax, %rsi
	movzbl	-1303(%rbp), %edx
	andl	$15, %edi
	movzbl	-1273(%rbp), %eax
	movaps	%xmm0, -1264(%rbp)
	movzbl	-1258(%rbp), %r14d
	andl	$15, %esi
	andl	$15, %edx
	movaps	%xmm0, -1216(%rbp)
	movzbl	-1213(%rbp), %r11d
	andl	$15, %eax
	movaps	%xmm0, -1232(%rbp)
	andl	$15, %r14d
	movzbl	-1228(%rbp), %r12d
	movaps	%xmm0, -1248(%rbp)
	movzbl	-1243(%rbp), %r13d
	andl	$15, %r11d
	movq	%rsi, -2656(%rbp)
	andl	$15, %r12d
	movq	%rdi, -2832(%rbp)
	andl	$15, %r13d
	movaps	%xmm0, -1328(%rbp)
	movq	%rdx, -2848(%rbp)
	movzbl	-1318(%rbp), %edx
	movaps	%xmm0, -1344(%rbp)
	movzbl	-1333(%rbp), %ecx
	movaps	%xmm0, -1360(%rbp)
	movzbl	-1348(%rbp), %esi
	andl	$15, %edx
	movaps	%xmm0, -1376(%rbp)
	movzbl	-1363(%rbp), %edi
	andl	$15, %ecx
	movaps	%xmm0, -1392(%rbp)
	movzbl	-1378(%rbp), %r8d
	andl	$15, %esi
	movaps	%xmm0, -1408(%rbp)
	movzbl	-144(%rbp,%rax), %eax
	movzbl	-144(%rbp,%r14), %r14d
	andl	$15, %edi
	movzbl	-144(%rbp,%r13), %r13d
	movzbl	-1393(%rbp), %r9d
	andl	$15, %r8d
	salq	$8, %rax
	movzbl	-144(%rbp,%r12), %r12d
	movzbl	-144(%rbp,%r11), %r11d
	orq	%r14, %rax
	movq	-2656(%rbp), %r14
	andl	$15, %r9d
	movzbl	-144(%rbp,%r8), %r8d
	salq	$8, %rax
	movzbl	-144(%rbp,%rdi), %edi
	movzbl	-144(%rbp,%r9), %r9d
	orq	%r13, %rax
	movzbl	-144(%rbp,%rsi), %esi
	movzbl	-144(%rbp,%rcx), %ecx
	salq	$8, %rax
	salq	$8, %r9
	movzbl	-144(%rbp,%rdx), %edx
	movzbl	-144(%rbp,%r10), %r10d
	orq	%r12, %rax
	salq	$8, %rax
	orq	%r11, %rax
	movzbl	-144(%rbp,%r14), %r11d
	salq	$8, %rax
	orq	%r11, %rax
	movq	-2704(%rbp), %r11
	salq	$8, %rax
	movzbl	-144(%rbp,%r11), %r11d
	orq	%r11, %rax
	salq	$8, %rax
	orq	%r8, %r9
	salq	$8, %r9
	orq	%r10, %rax
	orq	%rdi, %r9
	movq	-2832(%rbp), %rdi
	salq	$8, %r9
	orq	%rsi, %r9
	salq	$8, %r9
	orq	%rcx, %r9
	salq	$8, %r9
	orq	%rdx, %r9
	movq	-2848(%rbp), %rdx
	salq	$8, %r9
	movzbl	-144(%rbp,%rdx), %edx
	orq	%rdx, %r9
	movzbl	-144(%rbp,%rdi), %edx
	movq	%rax, -2704(%rbp)
	movdqa	-2752(%rbp), %xmm6
	movdqa	-2784(%rbp), %xmm0
	salq	$8, %r9
	movq	-2720(%rbp), %xmm4
	orq	%rdx, %r9
	movdqa	-2864(%rbp), %xmm1
	movdqa	%xmm6, %xmm5
	movq	%r9, -2696(%rbp)
	movdqa	-2880(%rbp), %xmm2
	punpcklqdq	%xmm4, %xmm4
	pcmpeqd	%xmm4, %xmm5
	psubq	%xmm4, %xmm0
	pcmpgtd	%xmm6, %xmm4
	pand	%xmm5, %xmm0
	por	%xmm4, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L438
	movdqa	-2704(%rbp), %xmm6
	movq	%xmm6, (%rbx)
.L438:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rax
	testq	%rax, %rax
	je	.L439
	movdqa	-2704(%rbp), %xmm6
	movhps	%xmm6, 8(%rbx)
.L439:
	movq	-2720(%rbp), %rax
	movmskpd	%xmm1, %r12d
	movaps	%xmm3, -2704(%rbp)
	movq	%r12, %rdi
	movaps	%xmm2, -2864(%rbp)
	salq	$4, %r12
	leaq	(%rbx,%rax,8), %rbx
	call	__popcountdi2@PLT
	movdqa	-2704(%rbp), %xmm3
	movslq	%eax, %r8
	movq	-2728(%rbp), %rax
	movaps	%xmm3, -128(%rbp)
	movdqa	(%rax,%r12), %xmm0
	movaps	%xmm0, -1424(%rbp)
	movzbl	-1423(%rbp), %eax
	movd	%xmm0, %ecx
	movaps	%xmm0, -1440(%rbp)
	andl	$15, %ecx
	andl	$15, %eax
	movaps	%xmm0, -1536(%rbp)
	movzbl	-1528(%rbp), %edx
	movq	%rax, -2720(%rbp)
	movzbl	-1438(%rbp), %eax
	movaps	%xmm0, -1520(%rbp)
	movq	%rdx, %rdi
	movq	%rax, %rsi
	movaps	%xmm0, -1552(%rbp)
	movzbl	-1513(%rbp), %eax
	andl	$15, %edi
	movzbl	-1543(%rbp), %edx
	movaps	%xmm0, -1504(%rbp)
	movzbl	-1498(%rbp), %r14d
	andl	$15, %esi
	andl	$15, %eax
	movq	%rsi, -2656(%rbp)
	andl	$15, %edx
	movq	%rdi, -2832(%rbp)
	andl	$15, %r14d
	movq	%rdx, -2848(%rbp)
	movaps	%xmm0, -1456(%rbp)
	movzbl	-1453(%rbp), %r11d
	movaps	%xmm0, -1472(%rbp)
	movzbl	-1468(%rbp), %r12d
	movaps	%xmm0, -1488(%rbp)
	movzbl	-1483(%rbp), %r13d
	andl	$15, %r11d
	movaps	%xmm0, -1568(%rbp)
	movzbl	-1558(%rbp), %edx
	andl	$15, %r12d
	movq	%rcx, -2704(%rbp)
	andl	$15, %r13d
	movaps	%xmm0, -1584(%rbp)
	movzbl	-1573(%rbp), %ecx
	andl	$15, %edx
	movaps	%xmm0, -1600(%rbp)
	movzbl	-1588(%rbp), %esi
	movaps	%xmm0, -1616(%rbp)
	movzbl	-1603(%rbp), %edi
	andl	$15, %ecx
	movaps	%xmm0, -1632(%rbp)
	movzbl	-1618(%rbp), %r9d
	andl	$15, %esi
	movaps	%xmm0, -1648(%rbp)
	movzbl	-128(%rbp,%rax), %eax
	andl	$15, %edi
	movzbl	-128(%rbp,%r14), %r14d
	movzbl	-128(%rbp,%r13), %r13d
	movzbl	-128(%rbp,%r12), %r12d
	andl	$15, %r9d
	salq	$8, %rax
	movzbl	-128(%rbp,%rdi), %edi
	movzbl	-128(%rbp,%rsi), %esi
	orq	%r14, %rax
	movzbl	-128(%rbp,%r11), %r11d
	movzbl	-1633(%rbp), %r10d
	salq	$8, %rax
	movzbl	-128(%rbp,%r9), %r9d
	movzbl	-128(%rbp,%rcx), %ecx
	orq	%r13, %rax
	movq	-2720(%rbp), %r14
	andl	$15, %r10d
	movzbl	-128(%rbp,%rdx), %edx
	salq	$8, %rax
	movzbl	-128(%rbp,%r10), %r10d
	orq	%r12, %rax
	movq	-2656(%rbp), %r12
	salq	$8, %rax
	salq	$8, %r10
	orq	%r11, %rax
	movzbl	-128(%rbp,%r12), %r11d
	salq	$8, %rax
	orq	%r11, %rax
	movzbl	-128(%rbp,%r14), %r11d
	salq	$8, %rax
	orq	%r11, %rax
	movq	-2704(%rbp), %r11
	salq	$8, %rax
	orq	%r9, %r10
	salq	$8, %r10
	movzbl	-128(%rbp,%r11), %r11d
	orq	%rdi, %r10
	movq	-2832(%rbp), %rdi
	salq	$8, %r10
	orq	%r11, %rax
	orq	%rsi, %r10
	movq	%rax, -2704(%rbp)
	salq	$8, %r10
	orq	%rcx, %r10
	salq	$8, %r10
	orq	%rdx, %r10
	movq	-2848(%rbp), %rdx
	salq	$8, %r10
	movzbl	-128(%rbp,%rdx), %edx
	orq	%rdx, %r10
	movzbl	-128(%rbp,%rdi), %edx
	salq	$8, %r10
	orq	%rdx, %r10
	movq	%r10, -2696(%rbp)
	movq	-2640(%rbp), %rax
	movdqa	-2704(%rbp), %xmm6
	movdqa	-2864(%rbp), %xmm2
	movups	%xmm6, (%r15,%rax,8)
	addq	%r8, %rax
	movq	%rax, -2640(%rbp)
.L435:
	movq	-2688(%rbp), %rcx
	leaq	-2(%rcx), %rax
	leaq	1(%rcx), %rdx
	movq	-2640(%rbp), %rcx
	andq	$-2, %rax
	addq	$2, %rax
	cmpq	$2, %rdx
	movl	$2, %edx
	cmovbe	%rdx, %rax
	leaq	0(,%rcx,8), %r13
.L432:
	movq	-2688(%rbp), %rcx
	cmpq	%rax, %rcx
	je	.L440
	movdqa	-2752(%rbp), %xmm7
	subq	%rax, %rcx
	movdqa	%xmm2, %xmm6
	movdqa	-2784(%rbp), %xmm1
	movq	%rcx, %xmm0
	movq	-2648(%rbp), %rcx
	movaps	%xmm2, -2896(%rbp)
	punpcklqdq	%xmm0, %xmm0
	movdqa	%xmm7, %xmm3
	pcmpeqd	%xmm0, %xmm3
	movdqu	(%rcx,%rax,8), %xmm5
	psubq	%xmm0, %xmm1
	pcmpgtd	%xmm7, %xmm0
	psubq	%xmm5, %xmm6
	movaps	%xmm5, -2688(%rbp)
	pand	%xmm3, %xmm1
	por	%xmm1, %xmm0
	movdqa	%xmm6, %xmm1
	movdqa	-2624(%rbp), %xmm6
	pshufd	$245, %xmm0, %xmm4
	movdqa	%xmm6, %xmm3
	movaps	%xmm4, -2864(%rbp)
	pcmpeqd	%xmm5, %xmm3
	pand	%xmm3, %xmm1
	movdqa	%xmm5, %xmm3
	pcmpgtd	%xmm6, %xmm3
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm6
	movaps	%xmm1, -2880(%rbp)
	pandn	%xmm4, %xmm6
	movmskpd	%xmm6, %r12d
	movq	%r12, %rdi
	salq	$4, %r12
	call	__popcountdi2@PLT
	movq	-2728(%rbp), %rcx
	movdqa	-2688(%rbp), %xmm5
	cltq
	movdqa	(%rcx,%r12), %xmm0
	movq	%rax, -2704(%rbp)
	movaps	%xmm5, -112(%rbp)
	movaps	%xmm0, -1664(%rbp)
	movzbl	-1663(%rbp), %eax
	movd	%xmm0, %ecx
	movaps	%xmm0, -1680(%rbp)
	andl	$15, %ecx
	movq	%rax, %rdi
	movzbl	-1678(%rbp), %eax
	movaps	%xmm0, -1760(%rbp)
	movaps	%xmm0, -1776(%rbp)
	movzbl	-1768(%rbp), %edx
	andl	$15, %edi
	movq	%rax, %rsi
	movzbl	-1753(%rbp), %eax
	movaps	%xmm0, -1744(%rbp)
	movzbl	-1738(%rbp), %r14d
	andl	$15, %esi
	andl	$15, %edx
	movaps	%xmm0, -1696(%rbp)
	movzbl	-1693(%rbp), %r10d
	andl	$15, %eax
	movaps	%xmm0, -1712(%rbp)
	andl	$15, %r14d
	movzbl	-1708(%rbp), %r11d
	movaps	%xmm0, -1728(%rbp)
	movzbl	-1723(%rbp), %r12d
	andl	$15, %r10d
	movq	%rcx, -2688(%rbp)
	andl	$15, %r11d
	movq	%rdi, -2720(%rbp)
	andl	$15, %r12d
	movq	%rsi, -2656(%rbp)
	movaps	%xmm0, -1792(%rbp)
	movq	%rdx, -2832(%rbp)
	movzbl	-1783(%rbp), %edx
	movaps	%xmm0, -1808(%rbp)
	movaps	%xmm0, -1824(%rbp)
	movq	%rdx, %r8
	movzbl	-1813(%rbp), %ecx
	movzbl	-1798(%rbp), %edx
	movaps	%xmm0, -1840(%rbp)
	andl	$15, %r8d
	movzbl	-1828(%rbp), %esi
	movaps	%xmm0, -1856(%rbp)
	andl	$15, %edx
	andl	$15, %ecx
	movzbl	-1843(%rbp), %edi
	movaps	%xmm0, -1872(%rbp)
	andl	$15, %esi
	movaps	%xmm0, -1888(%rbp)
	movzbl	-112(%rbp,%rax), %eax
	movzbl	-112(%rbp,%r14), %r14d
	andl	$15, %edi
	movzbl	-112(%rbp,%r12), %r12d
	movzbl	-112(%rbp,%r11), %r11d
	movq	%r8, -2848(%rbp)
	salq	$8, %rax
	movzbl	-112(%rbp,%r10), %r10d
	movzbl	-1873(%rbp), %r9d
	orq	%r14, %rax
	movzbl	-112(%rbp,%rdi), %edi
	movzbl	-112(%rbp,%rsi), %esi
	salq	$8, %rax
	movq	-2720(%rbp), %r14
	andl	$15, %r9d
	movzbl	-1858(%rbp), %r8d
	orq	%r12, %rax
	movzbl	-112(%rbp,%r9), %r9d
	movzbl	-112(%rbp,%rcx), %ecx
	salq	$8, %rax
	andl	$15, %r8d
	movzbl	-112(%rbp,%rdx), %edx
	orq	%r11, %rax
	movzbl	-112(%rbp,%r8), %r8d
	movq	-2688(%rbp), %r11
	salq	$8, %rax
	orq	%r10, %rax
	movq	-2656(%rbp), %r10
	salq	$8, %rax
	movzbl	-112(%rbp,%r10), %r10d
	orq	%r10, %rax
	movzbl	-112(%rbp,%r14), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-112(%rbp,%r11), %r10d
	salq	$8, %rax
	salq	$8, %r9
	orq	%r8, %r9
	movq	-2848(%rbp), %r8
	orq	%r10, %rax
	salq	$8, %r9
	orq	%rdi, %r9
	salq	$8, %r9
	orq	%rsi, %r9
	salq	$8, %r9
	orq	%rcx, %r9
	salq	$8, %r9
	orq	%rdx, %r9
	movzbl	-112(%rbp,%r8), %edx
	movq	%rax, -2688(%rbp)
	movdqa	-2752(%rbp), %xmm7
	movdqa	-2784(%rbp), %xmm0
	salq	$8, %r9
	movq	-2704(%rbp), %xmm3
	orq	%rdx, %r9
	movq	-2832(%rbp), %rdx
	movdqa	%xmm7, %xmm6
	salq	$8, %r9
	movdqa	-2864(%rbp), %xmm4
	movdqa	-2880(%rbp), %xmm1
	punpcklqdq	%xmm3, %xmm3
	movzbl	-112(%rbp,%rdx), %edx
	movdqa	-2896(%rbp), %xmm2
	pcmpeqd	%xmm3, %xmm6
	psubq	%xmm3, %xmm0
	pcmpgtd	%xmm7, %xmm3
	orq	%rdx, %r9
	movq	%r9, -2680(%rbp)
	pand	%xmm6, %xmm0
	por	%xmm3, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L441
	movdqa	-2688(%rbp), %xmm6
	movq	%xmm6, (%rbx)
.L441:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rax
	testq	%rax, %rax
	je	.L442
	movdqa	-2688(%rbp), %xmm6
	movhps	%xmm6, 8(%rbx)
.L442:
	pand	%xmm4, %xmm1
	movq	-2704(%rbp), %rax
	movaps	%xmm5, -2688(%rbp)
	movmskpd	%xmm1, %r12d
	movaps	%xmm2, -2864(%rbp)
	movq	%r12, %rdi
	leaq	(%rbx,%rax,8), %rbx
	salq	$4, %r12
	call	__popcountdi2@PLT
	movq	-2728(%rbp), %rcx
	movdqa	-2688(%rbp), %xmm5
	movl	%eax, -2848(%rbp)
	movdqa	(%rcx,%r12), %xmm0
	movaps	%xmm5, -96(%rbp)
	movaps	%xmm0, -1904(%rbp)
	movzbl	-1903(%rbp), %eax
	movd	%xmm0, %ecx
	movaps	%xmm0, -2016(%rbp)
	movzbl	-2008(%rbp), %edx
	andl	$15, %ecx
	movq	%rax, %rdi
	movaps	%xmm0, -1920(%rbp)
	movzbl	-1918(%rbp), %eax
	andl	$15, %edx
	movaps	%xmm0, -2032(%rbp)
	andl	$15, %edi
	movq	%rdx, -2656(%rbp)
	movzbl	-2023(%rbp), %edx
	movq	%rax, %rsi
	movaps	%xmm0, -2000(%rbp)
	movzbl	-1993(%rbp), %eax
	andl	$15, %esi
	movaps	%xmm0, -1984(%rbp)
	movq	%rdx, %r8
	movzbl	-1978(%rbp), %r14d
	andl	$15, %r8d
	andl	$15, %eax
	movq	%rcx, -2688(%rbp)
	movq	%rdi, -2704(%rbp)
	andl	$15, %r14d
	movq	%rsi, -2720(%rbp)
	movq	%r8, -2832(%rbp)
	movaps	%xmm0, -1936(%rbp)
	movzbl	-1933(%rbp), %r10d
	movaps	%xmm0, -1952(%rbp)
	movzbl	-1948(%rbp), %r11d
	movaps	%xmm0, -1968(%rbp)
	movzbl	-1963(%rbp), %r12d
	andl	$15, %r10d
	movaps	%xmm0, -2048(%rbp)
	movzbl	-2038(%rbp), %edx
	andl	$15, %r11d
	movaps	%xmm0, -2064(%rbp)
	andl	$15, %r12d
	movzbl	-2053(%rbp), %ecx
	movaps	%xmm0, -2080(%rbp)
	movzbl	-2068(%rbp), %esi
	andl	$15, %edx
	movaps	%xmm0, -2096(%rbp)
	movzbl	-2083(%rbp), %edi
	andl	$15, %ecx
	movaps	%xmm0, -2112(%rbp)
	movzbl	-2098(%rbp), %r8d
	andl	$15, %esi
	movaps	%xmm0, -2128(%rbp)
	movzbl	-96(%rbp,%rax), %eax
	andl	$15, %edi
	movzbl	-96(%rbp,%r14), %r14d
	movzbl	-96(%rbp,%r12), %r12d
	movzbl	-96(%rbp,%r11), %r11d
	andl	$15, %r8d
	salq	$8, %rax
	movzbl	-96(%rbp,%rdi), %edi
	movzbl	-96(%rbp,%rsi), %esi
	orq	%r14, %rax
	movzbl	-96(%rbp,%r10), %r10d
	movzbl	-2113(%rbp), %r9d
	salq	$8, %rax
	movzbl	-96(%rbp,%r8), %r8d
	movzbl	-96(%rbp,%rcx), %ecx
	orq	%r12, %rax
	movq	-2704(%rbp), %r14
	andl	$15, %r9d
	movzbl	-96(%rbp,%rdx), %edx
	salq	$8, %rax
	movzbl	-96(%rbp,%r9), %r9d
	orq	%r11, %rax
	movq	-2688(%rbp), %r11
	salq	$8, %rax
	salq	$8, %r9
	orq	%r10, %rax
	movq	-2720(%rbp), %r10
	salq	$8, %rax
	movzbl	-96(%rbp,%r10), %r10d
	orq	%r10, %rax
	movzbl	-96(%rbp,%r14), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-96(%rbp,%r11), %r10d
	salq	$8, %rax
	orq	%r8, %r9
	movq	-2832(%rbp), %r8
	salq	$8, %r9
	orq	%r10, %rax
	orq	%rdi, %r9
	movq	%rax, -2688(%rbp)
	salq	$8, %r9
	orq	%rsi, %r9
	salq	$8, %r9
	orq	%rcx, %r9
	salq	$8, %r9
	orq	%rdx, %r9
	movzbl	-96(%rbp,%r8), %edx
	salq	$8, %r9
	orq	%rdx, %r9
	movq	-2656(%rbp), %rdx
	salq	$8, %r9
	movzbl	-96(%rbp,%rdx), %edx
	orq	%rdx, %r9
	movq	%r9, -2680(%rbp)
	movdqa	-2688(%rbp), %xmm6
	movslq	-2848(%rbp), %rax
	addq	%rax, -2640(%rbp)
	movq	-2640(%rbp), %rcx
	movdqa	-2864(%rbp), %xmm2
	movups	%xmm6, (%r15,%r13)
	leaq	0(,%rcx,8), %r13
.L440:
	movq	-2648(%rbp), %rax
	movq	-2672(%rbp), %r12
	subq	-2640(%rbp), %r12
	leaq	(%rax,%r12,8), %rax
	cmpl	$8, %r13d
	jnb	.L443
	testl	%r13d, %r13d
	jne	.L597
.L444:
	cmpl	$8, %r13d
	jnb	.L447
.L601:
	testl	%r13d, %r13d
	jne	.L598
.L448:
	movq	-2672(%rbp), %rcx
	movq	%rbx, %rax
	subq	-2648(%rbp), %rax
	sarq	$3, %rax
	subq	%rax, %rcx
	subq	%rax, %r12
	movq	%rax, -2848(%rbp)
	movq	%rcx, -2832(%rbp)
	leaq	(%rbx,%r12,8), %rax
	je	.L493
	movdqu	(%rbx), %xmm6
	leaq	64(%rbx), %rdx
	leaq	-64(%rax), %rcx
	movaps	%xmm6, -2864(%rbp)
	movdqu	16(%rbx), %xmm6
	movaps	%xmm6, -2880(%rbp)
	movdqu	32(%rbx), %xmm6
	movaps	%xmm6, -2896(%rbp)
	movdqu	48(%rbx), %xmm6
	movaps	%xmm6, -2912(%rbp)
	movdqu	-64(%rax), %xmm6
	movaps	%xmm6, -2928(%rbp)
	movdqu	-48(%rax), %xmm6
	movaps	%xmm6, -2944(%rbp)
	movdqu	-32(%rax), %xmm6
	movaps	%xmm6, -2960(%rbp)
	movdqu	-16(%rax), %xmm6
	movaps	%xmm6, -2976(%rbp)
	cmpq	%rcx, %rdx
	je	.L494
	movq	%r15, -2984(%rbp)
	xorl	%r13d, %r13d
	movl	$2, %r14d
	movq	%rcx, %r15
	movaps	%xmm2, -2640(%rbp)
	jmp	.L455
	.p2align 4,,10
	.p2align 3
.L600:
	movdqu	-64(%r15), %xmm5
	movdqu	-48(%r15), %xmm4
	prefetcht0	-256(%r15)
	subq	$64, %r15
	movdqu	32(%r15), %xmm3
	movdqu	48(%r15), %xmm1
.L454:
	movdqa	-2624(%rbp), %xmm7
	movdqa	-2640(%rbp), %xmm0
	movq	%rdx, -2656(%rbp)
	movaps	%xmm1, -2720(%rbp)
	movdqa	%xmm7, %xmm2
	psubq	%xmm5, %xmm0
	movaps	%xmm3, -2704(%rbp)
	pcmpeqd	%xmm5, %xmm2
	movaps	%xmm4, -2688(%rbp)
	pand	%xmm2, %xmm0
	movdqa	%xmm5, %xmm2
	pcmpgtd	%xmm7, %xmm2
	pshufd	$78, %xmm5, %xmm7
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm6
	movdqa	%xmm0, %xmm2
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm6
	punpckhqdq	%xmm0, %xmm2
	pandn	%xmm6, %xmm2
	movdqa	%xmm2, %xmm6
	pand	%xmm7, %xmm2
	pandn	%xmm5, %xmm6
	por	%xmm6, %xmm2
	movaps	%xmm2, -2672(%rbp)
	call	__popcountdi2@PLT
	movdqa	-2672(%rbp), %xmm2
	leaq	-2(%r12,%r13), %rsi
	movdqa	-2624(%rbp), %xmm7
	movdqa	-2688(%rbp), %xmm4
	movdqa	-2640(%rbp), %xmm0
	cltq
	movups	%xmm2, (%rbx,%r13,8)
	addq	$2, %r13
	movups	%xmm2, (%rbx,%rsi,8)
	movdqa	%xmm7, %xmm2
	psubq	%xmm4, %xmm0
	subq	%rax, %r13
	pcmpeqd	%xmm4, %xmm2
	pshufd	$78, %xmm4, %xmm6
	pand	%xmm2, %xmm0
	movdqa	%xmm4, %xmm2
	pcmpgtd	%xmm7, %xmm2
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm5
	movdqa	%xmm0, %xmm2
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm5
	punpckhqdq	%xmm0, %xmm2
	pandn	%xmm5, %xmm2
	movdqa	%xmm2, %xmm5
	pand	%xmm6, %xmm2
	pandn	%xmm4, %xmm5
	por	%xmm5, %xmm2
	movaps	%xmm2, -2672(%rbp)
	call	__popcountdi2@PLT
	movdqa	-2672(%rbp), %xmm2
	leaq	-4(%r12,%r13), %rsi
	movdqa	-2624(%rbp), %xmm7
	movdqa	-2704(%rbp), %xmm3
	movdqa	-2640(%rbp), %xmm0
	cltq
	movups	%xmm2, (%rbx,%r13,8)
	movups	%xmm2, (%rbx,%rsi,8)
	movdqa	%xmm7, %xmm2
	psubq	%xmm3, %xmm0
	movq	%r14, %rsi
	pcmpeqd	%xmm3, %xmm2
	pshufd	$78, %xmm3, %xmm5
	subq	%rax, %rsi
	addq	%rsi, %r13
	pand	%xmm2, %xmm0
	movdqa	%xmm3, %xmm2
	pcmpgtd	%xmm7, %xmm2
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm4
	movdqa	%xmm0, %xmm2
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm4
	punpckhqdq	%xmm0, %xmm2
	pandn	%xmm4, %xmm2
	movdqa	%xmm2, %xmm4
	pand	%xmm5, %xmm2
	pandn	%xmm3, %xmm4
	por	%xmm4, %xmm2
	movaps	%xmm2, -2672(%rbp)
	call	__popcountdi2@PLT
	movdqa	-2672(%rbp), %xmm2
	leaq	-6(%r12,%r13), %rdi
	movq	%r14, %rcx
	movdqa	-2624(%rbp), %xmm7
	movdqa	-2720(%rbp), %xmm1
	cltq
	subq	$8, %r12
	movups	%xmm2, (%rbx,%r13,8)
	movdqa	-2640(%rbp), %xmm0
	subq	%rax, %rcx
	movups	%xmm2, (%rbx,%rdi,8)
	movdqa	%xmm7, %xmm2
	pshufd	$78, %xmm1, %xmm4
	addq	%rcx, %r13
	pcmpeqd	%xmm1, %xmm2
	psubq	%xmm1, %xmm0
	pand	%xmm2, %xmm0
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm7, %xmm2
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm2
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm2
	pandn	%xmm3, %xmm2
	movdqa	%xmm2, %xmm3
	pandn	%xmm1, %xmm3
	movdqa	%xmm2, %xmm1
	pand	%xmm4, %xmm1
	por	%xmm3, %xmm1
	movaps	%xmm1, -2672(%rbp)
	call	__popcountdi2@PLT
	movdqa	-2672(%rbp), %xmm1
	leaq	0(%r13,%r12), %rsi
	movq	-2656(%rbp), %rdx
	cltq
	movups	%xmm1, (%rbx,%r13,8)
	movups	%xmm1, (%rbx,%rsi,8)
	movq	%r14, %rsi
	subq	%rax, %rsi
	addq	%rsi, %r13
	cmpq	%r15, %rdx
	je	.L599
.L455:
	movq	%rdx, %rax
	subq	%rbx, %rax
	sarq	$3, %rax
	subq	%r13, %rax
	cmpq	$8, %rax
	ja	.L600
	movdqu	(%rdx), %xmm5
	movdqu	16(%rdx), %xmm4
	prefetcht0	256(%rdx)
	addq	$64, %rdx
	movdqu	-32(%rdx), %xmm3
	movdqu	-16(%rdx), %xmm1
	jmp	.L454
	.p2align 4,,10
	.p2align 3
.L429:
	cmpq	$23, %rdx
	je	.L592
.L428:
	movq	%rdx, %rcx
	addq	$1, %rdx
	cmpq	%rax, (%r15,%rdx,8)
	je	.L429
	movl	$12, %edx
	subq	$11, %rcx
	subq	%rbx, %rdx
	cmpq	%rdx, %rcx
	jb	.L427
.L592:
	movq	(%r15,%rbx,8), %rax
	jmp	.L427
	.p2align 4,,10
	.p2align 3
.L595:
	movq	%rdx, %rbx
	movl	$8, %edx
	subq	%rax, %rdx
	leaq	-8(%rax,%rbx), %r11
	movq	%r8, %rax
	leaq	(%rdi,%rdx,8), %r13
	jmp	.L381
	.p2align 4,,10
	.p2align 3
.L599:
	movdqa	-2640(%rbp), %xmm2
	movq	-2984(%rbp), %r15
	leaq	(%rbx,%r13,8), %rdx
	leaq	(%r12,%r13), %r14
	addq	$2, %r13
.L452:
	movdqa	-2864(%rbp), %xmm7
	movdqa	-2624(%rbp), %xmm6
	movdqa	%xmm2, %xmm0
	movq	%rdx, -2688(%rbp)
	movaps	%xmm2, -2672(%rbp)
	movdqa	%xmm7, %xmm1
	psubq	%xmm7, %xmm0
	pshufd	$78, %xmm7, %xmm4
	pcmpeqd	%xmm6, %xmm1
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -2640(%rbp)
	call	__popcountdi2@PLT
	movdqa	-2640(%rbp), %xmm1
	movdqa	-2880(%rbp), %xmm7
	movq	-2688(%rbp), %rdx
	movdqa	-2624(%rbp), %xmm6
	cltq
	movdqa	-2672(%rbp), %xmm2
	pshufd	$78, %xmm7, %xmm4
	subq	%rax, %r13
	movups	%xmm1, (%rdx)
	movups	%xmm1, -16(%rbx,%r14,8)
	movdqa	%xmm7, %xmm1
	movdqa	%xmm2, %xmm0
	pcmpeqd	%xmm6, %xmm1
	psubq	%xmm7, %xmm0
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -2640(%rbp)
	call	__popcountdi2@PLT
	movdqa	-2640(%rbp), %xmm1
	leaq	-4(%r12,%r13), %rdx
	movdqa	-2896(%rbp), %xmm7
	movdqa	-2624(%rbp), %xmm6
	movdqa	-2672(%rbp), %xmm2
	cltq
	movups	%xmm1, (%rbx,%r13,8)
	pshufd	$78, %xmm7, %xmm4
	subq	%rax, %r13
	movups	%xmm1, (%rbx,%rdx,8)
	movdqa	%xmm7, %xmm1
	movdqa	%xmm2, %xmm0
	leaq	2(%r13), %r14
	pcmpeqd	%xmm6, %xmm1
	psubq	%xmm7, %xmm0
	movl	$2, %r13d
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -2640(%rbp)
	call	__popcountdi2@PLT
	movdqa	-2912(%rbp), %xmm7
	leaq	-6(%r12,%r14), %rdx
	movdqa	-2640(%rbp), %xmm1
	movdqa	-2624(%rbp), %xmm6
	movdqa	-2672(%rbp), %xmm2
	cltq
	movups	%xmm1, (%rbx,%r14,8)
	pshufd	$78, %xmm7, %xmm4
	movups	%xmm1, (%rbx,%rdx,8)
	movdqa	%xmm7, %xmm1
	movdqa	%xmm2, %xmm0
	movq	%r13, %rdx
	pcmpeqd	%xmm6, %xmm1
	psubq	%xmm7, %xmm0
	subq	%rax, %rdx
	addq	%rdx, %r14
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -2640(%rbp)
	call	__popcountdi2@PLT
	movdqa	-2928(%rbp), %xmm7
	leaq	-8(%r12,%r14), %rdx
	movdqa	-2640(%rbp), %xmm1
	movdqa	-2624(%rbp), %xmm6
	movdqa	-2672(%rbp), %xmm2
	cltq
	movups	%xmm1, (%rbx,%r14,8)
	pshufd	$78, %xmm7, %xmm4
	movups	%xmm1, (%rbx,%rdx,8)
	movdqa	%xmm7, %xmm1
	movdqa	%xmm2, %xmm0
	movq	%r13, %rdx
	pcmpeqd	%xmm6, %xmm1
	psubq	%xmm7, %xmm0
	subq	%rax, %rdx
	addq	%rdx, %r14
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -2640(%rbp)
	call	__popcountdi2@PLT
	movdqa	-2944(%rbp), %xmm7
	leaq	-10(%r12,%r14), %rdx
	movdqa	-2640(%rbp), %xmm1
	movdqa	-2624(%rbp), %xmm6
	movdqa	-2672(%rbp), %xmm2
	cltq
	movups	%xmm1, (%rbx,%r14,8)
	pshufd	$78, %xmm7, %xmm4
	movups	%xmm1, (%rbx,%rdx,8)
	movdqa	%xmm7, %xmm1
	movdqa	%xmm2, %xmm0
	movq	%r13, %rdx
	pcmpeqd	%xmm6, %xmm1
	psubq	%xmm7, %xmm0
	subq	%rax, %rdx
	addq	%rdx, %r14
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -2640(%rbp)
	call	__popcountdi2@PLT
	movdqa	-2640(%rbp), %xmm1
	leaq	-12(%r12,%r14), %rdx
	cltq
	movups	%xmm1, (%rbx,%r14,8)
	movups	%xmm1, (%rbx,%rdx,8)
	movdqa	-2672(%rbp), %xmm2
	movq	%r13, %rdx
	movdqa	-2960(%rbp), %xmm7
	movdqa	-2624(%rbp), %xmm6
	subq	%rax, %rdx
	movdqa	%xmm7, %xmm1
	movdqa	%xmm2, %xmm0
	pshufd	$78, %xmm7, %xmm4
	addq	%rdx, %r14
	pcmpeqd	%xmm6, %xmm1
	psubq	%xmm7, %xmm0
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -2640(%rbp)
	call	__popcountdi2@PLT
	movdqa	-2976(%rbp), %xmm7
	movdqa	-2640(%rbp), %xmm1
	leaq	-14(%r12,%r14), %rdx
	movdqa	-2624(%rbp), %xmm6
	movdqa	-2672(%rbp), %xmm2
	cltq
	movups	%xmm1, (%rbx,%r14,8)
	pshufd	$78, %xmm7, %xmm4
	movups	%xmm1, (%rbx,%rdx,8)
	movdqa	%xmm7, %xmm1
	movdqa	%xmm2, %xmm0
	movq	%r13, %rdx
	pcmpeqd	%xmm6, %xmm1
	psubq	%xmm7, %xmm0
	subq	%rax, %rdx
	movaps	%xmm2, -2688(%rbp)
	addq	%rdx, %r14
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -2640(%rbp)
	call	__popcountdi2@PLT
	leaq	-16(%r12,%r14), %rdx
	movdqa	-2640(%rbp), %xmm1
	movdqa	-2688(%rbp), %xmm2
	cltq
	subq	%rax, %r13
	movups	%xmm1, (%rbx,%r14,8)
	leaq	0(%r13,%r14), %r12
	movups	%xmm1, (%rbx,%rdx,8)
	movq	-2832(%rbp), %rdx
	leaq	0(,%r12,8), %rax
	movq	%rax, -2672(%rbp)
	subq	%r12, %rdx
.L451:
	movq	-2832(%rbp), %rcx
	cmpq	$2, %rdx
	movdqa	-2624(%rbp), %xmm7
	leaq	-16(,%rcx,8), %rax
	cmovnb	-2672(%rbp), %rax
	movdqu	(%rbx,%rax), %xmm6
	movups	%xmm6, (%rbx,%rcx,8)
	movaps	%xmm6, -2640(%rbp)
	movdqa	-2800(%rbp), %xmm6
	movdqa	%xmm6, %xmm0
	psubq	%xmm6, %xmm2
	pcmpeqd	%xmm7, %xmm0
	movdqa	%xmm2, %xmm1
	pand	%xmm0, %xmm1
	movdqa	%xmm6, %xmm0
	pcmpgtd	%xmm7, %xmm0
	por	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm0
	pshufd	$245, %xmm1, %xmm1
	pxor	%xmm1, %xmm0
	movaps	%xmm1, -2832(%rbp)
	movmskpd	%xmm0, %r13d
	movq	%r13, %rdi
	salq	$4, %r13
	call	__popcountdi2@PLT
	movq	-2728(%rbp), %rcx
	movdqa	-2800(%rbp), %xmm6
	cltq
	movdqa	(%rcx,%r13), %xmm0
	movq	%rax, -2640(%rbp)
	movaps	%xmm6, -80(%rbp)
	movaps	%xmm0, -2144(%rbp)
	movzbl	-2143(%rbp), %eax
	movd	%xmm0, %ecx
	movaps	%xmm0, -2224(%rbp)
	andl	$15, %ecx
	movq	%rax, %rdi
	movzbl	-2218(%rbp), %eax
	movaps	%xmm0, -2160(%rbp)
	movzbl	-2158(%rbp), %r10d
	movaps	%xmm0, -2240(%rbp)
	andl	$15, %edi
	movaps	%xmm0, -2256(%rbp)
	movq	%rax, %rsi
	movq	%r10, %r11
	movzbl	-2233(%rbp), %eax
	movzbl	-2248(%rbp), %edx
	andl	$15, %r11d
	andl	$15, %esi
	movaps	%xmm0, -2176(%rbp)
	andl	$15, %eax
	movaps	%xmm0, -2192(%rbp)
	movq	%rsi, %r10
	movzbl	-2188(%rbp), %r13d
	andl	$15, %edx
	movaps	%xmm0, -2208(%rbp)
	movzbl	-2203(%rbp), %r14d
	movq	%rcx, -2624(%rbp)
	andl	$15, %r13d
	movq	%rdi, -2688(%rbp)
	andl	$15, %r14d
	movq	%r11, -2704(%rbp)
	movzbl	-2173(%rbp), %r11d
	movaps	%xmm0, -2272(%rbp)
	movq	%rdx, -2720(%rbp)
	movzbl	-2263(%rbp), %edx
	andl	$15, %r11d
	movaps	%xmm0, -2288(%rbp)
	movaps	%xmm0, -2304(%rbp)
	movq	%rdx, %r8
	movzbl	-2293(%rbp), %ecx
	movzbl	-2278(%rbp), %edx
	movaps	%xmm0, -2320(%rbp)
	andl	$15, %r8d
	movzbl	-2308(%rbp), %esi
	movaps	%xmm0, -2336(%rbp)
	andl	$15, %edx
	andl	$15, %ecx
	movzbl	-2323(%rbp), %edi
	movaps	%xmm0, -2352(%rbp)
	andl	$15, %esi
	movaps	%xmm0, -2368(%rbp)
	movzbl	-80(%rbp,%rax), %eax
	movzbl	-80(%rbp,%r10), %r10d
	andl	$15, %edi
	movzbl	-80(%rbp,%r14), %r14d
	movzbl	-80(%rbp,%r13), %r13d
	movq	%r8, -2656(%rbp)
	salq	$8, %rax
	movzbl	-80(%rbp,%r11), %r11d
	movzbl	-2353(%rbp), %r9d
	orq	%r10, %rax
	movzbl	-80(%rbp,%rdi), %edi
	movzbl	-80(%rbp,%rsi), %esi
	salq	$8, %rax
	andl	$15, %r9d
	movzbl	-80(%rbp,%rcx), %ecx
	movzbl	-2338(%rbp), %r8d
	orq	%r14, %rax
	movq	-2688(%rbp), %r14
	movzbl	-80(%rbp,%r9), %r9d
	salq	$8, %rax
	andl	$15, %r8d
	movzbl	-80(%rbp,%rdx), %edx
	orq	%r13, %rax
	movzbl	-80(%rbp,%r8), %r8d
	salq	$8, %r9
	salq	$8, %rax
	orq	%r11, %rax
	movq	-2704(%rbp), %r11
	salq	$8, %rax
	movzbl	-80(%rbp,%r11), %r10d
	movq	-2624(%rbp), %r11
	orq	%r10, %rax
	movzbl	-80(%rbp,%r14), %r10d
	salq	$8, %rax
	orq	%r10, %rax
	movzbl	-80(%rbp,%r11), %r10d
	salq	$8, %rax
	orq	%r8, %r9
	movq	-2656(%rbp), %r8
	salq	$8, %r9
	orq	%r10, %rax
	orq	%rdi, %r9
	salq	$8, %r9
	orq	%rsi, %r9
	salq	$8, %r9
	orq	%rcx, %r9
	salq	$8, %r9
	orq	%rdx, %r9
	movzbl	-80(%rbp,%r8), %edx
	movq	%rax, -2624(%rbp)
	movdqa	-2752(%rbp), %xmm6
	movdqa	-2784(%rbp), %xmm0
	salq	$8, %r9
	movq	-2640(%rbp), %xmm2
	orq	%rdx, %r9
	movq	-2720(%rbp), %rdx
	movdqa	%xmm6, %xmm3
	salq	$8, %r9
	movdqa	-2832(%rbp), %xmm1
	punpcklqdq	%xmm2, %xmm2
	movzbl	-80(%rbp,%rdx), %edx
	pcmpeqd	%xmm2, %xmm3
	psubq	%xmm2, %xmm0
	pcmpgtd	%xmm6, %xmm2
	orq	%rdx, %r9
	movq	%r9, -2616(%rbp)
	pand	%xmm3, %xmm0
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L457
	movq	-2672(%rbp), %rsi
	movdqa	-2624(%rbp), %xmm6
	movq	%xmm6, (%rbx,%rsi)
.L457:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rax
	testq	%rax, %rax
	je	.L458
	movq	-2672(%rbp), %rax
	movdqa	-2624(%rbp), %xmm6
	movhps	%xmm6, 8(%rbx,%rax)
.L458:
	addq	-2640(%rbp), %r12
	movmskpd	%xmm1, %r13d
	movq	%r13, %rdi
	leaq	0(,%r12,8), %rax
	salq	$4, %r13
	movq	%rax, -2656(%rbp)
	call	__popcountdi2@PLT
	movdqa	-2800(%rbp), %xmm6
	movl	%eax, -2720(%rbp)
	movq	-2728(%rbp), %rax
	movaps	%xmm6, -64(%rbp)
	movdqa	(%rax,%r13), %xmm0
	movaps	%xmm0, -2384(%rbp)
	movzbl	-2383(%rbp), %eax
	movd	%xmm0, %r10d
	movaps	%xmm0, -2496(%rbp)
	movzbl	-2488(%rbp), %edx
	andl	$15, %r10d
	andl	$15, %eax
	movaps	%xmm0, -2400(%rbp)
	movaps	%xmm0, -2512(%rbp)
	movq	%rdx, %rsi
	movzbl	-2503(%rbp), %edx
	movq	%rax, -2624(%rbp)
	movzbl	-2398(%rbp), %eax
	andl	$15, %esi
	movaps	%xmm0, -2464(%rbp)
	andl	$15, %edx
	movq	%rax, %rcx
	movzbl	-2458(%rbp), %eax
	movaps	%xmm0, -2528(%rbp)
	movq	%rdx, -2688(%rbp)
	movzbl	-2518(%rbp), %edx
	andl	$15, %ecx
	movaps	%xmm0, -2480(%rbp)
	movq	%rax, %rdi
	movzbl	-2473(%rbp), %eax
	movq	%rdx, %r9
	andl	$15, %edi
	movaps	%xmm0, -2416(%rbp)
	movzbl	-2413(%rbp), %r11d
	andl	$15, %r9d
	andl	$15, %eax
	movaps	%xmm0, -2432(%rbp)
	movq	%rdi, %r8
	movaps	%xmm0, -2448(%rbp)
	movzbl	-2428(%rbp), %r13d
	andl	$15, %r11d
	movzbl	-2443(%rbp), %r14d
	movq	%rcx, -2640(%rbp)
	movq	%rsi, -2672(%rbp)
	andl	$15, %r14d
	andl	$15, %r13d
	movq	%r9, -2704(%rbp)
	movaps	%xmm0, -2544(%rbp)
	movzbl	-2533(%rbp), %edx
	movaps	%xmm0, -2560(%rbp)
	movzbl	-2548(%rbp), %ecx
	movaps	%xmm0, -2576(%rbp)
	movzbl	-2563(%rbp), %esi
	andl	$15, %edx
	movaps	%xmm0, -2592(%rbp)
	movzbl	-2578(%rbp), %edi
	andl	$15, %ecx
	movaps	%xmm0, -2608(%rbp)
	movzbl	-64(%rbp,%rax), %eax
	andl	$15, %esi
	movzbl	-64(%rbp,%r8), %r8d
	movzbl	-64(%rbp,%r14), %r14d
	andl	$15, %edi
	movzbl	-64(%rbp,%rsi), %esi
	salq	$8, %rax
	movzbl	-64(%rbp,%r13), %r13d
	movzbl	-64(%rbp,%r11), %r11d
	orq	%r8, %rax
	movzbl	-64(%rbp,%rdi), %edi
	movzbl	-64(%rbp,%rcx), %ecx
	salq	$8, %rax
	movzbl	-2593(%rbp), %r9d
	movzbl	-64(%rbp,%rdx), %edx
	orq	%r14, %rax
	movq	-2640(%rbp), %r14
	movzbl	-64(%rbp,%r10), %r10d
	salq	$8, %rax
	andl	$15, %r9d
	orq	%r13, %rax
	movzbl	-64(%rbp,%r9), %r9d
	salq	$8, %rax
	orq	%r11, %rax
	movzbl	-64(%rbp,%r14), %r11d
	salq	$8, %rax
	orq	%r11, %rax
	movq	-2624(%rbp), %r11
	salq	$8, %rax
	movzbl	-64(%rbp,%r11), %r11d
	orq	%r11, %rax
	salq	$8, %rax
	salq	$8, %r9
	orq	%rdi, %r9
	orq	%r10, %rax
	salq	$8, %r9
	movq	%rax, -2624(%rbp)
	orq	%rsi, %r9
	movq	-2672(%rbp), %rsi
	salq	$8, %r9
	orq	%rcx, %r9
	movq	-2704(%rbp), %rcx
	salq	$8, %r9
	orq	%rdx, %r9
	movzbl	-64(%rbp,%rcx), %edx
	salq	$8, %r9
	orq	%rdx, %r9
	movq	-2688(%rbp), %rdx
	salq	$8, %r9
	movzbl	-64(%rbp,%rdx), %edx
	orq	%rdx, %r9
	movzbl	-64(%rbp,%rsi), %edx
	salq	$8, %r9
	orq	%rdx, %r9
	movq	%r9, -2616(%rbp)
	movslq	-2720(%rbp), %rax
	movdqa	-2752(%rbp), %xmm6
	movdqa	-2784(%rbp), %xmm0
	movq	%rax, %xmm1
	punpcklqdq	%xmm1, %xmm1
	movdqa	%xmm6, %xmm2
	pcmpeqd	%xmm1, %xmm2
	psubq	%xmm1, %xmm0
	pcmpgtd	%xmm6, %xmm1
	pand	%xmm2, %xmm0
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L459
	movdqa	-2624(%rbp), %xmm6
	movq	%xmm6, (%rbx,%r12,8)
.L459:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rax
	testq	%rax, %rax
	je	.L460
	movq	-2656(%rbp), %rax
	movdqa	-2624(%rbp), %xmm6
	movhps	%xmm6, 8(%rbx,%rax)
.L460:
	movq	-2848(%rbp), %rbx
	addq	%r12, %rbx
	movq	-2768(%rbp), %r12
	subq	$1, %r12
	cmpl	$2, -2812(%rbp)
	je	.L462
	movq	-2736(%rbp), %r8
	movq	%r12, %r9
	movq	%r15, %rcx
	movq	%rbx, %rdx
	movq	-2808(%rbp), %rsi
	movq	-2648(%rbp), %rdi
	call	_ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	cmpl	$3, -2812(%rbp)
	je	.L369
.L462:
	movq	-2760(%rbp), %rdx
	movq	%r12, %r9
	movq	%r15, %rcx
	movq	-2648(%rbp), %rax
	movq	-2736(%rbp), %r8
	movq	-2808(%rbp), %rsi
	subq	%rbx, %rdx
	leaq	(%rax,%rbx,8), %rdi
	call	_ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L369:
	addq	$2952, %rsp
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
.L443:
	.cfi_restore_state
	movq	(%rax), %rdx
	leaq	8(%rbx), %rdi
	movq	%rax, %rsi
	andq	$-8, %rdi
	movq	%rdx, (%rbx)
	movl	%r13d, %edx
	movq	-8(%rax,%rdx), %rcx
	movq	%rcx, -8(%rbx,%rdx)
	movq	%rbx, %rcx
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%r13d, %ecx
	shrl	$3, %ecx
	rep movsq
	cmpl	$8, %r13d
	jb	.L601
.L447:
	movq	(%r15), %rdx
	leaq	8(%rax), %rdi
	movq	%r15, %rsi
	andq	$-8, %rdi
	movq	%rdx, (%rax)
	movl	%r13d, %edx
	movq	-8(%r15,%rdx), %rcx
	movq	%rcx, -8(%rax,%rdx)
	subq	%rdi, %rax
	leal	0(%r13,%rax), %ecx
	subq	%rax, %rsi
	shrl	$3, %ecx
	rep movsq
	jmp	.L448
.L598:
	movzbl	(%r15), %edx
	movb	%dl, (%rax)
	jmp	.L448
.L597:
	movzbl	(%rax), %edx
	movb	%dl, (%rbx)
	jmp	.L444
.L594:
	cmpq	$1, %rdx
	jbe	.L369
	leaq	256(%rdi), %rax
	cmpq	%rax, %rsi
	jb	.L373
	movl	$2, %esi
	call	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	jmp	.L369
.L384:
	movq	-2648(%rbp), %rax
	andl	$1, %r12d
	movdqa	%xmm2, %xmm5
	movl	$2, %edi
	movdqa	.LC1(%rip), %xmm7
	subq	%r12, %rdi
	movdqu	(%rax), %xmm6
	movq	%rdi, %xmm3
	punpcklqdq	%xmm3, %xmm3
	movaps	%xmm7, -2752(%rbp)
	movaps	%xmm6, -2624(%rbp)
	movdqa	.LC0(%rip), %xmm6
	movdqa	-2624(%rbp), %xmm0
	movdqa	%xmm6, %xmm1
	movaps	%xmm6, -2784(%rbp)
	movdqa	%xmm7, %xmm6
	pcmpeqd	%xmm3, %xmm6
	pcmpeqd	%xmm2, %xmm0
	psubq	%xmm3, %xmm1
	pcmpgtd	%xmm7, %xmm3
	pand	%xmm6, %xmm1
	pshufd	$177, %xmm0, %xmm4
	por	%xmm3, %xmm1
	pand	%xmm4, %xmm0
	pshufd	$245, %xmm1, %xmm1
	pandn	%xmm1, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L602
	movq	-2648(%rbp), %rax
	pxor	%xmm1, %xmm1
	movq	-2760(%rbp), %r8
	pxor	%xmm6, %xmm6
	movdqa	%xmm1, %xmm0
	leaq	256(%rax,%rdi,8), %rsi
	.p2align 4,,10
	.p2align 3
.L390:
	movq	%rdi, %rcx
	leaq	32(%rdi), %rdi
	cmpq	%rdi, %r8
	jb	.L603
	leaq	-256(%rsi), %rax
.L389:
	movdqa	(%rax), %xmm3
	leaq	32(%rax), %rdx
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm0
	movdqa	16(%rax), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm1
	movdqa	32(%rax), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm0
	movdqa	48(%rax), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm1
	movdqa	64(%rax), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm0
	movdqa	80(%rax), %xmm3
	leaq	96(%rdx), %rax
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm1
	movdqa	64(%rdx), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm0
	movdqa	80(%rdx), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm1
	cmpq	%rsi, %rax
	jne	.L389
	movdqa	%xmm0, %xmm3
	leaq	352(%rdx), %rsi
	por	%xmm1, %xmm3
	pcmpeqd	%xmm6, %xmm3
	pshufd	$177, %xmm3, %xmm4
	pand	%xmm4, %xmm3
	movmskpd	%xmm3, %eax
	cmpl	$3, %eax
	je	.L390
	movq	-2648(%rbp), %rax
	movdqa	%xmm5, %xmm0
	pcmpeqd	%xmm3, %xmm3
	movq	-2648(%rbp), %rdx
	pcmpeqd	(%rax,%rcx,8), %xmm0
	pshufd	$177, %xmm0, %xmm1
	pand	%xmm1, %xmm0
	pxor	%xmm3, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L392
	.p2align 4,,10
	.p2align 3
.L391:
	addq	$2, %rcx
	movdqa	%xmm5, %xmm0
	pcmpeqd	(%rdx,%rcx,8), %xmm0
	pshufd	$177, %xmm0, %xmm1
	pand	%xmm1, %xmm0
	pxor	%xmm3, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	je	.L391
.L392:
	rep bsfl	%eax, %eax
	cltq
	addq	%rcx, %rax
.L388:
	movq	-2648(%rbp), %rcx
	movdqa	%xmm5, %xmm3
	movdqa	%xmm2, %xmm0
	leaq	(%rcx,%rax,8), %rdi
	movq	(%rdi), %rbx
	movq	%rbx, %xmm1
	punpcklqdq	%xmm1, %xmm1
	pcmpeqd	%xmm1, %xmm3
	psubq	%xmm1, %xmm0
	movaps	%xmm1, -2640(%rbp)
	pand	%xmm3, %xmm0
	movdqa	%xmm1, %xmm3
	pcmpgtd	%xmm5, %xmm3
	por	%xmm3, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movmskpd	%xmm0, %edx
	testl	%edx, %edx
	jne	.L397
	movq	-2760(%rbp), %rax
	movq	%r14, -2720(%rbp)
	xorl	%r13d, %r13d
	movq	%rbx, -2688(%rbp)
	movq	%rcx, %rbx
	leaq	-2(%rax), %r12
	movaps	%xmm5, -2624(%rbp)
	movq	%rax, %r14
	movaps	%xmm1, -2704(%rbp)
	movaps	%xmm2, -2672(%rbp)
	jmp	.L404
	.p2align 4,,10
	.p2align 3
.L398:
	movdqa	-2672(%rbp), %xmm6
	movmskpd	%xmm0, %edi
	movups	%xmm6, (%rbx,%r12,8)
	call	__popcountdi2@PLT
	cltq
	addq	%rax, %r13
	leaq	-2(%r12), %rax
	cmpq	%rax, %r14
	jbe	.L604
	movq	%rax, %r12
.L404:
	movdqu	(%rbx,%r12,8), %xmm0
	pcmpeqd	-2640(%rbp), %xmm0
	pshufd	$177, %xmm0, %xmm1
	movdqa	%xmm0, %xmm3
	movdqu	(%rbx,%r12,8), %xmm0
	pand	%xmm1, %xmm3
	pcmpeqd	-2624(%rbp), %xmm0
	pshufd	$177, %xmm0, %xmm1
	pand	%xmm1, %xmm0
	movdqa	%xmm3, %xmm1
	por	%xmm0, %xmm1
	movmskpd	%xmm1, %eax
	cmpl	$3, %eax
	je	.L398
	pcmpeqd	%xmm4, %xmm4
	movq	-2648(%rbp), %rcx
	leaq	2(%r12), %rdx
	movdqa	-2624(%rbp), %xmm5
	pxor	%xmm4, %xmm0
	movq	-2688(%rbp), %rbx
	movdqa	-2704(%rbp), %xmm1
	pandn	%xmm0, %xmm3
	movdqa	-2672(%rbp), %xmm2
	movmskpd	%xmm3, %eax
	rep bsfl	%eax, %eax
	cltq
	addq	%r12, %rax
	addq	$4, %r12
	movq	(%rcx,%rax,8), %xmm3
	movq	-2760(%rbp), %rax
	punpcklqdq	%xmm3, %xmm3
	subq	%r13, %rax
	movaps	%xmm3, -64(%rbp)
	cmpq	%r12, %rax
	jb	.L399
	.p2align 4,,10
	.p2align 3
.L400:
	movups	%xmm1, -16(%rcx,%r12,8)
	movq	%r12, %rdx
	addq	$2, %r12
	cmpq	%rax, %r12
	jbe	.L400
.L399:
	movdqa	-2752(%rbp), %xmm7
	subq	%rdx, %rax
	movdqa	-2784(%rbp), %xmm4
	leaq	0(,%rdx,8), %rcx
	movq	%rax, %xmm0
	punpcklqdq	%xmm0, %xmm0
	movdqa	%xmm7, %xmm6
	pcmpeqd	%xmm0, %xmm6
	psubq	%xmm0, %xmm4
	pcmpgtd	%xmm7, %xmm0
	pand	%xmm6, %xmm4
	por	%xmm4, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L401
	movq	-2648(%rbp), %rax
	movq	%rbx, (%rax,%rdx,8)
.L401:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rax
	testq	%rax, %rax
	je	.L403
	movq	-2648(%rbp), %rax
	movq	%rbx, 8(%rax,%rcx)
.L403:
	movdqa	%xmm5, %xmm0
	pcmpeqd	.LC5(%rip), %xmm0
	pshufd	$177, %xmm0, %xmm4
	pand	%xmm4, %xmm0
	movmskpd	%xmm0, %eax
	cmpl	$3, %eax
	je	.L480
	movdqa	%xmm5, %xmm0
	pcmpeqd	.LC6(%rip), %xmm0
	pshufd	$177, %xmm0, %xmm4
	pand	%xmm4, %xmm0
	movmskpd	%xmm0, %eax
	movl	%eax, -2812(%rbp)
	cmpl	$3, %eax
	je	.L605
	movdqa	%xmm1, %xmm4
	movdqa	%xmm1, %xmm0
	movdqa	%xmm1, %xmm6
	pcmpeqd	%xmm3, %xmm4
	psubq	%xmm3, %xmm0
	pand	%xmm4, %xmm0
	movdqa	%xmm3, %xmm4
	pcmpgtd	%xmm1, %xmm4
	por	%xmm4, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm4
	pand	%xmm0, %xmm6
	pandn	%xmm3, %xmm4
	por	%xmm4, %xmm6
	movdqa	%xmm6, %xmm7
	movdqa	%xmm6, %xmm4
	pcmpeqd	%xmm5, %xmm7
	psubq	%xmm2, %xmm4
	pand	%xmm7, %xmm4
	movdqa	%xmm5, %xmm7
	pcmpgtd	%xmm6, %xmm7
	por	%xmm7, %xmm4
	pshufd	$245, %xmm4, %xmm4
	movmskpd	%xmm4, %eax
	testl	%eax, %eax
	jne	.L606
	movdqa	%xmm2, %xmm0
	movl	$32, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L414:
	movq	-2648(%rbp), %rbx
	leaq	(%rcx,%rax,2), %rdx
	movdqa	%xmm0, %xmm1
	addq	$1, %rax
	movdqu	(%rbx,%rdx,8), %xmm3
	movdqa	%xmm3, %xmm4
	psubq	%xmm3, %xmm1
	pcmpeqd	%xmm0, %xmm4
	pand	%xmm4, %xmm1
	movdqa	%xmm3, %xmm4
	pcmpgtd	%xmm0, %xmm4
	por	%xmm4, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm0, %xmm1
	pandn	%xmm3, %xmm4
	por	%xmm4, %xmm1
	movdqa	%xmm1, %xmm0
	cmpq	$16, %rax
	jne	.L414
	movdqa	%xmm1, %xmm3
	psubq	%xmm2, %xmm1
	pcmpeqd	%xmm5, %xmm3
	pand	%xmm3, %xmm1
	movdqa	%xmm5, %xmm3
	pcmpgtd	%xmm0, %xmm3
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movmskpd	%xmm1, %eax
	testl	%eax, %eax
	jne	.L593
	leaq	32(%rsi), %rax
	cmpq	%rax, -2760(%rbp)
	jb	.L607
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L414
.L492:
	movdqa	.LC0(%rip), %xmm6
	leaq	_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %rcx
	movq	%rsi, %rbx
	xorl	%r13d, %r13d
	movq	$0, -2640(%rbp)
	movaps	%xmm6, -2784(%rbp)
	movdqa	.LC1(%rip), %xmm6
	movq	%rcx, -2728(%rbp)
	movaps	%xmm6, -2752(%rbp)
	jmp	.L432
.L494:
	movq	%r12, %r14
	movq	%rbx, %rdx
	movl	$2, %r13d
	jmp	.L452
.L493:
	movq	$0, -2672(%rbp)
	movq	%rcx, %rdx
	jmp	.L451
.L596:
	movq	-2760(%rbp), %rsi
	movq	-2648(%rbp), %rdi
	leaq	-1(%rsi), %rbx
	movq	%rbx, %r12
	shrq	%r12
	.p2align 4,,10
	.p2align 3
.L430:
	movq	%r12, %rdx
	call	_ZN3hwy6N_SSE26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	subq	$1, %r12
	jnb	.L430
	.p2align 4,,10
	.p2align 3
.L431:
	movq	(%rdi,%rbx,8), %rdx
	movq	(%rdi), %rax
	movq	%rbx, %rsi
	movq	%rdx, (%rdi)
	xorl	%edx, %edx
	movq	%rax, (%rdi,%rbx,8)
	call	_ZN3hwy6N_SSE26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	subq	$1, %rbx
	jne	.L431
	jmp	.L369
.L482:
	movl	$12, %edx
	movl	$11, %ebx
	jmp	.L428
.L483:
	movl	$9, %ebx
	movl	$10, %edx
	jmp	.L428
.L484:
	movl	$9, %edx
	jmp	.L428
.L485:
	movl	$8, %edx
	movl	$7, %ebx
	jmp	.L428
.L486:
	movl	$6, %ebx
	movl	$7, %edx
	jmp	.L428
.L603:
	movq	-2648(%rbp), %rdi
	movq	-2760(%rbp), %rsi
	pcmpeqd	%xmm3, %xmm3
	.p2align 4,,10
	.p2align 3
.L394:
	movq	%rcx, %rdx
	addq	$2, %rcx
	cmpq	%rcx, %rsi
	jb	.L608
	movdqa	%xmm5, %xmm0
	pcmpeqd	-16(%rdi,%rcx,8), %xmm0
	pshufd	$177, %xmm0, %xmm1
	pand	%xmm1, %xmm0
	pxor	%xmm3, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	je	.L394
.L591:
	rep bsfl	%eax, %eax
	cltq
	addq	%rdx, %rax
	jmp	.L388
.L487:
	movl	$5, %ebx
	movl	$6, %edx
	jmp	.L428
.L488:
	movl	$4, %ebx
	movl	$5, %edx
	jmp	.L428
.L397:
	movq	-2760(%rbp), %rsi
	leaq	-64(%rbp), %rdx
	movq	%r15, %rcx
	movdqa	%xmm2, %xmm0
	movaps	%xmm1, -2624(%rbp)
	subq	%rax, %rsi
	call	_ZN3hwy6N_SSE26detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L369
	movq	(%r15), %xmm2
	movdqa	-64(%rbp), %xmm3
	movdqa	-2624(%rbp), %xmm1
	punpcklqdq	%xmm2, %xmm2
	movdqa	%xmm2, %xmm5
	jmp	.L403
.L489:
	movl	$3, %ebx
	movl	$4, %edx
	jmp	.L428
.L490:
	movl	$2, %ebx
	movl	$3, %edx
	jmp	.L428
.L491:
	movl	$1, %ebx
	movl	$2, %edx
	jmp	.L428
.L604:
	movdqa	-2752(%rbp), %xmm6
	movq	-2648(%rbp), %rax
	movq	%r12, %xmm0
	movdqa	-2784(%rbp), %xmm3
	punpcklqdq	%xmm0, %xmm0
	movdqa	-2624(%rbp), %xmm5
	movdqa	%xmm6, %xmm4
	movdqa	-2704(%rbp), %xmm1
	movq	-2760(%rbp), %rcx
	pcmpeqd	%xmm0, %xmm4
	psubq	%xmm0, %xmm3
	movq	-2720(%rbp), %r14
	movq	-2688(%rbp), %rbx
	pcmpgtd	%xmm6, %xmm0
	movdqu	(%rax), %xmm6
	movdqa	-2672(%rbp), %xmm2
	subq	%r13, %rcx
	movaps	%xmm6, -2624(%rbp)
	movdqa	-2624(%rbp), %xmm6
	pand	%xmm4, %xmm3
	por	%xmm0, %xmm3
	movdqa	-2624(%rbp), %xmm0
	pcmpeqd	%xmm5, %xmm6
	pshufd	$245, %xmm3, %xmm3
	pcmpeqd	%xmm1, %xmm0
	pshufd	$177, %xmm6, %xmm4
	pand	%xmm3, %xmm4
	pshufd	$177, %xmm0, %xmm7
	pand	%xmm6, %xmm4
	pcmpeqd	%xmm6, %xmm6
	pand	%xmm7, %xmm0
	pxor	%xmm6, %xmm3
	por	%xmm3, %xmm0
	por	%xmm4, %xmm0
	movmskpd	%xmm0, %eax
	cmpl	$3, %eax
	jne	.L609
	movmskpd	%xmm4, %edi
	movaps	%xmm2, -2640(%rbp)
	movaps	%xmm1, -2624(%rbp)
	movq	%rcx, -2672(%rbp)
	call	__popcountdi2@PLT
	movq	-2648(%rbp), %rbx
	movdqa	-2640(%rbp), %xmm2
	movslq	%eax, %rdx
	movq	-2672(%rbp), %rax
	movdqa	-2624(%rbp), %xmm1
	movups	%xmm2, (%rbx)
	subq	%rdx, %rax
	cmpq	$1, %rax
	jbe	.L469
	leaq	-2(%rax), %rcx
	movq	%rcx, %rdx
	shrq	%rdx
	salq	$4, %rdx
	leaq	16(%rbx,%rdx), %rdx
.L411:
	movups	%xmm1, (%r14)
	addq	$16, %r14
	cmpq	%rdx, %r14
	jne	.L411
	andq	$-2, %rcx
	movq	%rcx, %rdx
	addq	$2, %rdx
	leaq	0(,%rdx,8), %rcx
	subq	%rdx, %rax
.L410:
	movaps	%xmm1, (%r15)
	testq	%rax, %rax
	je	.L369
	movq	-2648(%rbp), %rdi
	leaq	0(,%rax,8), %rdx
	movq	%r15, %rsi
	addq	%rcx, %rdi
	call	memcpy@PLT
	jmp	.L369
.L607:
	movq	-2760(%rbp), %rcx
	movq	%rbx, %rdx
	jmp	.L421
.L422:
	movdqu	-16(%rdx,%rsi,8), %xmm1
	movdqa	%xmm1, %xmm3
	movdqa	%xmm1, %xmm0
	pcmpeqd	%xmm5, %xmm3
	psubq	%xmm2, %xmm0
	pand	%xmm3, %xmm0
	movdqa	%xmm5, %xmm3
	pcmpgtd	%xmm1, %xmm3
	por	%xmm3, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L593
.L421:
	movq	%rsi, %rax
	addq	$2, %rsi
	cmpq	%rsi, %rcx
	jnb	.L422
	movq	-2760(%rbp), %rbx
	cmpq	%rax, %rbx
	je	.L480
	movq	-2648(%rbp), %rax
	movdqu	-16(%rax,%rbx,8), %xmm1
	movdqa	%xmm1, %xmm3
	movdqa	%xmm1, %xmm0
	pcmpeqd	%xmm5, %xmm3
	pcmpgtd	%xmm1, %xmm5
	psubq	%xmm2, %xmm0
	pand	%xmm3, %xmm0
	por	%xmm5, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movmskpd	%xmm0, %eax
	cmpl	$1, %eax
	movl	$1, %eax
	adcl	$0, %eax
	movl	%eax, -2812(%rbp)
	jmp	.L423
.L425:
	movl	$11, %edx
	movl	$10, %ebx
	jmp	.L428
.L608:
	movq	-2760(%rbp), %rax
	leaq	-2(%rax), %rdx
	movq	-2648(%rbp), %rax
	movdqu	(%rax,%rdx,8), %xmm6
	movaps	%xmm6, -2624(%rbp)
	movdqa	-2624(%rbp), %xmm0
	pcmpeqd	%xmm5, %xmm0
	pshufd	$177, %xmm0, %xmm1
	pand	%xmm1, %xmm0
	pcmpeqd	%xmm1, %xmm1
	pxor	%xmm1, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	je	.L369
	jmp	.L591
.L602:
	rep bsfl	%eax, %eax
	cltq
	jmp	.L388
.L373:
	movq	%rdx, %r11
	leaq	-2(%rdx), %rdx
	movq	-2648(%rbp), %r10
	leaq	8(%rcx), %rdi
	movq	%rdx, %rbx
	andq	$-8, %rdi
	andq	$-2, %rdx
	shrq	%rbx
	movq	(%r10), %rax
	movq	%r10, %rsi
	leaq	2(%rdx), %r12
	addq	$1, %rbx
	salq	$4, %rbx
	movq	%rax, (%rcx)
	movl	%ebx, %r8d
	leaq	8(%r10,%r8), %r14
	leaq	8(%rcx,%r8), %r13
	movq	-16(%r14), %rax
	movq	%rax, -16(%r13)
	movq	%rcx, %rax
	subq	%rdi, %rax
	subq	%rax, %rsi
	leal	(%rbx,%rax), %ecx
	movq	%r11, %rax
	shrl	$3, %ecx
	subq	%r12, %rax
	rep movsq
	movq	%r11, -2760(%rbp)
	movq	%rax, %rcx
	movq	%rax, -2624(%rbp)
	je	.L374
	leaq	0(,%r12,8), %rax
	leaq	0(,%rcx,8), %rdx
	movq	%r8, -2640(%rbp)
	leaq	(%r10,%rax), %rsi
	leaq	(%r15,%rax), %rdi
	call	memcpy@PLT
	movq	-2760(%rbp), %r11
	movl	$32, %ecx
	movq	-2640(%rbp), %r8
	movl	%r11d, %eax
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
	movq	%rsi, %rax
	salq	$4, %rax
	addq	$2, %rax
	cmpq	%rax, %r11
	jnb	.L610
.L375:
	movdqa	.LC4(%rip), %xmm0
	movq	-2760(%rbp), %rdx
.L379:
	movups	%xmm0, (%r15,%rdx,8)
	addq	$2, %rdx
	cmpq	%rax, %rdx
	jb	.L379
	movq	%r15, %rdi
	movq	%r8, -2640(%rbp)
	call	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	movq	-2648(%rbp), %rcx
	movq	(%r15), %rax
	movq	%r15, %rsi
	movq	%rax, (%rcx)
	movq	-2640(%rbp), %r8
	leaq	8(%rcx), %rdi
	andq	$-8, %rdi
	movq	-8(%r15,%r8), %rax
	movq	%rax, -8(%rcx,%r8)
	movq	%rcx, %rax
	subq	%rdi, %rax
	leal	(%rbx,%rax), %ecx
	subq	%rax, %rsi
	shrl	$3, %ecx
	rep movsq
	cmpq	$0, -2624(%rbp)
	je	.L369
.L377:
	movq	-2648(%rbp), %rdi
	movq	-2624(%rbp), %rdx
	salq	$3, %r12
	leaq	(%r15,%r12), %rsi
	addq	%r12, %rdi
	salq	$3, %rdx
	call	memcpy@PLT
	jmp	.L369
.L480:
	movl	$2, -2812(%rbp)
	jmp	.L423
.L605:
	pcmpeqd	%xmm0, %xmm0
	paddq	%xmm0, %xmm2
	jmp	.L423
.L606:
	movdqa	%xmm0, %xmm4
	pand	%xmm3, %xmm0
	pandn	%xmm1, %xmm4
	movdqa	%xmm2, %xmm1
	por	%xmm4, %xmm0
	movdqa	%xmm0, %xmm3
	psubq	%xmm0, %xmm1
	pcmpeqd	%xmm5, %xmm3
	pcmpgtd	%xmm5, %xmm0
	pand	%xmm3, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L593
	movdqa	%xmm2, %xmm0
	movl	$32, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L415:
	movq	-2648(%rbp), %rbx
	leaq	(%rcx,%rax,2), %rdx
	movdqa	%xmm0, %xmm1
	addq	$1, %rax
	movdqu	(%rbx,%rdx,8), %xmm3
	movdqa	%xmm3, %xmm4
	psubq	%xmm3, %xmm1
	pcmpeqd	%xmm0, %xmm4
	pand	%xmm4, %xmm1
	movdqa	%xmm3, %xmm4
	pcmpgtd	%xmm0, %xmm4
	por	%xmm4, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm3
	pandn	%xmm0, %xmm4
	por	%xmm4, %xmm3
	movdqa	%xmm3, %xmm0
	cmpq	$16, %rax
	jne	.L415
	pcmpeqd	%xmm5, %xmm3
	movdqa	%xmm2, %xmm1
	psubq	%xmm0, %xmm1
	pand	%xmm3, %xmm1
	movdqa	%xmm0, %xmm3
	pcmpgtd	%xmm5, %xmm3
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movmskpd	%xmm1, %eax
	testl	%eax, %eax
	jne	.L593
	leaq	32(%rsi), %rax
	cmpq	%rax, -2760(%rbp)
	jb	.L611
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L415
.L374:
	movq	-2760(%rbp), %rdi
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
	movq	%rsi, %rax
	salq	$4, %rax
	addq	$2, %rax
	cmpq	%rax, %rdi
	jb	.L375
	movq	%r15, %rdi
	call	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	movq	-2648(%rbp), %rcx
	movq	(%r15), %rax
	movq	%r15, %rsi
	movq	%rax, (%rcx)
	movq	-16(%r13), %rax
	leaq	8(%rcx), %rdi
	andq	$-8, %rdi
	movq	%rax, -16(%r14)
	movq	%rcx, %rax
	subq	%rdi, %rax
	leal	(%rbx,%rax), %ecx
	subq	%rax, %rsi
	shrl	$3, %ecx
	rep movsq
	jmp	.L369
	.p2align 4,,10
	.p2align 3
.L469:
	xorl	%ecx, %ecx
	jmp	.L410
.L609:
	pxor	%xmm6, %xmm0
	movq	-2648(%rbp), %rsi
	movmskpd	%xmm0, %eax
	rep bsfl	%eax, %eax
	cltq
	movq	(%rsi,%rax,8), %xmm3
	leaq	2(%r12), %rax
	punpcklqdq	%xmm3, %xmm3
	movaps	%xmm3, -64(%rbp)
	cmpq	%rcx, %rax
	ja	.L406
.L407:
	movq	-2648(%rbp), %rsi
	movq	%rax, %r12
	movups	%xmm1, -16(%rsi,%rax,8)
	addq	$2, %rax
	cmpq	%rcx, %rax
	jbe	.L407
.L406:
	movq	%rcx, %rax
	movdqa	-2752(%rbp), %xmm7
	movdqa	-2784(%rbp), %xmm0
	leaq	0(,%r12,8), %rdx
	subq	%r12, %rax
	movq	%rax, %xmm4
	movdqa	%xmm7, %xmm6
	punpcklqdq	%xmm4, %xmm4
	pcmpeqd	%xmm4, %xmm6
	psubq	%xmm4, %xmm0
	pcmpgtd	%xmm7, %xmm4
	pand	%xmm6, %xmm0
	por	%xmm4, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L408
	movq	-2648(%rbp), %rax
	movq	%rbx, (%rax,%r12,8)
.L408:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rax
	testq	%rax, %rax
	je	.L403
	movq	-2648(%rbp), %rax
	movq	%rbx, 8(%rax,%rdx)
	jmp	.L403
.L611:
	movq	%rbx, %rdx
	jmp	.L417
.L418:
	movdqu	-16(%rdx,%rsi,8), %xmm1
	movdqa	%xmm2, %xmm0
	movdqa	%xmm1, %xmm3
	psubq	%xmm1, %xmm0
	pcmpeqd	%xmm5, %xmm3
	pcmpgtd	%xmm5, %xmm1
	pand	%xmm3, %xmm0
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L593
.L417:
	movq	%rsi, %rax
	addq	$2, %rsi
	cmpq	%rsi, -2760(%rbp)
	jnb	.L418
	movq	-2760(%rbp), %rbx
	cmpq	%rax, %rbx
	je	.L419
	movq	-2648(%rbp), %rax
	movdqa	%xmm2, %xmm0
	movdqu	-16(%rax,%rbx,8), %xmm1
	movdqa	%xmm1, %xmm3
	psubq	%xmm1, %xmm0
	pcmpeqd	%xmm5, %xmm3
	pcmpgtd	%xmm5, %xmm1
	pand	%xmm3, %xmm0
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L593
.L419:
	movl	$3, -2812(%rbp)
	pcmpeqd	%xmm0, %xmm0
	paddq	%xmm0, %xmm2
	jmp	.L423
.L610:
	movq	%r15, %rdi
	call	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	movq	-2648(%rbp), %rcx
	movq	(%r15), %rax
	movq	%r15, %rsi
	movq	%rax, (%rcx)
	movq	-16(%r13), %rax
	leaq	8(%rcx), %rdi
	andq	$-8, %rdi
	movq	%rax, -16(%r14)
	movq	%rcx, %rax
	subq	%rdi, %rax
	leal	(%rbx,%rax), %ecx
	subq	%rax, %rsi
	shrl	$3, %ecx
	rep movsq
	jmp	.L377
	.cfi_endproc
.LFE18797:
	.size	_ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0, .-_ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	.section	.text._ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0, @function
_ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0:
.LFB18799:
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
	movq	%rsi, -136(%rbp)
	movq	%r9, -128(%rbp)
	cmpq	$128, %rdx
	jbe	.L786
	movq	%rdi, %r11
	movq	%rdi, -152(%rbp)
	movq	%r8, %rbx
	shrq	$3, %r11
	movq	%r11, %rdi
	andl	$7, %edi
	movq	%rdi, -144(%rbp)
	jne	.L787
	movq	%rdx, -120(%rbp)
	movq	%r13, %r11
.L625:
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
	movabsq	$34359738359, %r9
	xorq	%r14, %r10
	addq	%rsi, %r8
	movl	%esi, %esi
	vmovq	%r10, %xmm6
	movq	-120(%rbp), %r10
	vpinsrq	$1, %r8, %xmm6, %xmm0
	movq	%r10, %r14
	vmovdqu	%xmm0, (%rbx)
	shrq	$3, %r14
	cmpq	%r9, %r10
	movl	$4294967295, %r9d
	movq	%r14, %r8
	leaq	192(%r12), %r14
	cmova	%r9, %r8
	movl	%edi, %r9d
	shrq	$32, %rdi
	imulq	%r8, %r9
	imulq	%r8, %rdi
	imulq	%r8, %rsi
	shrq	$32, %r9
	salq	$6, %r9
	shrq	$32, %rdi
	vmovdqa64	(%r11,%r9), %zmm2
	movl	%ecx, %r9d
	shrq	$32, %rcx
	imulq	%r8, %r9
	salq	$6, %rdi
	shrq	$32, %rsi
	imulq	%r8, %rcx
	salq	$6, %rsi
	vmovdqa64	(%r11,%rsi), %zmm5
	shrq	$32, %r9
	salq	$6, %r9
	shrq	$32, %rcx
	vmovdqa64	(%r11,%r9), %zmm3
	salq	$6, %rcx
	vpminsq	%zmm3, %zmm2, %zmm0
	vpmaxsq	(%r11,%rdi), %zmm0, %zmm0
	vpmaxsq	%zmm3, %zmm2, %zmm2
	vpminsq	%zmm2, %zmm0, %zmm0
	vmovdqa64	(%r11,%rcx), %zmm2
	movq	%rdx, %rcx
	movl	%edx, %edx
	shrq	$32, %rcx
	imulq	%r8, %rdx
	vmovdqa64	%zmm0, (%r12)
	imulq	%r8, %rcx
	shrq	$32, %rdx
	shrq	$32, %rcx
	salq	$6, %rdx
	salq	$6, %rcx
	vmovdqa64	(%r11,%rcx), %zmm4
	vpminsq	%zmm4, %zmm2, %zmm3
	vpmaxsq	(%r11,%rdx), %zmm3, %zmm3
	movl	%eax, %edx
	shrq	$32, %rax
	imulq	%r8, %rdx
	vpmaxsq	%zmm4, %zmm2, %zmm2
	imulq	%r8, %rax
	vpminsq	%zmm2, %zmm3, %zmm3
	vmovdqa64	%zmm3, 64(%r12)
	shrq	$32, %rdx
	salq	$6, %rdx
	shrq	$32, %rax
	vmovdqa64	(%r11,%rdx), %zmm4
	salq	$6, %rax
	vpminsq	%zmm5, %zmm4, %zmm2
	vpmaxsq	(%r11,%rax), %zmm2, %zmm1
	vpmaxsq	%zmm5, %zmm4, %zmm4
	vpbroadcastq	%xmm0, %zmm5
	vpminsq	%zmm4, %zmm1, %zmm1
	vpxord	%zmm0, %zmm5, %zmm0
	vpxord	%zmm3, %zmm5, %zmm3
	vmovdqa64	%zmm1, 128(%r12)
	vpord	%zmm3, %zmm0, %zmm0
	vpxord	%zmm1, %zmm5, %zmm1
	vmovdqa32	%zmm5, %zmm2
	vpord	%zmm1, %zmm0, %zmm0
	vptestnmq	%zmm0, %zmm0, %k0
	kortestb	%k0, %k0
	jc	.L627
	vpbroadcastq	.LC10(%rip), %zmm0
	movl	$2, %esi
	movq	%r12, %rdi
	vmovdqu64	%zmm0, 192(%r12)
	vmovdqu64	%zmm0, 256(%r12)
	vzeroupper
	call	_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	vpbroadcastq	(%r12), %zmm2
	vpbroadcastq	184(%r12), %zmm1
	vpternlogd	$0xFF, %zmm0, %zmm0, %zmm0
	vpaddq	%zmm0, %zmm1, %zmm0
	vpcmpq	$0, %zmm0, %zmm2, %k0
	kortestb	%k0, %k0
	jnc	.L629
	leaq	-112(%rbp), %rdx
	movq	%r14, %rcx
	vmovdqa64	%zmm2, %zmm0
	movq	%r15, %rsi
	movq	%r13, %rdi
	call	_ZN3hwy11N_AVX3_ZEN46detail22MaybePartitionTwoValueINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L781
.L629:
	movq	96(%r12), %rdx
	cmpq	%rdx, 88(%r12)
	jne	.L727
	cmpq	80(%r12), %rdx
	jne	.L670
	cmpq	72(%r12), %rdx
	jne	.L728
	cmpq	64(%r12), %rdx
	jne	.L729
	cmpq	56(%r12), %rdx
	jne	.L730
	cmpq	48(%r12), %rdx
	jne	.L731
	cmpq	40(%r12), %rdx
	jne	.L732
	cmpq	32(%r12), %rdx
	jne	.L733
	cmpq	24(%r12), %rdx
	jne	.L734
	cmpq	16(%r12), %rdx
	jne	.L735
	cmpq	8(%r12), %rdx
	jne	.L736
	movq	(%r12), %rax
	cmpq	%rax, %rdx
	jne	.L788
.L672:
	vpbroadcastq	%rax, %zmm2
.L785:
	movl	$1, -144(%rbp)
.L667:
	cmpq	$0, -128(%rbp)
	je	.L789
	leaq	-8(%r15), %rdx
	movq	%rdx, %r9
	leaq	0(%r13,%rdx,8), %r10
	movq	%rdx, %rcx
	andl	$31, %r9d
	vmovdqu64	(%r10), %zmm16
	movq	%r9, -120(%rbp)
	andl	$24, %ecx
	je	.L677
	vmovdqu64	0(%r13), %zmm1
	leaq	_ZZN3hwy11N_AVX3_ZEN4L8CompressIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array(%rip), %r14
	vmovdqa64	.LC9(%rip), %zmm0
	movq	$-1, %rdi
	vpcmpq	$6, %zmm2, %zmm1, %k1
	knotb	%k1, %k4
	kmovb	%k4, %eax
	movq	%rax, %rsi
	kmovb	%k1, %ecx
	vpbroadcastq	(%r14,%rsi,8), %zmm3
	movzbl	%cl, %esi
	popcntq	%rax, %rax
	xorl	%ecx, %ecx
	bzhi	%rax, %rdi, %r8
	kmovb	%r8d, %k6
	leaq	0(%r13,%rax,8), %rax
	popcntq	%rsi, %rcx
	vpsrlvq	%zmm0, %zmm3, %zmm3
	vpermq	%zmm1, %zmm3, %zmm3
	vmovdqu64	%zmm3, 0(%r13){%k6}
	vpbroadcastq	(%r14,%rsi,8), %zmm3
	vpsrlvq	%zmm0, %zmm3, %zmm3
	vpermq	%zmm1, %zmm3, %zmm1
	vmovdqu64	%zmm1, (%r12)
	testb	$16, %dl
	je	.L678
	vmovdqu64	64(%r13), %zmm1
	vpcmpq	$6, %zmm2, %zmm1, %k1
	knotb	%k1, %k4
	kmovb	%k4, %esi
	movq	%rsi, %r10
	kmovb	%k1, %r8d
	vpbroadcastq	(%r14,%r10,8), %zmm3
	popcntq	%rsi, %rsi
	bzhi	%rsi, %rdi, %r11
	kmovb	%r11d, %k7
	vpsrlvq	%zmm0, %zmm3, %zmm3
	vpermq	%zmm1, %zmm3, %zmm3
	vmovdqu64	%zmm3, (%rax){%k7}
	leaq	(%rax,%rsi,8), %rax
	movzbl	%r8b, %esi
	xorl	%r8d, %r8d
	vpbroadcastq	(%r14,%rsi,8), %zmm3
	popcntq	%rsi, %r8
	vpsrlvq	%zmm0, %zmm3, %zmm3
	vpermq	%zmm1, %zmm3, %zmm1
	vmovdqu64	%zmm1, (%r12,%rcx,8)
	addq	%r8, %rcx
	cmpq	$23, %r9
	jbe	.L678
	vmovdqu64	128(%r13), %zmm1
	vpcmpq	$6, %zmm2, %zmm1, %k1
	knotb	%k1, %k4
	kmovb	%k4, %esi
	movq	%rsi, %r10
	kmovb	%k1, %r8d
	vpbroadcastq	(%r14,%r10,8), %zmm3
	popcntq	%rsi, %rsi
	bzhi	%rsi, %rdi, %rdi
	kmovb	%edi, %k2
	vpsrlvq	%zmm0, %zmm3, %zmm3
	vpermq	%zmm1, %zmm3, %zmm3
	vmovdqu64	%zmm3, (%rax){%k2}
	leaq	(%rax,%rsi,8), %rax
	movzbl	%r8b, %esi
	xorl	%r8d, %r8d
	vpbroadcastq	(%r14,%rsi,8), %zmm3
	popcntq	%rsi, %r8
	leaq	1(%r9), %rsi
	vpsrlvq	%zmm0, %zmm3, %zmm3
	vpermq	%zmm1, %zmm3, %zmm1
	vmovdqu64	%zmm1, (%r12,%rcx,8)
	addq	%r8, %rcx
	cmpq	$9, %rsi
	sbbq	%rsi, %rsi
	leaq	0(,%rcx,8), %r8
	andq	$-16, %rsi
	addq	$24, %rsi
	cmpq	%rsi, %r9
	jne	.L790
.L680:
	movq	%rax, %r9
	movq	%rdx, %rsi
	subq	%r13, %r9
	subq	%rcx, %rsi
	movq	%r9, %rdi
	leaq	0(%r13,%rsi,8), %r11
	sarq	$3, %rdi
	subq	%rdi, %rdx
	movq	%rdi, -120(%rbp)
	movq	%rdx, %rcx
	movq	%rdx, -152(%rbp)
	movq	%rsi, %rdx
	subq	%rdi, %rdx
	leaq	(%rax,%rcx,8), %rdi
	leaq	(%rax,%rdx,8), %r10
	vmovq	%rdi, %xmm17
	.p2align 4,,10
	.p2align 3
.L682:
	cmpl	$8, %r8d
	jnb	.L684
	testl	%r8d, %r8d
	jne	.L791
.L685:
	cmpl	$8, %r8d
	jnb	.L688
	testl	%r8d, %r8d
	jne	.L792
.L689:
	testq	%rdx, %rdx
	je	.L739
.L703:
	leaq	256(%rax), %rsi
	leaq	-256(%r10), %rdi
	vmovdqu64	(%rax), %zmm15
	vmovdqu64	64(%rax), %zmm14
	vmovdqu64	128(%rax), %zmm13
	vmovdqu64	192(%rax), %zmm12
	vmovdqu64	-128(%r10), %zmm9
	vmovdqu64	-64(%r10), %zmm8
	vmovdqu64	-256(%r10), %zmm11
	vmovdqu64	-192(%r10), %zmm10
	cmpq	%rdi, %rsi
	je	.L740
	xorl	%ecx, %ecx
	leaq	_ZZN3hwy11N_AVX3_ZEN4L11CompressNotIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array(%rip), %r9
	movl	$8, %r8d
	jmp	.L696
	.p2align 4,,10
	.p2align 3
.L794:
	vmovdqu64	-128(%rdi), %zmm3
	vmovdqu64	-64(%rdi), %zmm1
	prefetcht0	-1024(%rdi)
	subq	$256, %rdi
	vmovdqu64	(%rdi), %zmm5
	vmovdqu64	64(%rdi), %zmm4
.L695:
	vpcmpq	$6, %zmm2, %zmm5, %k2
	vpcmpq	$6, %zmm2, %zmm4, %k7
	vpcmpq	$6, %zmm2, %zmm3, %k6
	vpcmpq	$6, %zmm2, %zmm1, %k5
	kmovb	%k2, %r11d
	vpbroadcastq	(%r9,%r11,8), %zmm18
	movq	%r11, %r10
	leaq	-8(%rdx,%rcx), %r11
	popcntq	%r10, %r10
	vpsrlvq	%zmm0, %zmm18, %zmm18
	vpermq	%zmm5, %zmm18, %zmm5
	vmovdqu64	%zmm5, (%rax,%rcx,8)
	addq	$8, %rcx
	vmovdqu64	%zmm5, (%rax,%r11,8)
	kmovb	%k7, %r11d
	subq	%r10, %rcx
	vpbroadcastq	(%r9,%r11,8), %zmm5
	movq	%r11, %r10
	leaq	-16(%rdx,%rcx), %r11
	vpsrlvq	%zmm0, %zmm5, %zmm5
	popcntq	%r10, %r10
	vpermq	%zmm4, %zmm5, %zmm4
	vmovdqu64	%zmm4, (%rax,%rcx,8)
	vmovdqu64	%zmm4, (%rax,%r11,8)
	movq	%r8, %r11
	subq	%r10, %r11
	addq	%r11, %rcx
	kmovb	%k6, %r11d
	vpbroadcastq	(%r9,%r11,8), %zmm4
	movq	%r11, %r10
	leaq	-24(%rdx,%rcx), %r11
	popcntq	%r10, %r10
	subq	$32, %rdx
	vpsrlvq	%zmm0, %zmm4, %zmm4
	vpermq	%zmm3, %zmm4, %zmm3
	vmovdqu64	%zmm3, (%rax,%rcx,8)
	vmovdqu64	%zmm3, (%rax,%r11,8)
	movq	%r8, %r11
	subq	%r10, %r11
	addq	%rcx, %r11
	kmovb	%k5, %ecx
	vpbroadcastq	(%r9,%rcx,8), %zmm3
	movq	%rcx, %r10
	leaq	(%r11,%rdx), %rcx
	popcntq	%r10, %r10
	vpsrlvq	%zmm0, %zmm3, %zmm3
	vpermq	%zmm1, %zmm3, %zmm1
	vmovdqu64	%zmm1, (%rax,%r11,8)
	vmovdqu64	%zmm1, (%rax,%rcx,8)
	movq	%r8, %rcx
	subq	%r10, %rcx
	addq	%r11, %rcx
	cmpq	%rdi, %rsi
	je	.L793
.L696:
	movq	%rsi, %r10
	subq	%rax, %r10
	sarq	$3, %r10
	subq	%rcx, %r10
	cmpq	$32, %r10
	ja	.L794
	vmovdqu64	(%rsi), %zmm5
	vmovdqu64	64(%rsi), %zmm4
	prefetcht0	1024(%rsi)
	addq	$256, %rsi
	vmovdqu64	-128(%rsi), %zmm3
	vmovdqu64	-64(%rsi), %zmm1
	jmp	.L695
	.p2align 4,,10
	.p2align 3
.L787:
	movl	$8, %eax
	subq	%rdi, %rax
	leaq	0(%r13,%rax,8), %r11
	leaq	-8(%rdi,%rdx), %rax
	movq	%rax, -120(%rbp)
	jmp	.L625
	.p2align 4,,10
	.p2align 3
.L793:
	leaq	(%rdx,%rcx), %r8
	leaq	(%rax,%rcx,8), %r10
	addq	$8, %rcx
.L693:
	vpcmpq	$6, %zmm2, %zmm15, %k4
	vpcmpq	$6, %zmm2, %zmm14, %k3
	xorl	%esi, %esi
	vpcmpq	$6, %zmm2, %zmm13, %k1
	kmovb	%k4, %edi
	vpbroadcastq	(%r9,%rdi,8), %zmm1
	popcntq	%rdi, %rsi
	kmovb	%k3, %edi
	subq	%rsi, %rcx
	movq	%rdi, %rsi
	vpsrlvq	%zmm0, %zmm1, %zmm1
	popcntq	%rsi, %rsi
	vpcmpq	$6, %zmm2, %zmm12, %k4
	vpcmpq	$6, %zmm2, %zmm11, %k3
	vpermq	%zmm15, %zmm1, %zmm15
	vpbroadcastq	(%r9,%rdi,8), %zmm1
	leaq	-16(%rdx,%rcx), %rdi
	vmovdqu64	%zmm15, (%r10)
	vpsrlvq	%zmm0, %zmm1, %zmm1
	vmovdqu64	%zmm15, -64(%rax,%r8,8)
	vpermq	%zmm14, %zmm1, %zmm14
	vmovdqu64	%zmm14, (%rax,%rcx,8)
	subq	%rsi, %rcx
	vmovdqu64	%zmm14, (%rax,%rdi,8)
	kmovb	%k1, %edi
	addq	$8, %rcx
	vpbroadcastq	(%r9,%rdi,8), %zmm1
	movq	%rdi, %rsi
	vpcmpq	$6, %zmm2, %zmm10, %k1
	leaq	-24(%rdx,%rcx), %rdi
	vpsrlvq	%zmm0, %zmm1, %zmm1
	vpermq	%zmm13, %zmm1, %zmm13
	vmovdqu64	%zmm13, (%rax,%rcx,8)
	vmovdqu64	%zmm13, (%rax,%rdi,8)
	xorl	%edi, %edi
	popcntq	%rsi, %rdi
	movl	$8, %esi
	movq	%rsi, %r8
	subq	%rdi, %r8
	kmovb	%k4, %edi
	vpbroadcastq	(%r9,%rdi,8), %zmm1
	vpcmpq	$6, %zmm2, %zmm9, %k4
	addq	%rcx, %r8
	movq	%rdi, %rcx
	vpsrlvq	%zmm0, %zmm1, %zmm1
	leaq	-32(%rdx,%r8), %rdi
	popcntq	%rcx, %rcx
	vpermq	%zmm12, %zmm1, %zmm12
	vmovdqu64	%zmm12, (%rax,%r8,8)
	vmovdqu64	%zmm12, (%rax,%rdi,8)
	movq	%rsi, %rdi
	subq	%rcx, %rdi
	addq	%r8, %rdi
	kmovb	%k3, %r8d
	vpbroadcastq	(%r9,%r8,8), %zmm1
	movq	%r8, %rcx
	leaq	-40(%rdx,%rdi), %r8
	popcntq	%rcx, %rcx
	vpcmpq	$6, %zmm2, %zmm8, %k3
	vpsrlvq	%zmm0, %zmm1, %zmm1
	vpermq	%zmm11, %zmm1, %zmm11
	vmovdqu64	%zmm11, (%rax,%rdi,8)
	vmovdqu64	%zmm11, (%rax,%r8,8)
	movq	%rsi, %r8
	subq	%rcx, %r8
	addq	%rdi, %r8
	kmovb	%k1, %edi
	vpbroadcastq	(%r9,%rdi,8), %zmm1
	movq	%rdi, %rcx
	leaq	-48(%rdx,%r8), %rdi
	popcntq	%rcx, %rcx
	vpsrlvq	%zmm0, %zmm1, %zmm1
	vpermq	%zmm10, %zmm1, %zmm10
	vmovdqu64	%zmm10, (%rax,%r8,8)
	vmovdqu64	%zmm10, (%rax,%rdi,8)
	movq	%rsi, %rdi
	subq	%rcx, %rdi
	addq	%r8, %rdi
	kmovb	%k4, %r8d
	vpbroadcastq	(%r9,%r8,8), %zmm1
	movq	%r8, %rcx
	leaq	-56(%rdx,%rdi), %r8
	popcntq	%rcx, %rcx
	vpsrlvq	%zmm0, %zmm1, %zmm1
	vpermq	%zmm9, %zmm1, %zmm9
	vmovdqu64	%zmm9, (%rax,%rdi,8)
	vmovdqu64	%zmm9, (%rax,%r8,8)
	movq	%rsi, %r8
	subq	%rcx, %r8
	xorl	%ecx, %ecx
	addq	%rdi, %r8
	kmovb	%k3, %edi
	vpbroadcastq	(%r9,%rdi,8), %zmm1
	popcntq	%rdi, %rcx
	leaq	-64(%rdx,%r8), %rdx
	subq	%rcx, %rsi
	vpsrlvq	%zmm0, %zmm1, %zmm1
	vpermq	%zmm8, %zmm1, %zmm8
	vmovdqu64	%zmm8, (%rax,%r8,8)
	vmovdqu64	%zmm8, (%rax,%rdx,8)
	leaq	(%rsi,%r8), %rdx
	movq	-152(%rbp), %rsi
	leaq	(%rax,%rdx,8), %rcx
	subq	%rdx, %rsi
.L692:
	movq	%rcx, %rdi
	cmpq	$7, %rsi
	ja	.L697
	movq	-152(%rbp), %rdi
	leaq	-64(%rax,%rdi,8), %rdi
.L697:
	vpcmpq	$6, %zmm2, %zmm16, %k3
	vmovdqu64	(%rdi), %zmm7
	vmovq	%xmm17, %rsi
	vmovdqu64	%zmm7, (%rsi)
	movq	$-1, %rsi
	knotb	%k3, %k4
	kmovb	%k4, %edi
	movq	%rdi, %r10
	kmovb	%k3, %r8d
	vpbroadcastq	(%r14,%r10,8), %zmm1
	popcntq	%rdi, %rdi
	addq	%rdi, %rdx
	bzhi	%rdi, %rsi, %r11
	kmovb	%r11d, %k3
	vpsrlvq	%zmm0, %zmm1, %zmm1
	vpermq	%zmm16, %zmm1, %zmm1
	vmovdqu64	%zmm1, (%rcx){%k3}
	vpbroadcastq	(%r14,%r8,8), %zmm1
	movzbl	%r8b, %ecx
	popcntq	%rcx, %rcx
	bzhi	%rcx, %rsi, %rsi
	kmovb	%esi, %k4
	vpsrlvq	%zmm0, %zmm1, %zmm0
	vpermq	%zmm16, %zmm0, %zmm0
	vmovdqu64	%zmm0, (%rax,%rdx,8){%k4}
	movq	-120(%rbp), %r14
	movq	-128(%rbp), %r9
	addq	%rdx, %r14
	subq	$1, %r9
	cmpl	$2, -144(%rbp)
	je	.L795
	movq	-136(%rbp), %rsi
	movq	%rbx, %r8
	movq	%r12, %rcx
	movq	%r14, %rdx
	movq	%r13, %rdi
	movq	%r9, -120(%rbp)
	vzeroupper
	call	_ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	cmpl	$3, -144(%rbp)
	movq	-120(%rbp), %r9
	je	.L781
.L699:
	movq	%r15, %rdx
	movq	-136(%rbp), %rsi
	movq	%rbx, %r8
	movq	%r12, %rcx
	subq	%r14, %rdx
	leaq	0(%r13,%r14,8), %rdi
	call	_ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L781:
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
.L688:
	.cfi_restore_state
	movq	(%r12), %rcx
	movl	%r8d, %esi
	leaq	8(%r11), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%r11)
	movq	-8(%r12,%rsi), %rcx
	movq	%rcx, -8(%r11,%rsi)
	movq	%r11, %rcx
	movq	%r12, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%r8d, %ecx
	shrl	$3, %ecx
	rep movsq
	jmp	.L689
	.p2align 4,,10
	.p2align 3
.L684:
	movq	(%r11), %rcx
	movl	%r8d, %esi
	leaq	8(%rax), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%rax)
	movq	-8(%r11,%rsi), %rcx
	movq	%rcx, -8(%rax,%rsi)
	movq	%rax, %rcx
	movq	%r11, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%r8d, %ecx
	shrl	$3, %ecx
	rep movsq
	jmp	.L685
	.p2align 4,,10
	.p2align 3
.L790:
	subq	%rsi, -120(%rbp)
	leaq	0(%r13,%rsi,8), %r10
	leaq	(%r12,%r8), %rsi
.L702:
	movq	-120(%rbp), %r9
	movq	$-1, %rdi
	bzhi	%r9, %rdi, %rdi
	movzbl	%dil, %edi
	kmovd	%edi, %k1
.L683:
	vmovdqu64	(%r10), %zmm1
	movq	$-1, %rdi
	vpcmpq	$6, %zmm2, %zmm1, %k0
	kandnb	%k1, %k0, %k4
	kmovb	%k4, %r8d
	movq	%r8, %r9
	popcntq	%r8, %r8
	vpbroadcastq	(%r14,%r9,8), %zmm3
	bzhi	%r8, %rdi, %rdi
	kmovb	%edi, %k5
	kandb	%k1, %k0, %k1
	kmovb	%k1, %edi
	vpsrlvq	%zmm0, %zmm3, %zmm3
	vpermq	%zmm1, %zmm3, %zmm3
	vmovdqu64	%zmm3, (%rax){%k5}
	leaq	(%rax,%r8,8), %rax
	xorl	%r8d, %r8d
	vpbroadcastq	(%r14,%rdi,8), %zmm3
	movq	%rax, %r9
	popcntq	%rdi, %r8
	addq	%rcx, %r8
	movq	%rdx, %rcx
	subq	%r13, %r9
	vpsrlvq	%zmm0, %zmm3, %zmm3
	subq	%r8, %rcx
	salq	$3, %r8
	movq	%r9, %rdi
	leaq	0(%r13,%rcx,8), %r11
	sarq	$3, %rdi
	vpermq	%zmm1, %zmm3, %zmm1
	subq	%rdi, %rdx
	vmovdqu64	%zmm1, (%rsi)
	movq	%rdx, -152(%rbp)
	movq	%rdx, %rsi
	movq	%rcx, %rdx
	subq	%rdi, %rdx
	movq	%rdi, -120(%rbp)
	leaq	(%rax,%rsi,8), %rdi
	leaq	(%rax,%rdx,8), %r10
	vmovq	%rdi, %xmm17
	jmp	.L682
	.p2align 4,,10
	.p2align 3
.L792:
	movzbl	(%r12), %ecx
	movb	%cl, (%r11)
	jmp	.L689
	.p2align 4,,10
	.p2align 3
.L791:
	movzbl	(%r11), %ecx
	movb	%cl, (%rax)
	jmp	.L685
	.p2align 4,,10
	.p2align 3
.L678:
	movq	-120(%rbp), %r9
	leaq	0(,%rcx,8), %r8
	leaq	-8(%r9), %rsi
	leaq	1(%r9), %rdi
	andq	$-8, %rsi
	addq	$8, %rsi
	cmpq	$8, %rdi
	movl	$8, %edi
	cmovbe	%rdi, %rsi
	cmpq	%rsi, %r9
	je	.L680
	subq	%rsi, -120(%rbp)
	movq	-120(%rbp), %rdi
	leaq	0(%r13,%rsi,8), %r10
	leaq	(%r12,%r8), %rsi
	cmpq	$255, %rdi
	jbe	.L702
	movl	$255, %edi
	kmovd	%edi, %k1
	jmp	.L683
	.p2align 4,,10
	.p2align 3
.L786:
	cmpq	$1, %rdx
	jbe	.L781
	leaq	1024(%rdi), %rax
	cmpq	%rax, %rsi
	jb	.L796
	movl	$8, %esi
	call	_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	jmp	.L781
	.p2align 4,,10
	.p2align 3
.L627:
	vmovdqu64	0(%r13), %zmm6
	movl	$8, %esi
	movq	$-1, %rax
	subq	-144(%rbp), %rsi
	bzhi	%rsi, %rax, %rax
	kmovb	%eax, %k1
	vpcmpq	$4, %zmm5, %zmm6, %k0
	kandb	%k1, %k0, %k0
	kmovb	%k0, %eax
	kortestb	%k0, %k0
	jne	.L797
	vpxor	%xmm4, %xmm4, %xmm4
	leaq	1024(%r13,%rsi,8), %rdi
	vmovdqa64	%zmm4, %zmm3
	.p2align 4,,10
	.p2align 3
.L633:
	movq	%rsi, %rcx
	subq	$-128, %rsi
	cmpq	%rsi, %r15
	jb	.L637
	leaq	-1024(%rdi), %rax
.L632:
	vpxord	(%rax), %zmm2, %zmm0
	vpxord	64(%rax), %zmm2, %zmm1
	leaq	128(%rax), %rdx
	vpord	%zmm3, %zmm0, %zmm3
	vpord	%zmm4, %zmm1, %zmm4
	vpxord	128(%rax), %zmm2, %zmm0
	vpxord	192(%rax), %zmm2, %zmm1
	vpord	%zmm3, %zmm0, %zmm3
	vpxord	256(%rax), %zmm2, %zmm0
	vpord	%zmm4, %zmm1, %zmm4
	vpxord	320(%rax), %zmm2, %zmm1
	leaq	384(%rdx), %rax
	vpord	%zmm3, %zmm0, %zmm3
	vpxord	256(%rdx), %zmm2, %zmm0
	vpord	%zmm4, %zmm1, %zmm4
	vpxord	320(%rdx), %zmm2, %zmm1
	vpord	%zmm3, %zmm0, %zmm0
	vpord	%zmm4, %zmm1, %zmm1
	vmovdqa64	%zmm0, %zmm3
	vmovdqa64	%zmm1, %zmm4
	cmpq	%rdi, %rax
	jne	.L632
	vpord	%zmm1, %zmm0, %zmm0
	leaq	1408(%rdx), %rdi
	vptestnmq	%zmm0, %zmm0, %k0
	kortestb	%k0, %k0
	setc	%al
	testb	%al, %al
	jne	.L633
	vmovdqa64	0(%r13,%rcx,8), %zmm6
	vpcmpq	$4, %zmm5, %zmm6, %k0
	kortestb	%k0, %k0
	jne	.L635
	.p2align 4,,10
	.p2align 3
.L634:
	addq	$8, %rcx
	vmovdqa64	0(%r13,%rcx,8), %zmm7
	vpcmpq	$4, %zmm5, %zmm7, %k0
	kortestb	%k0, %k0
	je	.L634
.L635:
	kmovb	%k0, %eax
	tzcntl	%eax, %eax
	addq	%rcx, %rax
.L631:
	vpbroadcastq	0(%r13,%rax,8), %zmm1
	leaq	0(%r13,%rax,8), %rdi
	vpcmpq	$6, %zmm5, %zmm1, %k0
	kortestb	%k0, %k0
	jne	.L640
	leaq	-8(%r15), %rax
	xorl	%ecx, %ecx
	jmp	.L646
	.p2align 4,,10
	.p2align 3
.L641:
	kmovb	%k0, %edx
	popcntq	%rdx, %rdx
	addq	%rdx, %rcx
	leaq	-8(%rax), %rdx
	vmovdqu64	%zmm5, 0(%r13,%rax,8)
	cmpq	%rdx, %r15
	jbe	.L798
	movq	%rdx, %rax
.L646:
	vmovdqu64	0(%r13,%rax,8), %zmm6
	vpcmpq	$0, %zmm1, %zmm6, %k1
	vpcmpq	$0, %zmm5, %zmm6, %k0
	kmovb	%k1, %edx
	kmovb	%k0, %esi
	korb	%k0, %k1, %k1
	kortestb	%k1, %k1
	jc	.L641
	kmovb	%edx, %k1
	kmovb	%esi, %k4
	kxnorb	%k4, %k1, %k1
	kmovb	%k1, %edx
	tzcntl	%edx, %edx
	leaq	8(%rax), %rsi
	addq	%rax, %rdx
	addq	$16, %rax
	vpbroadcastq	0(%r13,%rdx,8), %zmm0
	movq	%r15, %rdx
	subq	%rcx, %rdx
	vmovdqa64	%zmm0, -112(%rbp)
	cmpq	%rdx, %rax
	ja	.L642
	.p2align 4,,10
	.p2align 3
.L643:
	vmovdqu64	%zmm1, -64(%r13,%rax,8)
	movq	%rax, %rsi
	addq	$8, %rax
	cmpq	%rax, %rdx
	jnb	.L643
.L642:
	subq	%rsi, %rdx
	leaq	0(%r13,%rsi,8), %rcx
	movl	$255, %eax
	cmpq	$255, %rdx
	ja	.L644
	movq	$-1, %rax
	bzhi	%rdx, %rax, %rax
	movzbl	%al, %eax
.L644:
	kmovb	%eax, %k2
	vmovdqu64	%zmm1, (%rcx){%k2}
.L645:
	vpbroadcastq	(%r12), %zmm2
	vpcmpq	$0, .LC8(%rip), %zmm2, %k0
	kortestb	%k0, %k0
	jc	.L725
	vpcmpq	$0, .LC7(%rip), %zmm2, %k0
	kortestb	%k0, %k0
	jc	.L663
	vpminsq	%zmm0, %zmm1, %zmm3
	vpcmpq	$6, %zmm3, %zmm2, %k0
	kortestb	%k0, %k0
	jne	.L799
	vmovdqa64	%zmm2, %zmm0
	movl	$128, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L658:
	leaq	(%rcx,%rax,8), %rdx
	addq	$1, %rax
	vpminsq	0(%r13,%rdx,8), %zmm0, %zmm0
	cmpq	$16, %rax
	jne	.L658
	vpcmpq	$6, %zmm0, %zmm2, %k0
	kortestb	%k0, %k0
	jne	.L785
	leaq	128(%rsi), %rax
	cmpq	%rax, %r15
	jb	.L665
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L658
	.p2align 4,,10
	.p2align 3
.L637:
	movq	%rcx, %rdx
	addq	$8, %rcx
	cmpq	%rcx, %r15
	jb	.L800
	vmovdqa64	-64(%r13,%rcx,8), %zmm6
	vpcmpq	$4, %zmm5, %zmm6, %k0
	kmovb	%k0, %eax
	kortestb	%k0, %k0
	je	.L637
.L783:
	tzcntl	%eax, %eax
	addq	%rdx, %rax
	jmp	.L631
	.p2align 4,,10
	.p2align 3
.L677:
	cmpq	$0, -120(%rbp)
	je	.L801
	movq	%r12, %rsi
	movq	%r13, %r10
	leaq	_ZZN3hwy11N_AVX3_ZEN4L8CompressIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array(%rip), %r14
	movq	%r13, %rax
	vmovdqa64	.LC9(%rip), %zmm0
	jmp	.L702
	.p2align 4,,10
	.p2align 3
.L795:
	vzeroupper
	jmp	.L699
	.p2align 4,,10
	.p2align 3
.L739:
	movq	-152(%rbp), %rsi
	movq	%rax, %rcx
	jmp	.L692
	.p2align 4,,10
	.p2align 3
.L740:
	movq	%rax, %r10
	movq	%rdx, %r8
	movl	$8, %ecx
	leaq	_ZZN3hwy11N_AVX3_ZEN4L11CompressNotIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array(%rip), %r9
	jmp	.L693
.L789:
	leaq	-1(%r15), %rbx
	movq	%rbx, %r12
	shrq	%r12
	.p2align 4,,10
	.p2align 3
.L675:
	movq	%r12, %rdx
	movq	%r15, %rsi
	movq	%r13, %rdi
	call	_ZN3hwy11N_AVX3_ZEN46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	subq	$1, %r12
	jnb	.L675
	.p2align 4,,10
	.p2align 3
.L676:
	movq	0(%r13,%rbx,8), %rdx
	movq	0(%r13), %rax
	movq	%rbx, %rsi
	movq	%r13, %rdi
	movq	%rdx, 0(%r13)
	xorl	%edx, %edx
	movq	%rax, 0(%r13,%rbx,8)
	call	_ZN3hwy11N_AVX3_ZEN46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	subq	$1, %rbx
	jne	.L676
	jmp	.L781
	.p2align 4,,10
	.p2align 3
.L666:
	vpcmpq	$6, -64(%r13,%rsi,8), %zmm2, %k0
	kortestb	%k0, %k0
	jne	.L785
.L665:
	movq	%rsi, %rax
	addq	$8, %rsi
	cmpq	%rsi, %r15
	jnb	.L666
	cmpq	%rax, %r15
	je	.L725
	vpcmpq	$6, -64(%r13,%r15,8), %zmm2, %k0
	xorl	%eax, %eax
	kortestb	%k0, %k0
	sete	%al
	addl	$1, %eax
	movl	%eax, -144(%rbp)
	jmp	.L667
.L727:
	movl	$12, %eax
	movl	$11, %esi
	jmp	.L673
	.p2align 4,,10
	.p2align 3
.L674:
	cmpq	$23, %rax
	je	.L784
.L673:
	movq	%rax, %rcx
	addq	$1, %rax
	cmpq	(%r12,%rax,8), %rdx
	je	.L674
	movl	$12, %edi
	subq	$11, %rcx
	movq	%rdx, %rax
	subq	%rsi, %rdi
	cmpq	%rdi, %rcx
	jb	.L672
.L784:
	movq	(%r12,%rsi,8), %rax
	jmp	.L672
.L728:
	movl	$9, %esi
	movl	$10, %eax
	jmp	.L673
.L729:
	movl	$8, %esi
	movl	$9, %eax
	jmp	.L673
.L730:
	movl	$7, %esi
	movl	$8, %eax
	jmp	.L673
.L731:
	movl	$6, %esi
	movl	$7, %eax
	jmp	.L673
.L732:
	movl	$5, %esi
	movl	$6, %eax
	jmp	.L673
.L640:
	movq	%r15, %rsi
	leaq	-112(%rbp), %rdx
	vmovdqa64	%zmm5, %zmm0
	movq	%r12, %rcx
	subq	%rax, %rsi
	call	_ZN3hwy11N_AVX3_ZEN46detail22MaybePartitionTwoValueINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L781
	vmovdqa64	-112(%rbp), %zmm0
	jmp	.L645
.L733:
	movl	$4, %esi
	movl	$5, %eax
	jmp	.L673
.L734:
	movl	$3, %esi
	movl	$4, %eax
	jmp	.L673
.L735:
	movl	$2, %esi
	movl	$3, %eax
	jmp	.L673
.L736:
	movl	$1, %esi
	movl	$2, %eax
	jmp	.L673
.L788:
	xorl	%esi, %esi
	movl	$1, %eax
	jmp	.L673
.L798:
	movl	$255, %edi
	vmovdqu64	0(%r13), %zmm0
	kmovd	%edi, %k2
	cmpq	$255, %rax
	ja	.L647
	movq	$-1, %rdx
	bzhi	%rax, %rdx, %rdx
	movzbl	%dl, %edi
	kmovd	%edi, %k2
.L647:
	vpcmpq	$0, %zmm1, %zmm0, %k0
	vpcmpq	$0, %zmm5, %zmm0, %k1
	movq	%r15, %rsi
	kandb	%k2, %k1, %k1
	knotb	%k2, %k2
	korb	%k1, %k0, %k0
	korb	%k2, %k0, %k0
	kortestb	%k0, %k0
	setc	%dl
	subq	%rcx, %rsi
	testb	%dl, %dl
	je	.L802
	movq	%rsi, %rdx
	kmovb	%k1, %eax
	popcntq	%rax, %rax
	vmovdqu64	%zmm5, 0(%r13)
	subq	%rax, %rdx
	cmpq	$7, %rdx
	jbe	.L652
	leaq	-8(%rdx), %rcx
	movq	-152(%rbp), %rsi
	movq	%rcx, %rax
	shrq	$3, %rax
	salq	$6, %rax
	leaq	64(%r13,%rax), %rax
	.p2align 4,,10
	.p2align 3
.L653:
	vmovdqu64	%zmm1, (%rsi)
	addq	$64, %rsi
	cmpq	%rsi, %rax
	jne	.L653
	andq	$-8, %rcx
	vmovdqa64	%zmm1, (%r12)
	leaq	8(%rcx), %rax
	leaq	0(%r13,%rax,8), %r13
	subq	%rax, %rdx
	movl	$255, %eax
	kmovd	%eax, %k1
	cmpq	$255, %rdx
	jbe	.L704
.L654:
	vmovdqu64	(%r12), %zmm0{%k1}{z}
	vmovdqu64	%zmm0, 0(%r13){%k1}
	vzeroupper
	jmp	.L781
.L670:
	movl	$11, %eax
	movl	$10, %esi
	jmp	.L673
.L800:
	leaq	-8(%r15), %rdx
	vmovdqu64	0(%r13,%rdx,8), %zmm7
	vpcmpq	$4, %zmm5, %zmm7, %k0
	kmovb	%k0, %eax
	kortestb	%k0, %k0
	jne	.L783
	vzeroupper
	jmp	.L781
.L662:
	vmovdqu64	-64(%r13,%rsi,8), %zmm7
	vpcmpq	$6, %zmm2, %zmm7, %k0
	kortestb	%k0, %k0
	jne	.L785
.L661:
	movq	%rsi, %rax
	addq	$8, %rsi
	cmpq	%rsi, %r15
	jnb	.L662
	cmpq	%rax, %r15
	je	.L663
	vmovdqu64	-64(%r13,%r15,8), %zmm7
	vpcmpq	$6, %zmm2, %zmm7, %k0
	kortestb	%k0, %k0
	jne	.L785
.L663:
	movl	$3, -144(%rbp)
	vpternlogd	$0xFF, %zmm0, %zmm0, %zmm0
	vpaddq	%zmm0, %zmm2, %zmm2
	jmp	.L667
.L797:
	tzcntl	%eax, %eax
	jmp	.L631
.L796:
	movq	%rdi, %rcx
	movq	%r12, %rdi
	cmpq	$7, %rdx
	jbe	.L618
	leaq	-8(%rdx), %rdx
	movq	0(%r13), %rcx
	leaq	8(%r12), %rdi
	movq	%rdx, %rax
	andq	$-8, %rdi
	shrq	$3, %rax
	movq	%rcx, (%r12)
	addq	$1, %rax
	salq	$6, %rax
	movl	%eax, %ecx
	movq	-8(%r13,%rcx), %rsi
	movq	%rsi, -8(%r12,%rcx)
	movq	%r12, %rcx
	movq	%r13, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%eax, %ecx
	movq	%rdx, %rax
	movq	%r15, %rdx
	andq	$-8, %rax
	shrl	$3, %ecx
	rep movsq
	addq	$8, %rax
	leaq	0(,%rax,8), %rcx
	subq	%rax, %rdx
	movl	$255, %eax
	leaq	(%r12,%rcx), %rdi
	addq	%r13, %rcx
	cmpq	$255, %rdx
	jbe	.L618
.L619:
	leal	-1(%r15), %edx
	movl	$32, %r8d
	movl	$1, %esi
	kmovb	%eax, %k7
	bsrl	%edx, %edx
	vmovdqu64	(%rcx), %zmm0{%k7}{z}
	movq	%r15, %rax
	xorl	$31, %edx
	subl	%edx, %r8d
	movl	$1, %edx
	vmovdqu64	%zmm0, (%rdi){%k7}
	vpbroadcastq	.LC10(%rip), %zmm0
	shlx	%r8, %rsi, %rsi
	shrq	$4, %rsi
	cmove	%rdx, %rsi
	movq	%rsi, %rdx
	salq	$4, %rdx
	addq	$8, %rdx
	cmpq	%rdx, %r15
	jnb	.L623
	.p2align 4,,10
	.p2align 3
.L620:
	vmovdqu64	%zmm0, (%r12,%rax,8)
	addq	$8, %rax
	cmpq	%rdx, %rax
	jb	.L620
.L623:
	movq	%r12, %rdi
	vzeroupper
	call	_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	cmpq	$7, %r15
	jbe	.L622
	leaq	-8(%r15), %rdx
	movq	(%r12), %rcx
	leaq	8(%r13), %rdi
	movq	%rdx, %rax
	andq	$-8, %rdi
	shrq	$3, %rax
	movq	%rcx, 0(%r13)
	addq	$1, %rax
	salq	$6, %rax
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
	leaq	0(,%rax,8), %rdx
	subq	%rax, %r15
	movl	$255, %eax
	addq	%rdx, %r13
	addq	%rdx, %r12
	cmpq	$255, %r15
	jbe	.L622
.L624:
	kmovb	%eax, %k7
	vmovdqu64	(%r12), %zmm0{%k7}{z}
	vmovdqu64	%zmm0, 0(%r13){%k7}
	vzeroupper
	jmp	.L781
.L652:
	vmovdqa64	%zmm1, (%r12)
.L704:
	movq	$-1, %rax
	bzhi	%rdx, %rax, %rax
	movzbl	%al, %eax
	kmovd	%eax, %k1
	jmp	.L654
.L801:
	movq	%rdx, -152(%rbp)
	vmovq	%r10, %xmm17
	movq	%r13, %rax
	vmovdqa64	.LC9(%rip), %zmm0
	leaq	_ZZN3hwy11N_AVX3_ZEN4L8CompressIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array(%rip), %r14
	jmp	.L703
.L725:
	movl	$2, -144(%rbp)
	jmp	.L667
.L799:
	vpmaxsq	%zmm0, %zmm1, %zmm1
	vpcmpq	$6, %zmm2, %zmm1, %k0
	kortestb	%k0, %k0
	jne	.L785
	vmovdqa64	%zmm2, %zmm0
	movl	$128, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L659:
	leaq	(%rcx,%rax,8), %rdx
	addq	$1, %rax
	vpmaxsq	0(%r13,%rdx,8), %zmm0, %zmm0
	cmpq	$16, %rax
	jne	.L659
	vpcmpq	$6, %zmm2, %zmm0, %k0
	kortestb	%k0, %k0
	jne	.L785
	leaq	128(%rsi), %rax
	cmpq	%rax, %r15
	jb	.L661
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L659
.L618:
	movq	$-1, %rax
	bzhi	%rdx, %rax, %rax
	movzbl	%al, %eax
	jmp	.L619
.L622:
	movq	$-1, %rax
	bzhi	%r15, %rax, %rax
	movzbl	%al, %eax
	jmp	.L624
.L802:
	knotb	%k0, %k1
	kmovb	%k1, %edx
	tzcntl	%edx, %edx
	vpbroadcastq	0(%r13,%rdx,8), %zmm0
	leaq	8(%rax), %rdx
	vmovdqa64	%zmm0, -112(%rbp)
	cmpq	%rsi, %rdx
	ja	.L649
.L650:
	vmovdqu64	%zmm1, -64(%r13,%rdx,8)
	movq	%rdx, %rax
	addq	$8, %rdx
	cmpq	%rsi, %rdx
	jbe	.L650
.L649:
	movq	%rsi, %rdx
	leaq	0(%r13,%rax,8), %rcx
	subq	%rax, %rdx
	movl	$-1, %eax
	cmpq	$255, %rdx
	ja	.L651
	orq	$-1, %rax
	bzhi	%rdx, %rax, %rax
.L651:
	kmovb	%eax, %k7
	vmovdqu64	%zmm1, (%rcx){%k7}
	jmp	.L645
	.cfi_endproc
.LFE18799:
	.size	_ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0, .-_ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	.section	.text._ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0, @function
_ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0:
.LFB18801:
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
	cmpq	$128, %rdx
	jbe	.L977
	movq	%rdi, %r11
	movq	%rdi, -152(%rbp)
	movq	%r8, %rbx
	shrq	$3, %r11
	movq	%r11, %rdi
	andl	$7, %edi
	movq	%rdi, -144(%rbp)
	jne	.L978
	movq	%rdx, -136(%rbp)
	movq	%r13, %r11
.L816:
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
	movabsq	$34359738359, %r9
	xorq	%r14, %r10
	addq	%rsi, %r8
	movl	%esi, %esi
	vmovq	%r10, %xmm6
	movq	-136(%rbp), %r10
	vpinsrq	$1, %r8, %xmm6, %xmm0
	movq	%r10, %r14
	vmovdqu	%xmm0, (%rbx)
	shrq	$3, %r14
	cmpq	%r9, %r10
	movl	$4294967295, %r9d
	movq	%r14, %r8
	leaq	192(%r12), %r14
	cmova	%r9, %r8
	movl	%edi, %r9d
	shrq	$32, %rdi
	imulq	%r8, %r9
	imulq	%r8, %rdi
	imulq	%r8, %rsi
	shrq	$32, %r9
	salq	$6, %r9
	shrq	$32, %rdi
	vmovdqa64	(%r11,%r9), %zmm2
	movl	%ecx, %r9d
	shrq	$32, %rcx
	imulq	%r8, %r9
	salq	$6, %rdi
	shrq	$32, %rsi
	imulq	%r8, %rcx
	salq	$6, %rsi
	vmovdqa64	(%r11,%rsi), %zmm5
	shrq	$32, %r9
	salq	$6, %r9
	shrq	$32, %rcx
	vmovdqa64	(%r11,%r9), %zmm3
	salq	$6, %rcx
	vpminsq	%zmm3, %zmm2, %zmm0
	vpmaxsq	(%r11,%rdi), %zmm0, %zmm0
	vpmaxsq	%zmm3, %zmm2, %zmm2
	vpminsq	%zmm2, %zmm0, %zmm0
	vmovdqa64	(%r11,%rcx), %zmm2
	movq	%rdx, %rcx
	movl	%edx, %edx
	shrq	$32, %rcx
	imulq	%r8, %rdx
	vmovdqa64	%zmm0, (%r12)
	imulq	%r8, %rcx
	shrq	$32, %rdx
	shrq	$32, %rcx
	salq	$6, %rdx
	salq	$6, %rcx
	vmovdqa64	(%r11,%rcx), %zmm4
	vpminsq	%zmm4, %zmm2, %zmm3
	vpmaxsq	(%r11,%rdx), %zmm3, %zmm3
	movl	%eax, %edx
	shrq	$32, %rax
	imulq	%r8, %rdx
	vpmaxsq	%zmm4, %zmm2, %zmm2
	imulq	%r8, %rax
	vpminsq	%zmm2, %zmm3, %zmm3
	vmovdqa64	%zmm3, 64(%r12)
	shrq	$32, %rdx
	salq	$6, %rdx
	shrq	$32, %rax
	vmovdqa64	(%r11,%rdx), %zmm4
	salq	$6, %rax
	vpminsq	%zmm5, %zmm4, %zmm2
	vpmaxsq	(%r11,%rax), %zmm2, %zmm1
	vpmaxsq	%zmm5, %zmm4, %zmm4
	vpbroadcastq	%xmm0, %zmm5
	vpminsq	%zmm4, %zmm1, %zmm1
	vpxord	%zmm0, %zmm5, %zmm0
	vpxord	%zmm3, %zmm5, %zmm3
	vmovdqa64	%zmm1, 128(%r12)
	vpord	%zmm3, %zmm0, %zmm0
	vpxord	%zmm1, %zmm5, %zmm1
	vmovdqa32	%zmm5, %zmm2
	vpord	%zmm1, %zmm0, %zmm0
	vptestnmq	%zmm0, %zmm0, %k0
	kortestb	%k0, %k0
	jc	.L818
	vpbroadcastq	.LC10(%rip), %zmm0
	movl	$2, %esi
	movq	%r12, %rdi
	vmovdqu64	%zmm0, 192(%r12)
	vmovdqu64	%zmm0, 256(%r12)
	vzeroupper
	call	_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	vpbroadcastq	(%r12), %zmm2
	vpbroadcastq	184(%r12), %zmm1
	vpternlogd	$0xFF, %zmm0, %zmm0, %zmm0
	vpaddq	%zmm0, %zmm1, %zmm0
	vpcmpq	$0, %zmm0, %zmm2, %k0
	kortestb	%k0, %k0
	jnc	.L820
	leaq	-112(%rbp), %rdx
	movq	%r14, %rcx
	vmovdqa64	%zmm2, %zmm0
	movq	%r15, %rsi
	movq	%r13, %rdi
	call	_ZN3hwy6N_AVX36detail22MaybePartitionTwoValueINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L972
.L820:
	movq	96(%r12), %rdx
	cmpq	%rdx, 88(%r12)
	jne	.L918
	cmpq	80(%r12), %rdx
	jne	.L861
	cmpq	72(%r12), %rdx
	jne	.L919
	cmpq	64(%r12), %rdx
	jne	.L920
	cmpq	56(%r12), %rdx
	jne	.L921
	cmpq	48(%r12), %rdx
	jne	.L922
	cmpq	40(%r12), %rdx
	jne	.L923
	cmpq	32(%r12), %rdx
	jne	.L924
	cmpq	24(%r12), %rdx
	jne	.L925
	cmpq	16(%r12), %rdx
	jne	.L926
	cmpq	8(%r12), %rdx
	jne	.L927
	movq	(%r12), %rax
	cmpq	%rax, %rdx
	jne	.L979
.L863:
	vpbroadcastq	%rax, %zmm0
.L976:
	movl	$1, -136(%rbp)
.L858:
	cmpq	$0, -120(%rbp)
	je	.L980
	leaq	-8(%r15), %rdx
	leaq	0(%r13,%rdx,8), %r10
	movq	%rdx, %r9
	movq	%rdx, %rcx
	vmovdqu64	(%r10), %zmm6
	andl	$31, %r9d
	andl	$24, %ecx
	je	.L868
	vmovdqu64	0(%r13), %zmm1
	vpcmpq	$6, %zmm0, %zmm1, %k2
	knotb	%k2, %k1
	vpcompressq	%zmm1, 0(%r13){%k1}
	kmovb	%k1, %eax
	kmovb	%k2, %ecx
	popcntq	%rax, %rax
	popcntq	%rcx, %rcx
	leaq	0(%r13,%rax,8), %rax
	vpcompressq	%zmm1, (%r12){%k2}
	testb	$16, %dl
	je	.L869
	vmovdqu64	64(%r13), %zmm1
	vpcmpq	$6, %zmm0, %zmm1, %k1
	knotb	%k1, %k2
	vpcompressq	%zmm1, (%rax){%k2}
	kmovb	%k2, %esi
	popcntq	%rsi, %rsi
	vpcompressq	%zmm1, (%r12,%rcx,8){%k1}
	leaq	(%rax,%rsi,8), %rax
	kmovb	%k1, %esi
	popcntq	%rsi, %rsi
	addq	%rsi, %rcx
	cmpq	$23, %r9
	jbe	.L869
	vmovdqu64	128(%r13), %zmm1
	vpcmpq	$6, %zmm0, %zmm1, %k1
	knotb	%k1, %k2
	vpcompressq	%zmm1, (%rax){%k2}
	kmovb	%k2, %esi
	popcntq	%rsi, %rsi
	vpcompressq	%zmm1, (%r12,%rcx,8){%k1}
	leaq	(%rax,%rsi,8), %rax
	kmovb	%k1, %esi
	popcntq	%rsi, %rsi
	addq	%rsi, %rcx
	leaq	1(%r9), %rsi
	cmpq	$9, %rsi
	leaq	0(,%rcx,8), %r8
	sbbq	%rsi, %rsi
	andq	$-16, %rsi
	addq	$24, %rsi
	cmpq	%rsi, %r9
	jne	.L981
.L871:
	movq	%rax, %r9
	movq	%rdx, %rsi
	movq	%rdx, %rdi
	subq	%r13, %r9
	subq	%rcx, %rsi
	sarq	$3, %r9
	leaq	0(%r13,%rsi,8), %r11
	subq	%r9, %rdi
	subq	%r9, %rsi
	movq	%rdi, -144(%rbp)
	leaq	(%rax,%rdi,8), %rdi
	movq	%rsi, %rdx
	leaq	(%rax,%rsi,8), %r10
	vmovq	%rdi, %xmm17
	.p2align 4,,10
	.p2align 3
.L873:
	cmpl	$8, %r8d
	jnb	.L875
	testl	%r8d, %r8d
	jne	.L982
.L876:
	cmpl	$8, %r8d
	jnb	.L879
	testl	%r8d, %r8d
	jne	.L983
.L880:
	testq	%rdx, %rdx
	je	.L930
.L894:
	leaq	256(%rax), %rsi
	leaq	-256(%r10), %rdi
	vmovdqu64	(%rax), %zmm15
	vmovdqu64	64(%rax), %zmm14
	vmovdqu64	128(%rax), %zmm13
	vmovdqu64	192(%rax), %zmm12
	vmovdqu64	-128(%r10), %zmm9
	vmovdqu64	-64(%r10), %zmm8
	vmovdqu64	-256(%r10), %zmm11
	vmovdqu64	-192(%r10), %zmm10
	cmpq	%rdi, %rsi
	je	.L931
	xorl	%ecx, %ecx
	leaq	_ZZN3hwy6N_AVX3L11CompressNotIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array(%rip), %r14
	vmovdqa64	.LC9(%rip), %zmm5
	movl	$8, %r8d
	jmp	.L887
	.p2align 4,,10
	.p2align 3
.L985:
	vmovdqu64	-128(%rdi), %zmm2
	vmovdqu64	-64(%rdi), %zmm1
	prefetcht0	-1024(%rdi)
	subq	$256, %rdi
	vmovdqu64	(%rdi), %zmm4
	vmovdqu64	64(%rdi), %zmm3
.L886:
	vpcmpq	$6, %zmm0, %zmm4, %k5
	vpcmpq	$6, %zmm0, %zmm3, %k6
	vpcmpq	$6, %zmm0, %zmm2, %k7
	vpcmpq	$6, %zmm0, %zmm1, %k4
	kmovb	%k5, %r11d
	vpbroadcastq	(%r14,%r11,8), %zmm7
	movq	%r11, %r10
	leaq	-8(%rdx,%rcx), %r11
	popcntq	%r10, %r10
	vpsrlvq	%zmm5, %zmm7, %zmm7
	vpermq	%zmm4, %zmm7, %zmm4
	vmovdqu64	%zmm4, (%rax,%rcx,8)
	addq	$8, %rcx
	vmovdqu64	%zmm4, (%rax,%r11,8)
	kmovb	%k6, %r11d
	subq	%r10, %rcx
	vpbroadcastq	(%r14,%r11,8), %zmm4
	movq	%r11, %r10
	leaq	-16(%rdx,%rcx), %r11
	vpsrlvq	%zmm5, %zmm4, %zmm4
	popcntq	%r10, %r10
	vpermq	%zmm3, %zmm4, %zmm3
	vmovdqu64	%zmm3, (%rax,%rcx,8)
	vmovdqu64	%zmm3, (%rax,%r11,8)
	movq	%r8, %r11
	subq	%r10, %r11
	leaq	(%r11,%rcx), %r10
	kmovb	%k7, %r11d
	vpbroadcastq	(%r14,%r11,8), %zmm3
	movq	%r11, %rcx
	leaq	-24(%rdx,%r10), %r11
	popcntq	%rcx, %rcx
	subq	$32, %rdx
	vpsrlvq	%zmm5, %zmm3, %zmm3
	vpermq	%zmm2, %zmm3, %zmm2
	vmovdqu64	%zmm2, (%rax,%r10,8)
	vmovdqu64	%zmm2, (%rax,%r11,8)
	movq	%r8, %r11
	subq	%rcx, %r11
	kmovb	%k4, %ecx
	vpbroadcastq	(%r14,%rcx,8), %zmm2
	addq	%r10, %r11
	movq	%rcx, %r10
	vpsrlvq	%zmm5, %zmm2, %zmm2
	leaq	(%r11,%rdx), %rcx
	popcntq	%r10, %r10
	vpermq	%zmm1, %zmm2, %zmm1
	vmovdqu64	%zmm1, (%rax,%r11,8)
	vmovdqu64	%zmm1, (%rax,%rcx,8)
	movq	%r8, %rcx
	subq	%r10, %rcx
	addq	%r11, %rcx
	cmpq	%rdi, %rsi
	je	.L984
.L887:
	movq	%rsi, %r10
	subq	%rax, %r10
	sarq	$3, %r10
	subq	%rcx, %r10
	cmpq	$32, %r10
	ja	.L985
	vmovdqu64	(%rsi), %zmm4
	vmovdqu64	64(%rsi), %zmm3
	prefetcht0	1024(%rsi)
	addq	$256, %rsi
	vmovdqu64	-128(%rsi), %zmm2
	vmovdqu64	-64(%rsi), %zmm1
	jmp	.L886
	.p2align 4,,10
	.p2align 3
.L978:
	movl	$8, %eax
	subq	%rdi, %rax
	leaq	0(%r13,%rax,8), %r11
	leaq	-8(%rdi,%rdx), %rax
	movq	%rax, -136(%rbp)
	jmp	.L816
	.p2align 4,,10
	.p2align 3
.L984:
	leaq	(%rdx,%rcx), %r8
	leaq	(%rax,%rcx,8), %r10
	addq	$8, %rcx
.L884:
	vpcmpq	$6, %zmm0, %zmm15, %k3
	xorl	%esi, %esi
	kmovb	%k3, %edi
	vpbroadcastq	(%r14,%rdi,8), %zmm1
	vpcmpq	$6, %zmm0, %zmm14, %k3
	popcntq	%rdi, %rsi
	subq	%rsi, %rcx
	vpsrlvq	%zmm5, %zmm1, %zmm1
	kmovb	%k3, %edi
	vpcmpq	$6, %zmm0, %zmm13, %k3
	movq	%rdi, %rsi
	vpermq	%zmm15, %zmm1, %zmm15
	vpbroadcastq	(%r14,%rdi,8), %zmm1
	leaq	-16(%rdx,%rcx), %rdi
	popcntq	%rsi, %rsi
	vmovdqu64	%zmm15, (%r10)
	vpsrlvq	%zmm5, %zmm1, %zmm1
	vmovdqu64	%zmm15, -64(%rax,%r8,8)
	vpermq	%zmm14, %zmm1, %zmm14
	vmovdqu64	%zmm14, (%rax,%rcx,8)
	subq	%rsi, %rcx
	vmovdqu64	%zmm14, (%rax,%rdi,8)
	kmovb	%k3, %edi
	addq	$8, %rcx
	vpbroadcastq	(%r14,%rdi,8), %zmm1
	movq	%rdi, %rsi
	vpcmpq	$6, %zmm0, %zmm12, %k3
	leaq	-24(%rdx,%rcx), %rdi
	vpsrlvq	%zmm5, %zmm1, %zmm1
	vpermq	%zmm13, %zmm1, %zmm13
	vmovdqu64	%zmm13, (%rax,%rcx,8)
	vmovdqu64	%zmm13, (%rax,%rdi,8)
	xorl	%edi, %edi
	popcntq	%rsi, %rdi
	movl	$8, %esi
	movq	%rsi, %r8
	subq	%rdi, %r8
	kmovb	%k3, %edi
	vpbroadcastq	(%r14,%rdi,8), %zmm1
	vpcmpq	$6, %zmm0, %zmm11, %k3
	addq	%rcx, %r8
	movq	%rdi, %rcx
	vpsrlvq	%zmm5, %zmm1, %zmm1
	leaq	-32(%rdx,%r8), %rdi
	popcntq	%rcx, %rcx
	vpermq	%zmm12, %zmm1, %zmm12
	vmovdqu64	%zmm12, (%rax,%r8,8)
	vmovdqu64	%zmm12, (%rax,%rdi,8)
	movq	%rsi, %rdi
	subq	%rcx, %rdi
	addq	%r8, %rdi
	kmovb	%k3, %r8d
	vpbroadcastq	(%r14,%r8,8), %zmm1
	movq	%r8, %rcx
	vpcmpq	$6, %zmm0, %zmm10, %k3
	leaq	-40(%rdx,%rdi), %r8
	popcntq	%rcx, %rcx
	vpsrlvq	%zmm5, %zmm1, %zmm1
	vpermq	%zmm11, %zmm1, %zmm11
	vmovdqu64	%zmm11, (%rax,%rdi,8)
	vmovdqu64	%zmm11, (%rax,%r8,8)
	movq	%rsi, %r8
	subq	%rcx, %r8
	addq	%rdi, %r8
	kmovb	%k3, %edi
	vpbroadcastq	(%r14,%rdi,8), %zmm1
	movq	%rdi, %rcx
	vpcmpq	$6, %zmm0, %zmm9, %k3
	leaq	-48(%rdx,%r8), %rdi
	popcntq	%rcx, %rcx
	vpsrlvq	%zmm5, %zmm1, %zmm1
	vpermq	%zmm10, %zmm1, %zmm10
	vmovdqu64	%zmm10, (%rax,%r8,8)
	vmovdqu64	%zmm10, (%rax,%rdi,8)
	movq	%rsi, %rdi
	subq	%rcx, %rdi
	addq	%r8, %rdi
	kmovb	%k3, %r8d
	vpbroadcastq	(%r14,%r8,8), %zmm1
	movq	%r8, %rcx
	vpcmpq	$6, %zmm0, %zmm8, %k3
	leaq	-56(%rdx,%rdi), %r8
	popcntq	%rcx, %rcx
	vpsrlvq	%zmm5, %zmm1, %zmm1
	vpermq	%zmm9, %zmm1, %zmm9
	vmovdqu64	%zmm9, (%rax,%rdi,8)
	vmovdqu64	%zmm9, (%rax,%r8,8)
	movq	%rsi, %r8
	subq	%rcx, %r8
	xorl	%ecx, %ecx
	addq	%rdi, %r8
	kmovb	%k3, %edi
	vpbroadcastq	(%r14,%rdi,8), %zmm1
	popcntq	%rdi, %rcx
	leaq	-64(%rdx,%r8), %rdx
	subq	%rcx, %rsi
	vpsrlvq	%zmm5, %zmm1, %zmm1
	vpermq	%zmm8, %zmm1, %zmm8
	vmovdqu64	%zmm8, (%rax,%r8,8)
	vmovdqu64	%zmm8, (%rax,%rdx,8)
	leaq	(%rsi,%r8), %rdx
	movq	-144(%rbp), %rsi
	leaq	(%rax,%rdx,8), %rcx
	subq	%rdx, %rsi
.L883:
	movq	%rcx, %rdi
	cmpq	$7, %rsi
	ja	.L888
	movq	-144(%rbp), %rdi
	leaq	-64(%rax,%rdi,8), %rdi
.L888:
	vmovdqu64	(%rdi), %zmm7
	vpcmpq	$6, %zmm0, %zmm6, %k2
	vmovq	%xmm17, %rsi
	vmovdqu64	%zmm7, (%rsi)
	knotb	%k2, %k1
	vpcompressq	%zmm6, (%rcx){%k1}
	kmovb	%k1, %ecx
	popcntq	%rcx, %rcx
	addq	%rcx, %rdx
	vpcompressq	%zmm6, (%rax,%rdx,8){%k2}
	leaq	(%r9,%rdx), %r14
	movq	-120(%rbp), %r9
	subq	$1, %r9
	cmpl	$2, -136(%rbp)
	je	.L986
	movq	-128(%rbp), %rsi
	movq	%rbx, %r8
	movq	%r12, %rcx
	movq	%r14, %rdx
	movq	%r13, %rdi
	movq	%r9, -120(%rbp)
	vzeroupper
	call	_ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	cmpl	$3, -136(%rbp)
	movq	-120(%rbp), %r9
	je	.L972
.L890:
	movq	%r15, %rdx
	movq	-128(%rbp), %rsi
	leaq	0(%r13,%r14,8), %rdi
	movq	%rbx, %r8
	subq	%r14, %rdx
	movq	%r12, %rcx
	call	_ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L972:
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
.L879:
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
	jmp	.L880
	.p2align 4,,10
	.p2align 3
.L875:
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
	jmp	.L876
	.p2align 4,,10
	.p2align 3
.L981:
	subq	%rsi, %r9
	leaq	0(%r13,%rsi,8), %r11
	leaq	(%r12,%r8), %rsi
.L893:
	movq	$-1, %rdi
	bzhi	%r9, %rdi, %rdi
	movzbl	%dil, %edi
	kmovd	%edi, %k0
.L874:
	vmovdqu64	(%r11), %zmm1
	vpcmpq	$6, %zmm0, %zmm1, %k1
	kandnb	%k0, %k1, %k2
	vpcompressq	%zmm1, (%rax){%k2}
	kmovb	%k2, %edi
	kandb	%k0, %k1, %k1
	popcntq	%rdi, %rdi
	leaq	(%rax,%rdi,8), %rax
	kmovb	%k1, %r8d
	popcntq	%r8, %r8
	movq	%rax, %r9
	addq	%rcx, %r8
	movq	%rdx, %rcx
	movq	%rdx, %rdi
	subq	%r13, %r9
	subq	%r8, %rcx
	vpcompressq	%zmm1, (%rsi){%k1}
	salq	$3, %r8
	sarq	$3, %r9
	movq	%rcx, %rdx
	leaq	0(%r13,%rcx,8), %r11
	subq	%r9, %rdi
	subq	%r9, %rdx
	movq	%rdi, -144(%rbp)
	leaq	(%rax,%rdi,8), %rdi
	leaq	(%rax,%rdx,8), %r10
	vmovq	%rdi, %xmm17
	jmp	.L873
	.p2align 4,,10
	.p2align 3
.L983:
	movzbl	(%r12), %ecx
	movb	%cl, (%r11)
	jmp	.L880
	.p2align 4,,10
	.p2align 3
.L982:
	movzbl	(%r11), %ecx
	movb	%cl, (%rax)
	jmp	.L876
	.p2align 4,,10
	.p2align 3
.L869:
	leaq	-8(%r9), %rsi
	leaq	1(%r9), %rdi
	andq	$-8, %rsi
	leaq	0(,%rcx,8), %r8
	addq	$8, %rsi
	cmpq	$8, %rdi
	movl	$8, %edi
	cmovbe	%rdi, %rsi
	cmpq	%rsi, %r9
	je	.L871
	subq	%rsi, %r9
	leaq	0(%r13,%rsi,8), %r11
	leaq	(%r12,%r8), %rsi
	cmpq	$255, %r9
	jbe	.L893
	movl	$255, %edi
	kmovd	%edi, %k0
	jmp	.L874
	.p2align 4,,10
	.p2align 3
.L977:
	cmpq	$1, %rdx
	jbe	.L972
	leaq	1024(%rdi), %rax
	cmpq	%rax, %rsi
	jb	.L987
	movl	$8, %esi
	call	_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	jmp	.L972
	.p2align 4,,10
	.p2align 3
.L818:
	vmovdqu64	0(%r13), %zmm6
	movl	$8, %esi
	movq	$-1, %rax
	subq	-144(%rbp), %rsi
	bzhi	%rsi, %rax, %rax
	kmovb	%eax, %k2
	vpcmpq	$4, %zmm5, %zmm6, %k0
	kandb	%k2, %k0, %k0
	kmovb	%k0, %eax
	kortestb	%k0, %k0
	jne	.L988
	vpxor	%xmm4, %xmm4, %xmm4
	leaq	1024(%r13,%rsi,8), %rdi
	vmovdqa64	%zmm4, %zmm3
	.p2align 4,,10
	.p2align 3
.L824:
	movq	%rsi, %rcx
	subq	$-128, %rsi
	cmpq	%rsi, %r15
	jb	.L828
	leaq	-1024(%rdi), %rax
.L823:
	vpxord	(%rax), %zmm2, %zmm0
	vpxord	64(%rax), %zmm2, %zmm1
	leaq	128(%rax), %rdx
	vpord	%zmm3, %zmm0, %zmm3
	vpord	%zmm4, %zmm1, %zmm4
	vpxord	128(%rax), %zmm2, %zmm0
	vpxord	192(%rax), %zmm2, %zmm1
	vpord	%zmm3, %zmm0, %zmm3
	vpxord	256(%rax), %zmm2, %zmm0
	vpord	%zmm4, %zmm1, %zmm4
	vpxord	320(%rax), %zmm2, %zmm1
	leaq	384(%rdx), %rax
	vpord	%zmm3, %zmm0, %zmm3
	vpxord	256(%rdx), %zmm2, %zmm0
	vpord	%zmm4, %zmm1, %zmm4
	vpxord	320(%rdx), %zmm2, %zmm1
	vpord	%zmm3, %zmm0, %zmm0
	vpord	%zmm4, %zmm1, %zmm1
	vmovdqa64	%zmm0, %zmm3
	vmovdqa64	%zmm1, %zmm4
	cmpq	%rax, %rdi
	jne	.L823
	vpord	%zmm1, %zmm0, %zmm0
	leaq	1408(%rdx), %rdi
	vptestnmq	%zmm0, %zmm0, %k0
	kortestb	%k0, %k0
	setc	%al
	testb	%al, %al
	jne	.L824
	vmovdqa64	0(%r13,%rcx,8), %zmm6
	vpcmpq	$4, %zmm5, %zmm6, %k0
	kortestb	%k0, %k0
	jne	.L826
	.p2align 4,,10
	.p2align 3
.L825:
	addq	$8, %rcx
	vmovdqa64	0(%r13,%rcx,8), %zmm6
	vpcmpq	$4, %zmm5, %zmm6, %k0
	kortestb	%k0, %k0
	je	.L825
.L826:
	kmovb	%k0, %eax
	tzcntl	%eax, %eax
	addq	%rcx, %rax
.L822:
	vpbroadcastq	0(%r13,%rax,8), %zmm1
	leaq	0(%r13,%rax,8), %rdi
	vpcmpq	$6, %zmm5, %zmm1, %k0
	kortestb	%k0, %k0
	jne	.L831
	leaq	-8(%r15), %rax
	xorl	%ecx, %ecx
	jmp	.L837
	.p2align 4,,10
	.p2align 3
.L832:
	kmovb	%k0, %edx
	popcntq	%rdx, %rdx
	addq	%rdx, %rcx
	leaq	-8(%rax), %rdx
	vmovdqu64	%zmm5, 0(%r13,%rax,8)
	cmpq	%rdx, %r15
	jbe	.L989
	movq	%rdx, %rax
.L837:
	vmovdqu64	0(%r13,%rax,8), %zmm6
	vpcmpq	$0, %zmm1, %zmm6, %k1
	vpcmpq	$0, %zmm5, %zmm6, %k0
	kmovb	%k1, %edx
	kmovb	%k0, %esi
	korb	%k0, %k1, %k1
	kortestb	%k1, %k1
	jc	.L832
	kmovb	%edx, %k2
	kmovb	%esi, %k3
	kxnorb	%k3, %k2, %k2
	kmovb	%k2, %edx
	tzcntl	%edx, %edx
	leaq	8(%rax), %rsi
	addq	%rax, %rdx
	addq	$16, %rax
	vpbroadcastq	0(%r13,%rdx,8), %zmm2
	movq	%r15, %rdx
	subq	%rcx, %rdx
	vmovdqa64	%zmm2, -112(%rbp)
	cmpq	%rdx, %rax
	ja	.L833
	.p2align 4,,10
	.p2align 3
.L834:
	vmovdqu64	%zmm1, -64(%r13,%rax,8)
	movq	%rax, %rsi
	addq	$8, %rax
	cmpq	%rax, %rdx
	jnb	.L834
.L833:
	subq	%rsi, %rdx
	leaq	0(%r13,%rsi,8), %rcx
	movl	$255, %eax
	cmpq	$255, %rdx
	ja	.L835
	movq	$-1, %rax
	bzhi	%rdx, %rax, %rax
	movzbl	%al, %eax
.L835:
	kmovb	%eax, %k3
	vmovdqu64	%zmm1, (%rcx){%k3}
.L836:
	vpbroadcastq	(%r12), %zmm0
	vpcmpq	$0, .LC8(%rip), %zmm0, %k0
	kortestb	%k0, %k0
	jc	.L916
	vpcmpq	$0, .LC7(%rip), %zmm0, %k0
	kortestb	%k0, %k0
	jc	.L854
	vpminsq	%zmm2, %zmm1, %zmm3
	vpcmpq	$6, %zmm3, %zmm0, %k0
	kortestb	%k0, %k0
	jne	.L990
	vmovdqa64	%zmm0, %zmm1
	movl	$128, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L849:
	leaq	(%rcx,%rax,8), %rdx
	addq	$1, %rax
	vpminsq	0(%r13,%rdx,8), %zmm1, %zmm1
	cmpq	$16, %rax
	jne	.L849
	vpcmpq	$6, %zmm1, %zmm0, %k0
	kortestb	%k0, %k0
	jne	.L976
	leaq	128(%rsi), %rax
	cmpq	%rax, %r15
	jb	.L856
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L849
	.p2align 4,,10
	.p2align 3
.L828:
	movq	%rcx, %rdx
	addq	$8, %rcx
	cmpq	%rcx, %r15
	jb	.L991
	vmovdqa64	-64(%r13,%rcx,8), %zmm6
	vpcmpq	$4, %zmm5, %zmm6, %k0
	kmovb	%k0, %eax
	kortestb	%k0, %k0
	je	.L828
.L974:
	tzcntl	%eax, %eax
	addq	%rdx, %rax
	jmp	.L822
	.p2align 4,,10
	.p2align 3
.L868:
	movq	%r12, %rsi
	movq	%r13, %r11
	movq	%r13, %rax
	testq	%r9, %r9
	jne	.L893
	movq	%rdx, -144(%rbp)
	vmovq	%r10, %xmm17
	jmp	.L894
	.p2align 4,,10
	.p2align 3
.L986:
	vzeroupper
	jmp	.L890
	.p2align 4,,10
	.p2align 3
.L930:
	movq	-144(%rbp), %rsi
	movq	%rax, %rcx
	jmp	.L883
	.p2align 4,,10
	.p2align 3
.L931:
	vmovdqa64	.LC9(%rip), %zmm5
	movq	%rax, %r10
	movq	%rdx, %r8
	movl	$8, %ecx
	leaq	_ZZN3hwy6N_AVX3L11CompressNotIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array(%rip), %r14
	jmp	.L884
.L980:
	leaq	-1(%r15), %rbx
	movq	%rbx, %r12
	shrq	%r12
	.p2align 4,,10
	.p2align 3
.L866:
	movq	%r12, %rdx
	movq	%r15, %rsi
	movq	%r13, %rdi
	call	_ZN3hwy6N_AVX36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	subq	$1, %r12
	jnb	.L866
	.p2align 4,,10
	.p2align 3
.L867:
	movq	0(%r13,%rbx,8), %rdx
	movq	0(%r13), %rax
	movq	%rbx, %rsi
	movq	%r13, %rdi
	movq	%rdx, 0(%r13)
	xorl	%edx, %edx
	movq	%rax, 0(%r13,%rbx,8)
	call	_ZN3hwy6N_AVX36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	subq	$1, %rbx
	jne	.L867
	jmp	.L972
	.p2align 4,,10
	.p2align 3
.L857:
	vpcmpq	$6, -64(%r13,%rsi,8), %zmm0, %k0
	kortestb	%k0, %k0
	jne	.L976
.L856:
	movq	%rsi, %rax
	addq	$8, %rsi
	cmpq	%rsi, %r15
	jnb	.L857
	cmpq	%rax, %r15
	je	.L916
	vpcmpq	$6, -64(%r13,%r15,8), %zmm0, %k0
	xorl	%eax, %eax
	kortestb	%k0, %k0
	sete	%al
	addl	$1, %eax
	movl	%eax, -136(%rbp)
	jmp	.L858
.L918:
	movl	$12, %eax
	movl	$11, %esi
	jmp	.L864
	.p2align 4,,10
	.p2align 3
.L865:
	cmpq	$23, %rax
	je	.L975
.L864:
	movq	%rax, %rcx
	addq	$1, %rax
	cmpq	(%r12,%rax,8), %rdx
	je	.L865
	movl	$12, %edi
	subq	$11, %rcx
	movq	%rdx, %rax
	subq	%rsi, %rdi
	cmpq	%rdi, %rcx
	jb	.L863
.L975:
	movq	(%r12,%rsi,8), %rax
	jmp	.L863
.L919:
	movl	$9, %esi
	movl	$10, %eax
	jmp	.L864
.L920:
	movl	$8, %esi
	movl	$9, %eax
	jmp	.L864
.L921:
	movl	$7, %esi
	movl	$8, %eax
	jmp	.L864
.L922:
	movl	$6, %esi
	movl	$7, %eax
	jmp	.L864
.L923:
	movl	$5, %esi
	movl	$6, %eax
	jmp	.L864
.L831:
	movq	%r15, %rsi
	leaq	-112(%rbp), %rdx
	vmovdqa64	%zmm5, %zmm0
	movq	%r12, %rcx
	subq	%rax, %rsi
	call	_ZN3hwy6N_AVX36detail22MaybePartitionTwoValueINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L972
	vmovdqa64	-112(%rbp), %zmm2
	jmp	.L836
.L924:
	movl	$4, %esi
	movl	$5, %eax
	jmp	.L864
.L925:
	movl	$3, %esi
	movl	$4, %eax
	jmp	.L864
.L926:
	movl	$2, %esi
	movl	$3, %eax
	jmp	.L864
.L927:
	movl	$1, %esi
	movl	$2, %eax
	jmp	.L864
.L979:
	xorl	%esi, %esi
	movl	$1, %eax
	jmp	.L864
.L989:
	movl	$255, %edi
	vmovdqu64	0(%r13), %zmm0
	kmovd	%edi, %k2
	cmpq	$255, %rax
	ja	.L838
	movq	$-1, %rdx
	bzhi	%rax, %rdx, %rdx
	movzbl	%dl, %edi
	kmovd	%edi, %k2
.L838:
	vpcmpq	$0, %zmm1, %zmm0, %k0
	vpcmpq	$0, %zmm5, %zmm0, %k1
	movq	%r15, %rsi
	kandb	%k2, %k1, %k1
	knotb	%k2, %k2
	korb	%k1, %k0, %k0
	korb	%k2, %k0, %k0
	kortestb	%k0, %k0
	setc	%dl
	subq	%rcx, %rsi
	testb	%dl, %dl
	je	.L992
	kmovb	%k1, %eax
	popcntq	%rax, %rax
	subq	%rax, %rsi
	vmovdqu64	%zmm5, 0(%r13)
	movq	%rsi, %rdx
	cmpq	$7, %rsi
	jbe	.L843
	leaq	-8(%rsi), %rcx
	movq	-152(%rbp), %rsi
	movq	%rcx, %rax
	shrq	$3, %rax
	salq	$6, %rax
	leaq	64(%r13,%rax), %rax
	.p2align 4,,10
	.p2align 3
.L844:
	vmovdqu64	%zmm1, (%rsi)
	addq	$64, %rsi
	cmpq	%rax, %rsi
	jne	.L844
	andq	$-8, %rcx
	vmovdqa64	%zmm1, (%r12)
	leaq	8(%rcx), %rax
	leaq	0(%r13,%rax,8), %r13
	subq	%rax, %rdx
	movl	$255, %eax
	kmovd	%eax, %k1
	cmpq	$255, %rdx
	jbe	.L895
.L845:
	vmovdqu64	(%r12), %zmm0{%k1}{z}
	vmovdqu64	%zmm0, 0(%r13){%k1}
	vzeroupper
	jmp	.L972
.L861:
	movl	$11, %eax
	movl	$10, %esi
	jmp	.L864
.L991:
	leaq	-8(%r15), %rdx
	vmovdqu64	0(%r13,%rdx,8), %zmm6
	vpcmpq	$4, %zmm5, %zmm6, %k0
	kmovb	%k0, %eax
	kortestb	%k0, %k0
	jne	.L974
	vzeroupper
	jmp	.L972
.L853:
	vmovdqu64	-64(%r13,%rsi,8), %zmm6
	vpcmpq	$6, %zmm0, %zmm6, %k0
	kortestb	%k0, %k0
	jne	.L976
.L852:
	movq	%rsi, %rax
	addq	$8, %rsi
	cmpq	%rsi, %r15
	jnb	.L853
	cmpq	%rax, %r15
	je	.L854
	vmovdqu64	-64(%r13,%r15,8), %zmm6
	vpcmpq	$6, %zmm0, %zmm6, %k0
	kortestb	%k0, %k0
	jne	.L976
.L854:
	movl	$3, -136(%rbp)
	vpternlogd	$0xFF, %zmm1, %zmm1, %zmm1
	vpaddq	%zmm1, %zmm0, %zmm0
	jmp	.L858
.L988:
	tzcntl	%eax, %eax
	jmp	.L822
.L987:
	movq	%rdi, %rcx
	movq	%r12, %rdi
	cmpq	$7, %rdx
	jbe	.L809
	leaq	-8(%rdx), %rdx
	movq	0(%r13), %rcx
	leaq	8(%r12), %rdi
	movq	%rdx, %rax
	andq	$-8, %rdi
	shrq	$3, %rax
	movq	%rcx, (%r12)
	addq	$1, %rax
	salq	$6, %rax
	movl	%eax, %ecx
	movq	-8(%r13,%rcx), %rsi
	movq	%rsi, -8(%r12,%rcx)
	movq	%r12, %rcx
	movq	%r13, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%eax, %ecx
	movq	%rdx, %rax
	movq	%r15, %rdx
	andq	$-8, %rax
	shrl	$3, %ecx
	rep movsq
	addq	$8, %rax
	leaq	0(,%rax,8), %rcx
	subq	%rax, %rdx
	movl	$255, %eax
	leaq	(%r12,%rcx), %rdi
	addq	%r13, %rcx
	cmpq	$255, %rdx
	jbe	.L809
.L810:
	leal	-1(%r15), %edx
	movl	$32, %r8d
	movl	$1, %esi
	kmovb	%eax, %k4
	bsrl	%edx, %edx
	vmovdqu64	(%rcx), %zmm0{%k4}{z}
	movq	%r15, %rax
	xorl	$31, %edx
	subl	%edx, %r8d
	movl	$1, %edx
	vmovdqu64	%zmm0, (%rdi){%k4}
	vpbroadcastq	.LC10(%rip), %zmm0
	shlx	%r8, %rsi, %rsi
	shrq	$4, %rsi
	cmove	%rdx, %rsi
	movq	%rsi, %rdx
	salq	$4, %rdx
	addq	$8, %rdx
	cmpq	%rdx, %r15
	jnb	.L814
	.p2align 4,,10
	.p2align 3
.L811:
	vmovdqu64	%zmm0, (%r12,%rax,8)
	addq	$8, %rax
	cmpq	%rdx, %rax
	jb	.L811
.L814:
	movq	%r12, %rdi
	vzeroupper
	call	_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	cmpq	$7, %r15
	jbe	.L813
	leaq	-8(%r15), %rdx
	movq	(%r12), %rcx
	leaq	8(%r13), %rdi
	movq	%rdx, %rax
	andq	$-8, %rdi
	shrq	$3, %rax
	movq	%rcx, 0(%r13)
	addq	$1, %rax
	salq	$6, %rax
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
	leaq	0(,%rax,8), %rdx
	subq	%rax, %r15
	movl	$255, %eax
	addq	%rdx, %r13
	addq	%rdx, %r12
	cmpq	$255, %r15
	jbe	.L813
.L815:
	kmovb	%eax, %k6
	vmovdqu64	(%r12), %zmm0{%k6}{z}
	vmovdqu64	%zmm0, 0(%r13){%k6}
	vzeroupper
	jmp	.L972
.L843:
	vmovdqa64	%zmm1, (%r12)
.L895:
	movq	$-1, %rax
	bzhi	%rdx, %rax, %rax
	movzbl	%al, %eax
	kmovd	%eax, %k1
	jmp	.L845
.L916:
	movl	$2, -136(%rbp)
	jmp	.L858
.L990:
	vpmaxsq	%zmm2, %zmm1, %zmm1
	vpcmpq	$6, %zmm0, %zmm1, %k0
	kortestb	%k0, %k0
	jne	.L976
	vmovdqa64	%zmm0, %zmm1
	movl	$128, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L850:
	leaq	(%rcx,%rax,8), %rdx
	addq	$1, %rax
	vpmaxsq	0(%r13,%rdx,8), %zmm1, %zmm1
	cmpq	$16, %rax
	jne	.L850
	vpcmpq	$6, %zmm0, %zmm1, %k0
	kortestb	%k0, %k0
	jne	.L976
	leaq	128(%rsi), %rax
	cmpq	%rax, %r15
	jb	.L852
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L850
.L813:
	movq	$-1, %rax
	bzhi	%r15, %rax, %rax
	movzbl	%al, %eax
	jmp	.L815
.L809:
	movq	$-1, %rax
	bzhi	%rdx, %rax, %rax
	movzbl	%al, %eax
	jmp	.L810
.L992:
	knotb	%k0, %k2
	kmovb	%k2, %edx
	tzcntl	%edx, %edx
	vpbroadcastq	0(%r13,%rdx,8), %zmm2
	leaq	8(%rax), %rdx
	vmovdqa64	%zmm2, -112(%rbp)
	cmpq	%rsi, %rdx
	ja	.L840
.L841:
	vmovdqu64	%zmm1, -64(%r13,%rdx,8)
	movq	%rdx, %rax
	addq	$8, %rdx
	cmpq	%rsi, %rdx
	jbe	.L841
.L840:
	subq	%rax, %rsi
	leaq	0(%r13,%rax,8), %rcx
	movl	$-1, %eax
	cmpq	$255, %rsi
	ja	.L842
	orq	$-1, %rax
	bzhi	%rsi, %rax, %rax
.L842:
	kmovb	%eax, %k4
	vmovdqu64	%zmm1, (%rcx){%k4}
	jmp	.L836
	.cfi_endproc
.LFE18801:
	.size	_ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0, .-_ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	.section	.text._ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0, @function
_ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0:
.LFB18803:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %r10
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	movq	%rcx, %r12
	pushq	%rbx
	subq	$104, %rsp
	.cfi_offset 3, -56
	movq	%rsi, -88(%rbp)
	movq	%rdx, -72(%rbp)
	movq	%r9, -80(%rbp)
	cmpq	$32, %rdx
	jbe	.L1219
	movq	%rdi, %rax
	movq	%rdi, -128(%rbp)
	movq	%r8, %rbx
	shrq	$3, %rax
	movq	%rax, %rdx
	movq	%rax, -112(%rbp)
	andl	$7, %edx
	jne	.L1220
	movq	-72(%rbp), %r14
	movq	%rdi, %rax
.L1005:
	movq	8(%rbx), %rdx
	movq	16(%rbx), %r11
	movq	%rdx, %rcx
	leaq	(%rdx,%rdx,8), %rsi
	leaq	1(%r11), %r8
	movq	%rdx, %rdi
	rolq	$24, %rcx
	xorq	(%rbx), %r8
	shrq	$11, %rdi
	leaq	2(%r11), %rdx
	addq	%r8, %rcx
	xorq	%rsi, %rdi
	xorq	%rdx, %rdi
	movq	%rcx, %rdx
	leaq	(%rcx,%rcx,8), %rsi
	shrq	$11, %rcx
	rolq	$24, %rdx
	xorq	%rsi, %rcx
	leaq	3(%r11), %rsi
	addq	%rdi, %rdx
	xorq	%rsi, %rcx
	movq	%rdx, %rsi
	leaq	(%rdx,%rdx,8), %r9
	shrq	$11, %rdx
	rolq	$24, %rsi
	xorq	%r9, %rdx
	leaq	4(%r11), %r9
	addq	$5, %r11
	addq	%rcx, %rsi
	xorq	%r9, %rdx
	movq	%r11, 16(%rbx)
	movq	%rsi, %r9
	leaq	(%rsi,%rsi,8), %r13
	shrq	$11, %rsi
	rolq	$24, %r9
	xorq	%r13, %rsi
	addq	%rdx, %r9
	xorq	%r11, %rsi
	movabsq	$34359738359, %r11
	movq	%r9, %r15
	leaq	(%r9,%r9,8), %r13
	rolq	$24, %r9
	shrq	$11, %r15
	addq	%rsi, %r9
	movl	%esi, %esi
	xorq	%r15, %r13
	movl	%edx, %r15d
	movq	%r13, %xmm0
	movl	%r8d, %r13d
	pinsrq	$1, %r9, %xmm0
	movq	%r14, %r9
	shrq	$3, %r9
	cmpq	%r11, %r14
	movl	$4294967295, %r11d
	movl	%ecx, %r14d
	cmova	%r11, %r9
	shrq	$32, %r8
	movl	%edi, %r11d
	movups	%xmm0, (%rbx)
	shrq	$32, %rdi
	imulq	%r9, %r13
	shrq	$32, %rcx
	shrq	$32, %rdx
	imulq	%r9, %r8
	imulq	%r9, %r11
	shrq	$32, %r13
	imulq	%r9, %rdi
	imulq	%r9, %r14
	shrq	$32, %r8
	imulq	%r9, %rcx
	shrq	$32, %r11
	imulq	%r9, %r15
	shrq	$32, %rdi
	imulq	%r9, %rdx
	shrq	$32, %r14
	imulq	%r9, %rsi
	movq	%r13, %r9
	shrq	$32, %rcx
	leaq	2(,%r13,8), %r13
	salq	$6, %r9
	shrq	$32, %r15
	movdqa	(%rax,%r9), %xmm4
	movq	%r8, %r9
	shrq	$32, %rdx
	salq	$6, %r9
	shrq	$32, %rsi
	movdqa	(%rax,%r9), %xmm3
	movq	%r11, %r9
	salq	$6, %r9
	movdqa	(%rax,%r9), %xmm2
	movq	%rdi, %r9
	salq	$6, %r9
	movdqa	%xmm2, %xmm0
	movdqa	%xmm2, %xmm1
	movdqa	(%rax,%r9), %xmm11
	movq	%r14, %r9
	pcmpgtq	%xmm4, %xmm0
	salq	$6, %r9
	leaq	2(,%r14,8), %r14
	pblendvb	%xmm0, %xmm4, %xmm1
	pblendvb	%xmm0, %xmm2, %xmm4
	movdqa	%xmm3, %xmm0
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm3, %xmm1
	movdqa	(%rax,%r9), %xmm3
	movdqa	%xmm4, %xmm0
	movq	%rcx, %r9
	pcmpgtq	%xmm1, %xmm0
	salq	$6, %r9
	movdqa	(%rax,%r9), %xmm2
	movq	%r15, %r9
	leaq	2(,%r15,8), %r15
	salq	$6, %r9
	movdqa	(%rax,%r15,8), %xmm6
	pblendvb	%xmm0, %xmm1, %xmm4
	movdqa	%xmm2, %xmm0
	movdqa	%xmm2, %xmm1
	movdqa	(%rax,%r9), %xmm7
	pcmpgtq	%xmm11, %xmm0
	movq	%rdx, %r9
	movaps	%xmm4, (%r12)
	leaq	2(,%rdx,8), %rdx
	salq	$6, %r9
	pblendvb	%xmm0, %xmm11, %xmm1
	pblendvb	%xmm0, %xmm2, %xmm11
	movdqa	%xmm3, %xmm0
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm3, %xmm1
	movdqa	(%rax,%r9), %xmm3
	movdqa	%xmm11, %xmm0
	movq	%rsi, %r9
	pcmpgtq	%xmm1, %xmm0
	salq	$6, %r9
	leaq	2(,%rsi,8), %rsi
	movdqa	(%rax,%r9), %xmm2
	leaq	0(,%r13,8), %r9
	movdqa	16(%rax,%r9), %xmm13
	pblendvb	%xmm0, %xmm1, %xmm11
	movdqa	%xmm2, %xmm0
	movdqa	%xmm2, %xmm1
	pcmpgtq	%xmm7, %xmm0
	movaps	%xmm11, 64(%r12)
	pblendvb	%xmm0, %xmm7, %xmm1
	pblendvb	%xmm0, %xmm2, %xmm7
	movdqa	%xmm3, %xmm0
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm3, %xmm1
	movdqa	(%rax,%r13,8), %xmm3
	movdqa	%xmm7, %xmm0
	leaq	2(,%r8,8), %r13
	movdqa	(%rax,%r13,8), %xmm5
	leaq	0(,%r13,8), %r8
	pcmpgtq	%xmm1, %xmm0
	leaq	2(,%r11,8), %r13
	movdqa	(%rax,%r13,8), %xmm2
	leaq	0(,%r13,8), %r11
	leaq	2(,%rdi,8), %r13
	movdqa	(%rax,%r13,8), %xmm10
	leaq	0(,%r13,8), %rdi
	leaq	0(,%r14,8), %r13
	pblendvb	%xmm0, %xmm1, %xmm7
	movdqa	%xmm2, %xmm0
	movdqa	%xmm2, %xmm1
	movaps	%xmm7, 128(%r12)
	pcmpgtq	%xmm3, %xmm0
	pblendvb	%xmm0, %xmm3, %xmm1
	pblendvb	%xmm0, %xmm2, %xmm3
	movdqa	%xmm5, %xmm0
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm5, %xmm1
	movdqa	%xmm3, %xmm0
	movdqa	(%rax,%r14,8), %xmm5
	leaq	2(,%rcx,8), %r14
	pcmpgtq	%xmm1, %xmm0
	movdqa	(%rax,%r14,8), %xmm2
	leaq	0(,%r14,8), %rcx
	leaq	0(,%r15,8), %r14
	leaq	0(,%rdx,8), %r15
	pblendvb	%xmm0, %xmm1, %xmm3
	movdqa	%xmm2, %xmm0
	movdqa	%xmm2, %xmm1
	pcmpgtq	%xmm10, %xmm0
	movaps	%xmm3, 16(%r12)
	pblendvb	%xmm0, %xmm10, %xmm1
	pblendvb	%xmm0, %xmm2, %xmm10
	movdqa	%xmm5, %xmm0
	movdqa	(%rax,%rsi,8), %xmm2
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm5, %xmm1
	movdqa	%xmm10, %xmm0
	movdqa	(%rax,%rdx,8), %xmm5
	leaq	0(,%rsi,8), %rdx
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm10
	movdqa	%xmm2, %xmm0
	movdqa	%xmm2, %xmm1
	pcmpgtq	%xmm6, %xmm0
	movaps	%xmm10, 80(%r12)
	pblendvb	%xmm0, %xmm6, %xmm1
	pblendvb	%xmm0, %xmm2, %xmm6
	movdqa	%xmm5, %xmm0
	movdqa	16(%rax,%r11), %xmm2
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm5, %xmm1
	movdqa	%xmm6, %xmm0
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm6
	movdqa	%xmm2, %xmm0
	movdqa	%xmm2, %xmm1
	movaps	%xmm6, 144(%r12)
	pcmpgtq	%xmm13, %xmm0
	pblendvb	%xmm0, %xmm13, %xmm1
	pblendvb	%xmm0, %xmm2, %xmm13
	movdqa	16(%rax,%r8), %xmm0
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, 16(%rax,%r8), %xmm1
	movdqa	%xmm13, %xmm0
	movdqa	16(%rax,%rcx), %xmm2
	movdqa	16(%rax,%rdi), %xmm9
	pcmpgtq	%xmm1, %xmm0
	movdqa	16(%rax,%r14), %xmm5
	movdqa	32(%rax,%r9), %xmm12
	movdqa	32(%rax,%r8), %xmm8
	movdqa	32(%rax,%rcx), %xmm14
	movdqa	32(%rax,%r15), %xmm15
	pblendvb	%xmm0, %xmm1, %xmm13
	movdqa	%xmm2, %xmm0
	movdqa	%xmm2, %xmm1
	pcmpgtq	%xmm9, %xmm0
	movaps	%xmm13, 32(%r12)
	pblendvb	%xmm0, %xmm9, %xmm1
	pblendvb	%xmm0, %xmm2, %xmm9
	movdqa	16(%rax,%r13), %xmm0
	movdqa	16(%rax,%rdx), %xmm2
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, 16(%rax,%r13), %xmm1
	movdqa	%xmm9, %xmm0
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm9
	movdqa	%xmm2, %xmm0
	movdqa	%xmm2, %xmm1
	pcmpgtq	%xmm5, %xmm0
	movaps	%xmm9, 96(%r12)
	pblendvb	%xmm0, %xmm5, %xmm1
	pblendvb	%xmm0, %xmm2, %xmm5
	movdqa	16(%rax,%r15), %xmm0
	movdqa	32(%rax,%r11), %xmm2
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, 16(%rax,%r15), %xmm1
	movdqa	%xmm5, %xmm0
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm5
	movdqa	%xmm2, %xmm0
	movdqa	%xmm2, %xmm1
	movaps	%xmm5, 160(%r12)
	pcmpgtq	%xmm12, %xmm0
	pblendvb	%xmm0, %xmm12, %xmm1
	pblendvb	%xmm0, %xmm2, %xmm12
	movdqa	%xmm8, %xmm0
	movdqa	32(%rax,%r13), %xmm2
	pcmpgtq	%xmm1, %xmm0
	leaq	192(%r12), %r13
	pblendvb	%xmm0, %xmm8, %xmm1
	movdqa	%xmm12, %xmm0
	movdqa	32(%rax,%rdi), %xmm8
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm12
	movdqa	%xmm14, %xmm0
	movdqa	%xmm14, %xmm1
	pcmpgtq	%xmm8, %xmm0
	movaps	%xmm12, 48(%r12)
	pblendvb	%xmm0, %xmm8, %xmm1
	pblendvb	%xmm0, %xmm14, %xmm8
	movdqa	%xmm2, %xmm0
	movdqa	32(%rax,%rdx), %xmm14
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm2, %xmm1
	movdqa	%xmm8, %xmm0
	movdqa	%xmm14, %xmm2
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm8
	movdqa	32(%rax,%r14), %xmm1
	movdqa	%xmm14, %xmm0
	movaps	%xmm8, 112(%r12)
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm2
	pblendvb	%xmm0, %xmm14, %xmm1
	movdqa	%xmm15, %xmm0
	pcmpgtq	%xmm2, %xmm0
	pblendvb	%xmm0, %xmm15, %xmm2
	movdqa	%xmm1, %xmm0
	pcmpgtq	%xmm2, %xmm0
	pblendvb	%xmm0, %xmm2, %xmm1
	movdqa	%xmm4, %xmm2
	punpcklqdq	%xmm2, %xmm2
	movdqa	%xmm1, %xmm0
	movaps	%xmm1, 176(%r12)
	movdqa	%xmm3, %xmm1
	pxor	%xmm2, %xmm1
	pxor	%xmm2, %xmm4
	pxor	%xmm2, %xmm13
	movdqa	192(%r12), %xmm3
	por	%xmm4, %xmm1
	pxor	%xmm2, %xmm12
	pxor	%xmm2, %xmm11
	por	%xmm13, %xmm1
	pxor	%xmm2, %xmm10
	pxor	%xmm2, %xmm9
	por	%xmm12, %xmm1
	pxor	%xmm2, %xmm8
	pxor	%xmm2, %xmm7
	por	%xmm11, %xmm1
	pxor	%xmm2, %xmm6
	pxor	%xmm2, %xmm5
	por	%xmm10, %xmm1
	pxor	%xmm2, %xmm0
	pxor	%xmm2, %xmm3
	por	%xmm9, %xmm1
	por	%xmm8, %xmm1
	por	%xmm7, %xmm1
	por	%xmm6, %xmm1
	por	%xmm5, %xmm1
	por	%xmm0, %xmm1
	pxor	%xmm0, %xmm0
	por	%xmm1, %xmm3
	pblendvb	%xmm0, %xmm3, %xmm1
	pxor	%xmm0, %xmm0
	pcmpeqq	%xmm0, %xmm1
	movmskpd	%xmm1, %eax
	cmpl	$3, %eax
	je	.L1007
	movdqa	.LC4(%rip), %xmm0
	movl	$2, %esi
	movq	%r12, %rdi
	movq	%r10, -112(%rbp)
	movups	%xmm0, 192(%r12)
	movups	%xmm0, 208(%r12)
	movups	%xmm0, 224(%r12)
	movups	%xmm0, 240(%r12)
	movups	%xmm0, 256(%r12)
	call	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	pcmpeqd	%xmm0, %xmm0
	movddup	(%r12), %xmm2
	movddup	184(%r12), %xmm1
	paddq	%xmm1, %xmm0
	movq	-112(%rbp), %r10
	pcmpeqq	%xmm2, %xmm0
	movmskpd	%xmm0, %eax
	cmpl	$3, %eax
	jne	.L1009
	movq	-72(%rbp), %rsi
	movq	%r10, %rdi
	leaq	-64(%rbp), %rdx
	movq	%r13, %rcx
	movdqa	%xmm2, %xmm0
	movq	%r10, -112(%rbp)
	call	_ZN3hwy6N_SSE46detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	movq	-112(%rbp), %r10
	testb	%al, %al
	jne	.L993
.L1009:
	movq	96(%r12), %rdx
	cmpq	%rdx, 88(%r12)
	jne	.L1105
	cmpq	80(%r12), %rdx
	jne	.L1048
	cmpq	72(%r12), %rdx
	jne	.L1106
	cmpq	64(%r12), %rdx
	jne	.L1107
	cmpq	56(%r12), %rdx
	jne	.L1108
	cmpq	48(%r12), %rdx
	jne	.L1109
	cmpq	40(%r12), %rdx
	jne	.L1110
	cmpq	32(%r12), %rdx
	jne	.L1111
	cmpq	24(%r12), %rdx
	jne	.L1112
	cmpq	16(%r12), %rdx
	jne	.L1113
	cmpq	8(%r12), %rdx
	jne	.L1114
	movq	(%r12), %rax
	cmpq	%rax, %rdx
	jne	.L1221
.L1050:
	movq	%rax, %xmm2
	punpcklqdq	%xmm2, %xmm2
.L1217:
	movl	$1, -112(%rbp)
.L1046:
	cmpq	$0, -80(%rbp)
	je	.L1222
	movq	-72(%rbp), %rax
	leaq	-2(%rax), %r9
	movq	%r9, %rdx
	movq	%r9, %rsi
	movdqu	(%r10,%r9,8), %xmm15
	andl	$7, %edx
	andl	$6, %esi
	je	.L1116
	movdqu	(%r10), %xmm1
	pcmpeqd	%xmm0, %xmm0
	xorl	%ecx, %ecx
	movdqa	.LC0(%rip), %xmm8
	leaq	_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %r13
	movdqa	%xmm1, %xmm3
	movdqa	%xmm1, %xmm4
	pcmpgtq	%xmm2, %xmm3
	pxor	%xmm3, %xmm0
	movmskpd	%xmm0, %eax
	popcntq	%rax, %rcx
	movq	%rcx, %xmm7
	salq	$4, %rax
	movddup	%xmm7, %xmm0
	pshufb	0(%r13,%rax), %xmm4
	pcmpgtq	%xmm8, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L1056
	movq	%xmm4, (%r10)
.L1056:
	pextrq	$1, %xmm0, %rax
	testq	%rax, %rax
	je	.L1057
	pextrq	$1, %xmm4, 8(%r10)
.L1057:
	movmskpd	%xmm3, %esi
	leaq	(%r10,%rcx,8), %rax
	xorl	%ecx, %ecx
	popcntq	%rsi, %rcx
	salq	$4, %rsi
	pshufb	0(%r13,%rsi), %xmm1
	movups	%xmm1, (%r12)
	testb	$4, %r9b
	je	.L1058
	movdqu	16(%r10), %xmm1
	pcmpeqd	%xmm0, %xmm0
	xorl	%edi, %edi
	movdqa	%xmm1, %xmm3
	movdqa	%xmm1, %xmm4
	pcmpgtq	%xmm2, %xmm3
	pxor	%xmm3, %xmm0
	movmskpd	%xmm0, %esi
	popcntq	%rsi, %rdi
	movq	%rdi, %xmm7
	salq	$4, %rsi
	movddup	%xmm7, %xmm0
	pshufb	0(%r13,%rsi), %xmm4
	pcmpgtq	%xmm8, %xmm0
	movq	%xmm0, %rsi
	testq	%rsi, %rsi
	je	.L1059
	movq	%xmm4, (%rax)
.L1059:
	pextrq	$1, %xmm0, %rsi
	testq	%rsi, %rsi
	je	.L1060
	pextrq	$1, %xmm4, 8(%rax)
.L1060:
	movmskpd	%xmm3, %esi
	leaq	(%rax,%rdi,8), %rax
	movq	%rsi, %rdi
	popcntq	%rsi, %rsi
	salq	$4, %rdi
	pshufb	0(%r13,%rdi), %xmm1
	movups	%xmm1, (%r12,%rcx,8)
	addq	%rsi, %rcx
	cmpq	$5, %rdx
	jbe	.L1058
	movdqu	32(%r10), %xmm1
	pcmpeqd	%xmm0, %xmm0
	xorl	%edi, %edi
	movdqa	%xmm1, %xmm3
	movdqa	%xmm1, %xmm4
	pcmpgtq	%xmm2, %xmm3
	pxor	%xmm3, %xmm0
	movmskpd	%xmm0, %esi
	popcntq	%rsi, %rdi
	movq	%rdi, %xmm7
	salq	$4, %rsi
	movddup	%xmm7, %xmm0
	pshufb	0(%r13,%rsi), %xmm4
	pcmpgtq	%xmm8, %xmm0
	movq	%xmm0, %rsi
	testq	%rsi, %rsi
	je	.L1061
	movq	%xmm4, (%rax)
.L1061:
	pextrq	$1, %xmm0, %rsi
	testq	%rsi, %rsi
	je	.L1062
	pextrq	$1, %xmm4, 8(%rax)
.L1062:
	movmskpd	%xmm3, %esi
	leaq	(%rax,%rdi,8), %rax
	movq	%rsi, %rdi
	popcntq	%rsi, %rsi
	salq	$4, %rdi
	pshufb	0(%r13,%rdi), %xmm1
	movups	%xmm1, (%r12,%rcx,8)
	addq	%rsi, %rcx
.L1058:
	leaq	-2(%rdx), %rsi
	leaq	1(%rdx), %rdi
	andq	$-2, %rsi
	leaq	0(,%rcx,8), %r8
	addq	$2, %rsi
	cmpq	$2, %rdi
	movl	$2, %edi
	cmovbe	%rdi, %rsi
.L1055:
	cmpq	%rsi, %rdx
	je	.L1063
	movdqu	(%r10,%rsi,8), %xmm3
	subq	%rsi, %rdx
	xorl	%esi, %esi
	movq	%rdx, %xmm0
	movdqa	%xmm3, %xmm4
	punpcklqdq	%xmm0, %xmm0
	movdqa	%xmm3, %xmm5
	pcmpgtq	%xmm2, %xmm4
	pcmpgtq	%xmm8, %xmm0
	movdqa	%xmm4, %xmm1
	pandn	%xmm0, %xmm1
	movmskpd	%xmm1, %edx
	popcntq	%rdx, %rsi
	movq	%rsi, %xmm7
	salq	$4, %rdx
	movddup	%xmm7, %xmm1
	pshufb	0(%r13,%rdx), %xmm5
	pcmpgtq	%xmm8, %xmm1
	movq	%xmm1, %rdx
	testq	%rdx, %rdx
	je	.L1064
	movq	%xmm5, (%rax)
.L1064:
	pextrq	$1, %xmm1, %rdx
	testq	%rdx, %rdx
	je	.L1065
	pextrq	$1, %xmm5, 8(%rax)
.L1065:
	pand	%xmm0, %xmm4
	leaq	(%rax,%rsi,8), %rax
	movmskpd	%xmm4, %edx
	movq	%rdx, %rsi
	popcntq	%rdx, %rdx
	addq	%rdx, %rcx
	salq	$4, %rsi
	pshufb	0(%r13,%rsi), %xmm3
	movups	%xmm3, (%r12,%r8)
	leaq	0(,%rcx,8), %r8
.L1063:
	movq	%r9, %rdx
	subq	%rcx, %rdx
	leaq	(%r10,%rdx,8), %r11
	cmpl	$8, %r8d
	jnb	.L1066
	testl	%r8d, %r8d
	jne	.L1223
.L1067:
	cmpl	$8, %r8d
	jnb	.L1070
	testl	%r8d, %r8d
	jne	.L1224
.L1071:
	movq	%rax, %rcx
	movq	%r9, %r14
	subq	%r10, %rcx
	sarq	$3, %rcx
	subq	%rcx, %r14
	subq	%rcx, %rdx
	movq	%rcx, %r15
	leaq	(%rax,%rdx,8), %rcx
	je	.L1117
	movdqu	-32(%rcx), %xmm7
	leaq	64(%rax), %rsi
	leaq	-64(%rcx), %rdi
	movdqu	(%rax), %xmm14
	movdqu	16(%rax), %xmm13
	movdqu	32(%rax), %xmm12
	movaps	%xmm7, -128(%rbp)
	movdqu	-16(%rcx), %xmm7
	movdqu	48(%rax), %xmm11
	movdqu	-64(%rcx), %xmm10
	movdqu	-48(%rcx), %xmm9
	movaps	%xmm7, -144(%rbp)
	cmpq	%rdi, %rsi
	je	.L1118
	xorl	%ecx, %ecx
	movl	$2, %r8d
	jmp	.L1078
	.p2align 4,,10
	.p2align 3
.L1226:
	movdqu	-64(%rdi), %xmm5
	movdqu	-48(%rdi), %xmm4
	prefetcht0	-256(%rdi)
	subq	$64, %rdi
	movdqu	32(%rdi), %xmm3
	movdqu	48(%rdi), %xmm1
.L1077:
	movdqa	%xmm5, %xmm6
	leaq	-2(%rdx,%rcx), %r11
	pcmpgtq	%xmm2, %xmm6
	movdqa	%xmm6, %xmm7
	movdqa	%xmm6, %xmm0
	movmskpd	%xmm6, %r9d
	punpcklqdq	%xmm6, %xmm7
	punpckhqdq	%xmm6, %xmm0
	popcntq	%r9, %r9
	pandn	%xmm7, %xmm0
	pshufd	$78, %xmm5, %xmm7
	pblendvb	%xmm0, %xmm7, %xmm5
	movups	%xmm5, (%rax,%rcx,8)
	addq	$2, %rcx
	movups	%xmm5, (%rax,%r11,8)
	movdqa	%xmm4, %xmm5
	subq	%r9, %rcx
	pcmpgtq	%xmm2, %xmm5
	leaq	-4(%rdx,%rcx), %r11
	movdqa	%xmm5, %xmm6
	movdqa	%xmm5, %xmm0
	movmskpd	%xmm5, %r9d
	punpcklqdq	%xmm5, %xmm6
	punpckhqdq	%xmm5, %xmm0
	popcntq	%r9, %r9
	pandn	%xmm6, %xmm0
	pshufd	$78, %xmm4, %xmm6
	pblendvb	%xmm0, %xmm6, %xmm4
	movups	%xmm4, (%rax,%rcx,8)
	movups	%xmm4, (%rax,%r11,8)
	movdqa	%xmm3, %xmm4
	movq	%r8, %r11
	pcmpgtq	%xmm2, %xmm4
	subq	%r9, %r11
	addq	%r11, %rcx
	leaq	-6(%rdx,%rcx), %r9
	subq	$8, %rdx
	movdqa	%xmm4, %xmm5
	movdqa	%xmm4, %xmm0
	movmskpd	%xmm4, %r11d
	punpcklqdq	%xmm4, %xmm5
	punpckhqdq	%xmm4, %xmm0
	popcntq	%r11, %r11
	pandn	%xmm5, %xmm0
	pshufd	$78, %xmm3, %xmm5
	pblendvb	%xmm0, %xmm5, %xmm3
	movups	%xmm3, (%rax,%rcx,8)
	movups	%xmm3, (%rax,%r9,8)
	movdqa	%xmm1, %xmm3
	movq	%r8, %r9
	pcmpgtq	%xmm2, %xmm3
	subq	%r11, %r9
	addq	%rcx, %r9
	leaq	(%r9,%rdx), %rcx
	movdqa	%xmm3, %xmm4
	movdqa	%xmm3, %xmm0
	movmskpd	%xmm3, %r11d
	punpcklqdq	%xmm3, %xmm4
	punpckhqdq	%xmm3, %xmm0
	popcntq	%r11, %r11
	pandn	%xmm4, %xmm0
	pshufd	$78, %xmm1, %xmm4
	pblendvb	%xmm0, %xmm4, %xmm1
	movups	%xmm1, (%rax,%r9,8)
	movups	%xmm1, (%rax,%rcx,8)
	movq	%r8, %rcx
	subq	%r11, %rcx
	addq	%r9, %rcx
	cmpq	%rdi, %rsi
	je	.L1225
.L1078:
	movq	%rsi, %r9
	subq	%rax, %r9
	sarq	$3, %r9
	subq	%rcx, %r9
	cmpq	$8, %r9
	ja	.L1226
	movdqu	(%rsi), %xmm5
	movdqu	16(%rsi), %xmm4
	prefetcht0	256(%rsi)
	addq	$64, %rsi
	movdqu	-32(%rsi), %xmm3
	movdqu	-16(%rsi), %xmm1
	jmp	.L1077
	.p2align 4,,10
	.p2align 3
.L1220:
	movl	$8, %eax
	subq	%rdx, %rax
	leaq	(%rdi,%rax,8), %rax
	movq	-72(%rbp), %rdi
	leaq	-8(%rdx,%rdi), %r14
	jmp	.L1005
	.p2align 4,,10
	.p2align 3
.L1225:
	leaq	2(%rcx), %rsi
	leaq	(%rax,%rcx,8), %rdi
	addq	%rdx, %rcx
.L1075:
	movdqa	%xmm14, %xmm1
	movdqa	-128(%rbp), %xmm7
	pcmpgtq	%xmm2, %xmm1
	movdqa	%xmm1, %xmm3
	movdqa	%xmm1, %xmm0
	movmskpd	%xmm1, %r8d
	punpcklqdq	%xmm1, %xmm3
	punpckhqdq	%xmm1, %xmm0
	movdqa	%xmm13, %xmm1
	popcntq	%r8, %r8
	pcmpgtq	%xmm2, %xmm1
	pandn	%xmm3, %xmm0
	pshufd	$78, %xmm14, %xmm3
	subq	%r8, %rsi
	pblendvb	%xmm0, %xmm3, %xmm14
	movups	%xmm14, (%rdi)
	leaq	-4(%rdx,%rsi), %rdi
	movdqa	%xmm1, %xmm3
	movdqa	%xmm1, %xmm0
	movups	%xmm14, -16(%rax,%rcx,8)
	movmskpd	%xmm1, %ecx
	punpcklqdq	%xmm1, %xmm3
	punpckhqdq	%xmm1, %xmm0
	movdqa	%xmm12, %xmm1
	pcmpgtq	%xmm2, %xmm1
	pandn	%xmm3, %xmm0
	pshufd	$78, %xmm13, %xmm3
	pblendvb	%xmm0, %xmm3, %xmm13
	movups	%xmm13, (%rax,%rsi,8)
	movdqa	%xmm1, %xmm3
	movups	%xmm13, (%rax,%rdi,8)
	movdqa	%xmm1, %xmm0
	xorl	%edi, %edi
	punpcklqdq	%xmm1, %xmm3
	popcntq	%rcx, %rdi
	punpckhqdq	%xmm1, %xmm0
	subq	%rdi, %rsi
	leaq	2(%rsi), %rcx
	pandn	%xmm3, %xmm0
	movmskpd	%xmm1, %edi
	pshufd	$78, %xmm12, %xmm3
	movdqa	%xmm11, %xmm1
	leaq	-6(%rdx,%rcx), %rsi
	popcntq	%rdi, %rdi
	pcmpgtq	%xmm2, %xmm1
	pblendvb	%xmm0, %xmm3, %xmm12
	movups	%xmm12, (%rax,%rcx,8)
	movups	%xmm12, (%rax,%rsi,8)
	movl	$2, %esi
	movq	%rsi, %r8
	movdqa	%xmm1, %xmm3
	movdqa	%xmm1, %xmm0
	subq	%rdi, %r8
	punpcklqdq	%xmm1, %xmm3
	punpckhqdq	%xmm1, %xmm0
	addq	%r8, %rcx
	movmskpd	%xmm1, %r8d
	movdqa	%xmm10, %xmm1
	pcmpgtq	%xmm2, %xmm1
	pandn	%xmm3, %xmm0
	pshufd	$78, %xmm11, %xmm3
	popcntq	%r8, %r8
	pblendvb	%xmm0, %xmm3, %xmm11
	leaq	-8(%rdx,%rcx), %rdi
	movups	%xmm11, (%rax,%rcx,8)
	movdqa	%xmm1, %xmm3
	movups	%xmm11, (%rax,%rdi,8)
	movdqa	%xmm1, %xmm0
	movq	%rsi, %rdi
	punpcklqdq	%xmm1, %xmm3
	subq	%r8, %rdi
	punpckhqdq	%xmm1, %xmm0
	movmskpd	%xmm1, %r8d
	movdqa	%xmm9, %xmm1
	pandn	%xmm3, %xmm0
	addq	%rcx, %rdi
	pcmpgtq	%xmm2, %xmm1
	pshufd	$78, %xmm10, %xmm3
	leaq	-10(%rdx,%rdi), %rcx
	popcntq	%r8, %r8
	pblendvb	%xmm0, %xmm3, %xmm10
	movups	%xmm10, (%rax,%rdi,8)
	movdqa	%xmm1, %xmm3
	movups	%xmm10, (%rax,%rcx,8)
	movdqa	%xmm1, %xmm0
	movq	%rsi, %rcx
	punpcklqdq	%xmm1, %xmm3
	subq	%r8, %rcx
	punpckhqdq	%xmm1, %xmm0
	movmskpd	%xmm1, %r8d
	movdqa	%xmm7, %xmm1
	pandn	%xmm3, %xmm0
	addq	%rdi, %rcx
	pcmpgtq	%xmm2, %xmm1
	pshufd	$78, %xmm9, %xmm3
	leaq	-12(%rdx,%rcx), %rdi
	popcntq	%r8, %r8
	pblendvb	%xmm0, %xmm3, %xmm9
	movups	%xmm9, (%rax,%rcx,8)
	movdqa	%xmm1, %xmm3
	movups	%xmm9, (%rax,%rdi,8)
	movdqa	%xmm1, %xmm0
	movq	%rsi, %rdi
	punpcklqdq	%xmm1, %xmm3
	subq	%r8, %rdi
	punpckhqdq	%xmm1, %xmm0
	addq	%rcx, %rdi
	pandn	%xmm3, %xmm0
	pshufd	$78, %xmm7, %xmm3
	pblendvb	%xmm0, %xmm3, %xmm7
	leaq	-14(%rdx,%rdi), %rcx
	movmskpd	%xmm1, %r8d
	movups	%xmm7, (%rax,%rdi,8)
	popcntq	%r8, %r8
	movups	%xmm7, (%rax,%rcx,8)
	movdqa	-144(%rbp), %xmm7
	movq	%rsi, %rcx
	subq	%r8, %rcx
	movdqa	%xmm7, %xmm1
	addq	%rdi, %rcx
	pcmpgtq	%xmm2, %xmm1
	leaq	-16(%rdx,%rcx), %rdx
	movdqa	%xmm1, %xmm3
	movdqa	%xmm1, %xmm0
	movmskpd	%xmm1, %edi
	punpcklqdq	%xmm1, %xmm3
	punpckhqdq	%xmm1, %xmm0
	popcntq	%rdi, %rdi
	subq	%rdi, %rsi
	pandn	%xmm3, %xmm0
	pshufd	$78, %xmm7, %xmm3
	movq	%r14, %rdi
	pblendvb	%xmm0, %xmm3, %xmm7
	movups	%xmm7, (%rax,%rcx,8)
	movups	%xmm7, (%rax,%rdx,8)
	leaq	(%rsi,%rcx), %rdx
	subq	%rdx, %rdi
	leaq	0(,%rdx,8), %rsi
.L1074:
	movdqa	%xmm15, %xmm1
	cmpq	$2, %rdi
	leaq	-16(,%r14,8), %rcx
	pcmpgtq	%xmm2, %xmm1
	cmovnb	%rsi, %rcx
	pcmpeqd	%xmm0, %xmm0
	movdqa	%xmm15, %xmm2
	movdqu	(%rax,%rcx), %xmm7
	xorl	%ecx, %ecx
	pxor	%xmm1, %xmm0
	movmskpd	%xmm0, %edi
	movups	%xmm7, (%rax,%r14,8)
	popcntq	%rdi, %rcx
	movq	%rcx, %xmm7
	salq	$4, %rdi
	movddup	%xmm7, %xmm0
	pshufb	0(%r13,%rdi), %xmm2
	pcmpgtq	%xmm8, %xmm0
	movq	%xmm0, %rdi
	testq	%rdi, %rdi
	je	.L1080
	movq	%xmm2, (%rax,%rsi)
.L1080:
	pextrq	$1, %xmm0, %rdi
	testq	%rdi, %rdi
	je	.L1081
	pextrq	$1, %xmm2, 8(%rax,%rsi)
.L1081:
	addq	%rdx, %rcx
	movmskpd	%xmm1, %edx
	movq	%rdx, %rdi
	leaq	0(,%rcx,8), %rsi
	salq	$4, %rdi
	pshufb	0(%r13,%rdi), %xmm15
	xorl	%edi, %edi
	popcntq	%rdx, %rdi
	movq	%rdi, %xmm0
	punpcklqdq	%xmm0, %xmm0
	pcmpgtq	%xmm8, %xmm0
	movq	%xmm0, %rdx
	testq	%rdx, %rdx
	je	.L1082
	movq	%xmm15, (%rax,%rcx,8)
.L1082:
	pextrq	$1, %xmm0, %rdx
	testq	%rdx, %rdx
	je	.L1083
	pextrq	$1, %xmm15, 8(%rax,%rsi)
.L1083:
	movq	-80(%rbp), %r14
	addq	%rcx, %r15
	subq	$1, %r14
	cmpl	$2, -112(%rbp)
	je	.L1085
	movq	-88(%rbp), %rsi
	movq	%r10, %rdi
	movq	%r14, %r9
	movq	%rbx, %r8
	movq	%r12, %rcx
	movq	%r15, %rdx
	movq	%r10, -80(%rbp)
	call	_ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	cmpl	$3, -112(%rbp)
	movq	-80(%rbp), %r10
	je	.L993
.L1085:
	movq	-72(%rbp), %rdx
	movq	-88(%rbp), %rsi
	leaq	(%r10,%r15,8), %rdi
	movq	%r14, %r9
	movq	%rbx, %r8
	movq	%r12, %rcx
	subq	%r15, %rdx
	call	_ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L993:
	addq	$104, %rsp
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
.L1070:
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
	jmp	.L1071
	.p2align 4,,10
	.p2align 3
.L1066:
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
	jmp	.L1067
.L1224:
	movzbl	(%r12), %ecx
	movb	%cl, (%r11)
	jmp	.L1071
.L1223:
	movzbl	(%r11), %ecx
	movb	%cl, (%rax)
	jmp	.L1067
.L1219:
	cmpq	$1, %rdx
	jbe	.L993
	leaq	256(%rdi), %rax
	cmpq	%rax, %rsi
	jb	.L997
	movl	$2, %esi
	call	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	jmp	.L993
.L1007:
	movq	-112(%rbp), %rax
	movl	$2, %edi
	movdqu	(%r10), %xmm0
	movdqa	.LC0(%rip), %xmm8
	andl	$1, %eax
	pcmpeqq	%xmm2, %xmm0
	subq	%rax, %rdi
	movq	%rdi, %xmm7
	movddup	%xmm7, %xmm1
	pcmpgtq	%xmm8, %xmm1
	pandn	%xmm1, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1227
	pxor	%xmm1, %xmm1
	movq	-72(%rbp), %r8
	leaq	256(%r10,%rdi,8), %rsi
	movdqa	%xmm1, %xmm0
	movdqa	%xmm1, %xmm4
	.p2align 4,,10
	.p2align 3
.L1013:
	movq	%rdi, %rcx
	leaq	32(%rdi), %rdi
	cmpq	%rdi, %r8
	jb	.L1228
	leaq	-256(%rsi), %rax
.L1012:
	movdqa	(%rax), %xmm3
	leaq	32(%rax), %rdx
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm0
	movdqa	16(%rax), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm1
	movdqa	32(%rax), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm0
	movdqa	48(%rax), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm1
	movdqa	64(%rax), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm0
	movdqa	80(%rax), %xmm3
	leaq	96(%rdx), %rax
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm1
	movdqa	64(%rdx), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm0
	movdqa	80(%rdx), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm1
	cmpq	%rsi, %rax
	jne	.L1012
	movdqa	%xmm0, %xmm3
	leaq	352(%rdx), %rsi
	por	%xmm1, %xmm3
	pcmpeqq	%xmm4, %xmm3
	movmskpd	%xmm3, %eax
	cmpl	$3, %eax
	je	.L1013
	movdqa	%xmm2, %xmm0
	pcmpeqd	%xmm1, %xmm1
	pcmpeqq	(%r10,%rcx,8), %xmm0
	pxor	%xmm1, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1015
	.p2align 4,,10
	.p2align 3
.L1014:
	addq	$2, %rcx
	movdqa	%xmm2, %xmm0
	pcmpeqq	(%r10,%rcx,8), %xmm0
	pxor	%xmm1, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1014
.L1015:
	rep bsfl	%eax, %eax
	cltq
	addq	%rcx, %rax
.L1011:
	leaq	(%r10,%rax,8), %r8
	movq	(%r8), %rdi
	movq	%rdi, %xmm7
	movddup	%xmm7, %xmm1
	movdqa	%xmm1, %xmm0
	pcmpgtq	%xmm2, %xmm0
	movmskpd	%xmm0, %edx
	testl	%edx, %edx
	jne	.L1020
	movq	-72(%rbp), %rsi
	xorl	%ecx, %ecx
	leaq	-2(%rsi), %rax
	jmp	.L1027
	.p2align 4,,10
	.p2align 3
.L1021:
	movmskpd	%xmm0, %edx
	movups	%xmm2, (%r10,%rax,8)
	popcntq	%rdx, %rdx
	addq	%rdx, %rcx
	leaq	-2(%rax), %rdx
	cmpq	%rdx, %rsi
	jbe	.L1229
	movq	%rdx, %rax
.L1027:
	movdqu	(%r10,%rax,8), %xmm3
	movdqu	(%r10,%rax,8), %xmm0
	pcmpeqq	%xmm1, %xmm3
	pcmpeqq	%xmm2, %xmm0
	movdqa	%xmm3, %xmm4
	por	%xmm0, %xmm4
	movmskpd	%xmm4, %edx
	cmpl	$3, %edx
	je	.L1021
	pcmpeqd	%xmm4, %xmm4
	leaq	2(%rax), %rsi
	pxor	%xmm4, %xmm0
	pandn	%xmm0, %xmm3
	movmskpd	%xmm3, %edx
	rep bsfl	%edx, %edx
	movslq	%edx, %rdx
	addq	%rax, %rdx
	addq	$4, %rax
	movddup	(%r10,%rdx,8), %xmm3
	movq	-72(%rbp), %rdx
	movaps	%xmm3, -64(%rbp)
	subq	%rcx, %rdx
	cmpq	%rax, %rdx
	jb	.L1022
	.p2align 4,,10
	.p2align 3
.L1023:
	movups	%xmm1, -16(%r10,%rax,8)
	movq	%rax, %rsi
	addq	$2, %rax
	cmpq	%rdx, %rax
	jbe	.L1023
.L1022:
	subq	%rsi, %rdx
	leaq	0(,%rsi,8), %rcx
	movq	%rdx, %xmm0
	punpcklqdq	%xmm0, %xmm0
	pcmpgtq	%xmm8, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L1031
	movq	%rdi, (%r10,%rsi,8)
.L1031:
	pextrq	$1, %xmm0, %rax
	testq	%rax, %rax
	je	.L1026
	movq	%rdi, 8(%r10,%rcx)
.L1026:
	movdqa	%xmm2, %xmm0
	pcmpeqq	.LC12(%rip), %xmm0
	movmskpd	%xmm0, %eax
	cmpl	$3, %eax
	je	.L1103
	movdqa	%xmm2, %xmm0
	pcmpeqq	.LC4(%rip), %xmm0
	movmskpd	%xmm0, %eax
	movl	%eax, -112(%rbp)
	cmpl	$3, %eax
	je	.L1230
	movdqa	%xmm3, %xmm0
	movdqa	%xmm3, %xmm5
	movdqa	%xmm2, %xmm4
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm1, %xmm5
	pcmpgtq	%xmm5, %xmm4
	movmskpd	%xmm4, %eax
	testl	%eax, %eax
	jne	.L1231
	movdqa	%xmm2, %xmm3
	movl	$32, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L1037:
	leaq	(%rcx,%rax,2), %rdx
	addq	$1, %rax
	movdqu	(%r10,%rdx,8), %xmm1
	movdqa	%xmm1, %xmm0
	pcmpgtq	%xmm3, %xmm0
	pblendvb	%xmm0, %xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	cmpq	$16, %rax
	jne	.L1037
	movdqa	%xmm2, %xmm0
	pcmpgtq	%xmm1, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1217
	leaq	32(%rsi), %rax
	cmpq	%rax, -72(%rbp)
	jb	.L1232
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L1037
.L1116:
	xorl	%r8d, %r8d
	xorl	%ecx, %ecx
	leaq	_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %r13
	movq	%r10, %rax
	movdqa	.LC0(%rip), %xmm8
	jmp	.L1055
.L1118:
	movq	%rdx, %rcx
	movq	%rax, %rdi
	movl	$2, %esi
	jmp	.L1075
.L1117:
	xorl	%esi, %esi
	movq	%r14, %rdi
	jmp	.L1074
.L1222:
	movq	-72(%rbp), %rsi
	movq	%r10, %rdi
	leaq	-1(%rsi), %rbx
	movq	%rbx, %r12
	shrq	%r12
	.p2align 4,,10
	.p2align 3
.L1053:
	movq	%r12, %rdx
	call	_ZN3hwy6N_SSE46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	subq	$1, %r12
	jnb	.L1053
	.p2align 4,,10
	.p2align 3
.L1054:
	movq	(%rdi,%rbx,8), %rdx
	movq	(%rdi), %rax
	movq	%rbx, %rsi
	movq	%rdx, (%rdi)
	xorl	%edx, %edx
	movq	%rax, (%rdi,%rbx,8)
	call	_ZN3hwy6N_SSE46detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	subq	$1, %rbx
	jne	.L1054
	jmp	.L993
.L1105:
	movl	$12, %eax
	movl	$11, %esi
	jmp	.L1051
	.p2align 4,,10
	.p2align 3
.L1052:
	cmpq	$23, %rax
	je	.L1216
.L1051:
	movq	%rax, %rcx
	addq	$1, %rax
	cmpq	(%r12,%rax,8), %rdx
	je	.L1052
	movl	$12, %edi
	subq	$11, %rcx
	movq	%rdx, %rax
	subq	%rsi, %rdi
	cmpq	%rdi, %rcx
	jb	.L1050
.L1216:
	movq	(%r12,%rsi,8), %rax
	jmp	.L1050
.L1106:
	movl	$9, %esi
	movl	$10, %eax
	jmp	.L1051
.L1107:
	movl	$8, %esi
	movl	$9, %eax
	jmp	.L1051
.L1108:
	movl	$7, %esi
	movl	$8, %eax
	jmp	.L1051
.L1109:
	movl	$6, %esi
	movl	$7, %eax
	jmp	.L1051
.L1228:
	movq	-72(%rbp), %rsi
	pcmpeqd	%xmm1, %xmm1
	.p2align 4,,10
	.p2align 3
.L1017:
	movq	%rcx, %rdx
	addq	$2, %rcx
	cmpq	%rcx, %rsi
	jb	.L1233
	movdqa	%xmm2, %xmm0
	pcmpeqq	-16(%r10,%rcx,8), %xmm0
	pxor	%xmm1, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1017
.L1214:
	rep bsfl	%eax, %eax
	cltq
	addq	%rdx, %rax
	jmp	.L1011
.L1110:
	movl	$5, %esi
	movl	$6, %eax
	jmp	.L1051
.L1111:
	movl	$4, %esi
	movl	$5, %eax
	jmp	.L1051
.L1020:
	movq	-72(%rbp), %rsi
	leaq	-64(%rbp), %rdx
	movq	%r12, %rcx
	movdqa	%xmm2, %xmm0
	movq	%r8, %rdi
	movq	%r10, -128(%rbp)
	subq	%rax, %rsi
	movaps	%xmm1, -112(%rbp)
	call	_ZN3hwy6N_SSE46detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L993
	movdqa	-64(%rbp), %xmm3
	movq	-128(%rbp), %r10
	movddup	(%r12), %xmm2
	movdqa	-112(%rbp), %xmm1
	jmp	.L1026
.L1112:
	movl	$3, %esi
	movl	$4, %eax
	jmp	.L1051
.L1113:
	movl	$2, %esi
	movl	$3, %eax
	jmp	.L1051
.L1114:
	movl	$1, %esi
	movl	$2, %eax
	jmp	.L1051
.L1221:
	xorl	%esi, %esi
	movl	$1, %eax
	jmp	.L1051
.L1229:
	movdqu	(%r10), %xmm4
	movdqu	(%r10), %xmm0
	movq	%rax, %xmm7
	movddup	%xmm7, %xmm3
	movq	-72(%rbp), %rdx
	pcmpeqq	%xmm2, %xmm4
	pcmpeqq	%xmm1, %xmm0
	pcmpgtq	%xmm8, %xmm3
	subq	%rcx, %rdx
	movdqa	%xmm4, %xmm5
	por	%xmm4, %xmm0
	pcmpeqd	%xmm4, %xmm4
	pand	%xmm3, %xmm5
	pxor	%xmm4, %xmm3
	por	%xmm3, %xmm0
	movmskpd	%xmm0, %esi
	cmpl	$3, %esi
	jne	.L1234
	movmskpd	%xmm5, %ecx
	movq	%rdx, %rax
	movups	%xmm2, (%r10)
	popcntq	%rcx, %rcx
	subq	%rcx, %rax
	cmpq	$1, %rax
	jbe	.L1092
	leaq	-2(%rax), %rcx
	movq	-128(%rbp), %rsi
	movq	%rcx, %rdx
	shrq	%rdx
	salq	$4, %rdx
	leaq	16(%r10,%rdx), %rdx
.L1034:
	movups	%xmm1, (%rsi)
	addq	$16, %rsi
	cmpq	%rsi, %rdx
	jne	.L1034
	movq	%rcx, %rdx
	andq	$-2, %rdx
	addq	$2, %rdx
	leaq	0(,%rdx,8), %rcx
	subq	%rdx, %rax
.L1033:
	movaps	%xmm1, (%r12)
	testq	%rax, %rax
	je	.L993
	leaq	(%r10,%rcx), %rdi
	leaq	0(,%rax,8), %rdx
	movq	%r12, %rsi
	call	memcpy@PLT
	jmp	.L993
.L1232:
	movq	-72(%rbp), %rdx
	jmp	.L1044
.L1045:
	movdqu	-16(%r10,%rsi,8), %xmm7
	movdqa	%xmm2, %xmm0
	pcmpgtq	%xmm7, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1217
.L1044:
	movq	%rsi, %rax
	addq	$2, %rsi
	cmpq	%rsi, %rdx
	jnb	.L1045
	movq	-72(%rbp), %rdi
	cmpq	%rax, %rdi
	je	.L1103
	movdqu	-16(%r10,%rdi,8), %xmm7
	movdqa	%xmm2, %xmm0
	pcmpgtq	%xmm7, %xmm0
	movaps	%xmm7, -112(%rbp)
	movmskpd	%xmm0, %eax
	cmpl	$1, %eax
	movl	$1, %eax
	adcl	$0, %eax
	movl	%eax, -112(%rbp)
	jmp	.L1046
.L1048:
	movl	$11, %eax
	movl	$10, %esi
	jmp	.L1051
.L1233:
	movq	-72(%rbp), %rax
	pcmpeqd	%xmm1, %xmm1
	leaq	-2(%rax), %rdx
	movdqu	(%r10,%rdx,8), %xmm0
	pcmpeqq	%xmm2, %xmm0
	pxor	%xmm1, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	je	.L993
	jmp	.L1214
.L1227:
	rep bsfl	%eax, %eax
	cltq
	jmp	.L1011
.L1103:
	movl	$2, -112(%rbp)
	jmp	.L1046
.L997:
	movq	%rdx, %r11
	leaq	-2(%rdx), %rdx
	movq	(%rdi), %rax
	movq	%r10, %rsi
	movq	%rdx, %rbx
	andq	$-2, %rdx
	shrq	%rbx
	movq	%rax, (%rcx)
	leaq	2(%rdx), %r13
	addq	$1, %rbx
	salq	$4, %rbx
	movl	%ebx, %r8d
	leaq	8(%rdi,%r8), %r15
	leaq	8(%rcx,%r8), %r14
	movq	-16(%r15), %rax
	leaq	8(%rcx), %rdi
	andq	$-8, %rdi
	movq	%rax, -16(%r14)
	movq	%rcx, %rax
	subq	%rdi, %rax
	subq	%rax, %rsi
	leal	(%rbx,%rax), %ecx
	movq	%r11, %rax
	shrl	$3, %ecx
	subq	%r13, %rax
	rep movsq
	movq	%r11, -72(%rbp)
	movq	%rax, %rsi
	movq	%rax, -80(%rbp)
	je	.L998
	leaq	0(,%r13,8), %rax
	leaq	0(,%rsi,8), %rdx
	movq	%r8, -112(%rbp)
	leaq	(%r10,%rax), %rsi
	leaq	(%r12,%rax), %rdi
	movq	%r10, -88(%rbp)
	call	memcpy@PLT
	movq	-72(%rbp), %r11
	movl	$32, %ecx
	movq	-88(%rbp), %r10
	movq	-112(%rbp), %r8
	movl	%r11d, %eax
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
	movq	%rsi, %rax
	salq	$4, %rax
	addq	$2, %rax
	cmpq	%rax, %r11
	jnb	.L1235
.L999:
	movdqa	.LC4(%rip), %xmm0
	movq	-72(%rbp), %rdx
.L1003:
	movups	%xmm0, (%r12,%rdx,8)
	addq	$2, %rdx
	cmpq	%rax, %rdx
	jb	.L1003
	movq	%r12, %rdi
	movq	%r8, -88(%rbp)
	movq	%r10, -72(%rbp)
	call	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	movq	-72(%rbp), %r10
	movq	(%r12), %rax
	movq	%r12, %rsi
	movq	%rax, (%r10)
	movq	-88(%rbp), %r8
	leaq	8(%r10), %rdi
	andq	$-8, %rdi
	movq	-8(%r12,%r8), %rax
	movq	%rax, -8(%r10,%r8)
	movq	%r10, %rax
	subq	%rdi, %rax
	leal	(%rbx,%rax), %ecx
	subq	%rax, %rsi
	shrl	$3, %ecx
	rep movsq
	cmpq	$0, -80(%rbp)
	je	.L993
.L1001:
	movq	-80(%rbp), %rdx
	salq	$3, %r13
	leaq	(%r10,%r13), %rdi
	leaq	(%r12,%r13), %rsi
	salq	$3, %rdx
	call	memcpy@PLT
	jmp	.L993
.L1230:
	pcmpeqd	%xmm0, %xmm0
	paddq	%xmm0, %xmm2
	jmp	.L1046
.L1231:
	movdqa	%xmm1, %xmm7
	pblendvb	%xmm0, %xmm3, %xmm7
	movdqa	%xmm7, %xmm0
	pcmpgtq	%xmm2, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1217
	movdqa	%xmm2, %xmm1
	movl	$32, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L1038:
	leaq	(%rcx,%rax,2), %rdx
	movdqa	%xmm1, %xmm7
	addq	$1, %rax
	movdqu	(%r10,%rdx,8), %xmm3
	movdqa	%xmm3, %xmm0
	pcmpgtq	%xmm1, %xmm0
	pblendvb	%xmm0, %xmm3, %xmm7
	movdqa	%xmm7, %xmm0
	movdqa	%xmm7, %xmm1
	cmpq	$16, %rax
	jne	.L1038
	pcmpgtq	%xmm2, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1217
	leaq	32(%rsi), %rax
	cmpq	%rax, -72(%rbp)
	jb	.L1040
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L1038
.L1041:
	movdqu	-16(%r10,%rsi,8), %xmm0
	pcmpgtq	%xmm2, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1217
.L1040:
	movq	%rsi, %rax
	addq	$2, %rsi
	cmpq	%rsi, -72(%rbp)
	jnb	.L1041
	movq	-72(%rbp), %rdi
	cmpq	%rax, %rdi
	je	.L1042
	movdqu	-16(%r10,%rdi,8), %xmm7
	movdqa	%xmm7, %xmm0
	movaps	%xmm7, -112(%rbp)
	pcmpgtq	%xmm2, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1217
.L1042:
	pcmpeqd	%xmm0, %xmm0
	movl	$3, -112(%rbp)
	paddq	%xmm0, %xmm2
	jmp	.L1046
.L998:
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
	movq	%rsi, %rax
	salq	$4, %rax
	addq	$2, %rax
	cmpq	%rax, %rdi
	jb	.L999
	movq	%r12, %rdi
	movq	%r10, -72(%rbp)
	call	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	movq	-72(%rbp), %r10
	movq	(%r12), %rax
	movq	%r12, %rsi
	leaq	8(%r10), %rdi
	movq	%rax, (%r10)
	movq	-16(%r14), %rax
	andq	$-8, %rdi
	subq	%rdi, %r10
	movq	%rax, -16(%r15)
	leal	(%rbx,%r10), %ecx
	subq	%r10, %rsi
	shrl	$3, %ecx
	rep movsq
	jmp	.L993
	.p2align 4,,10
	.p2align 3
.L1092:
	xorl	%ecx, %ecx
	jmp	.L1033
.L1234:
	pxor	%xmm4, %xmm0
	movmskpd	%xmm0, %ecx
	rep bsfl	%ecx, %ecx
	movslq	%ecx, %rcx
	movddup	(%r10,%rcx,8), %xmm3
	leaq	2(%rax), %rcx
	movaps	%xmm3, -64(%rbp)
	cmpq	%rdx, %rcx
	ja	.L1029
.L1030:
	movups	%xmm1, -16(%r10,%rcx,8)
	movq	%rcx, %rax
	addq	$2, %rcx
	cmpq	%rdx, %rcx
	jbe	.L1030
.L1029:
	subq	%rax, %rdx
	leaq	0(,%rax,8), %rcx
	movq	%rdx, %xmm0
	punpcklqdq	%xmm0, %xmm0
	pcmpgtq	%xmm8, %xmm0
	movq	%xmm0, %rdx
	testq	%rdx, %rdx
	je	.L1031
	movq	%rdi, (%r10,%rax,8)
	jmp	.L1031
.L1235:
	movq	%r12, %rdi
	movq	%r10, -72(%rbp)
	call	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	movq	-72(%rbp), %r10
	movq	(%r12), %rax
	movq	%r12, %rsi
	movq	%rax, (%r10)
	movq	-16(%r14), %rax
	leaq	8(%r10), %rdi
	andq	$-8, %rdi
	movq	%rax, -16(%r15)
	movq	%r10, %rax
	subq	%rdi, %rax
	leal	(%rbx,%rax), %ecx
	subq	%rax, %rsi
	shrl	$3, %ecx
	rep movsq
	jmp	.L1001
	.cfi_endproc
.LFE18803:
	.size	_ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0, .-_ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	.section	.text._ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0, @function
_ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0:
.LFB18805:
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
	subq	$392, %rsp
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	movq	%rdi, -104(%rbp)
	movq	%rsi, -264(%rbp)
	movq	%rdx, -216(%rbp)
	movq	%r8, -192(%rbp)
	movq	%r9, -224(%rbp)
	cmpq	$32, %rdx
	jbe	.L1463
	movq	%rdi, %r10
	movq	%rdi, %r12
	shrq	$3, %r10
	movq	%r10, %rax
	andl	$7, %eax
	jne	.L1464
	movq	%rdx, %r14
	movq	%rdi, %r15
	movq	%r8, %rax
.L1248:
	movq	8(%rax), %rdx
	movq	16(%rax), %r9
	movq	%rdx, %rsi
	leaq	1(%r9), %rdi
	leaq	(%rdx,%rdx,8), %rcx
	xorq	(%rax), %rdi
	rolq	$24, %rsi
	movq	%rsi, %rax
	movq	%rdx, %rsi
	leaq	2(%r9), %rdx
	addq	%rdi, %rax
	shrq	$11, %rsi
	xorq	%rsi, %rcx
	movq	%rax, %rsi
	movq	%rax, %r8
	rolq	$24, %rsi
	xorq	%rdx, %rcx
	shrq	$11, %r8
	leaq	(%rax,%rax,8), %rdx
	addq	%rcx, %rsi
	xorq	%r8, %rdx
	leaq	3(%r9), %rax
	movq	%rsi, %r11
	movq	%rsi, %r8
	xorq	%rax, %rdx
	shrq	$11, %r8
	rolq	$24, %r11
	leaq	(%rsi,%rsi,8), %rax
	leaq	4(%r9), %rsi
	addq	%rdx, %r11
	xorq	%r8, %rax
	addq	$5, %r9
	xorq	%rsi, %rax
	movq	%r11, %r8
	movq	%r11, %rsi
	shrq	$11, %rsi
	rolq	$24, %r8
	addq	%rax, %r8
	movq	%rsi, %rbx
	leaq	(%r11,%r11,8), %rsi
	xorq	%rbx, %rsi
	movq	%r8, %rbx
	leaq	(%r8,%r8,8), %r11
	rolq	$24, %r8
	xorq	%r9, %rsi
	shrq	$11, %rbx
	xorq	%rbx, %r11
	addq	%rsi, %r8
	movq	-192(%rbp), %rbx
	movl	%esi, %esi
	movq	%r11, %xmm0
	movq	%r8, %xmm6
	movl	%ecx, %r11d
	movabsq	$34359738359, %r8
	punpcklqdq	%xmm6, %xmm0
	movq	%r9, 16(%rbx)
	movl	%edx, %r9d
	movups	%xmm0, (%rbx)
	movq	%r14, %rbx
	shrq	$3, %rbx
	cmpq	%r8, %r14
	movl	$4294967295, %r8d
	movl	%edi, %r14d
	cmova	%r8, %rbx
	shrq	$32, %rdi
	movl	%eax, %r8d
	shrq	$32, %rcx
	shrq	$32, %rdx
	imulq	%rbx, %r14
	shrq	$32, %rax
	imulq	%rbx, %rdi
	imulq	%rbx, %r11
	imulq	%rbx, %rcx
	shrq	$32, %r14
	imulq	%rbx, %r9
	shrq	$32, %rdi
	salq	$6, %r14
	imulq	%rbx, %rdx
	shrq	$32, %r11
	salq	$6, %rdi
	addq	%r15, %r14
	imulq	%rbx, %r8
	shrq	$32, %rcx
	salq	$6, %r11
	addq	%r15, %rdi
	shrq	$32, %r9
	salq	$6, %rcx
	addq	%r15, %r11
	shrq	$32, %rdx
	salq	$6, %r9
	addq	%r15, %rcx
	shrq	$32, %r8
	salq	$6, %rdx
	addq	%r15, %r9
	salq	$6, %r8
	addq	%r15, %rdx
	addq	%r15, %r8
	imulq	%rbx, %rax
	imulq	%rbx, %rsi
	xorl	%ebx, %ebx
	shrq	$32, %rax
	shrq	$32, %rsi
	salq	$6, %rax
	salq	$6, %rsi
	addq	%r15, %rax
	addq	%r15, %rsi
.L1250:
	movdqa	(%r11,%rbx,8), %xmm2
	movdqa	(%r14,%rbx,8), %xmm4
	movdqa	(%rdi,%rbx,8), %xmm0
	movdqa	%xmm2, %xmm3
	movdqa	%xmm4, %xmm1
	pcmpeqd	%xmm4, %xmm3
	psubq	%xmm2, %xmm1
	pand	%xmm3, %xmm1
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm4, %xmm3
	por	%xmm3, %xmm1
	movdqa	%xmm4, %xmm3
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm5
	pand	%xmm1, %xmm3
	pandn	%xmm2, %xmm5
	pand	%xmm1, %xmm2
	por	%xmm5, %xmm3
	movdqa	%xmm1, %xmm5
	pandn	%xmm4, %xmm5
	movdqa	%xmm0, %xmm4
	movdqa	%xmm3, %xmm1
	pcmpeqd	%xmm3, %xmm4
	psubq	%xmm0, %xmm1
	por	%xmm5, %xmm2
	pand	%xmm4, %xmm1
	movdqa	%xmm0, %xmm4
	pcmpgtd	%xmm3, %xmm4
	por	%xmm4, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm0
	pandn	%xmm3, %xmm4
	movdqa	%xmm2, %xmm3
	por	%xmm4, %xmm0
	movdqa	(%rcx,%rbx,8), %xmm4
	pcmpeqd	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	psubq	%xmm2, %xmm1
	pand	%xmm3, %xmm1
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm0, %xmm3
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm1, %xmm0
	movdqa	%xmm4, %xmm1
	pandn	%xmm2, %xmm3
	movdqa	(%rdx,%rbx,8), %xmm2
	por	%xmm3, %xmm0
	movdqa	%xmm2, %xmm3
	psubq	%xmm2, %xmm1
	movaps	%xmm0, 0(%r13,%rbx,8)
	movdqa	(%r9,%rbx,8), %xmm0
	pcmpeqd	%xmm4, %xmm3
	pand	%xmm3, %xmm1
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm4, %xmm3
	por	%xmm3, %xmm1
	movdqa	%xmm4, %xmm3
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm5
	pand	%xmm1, %xmm3
	pandn	%xmm2, %xmm5
	pand	%xmm1, %xmm2
	por	%xmm5, %xmm3
	movdqa	%xmm1, %xmm5
	pandn	%xmm4, %xmm5
	movdqa	%xmm0, %xmm4
	movdqa	%xmm3, %xmm1
	pcmpeqd	%xmm3, %xmm4
	psubq	%xmm0, %xmm1
	por	%xmm5, %xmm2
	pand	%xmm4, %xmm1
	movdqa	%xmm0, %xmm4
	pcmpgtd	%xmm3, %xmm4
	por	%xmm4, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm0
	pandn	%xmm3, %xmm4
	movdqa	%xmm2, %xmm3
	por	%xmm4, %xmm0
	movdqa	(%r8,%rbx,8), %xmm4
	pcmpeqd	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	psubq	%xmm2, %xmm1
	pand	%xmm3, %xmm1
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm0, %xmm3
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm1, %xmm0
	movdqa	%xmm4, %xmm1
	pandn	%xmm2, %xmm3
	movdqa	(%rsi,%rbx,8), %xmm2
	por	%xmm3, %xmm0
	movdqa	%xmm2, %xmm3
	psubq	%xmm2, %xmm1
	movaps	%xmm0, 64(%r13,%rbx,8)
	movdqa	(%rax,%rbx,8), %xmm0
	pcmpeqd	%xmm4, %xmm3
	pand	%xmm3, %xmm1
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm4, %xmm3
	por	%xmm3, %xmm1
	movdqa	%xmm4, %xmm3
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm5
	pand	%xmm1, %xmm3
	pandn	%xmm2, %xmm5
	pand	%xmm1, %xmm2
	por	%xmm5, %xmm3
	movdqa	%xmm1, %xmm5
	pandn	%xmm4, %xmm5
	movdqa	%xmm0, %xmm4
	movdqa	%xmm3, %xmm1
	pcmpeqd	%xmm3, %xmm4
	psubq	%xmm0, %xmm1
	por	%xmm5, %xmm2
	pand	%xmm4, %xmm1
	movdqa	%xmm0, %xmm4
	pcmpgtd	%xmm3, %xmm4
	por	%xmm4, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm0
	pandn	%xmm3, %xmm4
	movdqa	%xmm2, %xmm3
	por	%xmm4, %xmm0
	pcmpeqd	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	psubq	%xmm2, %xmm1
	pand	%xmm3, %xmm1
	movdqa	%xmm2, %xmm3
	pcmpgtd	%xmm0, %xmm3
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm1, %xmm0
	pandn	%xmm2, %xmm3
	por	%xmm3, %xmm0
	movaps	%xmm0, 128(%r13,%rbx,8)
	addq	$2, %rbx
	cmpq	$8, %rbx
	jne	.L1250
	movdqa	16(%r13), %xmm0
	movdqa	0(%r13), %xmm1
	movddup	0(%r13), %xmm2
	leaq	192(%r13), %r14
	pxor	%xmm2, %xmm1
	pxor	%xmm2, %xmm0
	por	%xmm1, %xmm0
	movdqa	32(%r13), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	48(%r13), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	64(%r13), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	80(%r13), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	96(%r13), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	112(%r13), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	128(%r13), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	144(%r13), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	160(%r13), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	movdqa	176(%r13), %xmm1
	pxor	%xmm2, %xmm1
	por	%xmm1, %xmm0
	pxor	%xmm1, %xmm1
	pcmpeqd	%xmm1, %xmm0
	pshufd	$177, %xmm0, %xmm1
	pand	%xmm1, %xmm0
	movmskpd	%xmm0, %eax
	cmpl	$3, %eax
	je	.L1251
	movdqa	.LC4(%rip), %xmm0
	movl	$2, %esi
	movq	%r13, %rdi
	movups	%xmm0, 192(%r13)
	movups	%xmm0, 208(%r13)
	movups	%xmm0, 224(%r13)
	movups	%xmm0, 240(%r13)
	movups	%xmm0, 256(%r13)
	call	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	pcmpeqd	%xmm0, %xmm0
	movddup	0(%r13), %xmm2
	movddup	184(%r13), %xmm1
	paddq	%xmm1, %xmm0
	pcmpeqd	%xmm2, %xmm0
	pshufd	$177, %xmm0, %xmm3
	pand	%xmm3, %xmm0
	movmskpd	%xmm0, %eax
	cmpl	$3, %eax
	jne	.L1253
	movq	-104(%rbp), %rdi
	leaq	-64(%rbp), %rdx
	movq	%r14, %rcx
	movdqa	%xmm2, %xmm0
	movq	-216(%rbp), %rsi
	call	_ZN3hwy7N_SSSE36detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L1236
.L1253:
	movq	96(%r13), %rax
	cmpq	%rax, 88(%r13)
	jne	.L1349
	cmpq	80(%r13), %rax
	jne	.L1292
	cmpq	72(%r13), %rax
	jne	.L1350
	cmpq	64(%r13), %rax
	jne	.L1351
	cmpq	56(%r13), %rax
	jne	.L1352
	cmpq	48(%r13), %rax
	jne	.L1353
	cmpq	40(%r13), %rax
	jne	.L1354
	cmpq	32(%r13), %rax
	jne	.L1355
	cmpq	24(%r13), %rax
	jne	.L1356
	cmpq	16(%r13), %rax
	jne	.L1357
	cmpq	8(%r13), %rax
	jne	.L1358
	xorl	%ebx, %ebx
	movl	$1, %edx
	cmpq	%rax, 0(%r13)
	jne	.L1295
.L1294:
	movq	%rax, %xmm2
	punpcklqdq	%xmm2, %xmm2
.L1461:
	movl	$1, -268(%rbp)
.L1290:
	cmpq	$0, -224(%rbp)
	je	.L1465
	movq	-216(%rbp), %rax
	movq	-104(%rbp), %r12
	movaps	%xmm2, -80(%rbp)
	leaq	-2(%rax), %r15
	movdqu	(%r12,%r15,8), %xmm6
	movq	%r15, %rbx
	movq	%r15, %rax
	andl	$7, %ebx
	movaps	%xmm6, -256(%rbp)
	andl	$6, %eax
	je	.L1359
	movdqu	(%r12), %xmm4
	movdqa	%xmm2, %xmm1
	movaps	%xmm2, -128(%rbp)
	movdqa	%xmm4, %xmm0
	psubq	%xmm4, %xmm1
	movaps	%xmm4, -144(%rbp)
	pcmpeqd	%xmm2, %xmm0
	pand	%xmm0, %xmm1
	movdqa	%xmm4, %xmm0
	pcmpgtd	%xmm2, %xmm0
	por	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm0
	pshufd	$245, %xmm1, %xmm1
	pxor	%xmm1, %xmm0
	movaps	%xmm1, -96(%rbp)
	movmskpd	%xmm0, %r14d
	movq	%r14, %rdi
	salq	$4, %r14
	call	__popcountdi2@PLT
	movdqa	.LC1(%rip), %xmm7
	movdqa	.LC0(%rip), %xmm2
	leaq	_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %rsi
	cltq
	movdqa	-144(%rbp), %xmm4
	movdqa	-96(%rbp), %xmm1
	movq	%rsi, -184(%rbp)
	movq	%rax, %xmm6
	movdqa	%xmm2, %xmm0
	movaps	%xmm2, -240(%rbp)
	movdqa	-128(%rbp), %xmm2
	movddup	%xmm6, %xmm3
	movdqa	%xmm7, %xmm6
	movdqa	%xmm4, %xmm5
	movaps	%xmm7, -208(%rbp)
	pcmpeqd	%xmm3, %xmm6
	psubq	%xmm3, %xmm0
	pshufb	(%rsi,%r14), %xmm5
	pcmpgtd	%xmm7, %xmm3
	pand	%xmm6, %xmm0
	por	%xmm3, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rdx
	testq	%rdx, %rdx
	je	.L1300
	movq	%xmm5, (%r12)
.L1300:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rdx
	testq	%rdx, %rdx
	je	.L1301
	movq	-104(%rbp), %rsi
	movhps	%xmm5, 8(%rsi)
.L1301:
	movmskpd	%xmm1, %ecx
	movq	-104(%rbp), %rsi
	movaps	%xmm4, -144(%rbp)
	movq	%rcx, %rdi
	movq	%rcx, -96(%rbp)
	movaps	%xmm2, -128(%rbp)
	leaq	(%rsi,%rax,8), %r12
	call	__popcountdi2@PLT
	movq	-96(%rbp), %rcx
	movdqa	-128(%rbp), %xmm2
	movdqa	-144(%rbp), %xmm4
	movslq	%eax, %r14
	movq	-184(%rbp), %rax
	salq	$4, %rcx
	testb	$4, %r15b
	movdqa	%xmm4, %xmm0
	pshufb	(%rax,%rcx), %xmm0
	movups	%xmm0, 0(%r13)
	je	.L1302
	movq	-104(%rbp), %rsi
	movdqa	-80(%rbp), %xmm6
	movdqa	%xmm2, %xmm1
	movaps	%xmm2, -160(%rbp)
	movdqu	16(%rsi), %xmm3
	movdqa	%xmm6, %xmm0
	pcmpeqd	%xmm3, %xmm0
	psubq	%xmm3, %xmm1
	movaps	%xmm3, -144(%rbp)
	pand	%xmm0, %xmm1
	movdqa	%xmm3, %xmm0
	pcmpgtd	%xmm6, %xmm0
	por	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm0
	pshufd	$245, %xmm1, %xmm1
	pxor	%xmm1, %xmm0
	movaps	%xmm1, -128(%rbp)
	movmskpd	%xmm0, %ecx
	movq	%rcx, %rdi
	movq	%rcx, -96(%rbp)
	call	__popcountdi2@PLT
	movq	-96(%rbp), %rcx
	movdqa	-208(%rbp), %xmm7
	cltq
	movdqa	-240(%rbp), %xmm0
	movdqa	-144(%rbp), %xmm3
	movq	%rax, %xmm6
	movq	-184(%rbp), %rsi
	salq	$4, %rcx
	movdqa	-128(%rbp), %xmm1
	movddup	%xmm6, %xmm4
	movdqa	%xmm7, %xmm6
	movdqa	%xmm3, %xmm5
	movdqa	-160(%rbp), %xmm2
	pcmpeqd	%xmm4, %xmm6
	psubq	%xmm4, %xmm0
	pshufb	(%rsi,%rcx), %xmm5
	pcmpgtd	%xmm7, %xmm4
	pand	%xmm6, %xmm0
	por	%xmm4, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rcx
	testq	%rcx, %rcx
	je	.L1303
	movq	%xmm5, (%r12)
.L1303:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rcx
	testq	%rcx, %rcx
	je	.L1304
	movhps	%xmm5, 8(%r12)
.L1304:
	movmskpd	%xmm1, %ecx
	movaps	%xmm3, -144(%rbp)
	leaq	(%r12,%rax,8), %r12
	movq	%rcx, %rdi
	movq	%rcx, -96(%rbp)
	movaps	%xmm2, -128(%rbp)
	call	__popcountdi2@PLT
	movq	-96(%rbp), %rcx
	movdqa	-128(%rbp), %xmm2
	movdqa	-144(%rbp), %xmm3
	movq	-184(%rbp), %rsi
	cltq
	salq	$4, %rcx
	movdqa	%xmm3, %xmm0
	pshufb	(%rsi,%rcx), %xmm0
	movups	%xmm0, 0(%r13,%r14,8)
	addq	%rax, %r14
	cmpq	$5, %rbx
	jbe	.L1302
	movq	-104(%rbp), %rax
	movdqa	-80(%rbp), %xmm6
	movdqa	%xmm2, %xmm1
	movaps	%xmm2, -160(%rbp)
	movdqu	32(%rax), %xmm3
	movdqa	%xmm6, %xmm0
	pcmpeqd	%xmm3, %xmm0
	psubq	%xmm3, %xmm1
	movaps	%xmm3, -144(%rbp)
	pand	%xmm0, %xmm1
	movdqa	%xmm3, %xmm0
	pcmpgtd	%xmm6, %xmm0
	por	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm0
	pshufd	$245, %xmm1, %xmm1
	pxor	%xmm1, %xmm0
	movaps	%xmm1, -128(%rbp)
	movmskpd	%xmm0, %ecx
	movq	%rcx, %rdi
	movq	%rcx, -96(%rbp)
	call	__popcountdi2@PLT
	movq	-96(%rbp), %rcx
	movdqa	-208(%rbp), %xmm7
	cltq
	movdqa	-240(%rbp), %xmm0
	movdqa	-144(%rbp), %xmm3
	movq	%rax, %xmm5
	movq	-184(%rbp), %rsi
	salq	$4, %rcx
	movdqa	-128(%rbp), %xmm1
	movddup	%xmm5, %xmm4
	movdqa	%xmm7, %xmm5
	movdqa	%xmm3, %xmm6
	movdqa	-160(%rbp), %xmm2
	pcmpeqd	%xmm4, %xmm5
	psubq	%xmm4, %xmm0
	pshufb	(%rsi,%rcx), %xmm6
	pcmpgtd	%xmm7, %xmm4
	pand	%xmm5, %xmm0
	por	%xmm4, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rcx
	testq	%rcx, %rcx
	je	.L1305
	movq	%xmm6, (%r12)
.L1305:
	movhlps	%xmm0, %xmm5
	movq	%xmm5, %rcx
	testq	%rcx, %rcx
	je	.L1306
	movhps	%xmm6, 8(%r12)
.L1306:
	movmskpd	%xmm1, %ecx
	movaps	%xmm3, -144(%rbp)
	leaq	(%r12,%rax,8), %r12
	movq	%rcx, %rdi
	movq	%rcx, -96(%rbp)
	movaps	%xmm2, -128(%rbp)
	call	__popcountdi2@PLT
	movq	-96(%rbp), %rcx
	movdqa	-128(%rbp), %xmm2
	movdqa	-144(%rbp), %xmm3
	movq	-184(%rbp), %rsi
	cltq
	salq	$4, %rcx
	movdqa	%xmm3, %xmm0
	pshufb	(%rsi,%rcx), %xmm0
	movups	%xmm0, 0(%r13,%r14,8)
	addq	%rax, %r14
.L1302:
	leaq	-2(%rbx), %rax
	leaq	1(%rbx), %rcx
	andq	$-2, %rax
	leaq	0(,%r14,8), %r8
	addq	$2, %rax
	cmpq	$2, %rcx
	movl	$2, %ecx
	cmovbe	%rcx, %rax
.L1299:
	cmpq	%rax, %rbx
	je	.L1307
	subq	%rax, %rbx
	movdqa	-208(%rbp), %xmm4
	movq	-104(%rbp), %rsi
	movq	%r8, -176(%rbp)
	movq	%rbx, %xmm0
	movdqa	-240(%rbp), %xmm1
	movdqa	-80(%rbp), %xmm6
	movaps	%xmm2, -160(%rbp)
	punpcklqdq	%xmm0, %xmm0
	movdqa	%xmm4, %xmm3
	movdqu	(%rsi,%rax,8), %xmm5
	pcmpeqd	%xmm0, %xmm3
	psubq	%xmm0, %xmm1
	movdqa	%xmm6, %xmm7
	pcmpgtd	%xmm4, %xmm0
	pcmpeqd	%xmm5, %xmm7
	movaps	%xmm5, -144(%rbp)
	pand	%xmm3, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm4
	movdqa	%xmm2, %xmm0
	psubq	%xmm5, %xmm0
	movaps	%xmm4, -96(%rbp)
	pand	%xmm7, %xmm0
	movdqa	%xmm5, %xmm7
	pcmpgtd	%xmm6, %xmm7
	por	%xmm7, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm6
	movaps	%xmm0, -128(%rbp)
	pandn	%xmm4, %xmm6
	movmskpd	%xmm6, %ebx
	movq	%rbx, %rdi
	salq	$4, %rbx
	call	__popcountdi2@PLT
	movdqa	-208(%rbp), %xmm4
	movdqa	-240(%rbp), %xmm1
	cltq
	movdqa	-144(%rbp), %xmm5
	movq	-184(%rbp), %rsi
	movq	%rax, %xmm6
	movdqa	-128(%rbp), %xmm0
	movdqa	-160(%rbp), %xmm2
	movddup	%xmm6, %xmm3
	movdqa	%xmm4, %xmm6
	movdqa	%xmm5, %xmm7
	movq	-176(%rbp), %r8
	pcmpeqd	%xmm3, %xmm6
	psubq	%xmm3, %xmm1
	pshufb	(%rsi,%rbx), %xmm7
	pcmpgtd	%xmm4, %xmm3
	movdqa	-96(%rbp), %xmm4
	pand	%xmm6, %xmm1
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movq	%xmm1, %rcx
	testq	%rcx, %rcx
	je	.L1308
	movq	%xmm7, (%r12)
.L1308:
	movhlps	%xmm1, %xmm6
	movq	%xmm6, %rcx
	testq	%rcx, %rcx
	je	.L1309
	movhps	%xmm7, 8(%r12)
.L1309:
	pand	%xmm4, %xmm0
	movq	%r8, -128(%rbp)
	leaq	(%r12,%rax,8), %r12
	movmskpd	%xmm0, %ebx
	movaps	%xmm5, -144(%rbp)
	movq	%rbx, %rdi
	movaps	%xmm2, -96(%rbp)
	salq	$4, %rbx
	call	__popcountdi2@PLT
	movq	-184(%rbp), %rsi
	movq	-128(%rbp), %r8
	movdqa	-144(%rbp), %xmm5
	cltq
	movdqa	-96(%rbp), %xmm2
	addq	%rax, %r14
	movdqa	%xmm5, %xmm0
	pshufb	(%rsi,%rbx), %xmm0
	movups	%xmm0, 0(%r13,%r8)
	leaq	0(,%r14,8), %r8
.L1307:
	movq	-104(%rbp), %rax
	movq	%r15, %r9
	subq	%r14, %r9
	leaq	(%rax,%r9,8), %rax
	cmpl	$8, %r8d
	jnb	.L1310
	testl	%r8d, %r8d
	jne	.L1466
.L1311:
	cmpl	$8, %r8d
	jnb	.L1314
.L1470:
	testl	%r8d, %r8d
	jne	.L1467
.L1315:
	movq	%r12, %rax
	subq	-104(%rbp), %rax
	sarq	$3, %rax
	subq	%rax, %r15
	subq	%rax, %r9
	movq	%rax, -288(%rbp)
	movq	%r15, -280(%rbp)
	movq	%r9, %r14
	leaq	(%r12,%r9,8), %rax
	je	.L1360
	movdqu	(%r12), %xmm6
	leaq	64(%r12), %rcx
	leaq	-64(%rax), %rsi
	movaps	%xmm6, -304(%rbp)
	movdqu	16(%r12), %xmm6
	movaps	%xmm6, -320(%rbp)
	movdqu	32(%r12), %xmm6
	movaps	%xmm6, -336(%rbp)
	movdqu	48(%r12), %xmm6
	movaps	%xmm6, -352(%rbp)
	movdqu	-64(%rax), %xmm6
	movaps	%xmm6, -368(%rbp)
	movdqu	-48(%rax), %xmm6
	movaps	%xmm6, -384(%rbp)
	movdqu	-32(%rax), %xmm6
	movaps	%xmm6, -400(%rbp)
	movdqu	-16(%rax), %xmm6
	movaps	%xmm6, -416(%rbp)
	cmpq	%rsi, %rcx
	je	.L1361
	movq	%r13, -424(%rbp)
	xorl	%ebx, %ebx
	movl	$2, %r15d
	movq	%rsi, %r13
	movaps	%xmm2, -96(%rbp)
	jmp	.L1322
	.p2align 4,,10
	.p2align 3
.L1469:
	movdqu	-64(%r13), %xmm5
	movdqu	-48(%r13), %xmm4
	prefetcht0	-256(%r13)
	subq	$64, %r13
	movdqu	32(%r13), %xmm3
	movdqu	48(%r13), %xmm1
.L1321:
	movdqa	-80(%rbp), %xmm7
	movdqa	-96(%rbp), %xmm0
	movq	%rcx, -112(%rbp)
	movaps	%xmm1, -176(%rbp)
	movdqa	%xmm7, %xmm2
	psubq	%xmm5, %xmm0
	movaps	%xmm3, -160(%rbp)
	pcmpeqd	%xmm5, %xmm2
	movaps	%xmm4, -144(%rbp)
	pand	%xmm2, %xmm0
	movdqa	%xmm5, %xmm2
	pcmpgtd	%xmm7, %xmm2
	pshufd	$78, %xmm5, %xmm7
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm6
	movdqa	%xmm0, %xmm2
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm6
	punpckhqdq	%xmm0, %xmm2
	pandn	%xmm6, %xmm2
	movdqa	%xmm2, %xmm6
	pand	%xmm7, %xmm2
	pandn	%xmm5, %xmm6
	por	%xmm6, %xmm2
	movaps	%xmm2, -128(%rbp)
	call	__popcountdi2@PLT
	movdqa	-128(%rbp), %xmm2
	movdqa	-80(%rbp), %xmm7
	leaq	-2(%r14,%rbx), %rdx
	movdqa	-144(%rbp), %xmm4
	movdqa	-96(%rbp), %xmm0
	cltq
	movups	%xmm2, (%r12,%rbx,8)
	addq	$2, %rbx
	movups	%xmm2, (%r12,%rdx,8)
	movdqa	%xmm7, %xmm2
	psubq	%xmm4, %xmm0
	pshufd	$78, %xmm4, %xmm6
	pcmpeqd	%xmm4, %xmm2
	subq	%rax, %rbx
	pand	%xmm2, %xmm0
	movdqa	%xmm4, %xmm2
	pcmpgtd	%xmm7, %xmm2
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm5
	movdqa	%xmm0, %xmm2
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm5
	punpckhqdq	%xmm0, %xmm2
	pandn	%xmm5, %xmm2
	movdqa	%xmm2, %xmm5
	pand	%xmm6, %xmm2
	pandn	%xmm4, %xmm5
	por	%xmm5, %xmm2
	movaps	%xmm2, -128(%rbp)
	call	__popcountdi2@PLT
	movdqa	-128(%rbp), %xmm2
	movdqa	-80(%rbp), %xmm7
	leaq	-4(%r14,%rbx), %rdx
	movdqa	-160(%rbp), %xmm3
	movdqa	-96(%rbp), %xmm0
	cltq
	movups	%xmm2, (%r12,%rbx,8)
	movups	%xmm2, (%r12,%rdx,8)
	movdqa	%xmm7, %xmm2
	psubq	%xmm3, %xmm0
	movq	%r15, %rdx
	pcmpeqd	%xmm3, %xmm2
	pshufd	$78, %xmm3, %xmm5
	subq	%rax, %rdx
	addq	%rdx, %rbx
	pand	%xmm2, %xmm0
	movdqa	%xmm3, %xmm2
	pcmpgtd	%xmm7, %xmm2
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm4
	movdqa	%xmm0, %xmm2
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm4
	punpckhqdq	%xmm0, %xmm2
	pandn	%xmm4, %xmm2
	movdqa	%xmm2, %xmm4
	pand	%xmm5, %xmm2
	pandn	%xmm3, %xmm4
	por	%xmm4, %xmm2
	movaps	%xmm2, -128(%rbp)
	call	__popcountdi2@PLT
	movdqa	-128(%rbp), %xmm2
	movdqa	-80(%rbp), %xmm7
	movq	%r15, %rdx
	movdqa	-176(%rbp), %xmm1
	leaq	-6(%r14,%rbx), %rdi
	movdqa	-96(%rbp), %xmm0
	cltq
	movups	%xmm2, (%r12,%rbx,8)
	subq	%rax, %rdx
	subq	$8, %r14
	movups	%xmm2, (%r12,%rdi,8)
	movdqa	%xmm7, %xmm2
	psubq	%xmm1, %xmm0
	pshufd	$78, %xmm1, %xmm4
	pcmpeqd	%xmm1, %xmm2
	addq	%rdx, %rbx
	pand	%xmm2, %xmm0
	movdqa	%xmm1, %xmm2
	pcmpgtd	%xmm7, %xmm2
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm2
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm2
	pandn	%xmm3, %xmm2
	movdqa	%xmm2, %xmm3
	pandn	%xmm1, %xmm3
	movdqa	%xmm2, %xmm1
	pand	%xmm4, %xmm1
	por	%xmm3, %xmm1
	movaps	%xmm1, -128(%rbp)
	call	__popcountdi2@PLT
	movdqa	-128(%rbp), %xmm1
	leaq	(%rbx,%r14), %rdx
	movq	-112(%rbp), %rcx
	cltq
	movups	%xmm1, (%r12,%rbx,8)
	movups	%xmm1, (%r12,%rdx,8)
	movq	%r15, %rdx
	subq	%rax, %rdx
	addq	%rdx, %rbx
	cmpq	%r13, %rcx
	je	.L1468
.L1322:
	movq	%rcx, %rax
	subq	%r12, %rax
	sarq	$3, %rax
	subq	%rbx, %rax
	cmpq	$8, %rax
	ja	.L1469
	movdqu	(%rcx), %xmm5
	movdqu	16(%rcx), %xmm4
	prefetcht0	256(%rcx)
	addq	$64, %rcx
	movdqu	-32(%rcx), %xmm3
	movdqu	-16(%rcx), %xmm1
	jmp	.L1321
	.p2align 4,,10
	.p2align 3
.L1296:
	cmpq	$23, %rdx
	je	.L1460
.L1295:
	movq	%rdx, %rcx
	addq	$1, %rdx
	cmpq	%rax, 0(%r13,%rdx,8)
	je	.L1296
	movl	$12, %edx
	subq	$11, %rcx
	subq	%rbx, %rdx
	cmpq	%rdx, %rcx
	jb	.L1294
.L1460:
	movq	0(%r13,%rbx,8), %rax
	jmp	.L1294
	.p2align 4,,10
	.p2align 3
.L1464:
	movq	-216(%rbp), %rsi
	movl	$8, %edx
	subq	%rax, %rdx
	leaq	-8(%rax,%rsi), %r14
	leaq	(%rdi,%rdx,8), %r15
	movq	%r8, %rax
	jmp	.L1248
	.p2align 4,,10
	.p2align 3
.L1468:
	movdqa	-96(%rbp), %xmm2
	movq	-424(%rbp), %r13
	leaq	(%r12,%rbx,8), %rcx
	leaq	(%r14,%rbx), %r15
	addq	$2, %rbx
.L1319:
	movdqa	-304(%rbp), %xmm7
	movdqa	-80(%rbp), %xmm6
	movdqa	%xmm2, %xmm0
	movq	%rcx, -144(%rbp)
	movaps	%xmm2, -128(%rbp)
	movdqa	%xmm7, %xmm1
	psubq	%xmm7, %xmm0
	pshufd	$78, %xmm7, %xmm4
	pcmpeqd	%xmm6, %xmm1
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -96(%rbp)
	call	__popcountdi2@PLT
	movdqa	-96(%rbp), %xmm1
	movdqa	-80(%rbp), %xmm6
	movdqa	-320(%rbp), %xmm7
	movq	-144(%rbp), %rcx
	cltq
	movdqa	-128(%rbp), %xmm2
	subq	%rax, %rbx
	movups	%xmm1, (%rcx)
	pshufd	$78, %xmm7, %xmm4
	movups	%xmm1, -16(%r12,%r15,8)
	movdqa	%xmm7, %xmm1
	movdqa	%xmm2, %xmm0
	pcmpeqd	%xmm6, %xmm1
	psubq	%xmm7, %xmm0
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -96(%rbp)
	call	__popcountdi2@PLT
	movdqa	-96(%rbp), %xmm1
	movdqa	-80(%rbp), %xmm6
	leaq	-4(%r14,%rbx), %rcx
	movdqa	-336(%rbp), %xmm7
	movdqa	-128(%rbp), %xmm2
	cltq
	movups	%xmm1, (%r12,%rbx,8)
	subq	%rax, %rbx
	movups	%xmm1, (%r12,%rcx,8)
	movdqa	%xmm7, %xmm1
	movdqa	%xmm2, %xmm0
	leaq	2(%rbx), %r15
	pcmpeqd	%xmm6, %xmm1
	psubq	%xmm7, %xmm0
	pshufd	$78, %xmm7, %xmm4
	movl	$2, %ebx
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -96(%rbp)
	call	__popcountdi2@PLT
	movdqa	-96(%rbp), %xmm1
	movdqa	-80(%rbp), %xmm6
	leaq	-6(%r14,%r15), %rcx
	movdqa	-352(%rbp), %xmm7
	movdqa	-128(%rbp), %xmm2
	cltq
	movups	%xmm1, (%r12,%r15,8)
	movups	%xmm1, (%r12,%rcx,8)
	movdqa	%xmm7, %xmm1
	movdqa	%xmm2, %xmm0
	movq	%rbx, %rcx
	pcmpeqd	%xmm6, %xmm1
	psubq	%xmm7, %xmm0
	pshufd	$78, %xmm7, %xmm4
	subq	%rax, %rcx
	addq	%rcx, %r15
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -96(%rbp)
	call	__popcountdi2@PLT
	movdqa	-96(%rbp), %xmm1
	movdqa	-80(%rbp), %xmm6
	leaq	-8(%r14,%r15), %rcx
	movdqa	-368(%rbp), %xmm7
	movdqa	-128(%rbp), %xmm2
	cltq
	movups	%xmm1, (%r12,%r15,8)
	movups	%xmm1, (%r12,%rcx,8)
	movdqa	%xmm7, %xmm1
	movdqa	%xmm2, %xmm0
	movq	%rbx, %rcx
	pcmpeqd	%xmm6, %xmm1
	psubq	%xmm7, %xmm0
	pshufd	$78, %xmm7, %xmm4
	subq	%rax, %rcx
	addq	%rcx, %r15
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -96(%rbp)
	call	__popcountdi2@PLT
	movdqa	-96(%rbp), %xmm1
	movdqa	-80(%rbp), %xmm6
	leaq	-10(%r14,%r15), %rcx
	movdqa	-384(%rbp), %xmm7
	movdqa	-128(%rbp), %xmm2
	cltq
	movups	%xmm1, (%r12,%r15,8)
	movups	%xmm1, (%r12,%rcx,8)
	movdqa	%xmm7, %xmm1
	movdqa	%xmm2, %xmm0
	movq	%rbx, %rcx
	pcmpeqd	%xmm6, %xmm1
	psubq	%xmm7, %xmm0
	pshufd	$78, %xmm7, %xmm4
	subq	%rax, %rcx
	addq	%rcx, %r15
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -96(%rbp)
	call	__popcountdi2@PLT
	movdqa	-96(%rbp), %xmm1
	leaq	-12(%r14,%r15), %rcx
	cltq
	movups	%xmm1, (%r12,%r15,8)
	movups	%xmm1, (%r12,%rcx,8)
	movdqa	-128(%rbp), %xmm2
	movdqa	-80(%rbp), %xmm6
	movq	%rbx, %rcx
	movdqa	-400(%rbp), %xmm7
	subq	%rax, %rcx
	movdqa	%xmm2, %xmm0
	addq	%rcx, %r15
	movdqa	%xmm7, %xmm1
	psubq	%xmm7, %xmm0
	pshufd	$78, %xmm7, %xmm4
	pcmpeqd	%xmm6, %xmm1
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -96(%rbp)
	call	__popcountdi2@PLT
	movdqa	-96(%rbp), %xmm1
	leaq	-14(%r14,%r15), %rcx
	movdqa	-416(%rbp), %xmm7
	movdqa	-80(%rbp), %xmm6
	movdqa	-128(%rbp), %xmm2
	cltq
	movups	%xmm1, (%r12,%r15,8)
	pshufd	$78, %xmm7, %xmm4
	movups	%xmm1, (%r12,%rcx,8)
	movdqa	%xmm7, %xmm1
	movdqa	%xmm2, %xmm0
	movq	%rbx, %rcx
	pcmpeqd	%xmm6, %xmm1
	psubq	%xmm7, %xmm0
	subq	%rax, %rcx
	addq	%rcx, %r15
	pand	%xmm1, %xmm0
	movdqa	%xmm7, %xmm1
	pcmpgtd	%xmm6, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm3
	movdqa	%xmm0, %xmm1
	movmskpd	%xmm0, %edi
	punpcklqdq	%xmm0, %xmm3
	punpckhqdq	%xmm0, %xmm1
	pandn	%xmm3, %xmm1
	movdqa	%xmm1, %xmm3
	pand	%xmm4, %xmm1
	pandn	%xmm7, %xmm3
	por	%xmm3, %xmm1
	movaps	%xmm1, -96(%rbp)
	call	__popcountdi2@PLT
	movdqa	-96(%rbp), %xmm1
	leaq	-16(%r14,%r15), %rcx
	movdqa	-128(%rbp), %xmm2
	cltq
	movups	%xmm1, (%r12,%r15,8)
	subq	%rax, %rbx
	movups	%xmm1, (%r12,%rcx,8)
	movq	-280(%rbp), %rcx
	leaq	(%rbx,%r15), %r14
	leaq	0(,%r14,8), %r15
	subq	%r14, %rcx
.L1318:
	movq	-280(%rbp), %rsi
	cmpq	$2, %rcx
	movdqa	-80(%rbp), %xmm7
	leaq	-16(,%rsi,8), %rax
	cmovnb	%r15, %rax
	movdqu	(%r12,%rax), %xmm6
	movups	%xmm6, (%r12,%rsi,8)
	movaps	%xmm6, -96(%rbp)
	movdqa	-256(%rbp), %xmm6
	movdqa	%xmm6, %xmm0
	psubq	%xmm6, %xmm2
	pcmpeqd	%xmm7, %xmm0
	movdqa	%xmm2, %xmm1
	pand	%xmm0, %xmm1
	movdqa	%xmm6, %xmm0
	pcmpgtd	%xmm7, %xmm0
	por	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm0
	pshufd	$245, %xmm1, %xmm1
	pxor	%xmm1, %xmm0
	movaps	%xmm1, -80(%rbp)
	movmskpd	%xmm0, %ebx
	movq	%rbx, %rdi
	salq	$4, %rbx
	call	__popcountdi2@PLT
	movdqa	-240(%rbp), %xmm0
	movdqa	-80(%rbp), %xmm1
	cltq
	movq	-184(%rbp), %rsi
	movdqa	-256(%rbp), %xmm4
	movq	%rax, %xmm6
	movddup	%xmm6, %xmm2
	movdqa	-208(%rbp), %xmm6
	pshufb	(%rsi,%rbx), %xmm4
	psubq	%xmm2, %xmm0
	movdqa	%xmm6, %xmm3
	pcmpeqd	%xmm2, %xmm3
	pcmpgtd	%xmm6, %xmm2
	pand	%xmm3, %xmm0
	por	%xmm2, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rcx
	testq	%rcx, %rcx
	je	.L1324
	movq	%xmm4, (%r12,%r15)
.L1324:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rcx
	testq	%rcx, %rcx
	je	.L1325
	movhps	%xmm4, 8(%r12,%r15)
.L1325:
	movmskpd	%xmm1, %r15d
	addq	%rax, %r14
	movq	%r15, %rdi
	salq	$4, %r15
	leaq	0(,%r14,8), %rbx
	call	__popcountdi2@PLT
	movdqa	-208(%rbp), %xmm6
	movdqa	-240(%rbp), %xmm0
	cltq
	movq	-184(%rbp), %rsi
	movdqa	-256(%rbp), %xmm2
	movq	%rax, %xmm1
	movdqa	%xmm6, %xmm3
	punpcklqdq	%xmm1, %xmm1
	pshufb	(%rsi,%r15), %xmm2
	pcmpeqd	%xmm1, %xmm3
	psubq	%xmm1, %xmm0
	pcmpgtd	%xmm6, %xmm1
	pand	%xmm3, %xmm0
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L1326
	movq	%xmm2, (%r12,%r14,8)
.L1326:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rax
	testq	%rax, %rax
	je	.L1327
	movhps	%xmm2, 8(%r12,%rbx)
.L1327:
	movq	-288(%rbp), %rbx
	addq	%r14, %rbx
	movq	-224(%rbp), %r14
	subq	$1, %r14
	cmpl	$2, -268(%rbp)
	je	.L1329
	movq	-192(%rbp), %r8
	movq	%r14, %r9
	movq	%r13, %rcx
	movq	%rbx, %rdx
	movq	-264(%rbp), %rsi
	movq	-104(%rbp), %rdi
	call	_ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	cmpl	$3, -268(%rbp)
	je	.L1236
.L1329:
	movq	-216(%rbp), %rdx
	movq	-104(%rbp), %rax
	movq	%r14, %r9
	movq	%r13, %rcx
	movq	-192(%rbp), %r8
	movq	-264(%rbp), %rsi
	subq	%rbx, %rdx
	leaq	(%rax,%rbx,8), %rdi
	call	_ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L1236:
	addq	$392, %rsp
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
.L1310:
	.cfi_restore_state
	movq	(%rax), %rcx
	leaq	8(%r12), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%r12)
	movl	%r8d, %ecx
	movq	-8(%rax,%rcx), %rsi
	movq	%rsi, -8(%r12,%rcx)
	movq	%r12, %rcx
	movq	%rax, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%r8d, %ecx
	shrl	$3, %ecx
	rep movsq
	cmpl	$8, %r8d
	jb	.L1470
.L1314:
	movq	0(%r13), %rcx
	leaq	8(%rax), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%rax)
	movl	%r8d, %ecx
	movq	-8(%r13,%rcx), %rsi
	movq	%rsi, -8(%rax,%rcx)
	subq	%rdi, %rax
	movq	%r13, %rsi
	leal	(%r8,%rax), %ecx
	subq	%rax, %rsi
	shrl	$3, %ecx
	rep movsq
	jmp	.L1315
.L1467:
	movzbl	0(%r13), %ecx
	movb	%cl, (%rax)
	jmp	.L1315
.L1466:
	movzbl	(%rax), %ecx
	movb	%cl, (%r12)
	jmp	.L1311
.L1463:
	cmpq	$1, %rdx
	jbe	.L1236
	leaq	256(%rdi), %rax
	cmpq	%rax, %rsi
	jb	.L1240
	movl	$2, %esi
	call	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	jmp	.L1236
.L1251:
	movq	%r10, %rax
	movl	$2, %edi
	movdqa	.LC1(%rip), %xmm7
	movdqa	%xmm2, %xmm5
	andl	$1, %eax
	subq	%rax, %rdi
	movq	-104(%rbp), %rax
	movaps	%xmm7, -208(%rbp)
	movdqu	(%rax), %xmm6
	movaps	%xmm6, -80(%rbp)
	movq	%rdi, %xmm6
	movdqa	-80(%rbp), %xmm0
	movddup	%xmm6, %xmm3
	movdqa	.LC0(%rip), %xmm6
	pcmpeqd	%xmm2, %xmm0
	movdqa	%xmm6, %xmm1
	movaps	%xmm6, -240(%rbp)
	movdqa	%xmm7, %xmm6
	pcmpeqd	%xmm3, %xmm6
	psubq	%xmm3, %xmm1
	pcmpgtd	%xmm7, %xmm3
	pshufd	$177, %xmm0, %xmm4
	pand	%xmm4, %xmm0
	pand	%xmm6, %xmm1
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	pandn	%xmm1, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1471
	movq	-104(%rbp), %rax
	pxor	%xmm1, %xmm1
	movq	-216(%rbp), %r8
	pxor	%xmm6, %xmm6
	movdqa	%xmm1, %xmm0
	leaq	256(%rax,%rdi,8), %rsi
	.p2align 4,,10
	.p2align 3
.L1257:
	movq	%rdi, %rcx
	leaq	32(%rdi), %rdi
	cmpq	%rdi, %r8
	jb	.L1472
	leaq	-256(%rsi), %rax
.L1256:
	movdqa	(%rax), %xmm3
	leaq	32(%rax), %rdx
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm0
	movdqa	16(%rax), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm1
	movdqa	32(%rax), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm0
	movdqa	48(%rax), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm1
	movdqa	64(%rax), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm0
	movdqa	80(%rax), %xmm3
	leaq	96(%rdx), %rax
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm1
	movdqa	64(%rdx), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm0
	movdqa	80(%rdx), %xmm3
	pxor	%xmm2, %xmm3
	por	%xmm3, %xmm1
	cmpq	%rsi, %rax
	jne	.L1256
	movdqa	%xmm0, %xmm3
	leaq	352(%rdx), %rsi
	por	%xmm1, %xmm3
	pcmpeqd	%xmm6, %xmm3
	pshufd	$177, %xmm3, %xmm4
	pand	%xmm4, %xmm3
	movmskpd	%xmm3, %eax
	cmpl	$3, %eax
	je	.L1257
	movq	-104(%rbp), %rax
	movdqa	%xmm5, %xmm0
	pcmpeqd	%xmm3, %xmm3
	movq	-104(%rbp), %rdx
	pcmpeqd	(%rax,%rcx,8), %xmm0
	pshufd	$177, %xmm0, %xmm1
	pand	%xmm1, %xmm0
	pxor	%xmm3, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1259
	.p2align 4,,10
	.p2align 3
.L1258:
	addq	$2, %rcx
	movdqa	%xmm5, %xmm0
	pcmpeqd	(%rdx,%rcx,8), %xmm0
	pshufd	$177, %xmm0, %xmm1
	pand	%xmm1, %xmm0
	pxor	%xmm3, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1258
.L1259:
	rep bsfl	%eax, %eax
	cltq
	addq	%rcx, %rax
.L1255:
	movq	-104(%rbp), %rsi
	movdqa	%xmm5, %xmm3
	movdqa	%xmm2, %xmm0
	leaq	(%rsi,%rax,8), %rdi
	movq	(%rdi), %rbx
	movq	%rbx, %xmm6
	movddup	%xmm6, %xmm1
	pcmpeqd	%xmm1, %xmm3
	psubq	%xmm1, %xmm0
	movaps	%xmm1, -96(%rbp)
	pand	%xmm3, %xmm0
	movdqa	%xmm1, %xmm3
	pcmpgtd	%xmm5, %xmm3
	por	%xmm3, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movmskpd	%xmm0, %edx
	testl	%edx, %edx
	jne	.L1264
	movq	-216(%rbp), %rax
	xorl	%r15d, %r15d
	movaps	%xmm5, -80(%rbp)
	movq	%r12, -176(%rbp)
	leaq	-2(%rax), %r14
	movq	%rax, %r12
	movq	%r13, %rax
	movq	%rbx, -144(%rbp)
	movq	%r15, %r13
	movaps	%xmm1, -160(%rbp)
	movq	%rsi, %rbx
	movq	%rax, %r15
	movaps	%xmm2, -128(%rbp)
	jmp	.L1271
	.p2align 4,,10
	.p2align 3
.L1265:
	movdqa	-128(%rbp), %xmm6
	movmskpd	%xmm0, %edi
	movups	%xmm6, (%rbx,%r14,8)
	call	__popcountdi2@PLT
	cltq
	addq	%rax, %r13
	leaq	-2(%r14), %rax
	cmpq	%rax, %r12
	jbe	.L1473
	movq	%rax, %r14
.L1271:
	movdqu	(%rbx,%r14,8), %xmm0
	pcmpeqd	-96(%rbp), %xmm0
	pshufd	$177, %xmm0, %xmm1
	movdqa	%xmm0, %xmm3
	movdqu	(%rbx,%r14,8), %xmm0
	pand	%xmm1, %xmm3
	pcmpeqd	-80(%rbp), %xmm0
	pshufd	$177, %xmm0, %xmm1
	pand	%xmm1, %xmm0
	movdqa	%xmm3, %xmm1
	por	%xmm0, %xmm1
	movmskpd	%xmm1, %eax
	cmpl	$3, %eax
	je	.L1265
	pcmpeqd	%xmm4, %xmm4
	movq	%r15, %rax
	movq	%r13, %r15
	movq	-104(%rbp), %rcx
	pxor	%xmm4, %xmm0
	movq	%rax, %r13
	leaq	2(%r14), %rdx
	movdqa	-80(%rbp), %xmm5
	pandn	%xmm0, %xmm3
	movq	-144(%rbp), %rbx
	movdqa	-128(%rbp), %xmm2
	movmskpd	%xmm3, %eax
	movdqa	-160(%rbp), %xmm1
	rep bsfl	%eax, %eax
	cltq
	addq	%r14, %rax
	addq	$4, %r14
	movddup	(%rcx,%rax,8), %xmm3
	movq	-216(%rbp), %rax
	movaps	%xmm3, -64(%rbp)
	subq	%r15, %rax
	cmpq	%r14, %rax
	jb	.L1266
	.p2align 4,,10
	.p2align 3
.L1267:
	movups	%xmm1, -16(%rcx,%r14,8)
	movq	%r14, %rdx
	addq	$2, %r14
	cmpq	%rax, %r14
	jbe	.L1267
.L1266:
	movdqa	-208(%rbp), %xmm7
	subq	%rdx, %rax
	movdqa	-240(%rbp), %xmm4
	leaq	0(,%rdx,8), %rcx
	movq	%rax, %xmm0
	punpcklqdq	%xmm0, %xmm0
	movdqa	%xmm7, %xmm6
	pcmpeqd	%xmm0, %xmm6
	psubq	%xmm0, %xmm4
	pcmpgtd	%xmm7, %xmm0
	pand	%xmm6, %xmm4
	por	%xmm4, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L1275
	movq	-104(%rbp), %rax
	movq	%rbx, (%rax,%rdx,8)
.L1275:
	movhlps	%xmm0, %xmm6
	movq	%xmm6, %rax
	testq	%rax, %rax
	je	.L1270
	movq	-104(%rbp), %rax
	movq	%rbx, 8(%rax,%rcx)
.L1270:
	movdqa	%xmm5, %xmm0
	pcmpeqd	.LC5(%rip), %xmm0
	pshufd	$177, %xmm0, %xmm4
	pand	%xmm4, %xmm0
	movmskpd	%xmm0, %eax
	cmpl	$3, %eax
	je	.L1347
	movdqa	%xmm5, %xmm0
	pcmpeqd	.LC6(%rip), %xmm0
	pshufd	$177, %xmm0, %xmm4
	pand	%xmm4, %xmm0
	movmskpd	%xmm0, %eax
	movl	%eax, -268(%rbp)
	cmpl	$3, %eax
	je	.L1474
	movdqa	%xmm1, %xmm4
	movdqa	%xmm1, %xmm0
	movdqa	%xmm1, %xmm6
	pcmpeqd	%xmm3, %xmm4
	psubq	%xmm3, %xmm0
	pand	%xmm4, %xmm0
	movdqa	%xmm3, %xmm4
	pcmpgtd	%xmm1, %xmm4
	por	%xmm4, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movdqa	%xmm0, %xmm4
	pand	%xmm0, %xmm6
	pandn	%xmm3, %xmm4
	por	%xmm4, %xmm6
	movdqa	%xmm6, %xmm7
	movdqa	%xmm6, %xmm4
	pcmpeqd	%xmm5, %xmm7
	psubq	%xmm2, %xmm4
	pand	%xmm7, %xmm4
	movdqa	%xmm5, %xmm7
	pcmpgtd	%xmm6, %xmm7
	por	%xmm7, %xmm4
	pshufd	$245, %xmm4, %xmm4
	movmskpd	%xmm4, %eax
	testl	%eax, %eax
	jne	.L1475
	movdqa	%xmm2, %xmm0
	movl	$32, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L1281:
	movq	-104(%rbp), %rdi
	leaq	(%rcx,%rax,2), %rdx
	movdqa	%xmm0, %xmm1
	addq	$1, %rax
	movdqu	(%rdi,%rdx,8), %xmm3
	movdqa	%xmm3, %xmm4
	psubq	%xmm3, %xmm1
	pcmpeqd	%xmm0, %xmm4
	pand	%xmm4, %xmm1
	movdqa	%xmm3, %xmm4
	pcmpgtd	%xmm0, %xmm4
	por	%xmm4, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm0, %xmm1
	pandn	%xmm3, %xmm4
	por	%xmm4, %xmm1
	movdqa	%xmm1, %xmm0
	cmpq	$16, %rax
	jne	.L1281
	movdqa	%xmm1, %xmm3
	psubq	%xmm2, %xmm1
	pcmpeqd	%xmm5, %xmm3
	pand	%xmm3, %xmm1
	movdqa	%xmm5, %xmm3
	pcmpgtd	%xmm0, %xmm3
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movmskpd	%xmm1, %eax
	testl	%eax, %eax
	jne	.L1461
	leaq	32(%rsi), %rax
	cmpq	%rax, -216(%rbp)
	jb	.L1476
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L1281
.L1359:
	movdqa	.LC0(%rip), %xmm6
	leaq	_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices(%rip), %rsi
	xorl	%r8d, %r8d
	xorl	%r14d, %r14d
	movq	%rsi, -184(%rbp)
	movaps	%xmm6, -240(%rbp)
	movdqa	.LC1(%rip), %xmm6
	movaps	%xmm6, -208(%rbp)
	jmp	.L1299
.L1361:
	movq	%r9, %r15
	movq	%r12, %rcx
	movl	$2, %ebx
	jmp	.L1319
.L1360:
	movq	%r15, %rcx
	xorl	%r15d, %r15d
	jmp	.L1318
.L1465:
	movq	-216(%rbp), %rsi
	movq	-104(%rbp), %rdi
	leaq	-1(%rsi), %rbx
	movq	%rbx, %r13
	shrq	%r13
	.p2align 4,,10
	.p2align 3
.L1297:
	movq	%r13, %rdx
	call	_ZN3hwy7N_SSSE36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	subq	$1, %r13
	jnb	.L1297
	.p2align 4,,10
	.p2align 3
.L1298:
	movq	(%rdi,%rbx,8), %rdx
	movq	(%rdi), %rax
	movq	%rbx, %rsi
	movq	%rdx, (%rdi)
	xorl	%edx, %edx
	movq	%rax, (%rdi,%rbx,8)
	call	_ZN3hwy7N_SSSE36detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	subq	$1, %rbx
	jne	.L1298
	jmp	.L1236
.L1349:
	movl	$12, %edx
	movl	$11, %ebx
	jmp	.L1295
.L1350:
	movl	$9, %ebx
	movl	$10, %edx
	jmp	.L1295
.L1351:
	movl	$9, %edx
	jmp	.L1295
.L1352:
	movl	$8, %edx
	movl	$7, %ebx
	jmp	.L1295
.L1353:
	movl	$6, %ebx
	movl	$7, %edx
	jmp	.L1295
.L1472:
	movq	-104(%rbp), %rdi
	movq	-216(%rbp), %rsi
	pcmpeqd	%xmm3, %xmm3
	.p2align 4,,10
	.p2align 3
.L1261:
	movq	%rcx, %rdx
	addq	$2, %rcx
	cmpq	%rcx, %rsi
	jb	.L1477
	movdqa	%xmm5, %xmm0
	pcmpeqd	-16(%rdi,%rcx,8), %xmm0
	pshufd	$177, %xmm0, %xmm1
	pand	%xmm1, %xmm0
	pxor	%xmm3, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1261
.L1458:
	rep bsfl	%eax, %eax
	cltq
	addq	%rdx, %rax
	jmp	.L1255
.L1354:
	movl	$5, %ebx
	movl	$6, %edx
	jmp	.L1295
.L1355:
	movl	$4, %ebx
	movl	$5, %edx
	jmp	.L1295
.L1264:
	leaq	-64(%rbp), %rdx
	movq	%r13, %rcx
	movdqa	%xmm2, %xmm0
	movaps	%xmm1, -80(%rbp)
	movq	-216(%rbp), %rsi
	subq	%rax, %rsi
	call	_ZN3hwy7N_SSSE36detail22MaybePartitionTwoValueINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L1236
	movddup	0(%r13), %xmm2
	movdqa	-64(%rbp), %xmm3
	movdqa	-80(%rbp), %xmm1
	movdqa	%xmm2, %xmm5
	jmp	.L1270
.L1356:
	movl	$3, %ebx
	movl	$4, %edx
	jmp	.L1295
.L1357:
	movl	$2, %ebx
	movl	$3, %edx
	jmp	.L1295
.L1358:
	movl	$1, %ebx
	movl	$2, %edx
	jmp	.L1295
.L1473:
	movq	%r14, %xmm6
	movq	%r15, %rax
	movq	%r13, %r15
	movdqa	-240(%rbp), %xmm3
	movddup	%xmm6, %xmm0
	movq	%rax, %r13
	movdqa	-208(%rbp), %xmm6
	movq	-104(%rbp), %rax
	psubq	%xmm0, %xmm3
	movdqa	-80(%rbp), %xmm5
	movdqa	-160(%rbp), %xmm1
	movdqa	%xmm6, %xmm4
	movq	-216(%rbp), %rdx
	movq	-176(%rbp), %r12
	pcmpeqd	%xmm0, %xmm4
	pcmpgtd	%xmm6, %xmm0
	movdqu	(%rax), %xmm6
	movq	-144(%rbp), %rbx
	movdqa	-128(%rbp), %xmm2
	subq	%r15, %rdx
	movaps	%xmm6, -80(%rbp)
	movdqa	-80(%rbp), %xmm6
	pand	%xmm4, %xmm3
	pcmpeqd	%xmm5, %xmm6
	por	%xmm0, %xmm3
	movdqa	-80(%rbp), %xmm0
	pshufd	$245, %xmm3, %xmm3
	pcmpeqd	%xmm1, %xmm0
	pshufd	$177, %xmm6, %xmm4
	pand	%xmm3, %xmm4
	pand	%xmm6, %xmm4
	pshufd	$177, %xmm0, %xmm7
	pcmpeqd	%xmm6, %xmm6
	pand	%xmm7, %xmm0
	pxor	%xmm6, %xmm3
	por	%xmm3, %xmm0
	por	%xmm4, %xmm0
	movmskpd	%xmm0, %eax
	cmpl	$3, %eax
	jne	.L1478
	movmskpd	%xmm4, %edi
	movaps	%xmm2, -96(%rbp)
	movaps	%xmm1, -80(%rbp)
	movq	%rdx, -128(%rbp)
	call	__popcountdi2@PLT
	movq	-104(%rbp), %rsi
	movdqa	-96(%rbp), %xmm2
	movslq	%eax, %rcx
	movq	-128(%rbp), %rax
	movdqa	-80(%rbp), %xmm1
	movups	%xmm2, (%rsi)
	subq	%rcx, %rax
	cmpq	$1, %rax
	jbe	.L1336
	leaq	-2(%rax), %rcx
	movq	%rcx, %rdx
	shrq	%rdx
	salq	$4, %rdx
	leaq	16(%rsi,%rdx), %rdx
.L1278:
	movups	%xmm1, (%r12)
	addq	$16, %r12
	cmpq	%rdx, %r12
	jne	.L1278
	andq	$-2, %rcx
	movq	%rcx, %rdx
	addq	$2, %rdx
	leaq	0(,%rdx,8), %rcx
	subq	%rdx, %rax
.L1277:
	movaps	%xmm1, 0(%r13)
	testq	%rax, %rax
	je	.L1236
	movq	-104(%rbp), %rdi
	leaq	0(,%rax,8), %rdx
	movq	%r13, %rsi
	addq	%rcx, %rdi
	call	memcpy@PLT
	jmp	.L1236
.L1476:
	movq	-216(%rbp), %rcx
	movq	%rdi, %rdx
	jmp	.L1288
.L1289:
	movdqu	-16(%rdx,%rsi,8), %xmm1
	movdqa	%xmm1, %xmm3
	movdqa	%xmm1, %xmm0
	pcmpeqd	%xmm5, %xmm3
	psubq	%xmm2, %xmm0
	pand	%xmm3, %xmm0
	movdqa	%xmm5, %xmm3
	pcmpgtd	%xmm1, %xmm3
	por	%xmm3, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1461
.L1288:
	movq	%rsi, %rax
	addq	$2, %rsi
	cmpq	%rsi, %rcx
	jnb	.L1289
	movq	-216(%rbp), %rsi
	cmpq	%rax, %rsi
	je	.L1347
	movq	-104(%rbp), %rax
	movdqu	-16(%rax,%rsi,8), %xmm1
	movdqa	%xmm1, %xmm3
	movdqa	%xmm1, %xmm0
	pcmpeqd	%xmm5, %xmm3
	pcmpgtd	%xmm1, %xmm5
	psubq	%xmm2, %xmm0
	pand	%xmm3, %xmm0
	por	%xmm5, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movmskpd	%xmm0, %eax
	cmpl	$1, %eax
	movl	$1, %eax
	adcl	$0, %eax
	movl	%eax, -268(%rbp)
	jmp	.L1290
.L1292:
	movl	$11, %edx
	movl	$10, %ebx
	jmp	.L1295
.L1477:
	movq	-216(%rbp), %rax
	leaq	-2(%rax), %rdx
	movq	-104(%rbp), %rax
	movdqu	(%rax,%rdx,8), %xmm6
	movaps	%xmm6, -80(%rbp)
	movdqa	-80(%rbp), %xmm0
	pcmpeqd	%xmm5, %xmm0
	pshufd	$177, %xmm0, %xmm1
	pand	%xmm1, %xmm0
	pcmpeqd	%xmm1, %xmm1
	pxor	%xmm1, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	je	.L1236
	jmp	.L1458
.L1471:
	rep bsfl	%eax, %eax
	cltq
	jmp	.L1255
.L1240:
	movq	%rdx, %r10
	leaq	-2(%rdx), %rdx
	movq	-104(%rbp), %r11
	leaq	8(%rcx), %rdi
	movq	%rdx, %rbx
	andq	$-8, %rdi
	andq	$-2, %rdx
	movq	%r10, %r12
	shrq	%rbx
	movq	(%r11), %rax
	movq	%r11, %rsi
	addq	$1, %rbx
	salq	$4, %rbx
	movq	%rax, (%rcx)
	movl	%ebx, %r8d
	leaq	8(%r11,%r8), %r15
	leaq	8(%rcx,%r8), %r14
	movq	-16(%r15), %rax
	movq	%rax, -16(%r14)
	movq	%rcx, %rax
	subq	%rdi, %rax
	subq	%rax, %rsi
	leal	(%rbx,%rax), %ecx
	leaq	2(%rdx), %rax
	shrl	$3, %ecx
	rep movsq
	movq	%rax, -80(%rbp)
	movq	%r10, -216(%rbp)
	subq	%rax, %r12
	je	.L1241
	salq	$3, %rax
	leaq	0(,%r12,8), %rdx
	movq	%r8, -96(%rbp)
	leaq	(%r11,%rax), %rsi
	leaq	0(%r13,%rax), %rdi
	call	memcpy@PLT
	movq	-216(%rbp), %r10
	movl	$32, %ecx
	movq	-96(%rbp), %r8
	movl	%r10d, %eax
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
	movq	%rsi, %rax
	salq	$4, %rax
	addq	$2, %rax
	cmpq	%rax, %r10
	jnb	.L1479
.L1242:
	movdqa	.LC4(%rip), %xmm0
	movq	-216(%rbp), %rdx
.L1246:
	movups	%xmm0, 0(%r13,%rdx,8)
	addq	$2, %rdx
	cmpq	%rax, %rdx
	jb	.L1246
	movq	%r13, %rdi
	movq	%r8, -96(%rbp)
	call	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	movq	-104(%rbp), %rsi
	movq	0(%r13), %rax
	movq	%rax, (%rsi)
	movq	-96(%rbp), %r8
	leaq	8(%rsi), %rdi
	andq	$-8, %rdi
	movq	-8(%r13,%r8), %rax
	movq	%rax, -8(%rsi,%r8)
	movq	%rsi, %rax
	movq	%r13, %rsi
	subq	%rdi, %rax
	leal	(%rbx,%rax), %ecx
	subq	%rax, %rsi
	shrl	$3, %ecx
	rep movsq
	testq	%r12, %r12
	je	.L1236
.L1244:
	movq	-80(%rbp), %rax
	movq	-104(%rbp), %rdi
	leaq	0(,%r12,8), %rdx
	salq	$3, %rax
	addq	%rax, %rdi
	leaq	0(%r13,%rax), %rsi
	call	memcpy@PLT
	jmp	.L1236
.L1347:
	movl	$2, -268(%rbp)
	jmp	.L1290
.L1474:
	pcmpeqd	%xmm0, %xmm0
	paddq	%xmm0, %xmm2
	jmp	.L1290
.L1475:
	movdqa	%xmm0, %xmm4
	pand	%xmm3, %xmm0
	pandn	%xmm1, %xmm4
	movdqa	%xmm2, %xmm1
	por	%xmm4, %xmm0
	movdqa	%xmm0, %xmm3
	psubq	%xmm0, %xmm1
	pcmpeqd	%xmm5, %xmm3
	pcmpgtd	%xmm5, %xmm0
	pand	%xmm3, %xmm1
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1461
	movdqa	%xmm2, %xmm0
	movl	$32, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L1282:
	movq	-104(%rbp), %rdi
	leaq	(%rcx,%rax,2), %rdx
	movdqa	%xmm0, %xmm1
	addq	$1, %rax
	movdqu	(%rdi,%rdx,8), %xmm3
	movdqa	%xmm3, %xmm4
	psubq	%xmm3, %xmm1
	pcmpeqd	%xmm0, %xmm4
	pand	%xmm4, %xmm1
	movdqa	%xmm3, %xmm4
	pcmpgtd	%xmm0, %xmm4
	por	%xmm4, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movdqa	%xmm1, %xmm4
	pand	%xmm1, %xmm3
	pandn	%xmm0, %xmm4
	por	%xmm4, %xmm3
	movdqa	%xmm3, %xmm0
	cmpq	$16, %rax
	jne	.L1282
	pcmpeqd	%xmm5, %xmm3
	movdqa	%xmm2, %xmm1
	psubq	%xmm0, %xmm1
	pand	%xmm3, %xmm1
	movdqa	%xmm0, %xmm3
	pcmpgtd	%xmm5, %xmm3
	por	%xmm3, %xmm1
	pshufd	$245, %xmm1, %xmm1
	movmskpd	%xmm1, %eax
	testl	%eax, %eax
	jne	.L1461
	leaq	32(%rsi), %rax
	cmpq	%rax, -216(%rbp)
	jb	.L1480
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L1282
.L1241:
	movq	-216(%rbp), %rdi
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
	movq	%rsi, %rax
	salq	$4, %rax
	addq	$2, %rax
	cmpq	%rax, %rdi
	jb	.L1242
	movq	%r13, %rdi
	call	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	movq	-104(%rbp), %rsi
	movq	0(%r13), %rax
	movq	%rax, (%rsi)
	movq	-16(%r14), %rax
	leaq	8(%rsi), %rdi
	andq	$-8, %rdi
	movq	%rax, -16(%r15)
	movq	%rsi, %rax
	movq	%r13, %rsi
	subq	%rdi, %rax
	leal	(%rbx,%rax), %ecx
	subq	%rax, %rsi
	shrl	$3, %ecx
	rep movsq
	jmp	.L1236
	.p2align 4,,10
	.p2align 3
.L1336:
	xorl	%ecx, %ecx
	jmp	.L1277
.L1478:
	pxor	%xmm6, %xmm0
	movq	-104(%rbp), %rsi
	movmskpd	%xmm0, %eax
	rep bsfl	%eax, %eax
	cltq
	movddup	(%rsi,%rax,8), %xmm3
	leaq	2(%r14), %rax
	movaps	%xmm3, -64(%rbp)
	cmpq	%rdx, %rax
	ja	.L1273
.L1274:
	movq	-104(%rbp), %rsi
	movq	%rax, %r14
	movups	%xmm1, -16(%rsi,%rax,8)
	addq	$2, %rax
	cmpq	%rdx, %rax
	jbe	.L1274
.L1273:
	movq	%rdx, %rax
	movdqa	-208(%rbp), %xmm7
	movdqa	-240(%rbp), %xmm0
	leaq	0(,%r14,8), %rcx
	subq	%r14, %rax
	movq	%rax, %xmm6
	movddup	%xmm6, %xmm4
	movdqa	%xmm7, %xmm6
	pcmpeqd	%xmm4, %xmm6
	psubq	%xmm4, %xmm0
	pcmpgtd	%xmm7, %xmm4
	pand	%xmm6, %xmm0
	por	%xmm4, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movq	%xmm0, %rax
	testq	%rax, %rax
	je	.L1275
	movq	-104(%rbp), %rax
	movq	%rbx, (%rax,%r14,8)
	jmp	.L1275
.L1480:
	movq	%rdi, %rdx
	jmp	.L1284
.L1285:
	movdqu	-16(%rdx,%rsi,8), %xmm1
	movdqa	%xmm2, %xmm0
	movdqa	%xmm1, %xmm3
	psubq	%xmm1, %xmm0
	pcmpeqd	%xmm5, %xmm3
	pcmpgtd	%xmm5, %xmm1
	pand	%xmm3, %xmm0
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1461
.L1284:
	movq	%rsi, %rax
	addq	$2, %rsi
	cmpq	%rsi, -216(%rbp)
	jnb	.L1285
	movq	-216(%rbp), %rsi
	cmpq	%rax, %rsi
	je	.L1286
	movq	-104(%rbp), %rax
	movdqa	%xmm2, %xmm0
	movdqu	-16(%rax,%rsi,8), %xmm1
	movdqa	%xmm1, %xmm3
	psubq	%xmm1, %xmm0
	pcmpeqd	%xmm5, %xmm3
	pcmpgtd	%xmm5, %xmm1
	pand	%xmm3, %xmm0
	por	%xmm1, %xmm0
	pshufd	$245, %xmm0, %xmm0
	movmskpd	%xmm0, %eax
	testl	%eax, %eax
	jne	.L1461
.L1286:
	movl	$3, -268(%rbp)
	pcmpeqd	%xmm0, %xmm0
	paddq	%xmm0, %xmm2
	jmp	.L1290
.L1479:
	movq	%r13, %rdi
	call	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	movq	-104(%rbp), %rsi
	movq	0(%r13), %rax
	movq	%rax, (%rsi)
	movq	-16(%r14), %rax
	leaq	8(%rsi), %rdi
	andq	$-8, %rdi
	movq	%rax, -16(%r15)
	movq	%rsi, %rax
	movq	%r13, %rsi
	subq	%rdi, %rax
	leal	(%rbx,%rax), %ecx
	subq	%rax, %rsi
	shrl	$3, %ecx
	rep movsq
	jmp	.L1244
	.cfi_endproc
.LFE18805:
	.size	_ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0, .-_ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	.section	.text._ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0, @function
_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0:
.LFB18807:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %rax
	movq	%rdi, %rdx
	salq	$3, %rax
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
	andq	$-32, %rsp
	leaq	(%r10,%rax), %r9
	subq	$392, %rsp
	leaq	(%r9,%rax), %r8
	movq	%rsi, 192(%rsp)
	vmovdqu	(%rdi), %ymm3
	vmovdqu	(%r15), %ymm14
	leaq	(%r8,%rax), %rdi
	vmovdqu	0(%r13), %ymm11
	vmovdqu	(%r14), %ymm4
	leaq	(%rdi,%rax), %rsi
	vpcmpgtq	%ymm3, %ymm14, %ymm15
	vmovdqu	(%rbx), %ymm12
	vmovdqu	(%r12), %ymm1
	leaq	(%rsi,%rax), %rcx
	vmovdqu	(%r10), %ymm9
	vmovdqu	(%r11), %ymm2
	movq	%rcx, 184(%rsp)
	vmovdqu	(%r9), %ymm8
	vmovdqu	(%r8), %ymm10
	vpblendvb	%ymm15, %ymm3, %ymm14, %ymm13
	vpblendvb	%ymm15, %ymm14, %ymm3, %ymm3
	vmovdqu	(%rsi), %ymm7
	vmovdqu	(%rdi), %ymm0
	vpcmpgtq	%ymm4, %ymm11, %ymm15
	vmovdqu	(%rcx), %ymm5
	addq	%rax, %rcx
	movq	%rcx, 176(%rsp)
	vmovdqa	%ymm5, 360(%rsp)
	vmovdqu	(%rcx), %ymm5
	addq	%rax, %rcx
	vpblendvb	%ymm15, %ymm4, %ymm11, %ymm14
	vpblendvb	%ymm15, %ymm11, %ymm4, %ymm4
	addq	%rcx, %rax
	vpcmpgtq	%ymm1, %ymm12, %ymm15
	vmovdqu	(%rax), %ymm6
	vpblendvb	%ymm15, %ymm1, %ymm12, %ymm11
	vpblendvb	%ymm15, %ymm12, %ymm1, %ymm1
	vpcmpgtq	%ymm2, %ymm9, %ymm15
	vpblendvb	%ymm15, %ymm2, %ymm9, %ymm12
	vpblendvb	%ymm15, %ymm9, %ymm2, %ymm2
	vpcmpgtq	%ymm8, %ymm10, %ymm15
	vpblendvb	%ymm15, %ymm8, %ymm10, %ymm9
	vpblendvb	%ymm15, %ymm10, %ymm8, %ymm15
	vmovdqa	%ymm15, 328(%rsp)
	vpcmpgtq	%ymm0, %ymm7, %ymm8
	vmovdqa	360(%rsp), %ymm15
	vpblendvb	%ymm8, %ymm0, %ymm7, %ymm10
	vpblendvb	%ymm8, %ymm7, %ymm0, %ymm0
	vpcmpgtq	%ymm15, %ymm5, %ymm8
	vpblendvb	%ymm8, %ymm15, %ymm5, %ymm7
	vpblendvb	%ymm8, %ymm5, %ymm15, %ymm5
	vmovdqa	%ymm5, 360(%rsp)
	vpcmpgtq	(%rcx), %ymm6, %ymm5
	vpblendvb	%ymm5, (%rcx), %ymm6, %ymm8
	vmovdqu	(%rcx), %ymm15
	cmpq	$1, 192(%rsp)
	vpblendvb	%ymm5, %ymm6, %ymm15, %ymm5
	vpcmpgtq	%ymm13, %ymm14, %ymm15
	vpblendvb	%ymm15, %ymm13, %ymm14, %ymm6
	vpblendvb	%ymm15, %ymm14, %ymm13, %ymm15
	vpcmpgtq	%ymm3, %ymm4, %ymm14
	vpblendvb	%ymm14, %ymm3, %ymm4, %ymm13
	vpblendvb	%ymm14, %ymm4, %ymm3, %ymm14
	vpcmpgtq	%ymm11, %ymm12, %ymm4
	vpblendvb	%ymm4, %ymm11, %ymm12, %ymm3
	vpblendvb	%ymm4, %ymm12, %ymm11, %ymm4
	vpcmpgtq	%ymm1, %ymm2, %ymm12
	vpblendvb	%ymm12, %ymm1, %ymm2, %ymm11
	vpblendvb	%ymm12, %ymm2, %ymm1, %ymm1
	vpcmpgtq	%ymm9, %ymm10, %ymm12
	vpblendvb	%ymm12, %ymm9, %ymm10, %ymm2
	vpblendvb	%ymm12, %ymm10, %ymm9, %ymm10
	vmovdqa	328(%rsp), %ymm12
	vmovdqa	%ymm10, 296(%rsp)
	vpcmpgtq	%ymm12, %ymm0, %ymm10
	vpblendvb	%ymm10, %ymm12, %ymm0, %ymm9
	vpblendvb	%ymm10, %ymm0, %ymm12, %ymm0
	vmovdqa	360(%rsp), %ymm12
	vmovdqa	%ymm0, 328(%rsp)
	vpcmpgtq	%ymm7, %ymm8, %ymm0
	vpblendvb	%ymm0, %ymm7, %ymm8, %ymm10
	vpblendvb	%ymm0, %ymm8, %ymm7, %ymm7
	vpcmpgtq	%ymm12, %ymm5, %ymm0
	vpblendvb	%ymm0, %ymm12, %ymm5, %ymm8
	vpblendvb	%ymm0, %ymm5, %ymm12, %ymm0
	vpcmpgtq	%ymm6, %ymm3, %ymm12
	vpblendvb	%ymm12, %ymm6, %ymm3, %ymm5
	vpblendvb	%ymm12, %ymm3, %ymm6, %ymm12
	vpcmpgtq	%ymm13, %ymm11, %ymm6
	vpblendvb	%ymm6, %ymm13, %ymm11, %ymm3
	vpblendvb	%ymm6, %ymm11, %ymm13, %ymm13
	vpcmpgtq	%ymm15, %ymm4, %ymm11
	vpblendvb	%ymm11, %ymm15, %ymm4, %ymm6
	vpblendvb	%ymm11, %ymm4, %ymm15, %ymm15
	vpcmpgtq	%ymm14, %ymm1, %ymm11
	vpblendvb	%ymm11, %ymm14, %ymm1, %ymm4
	vpblendvb	%ymm11, %ymm1, %ymm14, %ymm1
	vmovdqa	296(%rsp), %ymm14
	vmovdqa	%ymm1, 264(%rsp)
	vpcmpgtq	%ymm2, %ymm10, %ymm1
	vpblendvb	%ymm1, %ymm2, %ymm10, %ymm11
	vpblendvb	%ymm1, %ymm10, %ymm2, %ymm2
	vpcmpgtq	%ymm9, %ymm8, %ymm1
	vpblendvb	%ymm1, %ymm9, %ymm8, %ymm10
	vpblendvb	%ymm1, %ymm8, %ymm9, %ymm8
	vpcmpgtq	%ymm14, %ymm7, %ymm1
	vpblendvb	%ymm1, %ymm14, %ymm7, %ymm9
	vpblendvb	%ymm1, %ymm7, %ymm14, %ymm7
	vmovdqa	%ymm7, 360(%rsp)
	vmovdqa	328(%rsp), %ymm7
	vpcmpgtq	%ymm7, %ymm0, %ymm14
	vpblendvb	%ymm14, %ymm7, %ymm0, %ymm1
	vpblendvb	%ymm14, %ymm0, %ymm7, %ymm0
	vpcmpgtq	%ymm5, %ymm11, %ymm14
	vpblendvb	%ymm14, %ymm5, %ymm11, %ymm7
	vpblendvb	%ymm14, %ymm11, %ymm5, %ymm14
	vpcmpgtq	%ymm3, %ymm10, %ymm5
	vmovdqa	%ymm7, 40(%rsp)
	vmovdqa	%ymm7, 328(%rsp)
	vpblendvb	%ymm5, %ymm3, %ymm10, %ymm7
	vpblendvb	%ymm5, %ymm10, %ymm3, %ymm3
	vmovdqa	%ymm7, 296(%rsp)
	vpcmpgtq	%ymm6, %ymm9, %ymm10
	vpblendvb	%ymm10, %ymm6, %ymm9, %ymm5
	vpblendvb	%ymm10, %ymm9, %ymm6, %ymm6
	vpcmpgtq	%ymm4, %ymm1, %ymm10
	vpblendvb	%ymm10, %ymm4, %ymm1, %ymm9
	vpblendvb	%ymm10, %ymm1, %ymm4, %ymm4
	vmovdqa	360(%rsp), %ymm1
	vpcmpgtq	%ymm12, %ymm2, %ymm10
	vpblendvb	%ymm10, %ymm12, %ymm2, %ymm7
	vpblendvb	%ymm10, %ymm2, %ymm12, %ymm10
	vpcmpgtq	%ymm15, %ymm1, %ymm12
	vpcmpgtq	%ymm13, %ymm8, %ymm2
	vmovdqa	360(%rsp), %ymm1
	vpblendvb	%ymm2, %ymm13, %ymm8, %ymm11
	vpblendvb	%ymm2, %ymm8, %ymm13, %ymm2
	vpblendvb	%ymm12, %ymm15, %ymm1, %ymm8
	vpblendvb	%ymm12, %ymm1, %ymm15, %ymm1
	vmovdqa	264(%rsp), %ymm15
	vpcmpgtq	%ymm15, %ymm0, %ymm13
	vpblendvb	%ymm13, %ymm15, %ymm0, %ymm12
	vpblendvb	%ymm13, %ymm0, %ymm15, %ymm0
	vmovdqa	296(%rsp), %ymm15
	vmovdqa	%ymm0, 136(%rsp)
	vpcmpgtq	%ymm11, %ymm6, %ymm13
	vpblendvb	%ymm13, %ymm11, %ymm6, %ymm0
	vpblendvb	%ymm13, %ymm6, %ymm11, %ymm6
	vpcmpgtq	%ymm8, %ymm3, %ymm13
	vpblendvb	%ymm13, %ymm8, %ymm3, %ymm11
	vpblendvb	%ymm13, %ymm3, %ymm8, %ymm8
	vpcmpgtq	%ymm9, %ymm10, %ymm3
	vpblendvb	%ymm3, %ymm9, %ymm10, %ymm13
	vpblendvb	%ymm3, %ymm10, %ymm9, %ymm9
	vpcmpgtq	%ymm12, %ymm4, %ymm10
	vpblendvb	%ymm10, %ymm12, %ymm4, %ymm3
	vpblendvb	%ymm10, %ymm4, %ymm12, %ymm4
	vpcmpgtq	%ymm2, %ymm1, %ymm12
	vpblendvb	%ymm12, %ymm2, %ymm1, %ymm10
	vpblendvb	%ymm12, %ymm1, %ymm2, %ymm2
	vpcmpgtq	%ymm7, %ymm14, %ymm1
	vpblendvb	%ymm1, %ymm7, %ymm14, %ymm12
	vpblendvb	%ymm1, %ymm14, %ymm7, %ymm14
	vpcmpgtq	%ymm15, %ymm5, %ymm7
	vpblendvb	%ymm7, %ymm15, %ymm5, %ymm1
	vpblendvb	%ymm7, %ymm5, %ymm15, %ymm5
	vpcmpgtq	%ymm1, %ymm12, %ymm7
	vpblendvb	%ymm7, %ymm1, %ymm12, %ymm15
	vpblendvb	%ymm7, %ymm12, %ymm1, %ymm1
	vpcmpgtq	%ymm3, %ymm10, %ymm7
	vmovdqa	%ymm15, 8(%rsp)
	vmovdqa	%ymm15, 360(%rsp)
	vpblendvb	%ymm7, %ymm3, %ymm10, %ymm12
	vpblendvb	%ymm7, %ymm10, %ymm3, %ymm10
	vpcmpgtq	%ymm5, %ymm14, %ymm7
	vpblendvb	%ymm7, %ymm5, %ymm14, %ymm3
	vpblendvb	%ymm7, %ymm14, %ymm5, %ymm7
	vpcmpgtq	%ymm4, %ymm2, %ymm5
	vpblendvb	%ymm5, %ymm4, %ymm2, %ymm14
	vpblendvb	%ymm5, %ymm2, %ymm4, %ymm4
	vpcmpgtq	%ymm3, %ymm1, %ymm5
	vmovdqa	%ymm4, 104(%rsp)
	vpcmpgtq	%ymm12, %ymm9, %ymm2
	vpblendvb	%ymm5, %ymm3, %ymm1, %ymm4
	vpblendvb	%ymm5, %ymm1, %ymm3, %ymm5
	vpcmpgtq	%ymm0, %ymm11, %ymm1
	vmovdqa	%ymm4, -24(%rsp)
	vmovdqa	%ymm4, 296(%rsp)
	vpblendvb	%ymm1, %ymm0, %ymm11, %ymm15
	vpblendvb	%ymm1, %ymm11, %ymm0, %ymm1
	vpcmpgtq	%ymm8, %ymm6, %ymm0
	vpblendvb	%ymm0, %ymm8, %ymm6, %ymm11
	vpblendvb	%ymm0, %ymm6, %ymm8, %ymm6
	vpcmpgtq	%ymm14, %ymm10, %ymm0
	vpblendvb	%ymm0, %ymm10, %ymm14, %ymm4
	vpblendvb	%ymm0, %ymm14, %ymm10, %ymm8
	vpblendvb	%ymm2, %ymm12, %ymm9, %ymm0
	vmovdqa	%ymm4, 72(%rsp)
	vpcmpgtq	%ymm13, %ymm7, %ymm4
	vpblendvb	%ymm2, %ymm9, %ymm12, %ymm2
	vpblendvb	%ymm4, %ymm13, %ymm7, %ymm3
	vpblendvb	%ymm4, %ymm7, %ymm13, %ymm4
	vpcmpgtq	%ymm3, %ymm15, %ymm7
	vpblendvb	%ymm7, %ymm3, %ymm15, %ymm12
	vpblendvb	%ymm7, %ymm15, %ymm3, %ymm3
	vpcmpgtq	%ymm1, %ymm4, %ymm7
	vpblendvb	%ymm7, %ymm1, %ymm4, %ymm10
	vpblendvb	%ymm7, %ymm4, %ymm1, %ymm4
	vpcmpgtq	%ymm0, %ymm11, %ymm1
	vpblendvb	%ymm1, %ymm0, %ymm11, %ymm7
	vpblendvb	%ymm1, %ymm11, %ymm0, %ymm0
	vpcmpgtq	%ymm6, %ymm2, %ymm1
	vpblendvb	%ymm1, %ymm6, %ymm2, %ymm9
	vpblendvb	%ymm1, %ymm2, %ymm6, %ymm2
	vpcmpgtq	%ymm12, %ymm5, %ymm6
	vpblendvb	%ymm6, %ymm12, %ymm5, %ymm1
	vpblendvb	%ymm6, %ymm5, %ymm12, %ymm15
	vmovdqa	%ymm1, 264(%rsp)
	vpcmpgtq	%ymm3, %ymm10, %ymm5
	vmovdqa	%ymm15, 232(%rsp)
	vpblendvb	%ymm5, %ymm3, %ymm10, %ymm11
	vpblendvb	%ymm5, %ymm10, %ymm3, %ymm3
	vmovdqa	%ymm11, 200(%rsp)
	vpcmpgtq	%ymm7, %ymm4, %ymm5
	vpblendvb	%ymm5, %ymm7, %ymm4, %ymm6
	vpblendvb	%ymm5, %ymm4, %ymm7, %ymm4
	vpcmpgtq	%ymm0, %ymm9, %ymm5
	vpblendvb	%ymm5, %ymm0, %ymm9, %ymm7
	vpblendvb	%ymm5, %ymm9, %ymm0, %ymm0
	vpcmpgtq	%ymm8, %ymm2, %ymm9
	vpblendvb	%ymm9, %ymm8, %ymm2, %ymm5
	vpblendvb	%ymm9, %ymm2, %ymm8, %ymm2
	vpcmpgtq	%ymm3, %ymm6, %ymm8
	vpblendvb	%ymm8, %ymm3, %ymm6, %ymm10
	vpblendvb	%ymm8, %ymm6, %ymm3, %ymm3
	vpcmpgtq	%ymm4, %ymm7, %ymm6
	vpblendvb	%ymm6, %ymm4, %ymm7, %ymm12
	vpblendvb	%ymm6, %ymm7, %ymm4, %ymm6
	jbe	.L1486
	vmovdqa	40(%rsp), %ymm13
	vpshufd	$78, 104(%rsp), %ymm4
	vpshufd	$78, 136(%rsp), %ymm7
	vpshufd	$78, 72(%rsp), %ymm9
	vpshufd	$78, %ymm2, %ymm2
	vpshufd	$78, %ymm5, %ymm5
	vpshufd	$78, %ymm0, %ymm0
	vpcmpgtq	%ymm13, %ymm7, %ymm8
	vpshufd	$78, %ymm6, %ymm6
	vpshufd	$78, %ymm12, %ymm12
	vpblendvb	%ymm8, %ymm13, %ymm7, %ymm14
	vpblendvb	%ymm8, %ymm7, %ymm13, %ymm7
	vmovdqa	8(%rsp), %ymm13
	vmovdqa	%ymm7, 360(%rsp)
	vpcmpgtq	%ymm13, %ymm4, %ymm8
	vpblendvb	%ymm8, %ymm13, %ymm4, %ymm7
	vpblendvb	%ymm8, %ymm4, %ymm13, %ymm4
	vmovdqa	-24(%rsp), %ymm13
	vmovdqa	%ymm4, 328(%rsp)
	vpcmpgtq	%ymm13, %ymm9, %ymm8
	vpblendvb	%ymm8, %ymm13, %ymm9, %ymm4
	vpblendvb	%ymm8, %ymm9, %ymm13, %ymm9
	vpcmpgtq	%ymm1, %ymm2, %ymm8
	vpshufd	$78, %ymm9, %ymm9
	vpblendvb	%ymm8, %ymm1, %ymm2, %ymm13
	vpblendvb	%ymm8, %ymm2, %ymm1, %ymm1
	vpcmpgtq	%ymm15, %ymm5, %ymm8
	vpshufd	$78, %ymm1, %ymm1
	vmovdqa	%ymm13, 296(%rsp)
	vpblendvb	%ymm8, %ymm15, %ymm5, %ymm2
	vpblendvb	%ymm8, %ymm5, %ymm15, %ymm5
	vpcmpgtq	%ymm11, %ymm0, %ymm8
	vpshufd	$78, %ymm2, %ymm2
	vmovdqa	%ymm5, 264(%rsp)
	vpblendvb	%ymm8, %ymm11, %ymm0, %ymm5
	vpblendvb	%ymm8, %ymm0, %ymm11, %ymm0
	vpcmpgtq	%ymm10, %ymm6, %ymm11
	vpshufd	$78, %ymm5, %ymm5
	vpblendvb	%ymm11, %ymm10, %ymm6, %ymm8
	vpblendvb	%ymm11, %ymm6, %ymm10, %ymm10
	vpcmpgtq	%ymm3, %ymm12, %ymm6
	vpshufd	$78, %ymm8, %ymm8
	vpblendvb	%ymm6, %ymm3, %ymm12, %ymm11
	vpblendvb	%ymm6, %ymm12, %ymm3, %ymm3
	vpshufd	$78, 360(%rsp), %ymm6
	vpshufd	$78, 328(%rsp), %ymm12
	vpshufd	$78, %ymm11, %ymm11
	vpcmpgtq	%ymm14, %ymm11, %ymm15
	vpblendvb	%ymm15, %ymm14, %ymm11, %ymm13
	vpblendvb	%ymm15, %ymm11, %ymm14, %ymm15
	vpcmpgtq	%ymm3, %ymm6, %ymm11
	vpblendvb	%ymm11, %ymm3, %ymm6, %ymm14
	vmovdqa	%ymm14, 360(%rsp)
	vpblendvb	%ymm11, %ymm6, %ymm3, %ymm14
	vpcmpgtq	%ymm7, %ymm8, %ymm3
	vmovdqa	296(%rsp), %ymm11
	vpblendvb	%ymm3, %ymm7, %ymm8, %ymm6
	vpblendvb	%ymm3, %ymm8, %ymm7, %ymm7
	vpcmpgtq	%ymm10, %ymm12, %ymm3
	vpshufd	$78, %ymm7, %ymm7
	vpblendvb	%ymm3, %ymm10, %ymm12, %ymm8
	vpblendvb	%ymm3, %ymm12, %ymm10, %ymm10
	vmovdqa	264(%rsp), %ymm12
	vpcmpgtq	%ymm4, %ymm5, %ymm3
	vpshufd	$78, %ymm10, %ymm10
	vmovdqa	%ymm8, 328(%rsp)
	vpblendvb	%ymm3, %ymm4, %ymm5, %ymm8
	vpblendvb	%ymm3, %ymm5, %ymm4, %ymm4
	vpcmpgtq	%ymm0, %ymm9, %ymm3
	vpshufd	$78, %ymm8, %ymm8
	vpblendvb	%ymm3, %ymm0, %ymm9, %ymm5
	vpblendvb	%ymm3, %ymm9, %ymm0, %ymm0
	vpcmpgtq	%ymm11, %ymm2, %ymm3
	vpshufd	$78, %ymm5, %ymm5
	vpblendvb	%ymm3, %ymm11, %ymm2, %ymm9
	vpblendvb	%ymm3, %ymm2, %ymm11, %ymm2
	vpcmpgtq	%ymm12, %ymm1, %ymm11
	vpshufd	$78, %ymm9, %ymm9
	vpblendvb	%ymm11, %ymm12, %ymm1, %ymm3
	vpblendvb	%ymm11, %ymm1, %ymm12, %ymm1
	vpshufd	$78, %ymm15, %ymm12
	vpcmpgtq	%ymm13, %ymm9, %ymm15
	vpshufd	$78, %ymm14, %ymm11
	vpshufd	$78, %ymm3, %ymm3
	vpblendvb	%ymm15, %ymm13, %ymm9, %ymm14
	vpblendvb	%ymm15, %ymm9, %ymm13, %ymm15
	vpcmpgtq	%ymm6, %ymm8, %ymm9
	vpblendvb	%ymm9, %ymm6, %ymm8, %ymm13
	vpblendvb	%ymm9, %ymm8, %ymm6, %ymm6
	vpcmpgtq	%ymm2, %ymm12, %ymm8
	vpblendvb	%ymm8, %ymm2, %ymm12, %ymm9
	vpblendvb	%ymm8, %ymm12, %ymm2, %ymm2
	vpcmpgtq	%ymm4, %ymm7, %ymm8
	vpshufd	$78, %ymm2, %ymm2
	vmovdqa	%ymm9, 296(%rsp)
	vpblendvb	%ymm8, %ymm4, %ymm7, %ymm9
	vpblendvb	%ymm8, %ymm7, %ymm4, %ymm4
	vmovdqa	360(%rsp), %ymm8
	vpshufd	$78, %ymm9, %ymm9
	vpcmpgtq	%ymm8, %ymm3, %ymm7
	vpblendvb	%ymm7, %ymm8, %ymm3, %ymm12
	vpblendvb	%ymm7, %ymm3, %ymm8, %ymm3
	vmovdqa	328(%rsp), %ymm8
	vmovdqa	%ymm3, 360(%rsp)
	vpcmpgtq	%ymm8, %ymm5, %ymm7
	vpblendvb	%ymm7, %ymm8, %ymm5, %ymm3
	vpblendvb	%ymm7, %ymm5, %ymm8, %ymm5
	vpcmpgtq	%ymm1, %ymm11, %ymm7
	vpblendvb	%ymm7, %ymm1, %ymm11, %ymm8
	vpblendvb	%ymm7, %ymm11, %ymm1, %ymm1
	vpcmpgtq	%ymm0, %ymm10, %ymm11
	vpshufd	$78, %ymm1, %ymm1
	vmovdqa	%ymm8, 328(%rsp)
	vpshufd	$78, %ymm3, %ymm8
	vpshufd	$78, 360(%rsp), %ymm3
	vpblendvb	%ymm11, %ymm0, %ymm10, %ymm7
	vpblendvb	%ymm11, %ymm10, %ymm0, %ymm0
	vpshufd	$78, %ymm13, %ymm11
	vpshufd	$78, %ymm15, %ymm10
	vpcmpgtq	%ymm14, %ymm11, %ymm15
	vpshufd	$78, %ymm7, %ymm7
	vpblendvb	%ymm15, %ymm14, %ymm11, %ymm13
	vpblendvb	%ymm15, %ymm11, %ymm14, %ymm14
	vmovdqa	%ymm13, 360(%rsp)
	vpcmpgtq	%ymm6, %ymm10, %ymm11
	vmovdqa	296(%rsp), %ymm13
	vpblendvb	%ymm11, %ymm6, %ymm10, %ymm15
	vpblendvb	%ymm11, %ymm10, %ymm6, %ymm10
	vpcmpgtq	%ymm13, %ymm9, %ymm6
	vpblendvb	%ymm6, %ymm13, %ymm9, %ymm11
	vpblendvb	%ymm6, %ymm9, %ymm13, %ymm9
	vpcmpgtq	%ymm4, %ymm2, %ymm6
	vpblendvb	%ymm6, %ymm4, %ymm2, %ymm13
	vpblendvb	%ymm6, %ymm2, %ymm4, %ymm4
	vpcmpgtq	%ymm12, %ymm8, %ymm2
	vpblendvb	%ymm2, %ymm12, %ymm8, %ymm6
	vpblendvb	%ymm2, %ymm8, %ymm12, %ymm8
	vmovdqa	%ymm6, 136(%rsp)
	vpcmpgtq	%ymm5, %ymm3, %ymm2
	vpblendvb	%ymm2, %ymm5, %ymm3, %ymm12
	vpblendvb	%ymm2, %ymm3, %ymm5, %ymm3
	vmovdqa	328(%rsp), %ymm5
	vmovdqa	%ymm12, 104(%rsp)
	vpcmpgtq	%ymm5, %ymm7, %ymm2
	vpblendvb	%ymm2, %ymm5, %ymm7, %ymm6
	vpblendvb	%ymm2, %ymm7, %ymm5, %ymm7
	vmovdqa	360(%rsp), %ymm5
	vpcmpgtq	%ymm0, %ymm1, %ymm2
	vmovdqa	%ymm6, 72(%rsp)
	vpblendvb	%ymm2, %ymm0, %ymm1, %ymm12
	vpblendvb	%ymm2, %ymm1, %ymm0, %ymm2
	vpshufd	$78, %ymm5, %ymm1
	vpcmpgtq	%ymm5, %ymm1, %ymm0
	vmovdqa	%ymm12, 40(%rsp)
	vmovdqa	%ymm2, 8(%rsp)
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm1, %ymm5, %ymm6
	vpshufd	$78, %ymm14, %ymm1
	vmovdqa	%ymm6, 328(%rsp)
	vpcmpgtq	%ymm14, %ymm1, %ymm0
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm1, %ymm14, %ymm1
	vmovdqa	%ymm1, -24(%rsp)
	vmovdqa	104(%rsp), %ymm5
	cmpq	$3, 192(%rsp)
	vmovdqa	%ymm1, 360(%rsp)
	vpshufd	$78, %ymm15, %ymm1
	vpcmpgtq	%ymm15, %ymm1, %ymm0
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm1, %ymm15, %ymm15
	vpshufd	$78, %ymm10, %ymm1
	vmovdqa	%ymm15, 296(%rsp)
	vpcmpgtq	%ymm10, %ymm1, %ymm0
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm1, %ymm10, %ymm1
	vpshufd	$78, %ymm8, %ymm10
	vmovdqa	%ymm1, -56(%rsp)
	vmovdqa	%ymm1, 264(%rsp)
	vpshufd	$78, %ymm11, %ymm1
	vpcmpgtq	%ymm11, %ymm1, %ymm0
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm1, %ymm11, %ymm11
	vpshufd	$78, %ymm9, %ymm1
	vmovdqa	%ymm11, 232(%rsp)
	vpcmpgtq	%ymm9, %ymm1, %ymm0
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm1, %ymm9, %ymm1
	vmovdqa	40(%rsp), %ymm9
	vmovdqa	%ymm1, -88(%rsp)
	vmovdqa	%ymm1, 200(%rsp)
	vpshufd	$78, %ymm13, %ymm1
	vpcmpgtq	%ymm13, %ymm1, %ymm0
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm1, %ymm13, %ymm12
	vpshufd	$78, %ymm4, %ymm1
	vpcmpgtq	%ymm4, %ymm1, %ymm0
	vmovdqa	%ymm12, %ymm14
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm1, %ymm4, %ymm1
	vmovdqa	136(%rsp), %ymm4
	vmovdqa	%ymm1, -120(%rsp)
	vmovdqa	%ymm1, %ymm13
	vpshufd	$78, %ymm4, %ymm1
	vpcmpgtq	%ymm4, %ymm1, %ymm0
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm1, %ymm4, %ymm4
	vpcmpgtq	%ymm8, %ymm10, %ymm0
	vpshufd	$78, %ymm5, %ymm1
	vpcmpgtq	%ymm5, %ymm1, %ymm2
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpunpckhqdq	%ymm2, %ymm2, %ymm2
	vpblendvb	%ymm0, %ymm10, %ymm8, %ymm10
	vmovdqa	8(%rsp), %ymm8
	vpblendvb	%ymm2, %ymm1, %ymm5, %ymm1
	vpshufd	$78, %ymm3, %ymm5
	vpcmpgtq	%ymm3, %ymm5, %ymm0
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm5, %ymm3, %ymm5
	vmovdqa	72(%rsp), %ymm0
	vpshufd	$78, %ymm0, %ymm2
	vpcmpgtq	%ymm0, %ymm2, %ymm3
	vpunpckhqdq	%ymm3, %ymm3, %ymm3
	vpblendvb	%ymm3, %ymm2, %ymm0, %ymm2
	vpshufd	$78, %ymm7, %ymm3
	vpcmpgtq	%ymm7, %ymm3, %ymm0
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm3, %ymm7, %ymm7
	vpshufd	$78, %ymm9, %ymm3
	vpcmpgtq	%ymm9, %ymm3, %ymm0
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm3, %ymm9, %ymm3
	vpshufd	$78, %ymm8, %ymm9
	vpcmpgtq	%ymm8, %ymm9, %ymm0
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm9, %ymm8, %ymm9
	jbe	.L1487
	vpshufd	$78, %ymm9, %ymm9
	vpshufd	$78, %ymm5, %ymm5
	vpshufd	$78, %ymm2, %ymm0
	vpermq	$78, %ymm9, %ymm9
	vpermq	$78, %ymm5, %ymm8
	vpshufd	$78, %ymm3, %ymm3
	vmovdqa	-24(%rsp), %ymm5
	vpcmpgtq	%ymm6, %ymm9, %ymm2
	vpermq	$78, %ymm3, %ymm3
	vpshufd	$78, %ymm7, %ymm7
	vpermq	$78, %ymm7, %ymm7
	vpermq	$78, %ymm0, %ymm0
	vpshufd	$78, %ymm1, %ymm1
	vpermq	$78, %ymm1, %ymm1
	vpshufd	$78, %ymm10, %ymm10
	vpshufd	$78, %ymm4, %ymm4
	vpblendvb	%ymm2, %ymm6, %ymm9, %ymm14
	vpblendvb	%ymm2, %ymm9, %ymm6, %ymm13
	vmovdqa	-56(%rsp), %ymm9
	vpermq	$78, %ymm10, %ymm10
	vpcmpgtq	%ymm5, %ymm3, %ymm2
	vpermq	$78, %ymm4, %ymm4
	vpblendvb	%ymm2, %ymm5, %ymm3, %ymm6
	vpblendvb	%ymm2, %ymm3, %ymm5, %ymm3
	vpcmpgtq	%ymm15, %ymm7, %ymm2
	vpshufd	$78, %ymm3, %ymm3
	vpermq	$78, %ymm3, %ymm3
	vpblendvb	%ymm2, %ymm15, %ymm7, %ymm5
	vpblendvb	%ymm2, %ymm7, %ymm15, %ymm7
	vpcmpgtq	%ymm9, %ymm0, %ymm2
	vpshufd	$78, %ymm7, %ymm7
	vpermq	$78, %ymm7, %ymm7
	vpblendvb	%ymm2, %ymm9, %ymm0, %ymm15
	vpblendvb	%ymm2, %ymm0, %ymm9, %ymm0
	vpcmpgtq	%ymm11, %ymm8, %ymm9
	vpshufd	$78, %ymm0, %ymm0
	vmovdqa	%ymm15, 360(%rsp)
	vmovdqa	-120(%rsp), %ymm15
	vpermq	$78, %ymm0, %ymm0
	vpblendvb	%ymm9, %ymm11, %ymm8, %ymm2
	vpblendvb	%ymm9, %ymm8, %ymm11, %ymm11
	vmovdqa	-88(%rsp), %ymm9
	vmovdqa	%ymm11, 328(%rsp)
	vpshufd	$78, %ymm2, %ymm2
	vpcmpgtq	%ymm9, %ymm1, %ymm8
	vpermq	$78, %ymm2, %ymm2
	vpblendvb	%ymm8, %ymm9, %ymm1, %ymm11
	vpblendvb	%ymm8, %ymm1, %ymm9, %ymm1
	vpcmpgtq	%ymm12, %ymm10, %ymm9
	vpshufd	$78, %ymm11, %ymm11
	vpermq	$78, %ymm11, %ymm11
	vpblendvb	%ymm9, %ymm12, %ymm10, %ymm8
	vpblendvb	%ymm9, %ymm10, %ymm12, %ymm10
	vpcmpgtq	%ymm15, %ymm4, %ymm9
	vpshufd	$78, %ymm8, %ymm8
	vpermq	$78, %ymm8, %ymm8
	vpblendvb	%ymm9, %ymm15, %ymm4, %ymm12
	vpblendvb	%ymm9, %ymm4, %ymm15, %ymm4
	vpshufd	$78, %ymm13, %ymm9
	vpshufd	$78, %ymm12, %ymm12
	vpermq	$78, %ymm9, %ymm9
	vpermq	$78, %ymm12, %ymm12
	vpcmpgtq	%ymm14, %ymm12, %ymm15
	vpblendvb	%ymm15, %ymm14, %ymm12, %ymm13
	vpblendvb	%ymm15, %ymm12, %ymm14, %ymm15
	vpcmpgtq	%ymm4, %ymm9, %ymm12
	vpblendvb	%ymm12, %ymm4, %ymm9, %ymm14
	vmovdqa	%ymm14, 296(%rsp)
	vpblendvb	%ymm12, %ymm9, %ymm4, %ymm14
	vpcmpgtq	%ymm6, %ymm8, %ymm4
	vpblendvb	%ymm4, %ymm6, %ymm8, %ymm9
	vpblendvb	%ymm4, %ymm8, %ymm6, %ymm6
	vpcmpgtq	%ymm10, %ymm3, %ymm4
	vpshufd	$78, %ymm6, %ymm6
	vpermq	$78, %ymm6, %ymm6
	vpblendvb	%ymm4, %ymm10, %ymm3, %ymm12
	vpblendvb	%ymm4, %ymm3, %ymm10, %ymm10
	vpcmpgtq	%ymm5, %ymm11, %ymm3
	vpshufd	$78, %ymm10, %ymm10
	vmovdqa	%ymm12, 264(%rsp)
	vpermq	$78, %ymm10, %ymm10
	vpblendvb	%ymm3, %ymm5, %ymm11, %ymm8
	vpblendvb	%ymm3, %ymm11, %ymm5, %ymm5
	vmovdqa	328(%rsp), %ymm11
	vpcmpgtq	%ymm1, %ymm7, %ymm3
	vpshufd	$78, %ymm8, %ymm8
	vpermq	$78, %ymm8, %ymm8
	vpblendvb	%ymm3, %ymm1, %ymm7, %ymm4
	vpblendvb	%ymm3, %ymm7, %ymm1, %ymm1
	vmovdqa	360(%rsp), %ymm7
	vpshufd	$78, %ymm4, %ymm4
	vpcmpgtq	%ymm7, %ymm2, %ymm3
	vpermq	$78, %ymm4, %ymm4
	vpblendvb	%ymm3, %ymm7, %ymm2, %ymm12
	vpblendvb	%ymm3, %ymm2, %ymm7, %ymm2
	vpcmpgtq	%ymm11, %ymm0, %ymm7
	vpshufd	$78, %ymm12, %ymm12
	vpermq	$78, %ymm12, %ymm12
	vpblendvb	%ymm7, %ymm11, %ymm0, %ymm3
	vpblendvb	%ymm7, %ymm0, %ymm11, %ymm0
	vpshufd	$78, %ymm15, %ymm7
	vpcmpgtq	%ymm13, %ymm12, %ymm15
	vpshufd	$78, %ymm14, %ymm11
	vpermq	$78, %ymm7, %ymm7
	vpshufd	$78, %ymm3, %ymm3
	vpermq	$78, %ymm11, %ymm11
	vpermq	$78, %ymm3, %ymm3
	vpblendvb	%ymm15, %ymm13, %ymm12, %ymm14
	vpblendvb	%ymm15, %ymm12, %ymm13, %ymm15
	vpcmpgtq	%ymm9, %ymm8, %ymm12
	vpblendvb	%ymm12, %ymm9, %ymm8, %ymm13
	vpblendvb	%ymm12, %ymm8, %ymm9, %ymm9
	vmovdqa	%ymm13, 360(%rsp)
	vpcmpgtq	%ymm2, %ymm7, %ymm8
	vpblendvb	%ymm8, %ymm2, %ymm7, %ymm13
	vpblendvb	%ymm8, %ymm7, %ymm2, %ymm2
	vpcmpgtq	%ymm5, %ymm6, %ymm7
	vpshufd	$78, %ymm2, %ymm2
	vpermq	$78, %ymm2, %ymm2
	vpblendvb	%ymm7, %ymm5, %ymm6, %ymm8
	vpblendvb	%ymm7, %ymm6, %ymm5, %ymm5
	vmovdqa	296(%rsp), %ymm7
	vpshufd	$78, %ymm8, %ymm8
	vpcmpgtq	%ymm7, %ymm3, %ymm6
	vpermq	$78, %ymm8, %ymm8
	vpblendvb	%ymm6, %ymm7, %ymm3, %ymm12
	vpblendvb	%ymm6, %ymm3, %ymm7, %ymm3
	vmovdqa	%ymm12, 328(%rsp)
	vpshufd	$78, %ymm3, %ymm3
	vmovdqa	264(%rsp), %ymm12
	vpermq	$78, %ymm3, %ymm3
	vpcmpgtq	%ymm12, %ymm4, %ymm6
	vpblendvb	%ymm6, %ymm12, %ymm4, %ymm7
	vpblendvb	%ymm6, %ymm4, %ymm12, %ymm4
	vpcmpgtq	%ymm0, %ymm11, %ymm6
	vpshufd	$78, %ymm7, %ymm7
	vpermq	$78, %ymm7, %ymm7
	vpblendvb	%ymm6, %ymm0, %ymm11, %ymm12
	vpblendvb	%ymm6, %ymm11, %ymm0, %ymm0
	vpcmpgtq	%ymm1, %ymm10, %ymm11
	vpshufd	$78, %ymm0, %ymm0
	vmovdqa	%ymm12, 296(%rsp)
	vpermq	$78, %ymm0, %ymm0
	vpblendvb	%ymm11, %ymm1, %ymm10, %ymm6
	vpblendvb	%ymm11, %ymm10, %ymm1, %ymm1
	vpshufd	$78, 360(%rsp), %ymm11
	vpermq	$78, %ymm11, %ymm11
	vpshufd	$78, %ymm15, %ymm10
	vpcmpgtq	%ymm14, %ymm11, %ymm15
	vpshufd	$78, %ymm6, %ymm6
	vpermq	$78, %ymm10, %ymm10
	vpermq	$78, %ymm6, %ymm6
	vpblendvb	%ymm15, %ymm14, %ymm11, %ymm12
	vpblendvb	%ymm15, %ymm11, %ymm14, %ymm14
	vmovdqa	%ymm12, 360(%rsp)
	vpcmpgtq	%ymm9, %ymm10, %ymm11
	vmovdqa	328(%rsp), %ymm12
	vpblendvb	%ymm11, %ymm9, %ymm10, %ymm15
	vpblendvb	%ymm11, %ymm10, %ymm9, %ymm10
	vpcmpgtq	%ymm13, %ymm8, %ymm9
	vpblendvb	%ymm9, %ymm13, %ymm8, %ymm11
	vpblendvb	%ymm9, %ymm8, %ymm13, %ymm9
	vpcmpgtq	%ymm5, %ymm2, %ymm8
	vpblendvb	%ymm8, %ymm5, %ymm2, %ymm13
	vpblendvb	%ymm8, %ymm2, %ymm5, %ymm5
	vpcmpgtq	%ymm12, %ymm7, %ymm2
	vpblendvb	%ymm2, %ymm12, %ymm7, %ymm8
	vpblendvb	%ymm2, %ymm7, %ymm12, %ymm7
	vpcmpgtq	%ymm4, %ymm3, %ymm2
	vpblendvb	%ymm2, %ymm4, %ymm3, %ymm12
	vpblendvb	%ymm2, %ymm3, %ymm4, %ymm3
	vmovdqa	%ymm12, 328(%rsp)
	vmovdqa	296(%rsp), %ymm12
	vpcmpgtq	%ymm12, %ymm6, %ymm2
	vpblendvb	%ymm2, %ymm12, %ymm6, %ymm4
	vpblendvb	%ymm2, %ymm6, %ymm12, %ymm6
	vpcmpgtq	%ymm1, %ymm0, %ymm2
	vpblendvb	%ymm2, %ymm1, %ymm0, %ymm12
	vpblendvb	%ymm2, %ymm0, %ymm1, %ymm2
	vmovdqa	%ymm12, 296(%rsp)
	vmovdqa	360(%rsp), %ymm12
	vmovdqa	%ymm2, 264(%rsp)
	vpshufd	$78, %ymm12, %ymm0
	vpermq	$78, %ymm0, %ymm0
	vpcmpgtq	%ymm12, %ymm0, %ymm1
	vpblendvb	%ymm1, %ymm12, %ymm0, %ymm2
	vpblendvb	%ymm1, %ymm0, %ymm12, %ymm12
	vpblendd	$15, %ymm2, %ymm12, %ymm0
	vmovdqa	%ymm0, 360(%rsp)
	vpshufd	$78, %ymm14, %ymm0
	vpermq	$78, %ymm0, %ymm0
	vpcmpgtq	%ymm14, %ymm0, %ymm1
	vpblendvb	%ymm1, %ymm14, %ymm0, %ymm2
	vpblendvb	%ymm1, %ymm0, %ymm14, %ymm14
	vpshufd	$78, %ymm15, %ymm0
	vpermq	$78, %ymm0, %ymm0
	vpblendd	$15, %ymm2, %ymm14, %ymm14
	vpcmpgtq	%ymm15, %ymm0, %ymm1
	vpblendvb	%ymm1, %ymm15, %ymm0, %ymm2
	vpblendvb	%ymm1, %ymm0, %ymm15, %ymm15
	vpshufd	$78, %ymm10, %ymm0
	vpblendd	$15, %ymm2, %ymm15, %ymm1
	vpermq	$78, %ymm0, %ymm0
	vmovdqa	328(%rsp), %ymm15
	vmovdqa	%ymm1, 232(%rsp)
	vpcmpgtq	%ymm10, %ymm0, %ymm1
	vpblendvb	%ymm1, %ymm10, %ymm0, %ymm2
	vpblendvb	%ymm1, %ymm0, %ymm10, %ymm10
	vpshufd	$78, %ymm11, %ymm0
	vpermq	$78, %ymm0, %ymm0
	vpblendd	$15, %ymm2, %ymm10, %ymm10
	vpcmpgtq	%ymm11, %ymm0, %ymm1
	vpblendvb	%ymm1, %ymm11, %ymm0, %ymm2
	vpblendvb	%ymm1, %ymm0, %ymm11, %ymm11
	vpshufd	$78, %ymm9, %ymm0
	vpermq	$78, %ymm0, %ymm0
	vpblendd	$15, %ymm2, %ymm11, %ymm2
	vpcmpgtq	%ymm9, %ymm0, %ymm1
	vpblendvb	%ymm1, %ymm9, %ymm0, %ymm11
	vpblendvb	%ymm1, %ymm0, %ymm9, %ymm9
	vpshufd	$78, %ymm13, %ymm1
	vpermq	$78, %ymm1, %ymm1
	vpblendd	$15, %ymm11, %ymm9, %ymm0
	vmovdqa	%ymm0, 200(%rsp)
	vpcmpgtq	%ymm13, %ymm1, %ymm9
	vmovdqa	264(%rsp), %ymm0
	vpblendvb	%ymm9, %ymm13, %ymm1, %ymm11
	vpblendvb	%ymm9, %ymm1, %ymm13, %ymm13
	vpshufd	$78, %ymm5, %ymm1
	vpermq	$78, %ymm1, %ymm1
	vpblendd	$15, %ymm11, %ymm13, %ymm13
	vpcmpgtq	%ymm5, %ymm1, %ymm9
	vpblendvb	%ymm9, %ymm5, %ymm1, %ymm11
	vpblendvb	%ymm9, %ymm1, %ymm5, %ymm5
	vpshufd	$78, %ymm8, %ymm1
	vpermq	$78, %ymm1, %ymm1
	vpblendd	$15, %ymm11, %ymm5, %ymm5
	vpcmpgtq	%ymm8, %ymm1, %ymm9
	vpblendvb	%ymm9, %ymm8, %ymm1, %ymm11
	vpblendvb	%ymm9, %ymm1, %ymm8, %ymm8
	vpshufd	$78, %ymm7, %ymm1
	vpermq	$78, %ymm1, %ymm1
	vpblendd	$15, %ymm11, %ymm8, %ymm8
	vpcmpgtq	%ymm7, %ymm1, %ymm9
	vpblendvb	%ymm9, %ymm7, %ymm1, %ymm11
	vpblendvb	%ymm9, %ymm1, %ymm7, %ymm7
	vpshufd	$78, %ymm15, %ymm1
	vpermq	$78, %ymm1, %ymm1
	vpblendd	$15, %ymm11, %ymm7, %ymm7
	vpcmpgtq	%ymm15, %ymm1, %ymm9
	vpblendvb	%ymm9, %ymm15, %ymm1, %ymm11
	vpblendvb	%ymm9, %ymm1, %ymm15, %ymm9
	vpshufd	$78, %ymm3, %ymm1
	vpermq	$78, %ymm1, %ymm1
	vpblendd	$15, %ymm11, %ymm9, %ymm9
	vpcmpgtq	%ymm3, %ymm1, %ymm11
	vpblendvb	%ymm11, %ymm3, %ymm1, %ymm12
	vpblendvb	%ymm11, %ymm1, %ymm3, %ymm3
	vpshufd	$78, %ymm4, %ymm1
	vpermq	$78, %ymm1, %ymm1
	vpblendd	$15, %ymm12, %ymm3, %ymm3
	vpcmpgtq	%ymm4, %ymm1, %ymm11
	vpblendvb	%ymm11, %ymm4, %ymm1, %ymm12
	vpblendvb	%ymm11, %ymm1, %ymm4, %ymm4
	vpshufd	$78, %ymm6, %ymm1
	vpermq	$78, %ymm1, %ymm1
	vpblendd	$15, %ymm12, %ymm4, %ymm4
	vmovdqa	296(%rsp), %ymm12
	vpcmpgtq	%ymm6, %ymm1, %ymm15
	vpblendvb	%ymm15, %ymm6, %ymm1, %ymm11
	vpblendvb	%ymm15, %ymm1, %ymm6, %ymm6
	vpshufd	$78, %ymm12, %ymm1
	vpermq	$78, %ymm1, %ymm1
	vpblendd	$15, %ymm11, %ymm6, %ymm15
	vpcmpgtq	%ymm12, %ymm1, %ymm11
	vpblendvb	%ymm11, %ymm12, %ymm1, %ymm6
	vpblendvb	%ymm11, %ymm1, %ymm12, %ymm11
	vpshufd	$78, %ymm0, %ymm1
	vpermq	$78, %ymm1, %ymm1
	vpblendd	$15, %ymm6, %ymm11, %ymm11
	vpcmpgtq	%ymm0, %ymm1, %ymm6
	vpblendvb	%ymm6, %ymm0, %ymm1, %ymm12
	vpblendvb	%ymm6, %ymm1, %ymm0, %ymm1
	vmovdqa	360(%rsp), %ymm0
	vpblendd	$15, %ymm12, %ymm1, %ymm1
	vpshufd	$78, %ymm0, %ymm12
	vpcmpgtq	%ymm0, %ymm12, %ymm6
	vpunpckhqdq	%ymm6, %ymm6, %ymm6
	vpblendvb	%ymm6, %ymm12, %ymm0, %ymm6
	vpshufd	$78, %ymm14, %ymm12
	vmovdqa	%ymm6, 328(%rsp)
	vpcmpgtq	%ymm14, %ymm12, %ymm6
	vpunpckhqdq	%ymm6, %ymm6, %ymm6
	vpblendvb	%ymm6, %ymm12, %ymm14, %ymm6
	vmovdqa	%ymm6, 360(%rsp)
	vmovdqa	232(%rsp), %ymm0
	vpshufd	$78, %ymm0, %ymm12
	vpcmpgtq	%ymm0, %ymm12, %ymm6
	vpunpckhqdq	%ymm6, %ymm6, %ymm6
	vpblendvb	%ymm6, %ymm12, %ymm0, %ymm6
	vpshufd	$78, %ymm10, %ymm12
	vmovdqa	200(%rsp), %ymm0
	vmovdqa	%ymm6, 296(%rsp)
	vpcmpgtq	%ymm10, %ymm12, %ymm6
	vpunpckhqdq	%ymm6, %ymm6, %ymm6
	vpblendvb	%ymm6, %ymm12, %ymm10, %ymm6
	vpshufd	$78, %ymm2, %ymm10
	vpshufd	$78, %ymm8, %ymm12
	vmovdqa	%ymm6, 264(%rsp)
	vpcmpgtq	%ymm2, %ymm10, %ymm6
	vpunpckhqdq	%ymm6, %ymm6, %ymm6
	vpblendvb	%ymm6, %ymm10, %ymm2, %ymm6
	vmovdqa	%ymm6, 232(%rsp)
	vpshufd	$78, %ymm0, %ymm6
	vpcmpgtq	%ymm0, %ymm6, %ymm2
	vpunpckhqdq	%ymm2, %ymm2, %ymm2
	vpblendvb	%ymm2, %ymm6, %ymm0, %ymm6
	vpshufd	$78, %ymm13, %ymm2
	vmovdqa	%ymm6, 200(%rsp)
	vpcmpgtq	%ymm13, %ymm2, %ymm0
	vpshufd	$78, %ymm7, %ymm6
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm2, %ymm13, %ymm14
	vpshufd	$78, %ymm5, %ymm2
	vpcmpgtq	%ymm5, %ymm2, %ymm0
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm2, %ymm5, %ymm13
	vpcmpgtq	%ymm8, %ymm12, %ymm0
	vpshufd	$78, %ymm3, %ymm5
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm12, %ymm8, %ymm12
	vpcmpgtq	%ymm7, %ymm6, %ymm0
	vpunpckhqdq	%ymm0, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm6, %ymm7, %ymm6
	vpshufd	$78, %ymm9, %ymm0
	vpshufd	$78, %ymm15, %ymm7
	vpcmpgtq	%ymm9, %ymm0, %ymm2
	vpunpckhqdq	%ymm2, %ymm2, %ymm2
	vpblendvb	%ymm2, %ymm0, %ymm9, %ymm0
	vpcmpgtq	%ymm3, %ymm5, %ymm2
	vpshufd	$78, %ymm1, %ymm9
	vpunpckhqdq	%ymm2, %ymm2, %ymm2
	vpblendvb	%ymm2, %ymm5, %ymm3, %ymm5
	vpshufd	$78, %ymm4, %ymm2
	vpcmpgtq	%ymm4, %ymm2, %ymm3
	vpunpckhqdq	%ymm3, %ymm3, %ymm3
	vpblendvb	%ymm3, %ymm2, %ymm4, %ymm2
	vpcmpgtq	%ymm15, %ymm7, %ymm3
	vpcmpgtq	%ymm1, %ymm9, %ymm4
	vpunpckhqdq	%ymm3, %ymm3, %ymm3
	vpblendvb	%ymm3, %ymm7, %ymm15, %ymm7
	vpshufd	$78, %ymm11, %ymm3
	vpunpckhqdq	%ymm4, %ymm4, %ymm4
	vpcmpgtq	%ymm11, %ymm3, %ymm8
	vpblendvb	%ymm4, %ymm9, %ymm1, %ymm9
	vpunpckhqdq	%ymm8, %ymm8, %ymm8
	vpblendvb	%ymm8, %ymm3, %ymm11, %ymm3
.L1483:
	vmovdqa	328(%rsp), %ymm4
	vmovdqu	%ymm4, (%rdx)
	movq	184(%rsp), %rdx
	vmovdqa	360(%rsp), %ymm4
	vmovdqu	%ymm4, (%r15)
	vmovdqa	296(%rsp), %ymm4
	vmovdqu	%ymm4, (%r14)
	vmovdqa	264(%rsp), %ymm4
	vmovdqu	%ymm4, 0(%r13)
	vmovdqa	232(%rsp), %ymm4
	vmovdqu	%ymm4, (%r12)
	vmovdqa	200(%rsp), %ymm4
	vmovdqu	%ymm4, (%rbx)
	movq	176(%rsp), %rbx
	vmovdqu	%ymm14, (%r11)
	vmovdqu	%ymm13, (%r10)
	vmovdqu	%ymm12, (%r9)
	vmovdqu	%ymm6, (%r8)
	vmovdqu	%ymm0, (%rdi)
	vmovdqu	%ymm5, (%rsi)
	vmovdqu	%ymm2, (%rdx)
	vmovdqu	%ymm7, (%rbx)
	vmovdqu	%ymm3, (%rcx)
	vmovdqu	%ymm9, (%rax)
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
.L1486:
	.cfi_restore_state
	vmovdqa	%ymm3, %ymm13
	vmovdqa	72(%rsp), %ymm7
	vmovdqa	104(%rsp), %ymm3
	vmovdqa	%ymm10, %ymm14
	vmovdqa	136(%rsp), %ymm9
	jmp	.L1483
	.p2align 4,,10
	.p2align 3
.L1487:
	vmovdqa	%ymm4, %ymm12
	vmovdqa	%ymm10, %ymm6
	vmovdqa	%ymm1, %ymm0
	jmp	.L1483
	.cfi_endproc
.LFE18807:
	.size	_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0, .-_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	.section	.text._ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0,"ax",@progbits
	.p2align 4
	.type	_ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0, @function
_ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0:
.LFB18808:
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
	cmpq	$64, %rdx
	jbe	.L1654
	movq	%rdi, %rax
	movq	%rdi, -112(%rbp)
	movq	%r8, %rbx
	shrq	$3, %rax
	movq	%rax, %rdx
	movq	%rax, -144(%rbp)
	andl	$7, %edx
	jne	.L1655
	movq	-88(%rbp), %r14
	movq	%rdi, %rax
.L1501:
	movq	8(%rbx), %rcx
	movq	16(%rbx), %r10
	movq	%rcx, %rdi
	leaq	1(%r10), %r8
	leaq	(%rcx,%rcx,8), %rsi
	xorq	(%rbx), %r8
	shrq	$11, %rdi
	rorx	$40, %rcx, %rdx
	leaq	2(%r10), %rcx
	addq	%r8, %rdx
	xorq	%rdi, %rsi
	movq	%rdx, %r9
	rorx	$40, %rdx, %rdi
	xorq	%rcx, %rsi
	shrq	$11, %r9
	leaq	(%rdx,%rdx,8), %rcx
	leaq	3(%r10), %rdx
	addq	%rsi, %rdi
	xorq	%r9, %rcx
	movq	%rdi, %r9
	xorq	%rdx, %rcx
	leaq	(%rdi,%rdi,8), %rdx
	rorx	$40, %rdi, %r11
	shrq	$11, %r9
	addq	%rcx, %r11
	leaq	4(%r10), %rdi
	addq	$5, %r10
	xorq	%r9, %rdx
	movq	%r11, %r15
	rorx	$40, %r11, %r9
	movq	%r10, 16(%rbx)
	xorq	%rdi, %rdx
	shrq	$11, %r15
	leaq	(%r11,%r11,8), %rdi
	addq	%rdx, %r9
	xorq	%r15, %rdi
	movq	%r9, %r15
	leaq	(%r9,%r9,8), %r11
	xorq	%r10, %rdi
	rorx	$40, %r9, %r9
	shrq	$11, %r15
	addq	%rdi, %r9
	movl	%edi, %edi
	movabsq	$34359738359, %r10
	xorq	%r15, %r11
	movl	%edx, %r15d
	vmovq	%r11, %xmm7
	movl	%r8d, %r11d
	vpinsrq	$1, %r9, %xmm7, %xmm0
	movq	%r14, %r9
	shrq	$3, %r9
	cmpq	%r10, %r14
	movl	$4294967295, %r10d
	movl	%ecx, %r14d
	cmova	%r10, %r9
	shrq	$32, %r8
	movl	%esi, %r10d
	vmovdqu	%xmm0, (%rbx)
	shrq	$32, %rsi
	imulq	%r9, %r11
	shrq	$32, %rcx
	shrq	$32, %rdx
	imulq	%r9, %r8
	imulq	%r9, %r10
	shrq	$32, %r11
	imulq	%r9, %rsi
	imulq	%r9, %r14
	shrq	$32, %r8
	imulq	%r9, %rcx
	shrq	$32, %r10
	imulq	%r9, %r15
	shrq	$32, %rsi
	imulq	%r9, %rdx
	shrq	$32, %r14
	imulq	%r9, %rdi
	movq	%r11, %r9
	shrq	$32, %rcx
	salq	$6, %r9
	shrq	$32, %r15
	vmovdqa	(%rax,%r9), %ymm2
	movq	%r8, %r9
	shrq	$32, %rdx
	leaq	4(,%r8,8), %r8
	salq	$6, %r9
	shrq	$32, %rdi
	vmovdqa	(%rax,%r9), %ymm3
	movq	%r10, %r9
	salq	$6, %r9
	vmovdqa	(%rax,%r9), %ymm1
	movq	%rsi, %r9
	leaq	4(,%rsi,8), %rsi
	salq	$6, %r9
	vpcmpgtq	%ymm2, %ymm1, %ymm4
	vmovdqa	(%rax,%r9), %ymm6
	movq	%r14, %r9
	salq	$6, %r9
	vpblendvb	%ymm4, %ymm2, %ymm1, %ymm0
	vpblendvb	%ymm4, %ymm1, %ymm2, %ymm2
	vpcmpgtq	%ymm0, %ymm3, %ymm1
	vpblendvb	%ymm1, %ymm3, %ymm0, %ymm0
	vmovdqa	(%rax,%r9), %ymm3
	movq	%rcx, %r9
	leaq	4(,%rcx,8), %rcx
	vpcmpgtq	%ymm0, %ymm2, %ymm1
	salq	$6, %r9
	vpblendvb	%ymm1, %ymm0, %ymm2, %ymm2
	vmovdqa	(%rax,%r9), %ymm1
	movq	%r15, %r9
	salq	$6, %r9
	vmovdqa	%ymm2, (%r12)
	vpcmpgtq	%ymm6, %ymm1, %ymm4
	vpblendvb	%ymm4, %ymm6, %ymm1, %ymm0
	vpblendvb	%ymm4, %ymm1, %ymm6, %ymm6
	vmovdqa	(%rax,%r9), %ymm4
	movq	%rdx, %r9
	vpcmpgtq	%ymm0, %ymm3, %ymm1
	salq	$6, %r9
	leaq	4(,%rdx,8), %rdx
	vpblendvb	%ymm1, %ymm3, %ymm0, %ymm0
	vmovdqa	(%rax,%r9), %ymm3
	movq	%rdi, %r9
	vpcmpgtq	%ymm0, %ymm6, %ymm1
	salq	$6, %r9
	vpblendvb	%ymm1, %ymm0, %ymm6, %ymm6
	vmovdqa	(%rax,%r9), %ymm1
	leaq	4(,%r11,8), %r9
	vmovdqa	%ymm6, 64(%r12)
	vpcmpgtq	%ymm4, %ymm1, %ymm5
	vpblendvb	%ymm5, %ymm4, %ymm1, %ymm0
	vpblendvb	%ymm5, %ymm1, %ymm4, %ymm4
	vmovdqa	(%rax,%r8,8), %ymm5
	leaq	4(,%r10,8), %r8
	vpcmpgtq	%ymm0, %ymm3, %ymm1
	vpblendvb	%ymm1, %ymm3, %ymm0, %ymm0
	vmovdqa	(%rax,%r8,8), %ymm3
	vpcmpgtq	%ymm0, %ymm4, %ymm1
	vpblendvb	%ymm1, %ymm0, %ymm4, %ymm4
	vmovdqa	(%rax,%r9,8), %ymm0
	vmovdqa	%ymm4, 128(%r12)
	vpcmpgtq	%ymm0, %ymm3, %ymm7
	vpblendvb	%ymm7, %ymm0, %ymm3, %ymm1
	vpblendvb	%ymm7, %ymm3, %ymm0, %ymm0
	vpcmpgtq	%ymm1, %ymm5, %ymm3
	vpblendvb	%ymm3, %ymm5, %ymm1, %ymm1
	vmovdqa	(%rax,%rcx,8), %ymm5
	leaq	4(,%r15,8), %rcx
	vpcmpgtq	%ymm1, %ymm0, %ymm3
	vpblendvb	%ymm3, %ymm1, %ymm0, %ymm0
	vmovdqa	(%rax,%rsi,8), %ymm1
	leaq	4(,%r14,8), %rsi
	leaq	192(%r12), %r14
	vmovdqa	(%rax,%rsi,8), %ymm7
	vmovdqa	%ymm0, 32(%r12)
	vpcmpgtq	%ymm1, %ymm5, %ymm8
	vpblendvb	%ymm8, %ymm1, %ymm5, %ymm3
	vpblendvb	%ymm8, %ymm5, %ymm1, %ymm1
	vmovdqa	(%rax,%rdx,8), %ymm8
	leaq	4(,%rdi,8), %rdx
	vpcmpgtq	%ymm3, %ymm7, %ymm5
	vpblendvb	%ymm5, %ymm7, %ymm3, %ymm3
	vmovdqa	(%rax,%rdx,8), %ymm7
	vpcmpgtq	%ymm3, %ymm1, %ymm5
	vpblendvb	%ymm5, %ymm3, %ymm1, %ymm5
	vmovdqa	(%rax,%rcx,8), %ymm1
	vmovdqa	%ymm5, 96(%r12)
	vpcmpgtq	%ymm1, %ymm7, %ymm9
	vpblendvb	%ymm9, %ymm1, %ymm7, %ymm3
	vpblendvb	%ymm9, %ymm7, %ymm1, %ymm1
	vpcmpgtq	%ymm3, %ymm8, %ymm7
	vpblendvb	%ymm7, %ymm8, %ymm3, %ymm3
	vpcmpgtq	%ymm3, %ymm1, %ymm7
	vpblendvb	%ymm7, %ymm3, %ymm1, %ymm1
	vpbroadcastq	%xmm2, %ymm3
	vpxor	%ymm2, %ymm3, %ymm2
	vpxor	%ymm0, %ymm3, %ymm0
	vpxor	%ymm6, %ymm3, %ymm6
	vmovdqa	%ymm1, 160(%r12)
	vpor	%ymm2, %ymm0, %ymm0
	vpxor	%ymm5, %ymm3, %ymm5
	vpxor	%ymm4, %ymm3, %ymm4
	vpor	%ymm6, %ymm0, %ymm0
	vpxor	%ymm1, %ymm3, %ymm1
	vpxor	%xmm2, %xmm2, %xmm2
	vpor	%ymm5, %ymm0, %ymm0
	vpor	%ymm4, %ymm0, %ymm0
	vpor	%ymm1, %ymm0, %ymm0
	vpxor	192(%r12), %ymm3, %ymm1
	vpor	%ymm0, %ymm1, %ymm1
	vpblendvb	%ymm2, %ymm1, %ymm0, %ymm0
	vpxor	%xmm1, %xmm1, %xmm1
	vpcmpeqq	%ymm1, %ymm0, %ymm0
	vmovmskpd	%ymm0, %eax
	cmpl	$15, %eax
	je	.L1503
	vmovdqa	.LC13(%rip), %ymm0
	movl	$2, %esi
	movq	%r12, %rdi
	vmovdqu	%ymm0, 192(%r12)
	vmovdqu	%ymm0, 224(%r12)
	vmovdqu	%ymm0, 256(%r12)
	vzeroupper
	call	_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	vpbroadcastq	(%r12), %ymm2
	vpcmpeqd	%ymm0, %ymm0, %ymm0
	vpbroadcastq	184(%r12), %ymm1
	vpaddq	%ymm0, %ymm1, %ymm0
	vpcmpeqq	%ymm2, %ymm0, %ymm0
	vmovmskpd	%ymm0, %eax
	cmpl	$15, %eax
	jne	.L1505
	movq	-88(%rbp), %rsi
	leaq	-80(%rbp), %rdx
	movq	%r14, %rcx
	vmovdqa	%ymm2, %ymm0
	movq	%r13, %rdi
	call	_ZN3hwy6N_AVX26detail22MaybePartitionTwoValueINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L1649
.L1505:
	movq	96(%r12), %rdx
	cmpq	%rdx, 88(%r12)
	jne	.L1585
	cmpq	80(%r12), %rdx
	jne	.L1540
	cmpq	72(%r12), %rdx
	jne	.L1586
	cmpq	64(%r12), %rdx
	jne	.L1587
	cmpq	56(%r12), %rdx
	jne	.L1588
	cmpq	48(%r12), %rdx
	jne	.L1589
	cmpq	40(%r12), %rdx
	jne	.L1590
	cmpq	32(%r12), %rdx
	jne	.L1591
	cmpq	24(%r12), %rdx
	jne	.L1592
	cmpq	16(%r12), %rdx
	jne	.L1593
	cmpq	8(%r12), %rdx
	jne	.L1594
	movq	(%r12), %rax
	cmpq	%rax, %rdx
	jne	.L1656
.L1542:
	vmovq	%rax, %xmm7
	vpbroadcastq	%xmm7, %ymm0
.L1653:
	movl	$1, -112(%rbp)
.L1538:
	cmpq	$0, -96(%rbp)
	je	.L1657
	movq	-88(%rbp), %rax
	leaq	-4(%rax), %r10
	movq	%r10, %rsi
	movq	%r10, %rdx
	vmovdqu	0(%r13,%r10,8), %ymm4
	andl	$15, %esi
	andl	$12, %edx
	je	.L1596
	vmovdqu	0(%r13), %ymm1
	vpcmpeqd	%ymm2, %ymm2, %ymm2
	leaq	_ZZN3hwy6N_AVX26detail18IndicesFromBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices(%rip), %r8
	xorl	%ecx, %ecx
	vpcmpgtq	%ymm0, %ymm1, %ymm5
	vpxor	%ymm2, %ymm5, %ymm3
	vmovmskpd	%ymm3, %eax
	movq	%rax, %rdx
	popcntq	%rax, %rax
	leaq	0(%r13,%rax,8), %rax
	salq	$5, %rdx
	vmovdqa	(%r8,%rdx), %ymm3
	vmovmskpd	%ymm5, %edx
	popcntq	%rdx, %rcx
	salq	$5, %rdx
	vpslld	$28, %ymm3, %ymm6
	vpermd	%ymm1, %ymm3, %ymm3
	vpmaskmovq	%ymm3, %ymm6, 0(%r13)
	vmovdqa	(%r8,%rdx), %ymm3
	vpermd	%ymm1, %ymm3, %ymm1
	vmovdqu	%ymm1, (%r12)
	testb	$8, %r10b
	je	.L1548
	vmovdqu	32(%r13), %ymm1
	vpcmpgtq	%ymm0, %ymm1, %ymm5
	vpxor	%ymm2, %ymm5, %ymm3
	vmovmskpd	%ymm3, %edx
	movq	%rdx, %rdi
	popcntq	%rdx, %rdx
	salq	$5, %rdi
	vmovdqa	(%r8,%rdi), %ymm3
	vpslld	$28, %ymm3, %ymm6
	vpermd	%ymm1, %ymm3, %ymm3
	vpmaskmovq	%ymm3, %ymm6, (%rax)
	leaq	(%rax,%rdx,8), %rax
	vmovmskpd	%ymm5, %edx
	movq	%rdx, %rdi
	popcntq	%rdx, %rdx
	salq	$5, %rdi
	vmovdqa	(%r8,%rdi), %ymm3
	vpermd	%ymm1, %ymm3, %ymm1
	vmovdqu	%ymm1, (%r12,%rcx,8)
	addq	%rdx, %rcx
	cmpq	$11, %rsi
	jbe	.L1548
	vmovdqu	64(%r13), %ymm1
	vpcmpgtq	%ymm0, %ymm1, %ymm3
	vpxor	%ymm2, %ymm3, %ymm2
	vmovmskpd	%ymm2, %edx
	movq	%rdx, %rdi
	popcntq	%rdx, %rdx
	salq	$5, %rdi
	vmovdqa	(%r8,%rdi), %ymm2
	vpslld	$28, %ymm2, %ymm5
	vpermd	%ymm1, %ymm2, %ymm2
	vpmaskmovq	%ymm2, %ymm5, (%rax)
	leaq	(%rax,%rdx,8), %rax
	vmovmskpd	%ymm3, %edx
	movq	%rdx, %rdi
	popcntq	%rdx, %rdx
	salq	$5, %rdi
	vmovdqa	(%r8,%rdi), %ymm2
	vpermd	%ymm1, %ymm2, %ymm1
	vmovdqu	%ymm1, (%r12,%rcx,8)
	addq	%rdx, %rcx
.L1548:
	leaq	-4(%rsi), %rdx
	leaq	1(%rsi), %rdi
	andq	$-4, %rdx
	leaq	0(,%rcx,8), %r9
	addq	$4, %rdx
	cmpq	$4, %rdi
	movl	$4, %edi
	cmovbe	%rdi, %rdx
.L1547:
	cmpq	%rsi, %rdx
	je	.L1549
	subq	%rdx, %rsi
	vmovdqu	0(%r13,%rdx,8), %ymm2
	vmovq	%rsi, %xmm1
	vpbroadcastq	%xmm1, %ymm1
	vpcmpgtq	%ymm0, %ymm2, %ymm3
	vpcmpgtq	.LC3(%rip), %ymm1, %ymm1
	vpandn	%ymm1, %ymm3, %ymm5
	vpand	%ymm1, %ymm3, %ymm3
	vmovmskpd	%ymm5, %edx
	movq	%rdx, %rsi
	popcntq	%rdx, %rdx
	salq	$5, %rsi
	vmovdqa	(%r8,%rsi), %ymm5
	vpslld	$28, %ymm5, %ymm6
	vpermd	%ymm2, %ymm5, %ymm5
	vpmaskmovq	%ymm5, %ymm6, (%rax)
	leaq	(%rax,%rdx,8), %rax
	vmovmskpd	%ymm3, %edx
	movq	%rdx, %rsi
	popcntq	%rdx, %rdx
	addq	%rdx, %rcx
	salq	$5, %rsi
	vmovdqa	(%r8,%rsi), %ymm1
	vpermd	%ymm2, %ymm1, %ymm2
	vmovdqu	%ymm2, (%r12,%r9)
	leaq	0(,%rcx,8), %r9
.L1549:
	movq	%r10, %rdx
	subq	%rcx, %rdx
	leaq	0(%r13,%rdx,8), %r11
	cmpl	$8, %r9d
	jnb	.L1550
	testl	%r9d, %r9d
	jne	.L1658
.L1551:
	cmpl	$8, %r9d
	jnb	.L1554
	testl	%r9d, %r9d
	jne	.L1659
.L1555:
	movq	%rax, %rcx
	subq	%r13, %rcx
	sarq	$3, %rcx
	subq	%rcx, %r10
	subq	%rcx, %rdx
	movq	%rcx, %r15
	movq	%r10, -144(%rbp)
	leaq	(%rax,%rdx,8), %rcx
	je	.L1597
	leaq	128(%rax), %rsi
	leaq	-128(%rcx), %r10
	vmovdqu	(%rax), %ymm12
	vmovdqu	32(%rax), %ymm11
	vmovdqu	64(%rax), %ymm10
	vmovdqu	96(%rax), %ymm9
	vmovdqu	-128(%rcx), %ymm8
	vmovdqu	-96(%rcx), %ymm7
	vmovdqu	-64(%rcx), %ymm6
	vmovdqu	-32(%rcx), %ymm5
	cmpq	%r10, %rsi
	je	.L1598
	xorl	%ecx, %ecx
	leaq	_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices(%rip), %rdi
	movl	$4, %r11d
	jmp	.L1562
	.p2align 4,,10
	.p2align 3
.L1661:
	vmovdqu	-128(%r10), %ymm13
	vmovdqu	-96(%r10), %ymm3
	prefetcht0	-512(%r10)
	addq	$-128, %r10
	vmovdqu	64(%r10), %ymm2
	vmovdqu	96(%r10), %ymm1
.L1561:
	vpcmpgtq	%ymm0, %ymm13, %ymm14
	vmovmskpd	%ymm14, %r9d
	movq	%r9, %r14
	popcntq	%r9, %r9
	salq	$5, %r14
	vmovdqa	(%rdi,%r14), %ymm14
	leaq	-4(%rdx,%rcx), %r14
	vpermd	%ymm13, %ymm14, %ymm13
	vmovdqu	%ymm13, (%rax,%rcx,8)
	addq	$4, %rcx
	vmovdqu	%ymm13, (%rax,%r14,8)
	vpcmpgtq	%ymm0, %ymm3, %ymm13
	subq	%r9, %rcx
	vmovmskpd	%ymm13, %r9d
	movq	%r9, %r14
	popcntq	%r9, %r9
	salq	$5, %r14
	vmovdqa	(%rdi,%r14), %ymm13
	leaq	-8(%rcx,%rdx), %r14
	vpermd	%ymm3, %ymm13, %ymm3
	vmovdqu	%ymm3, (%rax,%rcx,8)
	vmovdqu	%ymm3, (%rax,%r14,8)
	vpcmpgtq	%ymm0, %ymm2, %ymm3
	movq	%r11, %r14
	subq	%r9, %r14
	addq	%r14, %rcx
	vmovmskpd	%ymm3, %r9d
	movq	%r9, %r14
	popcntq	%r9, %r9
	salq	$5, %r14
	vmovdqa	(%rdi,%r14), %ymm3
	leaq	-12(%rcx,%rdx), %r14
	subq	$16, %rdx
	vpermd	%ymm2, %ymm3, %ymm2
	vmovdqu	%ymm2, (%rax,%rcx,8)
	vmovdqu	%ymm2, (%rax,%r14,8)
	vpcmpgtq	%ymm0, %ymm1, %ymm2
	movq	%r11, %r14
	subq	%r9, %r14
	leaq	(%r14,%rcx), %r9
	vmovmskpd	%ymm2, %ecx
	movq	%rcx, %r14
	popcntq	%rcx, %rcx
	salq	$5, %r14
	vmovdqa	(%rdi,%r14), %ymm2
	leaq	(%r9,%rdx), %r14
	vpermd	%ymm1, %ymm2, %ymm1
	vmovdqu	%ymm1, (%rax,%r9,8)
	vmovdqu	%ymm1, (%rax,%r14,8)
	movq	%r11, %r14
	subq	%rcx, %r14
	leaq	(%r14,%r9), %rcx
	cmpq	%r10, %rsi
	je	.L1660
.L1562:
	movq	%rsi, %r9
	subq	%rax, %r9
	sarq	$3, %r9
	subq	%rcx, %r9
	cmpq	$16, %r9
	ja	.L1661
	vmovdqu	(%rsi), %ymm13
	vmovdqu	32(%rsi), %ymm3
	prefetcht0	512(%rsi)
	subq	$-128, %rsi
	vmovdqu	-64(%rsi), %ymm2
	vmovdqu	-32(%rsi), %ymm1
	jmp	.L1561
	.p2align 4,,10
	.p2align 3
.L1655:
	movl	$8, %eax
	subq	%rdx, %rax
	leaq	(%rdi,%rax,8), %rax
	movq	-88(%rbp), %rdi
	leaq	-8(%rdx,%rdi), %r14
	jmp	.L1501
	.p2align 4,,10
	.p2align 3
.L1660:
	leaq	(%rdx,%rcx), %r9
	leaq	(%rax,%rcx,8), %r10
	addq	$4, %rcx
.L1559:
	vpcmpgtq	%ymm0, %ymm12, %ymm1
	vmovmskpd	%ymm1, %esi
	movq	%rsi, %r11
	popcntq	%rsi, %rsi
	subq	%rsi, %rcx
	salq	$5, %r11
	vmovdqa	(%rdi,%r11), %ymm1
	vpermd	%ymm12, %ymm1, %ymm12
	vpcmpgtq	%ymm0, %ymm11, %ymm1
	vmovdqu	%ymm12, (%r10)
	vmovdqu	%ymm12, -32(%rax,%r9,8)
	vmovmskpd	%ymm1, %esi
	movq	%rsi, %r9
	popcntq	%rsi, %rsi
	salq	$5, %r9
	vmovdqa	(%rdi,%r9), %ymm1
	leaq	-8(%rdx,%rcx), %r9
	vpermd	%ymm11, %ymm1, %ymm11
	vpcmpgtq	%ymm0, %ymm10, %ymm1
	vmovdqu	%ymm11, (%rax,%rcx,8)
	subq	%rsi, %rcx
	vmovdqu	%ymm11, (%rax,%r9,8)
	addq	$4, %rcx
	vmovmskpd	%ymm1, %r9d
	movq	%r9, %rsi
	popcntq	%r9, %r9
	salq	$5, %rsi
	vmovdqa	(%rdi,%rsi), %ymm1
	leaq	-12(%rdx,%rcx), %rsi
	vpermd	%ymm10, %ymm1, %ymm10
	vpcmpgtq	%ymm0, %ymm9, %ymm1
	vmovdqu	%ymm10, (%rax,%rcx,8)
	vmovdqu	%ymm10, (%rax,%rsi,8)
	movl	$4, %esi
	movq	%rsi, %r10
	subq	%r9, %r10
	vmovmskpd	%ymm1, %r9d
	addq	%r10, %rcx
	movq	%r9, %r10
	popcntq	%r9, %r9
	salq	$5, %r10
	vmovdqa	(%rdi,%r10), %ymm1
	leaq	-16(%rdx,%rcx), %r10
	vpermd	%ymm9, %ymm1, %ymm9
	vpcmpgtq	%ymm0, %ymm8, %ymm1
	vmovdqu	%ymm9, (%rax,%rcx,8)
	vmovdqu	%ymm9, (%rax,%r10,8)
	movq	%rsi, %r10
	subq	%r9, %r10
	leaq	(%r10,%rcx), %r9
	vmovmskpd	%ymm1, %ecx
	movq	%rcx, %r10
	popcntq	%rcx, %rcx
	salq	$5, %r10
	vmovdqa	(%rdi,%r10), %ymm1
	leaq	-20(%rdx,%r9), %r10
	vpermd	%ymm8, %ymm1, %ymm8
	vpcmpgtq	%ymm0, %ymm7, %ymm1
	vmovdqu	%ymm8, (%rax,%r9,8)
	vmovdqu	%ymm8, (%rax,%r10,8)
	movq	%rsi, %r10
	subq	%rcx, %r10
	leaq	(%r10,%r9), %rcx
	vmovmskpd	%ymm1, %r9d
	movq	%r9, %r10
	popcntq	%r9, %r9
	salq	$5, %r10
	vmovdqa	(%rdi,%r10), %ymm1
	leaq	-24(%rdx,%rcx), %r10
	vpermd	%ymm7, %ymm1, %ymm7
	vpcmpgtq	%ymm0, %ymm6, %ymm1
	vmovdqu	%ymm7, (%rax,%rcx,8)
	vmovdqu	%ymm7, (%rax,%r10,8)
	movq	%rsi, %r10
	subq	%r9, %r10
	leaq	(%r10,%rcx), %r9
	vmovmskpd	%ymm1, %ecx
	movq	%rcx, %r10
	salq	$5, %r10
	vmovdqa	(%rdi,%r10), %ymm1
	leaq	-28(%rdx,%r9), %r10
	vpermd	%ymm6, %ymm1, %ymm6
	vpcmpgtq	%ymm0, %ymm5, %ymm1
	vmovdqu	%ymm6, (%rax,%r9,8)
	vmovdqu	%ymm6, (%rax,%r10,8)
	xorl	%r10d, %r10d
	popcntq	%rcx, %r10
	movq	%rsi, %rcx
	subq	%r10, %rcx
	addq	%r9, %rcx
	vmovmskpd	%ymm1, %r9d
	movq	%r9, %r10
	leaq	-32(%rdx,%rcx), %rdx
	popcntq	%r9, %r9
	subq	%r9, %rsi
	salq	$5, %r10
	vmovdqa	(%rdi,%r10), %ymm1
	movq	-144(%rbp), %rdi
	vpermd	%ymm5, %ymm1, %ymm5
	vmovdqu	%ymm5, (%rax,%rcx,8)
	vmovdqu	%ymm5, (%rax,%rdx,8)
	leaq	(%rsi,%rcx), %rdx
	subq	%rdx, %rdi
	leaq	(%rax,%rdx,8), %rcx
.L1558:
	movq	%rcx, %rsi
	cmpq	$3, %rdi
	ja	.L1563
	movq	-144(%rbp), %rdi
	leaq	-32(%rax,%rdi,8), %rsi
.L1563:
	vpcmpgtq	%ymm0, %ymm4, %ymm0
	vpcmpeqd	%ymm1, %ymm1, %ymm1
	vmovdqu	(%rsi), %ymm6
	movq	-144(%rbp), %rdi
	vmovdqu	%ymm6, (%rax,%rdi,8)
	vpxor	%ymm1, %ymm0, %ymm1
	vmovdqa	%ymm6, -176(%rbp)
	vmovmskpd	%ymm1, %esi
	movq	%rsi, %rdi
	popcntq	%rsi, %rsi
	addq	%rsi, %rdx
	salq	$5, %rdi
	leaq	(%r15,%rdx), %r14
	vmovdqa	(%r8,%rdi), %ymm1
	vpslld	$28, %ymm1, %ymm2
	vpermd	%ymm4, %ymm1, %ymm1
	vpmaskmovq	%ymm1, %ymm2, (%rcx)
	vmovmskpd	%ymm0, %ecx
	salq	$5, %rcx
	vmovdqa	(%r8,%rcx), %ymm0
	vpslld	$28, %ymm0, %ymm1
	vpermd	%ymm4, %ymm0, %ymm4
	vpmaskmovq	%ymm4, %ymm1, (%rax,%rdx,8)
	movq	-96(%rbp), %r15
	subq	$1, %r15
	cmpl	$2, -112(%rbp)
	je	.L1662
	movq	-104(%rbp), %rsi
	movq	%r15, %r9
	movq	%rbx, %r8
	movq	%r12, %rcx
	movq	%r14, %rdx
	movq	%r13, %rdi
	vzeroupper
	call	_ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	cmpl	$3, -112(%rbp)
	je	.L1649
.L1565:
	movq	-88(%rbp), %rdx
	movq	-104(%rbp), %rsi
	leaq	0(%r13,%r14,8), %rdi
	movq	%r15, %r9
	movq	%rbx, %r8
	movq	%r12, %rcx
	subq	%r14, %rdx
	call	_ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L1649:
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
.L1554:
	.cfi_restore_state
	movq	(%r12), %rcx
	leaq	8(%r11), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%r11)
	movl	%r9d, %ecx
	movq	-8(%r12,%rcx), %rsi
	movq	%rsi, -8(%r11,%rcx)
	subq	%rdi, %r11
	movq	%r12, %rsi
	leal	(%r9,%r11), %ecx
	subq	%r11, %rsi
	shrl	$3, %ecx
	rep movsq
	jmp	.L1555
	.p2align 4,,10
	.p2align 3
.L1550:
	movq	(%r11), %rcx
	leaq	8(%rax), %rdi
	andq	$-8, %rdi
	movq	%rcx, (%rax)
	movl	%r9d, %ecx
	movq	-8(%r11,%rcx), %rsi
	movq	%rsi, -8(%rax,%rcx)
	movq	%rax, %rcx
	movq	%r11, %rsi
	subq	%rdi, %rcx
	subq	%rcx, %rsi
	addl	%r9d, %ecx
	shrl	$3, %ecx
	rep movsq
	jmp	.L1551
	.p2align 4,,10
	.p2align 3
.L1659:
	movzbl	(%r12), %ecx
	movb	%cl, (%r11)
	jmp	.L1555
	.p2align 4,,10
	.p2align 3
.L1658:
	movzbl	(%r11), %ecx
	movb	%cl, (%rax)
	jmp	.L1551
	.p2align 4,,10
	.p2align 3
.L1654:
	cmpq	$1, %rdx
	jbe	.L1649
	leaq	512(%rdi), %rax
	cmpq	%rax, %rsi
	jb	.L1663
	movl	$4, %esi
	call	_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	jmp	.L1649
	.p2align 4,,10
	.p2align 3
.L1503:
	movq	-144(%rbp), %rax
	movl	$4, %edi
	vmovdqa	.LC3(%rip), %ymm4
	vpcmpeqq	0(%r13), %ymm3, %ymm0
	andl	$3, %eax
	subq	%rax, %rdi
	vmovq	%rdi, %xmm6
	vpbroadcastq	%xmm6, %ymm1
	vpcmpgtq	%ymm4, %ymm1, %ymm1
	vpandn	%ymm1, %ymm0, %ymm0
	vmovmskpd	%ymm0, %eax
	testl	%eax, %eax
	jne	.L1664
	vpxor	%xmm1, %xmm1, %xmm1
	movq	-88(%rbp), %r8
	leaq	512(%r13,%rdi,8), %rsi
	vmovdqa	%ymm1, %ymm0
	vmovdqa	%ymm1, %ymm5
	.p2align 4,,10
	.p2align 3
.L1509:
	movq	%rdi, %rcx
	leaq	64(%rdi), %rdi
	cmpq	%rdi, %r8
	jb	.L1665
	leaq	-512(%rsi), %rax
.L1508:
	vpxor	(%rax), %ymm3, %ymm2
	leaq	64(%rax), %rdx
	vpor	%ymm0, %ymm2, %ymm0
	vpxor	32(%rax), %ymm3, %ymm2
	vpor	%ymm1, %ymm2, %ymm1
	vpxor	64(%rax), %ymm3, %ymm2
	vpor	%ymm0, %ymm2, %ymm0
	vpxor	96(%rax), %ymm3, %ymm2
	vpor	%ymm1, %ymm2, %ymm1
	vpxor	128(%rax), %ymm3, %ymm2
	vpor	%ymm0, %ymm2, %ymm0
	vpxor	160(%rax), %ymm3, %ymm2
	leaq	192(%rdx), %rax
	vpor	%ymm1, %ymm2, %ymm1
	vpxor	128(%rdx), %ymm3, %ymm2
	vpor	%ymm0, %ymm2, %ymm0
	vpxor	160(%rdx), %ymm3, %ymm2
	vpor	%ymm1, %ymm2, %ymm1
	cmpq	%rsi, %rax
	jne	.L1508
	vpor	%ymm1, %ymm0, %ymm2
	leaq	704(%rdx), %rsi
	vpcmpeqq	%ymm5, %ymm2, %ymm2
	vmovmskpd	%ymm2, %eax
	cmpl	$15, %eax
	je	.L1509
	vpcmpeqq	0(%r13,%rcx,8), %ymm3, %ymm0
	vpcmpeqd	%ymm1, %ymm1, %ymm1
	vpxor	%ymm1, %ymm0, %ymm0
	vmovmskpd	%ymm0, %eax
	testl	%eax, %eax
	jne	.L1511
	.p2align 4,,10
	.p2align 3
.L1510:
	addq	$4, %rcx
	vpcmpeqq	0(%r13,%rcx,8), %ymm3, %ymm0
	vpxor	%ymm1, %ymm0, %ymm0
	vmovmskpd	%ymm0, %eax
	testl	%eax, %eax
	je	.L1510
.L1511:
	tzcntl	%eax, %eax
	addq	%rcx, %rax
.L1507:
	vpbroadcastq	0(%r13,%rax,8), %ymm1
	leaq	0(%r13,%rax,8), %rdi
	vpcmpgtq	%ymm3, %ymm1, %ymm0
	vmovmskpd	%ymm0, %edx
	testl	%edx, %edx
	jne	.L1516
	movq	-88(%rbp), %rsi
	xorl	%ecx, %ecx
	leaq	-4(%rsi), %rax
	jmp	.L1521
	.p2align 4,,10
	.p2align 3
.L1517:
	vmovmskpd	%ymm0, %edx
	vmovdqu	%ymm3, 0(%r13,%rax,8)
	popcntq	%rdx, %rdx
	addq	%rdx, %rcx
	leaq	-4(%rax), %rdx
	cmpq	%rdx, %rsi
	jbe	.L1666
	movq	%rdx, %rax
.L1521:
	vpcmpeqq	0(%r13,%rax,8), %ymm1, %ymm2
	vpcmpeqq	0(%r13,%rax,8), %ymm3, %ymm0
	vpor	%ymm0, %ymm2, %ymm5
	vmovmskpd	%ymm5, %edx
	cmpl	$15, %edx
	je	.L1517
	vpcmpeqd	%ymm3, %ymm3, %ymm3
	leaq	4(%rax), %rsi
	vpxor	%ymm3, %ymm0, %ymm0
	vpandn	%ymm0, %ymm2, %ymm2
	vmovmskpd	%ymm2, %edx
	tzcntl	%edx, %edx
	addq	%rax, %rdx
	addq	$8, %rax
	vpbroadcastq	0(%r13,%rdx,8), %ymm3
	movq	-88(%rbp), %rdx
	subq	%rcx, %rdx
	vmovdqa	%ymm3, -80(%rbp)
	cmpq	%rdx, %rax
	ja	.L1518
	.p2align 4,,10
	.p2align 3
.L1519:
	vmovdqu	%ymm1, -32(%r13,%rax,8)
	movq	%rax, %rsi
	addq	$4, %rax
	cmpq	%rax, %rdx
	jnb	.L1519
.L1518:
	subq	%rsi, %rdx
	vmovq	%rdx, %xmm6
	vpbroadcastq	%xmm6, %ymm0
	vpcmpgtq	%ymm4, %ymm0, %ymm0
	vpmaskmovq	%ymm1, %ymm0, 0(%r13,%rsi,8)
.L1520:
	vpbroadcastq	(%r12), %ymm0
	vpcmpeqq	.LC14(%rip), %ymm0, %ymm2
	vmovmskpd	%ymm2, %eax
	cmpl	$15, %eax
	je	.L1583
	vpcmpeqq	.LC13(%rip), %ymm0, %ymm2
	vmovmskpd	%ymm2, %eax
	cmpl	$15, %eax
	je	.L1534
	vpcmpgtq	%ymm1, %ymm3, %ymm4
	vpblendvb	%ymm4, %ymm1, %ymm3, %ymm2
	vpcmpgtq	%ymm2, %ymm0, %ymm2
	vmovmskpd	%ymm2, %eax
	testl	%eax, %eax
	jne	.L1667
	vmovdqa	%ymm0, %ymm2
	movl	$64, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L1529:
	leaq	(%rcx,%rax,4), %rdx
	addq	$1, %rax
	vmovdqu	0(%r13,%rdx,8), %ymm1
	vpcmpgtq	%ymm2, %ymm1, %ymm3
	vpblendvb	%ymm3, %ymm2, %ymm1, %ymm1
	vmovdqa	%ymm1, %ymm2
	cmpq	$16, %rax
	jne	.L1529
	vpcmpgtq	%ymm1, %ymm0, %ymm1
	vmovmskpd	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1653
	leaq	64(%rsi), %rax
	cmpq	%rax, -88(%rbp)
	jb	.L1668
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L1529
	.p2align 4,,10
	.p2align 3
.L1596:
	xorl	%r9d, %r9d
	xorl	%ecx, %ecx
	leaq	_ZZN3hwy6N_AVX26detail18IndicesFromBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices(%rip), %r8
	movq	%r13, %rax
	jmp	.L1547
	.p2align 4,,10
	.p2align 3
.L1662:
	vzeroupper
	jmp	.L1565
	.p2align 4,,10
	.p2align 3
.L1597:
	movq	%r10, %rdi
	movq	%rax, %rcx
	jmp	.L1558
	.p2align 4,,10
	.p2align 3
.L1598:
	movq	%rax, %r10
	movq	%rdx, %r9
	movl	$4, %ecx
	leaq	_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices(%rip), %rdi
	jmp	.L1559
.L1657:
	movq	-88(%rbp), %rsi
	leaq	-1(%rsi), %rbx
	movq	%rbx, %r12
	shrq	%r12
	.p2align 4,,10
	.p2align 3
.L1545:
	movq	%r12, %rdx
	movq	%r13, %rdi
	call	_ZN3hwy6N_AVX26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	subq	$1, %r12
	jnb	.L1545
	.p2align 4,,10
	.p2align 3
.L1546:
	movq	0(%r13,%rbx,8), %rdx
	movq	0(%r13), %rax
	movq	%rbx, %rsi
	movq	%r13, %rdi
	movq	%rdx, 0(%r13)
	xorl	%edx, %edx
	movq	%rax, 0(%r13,%rbx,8)
	call	_ZN3hwy6N_AVX26detail8SiftDownINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_mm.isra.0
	subq	$1, %rbx
	jne	.L1546
	jmp	.L1649
.L1585:
	movl	$12, %eax
	movl	$11, %esi
	jmp	.L1543
	.p2align 4,,10
	.p2align 3
.L1544:
	cmpq	$23, %rax
	je	.L1652
.L1543:
	movq	%rax, %rcx
	addq	$1, %rax
	cmpq	(%r12,%rax,8), %rdx
	je	.L1544
	movl	$12, %edi
	subq	$11, %rcx
	movq	%rdx, %rax
	subq	%rsi, %rdi
	cmpq	%rdi, %rcx
	jb	.L1542
.L1652:
	movq	(%r12,%rsi,8), %rax
	jmp	.L1542
.L1586:
	movl	$9, %esi
	movl	$10, %eax
	jmp	.L1543
.L1587:
	movl	$8, %esi
	movl	$9, %eax
	jmp	.L1543
.L1588:
	movl	$7, %esi
	movl	$8, %eax
	jmp	.L1543
.L1589:
	movl	$6, %esi
	movl	$7, %eax
	jmp	.L1543
.L1665:
	movq	-88(%rbp), %rsi
	vpcmpeqd	%ymm1, %ymm1, %ymm1
	.p2align 4,,10
	.p2align 3
.L1513:
	movq	%rcx, %rdx
	addq	$4, %rcx
	cmpq	%rcx, %rsi
	jb	.L1669
	vpcmpeqq	-32(%r13,%rcx,8), %ymm3, %ymm0
	vpxor	%ymm1, %ymm0, %ymm0
	vmovmskpd	%ymm0, %eax
	testl	%eax, %eax
	je	.L1513
.L1651:
	tzcntl	%eax, %eax
	addq	%rdx, %rax
	jmp	.L1507
.L1590:
	movl	$5, %esi
	movl	$6, %eax
	jmp	.L1543
.L1591:
	movl	$4, %esi
	movl	$5, %eax
	jmp	.L1543
.L1516:
	movq	-88(%rbp), %rsi
	leaq	-80(%rbp), %rdx
	movq	%r12, %rcx
	vmovdqa	%ymm3, %ymm0
	vmovdqa	%ymm1, -144(%rbp)
	subq	%rax, %rsi
	call	_ZN3hwy6N_AVX26detail22MaybePartitionTwoValueINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEbT_T0_PT1_mDTcl4ZerocvSB__EEESF_RSF_SE_.isra.0
	testb	%al, %al
	jne	.L1649
	vmovdqa	-80(%rbp), %ymm3
	vmovdqa	-144(%rbp), %ymm1
	jmp	.L1520
.L1592:
	movl	$3, %esi
	movl	$4, %eax
	jmp	.L1543
.L1593:
	movl	$2, %esi
	movl	$3, %eax
	jmp	.L1543
.L1594:
	movl	$1, %esi
	movl	$2, %eax
	jmp	.L1543
.L1656:
	xorl	%esi, %esi
	movl	$1, %eax
	jmp	.L1543
.L1666:
	vmovq	%rax, %xmm6
	vpcmpeqq	0(%r13), %ymm3, %ymm5
	movq	-88(%rbp), %rdx
	vpbroadcastq	%xmm6, %ymm2
	vpcmpeqq	0(%r13), %ymm1, %ymm0
	vpcmpgtq	%ymm4, %ymm2, %ymm2
	subq	%rcx, %rdx
	vpor	%ymm5, %ymm0, %ymm0
	vpand	%ymm2, %ymm5, %ymm6
	vpcmpeqd	%ymm5, %ymm5, %ymm5
	vpxor	%ymm5, %ymm2, %ymm2
	vpor	%ymm2, %ymm0, %ymm0
	vmovmskpd	%ymm0, %esi
	cmpl	$15, %esi
	jne	.L1670
	vmovmskpd	%ymm6, %ecx
	movq	%rdx, %rax
	vmovdqu	%ymm3, 0(%r13)
	popcntq	%rcx, %rcx
	subq	%rcx, %rax
	cmpq	$3, %rax
	jbe	.L1572
	leaq	-4(%rax), %rcx
	movq	-112(%rbp), %rsi
	movq	%rcx, %rdx
	shrq	$2, %rdx
	salq	$5, %rdx
	leaq	32(%r13,%rdx), %rdx
	.p2align 4,,10
	.p2align 3
.L1526:
	vmovdqu	%ymm1, (%rsi)
	addq	$32, %rsi
	cmpq	%rsi, %rdx
	jne	.L1526
	andq	$-4, %rcx
	leaq	4(%rcx), %rdx
	leaq	0(,%rdx,8), %rcx
	subq	%rdx, %rax
.L1525:
	vmovdqa	%ymm1, (%r12)
	testq	%rax, %rax
	je	.L1648
	leaq	0(%r13,%rcx), %rdi
	leaq	0(,%rax,8), %rdx
	movq	%r12, %rsi
	vzeroupper
	call	memcpy@PLT
	jmp	.L1649
.L1668:
	movq	-88(%rbp), %rdx
	jmp	.L1536
	.p2align 4,,10
	.p2align 3
.L1537:
	vpcmpgtq	-32(%r13,%rsi,8), %ymm0, %ymm1
	vmovmskpd	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1653
.L1536:
	movq	%rsi, %rax
	addq	$4, %rsi
	cmpq	%rsi, %rdx
	jnb	.L1537
	movq	-88(%rbp), %rdi
	cmpq	%rax, %rdi
	je	.L1583
	vpcmpgtq	-32(%r13,%rdi,8), %ymm0, %ymm1
	vmovmskpd	%ymm1, %eax
	cmpl	$1, %eax
	movl	$1, %eax
	adcl	$0, %eax
	movl	%eax, -112(%rbp)
	jmp	.L1538
.L1540:
	movl	$11, %eax
	movl	$10, %esi
	jmp	.L1543
.L1669:
	movq	-88(%rbp), %rax
	vpcmpeqd	%ymm1, %ymm1, %ymm1
	vpcmpeqq	-32(%r13,%rax,8), %ymm3, %ymm0
	leaq	-4(%rax), %rdx
	vpxor	%ymm1, %ymm0, %ymm0
	vmovmskpd	%ymm0, %eax
	testl	%eax, %eax
	jne	.L1651
.L1648:
	vzeroupper
	jmp	.L1649
.L1533:
	vmovdqu	-32(%r13,%rsi,8), %ymm6
	vpcmpgtq	%ymm0, %ymm6, %ymm1
	vmovmskpd	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1653
.L1532:
	movq	%rsi, %rax
	addq	$4, %rsi
	cmpq	%rsi, -88(%rbp)
	jnb	.L1533
	movq	-88(%rbp), %rdi
	cmpq	%rax, %rdi
	je	.L1534
	vmovdqu	-32(%r13,%rdi,8), %ymm6
	vpcmpgtq	%ymm0, %ymm6, %ymm1
	vmovdqa	%ymm6, -144(%rbp)
	vmovmskpd	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1653
.L1534:
	vpcmpeqd	%ymm1, %ymm1, %ymm1
	movl	$3, -112(%rbp)
	vpaddq	%ymm1, %ymm0, %ymm0
	jmp	.L1538
.L1663:
	movq	%rdx, %rcx
	xorl	%eax, %eax
	cmpq	$3, %rdx
	jbe	.L1494
	movq	%rdx, %rbx
	leaq	-4(%rdx), %rdx
	movq	(%rdi), %rcx
	movq	%rdx, %rax
	shrq	$2, %rax
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
	andq	$-4, %rax
	shrl	$3, %ecx
	rep movsq
	addq	$4, %rax
	subq	%rax, %rbx
	movq	%rbx, %rcx
	je	.L1497
.L1494:
	salq	$3, %rax
	leaq	0(,%rcx,8), %rdx
	testq	%rcx, %rcx
	movl	$8, %ecx
	cmove	%rcx, %rdx
	leaq	(%r12,%rax), %rdi
	leaq	0(%r13,%rax), %rsi
	call	memcpy@PLT
.L1497:
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
	addq	$4, %rdx
	cmpq	%rdx, %rbx
	jnb	.L1496
	vmovdqa	.LC13(%rip), %ymm0
	movq	%rbx, %rax
	.p2align 4,,10
	.p2align 3
.L1495:
	vmovdqu	%ymm0, (%r12,%rax,8)
	addq	$4, %rax
	cmpq	%rdx, %rax
	jb	.L1495
	vzeroupper
.L1496:
	movq	%r12, %rdi
	call	_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	xorl	%eax, %eax
	cmpq	$3, -88(%rbp)
	jbe	.L1499
	movq	-88(%rbp), %rbx
	movq	(%r12), %rcx
	leaq	8(%r13), %rdi
	andq	$-8, %rdi
	leaq	-4(%rbx), %rdx
	movq	%rcx, 0(%r13)
	movq	%rdx, %rax
	shrq	$2, %rax
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
	andq	$-4, %rax
	shrl	$3, %ecx
	rep movsq
	addq	$4, %rax
	subq	%rax, %rbx
	movq	%rbx, -88(%rbp)
	je	.L1649
.L1499:
	movq	-88(%rbp), %rbx
	salq	$3, %rax
	movl	$8, %ecx
	leaq	0(%r13,%rax), %rdi
	leaq	(%r12,%rax), %rsi
	leaq	0(,%rbx,8), %rdx
	testq	%rbx, %rbx
	cmove	%rcx, %rdx
	call	memcpy@PLT
	jmp	.L1649
.L1664:
	tzcntl	%eax, %eax
	jmp	.L1507
.L1583:
	movl	$2, -112(%rbp)
	jmp	.L1538
.L1667:
	vpblendvb	%ymm4, %ymm3, %ymm1, %ymm1
	vpcmpgtq	%ymm0, %ymm1, %ymm1
	vmovmskpd	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1653
	vmovdqa	%ymm0, %ymm2
	movl	$64, %esi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L1530:
	leaq	(%rcx,%rax,4), %rdx
	addq	$1, %rax
	vmovdqu	0(%r13,%rdx,8), %ymm1
	vpcmpgtq	%ymm2, %ymm1, %ymm3
	vpblendvb	%ymm3, %ymm1, %ymm2, %ymm1
	vmovdqa	%ymm1, %ymm2
	cmpq	$16, %rax
	jne	.L1530
	vpcmpgtq	%ymm0, %ymm1, %ymm1
	vmovmskpd	%ymm1, %eax
	testl	%eax, %eax
	jne	.L1653
	leaq	64(%rsi), %rax
	cmpq	%rax, -88(%rbp)
	jb	.L1532
	movq	%rsi, %rcx
	movq	%rax, %rsi
	xorl	%eax, %eax
	jmp	.L1530
.L1572:
	xorl	%ecx, %ecx
	jmp	.L1525
.L1670:
	vpxor	%ymm5, %ymm0, %ymm0
	vmovmskpd	%ymm0, %ecx
	tzcntl	%ecx, %ecx
	vpbroadcastq	0(%r13,%rcx,8), %ymm3
	leaq	4(%rax), %rcx
	vmovdqa	%ymm3, -80(%rbp)
	cmpq	%rdx, %rcx
	ja	.L1523
.L1524:
	vmovdqu	%ymm1, -32(%r13,%rcx,8)
	movq	%rcx, %rax
	addq	$4, %rcx
	cmpq	%rdx, %rcx
	jbe	.L1524
.L1523:
	subq	%rax, %rdx
	vmovq	%rdx, %xmm0
	vpbroadcastq	%xmm0, %ymm0
	vpcmpgtq	%ymm4, %ymm0, %ymm0
	vpmaskmovq	%ymm1, %ymm0, 0(%r13,%rax,8)
	jmp	.L1520
	.cfi_endproc
.LFE18808:
	.size	_ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0, .-_ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
	.section	.text.unlikely._ZN3hwy7N_SSSE310SortI64AscEPlm,"ax",@progbits
.LCOLDB15:
	.section	.text._ZN3hwy7N_SSSE310SortI64AscEPlm,"ax",@progbits
.LHOTB15:
	.p2align 4
	.globl	_ZN3hwy7N_SSSE310SortI64AscEPlm
	.hidden	_ZN3hwy7N_SSSE310SortI64AscEPlm
	.type	_ZN3hwy7N_SSSE310SortI64AscEPlm, @function
_ZN3hwy7N_SSSE310SortI64AscEPlm:
.LFB2946:
	.cfi_startproc
	cmpq	$1, %rsi
	jbe	.L1692
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
	movq	%rsi, %r12
	pushq	%rbx
	subq	$312, %rsp
	.cfi_offset 3, -56
	cmpq	$32, %rsi
	jbe	.L1695
	cmpq	$0, 16+_ZZN3hwy17GetGeneratorStateEvE5state(%rip)
	je	.L1696
.L1682:
	leaq	-336(%rbp), %rcx
	leaq	0(%r13,%r12,8), %rsi
	movq	%r12, %rdx
	movq	%r13, %rdi
	movl	$50, %r9d
	leaq	_ZZN3hwy17GetGeneratorStateEvE5state(%rip), %r8
	call	_ZN3hwy7N_SSSE36detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L1671:
	addq	$312, %rsp
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
.L1692:
	.cfi_restore 3
	.cfi_restore 6
	.cfi_restore 12
	.cfi_restore 13
	.cfi_restore 14
	.cfi_restore 15
	ret
	.p2align 4,,10
	.p2align 3
.L1695:
	.cfi_def_cfa 6, 16
	.cfi_offset 3, -56
	.cfi_offset 6, -16
	.cfi_offset 12, -48
	.cfi_offset 13, -40
	.cfi_offset 14, -32
	.cfi_offset 15, -24
	leaq	0(,%rsi,8), %r8
	leaq	256(%rdi), %rax
	leaq	(%rdi,%r8), %rdx
	cmpq	%rax, %rdx
	jb	.L1675
	movl	$2, %esi
	call	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	jmp	.L1671
	.p2align 4,,10
	.p2align 3
.L1696:
	xorl	%edx, %edx
	movl	$16, %esi
	leaq	_ZZN3hwy17GetGeneratorStateEvE5state(%rip), %rdi
	call	getrandom@PLT
	cmpq	$16, %rax
	jne	.L1690
	movq	$1, 16+_ZZN3hwy17GetGeneratorStateEvE5state(%rip)
	jmp	.L1682
.L1675:
	leaq	-2(%rsi), %r14
	movq	%r12, %rdx
	leaq	-336(%rbp), %r15
	movq	%r13, %rsi
	movq	%r14, %rbx
	andq	$-2, %r14
	movq	%r15, %rdi
	shrq	%rbx
	addq	$2, %r14
	addq	$1, %rbx
	salq	$4, %rbx
	movl	%ebx, %ecx
	shrl	$3, %ecx
	subq	%r14, %rdx
	rep movsq
	movq	%rdx, -344(%rbp)
	je	.L1679
	leaq	0(,%r14,8), %rax
	salq	$3, %rdx
	movq	%r8, -352(%rbp)
	leaq	(%r15,%rax), %rdi
	leaq	0(%r13,%rax), %rsi
	call	memcpy@PLT
	movq	-352(%rbp), %r8
.L1679:
	leal	-1(%r12), %eax
	movl	$32, %ecx
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
	leaq	2(%rdx), %rax
	cmpq	%rax, %r12
	jnb	.L1680
	movdqa	.LC4(%rip), %xmm0
	leaq	2(%r12), %rdx
	movups	%xmm0, -336(%rbp,%r8)
	cmpq	%rdx, %rax
	jbe	.L1680
	movups	%xmm0, -320(%rbp,%r8)
	leaq	4(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1680
	movups	%xmm0, -304(%rbp,%r8)
	leaq	6(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1680
	movups	%xmm0, -288(%rbp,%r8)
	leaq	8(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1680
	movups	%xmm0, -272(%rbp,%r8)
	leaq	10(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1680
	movups	%xmm0, -256(%rbp,%r8)
	leaq	12(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1680
	movups	%xmm0, -240(%rbp,%r8)
	leaq	14(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1680
	movups	%xmm0, -224(%rbp,%r8)
	leaq	16(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1680
	movups	%xmm0, -208(%rbp,%r8)
	leaq	18(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1680
	movups	%xmm0, -192(%rbp,%r8)
	leaq	20(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1680
	movups	%xmm0, -176(%rbp,%r8)
	leaq	22(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1680
	movups	%xmm0, -160(%rbp,%r8)
	leaq	24(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1680
	movups	%xmm0, -144(%rbp,%r8)
	leaq	26(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1680
	leaq	28(%r12), %rdx
	movups	%xmm0, -128(%rbp,%r8)
	cmpq	%rdx, %rax
	jbe	.L1680
	addq	$30, %r12
	movups	%xmm0, -112(%rbp,%r8)
	cmpq	%r12, %rax
	jbe	.L1680
	movups	%xmm0, -96(%rbp,%r8)
.L1680:
	movq	%r15, %rdi
	call	_ZN3hwy7N_SSSE36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	movq	-336(%rbp), %rax
	leaq	8(%r13), %rdi
	movq	%r15, %rsi
	andq	$-8, %rdi
	movq	%rax, 0(%r13)
	movl	%ebx, %eax
	movq	-8(%r15,%rax), %rdx
	movq	%rdx, -8(%r13,%rax)
	movq	%r13, %rax
	subq	%rdi, %rax
	addl	%eax, %ebx
	subq	%rax, %rsi
	shrl	$3, %ebx
	movl	%ebx, %ecx
	rep movsq
	movq	-344(%rbp), %rax
	testq	%rax, %rax
	je	.L1671
	salq	$3, %r14
	salq	$3, %rax
	leaq	0(%r13,%r14), %rdi
	movq	%rax, %rdx
	leaq	(%r15,%r14), %rsi
	call	memcpy@PLT
	jmp	.L1671
	.cfi_endproc
	.section	.text.unlikely._ZN3hwy7N_SSSE310SortI64AscEPlm
	.cfi_startproc
	.type	_ZN3hwy7N_SSSE310SortI64AscEPlm.cold, @function
_ZN3hwy7N_SSSE310SortI64AscEPlm.cold:
.LFSB2946:
.L1690:
	.cfi_def_cfa 6, 16
	.cfi_offset 3, -56
	.cfi_offset 6, -16
	.cfi_offset 12, -48
	.cfi_offset 13, -40
	.cfi_offset 14, -32
	.cfi_offset 15, -24
	call	abort@PLT
	.cfi_endproc
.LFE2946:
	.section	.text._ZN3hwy7N_SSSE310SortI64AscEPlm
	.size	_ZN3hwy7N_SSSE310SortI64AscEPlm, .-_ZN3hwy7N_SSSE310SortI64AscEPlm
	.section	.text.unlikely._ZN3hwy7N_SSSE310SortI64AscEPlm
	.size	_ZN3hwy7N_SSSE310SortI64AscEPlm.cold, .-_ZN3hwy7N_SSSE310SortI64AscEPlm.cold
.LCOLDE15:
	.section	.text._ZN3hwy7N_SSSE310SortI64AscEPlm
.LHOTE15:
	.section	.text.unlikely._ZN3hwy6N_SSE410SortI64AscEPlm,"ax",@progbits
.LCOLDB16:
	.section	.text._ZN3hwy6N_SSE410SortI64AscEPlm,"ax",@progbits
.LHOTB16:
	.p2align 4
	.globl	_ZN3hwy6N_SSE410SortI64AscEPlm
	.hidden	_ZN3hwy6N_SSE410SortI64AscEPlm
	.type	_ZN3hwy6N_SSE410SortI64AscEPlm, @function
_ZN3hwy6N_SSE410SortI64AscEPlm:
.LFB4014:
	.cfi_startproc
	cmpq	$1, %rsi
	jbe	.L1718
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
	movq	%rsi, %r12
	pushq	%rbx
	subq	$312, %rsp
	.cfi_offset 3, -56
	cmpq	$32, %rsi
	jbe	.L1721
	cmpq	$0, 16+_ZZN3hwy17GetGeneratorStateEvE5state(%rip)
	je	.L1722
.L1708:
	leaq	-336(%rbp), %rcx
	leaq	0(%r13,%r12,8), %rsi
	movq	%r12, %rdx
	movq	%r13, %rdi
	movl	$50, %r9d
	leaq	_ZZN3hwy17GetGeneratorStateEvE5state(%rip), %r8
	call	_ZN3hwy6N_SSE46detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L1697:
	addq	$312, %rsp
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
.L1718:
	.cfi_restore 3
	.cfi_restore 6
	.cfi_restore 12
	.cfi_restore 13
	.cfi_restore 14
	.cfi_restore 15
	ret
	.p2align 4,,10
	.p2align 3
.L1721:
	.cfi_def_cfa 6, 16
	.cfi_offset 3, -56
	.cfi_offset 6, -16
	.cfi_offset 12, -48
	.cfi_offset 13, -40
	.cfi_offset 14, -32
	.cfi_offset 15, -24
	leaq	0(,%rsi,8), %r8
	leaq	256(%rdi), %rax
	leaq	(%rdi,%r8), %rdx
	cmpq	%rax, %rdx
	jb	.L1701
	movl	$2, %esi
	call	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	jmp	.L1697
	.p2align 4,,10
	.p2align 3
.L1722:
	xorl	%edx, %edx
	movl	$16, %esi
	leaq	_ZZN3hwy17GetGeneratorStateEvE5state(%rip), %rdi
	call	getrandom@PLT
	cmpq	$16, %rax
	jne	.L1716
	movq	$1, 16+_ZZN3hwy17GetGeneratorStateEvE5state(%rip)
	jmp	.L1708
.L1701:
	leaq	-2(%rsi), %r14
	movq	%r12, %rdx
	leaq	-336(%rbp), %r15
	movq	%r13, %rsi
	movq	%r14, %rbx
	andq	$-2, %r14
	movq	%r15, %rdi
	shrq	%rbx
	addq	$2, %r14
	addq	$1, %rbx
	salq	$4, %rbx
	movl	%ebx, %ecx
	shrl	$3, %ecx
	subq	%r14, %rdx
	rep movsq
	movq	%rdx, -344(%rbp)
	je	.L1705
	leaq	0(,%r14,8), %rax
	salq	$3, %rdx
	movq	%r8, -352(%rbp)
	leaq	(%r15,%rax), %rdi
	leaq	0(%r13,%rax), %rsi
	call	memcpy@PLT
	movq	-352(%rbp), %r8
.L1705:
	leal	-1(%r12), %eax
	movl	$32, %ecx
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
	leaq	2(%rdx), %rax
	cmpq	%rax, %r12
	jnb	.L1706
	movdqa	.LC4(%rip), %xmm0
	leaq	2(%r12), %rdx
	movups	%xmm0, -336(%rbp,%r8)
	cmpq	%rdx, %rax
	jbe	.L1706
	movups	%xmm0, -320(%rbp,%r8)
	leaq	4(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1706
	movups	%xmm0, -304(%rbp,%r8)
	leaq	6(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1706
	movups	%xmm0, -288(%rbp,%r8)
	leaq	8(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1706
	movups	%xmm0, -272(%rbp,%r8)
	leaq	10(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1706
	movups	%xmm0, -256(%rbp,%r8)
	leaq	12(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1706
	movups	%xmm0, -240(%rbp,%r8)
	leaq	14(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1706
	movups	%xmm0, -224(%rbp,%r8)
	leaq	16(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1706
	movups	%xmm0, -208(%rbp,%r8)
	leaq	18(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1706
	movups	%xmm0, -192(%rbp,%r8)
	leaq	20(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1706
	movups	%xmm0, -176(%rbp,%r8)
	leaq	22(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1706
	movups	%xmm0, -160(%rbp,%r8)
	leaq	24(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1706
	movups	%xmm0, -144(%rbp,%r8)
	leaq	26(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1706
	leaq	28(%r12), %rdx
	movups	%xmm0, -128(%rbp,%r8)
	cmpq	%rdx, %rax
	jbe	.L1706
	addq	$30, %r12
	movups	%xmm0, -112(%rbp,%r8)
	cmpq	%r12, %rax
	jbe	.L1706
	movups	%xmm0, -96(%rbp,%r8)
.L1706:
	movq	%r15, %rdi
	call	_ZN3hwy6N_SSE46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	movq	-336(%rbp), %rax
	leaq	8(%r13), %rdi
	movq	%r15, %rsi
	andq	$-8, %rdi
	movq	%rax, 0(%r13)
	movl	%ebx, %eax
	movq	-8(%r15,%rax), %rdx
	movq	%rdx, -8(%r13,%rax)
	movq	%r13, %rax
	subq	%rdi, %rax
	addl	%eax, %ebx
	subq	%rax, %rsi
	shrl	$3, %ebx
	movl	%ebx, %ecx
	rep movsq
	movq	-344(%rbp), %rax
	testq	%rax, %rax
	je	.L1697
	salq	$3, %r14
	salq	$3, %rax
	leaq	0(%r13,%r14), %rdi
	movq	%rax, %rdx
	leaq	(%r15,%r14), %rsi
	call	memcpy@PLT
	jmp	.L1697
	.cfi_endproc
	.section	.text.unlikely._ZN3hwy6N_SSE410SortI64AscEPlm
	.cfi_startproc
	.type	_ZN3hwy6N_SSE410SortI64AscEPlm.cold, @function
_ZN3hwy6N_SSE410SortI64AscEPlm.cold:
.LFSB4014:
.L1716:
	.cfi_def_cfa 6, 16
	.cfi_offset 3, -56
	.cfi_offset 6, -16
	.cfi_offset 12, -48
	.cfi_offset 13, -40
	.cfi_offset 14, -32
	.cfi_offset 15, -24
	call	abort@PLT
	.cfi_endproc
.LFE4014:
	.section	.text._ZN3hwy6N_SSE410SortI64AscEPlm
	.size	_ZN3hwy6N_SSE410SortI64AscEPlm, .-_ZN3hwy6N_SSE410SortI64AscEPlm
	.section	.text.unlikely._ZN3hwy6N_SSE410SortI64AscEPlm
	.size	_ZN3hwy6N_SSE410SortI64AscEPlm.cold, .-_ZN3hwy6N_SSE410SortI64AscEPlm.cold
.LCOLDE16:
	.section	.text._ZN3hwy6N_SSE410SortI64AscEPlm
.LHOTE16:
	.section	.text.unlikely._ZN3hwy6N_AVX210SortI64AscEPlm,"ax",@progbits
.LCOLDB17:
	.section	.text._ZN3hwy6N_AVX210SortI64AscEPlm,"ax",@progbits
.LHOTB17:
	.p2align 4
	.globl	_ZN3hwy6N_AVX210SortI64AscEPlm
	.hidden	_ZN3hwy6N_AVX210SortI64AscEPlm
	.type	_ZN3hwy6N_AVX210SortI64AscEPlm, @function
_ZN3hwy6N_AVX210SortI64AscEPlm:
.LFB10439:
	.cfi_startproc
	cmpq	$1, %rsi
	jbe	.L1751
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r14
	pushq	%r13
	.cfi_offset 14, -24
	.cfi_offset 13, -32
	movq	%rdi, %r13
	pushq	%r12
	.cfi_offset 12, -40
	movq	%rsi, %r12
	andq	$-32, %rsp
	subq	$576, %rsp
	cmpq	$64, %rsi
	jbe	.L1754
	cmpq	$0, 16+_ZZN3hwy17GetGeneratorStateEvE5state(%rip)
	je	.L1755
.L1738:
	movq	%rsp, %rcx
	leaq	0(%r13,%r12,8), %rsi
	movq	%r12, %rdx
	movq	%r13, %rdi
	movl	$50, %r9d
	leaq	_ZZN3hwy17GetGeneratorStateEvE5state(%rip), %r8
	call	_ZN3hwy6N_AVX26detail7RecurseINS0_4SimdIlLm4ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L1749:
	leaq	-24(%rbp), %rsp
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L1751:
	.cfi_restore 6
	.cfi_restore 12
	.cfi_restore 13
	.cfi_restore 14
	ret
	.p2align 4,,10
	.p2align 3
.L1754:
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	leaq	(%rdi,%rsi,8), %rax
	leaq	512(%rdi), %rdx
	cmpq	%rdx, %rax
	jb	.L1756
	movl	$4, %esi
	call	_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	jmp	.L1749
	.p2align 4,,10
	.p2align 3
.L1755:
	xorl	%edx, %edx
	movl	$16, %esi
	leaq	_ZZN3hwy17GetGeneratorStateEvE5state(%rip), %rdi
	call	getrandom@PLT
	cmpq	$16, %rax
	jne	.L1748
	movq	$1, 16+_ZZN3hwy17GetGeneratorStateEvE5state(%rip)
	jmp	.L1738
.L1756:
	movq	%rsi, %rcx
	xorl	%eax, %eax
	movq	%rsp, %r14
	cmpq	$3, %rsi
	jbe	.L1729
	leaq	-4(%rsi), %rax
	movq	%rsp, %r14
	movq	%r13, %rsi
	movq	%rax, %rdx
	andq	$-4, %rax
	movq	%r14, %rdi
	shrq	$2, %rdx
	addq	$4, %rax
	leal	4(,%rdx,4), %ecx
	andl	$536870908, %ecx
	rep movsq
	movq	%r12, %rcx
	subq	%rax, %rcx
	je	.L1733
.L1729:
	salq	$3, %rax
	leaq	0(,%rcx,8), %rdx
	testq	%rcx, %rcx
	movl	$8, %ecx
	cmove	%rcx, %rdx
	leaq	(%r14,%rax), %rdi
	leaq	0(%r13,%rax), %rsi
	call	memcpy@PLT
.L1733:
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
	addq	$4, %rdx
	cmpq	%rdx, %r12
	jnb	.L1734
	vmovdqa	.LC13(%rip), %ymm0
	movq	%r12, %rax
	.p2align 4,,10
	.p2align 3
.L1735:
	vmovdqu	%ymm0, (%r14,%rax,8)
	addq	$4, %rax
	cmpq	%rax, %rdx
	ja	.L1735
	vzeroupper
.L1734:
	movq	%r14, %rdi
	call	_ZN3hwy6N_AVX26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	cmpq	$3, %r12
	jbe	.L1740
	leaq	-4(%r12), %rdx
	movq	(%rsp), %rcx
	leaq	8(%r13), %rdi
	movq	%rdx, %rax
	andq	$-8, %rdi
	andq	$-4, %rdx
	shrq	$2, %rax
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
	leaq	4(%rdx), %rax
	rep movsq
	subq	%rax, %r12
	je	.L1749
.L1736:
	salq	$3, %rax
	leaq	0(,%r12,8), %rdx
	testq	%r12, %r12
	movl	$8, %ecx
	cmove	%rcx, %rdx
	leaq	0(%r13,%rax), %rdi
	leaq	(%r14,%rax), %rsi
	call	memcpy@PLT
	jmp	.L1749
.L1740:
	xorl	%eax, %eax
	jmp	.L1736
	.cfi_endproc
	.section	.text.unlikely._ZN3hwy6N_AVX210SortI64AscEPlm
	.cfi_startproc
	.type	_ZN3hwy6N_AVX210SortI64AscEPlm.cold, @function
_ZN3hwy6N_AVX210SortI64AscEPlm.cold:
.LFSB10439:
.L1748:
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	call	abort@PLT
	.cfi_endproc
.LFE10439:
	.section	.text._ZN3hwy6N_AVX210SortI64AscEPlm
	.size	_ZN3hwy6N_AVX210SortI64AscEPlm, .-_ZN3hwy6N_AVX210SortI64AscEPlm
	.section	.text.unlikely._ZN3hwy6N_AVX210SortI64AscEPlm
	.size	_ZN3hwy6N_AVX210SortI64AscEPlm.cold, .-_ZN3hwy6N_AVX210SortI64AscEPlm.cold
.LCOLDE17:
	.section	.text._ZN3hwy6N_AVX210SortI64AscEPlm
.LHOTE17:
	.section	.text.unlikely._ZN3hwy6N_AVX310SortI64AscEPlm,"ax",@progbits
.LCOLDB18:
	.section	.text._ZN3hwy6N_AVX310SortI64AscEPlm,"ax",@progbits
.LHOTB18:
	.p2align 4
	.globl	_ZN3hwy6N_AVX310SortI64AscEPlm
	.hidden	_ZN3hwy6N_AVX310SortI64AscEPlm
	.type	_ZN3hwy6N_AVX310SortI64AscEPlm, @function
_ZN3hwy6N_AVX310SortI64AscEPlm:
.LFB12848:
	.cfi_startproc
	cmpq	$1, %rsi
	jbe	.L1781
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	.cfi_offset 13, -24
	movq	%rdi, %r13
	pushq	%r12
	.cfi_offset 12, -32
	movq	%rsi, %r12
	pushq	%rbx
	andq	$-64, %rsp
	subq	$1152, %rsp
	.cfi_offset 3, -40
	cmpq	$128, %rsi
	jbe	.L1784
	cmpq	$0, 16+_ZZN3hwy17GetGeneratorStateEvE5state(%rip)
	je	.L1785
.L1772:
	movq	%rsp, %rcx
	leaq	0(%r13,%r12,8), %rsi
	movq	%r12, %rdx
	movq	%r13, %rdi
	movl	$50, %r9d
	leaq	_ZZN3hwy17GetGeneratorStateEvE5state(%rip), %r8
	call	_ZN3hwy6N_AVX36detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L1779:
	leaq	-24(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L1781:
	.cfi_restore 3
	.cfi_restore 6
	.cfi_restore 12
	.cfi_restore 13
	ret
	.p2align 4,,10
	.p2align 3
.L1784:
	.cfi_def_cfa 6, 16
	.cfi_offset 3, -40
	.cfi_offset 6, -16
	.cfi_offset 12, -32
	.cfi_offset 13, -24
	leaq	(%rdi,%rsi,8), %rax
	leaq	1024(%rdi), %rdx
	cmpq	%rdx, %rax
	jb	.L1786
	movl	$8, %esi
	call	_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	jmp	.L1779
	.p2align 4,,10
	.p2align 3
.L1785:
	xorl	%edx, %edx
	movl	$16, %esi
	leaq	_ZZN3hwy17GetGeneratorStateEvE5state(%rip), %rdi
	call	getrandom@PLT
	cmpq	$16, %rax
	jne	.L1778
	movq	$1, 16+_ZZN3hwy17GetGeneratorStateEvE5state(%rip)
	jmp	.L1772
.L1786:
	cmpq	$7, %rsi
	jbe	.L1787
	leaq	-8(%rsi), %rax
	movq	%rsp, %rbx
	movq	%r13, %rsi
	movq	%rax, %rdx
	andq	$-8, %rax
	movq	%rbx, %rdi
	shrq	$3, %rdx
	addq	$8, %rax
	leal	8(,%rdx,8), %ecx
	leaq	0(,%rax,8), %rdx
	andl	$536870904, %ecx
	rep movsq
	movq	%r12, %rcx
	leaq	(%rbx,%rdx), %rdi
	addq	%r13, %rdx
	subq	%rax, %rcx
	movl	$255, %eax
	kmovd	%eax, %k1
	cmpq	$255, %rcx
	jbe	.L1762
.L1766:
	leal	-1(%r12), %eax
	movl	$32, %ecx
	movl	$1, %esi
	vmovdqu64	(%rdx), %zmm0{%k1}{z}
	bsrl	%eax, %eax
	xorl	$31, %eax
	vmovdqa64	%zmm0, (%rdi){%k1}
	vpbroadcastq	.LC10(%rip), %zmm0
	subl	%eax, %ecx
	movl	$1, %eax
	shlx	%rcx, %rsi, %rsi
	shrq	$4, %rsi
	cmove	%rax, %rsi
	movq	%r12, %rax
	movq	%rsi, %rdx
	salq	$4, %rdx
	addq	$8, %rdx
	cmpq	%rdx, %r12
	jnb	.L1770
	.p2align 4,,10
	.p2align 3
.L1767:
	vmovdqu64	%zmm0, (%rbx,%rax,8)
	addq	$8, %rax
	cmpq	%rdx, %rax
	jb	.L1767
.L1770:
	movq	%rbx, %rdi
	vzeroupper
	call	_ZN3hwy6N_AVX36detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	cmpq	$7, %r12
	jbe	.L1769
	leaq	-8(%r12), %rdx
	movq	(%rsp), %rcx
	leaq	8(%r13), %rdi
	movq	%rdx, %rax
	andq	$-8, %rdi
	andq	$-8, %rdx
	shrq	$3, %rax
	movq	%rcx, 0(%r13)
	addq	$1, %rax
	salq	$6, %rax
	movl	%eax, %ecx
	movq	-8(%rbx,%rcx), %rsi
	movq	%rsi, -8(%r13,%rcx)
	movq	%r13, %rcx
	movq	%rbx, %rsi
	subq	%rdi, %rcx
	addl	%ecx, %eax
	subq	%rcx, %rsi
	shrl	$3, %eax
	movl	%eax, %ecx
	leaq	8(%rdx), %rax
	rep movsq
	leaq	0(,%rax,8), %rdx
	subq	%rax, %r12
	movl	$255, %eax
	addq	%rdx, %r13
	addq	%rdx, %rbx
	kmovd	%eax, %k1
	cmpq	$255, %r12
	jbe	.L1769
.L1771:
	vmovdqa64	(%rbx), %zmm0{%k1}{z}
	vmovdqu64	%zmm0, 0(%r13){%k1}
	vzeroupper
	jmp	.L1779
.L1787:
	movq	%rsp, %rbx
	movq	%rdi, %rdx
	movq	%rsi, %rcx
	movq	%rbx, %rdi
.L1762:
	movq	$-1, %rax
	bzhi	%rcx, %rax, %rax
	movzbl	%al, %eax
	kmovd	%eax, %k1
	jmp	.L1766
.L1769:
	movq	$-1, %rax
	bzhi	%r12, %rax, %rax
	movzbl	%al, %eax
	kmovd	%eax, %k1
	jmp	.L1771
	.cfi_endproc
	.section	.text.unlikely._ZN3hwy6N_AVX310SortI64AscEPlm
	.cfi_startproc
	.type	_ZN3hwy6N_AVX310SortI64AscEPlm.cold, @function
_ZN3hwy6N_AVX310SortI64AscEPlm.cold:
.LFSB12848:
.L1778:
	.cfi_def_cfa 6, 16
	.cfi_offset 3, -40
	.cfi_offset 6, -16
	.cfi_offset 12, -32
	.cfi_offset 13, -24
	call	abort@PLT
	.cfi_endproc
.LFE12848:
	.section	.text._ZN3hwy6N_AVX310SortI64AscEPlm
	.size	_ZN3hwy6N_AVX310SortI64AscEPlm, .-_ZN3hwy6N_AVX310SortI64AscEPlm
	.section	.text.unlikely._ZN3hwy6N_AVX310SortI64AscEPlm
	.size	_ZN3hwy6N_AVX310SortI64AscEPlm.cold, .-_ZN3hwy6N_AVX310SortI64AscEPlm.cold
.LCOLDE18:
	.section	.text._ZN3hwy6N_AVX310SortI64AscEPlm
.LHOTE18:
	.section	.text.unlikely._ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm,"ax",@progbits
.LCOLDB19:
	.section	.text._ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm,"ax",@progbits
.LHOTB19:
	.p2align 4
	.globl	_ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm
	.hidden	_ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm
	.type	_ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm, @function
_ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm:
.LFB15264:
	.cfi_startproc
	cmpq	$1, %rsi
	jbe	.L1812
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	.cfi_offset 13, -24
	movq	%rdi, %r13
	pushq	%r12
	.cfi_offset 12, -32
	movq	%rsi, %r12
	pushq	%rbx
	andq	$-64, %rsp
	subq	$1152, %rsp
	.cfi_offset 3, -40
	cmpq	$128, %rsi
	jbe	.L1815
	cmpq	$0, 16+_ZZN3hwy17GetGeneratorStateEvE5state(%rip)
	je	.L1816
.L1803:
	movq	%rsp, %rcx
	leaq	0(%r13,%r12,8), %rsi
	movq	%r12, %rdx
	movq	%r13, %rdi
	movl	$50, %r9d
	leaq	_ZZN3hwy17GetGeneratorStateEvE5state(%rip), %r8
	call	_ZN3hwy11N_AVX3_ZEN46detail7RecurseINS0_4SimdIlLm8ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L1810:
	leaq	-24(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L1812:
	.cfi_restore 3
	.cfi_restore 6
	.cfi_restore 12
	.cfi_restore 13
	ret
	.p2align 4,,10
	.p2align 3
.L1815:
	.cfi_def_cfa 6, 16
	.cfi_offset 3, -40
	.cfi_offset 6, -16
	.cfi_offset 12, -32
	.cfi_offset 13, -24
	leaq	(%rdi,%rsi,8), %rax
	leaq	1024(%rdi), %rdx
	cmpq	%rdx, %rax
	jb	.L1817
	movl	$8, %esi
	call	_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	jmp	.L1810
	.p2align 4,,10
	.p2align 3
.L1816:
	xorl	%edx, %edx
	movl	$16, %esi
	leaq	_ZZN3hwy17GetGeneratorStateEvE5state(%rip), %rdi
	call	getrandom@PLT
	cmpq	$16, %rax
	jne	.L1809
	movq	$1, 16+_ZZN3hwy17GetGeneratorStateEvE5state(%rip)
	jmp	.L1803
.L1817:
	cmpq	$7, %rsi
	jbe	.L1818
	leaq	-8(%rsi), %rax
	movq	%rsp, %rbx
	movq	%r13, %rsi
	movq	%rax, %rdx
	andq	$-8, %rax
	movq	%rbx, %rdi
	shrq	$3, %rdx
	addq	$8, %rax
	leal	8(,%rdx,8), %ecx
	leaq	0(,%rax,8), %rdx
	andl	$536870904, %ecx
	rep movsq
	movq	%r12, %rcx
	leaq	(%rbx,%rdx), %rdi
	addq	%r13, %rdx
	subq	%rax, %rcx
	movl	$255, %eax
	kmovd	%eax, %k1
	cmpq	$255, %rcx
	jbe	.L1793
.L1797:
	leal	-1(%r12), %eax
	movl	$32, %ecx
	movl	$1, %esi
	vmovdqu64	(%rdx), %zmm0{%k1}{z}
	bsrl	%eax, %eax
	xorl	$31, %eax
	vmovdqa64	%zmm0, (%rdi){%k1}
	vpbroadcastq	.LC10(%rip), %zmm0
	subl	%eax, %ecx
	movl	$1, %eax
	shlx	%rcx, %rsi, %rsi
	shrq	$4, %rsi
	cmove	%rax, %rsi
	movq	%r12, %rax
	movq	%rsi, %rdx
	salq	$4, %rdx
	addq	$8, %rdx
	cmpq	%rdx, %r12
	jnb	.L1801
	.p2align 4,,10
	.p2align 3
.L1798:
	vmovdqu64	%zmm0, (%rbx,%rax,8)
	addq	$8, %rax
	cmpq	%rdx, %rax
	jb	.L1798
.L1801:
	movq	%rbx, %rdi
	vzeroupper
	call	_ZN3hwy11N_AVX3_ZEN46detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	cmpq	$7, %r12
	jbe	.L1800
	leaq	-8(%r12), %rdx
	movq	(%rsp), %rcx
	leaq	8(%r13), %rdi
	movq	%rdx, %rax
	andq	$-8, %rdi
	andq	$-8, %rdx
	shrq	$3, %rax
	movq	%rcx, 0(%r13)
	addq	$1, %rax
	salq	$6, %rax
	movl	%eax, %ecx
	movq	-8(%rbx,%rcx), %rsi
	movq	%rsi, -8(%r13,%rcx)
	movq	%r13, %rcx
	movq	%rbx, %rsi
	subq	%rdi, %rcx
	addl	%ecx, %eax
	subq	%rcx, %rsi
	shrl	$3, %eax
	movl	%eax, %ecx
	leaq	8(%rdx), %rax
	rep movsq
	leaq	0(,%rax,8), %rdx
	subq	%rax, %r12
	movl	$255, %eax
	addq	%rdx, %r13
	addq	%rdx, %rbx
	kmovd	%eax, %k1
	cmpq	$255, %r12
	jbe	.L1800
.L1802:
	vmovdqa64	(%rbx), %zmm0{%k1}{z}
	vmovdqu64	%zmm0, 0(%r13){%k1}
	vzeroupper
	jmp	.L1810
.L1818:
	movq	%rsp, %rbx
	movq	%rdi, %rdx
	movq	%rsi, %rcx
	movq	%rbx, %rdi
.L1793:
	movq	$-1, %rax
	bzhi	%rcx, %rax, %rax
	movzbl	%al, %eax
	kmovd	%eax, %k1
	jmp	.L1797
.L1800:
	movq	$-1, %rax
	bzhi	%r12, %rax, %rax
	movzbl	%al, %eax
	kmovd	%eax, %k1
	jmp	.L1802
	.cfi_endproc
	.section	.text.unlikely._ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm
	.cfi_startproc
	.type	_ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm.cold, @function
_ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm.cold:
.LFSB15264:
.L1809:
	.cfi_def_cfa 6, 16
	.cfi_offset 3, -40
	.cfi_offset 6, -16
	.cfi_offset 12, -32
	.cfi_offset 13, -24
	call	abort@PLT
	.cfi_endproc
.LFE15264:
	.section	.text._ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm
	.size	_ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm, .-_ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm
	.section	.text.unlikely._ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm
	.size	_ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm.cold, .-_ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm.cold
.LCOLDE19:
	.section	.text._ZN3hwy11N_AVX3_ZEN410SortI64AscEPlm
.LHOTE19:
	.section	.text.unlikely._ZN3hwy6N_SSE210SortI64AscEPlm,"ax",@progbits
.LCOLDB20:
	.section	.text._ZN3hwy6N_SSE210SortI64AscEPlm,"ax",@progbits
.LHOTB20:
	.p2align 4
	.globl	_ZN3hwy6N_SSE210SortI64AscEPlm
	.hidden	_ZN3hwy6N_SSE210SortI64AscEPlm
	.type	_ZN3hwy6N_SSE210SortI64AscEPlm, @function
_ZN3hwy6N_SSE210SortI64AscEPlm:
.LFB16254:
	.cfi_startproc
	cmpq	$1, %rsi
	jbe	.L1840
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
	movq	%rsi, %r12
	pushq	%rbx
	subq	$312, %rsp
	.cfi_offset 3, -56
	cmpq	$32, %rsi
	jbe	.L1843
	cmpq	$0, 16+_ZZN3hwy17GetGeneratorStateEvE5state(%rip)
	je	.L1844
.L1830:
	leaq	-336(%rbp), %rcx
	leaq	0(%r13,%r12,8), %rsi
	movq	%r12, %rdx
	movq	%r13, %rdi
	movl	$50, %r9d
	leaq	_ZZN3hwy17GetGeneratorStateEvE5state(%rip), %r8
	call	_ZN3hwy6N_SSE26detail7RecurseINS0_4SimdIlLm2ELi0EEENS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_T0_PT1_SE_mSE_Pmm.isra.0
.L1819:
	addq	$312, %rsp
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
.L1840:
	.cfi_restore 3
	.cfi_restore 6
	.cfi_restore 12
	.cfi_restore 13
	.cfi_restore 14
	.cfi_restore 15
	ret
	.p2align 4,,10
	.p2align 3
.L1843:
	.cfi_def_cfa 6, 16
	.cfi_offset 3, -56
	.cfi_offset 6, -16
	.cfi_offset 12, -48
	.cfi_offset 13, -40
	.cfi_offset 14, -32
	.cfi_offset 15, -24
	leaq	0(,%rsi,8), %r8
	leaq	256(%rdi), %rax
	leaq	(%rdi,%r8), %rdx
	cmpq	%rax, %rdx
	jb	.L1823
	movl	$2, %esi
	call	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	jmp	.L1819
	.p2align 4,,10
	.p2align 3
.L1844:
	xorl	%edx, %edx
	movl	$16, %esi
	leaq	_ZZN3hwy17GetGeneratorStateEvE5state(%rip), %rdi
	call	getrandom@PLT
	cmpq	$16, %rax
	jne	.L1838
	movq	$1, 16+_ZZN3hwy17GetGeneratorStateEvE5state(%rip)
	jmp	.L1830
.L1823:
	leaq	-2(%rsi), %r14
	movq	%r12, %rdx
	leaq	-336(%rbp), %r15
	movq	%r13, %rsi
	movq	%r14, %rbx
	andq	$-2, %r14
	movq	%r15, %rdi
	shrq	%rbx
	addq	$2, %r14
	addq	$1, %rbx
	salq	$4, %rbx
	movl	%ebx, %ecx
	shrl	$3, %ecx
	subq	%r14, %rdx
	rep movsq
	movq	%rdx, -344(%rbp)
	je	.L1827
	leaq	0(,%r14,8), %rax
	salq	$3, %rdx
	movq	%r8, -352(%rbp)
	leaq	(%r15,%rax), %rdi
	leaq	0(%r13,%rax), %rsi
	call	memcpy@PLT
	movq	-352(%rbp), %r8
.L1827:
	leal	-1(%r12), %eax
	movl	$32, %ecx
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
	leaq	2(%rdx), %rax
	cmpq	%rax, %r12
	jnb	.L1828
	movdqa	.LC4(%rip), %xmm0
	leaq	2(%r12), %rdx
	movups	%xmm0, -336(%rbp,%r8)
	cmpq	%rdx, %rax
	jbe	.L1828
	movups	%xmm0, -320(%rbp,%r8)
	leaq	4(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1828
	movups	%xmm0, -304(%rbp,%r8)
	leaq	6(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1828
	movups	%xmm0, -288(%rbp,%r8)
	leaq	8(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1828
	movups	%xmm0, -272(%rbp,%r8)
	leaq	10(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1828
	movups	%xmm0, -256(%rbp,%r8)
	leaq	12(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1828
	movups	%xmm0, -240(%rbp,%r8)
	leaq	14(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1828
	movups	%xmm0, -224(%rbp,%r8)
	leaq	16(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1828
	movups	%xmm0, -208(%rbp,%r8)
	leaq	18(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1828
	movups	%xmm0, -192(%rbp,%r8)
	leaq	20(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1828
	movups	%xmm0, -176(%rbp,%r8)
	leaq	22(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1828
	movups	%xmm0, -160(%rbp,%r8)
	leaq	24(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1828
	movups	%xmm0, -144(%rbp,%r8)
	leaq	26(%r12), %rdx
	cmpq	%rdx, %rax
	jbe	.L1828
	leaq	28(%r12), %rdx
	movups	%xmm0, -128(%rbp,%r8)
	cmpq	%rdx, %rax
	jbe	.L1828
	addq	$30, %r12
	movups	%xmm0, -112(%rbp,%r8)
	cmpq	%r12, %rax
	jbe	.L1828
	movups	%xmm0, -96(%rbp,%r8)
.L1828:
	movq	%r15, %rdi
	call	_ZN3hwy6N_SSE26detail14SortingNetworkINS1_12SharedTraitsINS1_10TraitsLaneINS1_14OrderAscendingIlEEEEEElEEvT_PT0_m.isra.0
	movq	-336(%rbp), %rax
	leaq	8(%r13), %rdi
	movq	%r15, %rsi
	andq	$-8, %rdi
	movq	%rax, 0(%r13)
	movl	%ebx, %eax
	movq	-8(%r15,%rax), %rdx
	movq	%rdx, -8(%r13,%rax)
	movq	%r13, %rax
	subq	%rdi, %rax
	addl	%eax, %ebx
	subq	%rax, %rsi
	shrl	$3, %ebx
	movl	%ebx, %ecx
	rep movsq
	movq	-344(%rbp), %rax
	testq	%rax, %rax
	je	.L1819
	salq	$3, %r14
	salq	$3, %rax
	leaq	0(%r13,%r14), %rdi
	movq	%rax, %rdx
	leaq	(%r15,%r14), %rsi
	call	memcpy@PLT
	jmp	.L1819
	.cfi_endproc
	.section	.text.unlikely._ZN3hwy6N_SSE210SortI64AscEPlm
	.cfi_startproc
	.type	_ZN3hwy6N_SSE210SortI64AscEPlm.cold, @function
_ZN3hwy6N_SSE210SortI64AscEPlm.cold:
.LFSB16254:
.L1838:
	.cfi_def_cfa 6, 16
	.cfi_offset 3, -56
	.cfi_offset 6, -16
	.cfi_offset 12, -48
	.cfi_offset 13, -40
	.cfi_offset 14, -32
	.cfi_offset 15, -24
	call	abort@PLT
	.cfi_endproc
.LFE16254:
	.section	.text._ZN3hwy6N_SSE210SortI64AscEPlm
	.size	_ZN3hwy6N_SSE210SortI64AscEPlm, .-_ZN3hwy6N_SSE210SortI64AscEPlm
	.section	.text.unlikely._ZN3hwy6N_SSE210SortI64AscEPlm
	.size	_ZN3hwy6N_SSE210SortI64AscEPlm.cold, .-_ZN3hwy6N_SSE210SortI64AscEPlm.cold
.LCOLDE20:
	.section	.text._ZN3hwy6N_SSE210SortI64AscEPlm
.LHOTE20:
	.section	.text.unlikely._ZN3hwy17GetGeneratorStateEv,"ax",@progbits
.LCOLDB21:
	.section	.text._ZN3hwy17GetGeneratorStateEv,"ax",@progbits
.LHOTB21:
	.p2align 4
	.globl	_ZN3hwy17GetGeneratorStateEv
	.hidden	_ZN3hwy17GetGeneratorStateEv
	.type	_ZN3hwy17GetGeneratorStateEv, @function
_ZN3hwy17GetGeneratorStateEv:
.LFB16255:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r12
	.cfi_offset 12, -24
	leaq	_ZZN3hwy17GetGeneratorStateEvE5state(%rip), %r12
	subq	$8, %rsp
	cmpq	$0, 16+_ZZN3hwy17GetGeneratorStateEvE5state(%rip)
	je	.L1850
	movq	%r12, %rax
	movq	-8(%rbp), %r12
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L1850:
	.cfi_restore_state
	xorl	%edx, %edx
	movl	$16, %esi
	movq	%r12, %rdi
	call	getrandom@PLT
	cmpq	$16, %rax
	jne	.L1848
	movq	$1, 16+_ZZN3hwy17GetGeneratorStateEvE5state(%rip)
	movq	%r12, %rax
	movq	-8(%rbp), %r12
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
	.section	.text.unlikely._ZN3hwy17GetGeneratorStateEv
	.cfi_startproc
	.type	_ZN3hwy17GetGeneratorStateEv.cold, @function
_ZN3hwy17GetGeneratorStateEv.cold:
.LFSB16255:
.L1848:
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
	.cfi_offset 12, -24
	call	abort@PLT
	.cfi_endproc
.LFE16255:
	.section	.text._ZN3hwy17GetGeneratorStateEv
	.size	_ZN3hwy17GetGeneratorStateEv, .-_ZN3hwy17GetGeneratorStateEv
	.section	.text.unlikely._ZN3hwy17GetGeneratorStateEv
	.size	_ZN3hwy17GetGeneratorStateEv.cold, .-_ZN3hwy17GetGeneratorStateEv.cold
.LCOLDE21:
	.section	.text._ZN3hwy17GetGeneratorStateEv
.LHOTE21:
	.section	.text.vqsort_int64_avx2,"ax",@progbits
	.p2align 4
	.globl	vqsort_int64_avx2
	.hidden	vqsort_int64_avx2
	.type	vqsort_int64_avx2, @function
vqsort_int64_avx2:
.LFB16256:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	call	_ZN3hwy6N_AVX210SortI64AscEPlm
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16256:
	.size	vqsort_int64_avx2, .-vqsort_int64_avx2
	.section	.text.vqsort_int64_sse4,"ax",@progbits
	.p2align 4
	.globl	vqsort_int64_sse4
	.hidden	vqsort_int64_sse4
	.type	vqsort_int64_sse4, @function
vqsort_int64_sse4:
.LFB16257:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	call	_ZN3hwy6N_SSE410SortI64AscEPlm
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16257:
	.size	vqsort_int64_sse4, .-vqsort_int64_sse4
	.section	.text.vqsort_int64_ssse3,"ax",@progbits
	.p2align 4
	.globl	vqsort_int64_ssse3
	.hidden	vqsort_int64_ssse3
	.type	vqsort_int64_ssse3, @function
vqsort_int64_ssse3:
.LFB16258:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	call	_ZN3hwy7N_SSSE310SortI64AscEPlm
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16258:
	.size	vqsort_int64_ssse3, .-vqsort_int64_ssse3
	.section	.text.vqsort_int64_sse2,"ax",@progbits
	.p2align 4
	.globl	vqsort_int64_sse2
	.hidden	vqsort_int64_sse2
	.type	vqsort_int64_sse2, @function
vqsort_int64_sse2:
.LFB16259:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	call	_ZN3hwy6N_SSE210SortI64AscEPlm
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16259:
	.size	vqsort_int64_sse2, .-vqsort_int64_sse2
	.hidden	_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices
	.weak	_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices
	.section	.rodata._ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices,"aG",@progbits,_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices,comdat
	.balign 32
	.type	_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices, @object
	.size	_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices, 512
_ZZN3hwy6N_AVX26detail21IndicesFromNotBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices:
	.long	8
	.long	9
	.long	10
	.long	11
	.long	12
	.long	13
	.long	14
	.long	15
	.long	10
	.long	11
	.long	12
	.long	13
	.long	14
	.long	15
	.long	8
	.long	9
	.long	8
	.long	9
	.long	12
	.long	13
	.long	14
	.long	15
	.long	10
	.long	11
	.long	12
	.long	13
	.long	14
	.long	15
	.long	8
	.long	9
	.long	10
	.long	11
	.long	8
	.long	9
	.long	10
	.long	11
	.long	14
	.long	15
	.long	12
	.long	13
	.long	10
	.long	11
	.long	14
	.long	15
	.long	8
	.long	9
	.long	12
	.long	13
	.long	8
	.long	9
	.long	14
	.long	15
	.long	10
	.long	11
	.long	12
	.long	13
	.long	14
	.long	15
	.long	8
	.long	9
	.long	10
	.long	11
	.long	12
	.long	13
	.long	8
	.long	9
	.long	10
	.long	11
	.long	12
	.long	13
	.long	14
	.long	15
	.long	10
	.long	11
	.long	12
	.long	13
	.long	8
	.long	9
	.long	14
	.long	15
	.long	8
	.long	9
	.long	12
	.long	13
	.long	10
	.long	11
	.long	14
	.long	15
	.long	12
	.long	13
	.long	8
	.long	9
	.long	10
	.long	11
	.long	14
	.long	15
	.long	8
	.long	9
	.long	10
	.long	11
	.long	12
	.long	13
	.long	14
	.long	15
	.long	10
	.long	11
	.long	8
	.long	9
	.long	12
	.long	13
	.long	14
	.long	15
	.long	8
	.long	9
	.long	10
	.long	11
	.long	12
	.long	13
	.long	14
	.long	15
	.long	8
	.long	9
	.long	10
	.long	11
	.long	12
	.long	13
	.long	14
	.long	15
	.section	.rodata._ZZN3hwy6N_AVX3L11CompressNotIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array,"a"
	.balign 16
	.type	_ZZN3hwy6N_AVX3L11CompressNotIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array, @object
	.size	_ZZN3hwy6N_AVX3L11CompressNotIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array, 2048
_ZZN3hwy6N_AVX3L11CompressNotIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array:
	.quad	1985229328
	.quad	124076833
	.quad	392512288
	.quad	276190258
	.quad	660947728
	.quad	544625713
	.quad	561402928
	.quad	554132803
	.quad	929382928
	.quad	813061153
	.quad	829838368
	.quad	822568258
	.quad	846615568
	.quad	839345473
	.quad	840394048
	.quad	839939668
	.quad	1197814288
	.quad	1081496353
	.quad	1098273568
	.quad	1091003698
	.quad	1115050768
	.quad	1107780913
	.quad	1108829488
	.quad	1108375123
	.quad	1131827728
	.quad	1124558113
	.quad	1125606688
	.quad	1125152338
	.quad	1126655248
	.quad	1126200913
	.quad	1126266448
	.quad	1126238053
	.quad	1466184208
	.quad	1349927713
	.quad	1366704928
	.quad	1359438898
	.quad	1383482128
	.quad	1376216113
	.quad	1377264688
	.quad	1376810563
	.quad	1400259088
	.quad	1392993313
	.quad	1394041888
	.quad	1393587778
	.quad	1395090448
	.quad	1394636353
	.quad	1394701888
	.quad	1394673508
	.quad	1417032208
	.quad	1409770273
	.quad	1410818848
	.quad	1410364978
	.quad	1411867408
	.quad	1411413553
	.quad	1411479088
	.quad	1411450723
	.quad	1412915728
	.quad	1412462113
	.quad	1412527648
	.quad	1412499298
	.quad	1412593168
	.quad	1412564833
	.quad	1412568928
	.quad	1412567158
	.quad	1733571088
	.quad	1618297633
	.quad	1635074848
	.quad	1627870258
	.quad	1651852048
	.quad	1644647473
	.quad	1645696048
	.quad	1645245763
	.quad	1668629008
	.quad	1661424673
	.quad	1662473248
	.quad	1662022978
	.quad	1663521808
	.quad	1663071553
	.quad	1663137088
	.quad	1663108948
	.quad	1685402128
	.quad	1678201633
	.quad	1679250208
	.quad	1678800178
	.quad	1680298768
	.quad	1679848753
	.quad	1679914288
	.quad	1679886163
	.quad	1681347088
	.quad	1680897313
	.quad	1680962848
	.quad	1680934738
	.quad	1681028368
	.quad	1681000273
	.quad	1681004368
	.quad	1681002613
	.quad	1702113808
	.quad	1694974753
	.quad	1696023328
	.quad	1695577138
	.quad	1697071888
	.quad	1696625713
	.quad	1696691248
	.quad	1696663363
	.quad	1698120208
	.quad	1697674273
	.quad	1697739808
	.quad	1697711938
	.quad	1697805328
	.quad	1697777473
	.quad	1697781568
	.quad	1697779828
	.quad	1699164688
	.quad	1698722593
	.quad	1698788128
	.quad	1698760498
	.quad	1698853648
	.quad	1698826033
	.quad	1698830128
	.quad	1698828403
	.quad	1698918928
	.quad	1698891553
	.quad	1698895648
	.quad	1698893938
	.quad	1698899728
	.quad	1698898033
	.quad	1698898288
	.quad	1698898183
	.quad	1985229328
	.quad	1885684513
	.quad	1902461728
	.quad	1896240178
	.quad	1919238928
	.quad	1913017393
	.quad	1914065968
	.quad	1913677123
	.quad	1936015888
	.quad	1929794593
	.quad	1930843168
	.quad	1930454338
	.quad	1931891728
	.quad	1931502913
	.quad	1931568448
	.quad	1931544148
	.quad	1952789008
	.quad	1946571553
	.quad	1947620128
	.quad	1947231538
	.quad	1948668688
	.quad	1948280113
	.quad	1948345648
	.quad	1948321363
	.quad	1949717008
	.quad	1949328673
	.quad	1949394208
	.quad	1949369938
	.quad	1949459728
	.quad	1949435473
	.quad	1949439568
	.quad	1949438053
	.quad	1969500688
	.quad	1963344673
	.quad	1964393248
	.quad	1964008498
	.quad	1965441808
	.quad	1965057073
	.quad	1965122608
	.quad	1965098563
	.quad	1966490128
	.quad	1966105633
	.quad	1966171168
	.quad	1966147138
	.quad	1966236688
	.quad	1966212673
	.quad	1966216768
	.quad	1966215268
	.quad	1967534608
	.quad	1967153953
	.quad	1967219488
	.quad	1967195698
	.quad	1967285008
	.quad	1967261233
	.quad	1967265328
	.quad	1967263843
	.quad	1967350288
	.quad	1967326753
	.quad	1967330848
	.quad	1967329378
	.quad	1967334928
	.quad	1967333473
	.quad	1967333728
	.quad	1967333638
	.quad	1985229328
	.quad	1980056353
	.quad	1981104928
	.quad	1980781618
	.quad	1982153488
	.quad	1981830193
	.quad	1981895728
	.quad	1981875523
	.quad	1983201808
	.quad	1982878753
	.quad	1982944288
	.quad	1982924098
	.quad	1983009808
	.quad	1982989633
	.quad	1982993728
	.quad	1982992468
	.quad	1984246288
	.quad	1983927073
	.quad	1983992608
	.quad	1983972658
	.quad	1984058128
	.quad	1984038193
	.quad	1984042288
	.quad	1984041043
	.quad	1984123408
	.quad	1984103713
	.quad	1984107808
	.quad	1984106578
	.quad	1984111888
	.quad	1984110673
	.quad	1984110928
	.quad	1984110853
	.quad	1985229328
	.quad	1984971553
	.quad	1985037088
	.quad	1985020978
	.quad	1985102608
	.quad	1985086513
	.quad	1985090608
	.quad	1985089603
	.quad	1985167888
	.quad	1985152033
	.quad	1985156128
	.quad	1985155138
	.quad	1985160208
	.quad	1985159233
	.quad	1985159488
	.quad	1985159428
	.quad	1985229328
	.quad	1985217313
	.quad	1985221408
	.quad	1985220658
	.quad	1985225488
	.quad	1985224753
	.quad	1985225008
	.quad	1985224963
	.quad	1985229328
	.quad	1985228833
	.quad	1985229088
	.quad	1985229058
	.quad	1985229328
	.quad	1985229313
	.quad	1985229328
	.quad	1985229328
	.set	_ZZN3hwy11N_AVX3_ZEN4L11CompressNotIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array,_ZZN3hwy6N_AVX3L11CompressNotIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array
	.hidden	_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.weak	_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.section	.rodata._ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,"aG",@progbits,_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,comdat
	.balign 16
	.type	_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, @object
	.size	_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, 64
_ZZN3hwy6N_SSE26detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices:
	.string	""
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007"
	.ascii	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.section	.rodata._ZZN3hwy11N_AVX3_ZEN4L8CompressIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array,"a"
	.balign 16
	.type	_ZZN3hwy11N_AVX3_ZEN4L8CompressIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array, @object
	.size	_ZZN3hwy11N_AVX3_ZEN4L8CompressIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array, 2048
_ZZN3hwy11N_AVX3_ZEN4L8CompressIlLPv0EEENS0_6Vec512IT_EES5_NS0_7Mask512IS4_EEE12packed_array:
	.quad	1985229328
	.quad	1985229328
	.quad	1985229313
	.quad	1985229328
	.quad	1985229058
	.quad	1985229088
	.quad	1985228833
	.quad	1985229328
	.quad	1985224963
	.quad	1985225008
	.quad	1985224753
	.quad	1985225488
	.quad	1985220658
	.quad	1985221408
	.quad	1985217313
	.quad	1985229328
	.quad	1985159428
	.quad	1985159488
	.quad	1985159233
	.quad	1985160208
	.quad	1985155138
	.quad	1985156128
	.quad	1985152033
	.quad	1985167888
	.quad	1985089603
	.quad	1985090608
	.quad	1985086513
	.quad	1985102608
	.quad	1985020978
	.quad	1985037088
	.quad	1984971553
	.quad	1985229328
	.quad	1984110853
	.quad	1984110928
	.quad	1984110673
	.quad	1984111888
	.quad	1984106578
	.quad	1984107808
	.quad	1984103713
	.quad	1984123408
	.quad	1984041043
	.quad	1984042288
	.quad	1984038193
	.quad	1984058128
	.quad	1983972658
	.quad	1983992608
	.quad	1983927073
	.quad	1984246288
	.quad	1982992468
	.quad	1982993728
	.quad	1982989633
	.quad	1983009808
	.quad	1982924098
	.quad	1982944288
	.quad	1982878753
	.quad	1983201808
	.quad	1981875523
	.quad	1981895728
	.quad	1981830193
	.quad	1982153488
	.quad	1980781618
	.quad	1981104928
	.quad	1980056353
	.quad	1985229328
	.quad	1967333638
	.quad	1967333728
	.quad	1967333473
	.quad	1967334928
	.quad	1967329378
	.quad	1967330848
	.quad	1967326753
	.quad	1967350288
	.quad	1967263843
	.quad	1967265328
	.quad	1967261233
	.quad	1967285008
	.quad	1967195698
	.quad	1967219488
	.quad	1967153953
	.quad	1967534608
	.quad	1966215268
	.quad	1966216768
	.quad	1966212673
	.quad	1966236688
	.quad	1966147138
	.quad	1966171168
	.quad	1966105633
	.quad	1966490128
	.quad	1965098563
	.quad	1965122608
	.quad	1965057073
	.quad	1965441808
	.quad	1964008498
	.quad	1964393248
	.quad	1963344673
	.quad	1969500688
	.quad	1949438053
	.quad	1949439568
	.quad	1949435473
	.quad	1949459728
	.quad	1949369938
	.quad	1949394208
	.quad	1949328673
	.quad	1949717008
	.quad	1948321363
	.quad	1948345648
	.quad	1948280113
	.quad	1948668688
	.quad	1947231538
	.quad	1947620128
	.quad	1946571553
	.quad	1952789008
	.quad	1931544148
	.quad	1931568448
	.quad	1931502913
	.quad	1931891728
	.quad	1930454338
	.quad	1930843168
	.quad	1929794593
	.quad	1936015888
	.quad	1913677123
	.quad	1914065968
	.quad	1913017393
	.quad	1919238928
	.quad	1896240178
	.quad	1902461728
	.quad	1885684513
	.quad	1985229328
	.quad	1698898183
	.quad	1698898288
	.quad	1698898033
	.quad	1698899728
	.quad	1698893938
	.quad	1698895648
	.quad	1698891553
	.quad	1698918928
	.quad	1698828403
	.quad	1698830128
	.quad	1698826033
	.quad	1698853648
	.quad	1698760498
	.quad	1698788128
	.quad	1698722593
	.quad	1699164688
	.quad	1697779828
	.quad	1697781568
	.quad	1697777473
	.quad	1697805328
	.quad	1697711938
	.quad	1697739808
	.quad	1697674273
	.quad	1698120208
	.quad	1696663363
	.quad	1696691248
	.quad	1696625713
	.quad	1697071888
	.quad	1695577138
	.quad	1696023328
	.quad	1694974753
	.quad	1702113808
	.quad	1681002613
	.quad	1681004368
	.quad	1681000273
	.quad	1681028368
	.quad	1680934738
	.quad	1680962848
	.quad	1680897313
	.quad	1681347088
	.quad	1679886163
	.quad	1679914288
	.quad	1679848753
	.quad	1680298768
	.quad	1678800178
	.quad	1679250208
	.quad	1678201633
	.quad	1685402128
	.quad	1663108948
	.quad	1663137088
	.quad	1663071553
	.quad	1663521808
	.quad	1662022978
	.quad	1662473248
	.quad	1661424673
	.quad	1668629008
	.quad	1645245763
	.quad	1645696048
	.quad	1644647473
	.quad	1651852048
	.quad	1627870258
	.quad	1635074848
	.quad	1618297633
	.quad	1733571088
	.quad	1412567158
	.quad	1412568928
	.quad	1412564833
	.quad	1412593168
	.quad	1412499298
	.quad	1412527648
	.quad	1412462113
	.quad	1412915728
	.quad	1411450723
	.quad	1411479088
	.quad	1411413553
	.quad	1411867408
	.quad	1410364978
	.quad	1410818848
	.quad	1409770273
	.quad	1417032208
	.quad	1394673508
	.quad	1394701888
	.quad	1394636353
	.quad	1395090448
	.quad	1393587778
	.quad	1394041888
	.quad	1392993313
	.quad	1400259088
	.quad	1376810563
	.quad	1377264688
	.quad	1376216113
	.quad	1383482128
	.quad	1359438898
	.quad	1366704928
	.quad	1349927713
	.quad	1466184208
	.quad	1126238053
	.quad	1126266448
	.quad	1126200913
	.quad	1126655248
	.quad	1125152338
	.quad	1125606688
	.quad	1124558113
	.quad	1131827728
	.quad	1108375123
	.quad	1108829488
	.quad	1107780913
	.quad	1115050768
	.quad	1091003698
	.quad	1098273568
	.quad	1081496353
	.quad	1197814288
	.quad	839939668
	.quad	840394048
	.quad	839345473
	.quad	846615568
	.quad	822568258
	.quad	829838368
	.quad	813061153
	.quad	929382928
	.quad	554132803
	.quad	561402928
	.quad	544625713
	.quad	660947728
	.quad	276190258
	.quad	392512288
	.quad	124076833
	.quad	1985229328
	.hidden	_ZZN3hwy6N_AVX26detail18IndicesFromBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices
	.weak	_ZZN3hwy6N_AVX26detail18IndicesFromBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices
	.section	.rodata._ZZN3hwy6N_AVX26detail18IndicesFromBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices,"aG",@progbits,_ZZN3hwy6N_AVX26detail18IndicesFromBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices,comdat
	.balign 32
	.type	_ZZN3hwy6N_AVX26detail18IndicesFromBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices, @object
	.size	_ZZN3hwy6N_AVX26detail18IndicesFromBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices, 512
_ZZN3hwy6N_AVX26detail18IndicesFromBits256IlLPv0EEENS0_6Vec256IjEEmE11u32_indices:
	.long	0
	.long	1
	.long	2
	.long	3
	.long	4
	.long	5
	.long	6
	.long	7
	.long	8
	.long	9
	.long	2
	.long	3
	.long	4
	.long	5
	.long	6
	.long	7
	.long	10
	.long	11
	.long	0
	.long	1
	.long	4
	.long	5
	.long	6
	.long	7
	.long	8
	.long	9
	.long	10
	.long	11
	.long	4
	.long	5
	.long	6
	.long	7
	.long	12
	.long	13
	.long	0
	.long	1
	.long	2
	.long	3
	.long	6
	.long	7
	.long	8
	.long	9
	.long	12
	.long	13
	.long	2
	.long	3
	.long	6
	.long	7
	.long	10
	.long	11
	.long	12
	.long	13
	.long	0
	.long	1
	.long	6
	.long	7
	.long	8
	.long	9
	.long	10
	.long	11
	.long	12
	.long	13
	.long	6
	.long	7
	.long	14
	.long	15
	.long	0
	.long	1
	.long	2
	.long	3
	.long	4
	.long	5
	.long	8
	.long	9
	.long	14
	.long	15
	.long	2
	.long	3
	.long	4
	.long	5
	.long	10
	.long	11
	.long	14
	.long	15
	.long	0
	.long	1
	.long	4
	.long	5
	.long	8
	.long	9
	.long	10
	.long	11
	.long	14
	.long	15
	.long	4
	.long	5
	.long	12
	.long	13
	.long	14
	.long	15
	.long	0
	.long	1
	.long	2
	.long	3
	.long	8
	.long	9
	.long	12
	.long	13
	.long	14
	.long	15
	.long	2
	.long	3
	.long	10
	.long	11
	.long	12
	.long	13
	.long	14
	.long	15
	.long	0
	.long	1
	.long	8
	.long	9
	.long	10
	.long	11
	.long	12
	.long	13
	.long	14
	.long	15
	.hidden	_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.weak	_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.section	.rodata._ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,"aG",@progbits,_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,comdat
	.balign 16
	.type	_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, @object
	.size	_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, 64
_ZZN3hwy6N_SSE46detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices:
	.string	""
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007"
	.ascii	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.hidden	_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.weak	_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices
	.section	.rodata._ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,"aG",@progbits,_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices,comdat
	.balign 16
	.type	_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, @object
	.size	_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices, 64
_ZZN3hwy7N_SSSE36detail18IndicesFromBits128INS0_4SimdIlLm2ELi0EEELPv0EEEDTcl4ZerocvT__EEES6_mE10u8_indices:
	.string	""
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\b\t\n\013\f\r\016\017"
	.string	"\001\002\003\004\005\006\007"
	.ascii	"\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017"
	.section	.bss._ZZN3hwy17GetGeneratorStateEvE5state,"aw",@nobits
	.balign 16
	.type	_ZZN3hwy17GetGeneratorStateEvE5state, @object
	.size	_ZZN3hwy17GetGeneratorStateEvE5state, 24
_ZZN3hwy17GetGeneratorStateEvE5state:
	.zero	24
	.set	.LC0,.LC3
	.set	.LC1,.LC3
	.section	.rodata
	.balign 64
.LC2:
	.quad	7
	.quad	6
	.quad	5
	.quad	4
	.quad	3
	.quad	2
	.quad	1
	.quad	0
	.section	.rodata.cst32,"aM",@progbits,32
	.balign 32
.LC3:
	.quad	0
	.quad	1
	.quad	2
	.quad	3
	.set	.LC4,.LC7
	.set	.LC5,.LC8
	.set	.LC6,.LC7
	.section	.rodata
	.balign 64
.LC7:
	.quad	9223372036854775807
	.quad	9223372036854775807
	.quad	9223372036854775807
	.quad	9223372036854775807
	.quad	9223372036854775807
	.quad	9223372036854775807
	.quad	9223372036854775807
	.quad	9223372036854775807
	.balign 64
.LC8:
	.quad	-9223372036854775808
	.quad	-9223372036854775808
	.quad	-9223372036854775808
	.quad	-9223372036854775808
	.quad	-9223372036854775808
	.quad	-9223372036854775808
	.quad	-9223372036854775808
	.quad	-9223372036854775808
	.balign 64
.LC9:
	.quad	0
	.quad	4
	.quad	8
	.quad	12
	.quad	16
	.quad	20
	.quad	24
	.quad	28
	.set	.LC10,.LC7
	.set	.LC12,.LC8
	.set	.LC13,.LC7
	.set	.LC14,.LC8
