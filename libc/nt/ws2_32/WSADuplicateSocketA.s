.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSADuplicateSocketA,WSADuplicateSocketA,58

	.text.windows
WSADuplicateSocketA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSADuplicateSocketA(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSADuplicateSocketA,globl
	.previous
