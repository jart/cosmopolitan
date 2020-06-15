.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSADuplicateSocketW,WSADuplicateSocketW,59

	.text.windows
WSADuplicateSocket:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSADuplicateSocketW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSADuplicateSocket,globl
	.previous
