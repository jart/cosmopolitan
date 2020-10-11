.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CancelIo,CancelIo,101

	.text.windows
CancelIo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CancelIo(%rip)
	leave
	ret
	.endfn	CancelIo,globl
	.previous
