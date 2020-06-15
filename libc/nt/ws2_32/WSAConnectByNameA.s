.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAConnectByNameA,WSAConnectByNameA,48

	.text.windows
WSAConnectByNameA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAConnectByNameA(%rip),%rax
	jmp	__sysv2nt10
	.endfn	WSAConnectByNameA,globl
	.previous
