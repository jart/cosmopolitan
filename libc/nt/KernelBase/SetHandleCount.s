.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetHandleCount,SetHandleCount,1516

	.text.windows
SetHandleCount:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetHandleCount(%rip)
	leave
	ret
	.endfn	SetHandleCount,globl
	.previous
