.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAConnectByNameW,WSAConnectByNameW,49

	.text.windows
WSAConnectByName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAConnectByNameW(%rip),%rax
	jmp	__sysv2nt10
	.endfn	WSAConnectByName,globl
	.previous
