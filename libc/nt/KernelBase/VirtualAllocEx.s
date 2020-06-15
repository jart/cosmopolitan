.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_VirtualAllocEx,VirtualAllocEx,1758

	.text.windows
VirtualAllocEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualAllocEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	VirtualAllocEx,globl
	.previous
