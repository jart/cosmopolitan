.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_VirtualLock,VirtualLock,0

	.text.windows
VirtualLock:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualLock(%rip),%rax
	jmp	__sysv2nt
	.endfn	VirtualLock,globl
	.previous
