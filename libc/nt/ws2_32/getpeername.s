.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_getpeername,getpeername,5

	.text.windows
__getpeername$nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_getpeername(%rip),%rax
	jmp	__sysv2nt
	.endfn	__getpeername$nt,globl
	.previous
