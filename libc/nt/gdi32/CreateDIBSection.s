.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_CreateDIBSection,CreateDIBSection,1069

	.text.windows
CreateDIBSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateDIBSection(%rip),%rax
	jmp	__sysv2nt6
	.endfn	CreateDIBSection,globl
	.previous
