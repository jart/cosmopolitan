.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetErrorMode,SetErrorMode,0

	.text.windows
SetErrorMode:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetErrorMode(%rip)
	leave
	ret
	.endfn	SetErrorMode,globl
	.previous
