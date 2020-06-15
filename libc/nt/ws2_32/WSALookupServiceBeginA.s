.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSALookupServiceBeginA,WSALookupServiceBeginA,80

	.text.windows
WSALookupServiceBeginA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSALookupServiceBeginA(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSALookupServiceBeginA,globl
	.previous
