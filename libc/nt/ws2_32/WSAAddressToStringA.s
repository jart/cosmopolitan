.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAAddressToStringA,WSAAddressToStringA,42

	.text.windows
WSAAddressToStringA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAAddressToStringA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WSAAddressToStringA,globl
	.previous
