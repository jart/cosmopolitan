.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateProcessA,CreateProcessA,0

	.text.windows
CreateProcessA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateProcessA(%rip),%rax
	jmp	__sysv2nt10
	.endfn	CreateProcessA,globl
	.previous
