.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_VirtualQuery,VirtualQuery,0

	.text.windows
VirtualQuery:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualQuery(%rip),%rax
	jmp	__sysv2nt
	.endfn	VirtualQuery,globl
	.previous
