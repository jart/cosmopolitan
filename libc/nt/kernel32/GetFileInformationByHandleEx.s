.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetFileInformationByHandleEx,GetFileInformationByHandleEx,0

	.text.windows
GetFileInformationByHandleEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileInformationByHandleEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileInformationByHandleEx,globl
	.previous
