.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_VirtualFree,VirtualFree,1761

	.text.windows
VirtualFree:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualFree(%rip),%rax
	jmp	__sysv2nt
	.endfn	VirtualFree,globl
	.previous
