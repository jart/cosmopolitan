	.globl	_start
_start:	rep movsb
/	add	$333333,%rcx
/	test	%eax,(%rcx)			# 85 0001
	push	%r15
	pop	%r15
	mov	%al,%bl
	mov	%eax,%ecx			# 89 0301
	mov	%ecx,%eax			# 89 0310
	mov	%ecx,(%rbx)
	mov	(%rbx),%ecx
	mov	%xmm0,%xmm1
	movb	$1,(%rax)
	movl	$1,(%rax)
	movl	$1,0xffffff(%rax,%rbx,8)
	mov	$1,%bl
	mov	$123,%r8d
/	mov	%ebx,%r8d
/	mov	(%r8),%ebx
/	mov	%ebx,(%r8)
/	movd	%eax,%xmm0
/	movdqa	%xmm0,%xmm8
/	movdqa	%xmm8,%xmm1
/	paddw	%xmm8,%xmm1
/	paddw	%xmm1,%xmm8
	hlt
	ret	$1

a:	.asciz	"ho","ggg"
	.align	8
	.section .text.yo
	.zero	1
	.byte	1
	.hidden	doge
