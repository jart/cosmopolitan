.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetEnvironmentVariableA,GetEnvironmentVariableA,532

	.text.windows
GetEnvironmentVariableA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetEnvironmentVariableA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetEnvironmentVariableA,globl
	.previous
