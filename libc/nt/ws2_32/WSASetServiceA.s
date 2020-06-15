.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSASetServiceA,WSASetServiceA,117

	.text.windows
WSASetServiceA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSASetServiceA(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSASetServiceA,globl
	.previous
