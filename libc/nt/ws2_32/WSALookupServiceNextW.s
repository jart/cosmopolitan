.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSALookupServiceNextW,WSALookupServiceNextW,84

	.text.windows
WSALookupServiceNext:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSALookupServiceNextW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSALookupServiceNext,globl
	.previous
