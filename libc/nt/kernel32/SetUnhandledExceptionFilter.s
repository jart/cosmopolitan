.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetUnhandledExceptionFilter,SetUnhandledExceptionFilter,0

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
