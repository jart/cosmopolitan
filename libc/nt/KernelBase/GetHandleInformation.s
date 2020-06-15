.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetHandleInformation,GetHandleInformation,574

	.text.windows
GetHandleInformation:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetHandleInformation(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetHandleInformation,globl
	.previous
