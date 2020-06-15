.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAGetServiceClassInfoA,WSAGetServiceClassInfoA,70

	.text.windows
WSAGetServiceClassInfoA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAGetServiceClassInfoA(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAGetServiceClassInfoA,globl
	.previous
