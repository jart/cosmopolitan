.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetProcessHeaps,GetProcessHeaps,0

	.text.windows
GetProcessHeaps:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessHeaps(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessHeaps,globl
	.previous
