.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetEnvironmentVariableA,GetEnvironmentVariableA,0

	.text.windows
GetEnvironmentVariableA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetEnvironmentVariableA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetEnvironmentVariableA,globl
	.previous
