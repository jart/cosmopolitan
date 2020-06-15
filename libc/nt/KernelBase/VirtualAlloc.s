.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_VirtualAlloc,VirtualAlloc,1755

	.text.windows
VirtualAlloc:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualAlloc(%rip),%rax
	jmp	__sysv2nt
	.endfn	VirtualAlloc,globl
	.previous
