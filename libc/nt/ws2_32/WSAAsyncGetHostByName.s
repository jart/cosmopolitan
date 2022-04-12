.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAAsyncGetHostByName,WSAAsyncGetHostByName,103

	.text.windows
WSAAsyncGetHostByName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAAsyncGetHostByName(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WSAAsyncGetHostByName,globl
	.previous
