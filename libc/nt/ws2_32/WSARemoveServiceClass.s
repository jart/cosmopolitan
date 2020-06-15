.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSARemoveServiceClass,WSARemoveServiceClass,94

	.text.windows
WSARemoveServiceClass:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_WSARemoveServiceClass(%rip)
	leave
	ret
	.endfn	WSARemoveServiceClass,globl
	.previous
