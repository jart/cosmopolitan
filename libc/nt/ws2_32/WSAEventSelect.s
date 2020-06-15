.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAEventSelect,WSAEventSelect,67

	.text.windows
WSAEventSelect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAEventSelect(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAEventSelect,globl
	.previous
