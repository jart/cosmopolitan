.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetStdHandle,SetStdHandle,0

	.text.windows
SetStdHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetStdHandle(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetStdHandle,globl
	.previous
