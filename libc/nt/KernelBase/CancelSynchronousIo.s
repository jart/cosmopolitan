.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CancelSynchronousIo,CancelSynchronousIo,103

	.text.windows
CancelSynchronousIo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CancelSynchronousIo(%rip)
	leave
	ret
	.endfn	CancelSynchronousIo,globl
	.previous
