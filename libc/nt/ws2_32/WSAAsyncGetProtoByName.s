.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAAsyncGetProtoByName,WSAAsyncGetProtoByName,105

	.text.windows
WSAAsyncGetProtoByName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAAsyncGetProtoByName(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WSAAsyncGetProtoByName,globl
	.previous
