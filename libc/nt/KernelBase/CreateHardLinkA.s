.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CreateHardLinkA,CreateHardLinkA,193

	.text.windows
CreateHardLinkA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateHardLinkA(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateHardLinkA,globl
	.previous
