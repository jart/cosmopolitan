.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_shutdown,shutdown,22

	.text.windows
__shutdown$nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_shutdown(%rip),%rax
	jmp	__sysv2nt
	.endfn	__shutdown$nt,globl
	.previous
