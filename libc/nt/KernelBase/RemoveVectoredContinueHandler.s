.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RemoveVectoredContinueHandler,RemoveVectoredContinueHandler,1406

	.text.windows
RemoveVectoredContinueHandler:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RemoveVectoredContinueHandler(%rip)
	leave
	ret
	.endfn	RemoveVectoredContinueHandler,globl
	.previous
