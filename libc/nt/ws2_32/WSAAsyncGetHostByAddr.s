.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAAsyncGetHostByAddr,WSAAsyncGetHostByAddr,102

	.text.windows
WSAAsyncGetHostByAddr:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAAsyncGetHostByAddr(%rip),%rax
	jmp	__sysv2nt8
	.endfn	WSAAsyncGetHostByAddr,globl
	.previous
