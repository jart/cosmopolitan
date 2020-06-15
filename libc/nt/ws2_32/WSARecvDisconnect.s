.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSARecvDisconnect,WSARecvDisconnect,92

	.text.windows
WSARecvDisconnect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSARecvDisconnect(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSARecvDisconnect,globl
	.previous
