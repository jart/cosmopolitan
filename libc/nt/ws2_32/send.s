.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_send,send,19

	.text.windows
sys_send_nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_send(%rip),%rax
	jmp	__sysv2nt
	.endfn	sys_send_nt,globl
	.previous
