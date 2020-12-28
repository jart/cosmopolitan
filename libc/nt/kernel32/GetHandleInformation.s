.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetHandleInformation,GetHandleInformation,0

	.text.windows
GetHandleInformation:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetHandleInformation(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetHandleInformation,globl
	.previous
