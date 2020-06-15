.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAStringToAddressA,WSAStringToAddressA,121

	.text.windows
WSAStringToAddressA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAStringToAddressA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WSAStringToAddressA,globl
	.previous
