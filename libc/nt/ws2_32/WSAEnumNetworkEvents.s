.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAEnumNetworkEvents,WSAEnumNetworkEvents,64

	.text.windows
WSAEnumNetworkEvents:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAEnumNetworkEvents(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAEnumNetworkEvents,globl
	.previous
