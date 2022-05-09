.include "o/libc/nt/codegen.inc"
.imp	MsWSock,__imp_WSARecvEx,WSARecvEx,0

	.text.windows
WSARecvEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSARecvEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSARecvEx,globl
	.previous
