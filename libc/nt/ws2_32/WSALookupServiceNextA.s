.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSALookupServiceNextA,WSALookupServiceNextA,83

	.text.windows
WSALookupServiceNextA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSALookupServiceNextA(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSALookupServiceNextA,globl
	.previous
