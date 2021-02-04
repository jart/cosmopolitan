.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_getpeername,getpeername,5

	.text.windows
__sys_getpeername_nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_getpeername(%rip),%rax
	jmp	__sysv2nt
	.endfn	__sys_getpeername_nt,globl
	.previous
