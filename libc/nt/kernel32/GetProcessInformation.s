.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetProcessInformation,GetProcessInformation,0

	.text.windows
GetProcessInformation:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessInformation(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessInformation,globl
	.previous
