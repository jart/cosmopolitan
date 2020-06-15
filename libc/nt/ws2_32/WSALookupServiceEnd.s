.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSALookupServiceEnd,WSALookupServiceEnd,82

	.text.windows
WSALookupServiceEnd:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_WSALookupServiceEnd(%rip)
	leave
	ret
	.endfn	WSALookupServiceEnd,globl
	.previous
