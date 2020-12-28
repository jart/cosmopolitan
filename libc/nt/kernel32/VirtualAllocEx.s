.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_VirtualAllocEx,VirtualAllocEx,0

	.text.windows
VirtualAllocEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualAllocEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	VirtualAllocEx,globl
	.previous
