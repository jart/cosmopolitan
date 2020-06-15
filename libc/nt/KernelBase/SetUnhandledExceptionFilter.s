.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetUnhandledExceptionFilter,SetUnhandledExceptionFilter,1580

	.text.windows
SetUnhandledExceptionFilter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetUnhandledExceptionFilter(%rip)
	leave
	ret
	.endfn	SetUnhandledExceptionFilter,globl
	.previous
