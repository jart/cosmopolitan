.include "o/libc/nt/ntdllimport.inc"
.ntimp	LdrGetProcedureAddress

	.text.windows
LdrGetProcedureAddress:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LdrGetProcedureAddress(%rip),%rax
	jmp	__sysv2nt
	.endfn	LdrGetProcedureAddress,globl
	.previous
