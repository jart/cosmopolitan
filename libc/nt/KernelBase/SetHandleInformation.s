.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetHandleInformation,SetHandleInformation,1517

	.text.windows
SetHandleInformation:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetHandleInformation(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetHandleInformation,globl
	.previous
