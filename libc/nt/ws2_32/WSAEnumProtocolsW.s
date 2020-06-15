.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAEnumProtocolsW,WSAEnumProtocolsW,66

	.text.windows
WSAEnumProtocols:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAEnumProtocolsW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAEnumProtocols,globl
	.previous
