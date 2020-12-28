.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetHandleInformation,SetHandleInformation,0

	.text.windows
SetHandleInformation:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetHandleInformation(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetHandleInformation,globl
	.previous
