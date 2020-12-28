.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_RemoveVectoredExceptionHandler,RemoveVectoredExceptionHandler,0

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
