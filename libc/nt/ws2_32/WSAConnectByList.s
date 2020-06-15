.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAConnectByList,WSAConnectByList,47

	.text.windows
WSAConnectByList:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAConnectByList(%rip),%rax
	jmp	__sysv2nt8
	.endfn	WSAConnectByList,globl
	.previous
