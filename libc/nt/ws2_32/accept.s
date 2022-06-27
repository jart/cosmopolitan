.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_accept,accept,1

	.text.windows
__sys_accept_nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_accept(%rip),%rax
	jmp	__sysv2nt
	.endfn	__sys_accept_nt,globl
	.previous
