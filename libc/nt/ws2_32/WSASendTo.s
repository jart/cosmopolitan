.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSASendTo,WSASendTo,99

	.text.windows
WSASendTo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSASendTo(%rip),%rax
	jmp	__sysv2nt10
	.endfn	WSASendTo,globl
	.previous
