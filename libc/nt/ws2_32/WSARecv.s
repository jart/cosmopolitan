.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSARecv,WSARecv,91

	.text.windows
WSARecv:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSARecv(%rip),%rax
	jmp	__sysv2nt8
	.endfn	WSARecv,globl
	.previous
