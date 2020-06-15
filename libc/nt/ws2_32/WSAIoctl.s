.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAIoctl,WSAIoctl,78

	.text.windows
WSAIoctl:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAIoctl(%rip),%rax
	jmp	__sysv2nt10
	.endfn	WSAIoctl,globl
	.previous
