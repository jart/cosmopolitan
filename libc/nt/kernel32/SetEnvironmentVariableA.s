.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetEnvironmentVariableA,SetEnvironmentVariableA,0

	.text.windows
SetEnvironmentVariableA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetEnvironmentVariableA(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetEnvironmentVariableA,globl
	.previous
