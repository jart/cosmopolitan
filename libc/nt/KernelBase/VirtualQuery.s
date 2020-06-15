.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_VirtualQuery,VirtualQuery,1767

	.text.windows
VirtualQuery:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualQuery(%rip),%rax
	jmp	__sysv2nt
	.endfn	VirtualQuery,globl
	.previous
