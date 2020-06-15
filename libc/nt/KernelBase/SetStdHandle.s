.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetStdHandle,SetStdHandle,1550

	.text.windows
SetStdHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetStdHandle(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetStdHandle,globl
	.previous
