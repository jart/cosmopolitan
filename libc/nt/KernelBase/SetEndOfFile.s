.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetEndOfFile,SetEndOfFile,1497

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
