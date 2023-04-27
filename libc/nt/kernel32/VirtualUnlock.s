.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_VirtualUnlock,VirtualUnlock,0

	.text.windows
VirtualUnlock:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualUnlock(%rip),%rax
	jmp	__sysv2nt
	.endfn	VirtualUnlock,globl
	.previous
