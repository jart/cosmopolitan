.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetEnvironmentVariableW,GetEnvironmentVariableW,533

	.text.windows
GetEnvironmentVariable:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetEnvironmentVariableW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetEnvironmentVariable,globl
	.previous
