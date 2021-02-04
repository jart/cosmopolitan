.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_listen,listen,13

	.text.windows
__sys_listen_nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_listen(%rip),%rax
	jmp	__sysv2nt
	.endfn	__sys_listen_nt,globl
	.previous
