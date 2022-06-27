.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_sendto,sendto,20

	.text.windows
__sys_sendto_nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_sendto(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__sys_sendto_nt,globl
	.previous
