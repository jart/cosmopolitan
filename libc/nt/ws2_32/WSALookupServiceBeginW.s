.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSALookupServiceBeginW,WSALookupServiceBeginW,81

	.text.windows
WSALookupServiceBegin:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSALookupServiceBeginW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSALookupServiceBegin,globl
	.previous
