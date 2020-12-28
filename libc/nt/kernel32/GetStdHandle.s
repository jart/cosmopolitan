.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetStdHandle,GetStdHandle,0

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
