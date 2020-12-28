.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetFileInformationByHandle,GetFileInformationByHandle,0

	.text.windows
GetFileInformationByHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileInformationByHandle(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileInformationByHandle,globl
	.previous
