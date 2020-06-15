.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RemoveVectoredExceptionHandler,RemoveVectoredExceptionHandler,1407

	.text.windows
RemoveVectoredExceptionHandler:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RemoveVectoredExceptionHandler(%rip)
	leave
	ret
	.endfn	RemoveVectoredExceptionHandler,globl
	.previous
