.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAEnumProtocolsA,WSAEnumProtocolsA,65

	.text.windows
WSAEnumProtocolsA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAEnumProtocolsA(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAEnumProtocolsA,globl
	.previous
