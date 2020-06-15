.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAGetServiceClassNameByClassIdA,WSAGetServiceClassNameByClassIdA,72

	.text.windows
WSAGetServiceClassNameByClassIdA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAGetServiceClassNameByClassIdA(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAGetServiceClassNameByClassIdA,globl
	.previous
