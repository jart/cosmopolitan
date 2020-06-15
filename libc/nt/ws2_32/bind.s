.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_bind,bind,2

	.text.windows
__bind$nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_bind(%rip),%rax
	jmp	__sysv2nt
	.endfn	__bind$nt,globl
	.previous
