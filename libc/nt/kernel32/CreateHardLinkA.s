.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateHardLinkA,CreateHardLinkA,0

	.text.windows
CreateHardLinkA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateHardLinkA(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateHardLinkA,globl
	.previous
