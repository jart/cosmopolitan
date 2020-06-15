.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetStdHandle,GetStdHandle,724

	.text.windows
GetStdHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetStdHandle(%rip)
	leave
	ret
	.endfn	GetStdHandle,globl
	.previous
