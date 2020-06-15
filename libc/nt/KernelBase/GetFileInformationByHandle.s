.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetFileInformationByHandle,GetFileInformationByHandle,547

	.text.windows
GetFileInformationByHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileInformationByHandle(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileInformationByHandle,globl
	.previous
