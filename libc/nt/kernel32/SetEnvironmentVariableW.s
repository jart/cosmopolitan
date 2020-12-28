.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetEnvironmentVariableW,SetEnvironmentVariableW,0

	.text.windows
SetEnvironmentVariable:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetEnvironmentVariableW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetEnvironmentVariable,globl
	.previous
