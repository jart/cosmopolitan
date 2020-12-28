.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetEndOfFile,SetEndOfFile,0

	.text.windows
SetEndOfFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetEndOfFile(%rip)
	leave
	ret
	.endfn	SetEndOfFile,globl
	.previous
