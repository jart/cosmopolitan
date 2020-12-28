.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_RemoveVectoredContinueHandler,RemoveVectoredContinueHandler,0

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
