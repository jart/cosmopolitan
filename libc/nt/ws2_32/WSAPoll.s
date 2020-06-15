.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAPoll,WSAPoll,88

	.text.windows
WSAPoll:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAPoll(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAPoll,globl
	.previous
